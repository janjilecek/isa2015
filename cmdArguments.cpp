#include "cmdArguments.hpp"

/**
 * @brief Arguments::parseArgs
 * @param argc
 * @param argv
 */
void Arguments::parseArgs(int argc, char **argv)
{
    if (argc == 1)
    {
        printHelp();
        exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, "uaTlf:c:C:")) != -1) // specified arguments with options or without
    {
        switch (opt)
        {
        case 'T':
            setAktualizace(true);
            break;
        case 'a':
            setAutor(true);
            break;
        case 'u':
            setUrl(true);
            break;
        case 'l':
            setLatest(true);
            break;
        case 'f':
            setFeedfileUsed(true);
            m_sFeedFile = optarg;
            break;
        case 'c':
            setCertfileUsed(true);
            m_sCertFileName = optarg;
            break;
        case 'C':
            setCertfileFolderUsed(true);
            m_sCertFilesFolder = optarg;
            break;
        default:
            printHelp();
            exit(EXIT_FAILURE);
        }
    }

    int countNonOption = 0;
    if (optind < argc)
    {
        while (optind < argc)
        {
            if (countNonOption >= 1)
            {
                std::cerr << "Bad argument combination." << std::endl;
                exit(EXIT_FAILURE);
            }
            countNonOption++;
            m_sUrl = argv[optind++]; // save the url address
            setUrlUsed(true);
        }
    }

    if (getFeedfileUsed() && getUrlUsed()) // bad combination
    {
        std::cerr << "Can't combine URL and FeedFile" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (getCertfileFolderUsed() && getCertfileUsed()) // bad combination
    {
        std::cerr << "Can't combine Certificate Folder and Certificate file" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (feedfileUsed) // if we use feedfile, we load the urls into a vector
    {
        Gadgets g;
        g.loadFileIntoVector(sFeedFile(), &urls);
    }


}

/**
 * @brief Arguments::getUrlUsed
 * @return true if url is specified
 */
bool Arguments::getUrlUsed() const
{
    return urlUsed;
}

/**
 * @brief Arguments::setUrlUsed
 * @param value
 */
void Arguments::setUrlUsed(bool value)
{
    urlUsed = value;
}

/**
 * @brief Arguments::sUrl
 * @return string with url
 */
std::string Arguments::sUrl() const
{
    return m_sUrl;
}

/**
 * @brief Arguments::setSUrl
 * @param sUrl
 */
void Arguments::setSUrl(const std::string &sUrl)
{
    m_sUrl = sUrl;
}

/**
 * @brief Arguments::sCertFileName
 * @return string with certificate filename
 */
std::string Arguments::sCertFileName() const
{
    return m_sCertFileName;
}

/**
 * @brief Arguments::setSCertFileName
 * @param sCertFileName
 */
void Arguments::setSCertFileName(const std::string &sCertFileName)
{
    m_sCertFileName = sCertFileName;
}

/**
 * @brief Arguments::sCertFilesFolder
 * @return string with the name of the certificates folder
 */
std::string Arguments::sCertFilesFolder() const
{
    return m_sCertFilesFolder;
}

/**
 * @brief Arguments::setSCertFilesFolder
 * @param sCertFilesFolder
 */
void Arguments::setSCertFilesFolder(const std::string &sCertFilesFolder)
{
    m_sCertFilesFolder = sCertFilesFolder;
}

/**
 * @brief Arguments::sFeedFile
 * @return string with the filename of the feedfile
 */
std::string Arguments::sFeedFile() const
{
    return m_sFeedFile;
}

/**
 * @brief Arguments::setSFeedFile
 * @param sFeedFile
 */
void Arguments::setSFeedFile(const std::string &sFeedFile)
{
    m_sFeedFile = sFeedFile;
}

/**
 * @brief Arguments::getUrls
 * @return vector with strings of urls
 */
std::vector<std::string> Arguments::getUrls() const
{
    return urls;
}

/**
 * @brief Arguments::printHelp
 */
void Arguments::printHelp()
{
    std::cout << "Pouziti: arfeed http://tools.ietf.org/agenda/atom | -f feedfile [-c certfile] [-C certaddr] [-l] [-T] [-a] [-u]" << std::endl <<
                 "" << std::endl <<
                 "Poradi parametru je libovolne. Popis parametru:" << std::endl <<
                 "" << std::endl <<
                 "    Povinne je uveden bud nazev (URL) pozadovaneho zdroje, nebo parametr -f s dodatecnym parametrem urcujiciho umisteni souboru feedfile. Soubor feedfile je textovy soubor, kde je na kazdem radku uvedena jedna adresa zdroje ve formatu Atom." << std::endl <<
                 "    Volitelny parametr -c soubor s certifikaty, ktery se pouzije pro overeni platnosti certifikatu SSL/TLS serveru." << std::endl <<
                 "    Volitelny parametr -C urcuje adresar, ve kterem se maji vyhledavat certifikaty, ktere se pouziji pro overeni platnosti certifikatu SSL/TLS serveru. Vychozi hodnota je /etc/ssl/certs." << std::endl <<
                 "    Pri spusteni s parametrem -l se pro kazdy zdroj budou vypisovat pouze informace o nejnovejsim zaznamu." << std::endl <<
                 "    Pri spusteni s parametrem -T se pro kazdy zaznam zobrazi navic informace o case zmeny zaznamu (je-li ve stazenem souboru obsazeno)." << std::endl <<
                 "    Pri spusteni s parametrem -a se pro kazdy zaznam zobrazi jmeno autora (je-li ve stazenem souboru obsazeno)." << std::endl <<
                 "    Pri spusteni s parametrem -u se pro kazdy zaznam zobrazi asociovane URL (je-li ve stazenem souboru obsazeno)." << std::endl;
}
/**
 * @brief Arguments::getCertfileUsed
 * @return true if certifcate file is used
 */
bool Arguments::getCertfileUsed() const
{
    return certfileUsed;
}

/**
 * @brief Arguments::setCertfileUsed
 * @param value
 */
void Arguments::setCertfileUsed(bool value)
{
    certfileUsed = value;
}

/**
 * @brief Arguments::getCertfileFolderUsed
 * @return true if certificate folder is used
 */
bool Arguments::getCertfileFolderUsed() const
{
    return certfileFolderUsed;
}

/**
 * @brief Arguments::setCertfileFolderUsed
 * @param value
 */
void Arguments::setCertfileFolderUsed(bool value)
{
    certfileFolderUsed = value;
}

/**
 * @brief Arguments::getAktualizace
 * @return true if we will print time
 */
bool Arguments::getAktualizace() const
{
    return aktualizace;
}

/**
 * @brief Arguments::setAktualizace
 * @param value
 */
void Arguments::setAktualizace(bool value)
{
    aktualizace = value;
}

/**
 * @brief Arguments::getAutor
 * @return trrue if we print author
 */
bool Arguments::getAutor() const
{
return autor;
}

/**
 * @brief Arguments::setAutor
 * @param value
 */
void Arguments::setAutor(bool value)
{
autor = value;
}

/**
 * @brief Arguments::getUrl
 * @return true if we print url
 */
bool Arguments::getUrl() const
{
return url;
}

/**
 * @brief Arguments::setUrl
 * @param value
 */
void Arguments::setUrl(bool value)
{
url = value;
}

/**
 * @brief Arguments::getLatest
 * @return true if we print only the latest
 */
bool Arguments::getLatest() const
{
    return latest;
}

/**
 * @brief Arguments::setLatest
 * @param value
 */
void Arguments::setLatest(bool value)
{
    latest = value;
}

/**
 * @brief Arguments::getFeedfileUsed
 * @return true if we used feedfile
 */
bool Arguments::getFeedfileUsed() const
{
    return feedfileUsed;
}

/**
 * @brief Arguments::setFeedfileUsed
 * @param value
 */
void Arguments::setFeedfileUsed(bool value)
{
    feedfileUsed = value;
}



