using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using Syroot.NintenTools.Bfres;
using ResU = Syroot.NintenTools.Bfres;
using Syroot.Maths;
using OpenTK;

namespace BFRES_Importer
{
    /// <summary>
    /// Class used for getting BFRES FSKL values and writing them into XML
    /// </summary>
    public class FSKL
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="skeleton"></param>
        public static void WriteSkeleton(XmlWriter writer, Skeleton skeleton)
        {
            writer.WriteStartElement("FSKL");
            if (skeleton.MatrixToBoneList == null)
                skeleton.MatrixToBoneList = new List<ushort>();

            writer.WriteAttributeString("SkeletonBoneCount", skeleton.MatrixToBoneList.Count.ToString());
            writer.WriteAttributeString("FlagsRotation", skeleton.FlagsRotation.ToString());
            writer.WriteAttributeString("FlagsScaling", skeleton.FlagsScaling.ToString());
            // TODO figure out what the hell this Inverse Model Matrices is
            writer.WriteAttributeString("InverseModelMatrices", skeleton.InverseModelMatrices.ToString());

            int nodes = 0;
            string tempBoneList = "";
            foreach (ushort node in skeleton.MatrixToBoneList)
            {
                tempBoneList += (node + ",");
                writer.Flush();
                nodes++;
            }
            tempBoneList = tempBoneList.Trim(',');
            writer.WriteAttributeString("BoneList", tempBoneList);

            int boneIndex = 0;
            foreach (Bone bone in skeleton.Bones.Values)
            {
                writer.WriteStartElement("Bone");
                writer.WriteAttributeString("Index", boneIndex.ToString());
                WriteBone(writer, bone);
                writer.WriteEndElement();
                boneIndex++;
            }
            writer.WriteEndElement();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="bn"></param>
        /// <param name="SetParent"></param>
        public static void WriteBone(XmlWriter writer, Bone bn, bool SetParent = true)
        {
            writer.WriteAttributeString("Name", bn.Name);
            writer.WriteAttributeString("IsVisible", bn.Flags.HasFlag(BoneFlags.Visible).ToString());
            writer.WriteAttributeString("RigidMatrixIndex", bn.RigidMatrixIndex.ToString());
            writer.WriteAttributeString("SmoothMatrixIndex", bn.SmoothMatrixIndex.ToString());
            writer.WriteAttributeString("BillboardIndex", bn.BillboardIndex.ToString());
            writer.WriteAttributeString("FlagsRotation", bn.FlagsRotation.ToString());
            writer.WriteAttributeString("FlagsBillboard", bn.FlagsBillboard.ToString());
            writer.WriteAttributeString("FlagsTransform", bn.FlagsTransform.ToString());
            writer.WriteAttributeString("FlagsTransformCumulative", bn.FlagsTransformCumulative.ToString());

            bool bUseRigidMatrix = bn.RigidMatrixIndex != -1;
            writer.WriteAttributeString("UseRigidMatrix", bUseRigidMatrix.ToString());
            bool bUseSmoothMatrix = bn.SmoothMatrixIndex != -1;
            writer.WriteAttributeString("UseSmoothMatrix", bUseSmoothMatrix.ToString());

            short signedParentIndex = (short)bn.ParentIndex;
            if (SetParent)
                writer.WriteAttributeString("ParentIndex", signedParentIndex.ToString());
            if (bn.FlagsRotation == BoneFlagsRotation.Quaternion)
                writer.WriteAttributeString("RotationType", "Quaternion");
            else if (bn.FlagsRotation == BoneFlagsRotation.EulerXYZ)
                writer.WriteAttributeString("RotationType", "EulerXYZ");

            writer.WriteAttributeString("Scale", Program.Vector3FToString(bn.Scale));
            writer.WriteAttributeString("Rotation", Program.Vector4FToString(bn.Rotation));
            writer.WriteAttributeString("Position", Program.Vector3FToString(bn.Position));
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="model"></param>
        /// <param name="boneIndex"></param>
        /// <returns></returns>
    }

    /// <summary>
    /// Custom class to store read values into and apply processing
    /// </summary>
    public class JPSkeleton
    {
        public IList<ushort>                 boneList;
        public IList<ushort>                 rigidIndices;
        public IList<ushort>                 smoothIndices;
        public IList<Syroot.Maths.Matrix3x4> inverseModelMatrices;
        public List<JPBone>                  bones;
        public SkeletonFlagsRotation         flagsRotation;
        public SkeletonFlagsScaling          flagsScaling;
        public JPSkeleton()
        {
            rigidIndices         = new List<ushort>();
            smoothIndices        = new List<ushort>();
            boneList             = new List<ushort>();
            inverseModelMatrices = new List<Syroot.Maths.Matrix3x4>();
            bones                = new List<JPBone>();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="skeleton"></param>
        /// <returns></returns>
        public JPSkeleton ReadSkeleton(Skeleton skeleton)
        {
            rigidIndices         = skeleton.GetRigidIndices();
            smoothIndices        = skeleton.GetSmoothIndices();
            boneList             = skeleton.MatrixToBoneList;
            inverseModelMatrices = skeleton.InverseModelMatrices;
            flagsRotation        = skeleton.FlagsRotation;
            flagsScaling         = skeleton.FlagsScaling;

            this.bones.Clear();
            short count = 0;
            foreach (Bone bone in skeleton.Bones.Values)
            {
                JPBone jpBone = new JPBone(this, count++);
                jpBone = jpBone.ReadBone(bone);
                this.bones.Add(jpBone);
            }

            this.Reset();
            return this;
        }
        public static Quaternion FromQuaternionAngles(float z, float y, float x, float w)
        {
            {
                Quaternion q = new Quaternion();
                q.X = x;
                q.Y = y;
                q.Z = z;
                q.W = w;

                if (q.W < 0)
                    q *= -1;

                //return xRotation * yRotation * zRotation;
                return q;
            }
        }

        public static Quaternion FromEulerAngles(float z, float y, float x)
        {
            {
                Quaternion xRotation = Quaternion.FromAxisAngle(OpenTK.Vector3.UnitX, x);
                Quaternion yRotation = Quaternion.FromAxisAngle(OpenTK.Vector3.UnitY, y);
                Quaternion zRotation = Quaternion.FromAxisAngle(OpenTK.Vector3.UnitZ, z);

                Quaternion q = (zRotation * yRotation * xRotation);

                if (q.W < 0)
                    q *= -1;

                //return xRotation * yRotation * zRotation;
                return q;
            }
        }
        public void Reset(bool Main = true)
        {
            for (int i = 0; i < bones.Count; i++)
            {
                bones[i].pos = new OpenTK.Vector3(bones[i].position[0], bones[i].position[1], bones[i].position[2]);

                if (bones[i].rotationType == JPBone.BoneRotationType.Quaternion)
                {
                    bones[i].rot = (FromQuaternionAngles(bones[i].rotation[2], bones[i].rotation[1], bones[i].rotation[0], bones[i].rotation[3]));
                }
                else
                {
                    bones[i].rot = (FromEulerAngles(bones[i].rotation[2], bones[i].rotation[1], bones[i].rotation[0]));
                }
                bones[i].sca = new OpenTK.Vector3(bones[i].scale[0], bones[i].scale[1], bones[i].scale[2]);
            }
            Update(true);
            for (int i = 0; i < bones.Count; i++)
            {
                try
                {
                    bones[i].invert = OpenTK.Matrix4.Invert(bones[i].Transform);
                }
                catch (InvalidOperationException)
                {
                    bones[i].invert = OpenTK.Matrix4.Zero;
                }
            }
            Update();
        }


        private bool updated = false;
        public void Update(bool reset = false)
        {
            updated = true;
            List<JPBone> bonesToProcess = new List<JPBone>();

            // Add root bone
            foreach (JPBone bone in bones)
            {
                if (bone.parentIndex == -1)
                {
                    bonesToProcess.Add(bone);
                }
            }

            // some special processing for the root bones before we start
            foreach (JPBone rootBone in bonesToProcess)
            {
                rootBone.Transform = OpenTK.Matrix4.CreateScale(rootBone.sca) * OpenTK.Matrix4.CreateFromQuaternion(rootBone.rot) * OpenTK.Matrix4.CreateTranslation(rootBone.pos);

                // scale down the model in its entirety only when mid-animation (i.e. reset == false)
                // Maybe not relevant?
                if (!reset) rootBone.Transform *= OpenTK.Matrix4.CreateScale(1);
            }

            int numRootNodes = bonesToProcess.Count;
            for (int i = 0; i < numRootNodes; i++)
            {
                bonesToProcess.AddRange(bonesToProcess[0].GetChildren());
                bonesToProcess.RemoveAt(0);
            }
            while (bonesToProcess.Count > 0)
            {
                // BFS
                JPBone bone = bonesToProcess[0];
                bonesToProcess.RemoveAt(0);
                bonesToProcess.AddRange(bone.GetChildren());

                bone.Transform = OpenTK.Matrix4.CreateScale(bone.sca) * OpenTK.Matrix4.CreateFromQuaternion(bone.rot) * OpenTK.Matrix4.CreateTranslation(bone.pos);
                if (bone.parentIndex != -1)
                {
                    if (bone.UseSegmentScaleCompensate && bones[bone.parentIndex] != null
                        && bones[bone.parentIndex] is JPBone)
                    {
                        bone.Transform *= OpenTK.Matrix4.CreateScale(
                              1f / bones[bone.parentIndex].sca.X,
                              1f / bones[bone.parentIndex].sca.Y,
                              1f / bones[bone.parentIndex].sca.Z);

                        bone.Transform *= bones[bone.parentIndex].Transform;
                    }
                    else
                    {
                        bone.Transform = bone.Transform * bones[bone.parentIndex].Transform;
                    }
                }
            }

        }


        public static OpenTK.Matrix4 CalculateBoneCumulativeTransform(ref ResU.Model model, int boneIndex)
        {
            // Create the bone's transform
            OpenTK.Vector3 mPos = new OpenTK.Vector3(
                model.Skeleton.Bones[boneIndex].Position.X,
                model.Skeleton.Bones[boneIndex].Position.Y,
                model.Skeleton.Bones[boneIndex].Position.Z);

            OpenTK.Quaternion mRot;
            OpenTK.Vector3 v3 = new OpenTK.Vector3(
                    model.Skeleton.Bones[boneIndex].Rotation.X,
                    model.Skeleton.Bones[boneIndex].Rotation.Y,
                    model.Skeleton.Bones[boneIndex].Rotation.Z
                    );
            if (model.Skeleton.Bones[boneIndex].FlagsRotation == BoneFlagsRotation.Quaternion)
                mRot = new OpenTK.Quaternion(v3, model.Skeleton.Bones[boneIndex].Rotation.W);
            else if (model.Skeleton.Bones[boneIndex].FlagsRotation == BoneFlagsRotation.EulerXYZ)
                mRot = new OpenTK.Quaternion(v3);
            else
                mRot = new OpenTK.Quaternion();

            OpenTK.Vector3 mSca = new OpenTK.Vector3(
                model.Skeleton.Bones[boneIndex].Scale.X,
                model.Skeleton.Bones[boneIndex].Scale.Y,
                model.Skeleton.Bones[boneIndex].Scale.Z);

            while (model.Skeleton.Bones[boneIndex].ParentIndex < model.Skeleton.Bones.Count)
            {
                boneIndex = model.Skeleton.Bones[boneIndex].ParentIndex;

                // Create the parent Transform Matrix4
                OpenTK.Vector3 parentPos = new OpenTK.Vector3(
                    model.Skeleton.Bones[boneIndex].Position.X,
                    model.Skeleton.Bones[boneIndex].Position.Y,
                    model.Skeleton.Bones[boneIndex].Position.Z);

                OpenTK.Quaternion parentRot;
                OpenTK.Vector3 parentV3 = new OpenTK.Vector3(
                        model.Skeleton.Bones[boneIndex].Rotation.X,
                        model.Skeleton.Bones[boneIndex].Rotation.Y,
                        model.Skeleton.Bones[boneIndex].Rotation.Z
                        );
                if (model.Skeleton.Bones[boneIndex].FlagsRotation == BoneFlagsRotation.Quaternion)
                    parentRot = new OpenTK.Quaternion(v3, model.Skeleton.Bones[boneIndex].Rotation.W);
                else if (model.Skeleton.Bones[boneIndex].FlagsRotation == BoneFlagsRotation.EulerXYZ)
                    parentRot = new OpenTK.Quaternion(v3);
                else
                    parentRot = new OpenTK.Quaternion();

                OpenTK.Vector3 parentSca = new OpenTK.Vector3(
                    model.Skeleton.Bones[boneIndex].Scale.X,
                    model.Skeleton.Bones[boneIndex].Scale.Y,
                    model.Skeleton.Bones[boneIndex].Scale.Z);

                OpenTK.Matrix4 parentTransform = OpenTK.Matrix4.CreateScale(parentSca) * OpenTK.Matrix4.CreateFromQuaternion(parentRot) * OpenTK.Matrix4.CreateTranslation(parentPos);

                // Add transforms
                mSca = OpenTK.Vector3.Multiply(mSca, parentSca);
                mPos = OpenTK.Vector3.Add(mPos, parentPos);
                mRot = OpenTK.Quaternion.Add(mRot, parentRot);
            }

            OpenTK.Matrix4 boneTransform = OpenTK.Matrix4.CreateScale(mSca) * OpenTK.Matrix4.CreateFromQuaternion(mRot) * OpenTK.Matrix4.CreateTranslation(mPos);
            return boneTransform;
        }
    }

    /// <summary>
    /// Custom class to store read bone valuse into and apply processing
    /// </summary>
    public class JPBone
    {
        public short  index;
        public Bone   boneU;
        public bool   flagVisible;
        public string name;
        public ushort billboardIndex;
        public short  rigidMatrixIndex;
        public short  smoothMatrixIndex;
        public bool   useRigidMatrix;
        public bool   useSmoothMatrix;
        public short  parentIndex;
        public bool   UseSegmentScaleCompensate = true; // TODO read into this shit.

        public float[] scale;
        public float[] rotation;
        public float[] position;

        public OpenTK.Vector3 pos = OpenTK.Vector3.Zero;
        public OpenTK.Vector3 sca = new OpenTK.Vector3(1f, 1f, 1f);
        public OpenTK.Quaternion rot = OpenTK.Quaternion.FromMatrix(OpenTK.Matrix3.Zero);
        public OpenTK.Matrix4 Transform, invert;

        public BoneRotationType rotationType;

        public JPSkeleton skeletonParent;

        public enum BoneRotationType
        {
            Euler,
            Quaternion,
        }
        public JPBone(JPSkeleton jpSkeleton, short boneIndex)
        {
            skeletonParent = jpSkeleton;
            index = boneIndex;
        }

        public JPBone()
        {

        }

        public JPBone ReadBone(Bone bn, bool SetParent = true)
        {
            if (boneU == null)
                boneU = new Bone();
            boneU = bn;
            flagVisible = bn.Flags.HasFlag(BoneFlags.Visible);
            name = bn.Name;
            rigidMatrixIndex = bn.RigidMatrixIndex;
            smoothMatrixIndex = bn.SmoothMatrixIndex;
            billboardIndex = bn.BillboardIndex;
            useRigidMatrix = bn.RigidMatrixIndex != -1;
            useSmoothMatrix = bn.SmoothMatrixIndex != -1;

            if (SetParent)
                parentIndex = (short)bn.ParentIndex;
            scale = new float[3];
            rotation = new float[4];
            position = new float[3];
            if (bn.FlagsRotation == BoneFlagsRotation.Quaternion)
                rotationType = BoneRotationType.Quaternion;
            else
                rotationType = BoneRotationType.Euler;
            scale[0] = bn.Scale.X;
            scale[1] = bn.Scale.Y;
            scale[2] = bn.Scale.Z;
            rotation[0] = bn.Rotation.X;
            rotation[1] = bn.Rotation.Y;
            rotation[2] = bn.Rotation.Z;
            rotation[3] = bn.Rotation.W;
            position[0] = bn.Position.X;
            position[1] = bn.Position.Y;
            position[2] = bn.Position.Z;
            return this;
        }

        public List<JPBone> GetChildren()
        {
            List<JPBone> list = new List<JPBone>();
            foreach (JPBone bone in skeletonParent.bones)
                if (bone.parentIndex == this.index)
                    list.Add(bone);
            return list;
        }
    }
}
