using System;
using System.IO;
using System.Xml;
using System.Text;
using ResU = Syroot.NintenTools.Bfres;

namespace BFRES_Importer
{
    class Program
    {
        const string AssetDir = "../../../../TestAssets/";
        const string OutputDir = "../../../../MedianDumps/";
        public static string FilePath;

        static void Main(string[] args)
        {
            FilePath = (AssetDir + "Npc_Gerudo_Queen_Animation.bfres");
            ResU.ResFile res = new ResU.ResFile(FilePath);
            WriteResToXML(res);
        }

        public static void WriteResToXML(ResU.ResFile res)
        {
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "    ";
            settings.NewLineOnAttributes = false;
            settings.OmitXmlDeclaration = true;
            settings.Encoding = new UTF8Encoding(false); // The false means, do not emit the BOM.

            if (!Directory.Exists(Program.OutputDir))
                Directory.CreateDirectory(Program.OutputDir);
            XmlWriter writer = XmlWriter.Create(Program.OutputDir + "Dump.xml", settings);

            writer.WriteStartDocument();
            writer.WriteStartElement("BFRES");
            if (res.Models.Count > 0)
            {
                for (int ii = 0; ii < res.Models.Count; ii++)
                {
                    FMDL fMDL = new FMDL();
                    fMDL.WriteFMDLData(writer, res.Models[ii]);
                }
            }

            if (res.Textures.Count > 0)
            {
                writer.WriteStartElement("FTEXes");
                for (int ii = 0; ii < res.Textures.Count; ii++)
                {
                    JPTexture jpTexture = new JPTexture();
                    jpTexture.Read(res.Textures[ii]);
                    if (jpTexture.isTex2)
                        for (int i = 1; i < jpTexture.MipCount; i++)
                        {
                            if (!Directory.Exists(OutputDir + "Mips/"))
                                Directory.CreateDirectory(OutputDir + "Mips/");
                            jpTexture.SaveBitMap(OutputDir + "Mips/" + jpTexture.Name + i + ".tga", false, false, 0, i);
                        }
                    else
                        jpTexture.SaveBitMap(OutputDir + jpTexture.Name + ".tga");
                    FTEX.WriteFTEXData(writer, res.Textures[ii]);
                }
                writer.WriteEndElement();
            }

            if (res.SkeletalAnims.Count > 0)
            {
                writer.WriteStartElement("FSKA");
                FSKA.WriteSkeletalAnimations(writer, res);
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
            writer.WriteEndDocument();
            writer.Close();
        }

        /// <summary>
        /// Returns string in a format of "X,Y" without parentheses.
        /// </summary>
        /// <param name="vec2"></param>
        /// <returns></returns>
        public static string Vector2ToString(OpenTK.Vector2 vec2)
        {
            return vec2.X.ToString() + "," + vec2.Y.ToString();
        }
        /// <summary>
        /// Returns string in a format of "X,Y,Z" without parentheses.
        /// </summary>
        /// <param name="vec3"></param>
        /// <returns></returns>
        public static string Vector3ToString(OpenTK.Vector3 vec3)
        {
            return vec3.X.ToString() + "," + vec3.Y.ToString() + "," + vec3.Z.ToString();
        }
        /// <summary>
        /// Returns string in a format of "X,Y,Z" without parentheses.
        /// </summary>
        /// <param name="vec3"></param>
        /// <returns></returns>
        public static string Vector3FToString(Syroot.Maths.Vector3F vec3)
        {
            return vec3.X.ToString() + "," + vec3.Y.ToString() + "," + vec3.Z.ToString();
        }
        /// <summary>
        /// Returns string in a format of "X,Y,Z,W" without parentheses.
        /// </summary>
        /// <param name="vec4"></param>
        /// <returns></returns>
        public static string Vector4ToString(OpenTK.Vector4 vec4)
        {
            return vec4.X.ToString() + "," + vec4.Y.ToString() + "," + vec4.Z.ToString() + "," + vec4.W.ToString();
        }
        /// <summary>
        /// Returns string in a format of "X,Y,Z,W" without parentheses.
        /// </summary>
        /// <param name="vec4"></param>
        /// <returns></returns>
        public static string Vector4FToString(Syroot.Maths.Vector4F vec4)
        {
            return vec4.X.ToString() + "," + vec4.Y.ToString() + "," + vec4.Z.ToString() + "," + vec4.W.ToString();
        }
    }
}
