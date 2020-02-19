#pragma once
#include <string>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "BFRES.h"

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

    template<std::size_t uiLen>
    static bool ParseAttributeString(std::string& token, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeString(token, pElement, attrName, uiLen - 1); 
    }

    template<std::size_t uiLen>
    static bool ParseAttributeNumber(size_t& num, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeNumber(num, pElement, attrName, uiLen - 1); 
    }

    template<std::size_t uiLen>
    static bool ParseAttributeArray(std::vector<size_t>& arr, Element* pElement, const char(&attrName)[uiLen])
    { 
        return ParseAttributeArray(arr, pElement, attrName, uiLen - 1); 
    }

    static bool ParseAttributeString(std::string& token, Element* pElement, const char* attrName, std::size_t uiLen);
    static bool ParseAttributeNumber(size_t& num, Element* pElement, const char* attrName, std::size_t uiLen);
    static bool ParseAttributeArray(std::vector<size_t>& arr, Element* pElement, const char* attrName, std::size_t uiLen);
};


}
