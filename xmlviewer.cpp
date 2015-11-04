#include "xmlviewer.h"

std::string xmlViewer::dumpXML()
{
    xmlDocDumpFormatMemory(doc, &xmlBuff, &bufferSize, 1);
    return static_cast<std::string>((char*) xmlBuff);
}
