#include "xmlviewer.h"

std::string xmlViewer::dumpXML()
{
    xmlDocDumpFormatMemory(doc, &xmlBuff, &bufferSize, 1);
    return static_cast<std::string>((char*) xmlBuff);
}

void xmlViewer::loadTree()
{
    //TODO error checking
    root = xmlDocGetRootElement(doc);
    elements(root);
}

void xmlViewer::getEntry(xmlNode *inputNode)
{
    xmlNodePtr children = inputNode->children;
    xmlNodePtr node;
    std::string text, sBegin = "", sEnd = "";
    for (node = children; node != nullptr; node = node->next)
    {
        std::string name = (char*)node->name;

        if (name == "updated" && m_args->getAktualizace())
        {
            sBegin = "Aktualizace: ";
            //sEnd = "\n";
        }
        else if (name == "title") {}
        else if (name == "id" && m_args->getUrl())
        {
            sBegin = "URL: ";
            //sEnd = "\n";
        }
        else
        {
            continue;
        }

        if (node->children != nullptr && node->children->content != nullptr)
        {
            text = (char*) node->children->content;
            std::cout << sBegin << text << sEnd << std::endl;
        }

    }
}

void xmlViewer::elements(xmlNode *inputNode)
{
    xmlNodePtr node;
    for (node = inputNode->children; node; node = node->next)
    {
        if (node->children != nullptr && node->children->content != nullptr)
        {
            std::string name = (char*)node->name;

            if (name == "title")
            {
                std::cout << "*** " << node->children->content << " ***" << std::endl;
            }
            else if (name == "entry")
            {
                getEntry(node);
                if (m_args->getLatest()) break;
            }
            else if (name == "author" && m_args->getAutor())
            {
                xmlNodePtr temp;
                for (temp = node->children; temp != nullptr; temp = temp->next)
                {
                    if (temp->children != nullptr && temp->children->content != nullptr)
                    {
                        if ((std::string)(char*)temp->name == "name")
                        {
                            std::cout << "Autor: " << (char*) temp->children->content << std::endl;
                        }
                    }

                }
            }
        }
    }
    if (m_args->getFeedfileUsed()) std::cout << std::endl;
}
