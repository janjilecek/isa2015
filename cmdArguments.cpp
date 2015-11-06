#include "cmdArguments.hpp"

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



