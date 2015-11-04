#ifndef XMLVIEWER
#define XMLVIEWER

#include "shared.h"
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

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
public:
    xmlViewer()
    {
        std::cout << "Loading file output.xml" << std::endl;
        doc = xmlReadFile("output.xml", NULL, 0);
        if (doc == nullptr)
        {
            std::cerr << "XML file could not be loaded." << std::endl;
            throw ERR_XML_LOAD;
        }
    };

    xmlDocPtr* docPtr() { return &doc; }

    std::string dumpXML();

    ~xmlViewer()
    {
        xmlFreeDoc(xmlBuff);
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
};
