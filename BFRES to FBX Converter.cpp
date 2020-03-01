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
    lImporter->Initialize( "TestAssets/QueenModelASCII.fbx", -1, ios );
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
            std::cout << "Attribute type: " << attributeType << "\n";
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
                    std::cout << "Attribute type: " << attributeType << "\n";
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
                }
            }
        }
        std::cout << "\n";
    }
    return 0;
}
