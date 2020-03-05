using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;
using System.Diagnostics;
using Toolbox.Library.Animations;
using Bfres.Structs;

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

            // Flags 
            writer.WriteAttributeString("UseScale", boneAnim.UseScale.ToString());
            writer.WriteAttributeString("UseRotation", boneAnim.UseRotation.ToString());
            writer.WriteAttributeString("UseTranslation", boneAnim.UseTranslation.ToString());

            // writer.WriteAttributeString("FlagsCurve", boneAnim.FlagsCurve); // how do

            writer.WriteAttributeString("ApplySegmentScaleCompensate", boneAnim.ApplySegmentScaleCompensate.ToString());
            writer.WriteAttributeString("ApplyScaleUniform", boneAnim.ApplyScaleUniform.ToString());
            writer.WriteAttributeString("ApplyScaleVolumeOne", boneAnim.ApplyScaleVolumeOne.ToString());
            writer.WriteAttributeString("ApplyScaleOne", boneAnim.ApplyScaleOne.ToString());
            writer.WriteAttributeString("ApplyRotateZero", boneAnim.ApplyRotateZero.ToString());
            writer.WriteAttributeString("ApplyTranslateZero", boneAnim.ApplyTranslateZero.ToString());
            writer.WriteAttributeString("ApplyRotateTranslateZero", boneAnim.ApplyRotateTranslateZero.ToString());
            writer.WriteAttributeString("ApplyIdentity", boneAnim.ApplyIdentity.ToString());

            writer.WriteAttributeString("BeginRotate", boneAnim.BeginRotate.ToString()); // Unknown purpose
            writer.WriteAttributeString("BeginTranslate", boneAnim.BeginTranslate.ToString()); // Unknown purpose

            // Base Data stores the initial state of bones in the animations
            // writer.WriteAttributeString("BaseDataFlags", boneAnim.BaseData.Flags    .ToString()); // Unused
            writer.WriteAttributeString("Scale", boneAnim.BaseData.Scale.ToString());
            writer.WriteAttributeString("Rotate", boneAnim.BaseData.Rotate.ToString());
            writer.WriteAttributeString("Translate", boneAnim.BaseData.Translate.ToString());
            // writer.WriteAttributeString("BaseDataFlags", boneAnim.BaseData.Padding  .ToString()); // Unused

            writer.WriteAttributeString("CurveCount", boneAnim.Curves.Count.ToString());

            foreach (AnimCurve animCurve in boneAnim.Curves)
            {
                writer.WriteStartElement("Curve");
                WriteBoneAnimCurve(writer, animCurve);
                writer.WriteEndElement();
            }


            writer.WriteEndElement();
        }

        public static void WriteBoneAnimCurve(XmlWriter writer, AnimCurve animCurve)
        {
            // Flags
            writer.WriteAttributeString("FrameType", animCurve.FrameType.ToString());
            writer.WriteAttributeString("KeyType", animCurve.KeyType.ToString());
            writer.WriteAttributeString("CurveType", animCurve.CurveType.ToString());

            writer.WriteAttributeString("KeyCount", animCurve.Keys.Length.ToString());

            writer.WriteAttributeString("StartFrame", animCurve.StartFrame.ToString());
            writer.WriteAttributeString("EndFrame", animCurve.EndFrame.ToString());

            writer.WriteAttributeString("DataScale", animCurve.Scale.ToString()); // multiplier to the raw key values to get the final result. Together with Data offset, it is chosen carefully to consider an optimal granularity between the stored values.
            writer.WriteAttributeString("DataOffset", animCurve.Offset.ToString()); // added to the raw values (after multiplying them with Data scale) to get the final key value

            writer.WriteAttributeString("DataDelta", animCurve.Delta.ToString()); // stores the difference between the first and last key value

            for (int i = 0; i < animCurve.Keys.Length; i++)
            {
                writer.WriteStartElement("Key");

                Animation.KeyGroup grp = new Animation.KeyGroup();

                for (int j = 0; j < animCurve.Keys.Length; j++)
                {

                }

                writer.WriteEndElement();
            }
        }
    }
}
