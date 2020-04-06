using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using Syroot.NintenTools.Bfres.GX2;
using System.Collections.Generic;
using Syroot.BinaryData;
using System.IO;
using System.Xml;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;
using System.Diagnostics;

namespace BFRES_Importer
{
    class FMAT
    {
        public static float[]  ValueFloat;
        public static bool[]   ValueBool;
        public static uint[]   ValueUint;
        public static int[]    ValueInt;
        public static byte[]   ValueReserved;
        public static Srt2D    ValueSrt2D;
        public static Srt3D    ValueSrt3D;
        public static TexSrt   ValueTexSrt;
        public static TexSrtEx ValueTexSrtEx;

        public bool HasDiffuseMap              = false;
        public bool HasNormalMap               = false;
        public bool HasSpecularMap             = false;
        public bool HasEmissionMap             = false;
        public bool HasDiffuseLayer            = false;
        public bool HasTeamColorMap            = false; //Splatoon uses this (TLC)
        public bool HasTransparencyMap         = false;
        public bool HasShadowMap               = false;
        public bool HasAmbientOcclusionMap     = false;
        public bool HasLightMap                = false;
        public bool HasSphereMap               = false;
        public bool HasSubSurfaceScatteringMap = false;

        //PBR (Switch) data
        public bool HasMetalnessMap = false;
        public bool HasRoughnessMap = false;
        public bool HasMRA = false;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reader"></param>
        private static void ReadSRT2D(BinaryDataReader reader)
        {
            ValueSrt2D = new Srt2D();
            Syroot.Maths.Vector2F scaleVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueSrt2D.Scaling = scaleVector2F;
            ValueSrt2D.Rotation = reader.ReadSingle();
            Syroot.Maths.Vector2F transVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueSrt2D.Translation = transVector2F;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="reader"></param>
        private static void ReadSRT3D(BinaryDataReader reader)
        {
            ValueSrt3D = new Srt3D();
            Syroot.Maths.Vector3F scaleVector3F = new Syroot.Maths.Vector3F(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
            ValueSrt3D.Scaling = scaleVector3F;
            Syroot.Maths.Vector3F rotVector3F = new Syroot.Maths.Vector3F(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
            ValueSrt3D.Rotation = rotVector3F;
            Syroot.Maths.Vector3F transVector3F = new Syroot.Maths.Vector3F(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
            ValueSrt3D.Translation = transVector3F;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="reader"></param>
        private static void ReadTexSrt(BinaryDataReader reader)
        {
            ValueTexSrt = new TexSrt();
            ValueTexSrt.Mode = reader.ReadEnum<TexSrtMode>(false);
            Syroot.Maths.Vector2F scaleVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueTexSrt.Scaling = scaleVector2F;
            ValueTexSrt.Rotation = reader.ReadSingle();
            Syroot.Maths.Vector2F transVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueTexSrt.Translation = transVector2F;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="reader"></param>
        private static void ReadTexSrtEx(BinaryDataReader reader)
        {
            ValueTexSrtEx = new TexSrtEx();
            ValueTexSrtEx.Mode = reader.ReadEnum<TexSrtMode>(true);
            Syroot.Maths.Vector2F scaleVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueTexSrtEx.Scaling = scaleVector2F;
            ValueTexSrtEx.Rotation = reader.ReadSingle();
            Syroot.Maths.Vector2F transVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueTexSrtEx.Translation = transVector2F;
            ValueTexSrtEx.MatrixPointer = reader.ReadUInt32();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void ReadMaterial(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("FMAT");
            writer.WriteAttributeString("Name", mat.Name);

            if( mat.Flags == MaterialFlags.Visible )
            {
                writer.WriteAttributeString( "Visible", "True" );
            }
            else
            {
                Program.AssertAndLog( Program.ErrorType.eInvisibleMaterial, false, "Material is invisible. How are we handling this? May not be a problem." );
                writer.WriteAttributeString( "Visible", "False" );
            }

            ReadRenderInfo(writer, mat);
            ReadShaderAssign(writer, mat);
            //SetActiveGame();
            ReadShaderParams(writer, mat);
            ReadTextureRefs(writer, mat);
            ReadRenderState(writer, mat.RenderState);
            UpdateRenderPass(writer, mat);

            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void ReadRenderInfo(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("RenderInfo");
            foreach (RenderInfo rnd in mat.RenderInfos.Values)
            {
                writer.WriteStartElement("RenderInfo");
                writer.WriteAttributeString("Name", rnd.Name);
                writer.WriteAttributeString("Type", rnd.Type.ToString());
                switch (rnd.Type)
                {
                    case RenderInfoType.Int32:
                        string tempInt32s = "";
                        foreach (int value in rnd.GetValueInt32s())
                        {
                            tempInt32s += (value.ToString() + ',');
                        }
                        tempInt32s = tempInt32s.Trim(',');
                        writer.WriteAttributeString("ValueInt", tempInt32s);
                        break;
                    case RenderInfoType.Single:
                        string tempFloats = "";
                        foreach (float value in rnd.GetValueSingles())
                        {
                            tempFloats += (value.ToString() + ',');
                        }
                        tempFloats = tempFloats.Trim(',');
                        writer.WriteAttributeString("ValueFloat", tempFloats);
                        break;
                    case RenderInfoType.String:
                        string tempStrings = "";
                        foreach (string value in rnd.GetValueStrings())
                        {
                            tempStrings += (value + ',');
                        }
                        tempStrings = tempStrings.Trim(',');
                        writer.WriteAttributeString("ValueString", tempStrings);
                        break;
                }
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void ReadShaderAssign(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("ShaderAssign");

            if (mat.ShaderAssign == null)
                mat.ShaderAssign = new ShaderAssign();
            if (mat.ShaderAssign.ShaderOptions == null)
                mat.ShaderAssign.ShaderOptions = new ResDict<ResString>();
            if (mat.ShaderAssign.AttribAssigns == null)
                mat.ShaderAssign.AttribAssigns = new ResDict<ResString>();
            if (mat.ShaderAssign.SamplerAssigns == null)
                mat.ShaderAssign.SamplerAssigns = new ResDict<ResString>();

            writer.WriteAttributeString("ShaderArchive", mat.ShaderAssign.ShaderArchiveName);
            writer.WriteAttributeString("ShaderModel", mat.ShaderAssign.ShadingModelName);

            writer.WriteStartElement("ShaderOptions");
            foreach (var op in mat.ShaderAssign.ShaderOptions)
                writer.WriteAttributeString(op.Key, op.Value);
            writer.WriteEndElement();

            if (mat.ShaderAssign.SamplerAssigns != null)
            {
                writer.WriteStartElement("SamplerAssigns");
                foreach (var op in mat.ShaderAssign.SamplerAssigns)
                    writer.WriteAttributeString(op.Key, op.Value);
                writer.WriteEndElement();

            }
            if (mat.ShaderAssign.AttribAssigns != null)
            {
                writer.WriteStartElement("AttribAssigns");
                foreach (var op in mat.ShaderAssign.AttribAssigns)
                    writer.WriteAttributeString(op.Key, op.Value);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void ReadShaderParams(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("ShaderParams");

            if (mat.ShaderParamData == null)
                return;

            BinaryDataReader reader = new BinaryDataReader(new System.IO.MemoryStream(mat.ShaderParamData));
            reader.ByteOrder = Syroot.BinaryData.ByteOrder.BigEndian;
            foreach (ShaderParam param in mat.ShaderParams.Values)
            {
                writer.WriteStartElement("ShaderParam");

                writer.WriteAttributeString("Type", param.Type.ToString());
                writer.WriteAttributeString("Name", param.Name);
                writer.WriteAttributeString("HasPadding", param.UsePadding.ToString());
                writer.WriteAttributeString("PaddingLength", param.PaddingLength.ToString());
                writer.WriteAttributeString("DependedIndex", param.DependedIndex.ToString());
                writer.WriteAttributeString("DependIndex", param.DependIndex.ToString());

                reader.Seek(param.DataOffset, System.IO.SeekOrigin.Begin);
                WriteValue(writer, reader, (int)param.DataSize, param.Type);


                writer.WriteEndElement();
            }
            reader.Close();
            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="reader"></param>
        /// <param name="Size"></param>
        /// <param name="type"></param>
        public static void WriteValue(XmlWriter writer, BinaryDataReader reader, int Size, ShaderParamType type)
        {
            switch (type)
            {
                case ShaderParamType.Bool:
                case ShaderParamType.Bool2:
                case ShaderParamType.Bool3:
                case ShaderParamType.Bool4:
                    ValueBool = reader.ReadBooleans(Size / sizeof(bool));
                    string tempBools = "";
                    foreach (bool value in ValueBool)
                    {
                        tempBools += (value.ToString() + ',');
                    }
                    tempBools = tempBools.Trim(',');
                    writer.WriteAttributeString("Value", tempBools);
                    break;
                case ShaderParamType.Float:
                case ShaderParamType.Float2:
                case ShaderParamType.Float3:
                case ShaderParamType.Float4:
                case ShaderParamType.Float2x2:
                case ShaderParamType.Float2x3:
                case ShaderParamType.Float2x4:
                case ShaderParamType.Float4x2:
                case ShaderParamType.Float4x3:
                case ShaderParamType.Float4x4:
                    ValueFloat = reader.ReadSingles(Size / sizeof(float));
                    string tempFloat = "";
                    foreach (float value in ValueFloat)
                    {
                        tempFloat += (value.ToString() + ',');
                    }
                    tempFloat = tempFloat.Trim(',');
                    writer.WriteAttributeString("Value", tempFloat);
                    break;
                case ShaderParamType.Int:
                case ShaderParamType.Int2:
                case ShaderParamType.Int3:
                case ShaderParamType.Int4:
                    ValueInt = reader.ReadInt32s(Size / sizeof(int));
                    string tempInts = "";
                    foreach (int value in ValueInt)
                    {
                        tempInts += (value.ToString() + ',');
                    }
                    tempInts = tempInts.Trim(',');
                    writer.WriteAttributeString("Value", tempInts);
                    break;
                case ShaderParamType.Reserved2:
                case ShaderParamType.Reserved3:
                case ShaderParamType.Reserved4:
                    ValueReserved = reader.ReadBytes(Size / sizeof(byte));
                    string tempBytes = "";
                    foreach (byte value in ValueReserved)
                    {
                        tempBytes += (value.ToString() + ',');
                    }
                    tempBytes = tempBytes.Trim(',');
                    writer.WriteAttributeString("Value", tempBytes);
                    break;
                case ShaderParamType.Srt2D:
                    ReadSRT2D(reader);
                    string srt2DScaling = "";
                    srt2DScaling += (ValueSrt2D.Scaling.X.ToString() + ',');
                    srt2DScaling += (ValueSrt2D.Scaling.Y.ToString());
                    writer.WriteAttributeString("ValueScaleXY", srt2DScaling);
                    string srt2DRot = "";
                    srt2DRot += (ValueSrt2D.Rotation.ToString());
                    writer.WriteAttributeString("ValueRotationX", srt2DRot);
                    string srt2DTranslation = "";
                    srt2DTranslation += (ValueSrt2D.Translation.X.ToString() + ',');
                    srt2DTranslation += (ValueSrt2D.Translation.Y.ToString());
                    writer.WriteAttributeString("ValueTranslationXY", srt2DTranslation);
                    break;
                case ShaderParamType.Srt3D:
                    ReadSRT3D(reader);
                    string srt3DScaling = "";
                    srt3DScaling += (ValueSrt3D.Scaling.X.ToString() + ',');
                    srt3DScaling += (ValueSrt3D.Scaling.Y.ToString() + ',');
                    srt3DScaling += (ValueSrt3D.Scaling.Z.ToString());
                    writer.WriteAttributeString("ValueScaleXYZ", srt3DScaling);
                    string srt3DRot = "";
                    srt3DRot += (ValueSrt3D.Rotation.X.ToString() + ',');
                    srt3DRot += (ValueSrt3D.Rotation.Y.ToString() + ',');
                    srt3DRot += (ValueSrt3D.Rotation.Z.ToString());
                    writer.WriteAttributeString("ValueRotationXYZ", srt3DRot);
                    string srt3DTranslation = "";
                    srt3DTranslation += (ValueSrt3D.Translation.X.ToString() + ',');
                    srt3DTranslation += (ValueSrt3D.Translation.Y.ToString() + ',');
                    srt3DTranslation += (ValueSrt3D.Translation.Z.ToString());
                    writer.WriteAttributeString("ValueTranslationXYZ", srt3DTranslation);
                    break;
                case ShaderParamType.TexSrt:
                    ReadTexSrt(reader);
                    writer.WriteAttributeString("Mode", ValueTexSrt.Mode.ToString());
                    string texScaling = "";
                    texScaling += (ValueTexSrt.Scaling.X.ToString() + ',');
                    texScaling += (ValueTexSrt.Scaling.Y.ToString());
                    writer.WriteAttributeString("ValueScaleXY", texScaling);
                    string texRot = "";
                    texRot += (ValueTexSrt.Rotation.ToString());
                    writer.WriteAttributeString("ValueRotationX", texRot);
                    string texTranslation = "";
                    texTranslation += (ValueTexSrt.Translation.X.ToString() + ',');
                    texTranslation += (ValueTexSrt.Translation.Y.ToString());
                    writer.WriteAttributeString("ValueTranslationXY", texTranslation);
                    break;
                case ShaderParamType.TexSrtEx:
                    ReadTexSrtEx(reader);
                    writer.WriteAttributeString("Mode", ValueTexSrtEx.Mode.ToString());
                    string texExScaling = "";
                    texExScaling += (ValueTexSrtEx.Scaling.X.ToString() + ',');
                    texExScaling += (ValueTexSrtEx.Scaling.Y.ToString());
                    writer.WriteAttributeString("ValueScaleXY", texExScaling);
                    string texExRot = "";
                    texExRot += (ValueTexSrtEx.Rotation.ToString());
                    writer.WriteAttributeString("ValueRotationX", texExRot);
                    string texExTranslation = "";
                    texExTranslation += (ValueTexSrtEx.Translation.X.ToString() + ',');
                    texExTranslation += (ValueTexSrtEx.Translation.Y.ToString());
                    writer.WriteAttributeString("ValueTranslationXY", texExTranslation);
                    writer.WriteAttributeString("MatrixPointer", ValueTexSrtEx.MatrixPointer.ToString());
                    break;
                case ShaderParamType.UInt:
                case ShaderParamType.UInt2:
                case ShaderParamType.UInt3:
                case ShaderParamType.UInt4:
                    ValueUint = reader.ReadUInt32s(Size / sizeof(uint));
                    string tempUints = "";
                    foreach (uint value in ValueUint)
                    {
                        tempUints += (value.ToString() + ',');
                    }
                    tempUints = tempUints.Trim(',');
                    writer.WriteAttributeString("Value", tempUints);
                    break;
                // Invalid
                default:
                    //throw new ArgumentException($"Invalid {nameof(ShaderParamType)} {type}.", nameof(type));
                    break;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void ReadTextureRefs(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("TextureRefs");

            int AlbedoCount = 0;
            int id = 0;
            string TextureName = "";
            if (mat.TextureRefs == null)
                mat.TextureRefs = new List<TextureRef>();

            writer.WriteAttributeString("TextureCount", mat.TextureRefs.Count.ToString());

            // Write what kinds of textures this material contains
            for (int i = 0; i < mat.TextureRefs.Count; i++)
            {
                string texSamplerName;
                mat.Samplers.TryGetKey(mat.Samplers[i], out texSamplerName);
                string useSampler = texSamplerName;

                //Use the fragment sampler in the shader assign section. It's usually more accurate this way
                if (mat.ShaderAssign.SamplerAssigns.ContainsKey(useSampler))
                    useSampler = mat.ShaderAssign.SamplerAssigns[useSampler];

                if (useSampler == "s_diffuse")          writer.WriteAttributeString("HasDiffuseMap"             , "True");
                else if (useSampler == "s_normal")      writer.WriteAttributeString("HasNormalMap"              , "True");
                else if (useSampler == "s_specmask")    writer.WriteAttributeString("HasSpecularMap"            , "True");
                else if (useSampler == "_a0")           writer.WriteAttributeString("HasDiffuseMap"             , "True");
                else if (useSampler == "_a1")           writer.WriteAttributeString("HasDiffuseLayer2Map"       , "True");
                else if (useSampler == "_a2")           writer.WriteAttributeString("HasDiffuseLayer3Map"       , "True");
                else if (useSampler == "_n0")           writer.WriteAttributeString("HasNormalMap"              , "True");
                else if (useSampler == "_s0")           writer.WriteAttributeString("HasSpecularMap"            , "True");
                else if (useSampler == "_ao0")          writer.WriteAttributeString("HasAmbientOcclusionMap"    , "True");
                else if (useSampler == "_e0")           writer.WriteAttributeString("HasEmissionMap"            , "True");
                else if (useSampler == "_b0")           writer.WriteAttributeString("HasShadowMap"              , "True");
                else if (useSampler == "_b1")           writer.WriteAttributeString("HasLightMap"               , "True");
                else if (TextureName.Contains("Emm")) { writer.WriteAttributeString("HasEmissionMap"            , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("Spm")) { writer.WriteAttributeString("HasSpecularMap"            , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("b00")) { writer.WriteAttributeString("HasShadowMap"              , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (texSamplerName == "bake0")   { writer.WriteAttributeString("HasShadowMap"              , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("Moc")) { writer.WriteAttributeString("HasAmbientOcclusionMap"    , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("AO"))  { writer.WriteAttributeString("HasAmbientOcclusionMap"    , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("b01")) { writer.WriteAttributeString("HasLightMap"               , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("MRA")) { writer.WriteAttributeString("HasRoughnessMap"           , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("mtl")) { writer.WriteAttributeString("HasMetalnessMap"           , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("rgh")) { writer.WriteAttributeString("HasRoughnessMap"           , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (TextureName.Contains("sss")) { writer.WriteAttributeString("HasSubSurfaceScatteringMap", "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else if (texSamplerName == "_ao0")    { writer.WriteAttributeString("HasAmbientOcclusionMap"    , "True"); Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported"); }
                else Program.AssertAndLog( Program.ErrorType.eUnsupportedTexture, false, "Texture type not yet supported");
            }

            int textureUnit = 1;
            foreach (var tex in mat.TextureRefs)
            {

                writer.WriteStartElement("Texture");
                TextureName = tex.Name;

                // Asserts for assumptions made
                Program.AssertAndLog( Program.ErrorType.eUnsupportedTexClamp, mat.Samplers[ id ].TexSampler.ClampX == GX2TexClamp.Wrap,  $"{tex.Name} tex sampler clamp X is set to {mat.Samplers[id].TexSampler.ClampX} and not Wrap"  );
                Program.AssertAndLog( Program.ErrorType.eUnsupportedTexClamp, mat.Samplers[ id ].TexSampler.ClampY == GX2TexClamp.Wrap,  $"{tex.Name} tex sampler clamp Y is set to {mat.Samplers[id].TexSampler.ClampY} and not Wrap"  );
                Program.AssertAndLog( Program.ErrorType.eUnsupportedTexClamp, mat.Samplers[ id ].TexSampler.ClampZ == GX2TexClamp.Clamp, $"{tex.Name} tex sampler clamp Z is set to {mat.Samplers[id].TexSampler.ClampZ} and not Clamp" );

                writer.WriteAttributeString("TextureName", tex.Name);
                writer.WriteAttributeString("ClampX", mat.Samplers[id].TexSampler.ClampX.ToString());
                writer.WriteAttributeString("ClampY", mat.Samplers[id].TexSampler.ClampY.ToString());
                writer.WriteAttributeString("ClampZ", mat.Samplers[id].TexSampler.ClampZ.ToString());

                string texSamplerName;
                mat.Samplers.TryGetKey(mat.Samplers[id], out texSamplerName);
                string useSampler = texSamplerName;
                writer.WriteAttributeString("TexSamplerName", texSamplerName);

                //Use the fragment sampler in the shader assign section. It's usually more accurate this way
                if (mat.ShaderAssign.SamplerAssigns.ContainsKey(useSampler))
                    useSampler = mat.ShaderAssign.SamplerAssigns[useSampler];
                writer.WriteAttributeString("UseSampler", useSampler);

                if (mat.Samplers[id].TexSampler.MinFilter == GX2TexXYFilterType.Point)
                    writer.WriteAttributeString("MinFilter", "Nearest");
                if (mat.Samplers[id].TexSampler.MagFilter == GX2TexXYFilterType.Point)
                    writer.WriteAttributeString("MagFilter", "Nearest");
                if (mat.Samplers[id].TexSampler.MinFilter == GX2TexXYFilterType.Bilinear)
                    writer.WriteAttributeString("MinFilter", "Linear");
                if (mat.Samplers[id].TexSampler.MagFilter == GX2TexXYFilterType.Bilinear)
                    writer.WriteAttributeString("MagFilter", "Linear");

                // TODO : Figure out what the hell any of these are and why they are relevant.
                writer.WriteAttributeString("ZFilter"            , mat.Samplers[id].TexSampler.ZFilter            .ToString());
                writer.WriteAttributeString("MipFilter"          , mat.Samplers[id].TexSampler.MipFilter          .ToString());
                writer.WriteAttributeString("MaxAnisotropicRatio", mat.Samplers[id].TexSampler.MaxAnisotropicRatio.ToString());
                writer.WriteAttributeString("BorderType"         , mat.Samplers[id].TexSampler.BorderType         .ToString());
                writer.WriteAttributeString("DepthCompareFunc"   , mat.Samplers[id].TexSampler.DepthCompareFunc   .ToString());
                writer.WriteAttributeString("MinLod"             , mat.Samplers[id].TexSampler.MinLod             .ToString()); Program.AssertAndLog( Program.ErrorType.eUnsupportedMinLOD, mat.Samplers[ id ].TexSampler.MinLod == 0, $"{tex.Name} tex sampler MinLod is set to {mat.Samplers[id].TexSampler.MinLod} and not 0");
                writer.WriteAttributeString("MaxLod"             , mat.Samplers[id].TexSampler.MaxLod             .ToString()); 
                writer.WriteAttributeString("LodBias"            , mat.Samplers[id].TexSampler.LodBias            .ToString());
                writer.WriteAttributeString("DepthCompareEnabled", mat.Samplers[id].TexSampler.DepthCompareEnabled.ToString());

                // TODO This "HasDiffuseMap" etc, should be set as an attribute on the material, not the texture
                if (useSampler == "s_diffuse") { AlbedoCount++; writer.WriteAttributeString("Type", "Diffuse"             ); }
                else if (useSampler == "s_normal")              writer.WriteAttributeString("Type", "Normal"              );
                else if (useSampler == "s_specmask")            writer.WriteAttributeString("Type", "Specular"            );
                else if (useSampler == "_a0") {  AlbedoCount++; writer.WriteAttributeString("Type", "Diffuse"             ); }
                else if (useSampler == "_a1") {  AlbedoCount++; writer.WriteAttributeString("Type", "DiffuseLayer2"       ); }
                else if (useSampler == "_a2") {  AlbedoCount++; writer.WriteAttributeString("Type", "DiffuseLayer3"       ); }
                else if (useSampler == "_n0")                   writer.WriteAttributeString("Type", "Normal"              );
                else if (useSampler == "_s0")                   writer.WriteAttributeString("Type", "Specular"            );
                else if (useSampler == "_ao0")                  writer.WriteAttributeString("Type", "AO"                  );
                else if (useSampler == "_e0")                   writer.WriteAttributeString("Type", "Emission"            );
                else if (useSampler == "_b0")                   writer.WriteAttributeString("Type", "Shadow"              );
                else if (useSampler == "_b1")                   writer.WriteAttributeString("Type", "Light"               );
                else if (TextureName.Contains("Emm"))           writer.WriteAttributeString("Type", "Emission"            );
                else if (TextureName.Contains("Spm"))           writer.WriteAttributeString("Type", "Specular"            );
                else if (TextureName.Contains("b00"))           writer.WriteAttributeString("Type", "Shadow"              );
                else if (texSamplerName == "bake0")             writer.WriteAttributeString("Type", "Shadow"              );
                else if (TextureName.Contains("Moc"))           writer.WriteAttributeString("Type", "AO"                  );
                else if (TextureName.Contains("AO"))            writer.WriteAttributeString("Type", "AO"                  );
                else if (TextureName.Contains("b01"))           writer.WriteAttributeString("Type", "Light"               );
                //Metalness, Roughness, and Cavity Map in one
                else if (TextureName.Contains("MRA"))           writer.WriteAttributeString("Type", "MRA"                 );
                else if (TextureName.Contains("mtl"))           writer.WriteAttributeString("Type", "Metalness"           );
                else if (TextureName.Contains("rgh"))           writer.WriteAttributeString("Type", "Roughness"           );
                else if (TextureName.Contains("sss"))           writer.WriteAttributeString("Type", "SubSurfaceScattering");
                else if (texSamplerName == "_ao0")              writer.WriteAttributeString("Type", "AO"                  );
                else if (texSamplerName == "_sd0")
                {
                    Debug.WriteLine("_sd0 not yet verified to be of a type yet."); // TODO find out what _sd0 is
                    writer.WriteAttributeString("Type", "Shadow");
                }
                writer.WriteAttributeString("textureUnit", textureUnit++.ToString());
                writer.WriteEndElement();

                // TODO I believe this code is irrelevant now. Remove and test it.
                if (tex.Texture != null)
                {
                    writer.WriteStartElement("TextureInfo");

                    // TODO test that this works with texture bfres files
                    writer.WriteAttributeString("CompSelR"   , tex.Texture.CompSelR   .ToString());
                    writer.WriteAttributeString("CompSelG"   , tex.Texture.CompSelG   .ToString());
                    writer.WriteAttributeString("CompSelB"   , tex.Texture.CompSelB   .ToString());
                    writer.WriteAttributeString("CompSelA"   , tex.Texture.CompSelA   .ToString());
                    writer.WriteAttributeString("Name"       , tex.Texture.Name                  );
                    writer.WriteAttributeString("Path"       , tex.Texture.Path                  );
                    writer.WriteAttributeString("Width"      , tex.Texture.Width      .ToString());
                    writer.WriteAttributeString("Height"     , tex.Texture.Height     .ToString());
                    writer.WriteAttributeString("Depth"      , tex.Texture.Depth      .ToString());
                    writer.WriteAttributeString("Swizzle"    , tex.Texture.Swizzle    .ToString());
                    writer.WriteAttributeString("Alignment"  , tex.Texture.Alignment  .ToString());
                    writer.WriteAttributeString("ArrayLength", tex.Texture.ArrayLength.ToString());
                    writer.WriteAttributeString("Pitch"      , tex.Texture.Pitch      .ToString());
                    writer.WriteAttributeString("TileMode"   , tex.Texture.TileMode   .ToString());
                    writer.WriteAttributeString("AAMode"     , tex.Texture.AAMode     .ToString());
                    writer.WriteAttributeString("Dim"        , tex.Texture.Dim        .ToString());
                    writer.WriteAttributeString("Format"     , tex.Texture.Format     .ToString());
                    writer.WriteAttributeString("Data"       , tex.Texture.Data       .ToString());
                    writer.WriteAttributeString("MipData"    , tex.Texture.MipData    .ToString());
                    writer.WriteAttributeString("Regs"       , tex.Texture.Regs       .ToString());
                    writer.WriteAttributeString("UserData"   , tex.Texture.UserData   .ToString());

                    writer.WriteEndElement();
                }

                id++;

            }
            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="renderState"></param>
        public static void ReadRenderState(XmlWriter writer, RenderState renderState)
        {
            writer.WriteStartElement("RenderState");

            // TODO what the fuck is any of this?
            writer.WriteAttributeString("DepthTestEnabled", renderState.DepthTestEnabled.ToString());
            writer.WriteAttributeString("DepthWriteEnabled", renderState.DepthWriteEnabled.ToString());
            writer.WriteAttributeString("DepthFunc", renderState.DepthFunc.ToString());
            writer.WriteAttributeString("StencilTestEnabled", renderState.StencilTestEnabled.ToString());
            writer.WriteAttributeString("BackStencilEnabled", renderState.BackStencilEnabled.ToString());
            writer.WriteAttributeString("FrontStencilFunc", renderState.FrontStencilFunc.ToString());
            writer.WriteAttributeString("FrontStencilFail", renderState.FrontStencilFail.ToString());

            writer.WriteAttributeString("FrontStencilZPass", renderState.FrontStencilZPass.ToString());
            writer.WriteAttributeString("FrontStencilZFail", renderState.FrontStencilZFail.ToString());
            writer.WriteAttributeString("BackStencilFunc", renderState.BackStencilFunc.ToString());
            writer.WriteAttributeString("BackStencilFail", renderState.BackStencilFail.ToString());
            writer.WriteAttributeString("BackStencilZPass", renderState.BackStencilZPass.ToString());
            writer.WriteAttributeString("BackStencilZFail", renderState.BackStencilZFail.ToString());

            writer.WriteAttributeString("PolygonControl", renderState.PolygonControl.ToString());
            // TODO etc...



            writer.WriteEndElement();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="mat"></param>
        public static void UpdateRenderPass(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("RenderPass");
            if( mat != null )
            {
                bool bIsOpaque = mat.RenderState.FlagsMode == ResU.RenderStateFlagsMode.Opaque;
                bool bIsTranslucent = mat.RenderState.FlagsMode == ResU.RenderStateFlagsMode.Translucent;
                bool bIsTransparentMask = mat.RenderState.FlagsMode == ResU.RenderStateFlagsMode.AlphaMask;
                bool bIsCustom = mat.RenderState.FlagsMode == ResU.RenderStateFlagsMode.Custom;

                writer.WriteAttributeString( "IsOpaque"         , bIsOpaque         .ToString() );
                writer.WriteAttributeString( "IsTranslucent"    , bIsTranslucent    .ToString() );
                writer.WriteAttributeString( "IsTransparentMask", bIsTransparentMask.ToString() );
                writer.WriteAttributeString( "IsCustom"         , bIsCustom         .ToString() );
                Program.AssertAndLog( Program.ErrorType.eCustomRenderPass, !bIsCustom, "Render pass is custom" );
            }
            writer.WriteEndElement();
        }
    }
}
