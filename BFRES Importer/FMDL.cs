using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using ResU = Syroot.NintenTools.Bfres;

namespace BFRES_Importer
{
    public class FMDL
    {
        public void DumpFMDLData(ResU.Model model, StreamWriter writer)
        {
            writer.WriteLine();
            writer.WriteLine("[[FMDL]]");
            writer.WriteLine("Name: " + model.Name);
            writer.WriteLine("FVTX Count: " + model.VertexBuffers.Count);
            writer.WriteLine("FSHP Count: " + model.Shapes.Count);
            writer.WriteLine("FMAT Count: " + model.Materials.Count);
            writer.WriteLine("Total Vertices: " + model.TotalVertexCount);

            ReadSkeleton(writer, model.Skeleton);

            //if (model == null)
            //    model = new FMDL();

            //model.Text = mdl.Name;
            //model.Skeleton = new FSKL(mdl.Skeleton);

            //model.Nodes.RemoveAt(2);
            //model.Nodes.Add(model.Skeleton.node);

            //model.ModelU = mdl;


            foreach (Material mat in model.Materials.Values)
            {
                // Read Materials Function
            }
            foreach (Shape shp in model.Shapes.Values)
            {
                writer.WriteLine("Shape name: " + shp.Name);
                VertexBuffer vertexBuffer = model.VertexBuffers[shp.VertexBufferIndex];
                Material material = model.Materials[shp.MaterialIndex];
                ReadShapesVertices(writer, shp, vertexBuffer, model);
            }

            writer.Flush();
        }

        public static void ReadSkeleton(StreamWriter writer, Skeleton skeleton)
        {
            if (skeleton.MatrixToBoneList == null)
                skeleton.MatrixToBoneList = new List<ushort>();

            //RenderableSkeleton.Node_Array = new int[skeleton.MatrixToBoneList.Count];
            writer.WriteLine("Skeleton bone count: " + skeleton.MatrixToBoneList.Count);

            int nodes = 0;
            writer.Write("Skeleton bone list: {");
            foreach (ushort node in skeleton.MatrixToBoneList)
            {
                // RenderableSkeleton.Node_Array[nodes] = node;
                writer.Write(node + ", ");
                writer.Flush();
                nodes++;
            }
            writer.Write("}\n");

            //RenderableSkeleton.bones.Clear();
            int boneIndex = 0;
            foreach (Bone bone in skeleton.Bones.Values)
            {
                //BfresBone STBone = new BfresBone(RenderableSkeleton);
                //ReadBone(STBone, bone);
                writer.WriteLine("\nBone Index: " + boneIndex);
                WriteBones(writer, bone);
                boneIndex++;
                //RenderableSkeleton.bones.Add(STBone);
            }

            //skl.Nodes.Clear();
            //foreach (var bone in RenderableSkeleton.bones)
            //{
            //    if (bone.Parent == null)
            //    {
            //        skl.Nodes.Add(bone);
            //    }
            //}

            //RenderableSkeleton.update();
            //RenderableSkeleton.reset();
        }

        public static void WriteBones(StreamWriter writer, Bone bn, bool SetParent = true)
        {
            writer.WriteLine("Bone name: " + bn.Name);
            writer.WriteLine("IsVisible: " + bn.Flags.HasFlag(BoneFlags.Visible));
            writer.WriteLine("Rigid Matrix Index: " + bn.RigidMatrixIndex);
            writer.WriteLine("Smooth Matrix Index: " + bn.SmoothMatrixIndex);
            writer.WriteLine("Billboard Index: " + bn.BillboardIndex);

            bool bUseRigidMatrix = bn.RigidMatrixIndex != -1;
            writer.WriteLine("Use Rigid Matrix: " + bUseRigidMatrix);
            bool bUseSmoothMatrix = bn.SmoothMatrixIndex != -1;
            writer.WriteLine("Use Smooth Matrix: " + bUseSmoothMatrix);

            if (SetParent)
                writer.WriteLine("Parent Index: " + bn.ParentIndex);
            if (bn.FlagsRotation == BoneFlagsRotation.Quaternion)
                writer.WriteLine("Rotation Type: Quaternion");
            else if (bn.FlagsRotation == BoneFlagsRotation.EulerXYZ)
                writer.WriteLine("Rotation Type: EulerXYZ");

            writer.Write("Scale: "
                + bn.Scale.X + ", "
                + bn.Scale.Y + ", "
                + bn.Scale.Z + "\n"
                );

            writer.Write("Rotation: "
                + bn.Rotation.X + ", "
                + bn.Rotation.Y + ", "
                + bn.Rotation.Z + ", "
                + bn.Rotation.W + "\n"
                );

            writer.Write("Position: "
                + bn.Position.X + ", "
                + bn.Position.Y + ", "
                + bn.Position.Z + "\n"
                );
        }

