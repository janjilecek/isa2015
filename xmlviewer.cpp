#include "xmlviewer.h"
/**
 * @brief xmlViewer::dumpXML
 * @return
 */
std::string xmlViewer::dumpXML()
{
    xmlDocDumpFormatMemory(doc, &xmlBuff, &bufferSize, 1);
    return static_cast<std::string>((char*) xmlBuff);
}
/**
 * @brief xmlViewer::loadTree
 */
void xmlViewer::loadTree()
{
    root = xmlDocGetRootElement(doc);
    elements(root);
}
/**
 * @brief xmlViewer::getEntry
 * @param inputNode
 * @return vector of strings, lines containing the output
 */
std::vector<std::string> xmlViewer::getEntry(xmlNode *inputNode)
{
    std::vector<std::string> output; // prepare for the lines
    xmlNodePtr children = inputNode->children; // all of the children
    xmlNodePtr node;
    std::string text, sBegin = "";
    bool endLine = false; // default we will not print the last new line
    for (node = children; node != nullptr; node = node->next) // for every existing child
    {
        std::string name = (char*)node->name; // find the name

        if (name == "title") {} // if it is title, do nothing
        else if (name == "updated" && m_args->getAktualizace()) // if it is the time and we want to print int, add it
        {
            sBegin = "Aktualizace: ";
            endLine = true;
        }
        else if (name == "id" && m_args->getUrl()) // if it is the url and we want to print it, add it
        {
            sBegin = "URL: ";
            endLine = true;
        }
        else if (name == "author" && m_args->getAutor()) // if it is the author, traverse through its children and find his name
        {
            xmlNodePtr temp;
            for (temp = node->children; temp != nullptr; temp = temp->next) // children of the node
            {
                if (temp->children != nullptr && temp->children->content != nullptr) // if it contains something
                {
                    if ((std::string)(char*)temp->name == "name") // if it is name
                    {
                        sBegin = "Autor: "; // add it
                        endLine = true;
                        text = (char*) temp->children->content;
                    }
                }
            }
        }
        else
        {
            continue; // in every other case, do not add anything
        }

        if (node->children != nullptr && node->children->content != nullptr)
        {
            if (name != "author") // if the name is not author, proceed normaly
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
/**
 * @brief xmlViewer::elements
 * @param inputNode
 */
void xmlViewer::elements(xmlNode *inputNode)
{
    xmlNodePtr node;
    std::vector<std::string> testing; // testing, we will test if it is a real Atom response
    for (node = inputNode->children; node; node = node->next) // for every existing child
    {
        std::string name = (char*)node->name; // save the name
        if (node->children != nullptr) // if the node has children
        {
            name = (char*)node->name; // save the name again

            if (name == "title") // if it is the title, print it with stars
            {
                std::cout << "*** " << node->children->content << " ***" << std::endl;
                testing.push_back((char*)node->children->content);
            }
            else if (name == "entry") // if it is an entry, print every returned line from getEntry
            {
                for (auto &line : getEntry(node)) //call getEntry function and print every line of the returned vector
                {
                    std::cout << line << std::endl;
                    testing.push_back(line);
                }
                if (m_args->getLatest()) break; // if we print only the latest, break after the first one
            }

        }
    }
    if (m_args->getFeedfileUsed()) std::cout << std::endl; // if we used the feedfile, print another line between sources
    if (testing.size() == 0) std::cerr << "Error - Not an Atom feed" << std::endl;
}
