#pragma once
#include <string>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <sstream>
#include "Math.h"
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
        token.clear();
        Attribute* pAttribute;
        pAttribute = pElement->first_attribute(attrName, uiLen - 1, false);

        if (!pAttribute || pAttribute->value_size() == 0)
            return false;

        token.append(pAttribute->value(), pAttribute->value_size());
        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeBool(bool& bToken, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    template<uint32 uiLen>
    static bool ParseAttributeUInt(uint32& num, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        try
        {
            num = static_cast<uint32>(std::stoi(token));
            return true;
        }
        catch (...)
        {
            assert(0 && "Invalid argument");
        }

        return false;
    }

    template<uint32 uiLen>
    static bool ParseAttributeInt(int32& num, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    template<uint32 uiLen>
    static bool ParseAttributeFloat(float& value, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    template<uint32 uiLen>
    static bool ParseAttributeIntArray(std::vector<int32>& arr, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        // tokenize the string
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

    template<uint32 uiLen>
    static bool ParseAttributeUIntArray(std::vector<uint32>& arr, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    template<uint32 uiLen>
    static bool ParseAttributeFloatArray(std::vector<float>& arr, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    // Custom type parsers
    template<uint32 uiLen>
    static bool ParseAttributeVector2F(Math::vector2F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<float> vals;
        if (!ParseAttributeFloatArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];

        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector3F(Math::vector3F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<float> vals;
        if (!ParseAttributeFloatArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];
        vec.Z = vals[2];

        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector4F(Math::vector4F& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<float> vals;
        if (!ParseAttributeFloatArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];
        vec.Z = vals[2];
        vec.W = vals[3];

        return true;
    }

    // Enum parser templates
    template<uint32 uiLen>
    static bool ParseAttributeRotationType(BFRESStructs::RotationType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
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

    template<uint32 uiLen>
    static bool ParseAttributeGX2PrimitiveType(BFRESStructs::GX2PrimitiveType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        if      (token == "Points"                 )    eType = BFRESStructs::GX2PrimitiveType::Points;
        else if (token == "Lines"                  )    eType = BFRESStructs::GX2PrimitiveType::Lines;
        else if (token == "LineStrip"              )    eType = BFRESStructs::GX2PrimitiveType::LineStrip;
        else if (token == "Triangles"              )    eType = BFRESStructs::GX2PrimitiveType::Triangles;
        else if (token == "TriangleFan"            )    eType = BFRESStructs::GX2PrimitiveType::TriangleFan;
        else if (token == "TriangleStrip"          )    eType = BFRESStructs::GX2PrimitiveType::TriangleStrip;
        else if (token == "LinesAdjacency"         )    eType = BFRESStructs::GX2PrimitiveType::LinesAdjacency;
        else if (token == "LineStripAdjacency"     )    eType = BFRESStructs::GX2PrimitiveType::LineStripAdjacency;
        else if (token == "TriangleStripAdjacency" )    eType = BFRESStructs::GX2PrimitiveType::TriangleStripAdjacency;
        else if (token == "Rects"                  )    eType = BFRESStructs::GX2PrimitiveType::Rects;
        else if (token == "LineLoop"               )    eType = BFRESStructs::GX2PrimitiveType::LineLoop;
        else if (token == "Quads"                  )    eType = BFRESStructs::GX2PrimitiveType::Quads;
        else if (token == "QuadStrip"              )    eType = BFRESStructs::GX2PrimitiveType::QuadStrip;
        else if (token == "TessellateLines"        )    eType = BFRESStructs::GX2PrimitiveType::TessellateLines;
        else if (token == "TessellateLineStrip"    )    eType = BFRESStructs::GX2PrimitiveType::TessellateLineStrip;
        else if (token == "TessellateTriangles"    )    eType = BFRESStructs::GX2PrimitiveType::TessellateTriangles;
        else if (token == "TessellateTriangleStrip")    eType = BFRESStructs::GX2PrimitiveType::TessellateTriangleStrip;
        else if (token == "TessellateQuads"        )    eType = BFRESStructs::GX2PrimitiveType::TessellateQuads;
        else if (token == "TessellateQuadStrip"    )    eType = BFRESStructs::GX2PrimitiveType::TessellateQuadStrip;
        else
        {
            assert(0 && "Invalid argument");
            return false;
        }
        return true;
    }

};


}
