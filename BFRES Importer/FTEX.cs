using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using Syroot.NintenTools.Bfres.GX2;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;

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
}
