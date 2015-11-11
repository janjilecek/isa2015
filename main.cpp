#include <iostream>
#include "downloader.hpp"
#include "cmdArguments.hpp"
#include "datadownloader.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Arguments args;
    args.parseArgs(argc, argv);
    UrlDetail urldet(args.sUrl());

    if (urldet.port() == 80)
    {
        HTTP downloader(urldet.server(), urldet.path());
        downloader.download();
    }
    else
    {
        HTTPS downloader(urldet.server(), urldet.path(), &args);
        downloader.download();
    }


    xmlViewer xmlView(&args);
    //std::cout << xmlView.dumpXML() << std::endl;
    xmlView.loadTree();

}
