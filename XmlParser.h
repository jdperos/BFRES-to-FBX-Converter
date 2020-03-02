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

using namespace BFRESStructs;

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
    static void Parse(const char* filePath, BFRES &bfres);

    static void ParseDocument(File &file, Document &doc);

    static void ParseFMDL(FMDL& fmdl, Element* pElement);
    static void ParseFSKL(FSKL& fskl, Element* pElement);
    static void ParseBone(Bone& bone, Element* pElement);

    static void ParseTextureRefs(TextureRefs& textureRefs, Element* pElement);
    static void ParseMaterials(std::vector <FMAT>& fmats, Element* pElement);
    static void ParseFMAT(FMAT& fmat, Element* pElement);
    static void ParseShapes(std::vector<FSHP>& fshps, Element* pElement);
    static void ParseFSHP(FSHP& fshp, Element* pElement);
    static void ParseLODMesh(LODMesh& lodMesh, Element* pElement);
    static void ParseFVTX(FVTX& fvtx, Element* pElement);

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

    template<uint32 uiLen>
    static bool ParseAttributeVector2(Math::vector2& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<uint32> vals;
        if (!ParseAttributeUIntArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];

        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector3(Math::vector3& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<uint32> vals;
        if (!ParseAttributeUIntArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];
        vec.Z = vals[2];

        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeVector4(Math::vector4& vec, Element* pElement, const char(&attrName)[uiLen])
    {
        std::vector<uint32> vals;
        if (!ParseAttributeUIntArray(vals, pElement, attrName))
            return false;

        vec.X = vals[0];
        vec.Y = vals[1];
        vec.Z = vals[2];
        vec.W = vals[3];

        return true;
    }

    // Enum parser templates
    template<uint32 uiLen>
    static bool ParseAttributeRotationType(RotationType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        if (token == "Quaternion")
            eType = RotationType::Quaternion;
        else if (token == "EulerXYZ")
            eType = RotationType::EulerXYZ;
        else
        {
            assert(0 && "Invalid argument");
            return false;
        }
        return true;
    }

    template<uint32 uiLen>
    static bool ParseAttributeGX2PrimitiveType(GX2PrimitiveType& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        if      (token == "Points"                 )    eType = GX2PrimitiveType::Points;
        else if (token == "Lines"                  )    eType = GX2PrimitiveType::Lines;
        else if (token == "LineStrip"              )    eType = GX2PrimitiveType::LineStrip;
        else if (token == "Triangles"              )    eType = GX2PrimitiveType::Triangles;
        else if (token == "TriangleFan"            )    eType = GX2PrimitiveType::TriangleFan;
        else if (token == "TriangleStrip"          )    eType = GX2PrimitiveType::TriangleStrip;
        else if (token == "LinesAdjacency"         )    eType = GX2PrimitiveType::LinesAdjacency;
        else if (token == "LineStripAdjacency"     )    eType = GX2PrimitiveType::LineStripAdjacency;
        else if (token == "TriangleStripAdjacency" )    eType = GX2PrimitiveType::TriangleStripAdjacency;
        else if (token == "Rects"                  )    eType = GX2PrimitiveType::Rects;
        else if (token == "LineLoop"               )    eType = GX2PrimitiveType::LineLoop;
        else if (token == "Quads"                  )    eType = GX2PrimitiveType::Quads;
        else if (token == "QuadStrip"              )    eType = GX2PrimitiveType::QuadStrip;
        else if (token == "TessellateLines"        )    eType = GX2PrimitiveType::TessellateLines;
        else if (token == "TessellateLineStrip"    )    eType = GX2PrimitiveType::TessellateLineStrip;
        else if (token == "TessellateTriangles"    )    eType = GX2PrimitiveType::TessellateTriangles;
        else if (token == "TessellateTriangleStrip")    eType = GX2PrimitiveType::TessellateTriangleStrip;
        else if (token == "TessellateQuads"        )    eType = GX2PrimitiveType::TessellateQuads;
        else if (token == "TessellateQuadStrip"    )    eType = GX2PrimitiveType::TessellateQuadStrip;
        else
        {
            assert(0 && "Invalid argument");
            return false;
        }
        return true;
    }

	template<uint32 uiLen>
    static bool ParseAttributeGX2TexClamp(GX2TexClamp& eType, Element* pElement, const char(&attrName)[uiLen])
    {
        std::string token = "";
        if (!ParseAttributeString(token, pElement, attrName))
            return false;

        if      (token == "Wrap")                 eType = GX2TexClamp::Wrap;
        else if (token == "Mirror")               eType = GX2TexClamp::Mirror;
        else if (token == "Clamp")                eType = GX2TexClamp::Clamp;
        else if (token == "MirrorOnce")           eType = GX2TexClamp::MirrorOnce;
        else if (token == "ClampHalfBorder")      eType = GX2TexClamp::ClampHalfBorder;
        else if (token == "MirrorOnceHalfBorder") eType = GX2TexClamp::MirrorOnceHalfBorder;
        else if (token == "ClampBorder")          eType = GX2TexClamp::ClampBorder;
        else if (token == "MirrorOnceBorder")     eType = GX2TexClamp::MirrorOnceBorder;
        else
        {
            assert(0 && "Invalid argument");
            return false;
        }
        return true;
    }

	template<uint32 uiLen>
	static bool ParseAttributeGX2TexXYFilterType(GX2TexXYFilterType& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "Nearest") eType = GX2TexXYFilterType::Point;
		else if (token == "Linear")  eType = GX2TexXYFilterType::Bilinear;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

	template<uint32 uiLen>
	static bool ParseAttributeGX2TexZFilterType(GX2TexZFilterType& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "UseXY")  eType = GX2TexZFilterType::UseXY;
		else if (token == "Point")  eType = GX2TexZFilterType::Point;
        else if (token == "Linear") eType = GX2TexZFilterType::Linear;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

	template<uint32 uiLen>
	static bool ParseAttributeGX2TexMipFilterType(GX2TexMipFilterType& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "NoMip")  eType = GX2TexMipFilterType::NoMip;
		else if (token == "Point")  eType = GX2TexMipFilterType::Point;
		else if (token == "Linear") eType = GX2TexMipFilterType::Linear;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

	template<uint32 uiLen>
	static bool ParseAttributeGX2TexAnisoRatio(GX2TexAnisoRatio& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "OneToOne")     eType = GX2TexAnisoRatio::OneToOne;
		else if (token == "TwoToOne")     eType = GX2TexAnisoRatio::TwoToOne;
		else if (token == "FourToOne")    eType = GX2TexAnisoRatio::FourToOne;
        else if (token == "EightToOne")   eType = GX2TexAnisoRatio::EightToOne;
        else if (token == "SixteenToOne") eType = GX2TexAnisoRatio::SixteenToOne;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

	template<uint32 uiLen>
	static bool ParseAttributeGX2TexBorderType(GX2TexBorderType& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "ClearBlack")  eType = GX2TexBorderType::ClearBlack;
		else if (token == "SolidBlack")  eType = GX2TexBorderType::SolidBlack;
		else if (token == "SolidWhite")  eType = GX2TexBorderType::SolidWhite;
		else if (token == "UseRegister") eType = GX2TexBorderType::UseRegister;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}


	template<uint32 uiLen>
	static bool ParseAttributeGX2CompareFunction(GX2CompareFunction& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "Never")           eType = GX2CompareFunction::Never;
		else if (token == "Less")            eType = GX2CompareFunction::Less;
		else if (token == "Equal")           eType = GX2CompareFunction::Equal;
		else if (token == "LessOrEqual")     eType = GX2CompareFunction::LessOrEqual;
		else if (token == "Greater")         eType = GX2CompareFunction::Greater;
		else if (token == "NotEqual")        eType = GX2CompareFunction::NotEqual;
		else if (token == "GreaterOrEqual")  eType = GX2CompareFunction::GreaterOrEqual;
		else if (token == "Always")          eType = GX2CompareFunction::Always;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

	template<uint32 uiLen>
	static bool ParseAttributeGX2TextureMapType(GX2TextureMapType& eType, Element* pElement, const char(&attrName)[uiLen])
	{
		std::string token = "";
		if (!ParseAttributeString(token, pElement, attrName))
			return false;

		if      (token == "Albedo")               eType = GX2TextureMapType::Albedo;
        else if (token == "Diffuse")              eType = GX2TextureMapType::Albedo;
		else if (token == "Normal")               eType = GX2TextureMapType::Normal;
		else if (token == "Specular")             eType = GX2TextureMapType::Specular;
		else if (token == "AmbientOcclusion")     eType = GX2TextureMapType::AmbientOcclusion;
        else if (token == "AO")                   eType = GX2TextureMapType::AmbientOcclusion;
		else if (token == "Emission")             eType = GX2TextureMapType::Emission;
		else if (token == "Shadow")               eType = GX2TextureMapType::Shadow;
		else if (token == "Light")                eType = GX2TextureMapType::Light;
		else if (token == "MRA")                  eType = GX2TextureMapType::MRA;
		else if (token == "Metalness")            eType = GX2TextureMapType::Metalness;
		else if (token == "Roughness")            eType = GX2TextureMapType::Roughness;
		else if (token == "SubSurfaceScattering") eType = GX2TextureMapType::SubSurfaceScattering;
		else
		{
			assert(0 && "Invalid argument");
			return false;
		}
		return true;
	}

};


}
