#pragma once
#include <fbxsdk.h>
#include "BFRES.h"
#include <map>

using namespace BFRESStructs;

class FBXWriter
{
public:
    FBXWriter();
    ~FBXWriter();

    struct SkinCluster 
    {
        std::string         m_szName;
        std::vector<uint32> m_vControlPointIndices;
        std::vector<float>  m_vControlPointWeights;
    };

	enum class SkinningType
	{
		eSmooth,
		eRigid
	};

	struct BoneMetadata
	{
		uint32       uiBoneIndex;
		std::string  szName; // I know this is wasted space, but bite me. It'll make debugging simpler.
		SkinningType eSkinningType;
	};

    void CreateFBX(FbxScene*& pScene, const BFRES& bfres);
    void WriteModel(FbxScene*& pScene, const FMDL& fmdl);
    void WriteSkeleton(FbxScene*& pScene, const FSKL& fskl, std::vector<BoneMetadata>& boneListInfos);
    void WriteShape(FbxScene*& pScene, const FSHP& fshp, std::vector<BoneMetadata>& boneListInfos);
    void WriteMesh(FbxScene*& pScene, const FSHP& fshp, const LODMesh& lodMesh, std::vector<BoneMetadata>& boneListInfos);
    void SetTexturesToMaterial(FbxScene*& pScene, const FSHP& fshp, FbxSurfacePhong* lMaterial, FbxLayerElementUV* lLayerElementUV0, FbxLayerElementUV* lLayerElementUV1, FbxLayerElementUV* lLayerElementUV2);

    void MapFacesToVertices( const LODMesh& lodMesh, FbxMesh* lMesh );
    void MapPolygonsToVertices(const LODMesh& lodMesh, FbxMesh* lMesh);

    void WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap);
    void WriteBindPose(FbxScene*& pScene, FbxNode*& pMeshNode);

    void CreateSkinClusterData(const FVTX& vert, uint32 uiVertIndex, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap, std::vector<BoneMetadata>& boneListInfos, const FSHP& fshp);
    void CreateBone(FbxScene*& pScene, const Bone& bone, FbxNode*& lBoneNode, std::vector<BoneMetadata>& boneListInfos);
};