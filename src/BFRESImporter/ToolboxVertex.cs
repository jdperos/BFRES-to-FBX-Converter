using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using OpenTK;

namespace BFRES_Importer
{
    public class Vertex
    {
        public Vector3 pos = new Vector3(0);
        public Vector3 nrm = new Vector3(0);
        public Vector4 col = new Vector4(1);
        public Vector4 col2 = new Vector4(1);

        public Vector2 uv0 = new Vector2(0);
        public Vector2 uv1 = new Vector2(0);
        public Vector2 uv2 = new Vector2(0);
        public Vector2 uv3 = new Vector2(0);

        public Vector4 tan = new Vector4(0);
        public Vector4 bitan = new Vector4(0);

        public List<int> boneIds = new List<int>();
        public List<float> boneWeights = new List<float>();

        public float normalW = 1;

        public List<string> boneNames = new List<string>();

        public List<Bone> boneList = new List<Bone>();
        public class Bone
        {
            public string Name;
            public int Index;
            public bool HasWeights;
            public List<BoneWeight> weights = new List<BoneWeight>();
        }
        public class BoneWeight
        {
            public float weight;
        }
        //For vertex morphing 
        public Vector3 pos1 = new Vector3();
        public Vector3 pos2 = new Vector3();

        public List<Vector4> Unknowns = new List<Vector4>();

        public static void PrintVector2(StreamWriter writer, Vector2 vec2)
        {
            writer.Write(vec2.X + ", " + vec2.Y);
        }
        public static void PrintVector3(StreamWriter writer, Vector3 vec3)
        {
            writer.Write(vec3.X + ", " + vec3.Y + ", " + vec3.Z);
        }
        public static void PrintVector4(StreamWriter writer, Vector4 vec4)
        {
            writer.Write(vec4.X + ", " + vec4.Y + ", " + vec4.Z + ", " + vec4.W);
        }
    }
}
