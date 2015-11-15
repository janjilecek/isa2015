#include <iostream>
#include "downloader.hpp"
#include "cmdArguments.hpp"
#include "datadownloader.hpp"

using namespace std;

int DL(Arguments args, std::string mainUrl);

int main(int argc, char **argv)
{
    Arguments args;
    args.parseArgs(argc, argv);

    try
    {
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
            else throw std::runtime_error("Error - Feedfile is empty.");
        }
        else
        {
            return DL(args, args.sUrl());
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}


int DL(Arguments args, std::string mainUrl)
{
    UrlDetail urldet(mainUrl);

    try
    {
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
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    xmlViewer xmlView(&args);
    xmlView.loadTree();
    return 0;
}

