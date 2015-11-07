#include <iostream>
#include "downloader.hpp"
#include "cmdArguments.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Arguments args;
    args.parseArgs(argc, argv);
    Downloader downloader(args.sUrl(), &args);
    downloader.download();


}
