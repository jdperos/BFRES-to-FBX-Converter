#pragma once
#include <string>
#include <vector>
#include "Math.h"

namespace BFRESStructs
{

using namespace std;

enum RotationType
{
    Quaternion = 0,
    EulerXYZ = 4096

};

enum GX2PrimitiveType
{
    //
    // Summary:
    //     Requires at least 1 element and 1 more to draw another primitive.
    Points = 1,
    //
    // Summary:
    //     Requires at least 2 elements and 2 more to draw another primitive.
    Lines = 2,
    //
    // Summary:
    //     Requires at least 2 elements and 1 more to draw another primitive.
    LineStrip = 3,
    //
    // Summary:
    //     Requires at least 3 elements and 3 more to draw another primitive.
    Triangles = 4,
    //
    // Summary:
    //     Requires at least 3 elements and 1 more to draw another primitive.
    TriangleFan = 5,
    //
    // Summary:
    //     Requires at least 3 elements and 1 more to draw another primitive.
    TriangleStrip = 6,
    //
    // Summary:
    //     Requires at least 4 elements and 4 more to draw another primitive.
    LinesAdjacency = 10,
    //
    // Summary:
    //     Requires at least 4 elements and 1 more to draw another primitive.
    LineStripAdjacency = 11,
    //
    // Summary:
    //     Requires at least 6 elements and 6 more to draw another primitive.
    TrianglesAdjacency = 12,
    //
    // Summary:
    //     Requires at least 6 elements and 2 more to draw another primitive.
    TriangleStripAdjacency = 13,
    //
    // Summary:
    //     Requires at least 3 elements and 3 more to draw another primitive.
    Rects = 17,
    //
    // Summary:
    //     Requires at least 2 elements and 1 more to draw another primitive.
    LineLoop = 18,
    //
    // Summary:
    //     Requires at least 4 elements and 4 more to draw another primitive.
    Quads = 19,
    //
    // Summary:
    //     Requires at least 4 elements and 2 more to draw another primitive.
    QuadStrip = 20,
    //
    // Summary:
    //     Requires at least 2 elements and 2 more to draw another primitive.
    TessellateLines = 130,
    //
    // Summary:
    //     Requires at least 2 elements and 1 more to draw another primitive.
    TessellateLineStrip = 131,
    //
    // Summary:
    //     Requires at least 3 elements and 3 more to draw another primitive.
    TessellateTriangles = 132,
    //
    // Summary:
    //     Requires at least 3 elements and 1 more to draw another primitive.
    TessellateTriangleStrip = 134,
    //
    // Summary:
    //     Requires at least 4 elements and 4 more to draw another primitive.
    TessellateQuads = 147,
    //
    // Summary:
    //     Requires at least 4 elements and 2 more to draw another primitive.
    TessellateQuadStrip = 148
};

enum GX2IndexFormat
{
    UInt16LittleEndian = 0,
    UInt32LittleEndian = 1,
    UInt16 = 4,
    UInt32 = 9
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct ShaderAssign
{
    // TODO
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct ShaderParams
{
    // TODO
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct RenderInfo
{
    // TODO
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FMAT
{
    string       name;
    bool         isVisible;
    RenderInfo   renderInfo;
    ShaderAssign shaderAssign;
    ShaderParams shaderParams;
    // TODO
};


// -----------------------------------------------------------------------
// Represents a node in a Syroot.NintenTools.Bfres.SubMesh bounding tree to determine
// when to show which sub mesh of a Syroot.NintenTools.Bfres.Mesh.
// -----------------------------------------------------------------------
struct SubMeshBounding
{
    Math::vector3F center;
    Math::vector3F extent;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct SubMesh
{
    int count;
    int offset;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FVTX
{
    int index;
    Math::vector3F position0;
    Math::vector3F position1;
    Math::vector3F position2;
    Math::vector3F normal;
    Math::vector2F uv0;
    Math::vector2F uv1;
    Math::vector2F uv2;
    Math::vector4F color0;
    Math::vector4F color1;
    Math::vector4F tangent;
    Math::vector4F binormal;
    Math::vector4F blendWeights;
    Math::vector4  blendIndex;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct LODMesh
{
    GX2PrimitiveType primitiveType;
    GX2IndexFormat   indexFormat;
    int              indexCount;
    int              firstVertex;
    vector<int>      faceVertices;
    SubMesh          subMesh;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FSHP
{
    string                  name;
    int                     vertexBufferIndex;
    int                     vertexSkinCount;
    int                     boneIndex;
    int                     targetAttributeCount;
    int                     materialIndex;
    Math::vector2F          boundingRadius;
    vector<int>             skinBoneIndices;
    vector<SubMeshBounding> boundings;
    vector<LODMesh>         lodMeshes;
    vector<FVTX>            vertices;

};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct Bone
{
    uint32         index;
    string         name;
    bool           isVisible;
    int32          rigidMatrixIndex;
    int32          smoothMatrixIndex;
    int32          billboardIndex;
    bool           useRigidMatrix;
    bool           useSmoothMatrix;
    uint32         parentIndex;
    RotationType   rotationType;
    Math::vector3F scale;
    Math::vector4F rotation;
    Math::vector3F position;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FSKL
{
    uint32         boneCount;
    vector<uint32> boneList;
    vector<Bone>   bones;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FMDL
{
    string       name;
    int          fvtxCount;
    int          fshpCount;
    int          fmatCount;
    int          totalVertices;
    FSKL         fskl;
    vector<FMAT> fmats;
    vector<FSHP> fshps;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct BFRES
{
	vector<FMDL> fmdl;
};

}
