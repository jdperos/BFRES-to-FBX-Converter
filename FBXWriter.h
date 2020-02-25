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
        std::vector<uint32> m_vControlPointIndices;
        std::vector<float>  m_vControlPointWeights;
    };

    const BFRESStructs::BFRES* m_pBfres;

    void CreateFBX(FbxScene*& pScene, const BFRESStructs::BFRES& bfres);
    void WriteModel(FbxScene*& pScene, const BFRESStructs::FMDL& fmdl);
    void WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl);
    void WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp);
    void WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh);
    void WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::map<uint32,SkinCluster>& BoneIndexToSkinClusterMap);
    void WriteBindPose(FbxScene*& pScene, FbxNode*& pMeshNode);

    void CreateSkinClusterData(const BFRESStructs::FVTX& vert, uint32 uiVertIndex, std::map<uint32,SkinCluster>& BoneIndexToSkinClusterMap);
    void CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode);
};