#pragma once
#include <string>
#include <vector>
#include "JPMath.h"
#include "Primitives.h"

namespace BFRESStructs
{

using namespace std;

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



/// <summary>
/// Represents desired texture filter options within a plane.
/// </summary>
enum class GX2TexXYFilterType
{
    Point,
    Bilinear
};

/// <summary>
/// Represents desired texture filter options between Z planes.
/// </summary>
enum class GX2TexZFilterType
{
    UseXY,
    Point,
    Linear
};

/// <summary>
/// Represents desired texture filter options between mip levels.
/// </summary>
enum class GX2TexMipFilterType
{
    NoMip,
    Point,
    Linear
};

/// <summary>
/// Represents maximum desired anisotropic filter ratios. Higher ratios give better image quality, but slower
/// performance.
/// </summary>
enum class GX2TexAnisoRatio
{
    OneToOne,
    TwoToOne,
    FourToOne,
    EightToOne,
    SixteenToOne
};

/// <summary>
/// Represents type of border color to use.
/// </summary>
enum class GX2TexBorderType
{
    ClearBlack,
    SolidBlack,
    SolidWhite,
    UseRegister
};

/// <summary>
/// Represents compare functions used for depth and stencil tests.
/// </summary>
enum class GX2CompareFunction
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class GX2TextureMapType
{
    Albedo,
    Normal,
    Specular,
    AmbientOcclusion,
    Emission,
    Shadow,
    Light,
    MRA,
    Metalness,
    Roughness,
    SubSurfaceScattering,
    eMAX
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct TextureRef 
{
	/// <summary>
    /// Represents how to treat texture coordinates outside of the normalized coordinate texture range.
    /// </summary>
	enum class GX2TexClamp
	{
		Wrap,
		Mirror,
		Clamp,
		MirrorOnce,
		ClampHalfBorder,
		MirrorOnceHalfBorder,
		ClampBorder,
		MirrorOnceBorder
	};

    std::string         name;
    GX2TexClamp         clampX;
    GX2TexClamp         clampY;
    GX2TexClamp         clampZ;
    std::string         samplerName;
    std::string         useSampler;
    GX2TexXYFilterType  minFilter;
    GX2TexXYFilterType  magFilter;
    GX2TexZFilterType   zFilter;
    GX2TexMipFilterType mipFilter;
    GX2TexAnisoRatio    maxAnisotropicRatio;
    GX2TexBorderType    borderType;
    GX2CompareFunction  depthCompareFunc;
    float               minLod;
    float               maxLod;
    float               lodBias;
    bool                depthCompareEnabled;
    GX2TextureMapType   type;
    uint32              textureUnit;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct TextureRefs
{
    uint32 textureCount;
    vector<TextureRef> textures;
};

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FMAT
{
    string               name;
    bool                 isVisible;
    vector<RenderInfo>   renderInfo;
    ShaderAssign         shaderAssign;
    vector<ShaderParams> shaderParams;
    TextureRefs          textureRefs;
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
    string                  name;
    ShapeFlags              flags;
    uint32                  modelIndex;
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
	enum class RotationType
	{
		Quaternion = 0,
		EulerXYZ = 4096
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
    int32                        parentIndex;
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
    uint32       index;
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
struct KeyFrame
{
    uint32 m_uiFrame;
    float  m_fValue;
    bool   m_bIsDegrees = false;
    bool   m_bIsWeighted = false;
    float  m_fSlope1;
    float  m_fSlope2;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct AnimTrack 
{
    enum class CurveInterpolationType
    {
        LINEAR   = 0,
        CONSTANT = 1,
        HERMITE  = 2,
        STEP     = 3,
        STEPBOOL = 4
    };
    string                 m_szName;
    CurveInterpolationType m_eInterpolationType;
    bool                   m_bConstant;
    uint32                 m_cFrames;
    uint32                 m_uiStartFrame;
    uint32                 m_uiEndFrame;
    float                  m_fDelta;
    uint32                 m_cKeys;
    vector<KeyFrame>       m_vKeyFrames;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct BoneAnim 
{
    enum class AnimRotationType
	{
		EULER = 0,
		QUATERNION
	};

    string           m_szName;
    int32            m_iHash;
    AnimRotationType m_eRotType;
    bool             m_bUseSegmentScaleCompensate;
    AnimTrack        m_XSCA;
    AnimTrack        m_YSCA;
    AnimTrack        m_ZSCA;
    AnimTrack        m_XROT;
    AnimTrack        m_YROT;
    AnimTrack        m_ZROT;
    AnimTrack        m_WROT;
    AnimTrack        m_XPOS;
    AnimTrack        m_YPOS;
    AnimTrack        m_ZPOS;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct UserData 
{
	/// <summary>
	/// Represents the possible data types of values stored in <see cref="UserData"/> instances.
	/// </summary>
	enum class UserDataType
	{
		/// <summary>
		/// The values is an <see cref="Int32"/> array.
		/// </summary>
		Int32,

		/// <summary>
		/// The values is a <see cref="Single"/> array.
		/// </summary>
		Single,

		/// <summary>
		/// The values is a <see cref="String"/> array encoded in ASCII.
		/// </summary>
		String,

		/// <summary>
		/// The values is a <see cref="String"/> array encoded in UTF-16.
		/// </summary>
		WString,

		/// <summary>
		/// The values is a <see cref="Byte"/> array.
		/// </summary>
		Byte
	};

    string        m_szName;
    UserDataType  m_eType;
    vector<float> m_vfValues;
};

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct Anim 
{
	/// <summary>
    /// Represents the data format in which scaling values are stored.
    /// </summary>
	enum class SkeletalAnimFlagsScale
	{
		/// <summary>
		/// No scaling.
		/// </summary>
		None,

		/// <summary>
		/// Default scaling.
		/// </summary>
		Standard = 1 << 8,

		/// <summary>
		/// Autodesk Maya scaling.
		/// </summary>
		Maya = 2 << 8,

		/// <summary>
		/// Autodesk Softimage scaling.
		/// </summary>
		Softimage = 3 << 8
	};
    std::string            m_szName;
    bool                   m_bIsBaked;
    bool                   m_bIsLooping;
    SkeletalAnimFlagsScale m_eScalingType;
    uint32                 m_cFrames;
    uint32                 m_cBoneAnims;
    // BakedSize not stored
    uint32                 m_cUserData;
    // BindIndices not stored
    vector<BoneAnim>       m_vBoneAnims;
    vector<UserData>       m_vUserData;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct FSKA
{
    vector<Anim> anims;
};


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct BFRES
{
	vector<FMDL> fmdl;
    FSKA         fska;
};

class BFRESManager
{
private:
    BFRES* m_pBFRES;

public:

    BFRESManager()
    {
        m_pBFRES = new BFRES();
    }
    ~BFRESManager()
    {
        delete m_pBFRES;
    }

    BFRES* GetBFRES()
    {
        return m_pBFRES;
    }

    FSKL* GetSkeletonByModelIndex(uint32 modelIndex)
    {
        // TODO write asset check to ensure modelIndex is valid
        return &m_pBFRES->fmdl[modelIndex].fskl;
    }

    FMAT* GetMaterialByIndex(uint32 modelIndex, uint32 materialIndex)
    {
        return &m_pBFRES->fmdl[modelIndex].fmats[materialIndex];
    }

    TextureRef* GetTextureFromMaterialByType(FMAT* fmat, GX2TextureMapType type)
    {
        for (uint32 i = 0; i < fmat->textureRefs.textureCount; i++)
        {
            if (fmat->textureRefs.textures[i].type == type)
            {
                return &fmat->textureRefs.textures[i];
            }
        }

        return NULL;
    }
};

extern BFRESManager g_BFRESManager;

}
