#include <iostream>
#include "downloader.hpp"
#include "cmdArguments.hpp"
#include "datadownloader.hpp"

using namespace std;

void DL(Arguments args, std::string mainUrl);

int main(int argc, char **argv)
{
    Arguments args;
    args.parseArgs(argc, argv);

    if (args.getFeedfileUsed())
    {
        auto urls = args.getUrls();
        if (urls.size() > 0)
        {
            for (auto &lineUrl : urls)
            {
                DL(args, lineUrl);
            }
        }
        else throw NO_URLS;
    }
    else
    {
        DL(args, args.sUrl());
    }

    return 0;
}


void DL(Arguments args, std::string mainUrl)
{
    UrlDetail urldet(mainUrl);

    if (urldet.port() == 80)
    {
        HTTP downloader(urldet.server(), urldet.file());
        downloader.download();
    }
    else
    {
        HTTPS downloader(urldet.server(), urldet.file(), &args);
        downloader.download();
    }

    xmlViewer xmlView(&args);
    xmlView.loadTree();
}

