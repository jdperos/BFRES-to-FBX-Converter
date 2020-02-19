#pragma once
#include <string>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "BFRES.h"
#include "Primitives.h"

namespace XML
{

typedef rapidxml::file<>            File;
typedef rapidxml::xml_document<>    Document;
typedef rapidxml::xml_node<>        Element;
typedef rapidxml::xml_attribute<>   Attribute;


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
class XmlParser
{
public:
    static void Parse(const char* filePath, BFRESStructs::BFRES &bfres);

    static void ParseDocument(File &file, Document &doc);

    static void ParseFMDL(BFRESStructs::FMDL& fmdl, Element* pElement);
    static void ParseFSKL(BFRESStructs::FSKL& fskl, Element* pElement);
    static void ParseBone(BFRESStructs::Bone& bone, Element* pElement);

    // General type parsers
    template<uint32 uiLen>
    static bool ParseAttributeString(std::string& token, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeString(token, pElement, attrName, uiLen - 1); 
    }

    template<uint32 uiLen>
    static bool ParseAttributeBool(bool& bToken, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeBool(bToken, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeUInt(uint32& num, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeUInt(num, pElement, attrName, uiLen - 1); 
    }

    template<uint32 uiLen>
    static bool ParseAttributeInt(int32& num, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeInt(num, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeFloat(float& value, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeFloat(value, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeIntArray(std::vector<int32>& arr, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeIntArray(arr, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeUIntArray(std::vector<uint32>& arr, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeUIntArray(arr, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeFloatArray(std::vector<float>& arr, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeFloatArray(arr, pElement, attrName, uiLen - 1);
    }

    // Custom type parsers
    template<uint32 uiLen>
    static bool ParseAttributeVector2F(Math::vector2F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeVector2F(vec, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector3F(Math::vector3F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeVector3F(vec, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector4F(Math::vector4F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeVector4F(vec, pElement, attrName, uiLen - 1);
    }

    // Enum parser templates
    template<uint32 uiLen>
    static bool ParseAttributeRotationType(BFRESStructs::RotationType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeRotationType(eType, pElement, attrName, uiLen - 1);
    }

    template<uint32 uiLen>
    static bool ParseAttributeGX2PrimitiveType(BFRESStructs::GX2PrimitiveType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        return ParseAttributeGX2PrimitiveType(eType, pElement, attrName, uiLen - 1);
    }

    // General type parsers
    static bool ParseAttributeBool(bool& bToken, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeString(std::string& token, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeUInt(uint32& num, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeInt(int32& num, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeFloat(float& value, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeIntArray(std::vector<int32>& arr, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeUIntArray(std::vector<uint32>& arr, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeFloatArray(std::vector<float>& arr, Element* pElement, const char* attrName, uint32 uiLen);

    // Custom type parsers
    static bool ParseAttributeVector2F(Math::vector2F& vec, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeVector3F(Math::vector3F& vec, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeVector4F(Math::vector4F& vec, Element* pElement, const char* attrName, uint32 uiLen);

    // Enum parsers
    static bool ParseAttributeRotationType(BFRESStructs::RotationType& eType, Element* pElement, const char* attrName, uint32 uiLen);
    static bool ParseAttributeGX2PrimitiveType(BFRESStructs::GX2PrimitiveType& eType, Element* pElement, const char* attrName, uint32 uiLen);
};


}
