using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using Syroot.NintenTools.Bfres.GX2;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using System.Drawing;
using System;
using ResU = Syroot.NintenTools.Bfres;
using Toolbox.Library;
using Toolbox.Library.Rendering;
using Ryujinx.Graphics.Gal.Texture;

namespace BFRES_Importer
{
    public class FTEX
    {
        public static void WriteFTEXData(XmlWriter writer, ResU.Texture texture)
        {
            writer.WriteStartElement("FTEX");

            // TODO test that this works with texture bfres files
            writer.WriteAttributeString("Name"       , texture.Name                  );
            writer.WriteAttributeString("CompSelR"   , texture.CompSelR   .ToString());
            writer.WriteAttributeString("CompSelG"   , texture.CompSelG   .ToString());
            writer.WriteAttributeString("CompSelB"   , texture.CompSelB   .ToString());
            writer.WriteAttributeString("CompSelA"   , texture.CompSelA   .ToString());
            writer.WriteAttributeString("Path"       , texture.Path                  );
            writer.WriteAttributeString("Width"      , texture.Width      .ToString());
            writer.WriteAttributeString("Height"     , texture.Height     .ToString());
            writer.WriteAttributeString("Depth"      , texture.Depth      .ToString());
            writer.WriteAttributeString("Swizzle"    , texture.Swizzle    .ToString());
            writer.WriteAttributeString("Alignment"  , texture.Alignment  .ToString());
            writer.WriteAttributeString("ArrayLength", texture.ArrayLength.ToString());
            writer.WriteAttributeString("Pitch"      , texture.Pitch      .ToString());
            writer.WriteAttributeString("TileMode"   , texture.TileMode   .ToString());
            writer.WriteAttributeString("AAMode"     , texture.AAMode     .ToString());
            writer.WriteAttributeString("Dim"        , texture.Dim        .ToString());
            writer.WriteAttributeString("Format"     , texture.Format     .ToString());

            // These should be written as binary files, not ASCII
            writer.WriteAttributeString("Data"       , texture.Data.ToString()       );
            writer.WriteAttributeString("MipData"    , texture.MipData.ToString()    );
            writer.WriteAttributeString("Regs"       , texture.Regs.ToString()       );

            writer.WriteAttributeString("UserData"   , texture.UserData.ToString()   );


            writer.WriteEndElement();
        }
    }

    public class JPTexture
    {
        public int format;
        /// <summary>
        /// The <see cref="TEX_FORMAT"/> Format of the image. 
        /// </summary>
        public TEX_FORMAT Format { get; set; } = TEX_FORMAT.R8G8B8A8_UNORM;
        public PALETTE_FORMAT PaletteFormat { get; set; } = PALETTE_FORMAT.None;
        public STChannelType RedChannel { get; set; } = STChannelType.Red;
        public STChannelType GreenChannel { get; set; } = STChannelType.Green;
        public STChannelType BlueChannel { get; set; } = STChannelType.Blue;
        public STChannelType AlphaChannel { get; set; } = STChannelType.Alpha;
        public PlatformSwizzle PlatformSwizzle { get; set; }
        public RenderableTex RenderableTex { get; set; }
        public string Name { get; set; }
        public Texture texture { get; set; }
        public uint Width { get; set; }
        public uint Height { get; set; }
        public uint ArrayCount { get; set; }
        public uint MipCount { get; set; }
        
        //bool isTex2 = Program.FilePath.Contains(".Tex2.");

        private bool IsReplaced = false;
        private uint Tex2Swizzle = 0;


        //A class that configs how the image should output (on display, and on export/replace)
        private ImageParameters Parameters = new ImageParameters();

