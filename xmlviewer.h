#ifndef XMLVIEWER
#define XMLVIEWER

#include "shared.h"
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include "cmdArguments.hpp"

#endif // XMLVIEWER

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
        doc = xmlReadFile("output.xml", NULL, 0);
        if (doc == nullptr)
        {
            throw ISAException("Error - XML file could not be loaded.");
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
