#include "MyFBXCube.h"
#include <iostream>


MyFBXCube::MyFBXCube()
{
}

MyFBXCube::~MyFBXCube()
{
}

void MyFBXCube::CreateCube(FbxScene* pScene)
{


    // Create a node for our mesh in the scene.
    FbxNode* lMeshNode = FbxNode::Create(pScene, "meshNode");

    // Create a mesh.
    FbxMesh* lMesh = FbxMesh::Create(pScene, "mesh");

    // Set the node attribute of the mesh node.
    lMeshNode->SetNodeAttribute(lMesh);

    // Add the mesh node to the root node in the scene.
    FbxNode* lRootNode = pScene->GetRootNode();
    lRootNode->AddChild(lMeshNode);

    // Define the eight corners of the cube.
// The cube spans from
//    -50 to  50 along the X axis
//      0 to 100 along the Y axis
//    -50 to  50 along the Z axis
    FbxVector4 vertex0(-50, 0, 50);
    FbxVector4 vertex1(50, 0, 50);
    FbxVector4 vertex2(50, 100, 50);
    FbxVector4 vertex3(-50, 100, 50);
    FbxVector4 vertex4(-50, 0, -50);
    FbxVector4 vertex5(50, 0, -50);
    FbxVector4 vertex6(50, 100, -50);
    FbxVector4 vertex7(-50, 100, -50);

    // Initialize the control point array of the mesh.
    unsigned int uiNumControlPoints(24);
    unsigned int uiPolySize(3);
    lMesh->InitControlPoints(uiNumControlPoints);
    FbxVector4* lControlPoints = lMesh->GetControlPoints();

    // Define each face of the cube.
    // Face 1
    lControlPoints[0] = vertex0;
    lControlPoints[1] = vertex1;
    lControlPoints[2] = vertex2;
    lControlPoints[3] = vertex3;
    // Face 2
    lControlPoints[4] = vertex1;
    lControlPoints[5] = vertex5;
    lControlPoints[6] = vertex6;
    lControlPoints[7] = vertex2;
    // Face 3
    lControlPoints[8] = vertex5;
    lControlPoints[9] = vertex4;
    lControlPoints[10] = vertex7;
    lControlPoints[11] = vertex6;
    // Face 4
    lControlPoints[12] = vertex4;
    lControlPoints[13] = vertex0;
    lControlPoints[14] = vertex3;
    lControlPoints[15] = vertex7;
    // Face 5
    lControlPoints[16] = vertex3;
    lControlPoints[17] = vertex2;
    lControlPoints[18] = vertex6;
    lControlPoints[19] = vertex7;
    // Face 6
    lControlPoints[20] = vertex1;
    lControlPoints[21] = vertex0;
    lControlPoints[22] = vertex4;
    lControlPoints[23] = vertex5;

    // Define which control points belong to a poly
    for(unsigned int i = 0; i < uiNumControlPoints; ++i)
    {
        // first index
	    if((i % uiPolySize) == 0)
		    lMesh->BeginPolygon();
	
        lMesh->AddPolygon(i);
	
        // last index
	    if((i + 1) % uiPolySize == 0)
		    lMesh->EndPolygon();
    }

    // Define normal vectors along each axis.
    FbxVector4 lNormalXPos(1, 0, 0);
    FbxVector4 lNormalXNeg(-1, 0, 0);
    FbxVector4 lNormalYPos(0, 1, 0);
    FbxVector4 lNormalYNeg(0, -1, 0);
    FbxVector4 lNormalZPos(0, 0, 1);
    FbxVector4 lNormalZNeg(0, 0, -1);

    // Create layer 0 for the mesh if it does not already exist.
    // This is where we will define our normals.
    FbxLayer* lLayer = lMesh->GetLayer(0);
    if (lLayer == NULL) {
        lMesh->CreateLayer();
        lLayer = lMesh->GetLayer(0);
    }

    // Create a normal layer.
    FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");

    // Set its mapping mode to map each normal vector to each control point.
    lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);

    // Set the reference mode of so that the n'th element of the normal array maps to the n'th
    // element of the control point array.
    lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

    // Assign the normal vectors in the same order the control points were defined for the mesh.
    // Face 1
    lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
    // Face 2
    lLayerElementNormal->GetDirectArray().Add(lNormalXPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalXPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalXPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalXPos);
    // Face 3
    lLayerElementNormal->GetDirectArray().Add(lNormalZNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalZNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalZNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalZNeg);
    // Face 4
    lLayerElementNormal->GetDirectArray().Add(lNormalXNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalXNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalXNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalXNeg);
    // Face 5
    lLayerElementNormal->GetDirectArray().Add(lNormalYPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalYPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalYPos);
    lLayerElementNormal->GetDirectArray().Add(lNormalYPos);
    // Face 6
    lLayerElementNormal->GetDirectArray().Add(lNormalYNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalYNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalYNeg);
    lLayerElementNormal->GetDirectArray().Add(lNormalYNeg);

    // Finally, we set layer 0 of the mesh to the normal layer element.
    lLayer->SetNormals(lLayerElementNormal);
}