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

std::vector<std::string> xmlViewer::getEntry(xmlNode *inputNode)
{
    std::vector<std::string> output;
    xmlNodePtr children = inputNode->children;
    xmlNodePtr node;
    std::string text, sBegin = "";
    bool endLine = false;
    for (node = children; node != nullptr; node = node->next)
    {
        std::string name = (char*)node->name;

        if (name == "title") {}
        else if (name == "updated" && m_args->getAktualizace())
        {
            sBegin = "Aktualizace: ";
            endLine = true;
        }
        else if (name == "id" && m_args->getUrl())
        {
            sBegin = "URL: ";
            endLine = true;
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
                        sBegin = "Autor: ";
                        endLine = true;
                        text = (char*) temp->children->content;
                    }
                }
            }
        }
        else
        {
            continue;
        }

        if (node->children != nullptr && node->children->content != nullptr)
        {
            if (name != "author")
            {
                text = (char*) node->children->content;
            }
            std::ostringstream oss;
            oss <<  sBegin << text; //assemble the text
            if (name == "title")
            {
                output.insert(output.begin(), oss.str()); //the title needs to be the first printed item
            }
            else
            {
                output.push_back(oss.str()); //other items can be printed after
            }
        }
        sBegin = "";
    }
    if (endLine) output.push_back(""); //push one more line if any arguments were specified
    return output;
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
                for (auto &line : getEntry(node)) //call getEntry function and print every line of the returned vector
                {
                    std::cout << line << std::endl;
                }
                if (m_args->getLatest()) break;
            }

        }
    }
    if (m_args->getFeedfileUsed()) std::cout << std::endl;
}
