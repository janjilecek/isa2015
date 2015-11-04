#include <iostream>
#include "downloader.hpp"
#include "test.h"
#include "xmlviewer.h"

using namespace std;

int main()
{
    std::string url = "https://tools.ietf.org/dailydose/dailydose_atom.xml";
    url = "www.httpwatch.com";
    url = "tools.ietf.org";
    url = "xkcd.com";
    Downloader downloader(url);
    downloader.MakeUnsecuredConn();

    xmlViewer xmlView;
    //std::cout << xmlView.dumpXML() << std::endl;
    xmlView.loadTree();
}
