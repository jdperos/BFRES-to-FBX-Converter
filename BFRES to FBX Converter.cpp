#include <iostream>
#include <fbxsdk.h>
#include "MyFBXCube.h"
#include "FBXWriter.h"
#include "XmlParser.h"
#include "BFRES.h"
#include <windows.h>

#ifdef IOS_REF
    #undef  IOS_REF
    #define IOS_REF (*(pManager->GetIOSettings()))
#endif

#define MEDIAN_FILE_DIR "MedianDumps/"
#define OUTPUT_FILE_DIR "FBXExports/"

#define PRINT_SKELETON_INFO false
#define PRINT_MESH_LAYER_INFO true

// ---------------------------------------------
// ---------------------------------------------
void PrintMeshLayerInfo(FbxMesh* inMesh)
{
    std::cout << "Layer count " << inMesh->GetLayerCount() << "\n";
    for (uint32 i = 0; i < inMesh->GetLayerCount(); i++)
    {
        FbxLayer* layer = inMesh->GetLayer(i);
        std::cout << "Layer " << i << ": \n";
        std::cout << "UV count: " << layer->GetUVSetCount() << "\n";
        if (layer->GetNormals())
            std::cout << "Has Normals\n";
        if (layer->GetTangents())
            std::cout << "Has Tangents\n";
        if (layer->GetBinormals())
            std::cout << "Has Binormals\n";
        if (layer->GetMaterials())
        {
            std::cout << "Has Materials\n";
            FbxLayerElement::EMappingMode mappingMode = layer->GetMaterials()->GetMappingMode();
            if (mappingMode)
            {
                std::cout << "Mapping mode: ";
                switch (mappingMode)
                {
                case fbxsdk::FbxLayerElement::eNone:
                    std::cout << "eNone";
                    break;
                case fbxsdk::FbxLayerElement::eByControlPoint:
                    std::cout << "eByControlPoint";
                    break;
                case fbxsdk::FbxLayerElement::eByPolygonVertex:
                    std::cout << "eByPolygonVertex";
                    break;
                case fbxsdk::FbxLayerElement::eByPolygon:
                    std::cout << "eByPolygon";
                    break;
                case fbxsdk::FbxLayerElement::eByEdge:
                    std::cout << "eByEdge";
                    break;
                case fbxsdk::FbxLayerElement::eAllSame:
                    std::cout << "eAllSame";
                    break;
                default:
                    break;
                }
                std::cout << "\n";
            }
            FbxLayerElement::EReferenceMode referenceMode = layer->GetMaterials()->GetReferenceMode();
            if (referenceMode)
            {
				std::cout << "Reference mode: ";
				switch (referenceMode)
				{
				case fbxsdk::FbxLayerElement::eDirect:
					std::cout << "eDirect";
					break;
				case fbxsdk::FbxLayerElement::eIndex:
					std::cout << "eIndex";
					break;
				case fbxsdk::FbxLayerElement::eIndexToDirect:
					std::cout << "eIndexToDirect";
					break;
				default:
					break;
				}
				std::cout << "\n";
            }
        }
        std::cout << "UVs: " << layer->GetUVs() << "\n";
    }
}