        ResFile ResFileTexture2;
        private byte[] paletteData = new byte[0];
        private static readonly Dictionary<TEX_FORMAT, FormatInfo> FormatTable =
                 new Dictionary<TEX_FORMAT, FormatInfo>()
{
            { TEX_FORMAT.R32G32B32A32_FLOAT,   new FormatInfo(16, 1,  1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.R32G32B32A32_SINT,    new FormatInfo(16, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G32B32A32_UINT,    new FormatInfo(16, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G32B32_FLOAT,      new FormatInfo(8, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16B16A16_FLOAT,   new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16B16A16_SINT,    new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16B16A16_SNORM,   new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G32_FLOAT,         new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G32_SINT,          new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G32_UINT,          new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8B8A8_SINT,        new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8B8A8_SNORM,       new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8B8A8_UINT,        new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8B8A8_UNORM,       new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8B8A8_UNORM_SRGB,  new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32G8X24_FLOAT,       new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8_B8G8_UNORM,      new FormatInfo(4, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B8G8R8X8_UNORM,       new FormatInfo(4, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B5G5R5A1_UNORM,       new FormatInfo(2, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R5G5B5A1_UNORM,       new FormatInfo(2, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B8G8R8A8_UNORM,       new FormatInfo(4, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B8G8R8A8_UNORM_SRGB,  new FormatInfo(4, 1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R5G5B5_UNORM,         new FormatInfo(2, 1,  1, 1,  TargetBuffer.Color) },


            { TEX_FORMAT.R10G10B10A2_UINT,      new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R10G10B10A2_UNORM,     new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32_SINT,              new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32_UINT,              new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R32_FLOAT,             new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B4G4R4A4_UNORM,        new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16_FLOAT,          new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16_SINT,           new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16_SNORM,          new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16_UINT,           new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16G16_UNORM,          new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8_SINT,             new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8_SNORM,            new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8_UINT,             new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8G8_UNORM,            new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16_SINT,              new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16_SNORM,             new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16_UINT,              new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R16_UNORM,             new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8_SINT,               new FormatInfo(1,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8_SNORM,              new FormatInfo(1,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R4G4_UNORM,            new FormatInfo(1,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8_UINT,               new FormatInfo(1,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R8_UNORM,              new FormatInfo(1,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.R11G11B10_FLOAT,       new FormatInfo(4,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.B5G6R5_UNORM,          new FormatInfo(2,  1,  1, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC1_UNORM,             new FormatInfo(8,  4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC1_UNORM_SRGB,        new FormatInfo(8,  4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC2_UNORM,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC2_UNORM_SRGB,        new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC3_UNORM,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC3_UNORM_SRGB,        new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC4_UNORM,             new FormatInfo(8,  4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC4_SNORM,             new FormatInfo(8,  4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC5_UNORM,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC5_SNORM,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC6H_SF16,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC6H_UF16,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC7_UNORM,             new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.BC7_UNORM_SRGB,        new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },

            { TEX_FORMAT.ASTC_4x4_UNORM,        new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_4x4_SRGB,         new FormatInfo(16, 4,  4, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_5x4_UNORM,        new FormatInfo(16, 5,  4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_5x4_SRGB,         new FormatInfo(16, 5,  4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_5x5_UNORM,        new FormatInfo(16, 5,  5, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_5x5_SRGB,         new FormatInfo(16, 5,  5, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_6x5_UNORM,        new FormatInfo(16, 6,  5, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_6x5_SRGB,         new FormatInfo(16, 6,  5, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_6x6_UNORM,        new FormatInfo(16, 6,  6, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_6x6_SRGB,         new FormatInfo(16, 6,  6, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x5_UNORM,        new FormatInfo(16, 8,  5,  1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x5_SRGB,         new FormatInfo(16, 8,  5, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x6_UNORM,        new FormatInfo(16, 8,  6, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x6_SRGB,         new FormatInfo(16, 8,  6, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x8_UNORM,        new FormatInfo(16, 8,  8, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_8x8_SRGB,         new FormatInfo(16, 8,  8, 1,  TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x5_UNORM,       new FormatInfo(16, 10, 5, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x5_SRGB,        new FormatInfo(16, 10, 5, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x6_UNORM,       new FormatInfo(16, 10, 6, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x6_SRGB,        new FormatInfo(16, 10, 6, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x8_UNORM,       new FormatInfo(16, 10, 8, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x8_SRGB,        new FormatInfo(16, 10, 8, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x10_UNORM,      new FormatInfo(16, 10, 10, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_10x10_SRGB,       new FormatInfo(16, 10, 10, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_12x10_UNORM,      new FormatInfo(16, 12, 10, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_12x10_SRGB,       new FormatInfo(16, 12, 10, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_12x12_UNORM,      new FormatInfo(16, 12, 12, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ASTC_12x12_SRGB,       new FormatInfo(16, 12, 12, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ETC1_UNORM,            new FormatInfo(4, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ETC1_SRGB,             new FormatInfo(4, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.ETC1_A4,               new FormatInfo(8, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.HIL08,                 new FormatInfo(16, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.L4,                    new FormatInfo(4, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.LA4,                   new FormatInfo(4, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.L8,                    new FormatInfo(8, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.LA8,                   new FormatInfo(16, 1, 1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.A4,                    new FormatInfo(4, 1,  1, 1, TargetBuffer.Color) },
            { TEX_FORMAT.A8_UNORM,              new FormatInfo(8,  1,  1, 1,  TargetBuffer.Color) },

            { TEX_FORMAT.D16_UNORM,            new FormatInfo(2, 1, 1, 1, TargetBuffer.Depth)       },
            { TEX_FORMAT.D24_UNORM_S8_UINT,    new FormatInfo(4, 1, 1, 1, TargetBuffer.Depth)       },
            { TEX_FORMAT.D32_FLOAT,            new FormatInfo(4, 1, 1, 1, TargetBuffer.Depth)       },
            { TEX_FORMAT.D32_FLOAT_S8X24_UINT, new FormatInfo(8, 1, 1, 1, TargetBuffer.DepthStencil)},

            { TEX_FORMAT.I4,                   new FormatInfo(4,  8, 8, 1, TargetBuffer.Color) },
            { TEX_FORMAT.I8,                   new FormatInfo(8,  8, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.IA4,                  new FormatInfo(8,  8, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.IA8,                  new FormatInfo(16, 4, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.RGB565,               new FormatInfo(16, 4, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.RGB5A3,               new FormatInfo(16, 4, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.RGBA32,               new FormatInfo(32, 4, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.C4,                   new FormatInfo(4,  8, 8, 1, TargetBuffer.Color) },
            { TEX_FORMAT.C8,                   new FormatInfo(8,  8, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.C14X2,                new FormatInfo(16, 4, 4, 1, TargetBuffer.Color) },
            { TEX_FORMAT.CMPR,                 new FormatInfo(4,  8, 8, 1, TargetBuffer.Color) }

};
        public void Read(Texture tex)
        {
            Name = tex.Name;

            texture = tex;

            Width = tex.Width;
            Height = tex.Height;
            Format = ConvertFromGx2Format(tex.Format);
            ArrayCount = tex.Depth;
            MipCount = tex.MipCount;

            RedChannel = GX2ChanneToGeneric(texture.CompSelR);
            GreenChannel = GX2ChanneToGeneric(texture.CompSelG);
            BlueChannel = GX2ChanneToGeneric(texture.CompSelB);
            AlphaChannel = GX2ChanneToGeneric(texture.CompSelA);


            if (texture.MipData == null || texture.MipData.Length <= 0)
                MipCount = 1;
        }
        //For determining mip map file for botw (Tex2)
        public string GetFilePath()
        {
            // TODO fact check this shit below
            return Program.FilePath;
            //if (Parent == null)
            //    return "";

            //return ((BFRES)Parent.Parent).FilePath;
        }

        public void SaveBitMap(string FileName, bool ExportSurfaceLevel = false,
            bool ExportMipMapLevel = false, int SurfaceLevel = 0, int MipLevel = 0)
        {
            if (ArrayCount > 1 && !ExportSurfaceLevel)
            {
                string ext = Path.GetExtension(FileName);

                int index = FileName.LastIndexOf('.');
                string name = index == -1 ? FileName : FileName.Substring(0, index);

                for (int i = 0; i < ArrayCount; i++)
                {
                    Bitmap arrayBitMap = GetBitmap(i, 0);
                    arrayBitMap.Save($"{name}_Slice_{i}_{ext}");
                    arrayBitMap.Dispose();
                }
                return;
            }
            Bitmap bitMap = GetBitmap(SurfaceLevel, MipLevel);
            if (Runtime.ImageEditor.UseComponetSelector)
                bitMap = BitmapExtension.SetChannel(bitMap, RedChannel, GreenChannel, BlueChannel, AlphaChannel);
            if (Runtime.ImageEditor.PreviewGammaFix)
                bitMap = BitmapExtension.AdjustGamma(bitMap, 1.0f / 2.2f);

            if (Parameters.FlipY)
                bitMap.RotateFlip(RotateFlipType.RotateNoneFlipY);

            bitMap.Save(FileName);
            bitMap.Dispose();
        }

        public byte[] GetImageData(int ArrayLevel = 0, int MipLevel = 0, int DepthLevel = 0)
        {
            format = (int)texture.Format;
            int swizzle = (int)texture.Swizzle;
            int pitch = (int)texture.Pitch;
            uint bpp = GX2.surfaceGetBitsPerPixel((uint)format) >> 3;

            RedChannel = GX2ChanneToGeneric(texture.CompSelR);
            GreenChannel = GX2ChanneToGeneric(texture.CompSelG);
            BlueChannel = GX2ChanneToGeneric(texture.CompSelB);
            AlphaChannel = GX2ChanneToGeneric(texture.CompSelA);

            UpdateMipMaps();

            GX2.GX2Surface surf = new GX2.GX2Surface();
            surf.bpp = bpp;
            surf.height = texture.Height;
            surf.width = texture.Width;
            surf.aa = (uint)texture.AAMode;
            surf.alignment = texture.Alignment;
            surf.depth = texture.Depth;
            surf.dim = (uint)texture.Dim;
            surf.format = (uint)texture.Format;
            surf.use = (uint)texture.Use;
            surf.pitch = texture.Pitch;
            surf.data = texture.Data;
            surf.numMips = MipCount;
            surf.mipOffset = texture.MipOffsets;
            surf.mipData = texture.MipData;
            surf.tileMode = (uint)texture.TileMode;
            surf.swizzle = texture.Swizzle;
            surf.numArray = texture.ArrayLength;

            if (Tex2Swizzle != 0)
                surf.mip_swizzle = Tex2Swizzle;

            if (surf.mipData == null)
                surf.numMips = 1;


           	return GX2.Decode(surf, ArrayLevel, MipLevel);
        }

        public void UpdateMipMaps()
        {
            LoadTex2Bfres();
            LoadTex2MipMaps();

            if (texture.MipData != null && texture.MipData.Length > 0)
                MipCount = texture.MipCount;
        }

        private void LoadTex2Bfres()
        {
            if (ResFileTexture2 != null)
                return;

            //Determine tex2 botw files to get mip maps
            string Tex1 = GetFilePath();

            if (!IsReplaced && Tex1 != null && Tex1.Contains(".Tex1"))
            {
                string Tex2 = Tex1.Replace(".Tex1", ".Tex2");
                Console.WriteLine(Tex2 + " " + System.IO.File.Exists(Tex2) + " " + texture.Name);

                if (System.IO.File.Exists(Tex2))
                {
                    if (Tex2.EndsWith(".sbfres"))
                    {
                        ResFileTexture2 = new ResFile(new System.IO.MemoryStream(
                                        EveryFileExplorer.YAZ0.Decompress(Tex2)));
                    }
                    else
                    {
                        ResFileTexture2 = new ResFile(Tex2);
                    }
                }
            }
        }

        private void LoadTex2MipMaps()
        {
            if (ResFileTexture2 == null || IsReplaced)
                return;

            Console.WriteLine((ResFileTexture2.Textures.ContainsKey(texture.Name)));

            if (ResFileTexture2.Textures.ContainsKey(texture.Name))
            {
                texture.MipCount = ResFileTexture2.Textures[texture.Name].MipCount;
                texture.MipData = ResFileTexture2.Textures[texture.Name].MipData;
                texture.MipOffsets = ResFileTexture2.Textures[texture.Name].MipOffsets;
                Tex2Swizzle = ResFileTexture2.Textures[texture.Name].Swizzle;
            }
        }

        public virtual byte[] GetPaletteData() { return paletteData; }
        public Bitmap GetBitmap(int ArrayLevel = 0, int MipLevel = 0, int DepthLevel = 0)
        {
            uint width = Math.Max(1, Width >> MipLevel);
            uint height = Math.Max(1, Height >> MipLevel);
            byte[] data = GetImageData(ArrayLevel, MipLevel, DepthLevel);
            byte[] paletteData = GetPaletteData();
            if (Format == TEX_FORMAT.R8G8B8A8_UNORM) return BitmapExtension.GetBitmap(ConvertBgraToRgba(data), (int)width, (int)height);

            try
            {
                if (data == null)
                    throw new Exception("Data is null!");

                if (PlatformSwizzle == PlatformSwizzle.Platform_3DS)
                {
                    var Image = BitmapExtension.GetBitmap(ConvertBgraToRgba(CTR_3DS.DecodeBlock(data, (int)width, (int)height, Format)),
                      (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    Image.RotateFlip(RotateFlipType.RotateNoneFlipY); //It's upside down for some reason so flip it
                    return Image;
                }

                if (PlatformSwizzle == PlatformSwizzle.Platform_Gamecube)
                {
                    return BitmapExtension.GetBitmap(Decode_Gamecube.DecodeData(data, paletteData, width, height, Format, PaletteFormat),
                          (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                }

                switch (Format)
                {
                    case TEX_FORMAT.R4G4_UNORM:
                        return BitmapExtension.GetBitmap(R4G4.Decompress(data, (int)width, (int)height, false),
                        (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    case TEX_FORMAT.BC5_SNORM:
                        return DDSCompressor.DecompressBC5(data, (int)width, (int)height, true);
                    case TEX_FORMAT.ETC1_UNORM:
                        return BitmapExtension.GetBitmap(ETC1.ETC1Decompress(data, (int)width, (int)height, false),
                               (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    case TEX_FORMAT.ETC1_A4:
                        return BitmapExtension.GetBitmap(ETC1.ETC1Decompress(data, (int)width, (int)height, true),
                              (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    case TEX_FORMAT.R5G5B5A1_UNORM:
                    case TEX_FORMAT.LA8:
                    case TEX_FORMAT.L8:
                        return BitmapExtension.GetBitmap(RGBAPixelDecoder.Decode(data, (int)width, (int)height, Format),
                              (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                }

                if (Runtime.UseDirectXTexDecoder)
                {
                    return BitmapExtension.GetBitmap(DecodeBlock(data, width, height, Format, new byte[0], Parameters),
                      (int)width, (int)height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                }
                else
                    return DecodeNotDirectXTex(data, width, height, Format);

            }
            catch (Exception ex)
            {
                /*       Forms.STErrorDialog.Show($"Texture failed to load!", "Texture [GetBitmap({MipLevel},{ArrayLevel})]", DebugInfo() + " \n" + ex);

                       try
                       {
                           return DecodeNotDirectXTex(data, width, height, Format);
                       }
                       catch
                       {
                           Forms.STErrorDialog.Show($"Texture failed to load!", "Texture [GetBitmap({MipLevel},{ArrayLevel})]", DebugInfo() + " \n" + ex);
                       }*/

                return null;
            }
        }
        private STChannelType GX2ChanneToGeneric(GX2CompSel comp)
        {
            if (comp == GX2CompSel.ChannelR) return STChannelType.Red;
            else if (comp == GX2CompSel.ChannelG) return STChannelType.Green;
            else if (comp == GX2CompSel.ChannelB) return STChannelType.Blue;
            else if (comp == GX2CompSel.ChannelA) return STChannelType.Alpha;
            else if (comp == GX2CompSel.Always0) return STChannelType.Zero;
            else return STChannelType.One;
        }
        public static TEX_FORMAT ConvertFromGx2Format(GX2SurfaceFormat GX2Format)
        {
            switch (GX2Format)
            {
                case GX2SurfaceFormat.T_BC1_UNorm: return TEX_FORMAT.BC1_UNORM;
                case GX2SurfaceFormat.T_BC1_SRGB: return TEX_FORMAT.BC1_UNORM_SRGB;
                case GX2SurfaceFormat.T_BC2_UNorm: return TEX_FORMAT.BC2_UNORM;
                case GX2SurfaceFormat.T_BC2_SRGB: return TEX_FORMAT.BC2_UNORM_SRGB;
                case GX2SurfaceFormat.T_BC3_UNorm: return TEX_FORMAT.BC3_UNORM;
                case GX2SurfaceFormat.T_BC3_SRGB: return TEX_FORMAT.BC3_UNORM_SRGB;
                case GX2SurfaceFormat.T_BC4_UNorm: return TEX_FORMAT.BC4_UNORM;
                case GX2SurfaceFormat.T_BC4_SNorm: return TEX_FORMAT.BC4_SNORM;
                case GX2SurfaceFormat.T_BC5_UNorm: return TEX_FORMAT.BC5_UNORM;
                case GX2SurfaceFormat.T_BC5_SNorm: return TEX_FORMAT.BC5_SNORM;
                case GX2SurfaceFormat.TC_R5_G5_B5_A1_UNorm: return TEX_FORMAT.B5G5R5A1_UNORM;
                case GX2SurfaceFormat.TC_A1_B5_G5_R5_UNorm: return TEX_FORMAT.B5G5R5A1_UNORM;
                case GX2SurfaceFormat.TC_R4_G4_B4_A4_UNorm: return TEX_FORMAT.B4G4R4A4_UNORM;
                case GX2SurfaceFormat.TCS_R5_G6_B5_UNorm: return TEX_FORMAT.B5G6R5_UNORM;
                case GX2SurfaceFormat.TCS_R8_G8_B8_A8_SRGB: return TEX_FORMAT.R8G8B8A8_UNORM_SRGB;
                case GX2SurfaceFormat.TCS_R8_G8_B8_A8_UNorm: return TEX_FORMAT.R8G8B8A8_UNORM;
                case GX2SurfaceFormat.TCS_R10_G10_B10_A2_UNorm: return TEX_FORMAT.R10G10B10A2_UNORM;
                case GX2SurfaceFormat.TC_R11_G11_B10_Float: return TEX_FORMAT.R11G11B10_FLOAT;
                case GX2SurfaceFormat.TCD_R16_UNorm: return TEX_FORMAT.R16_UNORM;
                case GX2SurfaceFormat.TCD_R32_Float: return TEX_FORMAT.R32_FLOAT;
                case GX2SurfaceFormat.T_R4_G4_UNorm: return TEX_FORMAT.R4G4_UNORM;
                case GX2SurfaceFormat.TC_R8_G8_UNorm: return TEX_FORMAT.R8G8_UNORM;
                case GX2SurfaceFormat.TC_R8_G8_SNorm: return TEX_FORMAT.R8G8_SNORM;
                case GX2SurfaceFormat.TC_R8_UNorm: return TEX_FORMAT.R8_UNORM;
                case GX2SurfaceFormat.TC_R32_G32_B32_A32_Float: return TEX_FORMAT.R32G32B32A32_FLOAT;
                case GX2SurfaceFormat.TC_R16_G16_B16_A16_Float: return TEX_FORMAT.R16G16B16A16_FLOAT;
                case GX2SurfaceFormat.TC_R32_G32_B32_A32_SInt: return TEX_FORMAT.R32G32B32A32_SINT;
                case GX2SurfaceFormat.TC_R32_G32_B32_A32_UInt: return TEX_FORMAT.R32G32B32A32_UINT;
                case GX2SurfaceFormat.Invalid: throw new Exception("Invalid Format");
                default:
                    throw new Exception($"Cannot convert format {GX2Format}");
            }
        }
        public static byte[] ConvertBgraToRgba(byte[] bytes)
        {
            if (bytes == null)
                throw new Exception("Data block returned null. Make sure the parameters and image properties are correct!");

            for (int i = 0; i < bytes.Length; i += 4)
            {
                var temp = bytes[i];
                bytes[i] = bytes[i + 2];
                bytes[i + 2] = temp;
            }
            return bytes;
        }

        /// <summary>
        /// Decodes a byte array of image data given the source image in bytes, width, height, and DXGI format.
        /// </summary>
        /// <param name="byte[]">The byte array of the image</param>
        /// <param name="Width">The width of the image in pixels.</param>
        /// <param name="Height">The height of the image in pixels.</param>
        /// <param name=" DDS.DXGI_FORMAT">The image format.</param>
        /// <returns>Returns a byte array of decoded data. </returns>
        public static byte[] DecodeBlock(byte[] data, uint Width, uint Height, TEX_FORMAT Format, byte[] paletteData, ImageParameters parameters, PALETTE_FORMAT PaletteFormat = PALETTE_FORMAT.None, PlatformSwizzle PlatformSwizzle = PlatformSwizzle.None)
        {
            if (data == null) throw new Exception($"Data is null!");
            if (Format <= 0) throw new Exception($"Invalid Format!");
            if (data.Length <= 0) throw new Exception($"Data is empty!");
            if (Width <= 0) throw new Exception($"Invalid width size {Width}!");
            if (Height <= 0) throw new Exception($"Invalid height size {Height}!");

            byte[] imageData = new byte[0];
            bool DontSwapRG = false;

            if (PlatformSwizzle == PlatformSwizzle.Platform_3DS)
            {
                imageData = CTR_3DS.DecodeBlock(data, (int)Width, (int)Height, Format);
                DontSwapRG = true;
            }
            else if (PlatformSwizzle == PlatformSwizzle.Platform_Gamecube)
                imageData = Decode_Gamecube.DecodeData(data, paletteData, Width, Height, Format, PaletteFormat);
            else
            {
                if (Format == TEX_FORMAT.R32G8X24_FLOAT)
                    imageData = DDSCompressor.DecodePixelBlock(data, (int)Width, (int)Height, DDS.DXGI_FORMAT.DXGI_FORMAT_R32G8X24_TYPELESS);

                if (Format == TEX_FORMAT.BC5_SNORM)
                    imageData = DDSCompressor.DecompressBC5(data, (int)Width, (int)Height, true, true);

                if (Format == TEX_FORMAT.L8)
                    return RGBAPixelDecoder.Decode(data, (int)Width, (int)Height, Format);
                if (Format == TEX_FORMAT.LA8)
                    return RGBAPixelDecoder.Decode(data, (int)Width, (int)Height, Format);
                if (Format == TEX_FORMAT.R5G5B5A1_UNORM)
                    return RGBAPixelDecoder.Decode(data, (int)Width, (int)Height, Format);

                if (IsCompressed(Format))
                    imageData = DDSCompressor.DecompressBlock(data, (int)Width, (int)Height, (DDS.DXGI_FORMAT)Format);
                else
                {
                    if (IsAtscFormat(Format))
                        imageData = ASTCDecoder.DecodeToRGBA8888(data, (int)GetBlockWidth(Format), (int)GetBlockHeight(Format), 1, (int)Width, (int)Height, 1);
                    else
                        imageData = DDSCompressor.DecodePixelBlock(data, (int)Width, (int)Height, (DDS.DXGI_FORMAT)Format);

                    //    imageData = RGBAPixelDecoder.Decode(data, (int)Width, (int)Height, Format);
                }
            }

            if (parameters.DontSwapRG || DontSwapRG)
                return imageData;
            else
                return ConvertBgraToRgba(imageData);
        }
        private Bitmap DecodeNotDirectXTex(byte[] data, uint Width, uint Height, TEX_FORMAT Format)
        {
            if (Format == TEX_FORMAT.R8G8B8A8_UNORM)
                return BitmapExtension.GetBitmap(ConvertBgraToRgba(data), (int)Width, (int)Height);
            else if (Format == TEX_FORMAT.R8G8B8A8_UNORM_SRGB)
                return BitmapExtension.GetBitmap(ConvertBgraToRgba(data), (int)Width, (int)Height);
            else if (Format == TEX_FORMAT.BC1_UNORM)
                return DDSCompressor.DecompressBC1(data, (int)Width, (int)Height, false);
            else if (Format == TEX_FORMAT.BC1_UNORM_SRGB)
                return DDSCompressor.DecompressBC1(data, (int)Width, (int)Height, true);
            else if (Format == TEX_FORMAT.BC3_UNORM_SRGB)
                return DDSCompressor.DecompressBC3(data, (int)Width, (int)Height, false);
            else if (Format == TEX_FORMAT.BC3_UNORM)
                return DDSCompressor.DecompressBC3(data, (int)Width, (int)Height, true);
            else if (Format == TEX_FORMAT.BC4_UNORM)
                return DDSCompressor.DecompressBC4(data, (int)Width, (int)Height, false);
            else if (Format == TEX_FORMAT.BC4_SNORM)
                return DDSCompressor.DecompressBC4(data, (int)Width, (int)Height, true);
            else if (Format == TEX_FORMAT.BC5_UNORM)
                return DDSCompressor.DecompressBC5(data, (int)Width, (int)Height, false);
            else if (Format == TEX_FORMAT.BC7_UNORM)
                return BitmapExtension.GetBitmap(CSharpImageLibrary.DDS.Dxt.DecompressBc7(data, (int)Width, (int)Height), (int)Width, (int)Height);
            else if (Format == TEX_FORMAT.BC7_UNORM_SRGB)
                return BitmapExtension.GetBitmap(CSharpImageLibrary.DDS.Dxt.DecompressBc7(data, (int)Width, (int)Height), (int)Width, (int)Height);
            else
            {

                if (Runtime.UseOpenGL)
                {
                    Runtime.OpenTKInitialized = true;
                    if (RenderableTex == null || !RenderableTex.GLInitialized)
                        LoadOpenGLTexture();

                    return RenderableTex.ToBitmap();
                }
            }
            return null;
        }
        public void LoadOpenGLTexture()
        {
            // TODO this doesn't seem to be hit for our use case, but it will fuckin break things if it is ever hit.
            //if (!Runtime.UseOpenGL)
            //    return;

            //if (RenderableTex == null)
            //    RenderableTex = new RenderableTex();

            //RenderableTex.GLInitialized = false;
            //RenderableTex.LoadOpenGLTexture(this);
        }
        public static bool IsCompressed(TEX_FORMAT Format)
        {
            switch (Format)
            {
                case TEX_FORMAT.BC1_UNORM:
                case TEX_FORMAT.BC1_UNORM_SRGB:
                case TEX_FORMAT.BC1_TYPELESS:
                case TEX_FORMAT.BC2_UNORM_SRGB:
                case TEX_FORMAT.BC2_UNORM:
                case TEX_FORMAT.BC2_TYPELESS:
                case TEX_FORMAT.BC3_UNORM_SRGB:
                case TEX_FORMAT.BC3_UNORM:
                case TEX_FORMAT.BC3_TYPELESS:
                case TEX_FORMAT.BC4_UNORM:
                case TEX_FORMAT.BC4_TYPELESS:
                case TEX_FORMAT.BC4_SNORM:
                case TEX_FORMAT.BC5_UNORM:
                case TEX_FORMAT.BC5_TYPELESS:
                case TEX_FORMAT.BC5_SNORM:
                case TEX_FORMAT.BC6H_UF16:
                case TEX_FORMAT.BC6H_SF16:
                case TEX_FORMAT.BC7_UNORM:
                case TEX_FORMAT.BC7_UNORM_SRGB:
                    return true;
                default:
                    return false;
            }
        }
        public static bool IsAtscFormat(TEX_FORMAT Format)
        {
            if (Format.ToString().Contains("ASTC"))
                return true;
            else
                return false;
        }
        public static uint GetBlockWidth(TEX_FORMAT Format)
        {
            return FormatTable[Format].BlockWidth;
        }
        public static uint GetBlockHeight(TEX_FORMAT Format)
        {
            return FormatTable[Format].BlockHeight;
        }

        private enum TargetBuffer
        {
            Color = 1,
            Depth = 2,
            Stencil = 3,
            DepthStencil = 4,
        }
        private class FormatInfo
        {
            public uint BytesPerPixel { get; private set; }
            public uint BlockWidth { get; private set; }
            public uint BlockHeight { get; private set; }
            public uint BlockDepth { get; private set; }

            public TargetBuffer TargetBuffer;

            public FormatInfo(uint bytesPerPixel, uint blockWidth, uint blockHeight, uint blockDepth, TargetBuffer targetBuffer)
            {
                BytesPerPixel = bytesPerPixel;
                BlockWidth = blockWidth;
                BlockHeight = blockHeight;
                BlockDepth = blockDepth;
                TargetBuffer = targetBuffer;
            }
        }
    }
}
