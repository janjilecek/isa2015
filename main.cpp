#include <iostream>
#include "downloader.hpp"
#include "test.h"
#include "xmlviewer.h"
#include "cmdArguments.hpp"

using namespace std;

int main(int argc, char **argv)
{
    std::string url = "https://tools.ietf.org/dailydose/dailydose_atom.xml";
    url = "www.httpwatch.com";
    url = "tools.ietf.org";
    url = "xkcd.com";
    url = "www.theregister.co.uk";
    Arguments args;
    args.parseArgs(argc, argv);
    Downloader downloader(url);
    downloader.MakeUnsecuredConn();

    xmlViewer xmlView(&args);
    //std::cout << xmlView.dumpXML() << std::endl;
    xmlView.loadTree();
}
