using System;
using System.IO;
using ResU = Syroot.NintenTools.Bfres;

namespace BFRES_Importer
{
    class Program
    {
        public static ResU.ResFile res;

        static void Main(string[] args)
        {
            res = new ResU.ResFile("C:/Users/Jon/Desktop/Npc_Gerudo_Queen.bfres");
            LoadFile(res);
        }

        public static void LoadFile(ResU.ResFile res)
        {
            StreamWriter writer = new StreamWriter("C:/Users/Jon/Desktop/Dump.txt");
            if (res.Models.Count > 0)
            {
                for (int ii = 0; ii < res.Models.Count; ii++)
                {
                    writer.WriteLine("Name: "                   + res.Models[ii].Name);
                    writer.WriteLine("Vertex Buffers Count: "   + res.Models[ii].VertexBuffers.Count);
                    for (int jj = 0; jj < res.Models[ii].VertexBuffers.Count; jj++)
                    {
                        writer.WriteLine(res.Models[ii].VertexBuffers[jj].VertexCount);
                    }
                    writer.WriteLine("Materials Count: "        + res.Models[ii].Materials.Count);
                    writer.WriteLine("Path: "                   + res.Models[ii].Path);
                    writer.WriteLine("Shape Count: "            + res.Models[ii].Shapes.Count);
                    writer.WriteLine("Skeleton Bone Count: "    + res.Models[ii].Skeleton.Bones.Count);
                    writer.WriteLine("Total Vertex Count: "     + res.Models[ii].TotalVertexCount);
                    writer.WriteLine("User data: "              + res.Models[ii].UserData);

                    writer.Flush();
                }
            }
            writer.Close();
        }
    }
}
