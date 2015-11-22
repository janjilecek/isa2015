#ifndef XMLVIEWER
#define XMLVIEWER

#include "shared.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "cmdArguments.hpp"

#endif // XMLVIEWER

/**
 * @brief The xmlViewer class
 */
class xmlViewer
{
private:
    xmlDocPtr doc;
    xmlChar *xmlBuff;
    int bufferSize;
    xmlNode *root;
    Arguments *m_args;
public:
    /**
     * @brief xmlViewer
     * @param args
     */
    xmlViewer(Arguments *args) : m_args(args)
    {
        xmlBuff = NULL;
        doc = xmlReadFile("output.xml", NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
        if (doc == NULL)
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
        if (xmlBuff)
        {
            xmlFree(xmlBuff);
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
};
