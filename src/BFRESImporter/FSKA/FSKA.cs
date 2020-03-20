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
using System;

namespace BFRES_Importer
{
    public enum RotationType
    {
        EULER = 0,
        QUATERNION
    }
    public enum TrackType
    {
        XSCA = 0x4,
        YSCA = 0x8,
        ZSCA = 0xC,
        XPOS = 0x10,
        YPOS = 0x14,
        ZPOS = 0x18,
        XROT = 0x20,
        YROT = 0x24,
        ZROT = 0x28,
        WROT = 0x2C,
    }
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
            JPSkeletalAnim jPSkeletalAnim = new JPSkeletalAnim();
            jPSkeletalAnim.LoadAnimData(anim);

            writer.WriteStartElement("Anim");

            writer.WriteAttributeString("Name"              , anim.Name                      );
            writer.WriteAttributeString("IsBaked"           , anim.Baked          .ToString());
            writer.WriteAttributeString("IsLooping"         , anim.Loop           .ToString());
            writer.WriteAttributeString("ScalingType"       , anim.FlagsScale     .ToString());
            writer.WriteAttributeString("RotationType"      , anim.FlagsRotate    .ToString());
            writer.WriteAttributeString("FrameCount"        , anim.FrameCount     .ToString());
            writer.WriteAttributeString("BoneAnimationCount", anim.BoneAnims.Count.ToString());
            writer.WriteAttributeString("BakedSize"         , anim.BakedSize      .ToString());
            writer.WriteAttributeString("UserDataCount"     , anim.UserData.Count .ToString()); // TODO Is this printing IK data??
            string bindIndices = "";
            for (int i = 0; i < anim.BindIndices.Length; i++)
            {
                short index = (short)anim.BindIndices[i];
                bindIndices += index.ToString() + ",";
            }
            bindIndices = bindIndices.Trim(',');
            writer.WriteAttributeString("BindIndices", bindIndices);

            writer.WriteStartElement("BoneAnims");

            // Check for non hermite curves
            for (int i = 0; i < anim.BoneAnims.Count; i++)
            {
                for (int j = 0; j < anim.BoneAnims[i].Curves.Count; j++)
                {
                    Debug.Assert(anim.BoneAnims[i].Curves[j].CurveType == AnimCurveType.Cubic, "Hey buddy this animation has a non hermite curve");
                }
            }
            
            WriteJPSkeletonBoneAnimData(writer, jPSkeletalAnim);

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

