#include "XmlParser.h"
#include "rapidxml_utils.hpp"
#include <iostream>

using namespace std;

#define FILE_PATH "../TestAssets/Dump.xml"

void XmlParser::Parse()
{
    file<> xmlFile("Dump.xml");
    xml_document<> *doc = new xml_document<>();
    doc->parse<0>(xmlFile.data());

    cout << "Name of my first node is: " << doc->first_node()->name() << "\n";
    xml_node<>* node = doc->first_node("foobar");
    cout << "Node foobar has value " << node->value() << "\n";
    for (xml_attribute<>* attr = node->first_attribute();
        attr; attr = attr->next_attribute())
    {
        cout << "Node foobar has attribute " << attr->name() << " ";
        cout << "with value " << attr->value() << "\n";
    }





    delete doc;
}
