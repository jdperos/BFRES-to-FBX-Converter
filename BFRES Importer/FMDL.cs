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
                writer.WriteLine("Attributes Count: " + model.VertexBuffers[i].Attributes.Count);
                writer.Flush();

                for (int j = 0; j < model.VertexBuffers[i].Attributes.Count; j++)
                {
                    writer.Write("Name: ");
                    switch (model.VertexBuffers[i].Attributes[j].Name)
                    {
                        case "_p0":
                            writer.Write("position0");
                            break;
                        case "_n0":
                            writer.Write("normal0");
                            break;
                        case "_t0":
                            writer.Write("tangent0");
                            break;
                        case "_b0":
                            writer.Write("binormal0");
                            break;
                        case "_w0":
                            writer.Write("blendweight0");
                            break;
                        case "_i0":
                            writer.Write("blendindex0");
                            break;
                        case "_u0":
                            writer.Write("uv0");
                            break;
                        case "_u1":
                            writer.Write("uv1");
                            break;
                        case "_u2":
                            writer.Write("uv2");
                            break;
                        case "_u3":
                            writer.Write("uv3");
                            break;
                        case "_c0":
                            writer.Write("color0");
                            break;
                        case "_c1":
                            writer.Write("color1");
                            break;
                        default:
                            break;
                    }
                    writer.Write("\n");
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
