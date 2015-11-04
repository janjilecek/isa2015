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
    xmlNode *root;
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

    std::string dumpXML();

    void loadTree();

    void elements(xmlNode *inputNode);

    void getEntry(xmlNode *inputNode);

    ~xmlViewer()
    {
        //xmlFree(xmlBuff);
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
};
