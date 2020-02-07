using System.IO;
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

            FVTX.DumpFVTXData(model, writer);

            writer.Flush();
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
                writer.WriteLine("Attributes Count:" + model.VertexBuffers[i].Attributes.Count);
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
                    writer.WriteLine("Buffer Offset: " + model.VertexBuffers[i].Buffers[j].BufferOffset);
                }

                writer.WriteLine("Vertex Count:" + model.VertexBuffers[i].VertexCount);
                writer.WriteLine("Vertex Skin Count:" + model.VertexBuffers[i].VertexSkinCount);
                writer.Flush();
            }
        }
    }
}
