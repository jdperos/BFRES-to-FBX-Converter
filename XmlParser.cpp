#include "XmlParser.h"
#include <iostream>
#include <assert.h>
#include <sstream>


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
    if (ParseAttributeString(token, pElement, "Name"))        fmdl.Name = token;

    // Parse FSKL
    {
        Element* pNode = pElement->first_node("FSKL");
        ParseFSKL(fmdl.fskl, pNode);
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void XmlParser::ParseFSKL(BFRESStructs::FSKL& fskl, Element* pElement)
{    
    std::string token = "";

    ParseAttributeNumber(fskl.boneCount, pElement, "SkeletonBoneCount");
    ParseAttributeArray( fskl.boneList,  pElement, "BoneList"         );

    // Parse Bones
    Element* pNode = pElement->first_node();
    while(pNode)
    {
        pNode = pNode->next_sibling();
    }
}


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool XmlParser::ParseAttributeString(std::string& token, Element* pElement, const char* attrName, std::size_t uiLen)
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
bool XmlParser::ParseAttributeNumber(size_t& num, Element* pElement, const char* attrName, std::size_t uiLen)
{
    std::string token = "";
    if (!ParseAttributeString(token, pElement, attrName, uiLen))
        return false;
    
    try
    {
        num = std::stoi(token);
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
bool XmlParser::ParseAttributeArray(std::vector<size_t>& arr, Element* pElement, const char* attrName, std::size_t uiLen)
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
            size_t number = std::stoi(subStr);
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

}
