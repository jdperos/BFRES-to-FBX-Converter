#include "FBXWriter.h"
#include <iostream>
#include "Primitives.h"
#include "assert.h"

#define FLIP_UV_VERTICAL true // This should always be true for BFRES

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
	// Create an array to store the smooth and rigid bone indices
	std::vector<BoneMetadata> boneInfoList(fmdl.fskl.boneList.size());

    WriteSkeleton(pScene, fmdl.fskl, boneInfoList);

    for (uint32 i = 0; i < fmdl.fshps.size(); i++)
    {
        WriteShape(pScene, fmdl.fshps[i], boneInfoList);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteSkeleton(FbxScene*& pScene, const BFRESStructs::FSKL& fskl, std::vector<BoneMetadata>& boneInfoList)
{
    const uint32 uiTotalBones = fskl.bones.size();
    std::vector<FbxNode*> boneNodes(uiTotalBones);

    for (int32 i = 0; i < uiTotalBones; i++)
        CreateBone(pScene, fskl.bones[i], boneNodes[i], boneInfoList);

    for (int32 i = 0; i < uiTotalBones; i++)
    {
        const BFRESStructs::Bone& bone = fskl.bones[i];
        if (bone.parentIndex >= 0)
            boneNodes[bone.parentIndex]->AddChild(boneNodes[i]);
        else
            pScene->GetRootNode()->AddChild(boneNodes[i]);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::CreateBone(FbxScene*& pScene, const BFRESStructs::Bone& bone, FbxNode*& lBoneNode, std::vector<BoneMetadata>& boneInfoList)
{
    // Create a node for our mesh in the scene.
    lBoneNode = FbxNode::Create(pScene, bone.name.c_str());

    // Set transform data
    FbxDouble3 fBoneScale = FbxDouble3(bone.scale.X, bone.scale.Y, bone.scale.Z);
    lBoneNode->LclScaling.Set(fBoneScale);
    if (bone.rotationType == BFRESStructs::RotationType::EulerXYZ)
    {
        FbxDouble3 fBoneRot = FbxDouble3(
            Math::ConvertRadiansToDegrees(bone.rotation.X),
            Math::ConvertRadiansToDegrees(bone.rotation.Y),
            Math::ConvertRadiansToDegrees(bone.rotation.Z));
        lBoneNode->LclRotation.Set(fBoneRot);
    }
    else
    {
        // This FbxWriter doesn't support bone quaternion rotation yet
        assert(0 && "FbxWriter doesn't support bone quaternion rotation yet" );
    }
    FbxDouble3 fBonePos = FbxDouble3(bone.position.X, bone.position.Y, bone.position.Z);
    lBoneNode->LclTranslation.Set(fBonePos);

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

	// Add bone data to the bone info list
	if (bone.useSmoothMatrix)
	{
		boneInfoList[bone.smoothMatrixIndex].uiBoneIndex   = bone.index;
		boneInfoList[bone.smoothMatrixIndex].szName        = bone.name;
		boneInfoList[bone.smoothMatrixIndex].eSkinningType = SkinningType::eSmooth;
	}
	if (bone.useRigidMatrix)
	{
		boneInfoList[bone.rigidMatrixIndex].uiBoneIndex   = bone.index;
		boneInfoList[bone.rigidMatrixIndex].szName        = bone.name;
		boneInfoList[bone.rigidMatrixIndex].eSkinningType = SkinningType::eRigid;
	}
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteShape(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, std::vector<BoneMetadata>& boneInfoList)
{
    WriteMesh(pScene, fshp, fshp.lodMeshes[0], boneInfoList);
}


// Create materials for pyramid.
void CreateMaterials(FbxScene* pScene, FbxMesh* pMesh)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		FbxString lMaterialName = "material";
		FbxString lShadingName = "Phong";
		lMaterialName += i;
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lRed(1.0, 0.0, 0.0);
		FbxDouble3 lColor;
		FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());


		// Generate primary and secondary colors.
		lMaterial->Emissive.Set(lBlack);
		lMaterial->Ambient.Set(lRed);
		lColor = FbxDouble3(i > 2 ? 1.0 : 0.0,
			i > 0 && i < 4 ? 1.0 : 0.0,
			i % 2 ? 0.0 : 1.0);
		lMaterial->Diffuse.Set(lColor);
		lMaterial->TransparencyFactor.Set(0.0);
		lMaterial->ShadingModel.Set(lShadingName);
		lMaterial->Shininess.Set(0.5);

		//get the node of mesh, add material for it.
		FbxNode* lNode = pMesh->GetNode();
		if (lNode)
			lNode->AddMaterial(lMaterial);
	}
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteMesh(FbxScene*& pScene, const BFRESStructs::FSHP& fshp, const BFRESStructs::LODMesh& lodMesh, std::vector<BoneMetadata>& boneInfoList)
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
    FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "_p0");
    FbxLayerElementUV* lLayerElementUV0 = FbxLayerElementUV::Create(lMesh, "_uv0");
    FbxLayerElementTangent* lLayerElementTangent = FbxLayerElementTangent::Create(lMesh, "_t0");
    FbxLayerElementBinormal* lLayerElementBinormal = FbxLayerElementBinormal::Create(lMesh, "_b0");

    // Set its mapping mode to map each normal vector to each control point.
    lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementUV0->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementTangent->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementBinormal->SetMappingMode(FbxLayerElement::eByControlPoint);


    // Set the reference mode of so that the n'th element of the normal array maps to the n'th
    // element of the control point array.
    lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementUV0->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementTangent->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementBinormal->SetReferenceMode(FbxLayerElement::eDirect);

    //std::vector<SkinCluster> vSkinClusters((*m_pBfres).fmdl[1].fskl.boneList.size());
    std::map<uint32, SkinCluster> BoneIndexToSkinClusterMap;

    for (uint32 i = 0; i < uiNumControlPoints; i++)
    {
        // TODO make this iterative
        const Math::vector3F& posVec = fshp.vertices[i].position0;
        lControlPoints[i] = FbxVector4(posVec.X, posVec.Y, posVec.Z);

        const Math::vector3F& normalVec = fshp.vertices[i].normal;
        lLayerElementNormal->GetDirectArray().Add(FbxVector4(normalVec.X, normalVec.Y, normalVec.Z));

#if FLIP_UV_VERTICAL
        const Math::vector2F& uv0Vec = fshp.vertices[i].uv0;
        lLayerElementUV0->GetDirectArray().Add(FbxVector2(uv0Vec.X, 1 - uv0Vec.Y));
#else 
		const Math::vector2F& uv0Vec = fshp.vertices[i].uv0;
		lLayerElementUV0->GetDirectArray().Add(FbxVector2(uv0Vec.X, uv0Vec.Y));
#endif

        const Math::vector4F& tangentVec = fshp.vertices[i].tangent;
        lLayerElementTangent->GetDirectArray().Add(FbxVector4(tangentVec.X, tangentVec.Y, tangentVec.Z, tangentVec.W));

        const Math::vector4F& binormalVec = fshp.vertices[i].binormal;
        lLayerElementBinormal->GetDirectArray().Add(FbxVector4(binormalVec.X, binormalVec.Y, binormalVec.Z, binormalVec.W));

        CreateSkinClusterData(fshp.vertices[i], i, BoneIndexToSkinClusterMap, boneInfoList, fshp);  // Convert the vertex-to-bone mapping to bone-to-vertex so it conforms with fbx cluster data
    }

    WriteSkin(pScene, lMesh, BoneIndexToSkinClusterMap);

    // Create layer 0 for the mesh if it does not already exist.
    // This is where we will define our normals.
    FbxLayer* lLayer = lMesh->GetLayer(0);
    if (lLayer == NULL) {
        lMesh->CreateLayer();
        lLayer = lMesh->GetLayer(0);
    }
    lLayer->SetNormals(lLayerElementNormal);
    lLayer->SetUVs(lLayerElementUV0);
    lLayer->SetTangents(lLayerElementTangent);
    lLayer->SetBinormals(lLayerElementBinormal);

    // Define which control points belong to a poly
    uint32 uiPolySize(3);
    uint32 uiNumFaces(0);
    // TODO make this iterative
    for (uint32 i = 0; i < lodMesh.faceVertices.size(); ++i)
    {
        // first index
        if ((i % uiPolySize) == 0)
        {
            lMesh->BeginPolygon();
            uiNumFaces++;
        }

        // TODO make this iterative
        lMesh->AddPolygon(lodMesh.faceVertices[i]);

        // last index
        if ((i + 1) % uiPolySize == 0)
            lMesh->EndPolygon();
    }

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eDirect);

	FbxString lMaterialName = "M_";
    lMaterialName += fshp.name.c_str();
    FbxSurfaceMaterial* lMaterial = FbxSurfaceMaterial::Create(pScene, lMaterialName);
	//get the node of mesh, add material for it.
	FbxNode* lNode = lMesh->GetNode();
	if (lNode)
		lNode->AddMaterial(lMaterial);

    // TODO move this function call into write animations
    WriteBindPose(pScene, lMeshNode);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap)
{
    FbxSkin* pSkin = FbxSkin::Create(pScene, "");
    FbxAMatrix& lXMatrix = pMesh->GetNode()->EvaluateGlobalTransform();
    const BFRESStructs::FSKL& fskl = (*m_pBfres).fmdl[1].fskl;

    std::map<uint32, SkinCluster>::iterator iter = BoneIndexToSkinClusterMap.begin();
    std::map<uint32, SkinCluster>::iterator end = BoneIndexToSkinClusterMap.end();


    for (; iter != end; ++iter)
    {
        SkinCluster& skinCluster = iter->second;
        if (skinCluster.m_vControlPointIndices.size() == 0)	        
            std::string boneName = fskl.bones[iter->first].name;

        FbxNode* pBoneNode = pScene->FindNodeByName(FbxString(skinCluster.m_szName.c_str()));
        assert(pBoneNode != NULL);

        FbxCluster* pCluster = FbxCluster::Create(pScene, skinCluster.m_szName.c_str());
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
// Add the specified node to the node array. Also, add recursively
// all the parent node of the specified node to the array.
void AddNodeRecursively(FbxArray<FbxNode*>& pNodeArray, FbxNode* pNode)
{
    if (pNode)
    {
        AddNodeRecursively(pNodeArray, pNode->GetParent());

        if (pNodeArray.Find(pNode) == -1)
        {
            // Node not in the list, add it
            pNodeArray.Add(pNode);
        }
    }
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteBindPose(FbxScene*& pScene, FbxNode*& pMeshNode)
{
    // In the bind pose, we must store all the link's global matrix at the time of the bind.
// Plus, we must store all the parent(s) global matrix of a link, even if they are not
// themselves deforming any model.

// In this example, since there is only one model deformed, we don't need walk through 
// the scene
//

// Now list the all the link involve in the patch deformation
    FbxArray<FbxNode*> lClusteredFbxNodes;
    int                       i, j;

    if (pMeshNode && pMeshNode->GetNodeAttribute())
    {
        int lSkinCount = 0;
        int lClusterCount = 0;


        lSkinCount = ((FbxGeometry*)pMeshNode->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
        //Go through all the skins and count them
        //then go through each skin and get their cluster count
        for (i = 0; i < lSkinCount; ++i)
        {
            FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)pMeshNode->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
            lClusterCount += lSkin->GetClusterCount();
        }

        //if we found some clusters we must add the node
        if (lClusterCount)
        {
            //Again, go through all the skins get each cluster link and add them
            for (i = 0; i < lSkinCount; ++i)
            {
                FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)pMeshNode->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
                lClusterCount = lSkin->GetClusterCount();
                for (j = 0; j < lClusterCount; ++j)
                {
                    FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
                    AddNodeRecursively(lClusteredFbxNodes, lClusterNode);
                }

            }

            // Add the patch to the pose
            lClusteredFbxNodes.Add(pMeshNode);
        }
    }

    // Now create a bind pose with the link list
    if (lClusteredFbxNodes.GetCount())
    {
        // A pose must be named. Arbitrarily use the name of the patch node.
        FbxPose* lPose = FbxPose::Create(pScene, pMeshNode->GetName());

        // default pose type is rest pose, so we need to set the type as bind pose
        lPose->SetIsBindPose(true);

        for (i = 0; i < lClusteredFbxNodes.GetCount(); i++)
        {
            FbxNode* lKFbxNode = lClusteredFbxNodes.GetAt(i);
            FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();

            lPose->Add(lKFbxNode, lBindMatrix);
        }

        // Add the pose to the scene
        pScene->AddPose(lPose);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::CreateSkinClusterData(const BFRESStructs::FVTX& vert, uint32 uiVertIndex, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap, std::vector<BoneMetadata>& boneListInfos, const BFRESStructs::FSHP& fshp)
{
    uint32 uiBlendIndices[4] = { vert.blendIndex.X, vert.blendIndex.Y, vert.blendIndex.Z, vert.blendIndex.W };
    float  fBlendWeights[4] = { vert.blendWeights.X, vert.blendWeights.Y, vert.blendWeights.Z, vert.blendWeights.W };

	if (boneListInfos[uiBlendIndices[0]].eSkinningType == SkinningType::eRigid)
    {
        std::map<uint32, SkinCluster>::iterator iter = BoneIndexToSkinClusterMap.find(boneListInfos[uiBlendIndices[0]].uiBoneIndex); // Try to find a skin cluster with the given bone index

        if (iter != BoneIndexToSkinClusterMap.end()) // If found a skin cluster with the given bone index
        {
            iter->second.m_vControlPointIndices.push_back(uiVertIndex);
            iter->second.m_vControlPointWeights.push_back(1);
        }
        else // Create new cluster with data
        {
            SkinCluster skinCluster;
            skinCluster.m_szName = boneListInfos[uiBlendIndices[0]].szName;
            skinCluster.m_vControlPointIndices.push_back(uiVertIndex);
            skinCluster.m_vControlPointWeights.push_back(1);
            BoneIndexToSkinClusterMap.insert(std::pair<uint32, SkinCluster>(boneListInfos[uiBlendIndices[0]].uiBoneIndex, skinCluster));
        }
    }
    else if (boneListInfos[uiBlendIndices[0]].eSkinningType == SkinningType::eSmooth)
    {
        for (uint32 uiBlendEntry = 0; uiBlendEntry < fshp.vertexSkinCount; ++uiBlendEntry)
        {
            if (fBlendWeights[uiBlendEntry] > 0) // Only write blend weights that are non-zero and that have not already had weights written for this bone index
            {
                std::map<uint32, SkinCluster>::iterator iter = BoneIndexToSkinClusterMap.find(boneListInfos[uiBlendIndices[uiBlendEntry]].uiBoneIndex); // Try to find a skin cluster with the given bone index

                if (iter != BoneIndexToSkinClusterMap.end()) // If found a skin cluster with the given bone index
                {
                    iter->second.m_vControlPointIndices.push_back(uiVertIndex);
                    iter->second.m_vControlPointWeights.push_back(fBlendWeights[uiBlendEntry]);
                }
                else // Create new cluster with data
                {
                    SkinCluster skinCluster;
                    skinCluster.m_szName = boneListInfos[uiBlendIndices[uiBlendEntry]].szName;
                    skinCluster.m_vControlPointIndices.push_back(uiVertIndex);
                    skinCluster.m_vControlPointWeights.push_back(fBlendWeights[uiBlendEntry]);
                    BoneIndexToSkinClusterMap.insert(std::pair<uint32, SkinCluster>(boneListInfos[uiBlendIndices[uiBlendEntry]].uiBoneIndex, skinCluster));
                }
            }
        }
    }
}