        public static void ReadShapesVertices(StreamWriter writer, Shape shp, VertexBuffer vertexBuffer, ResU.Model model)
        {
            // TODO: Clear Bounding Boxes
            // TODO: Clear Bounding Radius
            // TODO: Clear Bone Indices

            foreach (Bounding bnd in shp.SubMeshBoundings)
            {
                writer.WriteLine("Bounding Center: (" + bnd.Center.X + ", " + bnd.Center.Y + ", " + bnd.Center.Z + ")");
                writer.WriteLine("Bounding Extent: (" + bnd.Extent.X + ", " + bnd.Extent.Y + ", " + bnd.Extent.Z + ")");
            }

            foreach (float rad in shp.RadiusArray)
            {
                // ???
            }

            writer.WriteLine("Shape Vertex Buffer Index: " + shp.VertexBufferIndex);
            writer.WriteLine("Shape Vertex Skin Count: " + shp.VertexSkinCount);
            writer.WriteLine("Shape Bone Index: " + shp.BoneIndex);
            writer.WriteLine("Shape Target Attribute Count: " + shp.TargetAttribCount);
            writer.WriteLine("Shape Material Index: " + shp.MaterialIndex);

            if (shp.SkinBoneIndices != null)
            {
                writer.Write("Shape Skin Bone Indices: {");
                foreach (ushort bn in shp.SkinBoneIndices)
                {
                    writer.Write(bn + ", ");
                }
                writer.Write("}\n");
            }

            //ReadMeshes()
            ReadVertexBuffer(writer, shp, vertexBuffer, model);
        }

