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
    public class FMDL
    {
        public void WriteFMDLData(ResU.Model model, XmlWriter writer)
        {
            writer.WriteStartElement("FMDL");
            writer.WriteAttributeString("Name"         , model.Name                          );

            // TODO Wrap these functions in if exists conditions, so that the xml only contains relevant data
            ReadSkeleton(writer, model.Skeleton);

            writer.WriteStartElement("Materials");
            writer.WriteAttributeString("FMATCount", model.Materials.Count.ToString());
            foreach (Material mat in model.Materials.Values)
            {
                FMAT.ReadMaterial(writer, mat);
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Shapes");
            writer.WriteAttributeString("FSHPCount", model.Shapes.Count.ToString());
            writer.WriteAttributeString("FVTXCount", model.VertexBuffers.Count.ToString());
            writer.WriteAttributeString("TotalVertices", model.TotalVertexCount.ToString());
            foreach (Shape shp in model.Shapes.Values)
            {
                writer.WriteStartElement("FSHP");
                VertexBuffer vertexBuffer = model.VertexBuffers[shp.VertexBufferIndex];
                Material material = model.Materials[shp.MaterialIndex];
                ReadShapesVertices(writer, shp, vertexBuffer, model);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteEndElement();
            writer.Flush();
        }

        public static void ReadSkeleton(XmlWriter writer, Skeleton skeleton)
        {
            writer.WriteStartElement("FSKL");
            if (skeleton.MatrixToBoneList == null)
                skeleton.MatrixToBoneList = new List<ushort>();

            writer.WriteAttributeString("SkeletonBoneCount", skeleton.MatrixToBoneList.Count.ToString());

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
                WriteBones(writer, bone);
                writer.WriteEndElement();
                boneIndex++;
            }
            writer.WriteEndElement();
        }

        public static void WriteBones(XmlWriter writer, Bone bn, bool SetParent = true)
        {
            writer.WriteAttributeString("Name"             , bn.Name                                       );
            writer.WriteAttributeString("IsVisible"        , bn.Flags.HasFlag(BoneFlags.Visible).ToString());
            writer.WriteAttributeString("RigidMatrixIndex" , bn.RigidMatrixIndex.ToString()                );
            writer.WriteAttributeString("SmoothMatrixIndex", bn.SmoothMatrixIndex.ToString()               );
            writer.WriteAttributeString("BillboardIndex"   , bn.BillboardIndex.ToString()                  );

            bool bUseRigidMatrix = bn.RigidMatrixIndex != -1;
            writer.WriteAttributeString("UseRigidMatrix"   , bUseRigidMatrix.ToString());
            bool bUseSmoothMatrix = bn.SmoothMatrixIndex != -1;
            writer.WriteAttributeString("UseSmoothMatrix"  , bUseSmoothMatrix.ToString());

            if (SetParent)
                writer.WriteAttributeString("ParentIndex"  , bn.ParentIndex.ToString());
            if (bn.FlagsRotation == BoneFlagsRotation.Quaternion)
                writer.WriteAttributeString("RotationType" , "Quaternion");
            else if (bn.FlagsRotation == BoneFlagsRotation.EulerXYZ)
                writer.WriteAttributeString("RotationType" , "EulerXYZ");

            writer.WriteAttributeString("Scale"   , Program.Vector3FToString(bn.Scale)   );
            writer.WriteAttributeString("Rotation", Program.Vector4FToString(bn.Rotation));
            writer.WriteAttributeString("Position", Program.Vector3FToString(bn.Position));
        }

        public static void ReadShapesVertices(XmlWriter writer, Shape shp, VertexBuffer vertexBuffer, ResU.Model model)
        {
            writer.WriteAttributeString("Name"                , shp.Name                        );
            writer.WriteAttributeString("VertexBufferIndex"   , shp.VertexBufferIndex.ToString());
            writer.WriteAttributeString("VertexSkinCount"     , shp.VertexSkinCount.ToString()  );
            writer.WriteAttributeString("BoneIndex"           , shp.BoneIndex.ToString()        );
            writer.WriteAttributeString("TargetAttributeCount", shp.TargetAttribCount.ToString());
            writer.WriteAttributeString("MaterialIndex"       , shp.MaterialIndex.ToString()    );

            // Write Bounding Radius
            string tempRadiusArray = "";
            foreach (float rad in shp.RadiusArray)
            {
                tempRadiusArray += (rad.ToString() + ',');
            }
            tempRadiusArray = tempRadiusArray.Trim(',');
            writer.WriteAttributeString("BoundingRadius", tempRadiusArray);

            // Write Skin Bone Indices
            if (shp.SkinBoneIndices != null)
            {
                string tempSkinBoneIndices = "";
                foreach (ushort bn in shp.SkinBoneIndices)
                {
                    tempSkinBoneIndices += (bn + ",");
                }
                tempSkinBoneIndices = tempSkinBoneIndices.Trim(',');
                writer.WriteAttributeString("SkinBoneIndices", tempSkinBoneIndices);
            }


            // Write Bounding Boxes
            //
            // Summary:
            //     Represents a spatial bounding box.
            writer.WriteStartElement("SubMeshBoundings");
            writer.WriteAttributeString("SubMeshBoundingsCount", shp.SubMeshBoundings.Count.ToString());
            foreach (Bounding bnd in shp.SubMeshBoundings)
            {
                writer.WriteStartElement("SubMeshBounding");
                writer.WriteAttributeString("Center", Program.Vector3FToString(bnd.Center));
                writer.WriteAttributeString("Extent", Program.Vector3FToString(bnd.Extent));
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            //
            // Summary:
            //     Represents a node in a Syroot.NintenTools.Bfres.SubMesh bounding tree to determine
            //     when to show which sub mesh of a Syroot.NintenTools.Bfres.Mesh.
            writer.WriteStartElement("SubMeshBoundingNodes");
            writer.WriteAttributeString("SubMeshBoundingNodesCount", shp.SubMeshBoundingNodes.Count.ToString());
            foreach (BoundingNode node in shp.SubMeshBoundingNodes)
            {
                writer.WriteStartElement("SubMeshBoundingNode");
                writer.WriteAttributeString("LeftChildIndex" , node.LeftChildIndex.ToString() );
                writer.WriteAttributeString("NextSibling"    , node.NextSibling.ToString()    );
                writer.WriteAttributeString("RightChildIndex", node.RightChildIndex.ToString());
                writer.WriteAttributeString("Unknown"        , node.Unknown.ToString()        );
                writer.WriteAttributeString("SubMeshIndex"   , node.SubMeshIndex.ToString()   );
                writer.WriteAttributeString("SubMeshCount"   , node.SubMeshCount.ToString()   );
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            // Write SubMesh Bounding Indices
            writer.WriteStartElement("SubMeshBoundingIndices");
            writer.WriteAttributeString("SubMeshBoundingIndicesCount", shp.SubMeshBoundingIndices.Count.ToString());
            if (shp.SubMeshBoundingIndices != null)
            {
                string tempSubMeshBoundingIndices = "";
                foreach (ushort smbi in shp.SubMeshBoundingIndices)
                {
                    tempSubMeshBoundingIndices += (smbi + ",");
                }
                tempSubMeshBoundingIndices = tempSubMeshBoundingIndices.Trim(',');
                writer.WriteAttributeString("SubMeshBoundingIndices", tempSubMeshBoundingIndices);
            }
            writer.WriteEndElement();

            ReadMeshes(writer, shp);
            ReadVertexBuffer(writer, shp, vertexBuffer, model);
        }

        /// <summary>
        /// Works out polygon data in the meshes
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="shp"></param>
        private static void ReadMeshes(XmlWriter writer, Shape shp)
        {
            writer.WriteStartElement("Meshes");
            writer.WriteAttributeString("LODMeshCount", shp.Meshes.Count.ToString());
            foreach (Mesh msh in shp.Meshes)
            {
                uint FaceCount = msh.IndexCount;
                uint[] indicesArray = msh.GetIndices().ToArray();


                writer.WriteStartElement("LODMesh");
                writer.WriteAttributeString("PrimitiveType", msh.PrimitiveType.ToString());
                writer.WriteAttributeString("IndexFormat"  , msh.IndexFormat.ToString()  );
                writer.WriteAttributeString("IndexCount"   , msh.IndexCount.ToString()   );
                writer.WriteAttributeString("FirstVertex"  , msh.FirstVertex.ToString()  );

                List<int> tempFaceList = new List<int>();
                for (int face = 0; face < FaceCount; face++)
                    tempFaceList.Add((int)indicesArray[face] + (int)msh.FirstVertex);
                string tempFaces = "";
                foreach (int fc in tempFaceList)
                    tempFaces += (fc + ",");
                tempFaces = tempFaces.Trim(',');
                writer.WriteAttributeString("FaceVertices", tempFaces);

                foreach (SubMesh subMsh in msh.SubMeshes)
                {
                    writer.WriteStartElement("SubMesh");
                    writer.WriteAttributeString("Count" , subMsh.Count.ToString() );
                    writer.WriteAttributeString("Offset", subMsh.Offset.ToString());
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
        }

        private static void ReadVertexBuffer(XmlWriter writer, Shape shp, VertexBuffer vtx, ResU.Model model)
        {
            //Create a buffer instance which stores all the buffer data
            VertexBufferHelper helper = new VertexBufferHelper(vtx, Syroot.BinaryData.ByteOrder.BigEndian);

            //Set each array first from the lib if exist. Then add the data all in one loop
            Syroot.Maths.Vector4F[] vec4Positions = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4Normals   = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv0       = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv1       = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4uv2       = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4c0        = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4c1        = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4t0        = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4b0        = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4w0        = new Syroot.Maths.Vector4F[0];
            Syroot.Maths.Vector4F[] vec4i0        = new Syroot.Maths.Vector4F[0];

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
                if (att.Name == "_c1")
                    vec4c1 = AttributeData(att, helper, "_c1");
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
                    v.pos  = new OpenTK.Vector3(vec4Positions[i].X,  vec4Positions[i].Y,  vec4Positions[i].Z);
                if (vec4Positions1.Length > 0)
                    v.pos1 = new OpenTK.Vector3(vec4Positions1[i].X, vec4Positions1[i].Y, vec4Positions1[i].Z);
                if (vec4Positions2.Length > 0)
                    v.pos2 = new OpenTK.Vector3(vec4Positions2[i].X, vec4Positions2[i].Y, vec4Positions2[i].Z);
                if (vec4Normals.Length > 0)
                    v.nrm  = new OpenTK.Vector3(vec4Normals[i].X, vec4Normals[i].Y, vec4Normals[i].Z);
                if (vec4uv0.Length > 0)
                    v.uv0  = new OpenTK.Vector2(vec4uv0[i].X, vec4uv0[i].Y);
                if (vec4uv1.Length > 0)
                    v.uv1  = new OpenTK.Vector2(vec4uv1[i].X, vec4uv1[i].Y);
                if (vec4uv2.Length > 0)
                    v.uv2  = new OpenTK.Vector2(vec4uv2[i].X, vec4uv2[i].Y);
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
                    v.tan   = new OpenTK.Vector4(vec4t0[i].X, vec4t0[i].Y, vec4t0[i].Z, vec4t0[i].W);
                if (vec4b0.Length > 0)
                    v.bitan = new OpenTK.Vector4(vec4b0[i].X, vec4b0[i].Y, vec4b0[i].Z, vec4b0[i].W);
                if (vec4c0.Length > 0)
                    v.col   = new OpenTK.Vector4(vec4c0[i].X, vec4c0[i].Y, vec4c0[i].Z, vec4c0[i].W);
                if (vec4c1.Length > 0)
                    v.col2  = new OpenTK.Vector4(vec4c1[i].X, vec4c1[i].Y, vec4c1[i].Z, vec4c1[i].W);


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

            // Write Vertex Data
            writer.WriteStartElement("Vertices");
            writer.WriteAttributeString("VertexCount", vtx.VertexCount.ToString());
            for (int i = 0; i < vertices.Count; i++)
            {
                writer.WriteStartElement("Vertex");

                writer.WriteAttributeString("Index"    , i.ToString()                              );
                writer.WriteAttributeString("Position0", Program.Vector3ToString(vertices[i].pos)  );
                writer.WriteAttributeString("Position1", Program.Vector3ToString(vertices[i].pos1) );
                writer.WriteAttributeString("Position2", Program.Vector3ToString(vertices[i].pos2) );
                writer.WriteAttributeString("Normal"   , Program.Vector3ToString(vertices[i].nrm)  );
                writer.WriteAttributeString("UV0"      , Program.Vector2ToString(vertices[i].uv0)  );
                writer.WriteAttributeString("UV1"      , Program.Vector2ToString(vertices[i].uv1)  );
                writer.WriteAttributeString("UV2"      , Program.Vector2ToString(vertices[i].uv2)  );
                writer.WriteAttributeString("Color0"   , Program.Vector4ToString(vertices[i].col)  );
                writer.WriteAttributeString("Color1"   , Program.Vector4ToString(vertices[i].col2) );
                writer.WriteAttributeString("Tangent"  , Program.Vector4ToString(vertices[i].tan)  );
                writer.WriteAttributeString("Binormal" , Program.Vector4ToString(vertices[i].bitan));
                
                string tempBoneWeights = "";
                foreach (var w in vertices[i].boneWeights)
                {
                    tempBoneWeights += (w.ToString() + ',');
                }
                tempBoneWeights = tempBoneWeights.Trim(',');
                writer.WriteAttributeString("BlendWeights", tempBoneWeights);

                string tempBoneIds = "";
                foreach (var w in vertices[i].boneIds)
                {
                    tempBoneIds += (w.ToString() + ',');
                }
                tempBoneIds = tempBoneIds.Trim(',');
                writer.WriteAttributeString("BlendIndex", tempBoneIds);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();



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