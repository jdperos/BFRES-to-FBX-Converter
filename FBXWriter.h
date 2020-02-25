#pragma once
#include <fbxsdk.h>
#include "BFRES.h"

class FBXWriter
{
public:
    FBXWriter();
    ~FBXWriter();

    struct SkinCluster 
    {
        std::vector<uint32> m_vControlPointIndices;
        std::vector<float>  m_vControlPointWeights;
    };

	enum class SkinningType
	{
		eSmooth,
		eRigid
	};

	struct BoneListInfo
	{
		uint32       uiBoneIndex;
		std::string  szName; // I know this is wasted space, but bite me. It'll make debugging simpler.
		SkinningType eSkinningType;
	};

    const BFRESStructs::BFRES* m_pBfres;

    void CreateFBX(FbxScene*& pScene, const BFRESStructs::BFRES& bfres);
    void WriteModel(FbxScene*& pScene, const BFRESStructs::FMDL& fmdl);
    void WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl, std::vector<BoneListInfo>& boneListInfos);
    void WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, std::vector<BoneListInfo> boneListInfos);
    void WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh, std::vector<BoneListInfo> boneListInfos);
    void WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::vector<SkinCluster>& vSkinClusters);
    void WriteBindPose(FbxScene*& pScene, FbxNode*& pMeshNode);

    void CreateSkinClusterData(const BFRESStructs::FVTX& vert, uint32 uiVertIndex, std::vector<SkinCluster>& vSkinClusters, std::vector<BoneListInfo>& boneListInfos);
    void CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode, std::vector<BoneListInfo>& boneListInfos);
};