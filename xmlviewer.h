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
public:
    xmlViewer()
    {
        doc = xmlReadFile("output.xml", NULL, 0);
        if (doc == nullptr)
        {
            std::cerr << "XML file could not be loaded." << std::endl;
            throw ERR_XML_LOAD;
        }
    };

    xmlDocPtr* docPtr() { return &doc; }

    ~xmlViewer()
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
};