            writer.WriteEndElement();
        }

        public static void WriteJPSkeletonBoneAnimData(XmlWriter writer, JPSkeletalAnim anim)
        {
            foreach (var bone in anim.Bones)
            {
                writer.WriteStartElement("BoneAnim");
                writer.WriteAttributeString("Name", bone.Name);
                writer.WriteAttributeString("Hash", bone.Hash.ToString());
                writer.WriteAttributeString("RotType", bone.RotType.ToString());
                writer.WriteAttributeString("UseSegmentScaleCompensate", bone.UseSegmentScaleCompensate.ToString());
                
                // Animation tracks
                writer.WriteStartElement("AnimationTracks");
                WriteAnimTrack(writer, bone.XSCA);
                WriteAnimTrack(writer, bone.YSCA);
                WriteAnimTrack(writer, bone.ZSCA);

                WriteAnimTrack(writer, bone.XROT);
                WriteAnimTrack(writer, bone.YROT);
                WriteAnimTrack(writer, bone.ZROT);
                WriteAnimTrack(writer, bone.WROT);

                WriteAnimTrack(writer, bone.XPOS);
                WriteAnimTrack(writer, bone.YPOS);
                WriteAnimTrack(writer, bone.ZPOS);
                writer.WriteEndElement();
                writer.WriteEndElement();
            }
        }

        public static void WriteAnimTrack(XmlWriter writer, KeyGroup track)
        {
            writer.WriteStartElement(track.Name);
            writer.WriteAttributeString("Name", track.Name);
            writer.WriteAttributeString("InterpolationType", track.InterpolationType.ToString());
            writer.WriteAttributeString("Constant", track.Constant.ToString());
            writer.WriteAttributeString("FrameCount", track.FrameCount.ToString());
            writer.WriteAttributeString("StartFrame", track.StartFrame.ToString());
            writer.WriteAttributeString("EndFrame", track.EndFrame.ToString());
            writer.WriteAttributeString("Delta", track.Delta.ToString());

            writer.WriteAttributeString("KeyCount", track.Keys.Count.ToString());
            foreach (Animation.KeyFrame keyFrame in track.Keys)
            {
                // TODO there is some other data in here, that maybe is needed. But this dump is getting p big.
                // TODO in fact it's getting so big that we should cut out what is not needed from here when we figure that out
                writer.WriteStartElement("KeyFrame");
                writer.WriteAttributeString("Frame", keyFrame.Frame.ToString());
                writer.WriteAttributeString("Value", keyFrame.Value.ToString());
                writer.WriteAttributeString("IsDegrees", keyFrame.Degrees.ToString());
                writer.WriteAttributeString("Weighted", keyFrame.Weighted.ToString());
                writer.WriteAttributeString("Slope1", keyFrame.Slope1.ToString());
                writer.WriteAttributeString("Slope2", keyFrame.Slope2.ToString());
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }
    }

    class JPSkeletalAnim
    {
        public string Name { get; set; }
        public bool CanLoop { get; set; }
        public bool UseSegmentScaleCompensate = false;
        public List<KeyNode> Bones = new List<KeyNode>();
        public JPSkeletalAnim() { }
        public List<KeyNode> LoadAnimData(ResU.SkeletalAnim ska)
        {
            // Nodes.Clear();
            Bones.Clear();

            CanLoop = ska.FlagsAnimSettings.HasFlag(ResU.SkeletalAnimFlags.Looping);

            foreach (ResU.BoneAnim bn in ska.BoneAnims)
            {
                KeyNode keyNode = new KeyNode();
                keyNode.Name = bn.Name;
                keyNode.UseSegmentScaleCompensate = bn.ApplySegmentScaleCompensate;

                Bones.Add(keyNode);
                //    Nodes.Add(keyNode);

                if (ska.FlagsRotate == ResU.SkeletalAnimFlagsRotate.EulerXYZ)
                    keyNode.RotType = RotationType.EULER;
                else
                    keyNode.RotType = RotationType.QUATERNION;


                if (bn.FlagsBase.HasFlag(ResU.BoneAnimFlagsBase.Scale))
                {
                    keyNode.XSCA.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Scale.X, IsKeyed = true });
                    keyNode.YSCA.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Scale.Y, IsKeyed = true });
                    keyNode.ZSCA.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Scale.Z, IsKeyed = true });
                }
                if (bn.FlagsBase.HasFlag(ResU.BoneAnimFlagsBase.Rotate))
                {
                    keyNode.XROT.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Rotate.X, IsKeyed = true });
                    keyNode.YROT.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Rotate.Y, IsKeyed = true });
                    keyNode.ZROT.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Rotate.Z, IsKeyed = true });
                    keyNode.WROT.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Rotate.W, IsKeyed = true });
                }
                if (bn.FlagsBase.HasFlag(ResU.BoneAnimFlagsBase.Translate))
                {
                    keyNode.XPOS.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Translate.X, IsKeyed = true });
                    keyNode.YPOS.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Translate.Y, IsKeyed = true });
                    keyNode.ZPOS.Keys.Add(new Animation.KeyFrame() { Frame = 0, Value = bn.BaseData.Translate.Z, IsKeyed = true });
                }

                for (int curve = 0; curve < bn.Curves.Count; curve++)
                {
                    Animation.KeyGroup keyGroup = CurveHelper.CreateTrackWiiU(bn.Curves[curve]);
                    keyGroup.AnimDataOffset = bn.Curves[curve].AnimDataOffset;
                    switch (keyGroup.AnimDataOffset)
                    {
                        case (int)TrackType.XPOS: keyNode.XPOS.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.YPOS: keyNode.YPOS.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.ZPOS: keyNode.ZPOS.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.XROT: keyNode.XROT.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.YROT: keyNode.YROT.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.ZROT: keyNode.ZROT.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.WROT: keyNode.WROT.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.XSCA: keyNode.XSCA.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.YSCA: keyNode.YSCA.Keys.AddRange(keyGroup.Keys); break;
                        case (int)TrackType.ZSCA: keyNode.ZSCA.Keys.AddRange(keyGroup.Keys); break;
                        default: throw new Exception("Unknown Anim Offset " + keyGroup.AnimDataOffset);
                    }
                }
            }

            return Bones;
        }
    }

    class KeyNode
    {
        public string Name { get; set; }
        public int Hash = -1;

        public bool UseSegmentScaleCompensate = false;

        public KeyGroup XPOS = new KeyGroup() { Name = "XPOS" };
        public KeyGroup YPOS = new KeyGroup() { Name = "YPOS" };
        public KeyGroup ZPOS = new KeyGroup() { Name = "ZPOS" };

        public RotationType RotType = RotationType.QUATERNION;
        public KeyGroup XROT = new KeyGroup() { Name = "XROT" };
        public KeyGroup YROT = new KeyGroup() { Name = "YROT" };
        public KeyGroup ZROT = new KeyGroup() { Name = "ZROT" };
        public KeyGroup WROT = new KeyGroup() { Name = "WROT" };

        public KeyGroup XSCA = new KeyGroup() { Name = "XSCA" };
        public KeyGroup YSCA = new KeyGroup() { Name = "YSCA" };
        public KeyGroup ZSCA = new KeyGroup() { Name = "ZSCA" };
    }
    public class KeyGroup
    {
        public InterpolationType InterpolationType = InterpolationType.HERMITE;

        //Determines which data to use. Offset will shift for params.
        //Some values detmine the type ie (SRT) or (XYZW)
        public string Name { get; set; }
        public uint AnimDataOffset;
        public bool Constant = false;
        public float Scale { get; set; }
        public float Offset { get; set; }
        public float StartFrame { get; set; }
        public float EndFrame { get; set; }
        public float Delta { get; set; }

        public float CalculateDelta()
        {
            if (Keys.Count >= 2)
            {
                float startValue = GetValue(0);
                float endValue = GetValue(FrameCount);
                return endValue - startValue;
            }
            else
                return 0;
        }

        public bool HasAnimation()
        {
            return Keys.Count > 0;
        }

        public List<Animation.KeyFrame> Keys = new List<Animation.KeyFrame>();
        public float FrameCount
        {
            get
            {
                float fc = 0;
                foreach (Animation.KeyFrame k in Keys)
                    if (k.Frame > fc) fc = k.Frame;
                return fc;
            }
        }

        public Animation.KeyFrame GetKeyFrame(float frame, bool InsertNewKey = true)
        {
            if (Keys.Count == 0) return null;
            Animation.KeyFrame k1 = (Animation.KeyFrame)Keys[0], k2 = (Animation.KeyFrame)Keys[0];
            foreach (Animation.KeyFrame k in Keys)
            {
                if (k.Frame < frame)
                {
                    k1 = k;
                }
                else
                {
                    k2 = k;
                    break;
                }
            }

            return k1;
        }

        public bool SetValue(float Value, int frame)
        {
            for (int i = 0; i < Keys.Count; i++)
            {
                Animation.KeyFrame key = Keys[i];
                if (key.Frame == frame)
                {
                    key.Value = Value;
                    return true;
                }
            }
            return false;
        }


        int LastFound = 0;
        float LastFrame;
        public float GetValue(float frame)
        {
            if (Keys.Count == 0)
                return 0;

            float startFrame = Keys.First().Frame;

            Animation.KeyFrame LK = Keys.First();
            Animation.KeyFrame RK = Keys.Last();

            foreach (Animation.KeyFrame keyFrame in Keys)
            {
                if (keyFrame.Frame <= frame) LK = keyFrame;
                if (keyFrame.Frame >= frame && keyFrame.Frame < RK.Frame) RK = keyFrame;
            }

            if (LK.Frame != RK.Frame)
            {
                //  float FrameDiff = frame - LK.Frame;
                //  float Weight = 1.0f / (RK.Frame - LK.Frame);
                //  float Weight = FrameDiff / (RK.Frame - LK.Frame);

                float FrameDiff = frame - LK.Frame;
                float Weight = FrameDiff / (RK.Frame - LK.Frame);

                Console.WriteLine($"frame diff {FrameDiff} frame {frame} LK {LK.Frame} RK {RK} ratio {Weight}");

                switch (InterpolationType)
                {
                    case InterpolationType.CONSTANT: return LK.Value;
                    case InterpolationType.STEP: return LK.Value;
                    case InterpolationType.LINEAR: return InterpolationHelper.Lerp(LK.Value, RK.Value, Weight);
                    case InterpolationType.HERMITE:
                        float val = Hermite(frame, LK.Frame, RK.Frame, LK.In, LK.Out != -1 ? LK.Out : RK.In, LK.Value, RK.Value);
                        return val;
                }
            }
            return LK.Value;
        }

        public static float Hermite(float frame, float frame1, float frame2, float outSlope, float inSlope, float val1, float val2)
        {
            if (frame == frame1) return val1;
            if (frame == frame2) return val2;

            float distance = frame - frame1;
            float invDuration = 1f / (frame2 - frame1);
            float t = distance * invDuration;
            float t1 = t - 1f;
            return (val1 + ((((val1 - val2) * ((2f * t) - 3f)) * t) * t)) + ((distance * t1) * ((t1 * outSlope) + (t * inSlope)));
        }

        public Animation.KeyFrame[] GetFrame(float frame)
        {
            if (Keys.Count == 0) return null;
            Animation.KeyFrame k1 = (Animation.KeyFrame)Keys[0], k2 = (Animation.KeyFrame)Keys[0];
            foreach (Animation.KeyFrame k in Keys)
            {
                if (k.Frame < frame)
                {
                    k1 = k;
                }
                else
                {
                    k2 = k;
                    break;
                }
            }

            return new Animation.KeyFrame[] { k1, k2 };
        }
    }

    //public class Animation.KeyFrame
    //{
    //    public bool IsKeyed = false;

    //    public float Value1, Delta;

    //    public float Slope1;
    //    public float Slope2;

    //    public float Value
    //    {
    //        get { if (Degrees) return _value * 180 / (float)Math.PI; else return _value; }
    //        set { _value = value; }
    //    }
    //    public float _value;
    //    public float Frame
    //    {
    //        get { return _frame; }
    //        set { _frame = value; }
    //    }
    //    public string Text;
    //    public float _frame;

    //    public float In = 0;
    //    public float Out = -1;

    //    //     public float In = 0, Out = -1;
    //    public bool Weighted = false;
    //    public bool Degrees = false; // Use Degrees
    //    public InterpolationType InterType = InterpolationType.LINEAR;

    //    public Animation.KeyFrame(float value, float frame)
    //    {
    //        Value = value;
    //        Frame = frame;
    //    }

    //    public Animation.KeyFrame()
    //    {

    //    }

    //    public override string ToString()
    //    {
    //        return Frame + " " + Value;
    //    }
    //}
}
