#include <iostream>
#include "downloader.hpp"
#include "test.h"

using namespace std;

int main()
{
    std::string url = "https://tools.ietf.org/dailydose/dailydose_atom.xml";
    url = "www.httpwatch.com";
    url = "tools.ietf.org";
    Downloader downloader(url);
    std::cout << downloader.MakeUnsecuredConn() << std::endl;
}
