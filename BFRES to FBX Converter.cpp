#include <iostream>
#include <fbxsdk.h>
#include "MyFBXCube.h"
#include "XmlParser.h"

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

int main()
{
    XmlParser::Parse();

    //FbxManager* lSdkManager = FbxManager::Create();
    //FbxScene* pScene = FbxScene::Create(lSdkManager, "Scene lame");

    //MyFBXCube* cube = new MyFBXCube();
    //cube->CreateCube(pScene);

    //SaveDocument(lSdkManager, pScene, "Name.fbx");

    return 0;
}

