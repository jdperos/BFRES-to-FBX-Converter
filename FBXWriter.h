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
        uint32              m_uiBoneIndex;
        std::vector<uint32> m_vControlPointIndices;
        std::vector<float>  m_vControlPointWeights;
    };

    const BFRESStructs::BFRES* m_pBfres;

    void CreateFBX(FbxScene*& pScene, const BFRESStructs::BFRES& bfres);
    void WriteModel(FbxScene*& pScene, const BFRESStructs::FMDL& fmdl);
    void WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl);
    void WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp);
    void WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh);
    void WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::vector<SkinCluster>& vSkinClusters);

    void CreateVertBlendDataToSkinCluster(const BFRESStructs::FSHP& fshp, std::vector<SkinCluster>& vSkinClusters);
    void CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode);
};