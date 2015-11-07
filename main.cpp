#include <iostream>
#include "downloader.hpp"
#include "test.h"
#include "xmlviewer.h"
#include "cmdArguments.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Arguments args;
    args.parseArgs(argc, argv);
    Downloader downloader(args.sUrl());
    downloader.MakeUnsecuredConn();

    xmlViewer xmlView(&args);
    std::cout << xmlView.dumpXML() << std::endl;
    xmlView.loadTree();
}
