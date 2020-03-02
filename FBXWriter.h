#pragma once
#include <fbxsdk.h>
#include "BFRES.h"
#include <map>

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

    const BFRESStructs::BFRES* m_pBfres;

    void CreateFBX(FbxScene*& pScene, const BFRESStructs::BFRES& bfres);
    void WriteModel(FbxScene*& pScene, const BFRESStructs::FMDL& fmdl);
    void WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl, std::vector<BoneMetadata>& boneListInfos);
    void WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, std::vector<BoneMetadata>& boneListInfos);
    void WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh, std::vector<BoneMetadata>& boneListInfos);

    void MapFacesToVertices( const BFRESStructs::LODMesh& lodMesh, FbxMesh* lMesh );
    void MapPolygonsToVertices(const BFRESStructs::LODMesh& lodMesh, FbxMesh* lMesh);

    void WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap);
    void WriteBindPose(FbxScene*& pScene, FbxNode*& pMeshNode);

    void CreateSkinClusterData(const BFRESStructs::FVTX& vert, uint32 uiVertIndex, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap, std::vector<BoneMetadata>& boneListInfos, const BFRESStructs::FSHP& fshp);
    void CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode, std::vector<BoneMetadata>& boneListInfos);
};