#pragma once
#include <fbxsdk.h>

class MyFBXCube
{
public:
    MyFBXCube();
    ~MyFBXCube();

    void CreateCube(FbxScene* pScene);
};