#pragma once
#include <fbxsdk.h>
#include "BFRES.h"

class FBXWriter
{
public:
    FBXWriter();
    ~FBXWriter();

    void CreateFBX(FbxScene* pScene, const BFRESStructs::BFRES& bfres);
    void WriteModel(FbxScene* pScene, const BFRESStructs::FMDL& fmdl);
    void WriteSkeleton(FbxScene* pScene, const BFRESStructs::FSKL& fskl);
    void WriteBone(FbxScene* pScene, const BFRESStructs::Bone& bone);
    void WriteShape(FbxScene* pScene, const BFRESStructs::FSHP& fshp);
    void WriteMesh(FbxScene* pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh, const std::vector<BFRESStructs::FVTX>& vertices);
};