// ---------------------------------------------
// ---------------------------------------------
void PrintMeshDeformerInfo(FbxMesh* inMesh)
{
    // Get Number of Deformers
    uint32 numOfDeformers = inMesh->GetDeformerCount();
    std::cout << "Deformer count: " << numOfDeformers << "\n";

    // Loop through Node Deformers for Skins to get Cluster Data
    for (uint32 deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
    {
        // Check if Deformer is a Skin
        FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(inMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
        if (!currSkin)
        {
            continue;
        }

        std::cout << "Skin: " << currSkin->GetName() << "\n";

        // Get Number of Clusters in Skin
        uint32 numOfClusters = currSkin->GetClusterCount();

        // Process Cluster Data
        for (uint32 clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
        {
            // Get Cluster from Skin
            FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);

            // Get Joint Name
            FbxNode* currJoint = currCluster->GetLink();
            std::cout << "Joint Begin: " << currJoint->GetName() << "\n";

            uint32 numOfIndices = currCluster->GetControlPointIndicesCount();
            std::cout << "Control point indices count: " << numOfIndices << "\n";

            int* controlPointIndices = currCluster->GetControlPointIndices();
            double* controlPointWeights = currCluster->GetControlPointWeights();
            for (uint32 idxCount = 0; idxCount < numOfIndices; ++idxCount)
            {
                std::cout << "Control point index : " << controlPointIndices[idxCount] << "\n";
                std::cout << "Control point weight: " << controlPointWeights[idxCount] << "\n";
            }

            std::cout << "Joint End: " << currJoint->GetName() << "\n";

        }
    }
}


// ---------------------------------------------
// ---------------------------------------------
void PrintMesh(FbxNode* currNode)
{
	FbxMesh* inMesh = currNode->GetMesh();
	// Node Geomtric Transformation
	FbxAMatrix geometryTransform = FbxAMatrix(currNode->GetGeometricTranslation(FbxNode::eSourcePivot), currNode->GetGeometricRotation(FbxNode::eSourcePivot), currNode->GetGeometricScaling(FbxNode::eSourcePivot));
    //std::cout << "Mesh Name " << inMesh->GetName() << "\n";
#if PRINT_MESH_LAYER_INFO
    PrintMeshLayerInfo(inMesh);
#endif
#if PRINT_SKELETON_INFO
    PrintMeshDeformerInfo(inMesh);
#endif
}


// ---------------------------------------------
// ---------------------------------------------
void PrintAnimLayerMembersAndChannels(FbxAnimLayer* lAnimLayer)
{
    std::cout << "Anim layer Member count: " << lAnimLayer->GetMemberCount() << "\n";
    for (uint32 i = 0; i < lAnimLayer->GetMemberCount(); ++i)
    {
        FbxAnimCurveNode* pCurveNode = (FbxAnimCurveNode*)lAnimLayer->GetMember(i);
        std::cout << "Curve Node " << i << " name: " << pCurveNode->GetName() << "\n";
        std::cout << "Curve Node " << i << " Channels: " << pCurveNode->GetChannelsCount() << "\n";
        for (uint32 j = 0; j < pCurveNode->GetChannelsCount(); j++)
        {
            std::cout << "Channel " << j << " name: " << pCurveNode->GetChannelName(j) << "\n";
        }
        std::cout << "\n";
    }
}



// ---------------------------------------------
// ---------------------------------------------
int main()
{
    // create a SdkManager
    FbxManager* lSdkManager = FbxManager::Create();
    // create an IOSettings object
    FbxIOSettings* ios = FbxIOSettings::Create( lSdkManager, IOSROOT );
    // set some IOSettings options 
    ios->SetBoolProp( IMP_FBX_MATERIAL, true );
    ios->SetBoolProp( IMP_FBX_TEXTURE, true );
    // create an empty scene
    FbxScene* pScene = FbxScene::Create( lSdkManager, "" );
    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create( lSdkManager, "" );
    // Initialize the importer by providing a filename and the IOSettings to use
    lImporter->Initialize( "TestAssets/TwoAnimations.fbx", -1, ios );
    // Import the scene.
    lImporter->Import( pScene );
    // Destroy the importer.
    lImporter->Destroy();

    std::cout << pScene->GetMemberCount() << "\n\n";

    int sceneNodes = pScene->GetNodeCount();
    std::cout << "Scene Nodes: " << sceneNodes << "\n";
    for( uint32 i = 0; i < pScene->GetNodeCount(); i++ )
    {
        std::cout << "Name: " << pScene->GetNode( i )->GetName() << "\n";
        if( pScene->GetNode( i )->GetNodeAttribute() )
        {
            std::string attributeType;
            switch( pScene->GetNode( i )->GetNodeAttribute()->GetAttributeType() )
            {
            case 3:
                attributeType = "Skeleton";
                break;
            case 4:
                attributeType = "Mesh";
				PrintMesh(pScene->GetNode(i));
                break;
            default:
                break;
            }
            std::cout << "Attribute type: " << attributeType << "\n";
        }

        if( pScene->GetNode( i )->GetParent() )
            std::cout << "Parent: " << pScene->GetNode( i )->GetParent()->GetName() << "\n";

        for( uint32 j = 0; j < pScene->GetNode( i )->GetChildCount(); j++ )
        {
            if (pScene->GetNode(i)->GetChild(j))
            {
                FbxNode* childNode = pScene->GetNode(i)->GetChild(j);
                std::cout << "\nChild: " << childNode->GetName() << "\n";
                if (childNode->GetNodeAttribute())
                {
                    std::string attributeType;
                    switch (childNode->GetNodeAttribute()->GetAttributeType())
                    {
                    case 3:
                        attributeType = "Skeleton";
                        break;
                    case 4:
                        attributeType = "Mesh";
                        PrintMesh(childNode);
                        break;
                    default:
                        break;
                    }
                    std::cout << "Attribute type: " << attributeType << "\n";
                }
            }
        }
        std::cout << "\n";

        
    }
    
    FbxAnimStack* animStack = pScene->GetCurrentAnimationStack();
    FbxArray< FbxString* > stringArray;
    pScene->FillAnimStackNameArray(stringArray);

    // An anim stack seems to correlate with an animation
    std::cout << "Anim stack name: " << animStack->GetName() << "\n";
    // It seems to have 1 member, an Anim Layer
    std::cout << "Anim stack member count: " << animStack->GetMemberCount() << "\n";
    // The anim layer is named the same thing as the anim stack - one per animation
    FbxAnimLayer* lAnimLayer = (FbxAnimLayer*) animStack->GetMember(0);
    std::cout << "Member name: " << lAnimLayer->GetName() << "\n";

    // The anim layer has 405 members, each a curve node named 'T', 'R', or 'S'.
    // 405 divided by 3 is 135, which is the amount of bones in the skeleton.
    
    // PrintAnimLayerMembersAndChannels(lAnimLayer);

    std::cout << "Anim layer Member count: " << lAnimLayer->GetMemberCount() << "\n";
    uint32 cBones = (lAnimLayer->GetMemberCount() / 3);
    for (uint32 i = 0; i < cBones; i++)
    {
        std::cout << "Bone index: " << i << "\n";
        FbxAnimCurveNode* pCurveNode = (FbxAnimCurveNode*)lAnimLayer->GetMember(i * 3);
        std::cout << "Curve Node " << (i * 3) << " name: " << pCurveNode->GetName() << "\n";
        for (uint32 j = 0; j < pCurveNode->GetChannelsCount(); j++)
        {
            std::cout << "Channel " << j << " name: " << pCurveNode->GetChannelName(j) << "\n";
        }

        pCurveNode = (FbxAnimCurveNode*)lAnimLayer->GetMember((i * 3) + 1);
        std::cout << "Curve Node " << ((i * 3) + 1) << " name: " << pCurveNode->GetName() << "\n";
        for (uint32 j = 0; j < pCurveNode->GetChannelsCount(); j++)
        {
            FbxAnimCurve* lCurve = pCurveNode->GetCurve(j);
            std::cout << "Channel " << j << " name: " << lCurve->GetName() << "\n";
            std::cout << "Channel " << j << " Key count: " << lCurve->KeyGetCount() << "\n";
            //std::cout << "Channel " << j << " name: " << pCurveNode->GetChannelName(j) << "\n";
        }

        pCurveNode = (FbxAnimCurveNode*)lAnimLayer->GetMember((i * 3) + 2);
        std::cout << "Curve Node " << ((i * 3) + 2) << " name: " << pCurveNode->GetName() << "\n";
        for (uint32 j = 0; j < pCurveNode->GetChannelsCount(); j++)
        {
            //FbxAnimCurve* lCurve = pCurveNode->GetCurve(j);
            //std::cout << "Channel " << j << " name: " << lCurve->GetName() << "\n";
            //std::cout << "Channel " << j << " Key count: " << lCurve->KeyGetCount() << "\n";
            std::cout << "Channel " << j << " name: " << pCurveNode->GetChannelName(j) << "\n";
        }
        std::cout << "\n";
    }
    
    
    return 0;
}
