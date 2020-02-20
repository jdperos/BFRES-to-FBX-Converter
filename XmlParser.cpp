#include "XmlParser.h"

namespace XML
{


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::Parse(const char* filePath, BFRESStructs::BFRES& bfres)
    {
        File file(filePath);
        Document doc;
        ParseDocument(file, doc);

        Element* pRoot = doc.first_node();
        std::string token = "";

        // Parse FMDLs
        Element* pNode = pRoot->first_node("FMDL");
        while (pNode)
        {
            BFRESStructs::FMDL fmdl;
            ParseFMDL(fmdl, pNode);
            bfres.fmdl.push_back(fmdl);
            pNode = pNode->next_sibling("FMDL");
        }

        //pNode = pNode->first_node("Shapes")->first_node();
        //while(pNode)
        //{
        //    if (!(pNode->next_sibling()))
        //        break;
        //    pNode = pNode->next_sibling();
        //}

        //pNode = pNode->first_node("Vertices")->first_node();
        //while(pNode)
        //{
        //    if (!(pNode->next_sibling()))
        //        break;
        //    pNode = pNode->next_sibling();
        //}

        //if (ParseAttributeString(token, pNode, "Index"))
        //    printf("Last Index is %s", token.c_str());
        //for (Attribute* attr = node->first_attribute();
        //    attr; attr = attr->next_attribute())
        //{
        //    std::cout << "Node foobar has attribute " << attr->name() << " ";
        //    std::cout << "with value " << attr->value() << "\n";
        //}
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseDocument(File& file, Document& doc)
    {
        doc.parse<0>(file.data());
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFMDL(BFRESStructs::FMDL& fmdl, Element* pElement)
    {
        std::string token = "";
        if (ParseAttributeString(token, pElement, "Name"))        fmdl.name = token;

        // Parse FSKL
        {
            Element* pNode = pElement->first_node("FSKL");
            ParseFSKL(fmdl.fskl, pNode);
        }

        // Parse Material
        {
            // TODO
            Element* pNode = pElement->first_node("Material");
            //ParseFSKL(fmdl.fskl, pNode);
        }

        // Parse Shapes
        {
            // TODO
            Element* pNode = pElement->first_node("Shapes");
            ParseShapes(fmdl.fshps, pNode);
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFSKL(BFRESStructs::FSKL& fskl, Element* pElement)
    {
        std::string token = "";

        ParseAttributeUInt(fskl.boneCount, pElement, "SkeletonBoneCount");
        ParseAttributeUIntArray(fskl.boneList, pElement, "BoneList");

        // Parse Bones
        Element* pNode = pElement->first_node("Bone");
        while (pNode)
        {
            BFRESStructs::Bone bone;
            ParseBone(bone, pNode);
            fskl.bones.push_back(bone);
            pNode = pNode->next_sibling("Bone");
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseBone(BFRESStructs::Bone& bone, Element* pElement)
    {
        ParseAttributeString(bone.name, pElement, "Name");
        ParseAttributeUInt(bone.index, pElement, "Index");
        ParseAttributeBool(bone.isVisible, pElement, "IsVisible");
        ParseAttributeInt(bone.rigidMatrixIndex, pElement, "RigidMatrixIndex");
        ParseAttributeInt(bone.smoothMatrixIndex, pElement, "SmoothMatrixIndex");
        ParseAttributeInt(bone.billboardIndex, pElement, "BillboardIndex");
        ParseAttributeBool(bone.useRigidMatrix, pElement, "UseRigidMatrix");
        ParseAttributeBool(bone.useSmoothMatrix, pElement, "UseSmoothMatrix");
        ParseAttributeUInt(bone.parentIndex, pElement, "ParentIndex");
        ParseAttributeRotationType(bone.rotationType, pElement, "RotationType");
        ParseAttributeVector3F(bone.scale, pElement, "Scale");
        ParseAttributeVector4F(bone.rotation, pElement, "Rotation");
        ParseAttributeVector3F(bone.position, pElement, "Position");
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseShapes(std::vector<BFRESStructs::FSHP>& fshps, Element* pElement)
    {
        // Parse FSHPs
        Element* pNode = pElement->first_node("FSHP");
        while (pNode)
        {
            BFRESStructs::FSHP fshp;
            ParseFSHP(fshp, pNode);
            fshps.push_back(fshp);
            pNode = pNode->next_sibling("FSHP");
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFSHP(BFRESStructs::FSHP& fshp, Element* pElement)
    {
        ParseAttributeString(fshp.name, pElement, "Name");
        // TODO ParseAttributeShapeFlags
        ParseAttributeUInt(fshp.materialIndex, pElement, "MaterialIndex");
        ParseAttributeUInt(fshp.boneIndex, pElement, "BoneIndex");
        ParseAttributeUInt(fshp.vertexBufferIndex, pElement, "VertexBufferIndex");
        ParseAttributeFloatArray(fshp.radiusArray, pElement, "RadiusArray");
        ParseAttributeUInt(fshp.vertexSkinCount, pElement, "VertexSkinCount");
        ParseAttributeUInt(fshp.targetAttributeCount, pElement, "TargetAttributeCount");
        // TODO MORE CHILDREN

        // Parse Meshes
        Element* pNode = pElement->first_node("Meshes")->first_node("LODMesh");
        while (pNode)
        {
            BFRESStructs::LODMesh lodMesh;
            ParseLODMesh(lodMesh, pNode);
            fshp.lodMeshes.push_back(lodMesh);
            pNode = pNode->next_sibling("LODMesh");
        }

        // Parse Vertices
        pNode = pElement->first_node("Vertices")->first_node("Vertex");
        while (pNode)
        {
            BFRESStructs::FVTX fvtx;
            ParseFVTX(fvtx, pNode);
            fshp.vertices.push_back(fvtx);
            pNode = pNode->next_sibling("Vertex");
        }

    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseLODMesh(BFRESStructs::LODMesh& lodMesh, Element* pElement)
    {
        // Parse primitive and index type for real instead of hardcoding these values
        lodMesh.primitiveType = BFRESStructs::GX2PrimitiveType::Triangles;
        lodMesh.indexFormat = BFRESStructs::GX2IndexFormat::UInt16;
        ParseAttributeUInt(lodMesh.indexCount      , pElement, "IndexCount"  );
        ParseAttributeUInt(lodMesh.firstVertex     , pElement, "FirstVertex" );
        ParseAttributeIntArray(lodMesh.faceVertices, pElement, "FaceVertices");
        // TODO add parse submesh function
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFVTX(BFRESStructs::FVTX& fvtx, Element* pElement)
    {
        ParseAttributeUInt(fvtx.index           , pElement, "Index"       );
        ParseAttributeVector3F(fvtx.position0   , pElement, "Position0"   );
        ParseAttributeVector3F(fvtx.position1   , pElement, "Position1"   );
        ParseAttributeVector3F(fvtx.position2   , pElement, "Position2"   );
        ParseAttributeVector3F(fvtx.normal      , pElement, "Normal"      );
        ParseAttributeVector2F(fvtx.uv0         , pElement, "UV0"         );
        ParseAttributeVector2F(fvtx.uv1         , pElement, "UV1"         );
        ParseAttributeVector2F(fvtx.uv2         , pElement, "UV2"         );
        ParseAttributeVector4F(fvtx.color0      , pElement, "Color0"      );
        ParseAttributeVector4F(fvtx.color1      , pElement, "Color1"      );
        ParseAttributeVector4F(fvtx.tangent     , pElement, "Tangent"     );
        ParseAttributeVector4F(fvtx.binormal    , pElement, "Binormal"    );
        ParseAttributeVector4F(fvtx.blendWeights, pElement, "BlendWeights");
        ParseAttributeVector4(fvtx.blendIndex   , pElement, "BlendIndex"  );
    }

}
