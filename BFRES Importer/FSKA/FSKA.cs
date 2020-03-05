using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;
using System.Diagnostics;

namespace BFRES_Importer
{
    class FSKA
    {

        public static void WriteSkeletalAnimations(XmlWriter writer, ResU.ResFile res)
        {
            for (int i = 0; i < res.SkeletalAnims.Count; i++)
            {
                WriteSkeletalAnimation(writer, res.SkeletalAnims[i]);
            }
        }

        private static void WriteSkeletalAnimation(XmlWriter writer, SkeletalAnim anim)
        {
            writer.WriteStartElement("Anim");
            writer.WriteAttributeString("Name", anim.Name);
            writer.WriteAttributeString("IsBaked", anim.Baked.ToString());
            writer.WriteAttributeString("IsLooping", anim.Loop.ToString());
            writer.WriteAttributeString("ScalingType", anim.FlagsScale.ToString());
            writer.WriteAttributeString("RotationType", anim.FlagsRotate.ToString());
            writer.WriteAttributeString("FrameCount", anim.FrameCount.ToString());
            writer.WriteAttributeString("BoneAnimationCount", anim.BoneAnims.Count.ToString());
            writer.WriteAttributeString("BakedSize", anim.BakedSize.ToString());
            writer.WriteAttributeString("UserDataCount", anim.UserData.Count.ToString()); // TODO Is this printing IK data??
            string bindIndices = "";
            for (int i = 0; i < anim.BindIndices.Length; i++)
            {
                short index = (short)anim.BindIndices[i];
                bindIndices += index.ToString() + ",";
            }
            bindIndices = bindIndices.Trim(',');
            writer.WriteAttributeString("BindIndices", bindIndices);

            writer.WriteStartElement("BoneAnims");
            foreach (BoneAnim boneAnim in anim.BoneAnims)
            {
                WriteBoneAnimData(writer, boneAnim);

            }
            writer.WriteEndElement();

            writer.WriteStartElement("UserDatas");
            for (int i = 0; i < anim.UserData.Count; i++)
            {
                writer.WriteStartElement("UserData");
                writer.WriteAttributeString("Name", anim.UserData[i].Name.ToString());
                writer.WriteAttributeString("Type", anim.UserData[i].Type.ToString());
                string values = "";
                switch (anim.UserData[i].Type)
                {
                    case UserDataType.Int32:
                        foreach (int value in anim.UserData[i].GetValueInt32Array())
                        {
                            values += value.ToString() + ',';
                        }
                        values = values.Trim(',');
                        writer.WriteAttributeString("Values", values);
                        break;
                    case UserDataType.Single:
                        foreach (float value in anim.UserData[i].GetValueSingleArray())
                        {
                            values += value.ToString() + ',';
                        }
                        values = values.Trim(',');
                        writer.WriteAttributeString("Values", values);
                        break;
                    case UserDataType.String:
                        foreach (string value in anim.UserData[i].GetValueStringArray())
                        {
                            values += value.ToString() + ',';
                        }
                        values = values.Trim(',');
                        writer.WriteAttributeString("Values", values);
                        break;
                    case UserDataType.WString:
                        foreach (string value in anim.UserData[i].GetValueStringArray())
                        {
                            values += value.ToString() + ',';
                        }
                        values = values.Trim(',');
                        writer.WriteAttributeString("Values", values);
                        break;
                    case UserDataType.Byte:
                        foreach (byte value in anim.UserData[i].GetValueByteArray())
                        {
                            values += value.ToString() + ',';
                        }
                        values = values.Trim(',');
                        writer.WriteAttributeString("Values", values);
                        break;
                    default:
                        break;
                }
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
            // anim.BindSkeleton just know this exists

            //writer.WriteAttributeString("CurveCount", anim..ToString()); Where is this curve information??

            writer.WriteEndElement();
        }

        private static void WriteBoneAnimData(XmlWriter writer, BoneAnim boneAnim)
        {
            writer.WriteStartElement("BoneAnim");
            writer.WriteAttributeString("Name", boneAnim.Name);
            writer.WriteEndElement();
        }

    }
}
