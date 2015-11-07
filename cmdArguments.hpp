#ifndef CMDARGUMENTS_HPP
#define CMDARGUMENTS_HPP

#include <unistd.h>
#include <cstdlib>
#include <iostream>

class Arguments
{
public:
    Arguments() : aktualizace(false), autor(false), url(false), latest(false), feedfileUsed(false) {}
    bool getAktualizace() const;
    void setAktualizace(bool value);

    bool getAutor() const;
    void setAutor(bool value);

    bool getUrl() const;
    void setUrl(bool value);

    bool getLatest() const;
    void setLatest(bool value);

    bool getFeedfileUsed() const;
    void setFeedfileUsed(bool value);

    void parseArgs(int argc, char **argv);
    void printHelp();
private:
    int opt;
    bool aktualizace;
    bool autor;
    bool url;
    bool latest;
    bool feedfileUsed;
};


#endif // CMDARGUMENTS_HPP



