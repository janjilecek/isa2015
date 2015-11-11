#ifndef XMLVIEWER
#define XMLVIEWER

#include "shared.h"
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include "cmdArguments.hpp"

#endif // XMLVIEWER

//class Arguments;
typedef enum
{
    ERR_XML_LOAD
} xml_errors;

class xmlViewer
{
private:
    xmlDocPtr doc;
    xmlChar *xmlBuff;
    int bufferSize;
    xmlNode *root;
    Arguments *m_args;
public:
    xmlViewer(Arguments *args) : m_args(args)
    {
        std::cout << "Loading file output.xml" << std::endl;
        doc = xmlReadFile("output.xml", NULL, 0);
        if (doc == nullptr)
        {
            std::cerr << "XML file could not be loaded." << std::endl;
            throw ERR_XML_LOAD;
        }
    };

    std::string dumpXML();

    void loadTree();

    void elements(xmlNode *inputNode);

    std::vector<std::string> getEntry(xmlNode *inputNode);

    ~xmlViewer()
    {
        //xmlFree(xmlBuff);
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
};
