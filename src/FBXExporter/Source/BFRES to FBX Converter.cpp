#include <iostream>
#include <fbxsdk.h>
#include "MyFBXCube.h"
#include "FBXWriter.h"
#include "XmlParser.h"
#include "BFRES.h"
#include <windows.h>
#include "Globals.h"

#ifdef IOS_REF
    #undef  IOS_REF
    #define IOS_REF (*(pManager->GetIOSettings()))
#endif

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

int main(int argc, char* argv[])
{
    assert(argc == 1 || argc == 3);
    std::string medianFilePath;
    if (argc == 1)
	{
        medianFilePath = MEDIAN_FILE_DIR;
		medianFilePath.append("Npc_Gerudo_Queen.xml");
    }
    else
    {
        medianFilePath = argv[1];
    }

	uint32 lastIndex = medianFilePath.find_last_of(".");
    uint32 lastSlashIndex = medianFilePath.find_last_of("\\");
    std::string fileName = medianFilePath.substr(lastSlashIndex + 1, lastIndex - lastSlashIndex - 1);


    
    BFRESStructs::BFRES* bfres = g_BFRESManager.GetBFRES();
    XML::XmlParser::Parse(medianFilePath.c_str(), *bfres);

    FbxManager* lSdkManager = FbxManager::Create();
    FbxScene* pScene = FbxScene::Create(lSdkManager, "Scene lame");

    // Convert the scene to meters using the defined options.
    const FbxSystemUnit::ConversionOptions lConversionOptions = {
      false, /* mConvertRrsNodes */
      true, /* mConvertLimits */
      true, /* mConvertClusters */
      true, /* mConvertLightIntensity */
      true, /* mConvertPhotometricLProperties */
      true  /* mConvertCameraClipPlanes */
    };

    FbxSystemUnit::m.ConvertScene( pScene, lConversionOptions );

    FBXWriter* fbx = new FBXWriter();
    fbx->CreateFBX(pScene, *bfres);

    std::string fbxExportPath;
    if (argc == 1)
    {
		if (!CreateDirectoryA(OUTPUT_FILE_DIR, NULL) && ERROR_ALREADY_EXISTS != GetLastError())
			assert(0 && "Failed to create directory.");

        fbxExportPath = OUTPUT_FILE_DIR;
		fbxExportPath.append("Name.fbx");
    }
    else
    {
        if (!CreateDirectoryA(argv[2], NULL) && ERROR_ALREADY_EXISTS != GetLastError())
            assert(0 && "Failed to create directory.");

        fbxExportPath = argv[2];
        fbxExportPath.append( fileName + ".fbx" );
    }


	std::cout << pScene->GetMemberCount() << "\n\n";
	
#ifdef PRINT_DEBUG_INFO
	for (int32 i = 0; i < pScene->GetNodeCount(); i++)
	{
		std::cout << "Name: " << pScene->GetNode(i)->GetName() << "\n";
		if (pScene->GetNode(i)->GetNodeAttribute())
		{
			std::string attributeType;
			switch (pScene->GetNode(i)->GetNodeAttribute()->GetAttributeType())
			{
			case 3:
				attributeType = "Skeleton";
				break;
			case 4:
				attributeType = "Mesh";
				break;
			default:
				break;
			}
			std::cout << "Attribute type: " << attributeType << "\n";
		}

		if (pScene->GetNode(i)->GetParent())
			std::cout << "Parent: " << pScene->GetNode(i)->GetParent()->GetName() << "\n";

		for (int32 j = 0; j < pScene->GetNode(i)->GetChildCount(); j++)
		{
			if (pScene->GetNode(i)->GetChild(j))
				std::cout << "Child: " << pScene->GetNode(i)->GetChild(j)->GetName() << "\n";
		}
		std::cout << "\n";
	}
#endif // PRINT_DEBUG_INFO

    // Convert the scene to centimeters using the defined options.
    FbxSystemUnit::cm.ConvertScene( pScene, lConversionOptions );

    SaveDocument(lSdkManager, pScene, fbxExportPath.c_str());

    return 0;
}
