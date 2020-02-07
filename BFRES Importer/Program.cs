using System;
using System.IO;
using ResU = Syroot.NintenTools.Bfres;

namespace BFRES_Importer
{
    class Program
    {
        static void Main(string[] args)
        {
            ResU.ResFile res = new ResU.ResFile("../../../../TestAssets/Npc_Gerudo_Queen.bfres");
            LoadFile(res);
        }

        public static void LoadFile(ResU.ResFile res)
        {
            StreamWriter writer = new StreamWriter("../../../../TestAssets/Dump.txt");
            if (res.Models.Count > 0)
            {
                for (int ii = 0; ii < res.Models.Count; ii++)
                {
                    FMDL fMDL = new FMDL();
                    fMDL.DumpFMDLData(res.Models[ii], writer);
                }
            }
            writer.Close();
        }
    }
}
