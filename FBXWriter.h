#pragma once
#include <fbxsdk.h>
#include "BFRES.h"

class FBXWriter
{
public:
    FBXWriter();
    ~FBXWriter();

    void CreateFBX(FbxScene* pScene, const BFRESStructs::BFRES& bfres);
};