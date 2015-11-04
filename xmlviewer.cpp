#include "xmlviewer.h"

std::string xmlViewer::dumpXML()
{
    xmlDocDumpFormatMemory(doc, &xmlBuff, &bufferSize, 1);
    return static_cast<std::string>((char*) xmlBuff);
}

void xmlViewer::loadTree()
{
    root = xmlDocGetRootElement(doc);
    elements(root);
}

void xmlViewer::getEntry(xmlNodePtr inputNode)
{
    auto node = inputNode->children;
    xmlNodePtr temp;
    std::string name, sBegin = "", sEnd = "";
    for (temp = node; temp != nullptr; temp = temp->next)
    {
        auto print = temp->children->content;
        name = (char*) temp->name;
        if (name == "id")
        {
            sBegin = "URL: "; sEnd = "\n";
        }
        else if (name == "updated")
        {
            sBegin = "Aktualizace: "; sEnd = "\n";
        }
        std::cout << sBegin << print << sEnd << std::endl;
    }
}

void xmlViewer::elements(xmlNode *inputNode)
{
    xmlNodePtr node;
    for (node = inputNode->children; node; node = node->next)
    {

        //std::cout << "node type: Element, name:" << node->name << std::endl;
        std::string name = (char*)node->name;
        std::cout << name << std::endl;
        if (name == "title")
        {
            std::cout << "*** " << node->children->content << " ***" << std::endl;
        }
        else if (name == "entry")
        {
            getEntry(node);
            //std::cout << "asdsad";
        }


    }
}
