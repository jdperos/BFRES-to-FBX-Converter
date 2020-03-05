#include "FBXWriter.h"
#include <iostream>
#include "ConsoleColor.h"
#include "Primitives.h"
#include "assert.h"
#include "Globals.h"



FBXWriter::FBXWriter()
{
}

FBXWriter::~FBXWriter()
{
}

void FBXWriter::CreateFBX(FbxScene*& pScene, const BFRES& bfres)
{
    for (uint32 i = 0; i < bfres.fmdl.size(); i++)
    {
        WriteModel(pScene, bfres.fmdl[i]);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteModel(FbxScene*& pScene, const FMDL& fmdl)
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
void FBXWriter::WriteSkeleton(FbxScene*& pScene, const FSKL& fskl, std::vector<BoneMetadata>& boneInfoList)
{
    const uint32 uiTotalBones = fskl.bones.size();
    std::vector<FbxNode*> boneNodes(uiTotalBones);

    for (int32 i = 0; i < uiTotalBones; i++)
        CreateBone(pScene, fskl.bones[i], boneNodes[i], boneInfoList);

    for (int32 i = 0; i < uiTotalBones; i++)
    {
        const Bone& bone = fskl.bones[i];
        if (bone.parentIndex >= 0)
            boneNodes[bone.parentIndex]->AddChild(boneNodes[i]);
        else
            pScene->GetRootNode()->AddChild(boneNodes[i]);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::CreateBone(FbxScene*& pScene, const Bone& bone, FbxNode*& lBoneNode, std::vector<BoneMetadata>& boneInfoList)
{
    // Create a node for our mesh in the scene.
    lBoneNode = FbxNode::Create(pScene, bone.name.c_str());

    // Set transform data
    FbxDouble3 fBoneScale = FbxDouble3(bone.scale.X, bone.scale.Y, bone.scale.Z);
    lBoneNode->LclScaling.Set(fBoneScale);
    if (bone.rotationType == RotationType::EulerXYZ)
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
    if (bone.parentIndex == -1)
    {
        lBone->SetSkeletonType(FbxSkeleton::eRoot);
    }
    else
    {
        lBone->SetSkeletonType(FbxSkeleton::eLimbNode);
    }

    lBone->Size.Set( 0.03 );
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
void FBXWriter::WriteShape(FbxScene*& pScene, const FSHP& fshp, std::vector<BoneMetadata>& boneInfoList)
{
    std::string meshName = fshp.name + "_LODGroup";
    FbxNode* lLodGroup = FbxNode::Create( pScene, meshName.c_str() );
    FbxLODGroup* lLodGroupAttr = FbxLODGroup::Create( pScene, meshName.c_str() );
    // Array lChildNodes contains geometries of all LOD levels
    for( int j = 0; j < fshp.lodMeshes.size(); j++ )
    {
        WriteMesh(pScene, lLodGroup, fshp, fshp.lodMeshes[j], boneInfoList);
    }
    pScene->GetRootNode()->AddChild( lLodGroup );
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteMesh(FbxScene*& pScene, FbxNode*& pLodGroup, const FSHP& fshp, const LODMesh& lodMesh, std::vector<BoneMetadata>& boneInfoList)
{
    bool hasSkeleton = boneInfoList.size() > 0;

    std::string meshName = fshp.name;
    meshName += "_LOD" + std::to_string( pLodGroup->GetChildCount() );
    
    // Create a node for our mesh in the scene.
    FbxNode* lMeshNode = FbxNode::Create(pScene, meshName.c_str());

    // Create a mesh.
    FbxMesh* lMesh = FbxMesh::Create(pScene, meshName.c_str());

    // Set the node attribute of the mesh node.
    lMeshNode->SetNodeAttribute(lMesh);

    // Add the mesh node to the root node in the scene.
    pLodGroup->AddChild(lMeshNode);

    // Initialize the control point array of the mesh.
    uint32 uiNumControlPoints(fshp.vertices.size());
    lMesh->InitControlPoints(uiNumControlPoints);
    FbxVector4* lControlPoints = lMesh->GetControlPoints();

    //Create layer elements and set mapping modes and reference modes
    FbxLayerElementNormal*   lLayerElementNormal   = FbxLayerElementNormal  ::Create(lMesh, "_n0" );
    FbxLayerElementUV*       lLayerElementUV0      = FbxLayerElementUV      ::Create(lMesh, "_uv0");
    FbxLayerElementUV*       lLayerElementUV1      = FbxLayerElementUV      ::Create(lMesh, "_uv1");
    FbxLayerElementUV*       lLayerElementUV2      = FbxLayerElementUV      ::Create(lMesh, "_uv2");
    FbxLayerElementTangent*  lLayerElementTangent  = FbxLayerElementTangent ::Create(lMesh, "_t0" );
    FbxLayerElementBinormal* lLayerElementBinormal = FbxLayerElementBinormal::Create(lMesh, "_b0" );

    lLayerElementNormal  ->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementUV0     ->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementUV1     ->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementUV2     ->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementTangent ->SetMappingMode(FbxLayerElement::eByControlPoint);
    lLayerElementBinormal->SetMappingMode(FbxLayerElement::eByControlPoint);

    lLayerElementNormal  ->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementUV0     ->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementUV1     ->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementUV2     ->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementTangent ->SetReferenceMode(FbxLayerElement::eDirect);
    lLayerElementBinormal->SetReferenceMode(FbxLayerElement::eDirect);

    std::map<uint32, SkinCluster> SkinClusterMap;

    for (uint32 i = 0; i < uiNumControlPoints; i++)
    {
        const Math::vector3F& posVec = fshp.vertices[i].position0;
        lControlPoints[i] = FbxVector4(posVec.X, posVec.Y, posVec.Z);

        const Math::vector3F& normalVec = fshp.vertices[i].normal;
        lLayerElementNormal->GetDirectArray().Add(FbxVector4(normalVec.X, normalVec.Y, normalVec.Z));

#if FLIP_UV_VERTICAL
        const Math::vector2F& uv0Vec = fshp.vertices[i].uv0;
        lLayerElementUV0->GetDirectArray().Add(FbxVector2(uv0Vec.X, 1 - uv0Vec.Y));
		const Math::vector2F& uv1Vec = fshp.vertices[i].uv1;
		lLayerElementUV1->GetDirectArray().Add(FbxVector2(uv1Vec.X, 1 - uv1Vec.Y));
		const Math::vector2F& uv2Vec = fshp.vertices[i].uv2;
		lLayerElementUV2->GetDirectArray().Add(FbxVector2(uv2Vec.X, 1 - uv2Vec.Y));
#else 
		const Math::vector2F& uv0Vec = fshp.vertices[i].uv0;
		lLayerElementUV0->GetDirectArray().Add(FbxVector2(uv0Vec.X, uv0Vec.Y));
        const Math::vector2F& uv1Vec = fshp.vertices[i].uv1;
		lLayerElementUV1->GetDirectArray().Add(FbxVector2(uv1Vec.X, uv1Vec.Y));
		const Math::vector2F& uv2Vec = fshp.vertices[i].uv2;
		lLayerElementUV2->GetDirectArray().Add(FbxVector2(uv2Vec.X, uv2Vec.Y));
#endif

        const Math::vector4F& tangentVec = fshp.vertices[i].tangent;
        lLayerElementTangent->GetDirectArray().Add(FbxVector4(tangentVec.X, tangentVec.Y, tangentVec.Z, tangentVec.W));

        const Math::vector4F& binormalVec = fshp.vertices[i].binormal;
        lLayerElementBinormal->GetDirectArray().Add(FbxVector4(binormalVec.X, binormalVec.Y, binormalVec.Z, binormalVec.W));
        
        if (hasSkeleton)
            CreateSkinClusterData(fshp.vertices[i], i, SkinClusterMap, boneInfoList, fshp);  // Convert the vertex-to-bone mapping to bone-to-vertex so it conforms with fbx cluster data
    }

    if (hasSkeleton)
        WriteSkin(pScene, lMesh, SkinClusterMap);

    // Create layer 0 for the mesh if it does not already exist.
    // This is where we will define our normals.
    FbxLayer* lLayer00 = lMesh->GetLayer(0);
    if (lLayer00 == NULL) {
        lMesh->CreateLayer();
        lLayer00 = lMesh->GetLayer(0);
    }
    lLayer00->SetNormals(lLayerElementNormal);
    lLayer00->SetUVs(lLayerElementUV0, FbxLayerElement::eTextureDiffuse);
    lLayer00->SetUVs(lLayerElementUV1, FbxLayerElement::eTextureNormalMap);
    lLayer00->SetUVs(lLayerElementUV2, FbxLayerElement::eTextureTransparency); // We dont know what this is used for
    lLayer00->SetTangents(lLayerElementTangent);
    lLayer00->SetBinormals(lLayerElementBinormal);

    MapFacesToVertices(lodMesh, lMesh);

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eDirect);

	FbxString lMaterialName = "M_";
    lMaterialName += fshp.name.c_str();
    FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(pScene, lMaterialName);
	lMeshNode->AddMaterial(lMaterial);
    
    lMeshNode->SetShadingMode(FbxNode::eTextureShading);

#if WRITE_TEXTURES // Currently it appears that FBX doesn't support AO maps
    SetTexturesToMaterial(pScene, fshp, lMaterial, lLayerElementUV0, lLayerElementUV1, lLayerElementUV2);
#endif // WRITE_TEXTURES

    // TODO move this function call into write animations
    WriteBindPose(pScene, lMeshNode);
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// Currently as far as it will get. Certain things, like AO maps, are not
// supported by FBX's Phong material as far as I can tell.
void FBXWriter::SetTexturesToMaterial(FbxScene*& pScene, const FSHP& fshp, FbxSurfacePhong* lMaterial, FbxLayerElementUV* lLayerElementUV0, FbxLayerElementUV* lLayerElementUV1, FbxLayerElementUV* lLayerElementUV2)
{
    // Get Material used for this mesh
    FMAT* fmat = g_BFRESManager.GetMaterialByIndex(fshp.modelIndex, fshp.materialIndex);

    for (uint32 i = 0; i < fmat->textureRefs.textureCount; i++)
    {
        TextureRef& tex = fmat->textureRefs.textures[i];
        GX2TextureMapType type = tex.type;
        FbxTexture::ETextureUse textureUse;
        FbxString uvLayerName;

        std::string& textureName = g_BFRESManager.GetTextureFromMaterialByType(fmat, type)->name;
		FbxFileTexture* lTexture = FbxFileTexture::Create(pScene, textureName.c_str());

        switch (type)
        {
        case GX2TextureMapType::Albedo:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV0->GetName();
            lMaterial->Diffuse.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::Normal:
            textureUse = FbxTexture::ETextureUse::eBumpNormalMap;
            uvLayerName = lLayerElementUV1->GetName();
            lMaterial->NormalMap.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::Specular:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            lMaterial->Specular.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::AmbientOcclusion:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            lMaterial->Ambient.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::Emission:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            lMaterial->Emissive.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::Shadow:
            textureUse = FbxTexture::ETextureUse::eShadowMap;
            uvLayerName = lLayerElementUV1->GetName();
            lMaterial->Diffuse.ConnectSrcObject(lTexture);
            break;
        case GX2TextureMapType::Light:
            textureUse = FbxTexture::ETextureUse::eLightMap;
            uvLayerName = lLayerElementUV1->GetName();
            break;
        case GX2TextureMapType::MRA:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            break;
        case GX2TextureMapType::Metalness:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            break;
        case GX2TextureMapType::Roughness:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            break;
        case GX2TextureMapType::SubSurfaceScattering:
            textureUse = FbxTexture::ETextureUse::eStandard;
            uvLayerName = lLayerElementUV1->GetName();
            break;
        default:
            break;
        }

        std::string filePath = MEDIAN_FILE_DIR + textureName + ".tga";
		lTexture->SetFileName(filePath.c_str());
		lTexture->SetTextureUse(textureUse);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		lTexture->SetSwapUV(false);
		lTexture->SetTranslation(0.0, 0.0);
		lTexture->SetScale(1.0, 1.0);
		lTexture->SetRotation(0.0, 0.0);
		lTexture->UVSet.Set(uvLayerName); // Connect texture to the proper UV
			
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::MapFacesToVertices(const LODMesh& lodMesh, FbxMesh* lMesh)
{
    // Define which control points belong to a poly
    uint32 uiPolySize(3);
    // TODO make this iterative
    for (uint32 i = 0; i < lodMesh.faceVertices.size(); ++i)
    {
        // first index
        if ((i % uiPolySize) == 0)
        {
            lMesh->BeginPolygon();
        }

        // TODO make this iterative
        lMesh->AddPolygon(lodMesh.faceVertices[i]);

        // last index
        if ((i + 1) % uiPolySize == 0)
            lMesh->EndPolygon();
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void FBXWriter::WriteSkin(FbxScene*& pScene, FbxMesh*& pMesh, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap)
{
    FbxSkin* pSkin = FbxSkin::Create(pScene, pMesh->GetNode()->GetName());
    FbxAMatrix& lXMatrix = pMesh->GetNode()->EvaluateGlobalTransform();
    const FSKL& fskl = *g_BFRESManager.GetSkeletonByModelIndex(1); // TODO do not hardcode this value! HACK HACK HACK HACK

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
void FBXWriter::CreateSkinClusterData(const FVTX& vert, uint32 uiVertIndex, std::map<uint32, SkinCluster>& BoneIndexToSkinClusterMap, std::vector<BoneMetadata>& boneListInfos, const FSHP& fshp)
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
