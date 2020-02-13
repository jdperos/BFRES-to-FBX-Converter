using System.IO;
using ResU = Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres;
using Syroot.NintenTools.Bfres.Helpers;

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
            // Clear Bounding Boxes
            // Clear Bounding Radius
            // Clear Bone Indices

            foreach (Bounding bnd in shp.SubMeshBoundings)
            {
                writer.WriteLine(bnd.Center.X + );
            }
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
