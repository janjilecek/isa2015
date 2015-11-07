#include "cmdArguments.hpp"


void Arguments::parseArgs(int argc, char **argv)
{
    while ((opt = getopt(argc, argv, "uaTl")) != -1)
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
        default:
            printHelp();
            exit(EXIT_FAILURE);
        }
    }


}


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


bool Arguments::getAktualizace() const
{
return aktualizace;
}

void Arguments::setAktualizace(bool value)
{
aktualizace = value;
}
bool Arguments::getAutor() const
{
return autor;
}

void Arguments::setAutor(bool value)
{
autor = value;
}
bool Arguments::getUrl() const
{
return url;
}

void Arguments::setUrl(bool value)
{
url = value;
}
bool Arguments::getLatest() const
{
    return latest;
}

void Arguments::setLatest(bool value)
{
    latest = value;
}
bool Arguments::getFeedfileUsed() const
{
    return feedfileUsed;
}

void Arguments::setFeedfileUsed(bool value)
{
    feedfileUsed = value;
}



