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

// Export document, the format is ascii by default
bool SaveDocument(FbxManager* pManager, FbxDocument* pDocument, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount())
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

            for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
            {
                if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                {
                    FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                    const char* lASCII = "ascii";
                    if (lDesc.Find(lASCII) >= 0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    //IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
    //IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
    //IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
    //IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
    //IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if (lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    FBXSDK_printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pDocument);

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

void PrintMesh(FbxNode* currNode)
{
	FbxMesh* inMesh = currNode->GetMesh();
	// Node Geomtric Transformation
	FbxAMatrix geometryTransform = FbxAMatrix(currNode->GetGeometricTranslation(FbxNode::eSourcePivot), currNode->GetGeometricRotation(FbxNode::eSourcePivot), currNode->GetGeometricScaling(FbxNode::eSourcePivot));

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

			int*    controlPointIndices = currCluster->GetControlPointIndices();
			double* controlPointWeights = currCluster->GetControlPointWeights();
			for (uint32 idxCount = 0; idxCount < numOfIndices; ++idxCount)
			{
				std::cout << "Control point index : " << controlPointIndices[idxCount] << "\n";
				std::cout << "Control point weight: " << controlPointWeights[idxCount] << "\n";
			}

			std::cout << "Joint End: " << currJoint->GetName() << "\n";
			//// Get Joint and Joint Index from Name
			//ModelObject::Joint* joint = nullptr;
			//uint32 jointIdx = FindJointIndexUsingName(currJointName, &joint);
			
			//// Matrices
			//FbxAMatrix transformMatrix;					// Mesh Tranformation at Binding Time
			//FbxAMatrix transformLinkMatrix;				// The Transformation of the Cluster(joint) at Binding Time from Joint Space to World Space
			//FbxAMatrix globalBindposeInverseMatrix;		// Bindpose Inverse Matrix

			//// Get Matrix Data
			//currCluster->GetTransformMatrix(transformMatrix);
			//currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			//globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			//// Update Joint Matrix Data
			//joint->globalBindposeInverse = FbxAMatrixToMatrix44(globalBindposeInverseMatrix);

			//// Associate Each Joint with the Control Points it Affects
			//uint32 numOfIndices = currCluster->GetControlPointIndicesCount();
			//for (uint32 idxCount = 0; idxCount < numOfIndices; ++idxCount)
			//{
			//	ModelObject::BlendingIndexWeightPair currBlendingIndexWeightPair;
			//	currBlendingIndexWeightPair.blendingIndex = jointIdx;
			//	currBlendingIndexWeightPair.blendingWeight = currCluster->GetControlPointWeights()[idxCount];

			//	// If Blending Weight is Above Minimum Value
			//	if (currBlendingIndexWeightPair.blendingWeight > 0.1f)
			//	{
			//		// Get Control Index
			//		int ctrlIdx = currCluster->GetControlPointIndices()[idxCount];

			//		// Get Model Blend Data Index
			//		int blendIdx = 0;
			//		for (blendIdx = 0; blendIdx < 4; blendIdx++)
			//		{
			//			if (m_meshHold.ctrl.data[ctrlIdx].blendingInfo[blendIdx].blendingWeight == 0)
			//			{
			//				// Add Blending Data to Control Point
			//				m_meshHold.ctrl.data[ctrlIdx].blendingInfo[blendIdx] = currBlendingIndexWeightPair;
			//				break;
			//			}
			//		}
			//	}
			//}

			//// Get Animation Data
			//// ( Currently only Supports one Take )
			//FbxAnimStack* currAnimStack = m_fbxScene->GetSrcObject<FbxAnimStack>(0);

			//// Get Animation Name
			//FbxString animStackName = currAnimStack->GetName();
			//m_animationName = animStackName.Buffer();

			//// Get Take Info using Animation Name
			//FbxTakeInfo* takeInfo = m_fbxScene->GetTakeInfo(animStackName);

			//// Get Animation Length
			//FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			//FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			//m_animationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

			//// Create Frames Based on Animation Length
			//joint->animation = new ModelObject::Frame[(int)m_animationLength];

			//// Process Animation Data
			//int animCount = 0;
			//for (FbxLongLong frameCount = start.GetFrameCount(FbxTime::eFrames24); frameCount <= end.GetFrameCount(FbxTime::eFrames24); ++frameCount)
			//{
			//	// Set Frame Time
			//	FbxTime currTime;
			//	currTime.SetFrame(frameCount, FbxTime::eFrames24);

			//	// Save Frame Number
			//	joint->animation[animCount].curFrameNum = (int)frameCount;

			//	// Get Global Transform Matrix
			//	FbxAMatrix currentTransformOffset = currNode->EvaluateGlobalTransform(currTime) * geometryTransform;
			//	FbxAMatrix globalMtx = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
			//	joint->animation[animCount].globalTransform = FbxAMatrixToMatrix44(globalMtx);

			//	// Increase Animation Array Count
			//	animCount++;
			//}
		}
	}
}


int main()
{
//    std::string medianFilePath = MEDIAN_FILE_DIR;
//    medianFilePath.append( "Dump.xml" );
//
//    BFRESStructs::BFRES bfres;
//    XML::XmlParser::Parse(medianFilePath.c_str(), bfres);
//
//    FbxManager* lSdkManager = FbxManager::Create();
//    FbxScene* pScene = FbxScene::Create(lSdkManager, "Scene lame");
//
//    FBXWriter* fbx = new FBXWriter();
//    fbx->CreateFBX(pScene, bfres);
//
//#pragma region CreateDirectory
//    // CreateDirectory if it doesn't exist
//    {
//        int wchars_num = MultiByteToWideChar( CP_UTF8, 0, OUTPUT_FILE_DIR, -1, NULL, 0 );
//        wchar_t* wstr = new wchar_t[wchars_num];
//        MultiByteToWideChar( CP_UTF8, 0, OUTPUT_FILE_DIR, -1, wstr, wchars_num );
//        if( !CreateDirectory( wstr, NULL ) && ERROR_ALREADY_EXISTS != GetLastError() )         
//            assert( 0 && "Failed to create directory." );
//    }
//#pragma endregion
//
//    std::string fbxExportPath = OUTPUT_FILE_DIR;
//    fbxExportPath.append( "Name.fbx" );
//
//    SaveDocument(lSdkManager, pScene, fbxExportPath.c_str());

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
            if( pScene->GetNode( i )->GetChild( j ) )
                std::cout << "Child: " << pScene->GetNode( i )->GetChild( j )->GetName() << "\n";
        }
        std::cout << "\n";
    }
    return 0;
}
