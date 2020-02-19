#include "XmlParser.h"
#include <iostream>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include "Math.h"


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


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeBool(bool& bToken, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    if (token == "true")
        bToken = true;
    else if (token == "false")
        bToken = false;
    else
    {
        assert(0 && "Invalid argument");
        return false;
    }
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeString(std::string& token, Element* pElement, const char* attrName, uint32 uiLen)
{
    token.clear();
    Attribute* pAttribute;
    pAttribute = pElement->first_attribute(attrName, uiLen, false);

    if (!pAttribute || pAttribute->value_size() == 0)
        return false;

    token.append(pAttribute->value(), pAttribute->value_size());
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeUInt(uint32& num, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;
    
    try
    {
        num = static_cast<uint32>(std::stoi(token));
        return true;
    }
    catch(...)
    {
        assert(0 && "Invalid argument");
    }

    return false;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeInt(int32& num, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    try
    {
        num = static_cast<int32>(std::stoi(token));
        return true;
    }
    catch (...)
    {
        assert(0 && "Invalid argument");
    }

    return false;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeFloat(float& value, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    try
    {
        value = static_cast<float>(std::stof(token));
        return true;
    }
    catch (...)
    {
        assert(0 && "Invalid argument");
    }

    return false;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeIntArray(std::vector<int32>& arr, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    // tokenise the string
    std::istringstream iss(token);
    std::string subStr;
    while (getline(iss, subStr, ','))
    {
        try
        {
            int32 number = std::stoi(subStr);
            arr.push_back(number);
        }
        catch (...)
        {
            assert(0 && "Invalid argument");
            return false;
        }

    }
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeUIntArray(std::vector<uint32>& arr, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    // tokenise the string
    std::istringstream iss(token);
    std::string subStr;
    while (getline(iss, subStr, ','))
    {
        try
        {
            uint32 number = static_cast<uint32>(std::stoi(subStr));
            arr.push_back(number);
        }
        catch (...)
        {
            assert(0 && "Invalid argument");
            return false;
        }

    }
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeFloatArray(std::vector<float>& arr, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    // tokenise the string
    std::istringstream iss(token);
    std::string subStr;
    while (getline(iss, subStr, ','))
    {
        try
        {
            float value = static_cast<float>(std::stof(subStr));
            arr.push_back(value);
        }
        catch (...)
        {
            assert(0 && "Invalid argument");
            return false;
        }

    }
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeVector2F(Math::vector2F& vec, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::vector<float> vals;
    if (!ParseAttributeFloatArray(vals, pElement, attrName, uiLen))
        return false;

    vec.X = vals[0];
    vec.Y = vals[1];

    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeVector3F(Math::vector3F& vec, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::vector<float> vals;
    if (!ParseAttributeFloatArray(vals, pElement, attrName, uiLen))
        return false;

    vec.X = vals[0];
    vec.Y = vals[1];
    vec.Z = vals[2];

    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeVector4F(Math::vector4F& vec, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::vector<float> vals;
    if (!ParseAttributeFloatArray(vals, pElement, attrName, uiLen))
        return false;

    vec.X = vals[0];
    vec.Y = vals[1];
    vec.Z = vals[2];
    vec.W = vals[3];

    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeRotationType(BFRESStructs::RotationType& eType, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    if (token == "Quaternion")
        eType = BFRESStructs::RotationType::Quaternion;
    else if (token == "EulerXYZ")
        eType = BFRESStructs::RotationType::EulerXYZ;
    else
    {
        assert(0 && "Invalid argument");
        return false;
    }
    return true;
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeGX2PrimitiveType(BFRESStructs::GX2PrimitiveType& eType, Element* pElement, const char* attrName, uint32 uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;

    if (token == "Points")
        eType = BFRESStructs::GX2PrimitiveType::Points;
    else if (token == "Lines")
        eType = BFRESStructs::GX2PrimitiveType::Lines;
    else if (token == "LineStrip")
        eType = BFRESStructs::GX2PrimitiveType::LineStrip;
    else if (token == "Triangles")
        eType = BFRESStructs::GX2PrimitiveType::Triangles;
    else if (token == "TriangleFan")
        eType = BFRESStructs::GX2PrimitiveType::TriangleFan;
    else if (token == "TriangleStrip")
        eType = BFRESStructs::GX2PrimitiveType::TriangleStrip;
    else if (token == "LinesAdjacency")
        eType = BFRESStructs::GX2PrimitiveType::LinesAdjacency;
    else if (token == "LineStripAdjacency")
        eType = BFRESStructs::GX2PrimitiveType::LineStripAdjacency;
    else if (token == "TriangleStripAdjacency")
        eType = BFRESStructs::GX2PrimitiveType::TriangleStripAdjacency;
    else if (token == "Rects")
        eType = BFRESStructs::GX2PrimitiveType::Rects;
    else if (token == "LineLoop")
        eType = BFRESStructs::GX2PrimitiveType::LineLoop;
    else if (token == "Quads")
        eType = BFRESStructs::GX2PrimitiveType::Quads;
    else if (token == "QuadStrip")
        eType = BFRESStructs::GX2PrimitiveType::QuadStrip;
    else if (token == "TessellateLines")
        eType = BFRESStructs::GX2PrimitiveType::TessellateLines;
    else if (token == "TessellateLineStrip")
        eType = BFRESStructs::GX2PrimitiveType::TessellateLineStrip;
    else if (token == "TessellateTriangles")
        eType = BFRESStructs::GX2PrimitiveType::TessellateTriangles;
    else if (token == "TessellateTriangleStrip")
        eType = BFRESStructs::GX2PrimitiveType::TessellateTriangleStrip;
    else if (token == "TessellateQuads")
        eType = BFRESStructs::GX2PrimitiveType::TessellateQuads;
    else if (token == "TessellateQuadStrip")
        eType = BFRESStructs::GX2PrimitiveType::TessellateQuadStrip;
    else
    {
        assert(0 && "Invalid argument");
        return false;
    }
    return true;
}

}
