using System.IO;
using ResU = Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Core;
using Syroot.NintenTools.Bfres.Helpers;
using Syroot.NintenTools.Bfres.GX2;
using Syroot.BinaryData;
using Syroot.BinaryData.Core;
using Syroot.Maths;

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

            // FVTX.DumpFVTXData(model, writer);

            foreach (Material mat in model.Materials.Values)
            {
                // Read Materials Function
            }
            foreach (Shape shp in model.Shapes.Values)
            {
                VertexBuffer vertexBuffer = model.VertexBuffers[shp.VertexBufferIndex];
                Material material = model.Materials[shp.MaterialIndex];
                ReadShapesVertices(writer, shp, vertexBuffer, model);
            }

            writer.Flush();
        }

        public static void ReadShapesVertices(StreamWriter writer, Shape shp, VertexBuffer vertexBuffer, ResU.Model model)
        {
            // TODO: Clear Bounding Boxes
            // TODO: Clear Bounding Radius
            // TODO: Clear Bone Indices
            writer.WriteLine("Shape name: " + shp.Name);

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
        }
    }

    public class FVTX
    {
        public static void DumpFVTXData(ResU.Model model, StreamWriter writer)
        {

            for (int i = 0; i < model.VertexBuffers.Count; i++)
            {
                writer.WriteLine();
                writer.WriteLine("[[FVTX]]");
                writer.WriteLine("Attributes Count: " + model.VertexBuffers[i].Attributes.Count);
                writer.Flush();

                for (int j = 0; j < model.VertexBuffers[i].Attributes.Count; j++)
                {
                    writer.WriteLine("Name: " + model.VertexBuffers[i].Attributes[j].Name);
                    writer.WriteLine("Buffer Index: " + model.VertexBuffers[i].Attributes[j].BufferIndex);
                    writer.WriteLine("Offset: " + model.VertexBuffers[i].Attributes[j].Offset);
                    writer.WriteLine("Format: " + model.VertexBuffers[i].Attributes[j].Format);
                    writer.Flush();
                }

                writer.WriteLine("Buffers Count:" + model.VertexBuffers[i].Buffers.Count);

                for (int j = 0; j < model.VertexBuffers[i].Buffers.Count; j++)
                {
                    writer.WriteLine("Stride: " + model.VertexBuffers[i].Buffers[j].Stride);
                    writer.WriteLine("Data: " + model.VertexBuffers[i].Buffers[j].Data);

                    //// TODO: Figure out how tf to spit out the vertex data
                    //writer.WriteLine("Data[" + model.VertexBuffers[i].Buffers[j].Data.Length + "][" + model.VertexBuffers[i].Buffers[j].Data[0].Length + "]");
                    //writer.Write("{ ");

                    //for (int k = 0; k < model.VertexBuffers[i].Buffers[j].Data.Length; k++)
                    //{
                    //    for (int l = 0; l < model.VertexBuffers[i].Buffers[j].Data[k].Length; l++)
                    //    {
                    //        writer.Write(model.VertexBuffers[i].Buffers[j].Data[k][l] + ", ");
                    //    }

                    //}
                    //writer.Write("}\n");

                    writer.WriteLine("Buffer Offset: " + model.VertexBuffers[i].Buffers[j].BufferOffset);
                }

                writer.WriteLine("Vertex Count:" + model.VertexBuffers[i].VertexCount);
                writer.WriteLine("Vertex Skin Count:" + model.VertexBuffers[i].VertexSkinCount);
                writer.Flush();
            }
        }
    }
}
