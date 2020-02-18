using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using Syroot.NintenTools.Bfres.GX2;
using System.Collections.Generic;
using Syroot.BinaryData;
using System.IO;
using System.Xml;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;

namespace BFRES_Importer
{
    class FMAT
    {
        public static float[] ValueFloat;
        public static bool[] ValueBool;
        public static uint[] ValueUint;
        public static int[] ValueInt;
        public static byte[] ValueReserved;
        public static Srt2D ValueSrt2D;
        public static Srt3D ValueSrt3D;
        public static TexSrt ValueTexSrt;
        public static TexSrtEx ValueTexSrtEx;

        private static void ReadSRT2D(BinaryDataReader reader)
        {
            ValueSrt2D = new Srt2D();
            Syroot.Maths.Vector2F scaleVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueSrt2D.Scaling = scaleVector2F;
            ValueSrt2D.Rotation = reader.ReadSingle();
            Syroot.Maths.Vector2F transVector2F = new Syroot.Maths.Vector2F(reader.ReadSingle(), reader.ReadSingle());
            ValueSrt2D.Translation = transVector2F;
        }
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
        public static void ReadMaterial(XmlWriter writer, Material mat)
        {
            writer.WriteStartElement("FMAT");
            writer.WriteAttributeString("Name", mat.Name);

            if (mat.Flags == MaterialFlags.Visible)
                //m.Enabled = true;
                writer.WriteAttributeString("Visible", "True");
            else
                //m.Enabled = false;
                writer.WriteAttributeString("Visible", "False");

            ReadRenderInfo(writer, mat);
            ReadShaderAssign(writer, mat);
            //SetActiveGame();
            ReadShaderParams(writer, mat);
            //ReadTextureRefs(mat);
            //ReadRenderState(mat.RenderState);
            //UpdateRenderPass();

            writer.WriteEndElement();
        }

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
                writer.WriteAttributeString("Type"         , param.Type.ToString()         );
                writer.WriteAttributeString("Name"         , param.Name                    );
                writer.WriteAttributeString("HasPadding"   , param.UsePadding.ToString()   );
                writer.WriteAttributeString("PaddingLength", param.PaddingLength.ToString());
                writer.WriteAttributeString("DependedIndex", param.DependedIndex.ToString());
                writer.WriteAttributeString("DependIndex"  , param.DependIndex.ToString()  );

                reader.Seek(param.DataOffset, System.IO.SeekOrigin.Begin);
                WriteValue(writer, reader, (int)param.DataSize, param.Type);


                writer.WriteEndElement();
                }
            reader.Close();
            writer.WriteEndElement();
        }

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
    }
}
