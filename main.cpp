#include <iostream>
#include "cmdArguments.hpp"
#include "datadownloader.hpp"

using namespace std;
/**
 * @brief DL
 * @param args
 * @param mainUrl
 * @return int
 */
int DL(Arguments args, std::string mainUrl);

/**
 * @brief main
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv)
{
    Arguments args; // new argument class
    args.parseArgs(argc, argv); // parse the arguments and save them

    try
    {
        if (args.getFeedfileUsed()) // if we used the feedfile
        {
            auto urls = args.getUrls(); // load the urls vector
            if (urls.size() > 0) // and if it contains some line
            {
                for (auto &lineUrl : urls) // for every single one
                {
                    DL(args, lineUrl); // call the download function
                }
            }
            else throw ISAException("Error - Feedfile is empty.");
        }
        else
        {
            return DL(args, args.sUrl()); // else download only the one url
        }
    }
    catch (ISAException &e)
    {
        if (e.what() != Gadgets::redirc)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unspecified exception. - " << e.what() << std::endl;
        return 1;
    }


    return 0;
}


int DL(Arguments args, std::string mainUrl)
{
    UrlDetail urldet(mainUrl); // parse the URL

    try
    {
        if (urldet.port() == 80) // if the port is 80
        {
            HTTP downloader(urldet.server(), urldet.file()); // create HTTP instance
            downloader.download();  // and download
        }
        else
        {
            HTTPS downloader(urldet.server(), urldet.file(), &args); // else create HTTPS instance
            downloader.download();
        }
    }
    catch (ISAException &e)
    {
        if (e.what() != Gadgets::redirc) // if the exceptions was not a redirection complete
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }


    xmlViewer xmlView(&args); // construct the xml object
    xmlView.loadTree(); // and begin parsing the XML (Atom, hopefully)
    return 0;
}

