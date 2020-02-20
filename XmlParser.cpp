#include "XmlParser.h"

namespace XML 
{


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void XmlParser::Parse(const char* filePath, BFRESStructs::BFRES &bfres)
{
    File file(filePath);
    Document doc;
    ParseDocument(file, doc);

    Element* pRoot = doc.first_node();
    std::string token = "";

    // Parse FMDLs
    Element* pNode = pRoot->first_node("FMDL");
    while(pNode)
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
void XmlParser::ParseDocument(File& file, Document &doc)
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
    
    // Parse Material
    {
        // TODO
        Element* pNode = pElement->first_node("Shapes");
        //ParseFSKL(fmdl.fskl, pNode);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void XmlParser::ParseFSKL(BFRESStructs::FSKL& fskl, Element* pElement)
{    
    std::string token = "";

    ParseAttributeUInt(fskl.boneCount, pElement, "SkeletonBoneCount");
    ParseAttributeUIntArray( fskl.boneList,  pElement, "BoneList"   );

    // Parse Bones
    Element* pNode = pElement->first_node("Bone");
    while(pNode)
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
    std::string token = "";
    if (ParseAttributeString(token, pElement, "Name"))        bone.name = token;
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

}
