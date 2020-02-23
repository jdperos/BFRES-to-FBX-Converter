#include "FBXWriter.h"
#include <iostream>
#include "Primitives.h"
#include "assert.h"


FBXWriter::FBXWriter()
{
}

FBXWriter::~FBXWriter()
{
}

void FBXWriter::CreateFBX(FbxScene*& pScene, const BFRESStructs::BFRES& bfres)
{
    m_pBfres = &bfres;
    WriteModel(pScene, bfres.fmdl[1]);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteModel(FbxScene*& pScene, const BFRESStructs::FMDL& fmdl)
{
    WriteSkeleton(pScene, fmdl.fskl);

    for (uint32 i = 0; i < fmdl.fshps.size(); i++)
    {
        WriteShape(pScene, fmdl.fshps[i]);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl)
{
    const uint32 uiTotalBones = fskl.bones.size();
    std::vector<FbxNode*> boneNodes(uiTotalBones);
    for (int32 i = 0; i < uiTotalBones; i++)
        CreateBone(pScene, fskl.bones[i], boneNodes[i]);

    for (int32 i = 0; i < uiTotalBones; i++)
    {
        const BFRESStructs::Bone& bone = fskl.bones[i];
        if (bone.parentIndex >= 0 && bone.parentIndex < uiTotalBones)
            boneNodes[bone.parentIndex]->AddChild(boneNodes[i]);
        else
            pScene->GetRootNode()->AddChild(boneNodes[i]);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode)
{
    // Create a node for our mesh in the scene.
    lBoneNode = FbxNode::Create(pScene, bone.name.c_str());

    // Set transform data
    FbxDouble3 fBoneScale = FbxDouble3(bone.scale.X, bone.scale.Y, bone.scale.Z);
    lBoneNode->LclScaling.Set(fBoneScale);
    if (bone.rotationType == BFRESStructs::RotationType::EulerXYZ)
    {
        FbxDouble3 fBoneRot = FbxDouble3(bone.rotation.X, bone.rotation.Y, bone.rotation.Z);
        lBoneNode->LclRotation.Set(fBoneRot);
        FbxDouble3 fBonePos = FbxDouble3(bone.position.X, bone.position.Y, bone.position.Z);
        lBoneNode->LclTranslation.Set(fBonePos);
    }
    else
    {
        // This FbxWriter doesn't support bone quaternion rotation yet
    }

    // Create a bone.
    FbxSkeleton* lBone = FbxSkeleton::Create(pScene, bone.name.c_str());
    if (bone.name.c_str() == "Root")
    {
        lBone->SetSkeletonType(FbxSkeleton::eRoot);
    }
    else
    {
        lBone->SetSkeletonType(FbxSkeleton::eLimbNode);
    }

    // Set the node attribute of the bone node.
    lBoneNode->SetNodeAttribute(lBone);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp)
{
    WriteMesh(pScene, fshp, fshp.lodMeshes[0]);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh)
{

    // Create a node for our mesh in the scene.
    FbxNode* lMeshNode = FbxNode::Create(pScene, fshp.name.c_str());

    // Create a mesh.
    FbxMesh* lMesh = FbxMesh::Create(pScene, "");

    // Set the node attribute of the mesh node.
    lMeshNode->SetNodeAttribute(lMesh);

    // Add the mesh node to the root node in the scene.
    FbxNode* lRootNode = pScene->GetRootNode();
    lRootNode->AddChild(lMeshNode);

    // Initialize the control point array of the mesh.
    uint32 uiNumControlPoints(fshp.vertices.size());
    lMesh->InitControlPoints(uiNumControlPoints);
    FbxVector4* lControlPoints = lMesh->GetControlPoints();


    // Create a normal layer.
    FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");

    // Set its mapping mode to map each normal vector to each control point.
    lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);

    // Set the reference mode of so that the n'th element of the normal array maps to the n'th
    // element of the control point array.
    lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

    std::vector<SkinCluster> vSkinClusters((*m_pBfres).fmdl[1].fskl.boneList.size());

    FbxVector4 normal(0, 0, 0, 1.0f);
    for (uint32 i = 0; i < uiNumControlPoints; i++)
    {
        // TODO make this iterative
        const Math::vector3F& posVec = fshp.vertices[i].position0;
        lControlPoints[i] = FbxVector4(posVec.X, posVec.Y, posVec.Z);

        normal.mData[0] = fshp.vertices[i].normal.X;
        normal.mData[1] = fshp.vertices[i].normal.Y;
        normal.mData[2] = fshp.vertices[i].normal.Z;
        normal.mData[3] = 1.0f;

        lLayerElementNormal->GetDirectArray().Add(normal);

        CreateVertBlendDataToSkinCluster(fshp, vSkinClusters); 
    }

    //WriteSkin(pScene, lMesh, vSkinClusters);

    // Create layer 0 for the mesh if it does not already exist.
    // This is where we will define our normals.
    FbxLayer* lLayer = lMesh->GetLayer(0);
    if (lLayer == NULL) {
        lMesh->CreateLayer();
        lLayer = lMesh->GetLayer(0);
    }

    lLayer->SetNormals(lLayerElementNormal);

    // Define which control points belong to a poly
    uint32 uiPolySize(3);
    // TODO make this iterative
    for (uint32 i = 0; i < lodMesh.faceVertices.size(); ++i)
    {
        // first index
        if ((i % uiPolySize) == 0)
            lMesh->BeginPolygon();

        // TODO make this iterative
        lMesh->AddPolygon(lodMesh.faceVertices[i]);

        // last index
        if ((i + 1) % uiPolySize == 0)
            lMesh->EndPolygon();
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::vector<SkinCluster>& vSkinClusters)
{
    FbxSkin* pSkin = FbxSkin::Create(pScene, "");
    FbxAMatrix& lXMatrix = pMesh->GetNode()->EvaluateGlobalTransform();
    const BFRESStructs::FSKL& fskl = (*m_pBfres).fmdl[1].fskl;

    for (uint32 uiSkinCluster = 0; uiSkinCluster < vSkinClusters.size(); ++uiSkinCluster)
    {
        SkinCluster& skinCluster = vSkinClusters[uiSkinCluster];

        std::string boneName = fskl.bones[skinCluster.m_uiBoneIndex].name;

        FbxNode* pBoneNode = pScene->FindNodeByName(FbxString(boneName.c_str()));
        assert(pBoneNode != NULL);

        FbxCluster* pCluster = FbxCluster::Create(pScene, "");
        pCluster->SetLink(pBoneNode);
        // eTotalOne means Mode eTotalOne is identical to mode eNormalize except that the sum of the weights assigned to a control point is not normalized and must equal 1.0.
        // https://help.autodesk.com/view/FBX/2017/ENU/?guid=__cpp_ref_class_fbx_cluster_html
        pCluster->SetLinkMode(FbxCluster::eTotalOne); 
        
        for(uint32 uiControlPoint = 0; uiControlPoint < skinCluster.m_vControlPointIndices.size(); ++uiControlPoint)
        {
            pCluster->AddControlPointIndex(skinCluster.m_vControlPointIndices[uiControlPoint],
                                           skinCluster.m_vControlPointWeights[uiControlPoint]);
        }

        // Now we have the mesh and the skeleton correctly positioned,
        // set the Transform and TransformLink matrix accordingly.
        pCluster->SetTransformMatrix(lXMatrix);
        pCluster->SetTransformLinkMatrix(pBoneNode->EvaluateGlobalTransform());

        // Add the clusters to the skin
        pSkin->AddCluster(pCluster);
    }
    
    pMesh->AddDeformer(pSkin);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::CreateVertBlendDataToSkinCluster(const BFRESStructs::FSHP& fshp, std::vector<SkinCluster>& vSkinClusters)
{
    const std::vector<BFRESStructs::FVTX>& verts = fshp.vertices;

    for(uint32 uiVertIndex = 0; uiVertIndex < verts.size(); ++uiVertIndex)
    {
        const BFRESStructs::FVTX& vert = verts[uiVertIndex];

        uint32 uiBlendIndices[4] = { vert.blendIndex.X, vert.blendIndex.Y, vert.blendIndex.Z, vert.blendIndex.W };
        float  fBlendWeights[4]  = { vert.blendWeights.X, vert.blendWeights.Y, vert.blendWeights.Z, vert.blendWeights.W };
        for(uint32 uiBlendEntry = 0; uiBlendEntry < 4; ++uiBlendEntry) // max limit for uiBlendEntry is 4 because FLOAT FUCKING 4
        {
            if (fBlendWeights[uiBlendEntry] > 0)
            {
                uint32 uiBlendIndex = uiBlendIndices[uiBlendEntry]; // index into the SkinBoneIndices array
                
                SkinCluster& skinCluster = vSkinClusters[uiBlendIndex];
                skinCluster.m_uiBoneIndex = (*m_pBfres).fmdl[1].fskl.boneList[uiBlendIndex];
                skinCluster.m_vControlPointIndices.push_back(uiVertIndex);
                skinCluster.m_vControlPointWeights.push_back(fBlendWeights[uiBlendEntry]);
            }
        }
    }   
}
