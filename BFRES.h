#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include "Primitives.h"

namespace BFRESStructs
{

using namespace std;

enum class RotationType
{
    Quaternion = 0,
    EulerXYZ = 4096

};

enum class BoneFlagsRotation
{
    /// <summary>
    /// A quaternion represents the rotation.
    /// </summary>
    Quaternion,

    /// <summary>
    /// A <see cref="Vector3F"/> represents the Euler rotation in XYZ order.
    /// </summary>
    EulerXYZ
};

enum class BoneFlagsBillboard
{
    /// <summary>
    /// No transformation is applied.
    /// </summary>
    None,

    /// <summary>
    /// Transforms of the child are applied.
    /// </summary>
    Child,

    /// <summary>
    /// Transforms the Z axis parallel to the camera.
    /// </summary>
    WorldViewVector,

    /// <summary>
    /// Transforms the Z axis parallel to the direction of the camera.
    /// </summary>
    WorldViewPoint,

    /// <summary>
    /// Transforms the Y axis parallel to the camera up vector, and the Z parallel to the camera up-vector.
    /// </summary>
    ScreenViewVector,

    /// <summary>
    /// Transforms the Y axis parallel to the camera up vector, and the Z axis parallel to the direction of the
    /// camera.
    /// </summary>
    ScreenViewPoint,

    /// <summary>
    /// Transforms the Z axis parallel to the camera by rotating only the Y axis.
    /// </summary>
    YAxisViewVector,

    /// <summary>
    /// Transforms the Z axis parallel to the direction of the camera by rotating only the Y axis.
    /// </summary>
    YAxisViewPoint
};

enum class BoneFlagsTransform
{
    None,
    ScaleUniform,
    ScaleVolumeOne,
    RotateZero,
    TranslateZero,
    ScaleOne,
    RotateTranslateZero,
    Identity
};

enum class BoneFlagsTransformCumulative
{
	None,
	ScaleUniform,
	ScaleVolumeOne,
	RotateZero,
	TranslateZero,
	ScaleOne,
	RotateTranslateZero,
	Identity
};

enum class GX2PrimitiveType
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

enum class GX2IndexFormat
{
    UInt16LittleEndian = 0,
    UInt32LittleEndian = 1,
    UInt16 = 4,
    UInt32 = 9
};

enum class ShapeFlags
{
    /// <summary>
    /// The <see cref="Shape"/> instance references a <see cref="VertexBuffer"/>.
    /// </summary>
    HasVertexBuffer = 1 << 1,

    /// <summary>
    /// The boundings in all submeshes are consistent.
    /// </summary>
    SubMeshBoundaryConsistent = 1 << 2
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
    uint32         index;
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
    uint32           indexCount;
    uint32           firstVertex;
    vector<int32>    faceVertices; // TODO should this be uint32?
    SubMesh          subMesh;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FSHP
{
    string                  name;
    ShapeFlags              flags;
    uint32                  materialIndex;
    uint32                  boneIndex;
    uint32                  vertexBufferIndex;
    vector<float>           radiusArray;
    uint32                  vertexSkinCount;
    uint32                  targetAttributeCount;
    vector<LODMesh>         lodMeshes;
    vector<uint32>          skinBoneIndices;
    // TODO add KeyShapes
    vector<SubMeshBounding> boundings;
    // TODO add SubmeshBoundingNodes and SubmeshBoundingIndices
    vector<FVTX>            vertices;

};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct Bone
{
    uint32                       index;
    string                       name;
    bool                         isVisible;
    int32                        rigidMatrixIndex;
    int32                        smoothMatrixIndex;
    int32                        billboardIndex;
    BoneFlagsRotation            boneFlagsRotation;
    BoneFlagsBillboard           boneFlagsBillboard;
    BoneFlagsTransform           boneFlagsTransform;
    BoneFlagsTransformCumulative boneFlagsTransformCumulative;
    bool                         useRigidMatrix;
    bool                         useSmoothMatrix;
    uint32                       parentIndex;
    RotationType                 rotationType;
    Math::vector3F               scale;
    Math::vector4F               rotation;
    Math::vector3F               position;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FSKL
{
    uint32         boneCount;
    vector<uint32> boneList;
    vector<Bone>   bones;
    // TODO check that there is not more data to grab from the FSKL class in Nintentools
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