        private static void ReadVertexBuffer(StreamWriter writer, Shape shp, VertexBuffer vtx, ResU.Model model)
        {
            // TODO: Clear vertices
            // TODO: Clear vertex attributes

            //Create a buffer instance which stores all the buffer data
            VertexBufferHelper helper = new VertexBufferHelper(vtx, Syroot.BinaryData.ByteOrder.BigEndian);

            //Set each array first from the lib if exist. Then add the data all in one loop
            Syroot.Maths.Vector4F[] vec4Positions = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4Normals = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv0 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv1 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv2 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4c0 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4t0 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4b0 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4w0 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4i0 = new Syroot.Maths.Vector4F[0];

            //For shape morphing
            Syroot.Maths.Vector4F[] vec4Positions1 = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4Positions2 = new Syroot.Maths.Vector4F[0];

            List<VertexAttrib> SortedList = vtx.Attributes.Values.OrderBy(o => o.BufferIndex).ToList();

            foreach (VertexAttrib att in vtx.Attributes.Values)
            {
                if (att.Name == "_p0")
                    vec4Positions = AttributeData(att, helper, "_p0");
                if (att.Name == "_n0")
                    vec4Normals = AttributeData(att, helper, "_n0");
                if (att.Name == "_u0")
                    vec4uv0 = AttributeData(att, helper, "_u0");
                if (att.Name == "_u1")
                    vec4uv1 = AttributeData(att, helper, "_u1");
                if (att.Name == "_u2")
                    vec4uv2 = AttributeData(att, helper, "_u2");
                if (att.Name == "_c0")
                    vec4c0 = AttributeData(att, helper, "_c0");
                if (att.Name == "_t0")
                    vec4t0 = AttributeData(att, helper, "_t0");
                if (att.Name == "_b0")
                    vec4b0 = AttributeData(att, helper, "_b0");
                if (att.Name == "_w0")
                    vec4w0 = AttributeData(att, helper, "_w0");
                if (att.Name == "_i0")
                    vec4i0 = AttributeData(att, helper, "_i0");

                if (att.Name == "_p1")
                    vec4Positions1 = AttributeData(att, helper, "_p1");
                if (att.Name == "_p2")
                    vec4Positions2 = AttributeData(att, helper, "_p2");
            }
            List<Vertex> vertices = new List<Vertex>();
            for (int i = 0; i < vec4Positions.Length; i++)
            {
                Vertex v = new Vertex();
                if (vec4Positions.Length > 0)
                    v.pos = new OpenTK.Vector3(vec4Positions[i].X, vec4Positions[i].Y, vec4Positions[i].Z);
                if (vec4Positions1.Length > 0)
                    v.pos1 = new OpenTK.Vector3(vec4Positions1[i].X, vec4Positions1[i].Y, vec4Positions1[i].Z);
                if (vec4Positions2.Length > 0)
                    v.pos2 = new OpenTK.Vector3(vec4Positions2[i].X, vec4Positions2[i].Y, vec4Positions2[i].Z);
                if (vec4Normals.Length > 0)
                    v.nrm = new OpenTK.Vector3(vec4Normals[i].X, vec4Normals[i].Y, vec4Normals[i].Z);
                if (vec4uv0.Length > 0)
                    v.uv0 = new OpenTK.Vector2(vec4uv0[i].X, vec4uv0[i].Y);
                if (vec4uv1.Length > 0)
                    v.uv1 = new OpenTK.Vector2(vec4uv1[i].X, vec4uv1[i].Y);
                if (vec4uv2.Length > 0)
                    v.uv2 = new OpenTK.Vector2(vec4uv2[i].X, vec4uv2[i].Y);
                if (vec4w0.Length > 0)
                {
                    v.boneWeights.Add(vec4w0[i].X);
                    v.boneWeights.Add(vec4w0[i].Y);
                    v.boneWeights.Add(vec4w0[i].Z);
                    v.boneWeights.Add(vec4w0[i].W);
                }
                if (vec4i0.Length > 0)
                {
                    v.boneIds.Add((int)vec4i0[i].X);
                    v.boneIds.Add((int)vec4i0[i].Y);
                    v.boneIds.Add((int)vec4i0[i].Z);
                    v.boneIds.Add((int)vec4i0[i].W);
                }

                if (vec4t0.Length > 0)
                    v.tan = new OpenTK.Vector4(vec4t0[i].X, vec4t0[i].Y, vec4t0[i].Z, vec4t0[i].W);
                if (vec4b0.Length > 0)
                    v.bitan = new OpenTK.Vector4(vec4b0[i].X, vec4b0[i].Y, vec4b0[i].Z, vec4b0[i].W);
                if (vec4c0.Length > 0)
                    v.col = new OpenTK.Vector4(vec4c0[i].X, vec4c0[i].Y, vec4c0[i].Z, vec4c0[i].W);


                int[] nodeArray = new int[model.Skeleton.MatrixToBoneList.Count];
                int nodes = 0;
                foreach (ushort node in model.Skeleton.MatrixToBoneList)
                {
                    nodeArray[nodes] = node;
                    nodes++;
                }
                if (shp.VertexSkinCount == 1)
                {
                    int boneIndex = shp.BoneIndex;
                    if (v.boneIds.Count > 0)
                        boneIndex = nodeArray[v.boneIds[0]];

                    //Check if the bones are a rigid type
                    //In game it seems to not transform if they are not rigid
                    if (model.Skeleton.Bones[boneIndex].RigidMatrixIndex != -1)
                    {
                        // Create the Transform Matrix4
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
                        {
                            mRot = new OpenTK.Quaternion(v3);
                        }
                        else
                        {
                            mRot = new OpenTK.Quaternion();
                        }
                        OpenTK.Vector3 mSca = new OpenTK.Vector3(
                            model.Skeleton.Bones[boneIndex].Scale.X,
                            model.Skeleton.Bones[boneIndex].Scale.Y,
                            model.Skeleton.Bones[boneIndex].Scale.Z);

                        OpenTK.Matrix4 sb = OpenTK.Matrix4.CreateScale(mSca) * OpenTK.Matrix4.CreateFromQuaternion(mRot) * OpenTK.Matrix4.CreateTranslation(mPos);
                        v.pos = OpenTK.Vector3.TransformPosition(v.pos, sb);
                        v.nrm = OpenTK.Vector3.TransformNormal(v.nrm, sb);
                    }
                }
                if (shp.VertexSkinCount == 0)
                {
                    try
                    {
                        if (model.Skeleton.Bones.Count > 0)
                        {
                            int boneIndex = shp.BoneIndex;


                            // Create the Transform Matrix4
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
                            {
                                mRot = new OpenTK.Quaternion(v3);
                            }
                            else
                            {
                                mRot = new OpenTK.Quaternion();
                            }
                            OpenTK.Vector3 mSca = new OpenTK.Vector3(
                                model.Skeleton.Bones[boneIndex].Scale.X,
                                model.Skeleton.Bones[boneIndex].Scale.Y,
                                model.Skeleton.Bones[boneIndex].Scale.Z);

                            OpenTK.Matrix4 NoBindFix = OpenTK.Matrix4.CreateScale(mSca) * OpenTK.Matrix4.CreateFromQuaternion(mRot) * OpenTK.Matrix4.CreateTranslation(mPos);
                            v.pos = OpenTK.Vector3.TransformPosition(v.pos, NoBindFix);
                            v.nrm = OpenTK.Vector3.TransformNormal(v.nrm, NoBindFix);
                        }
                    }
                    catch //Matrix failed. Print the coordinate data of the bone
                    {
                        //Console.WriteLine(model.Skeleton.bones[fshp.BoneIndex].Text);
                        //Console.WriteLine(model.Skeleton.bones[fshp.BoneIndex].GetPosition());
                        //Console.WriteLine(model.Skeleton.bones[fshp.BoneIndex].GetRotation());
                        //Console.WriteLine(model.Skeleton.bones[fshp.BoneIndex].GetScale());
                    }
                }

                vertices.Add(v);

            }

            for (int i = 0; i < vertices.Count; i++)
            {
                Vertex.PrintVector3(writer, vertices[i].pos);
            }



        }
        static Syroot.Maths.Vector4F[] AttributeData(VertexAttrib att, VertexBufferHelper helper, string attName)
        {
            VertexBufferHelperAttrib attd = helper[attName];
            return attd.Data;
        }
        static OpenTK.Quaternion FromQuaternionAngles(float z, float y, float x, float w)
        {
            {
                OpenTK.Quaternion q = new OpenTK.Quaternion();
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
    }
}