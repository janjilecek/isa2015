#ifndef GADGETS_H
#define GADGETS_H

#include "shared.h"
#include <algorithm>
#include <cctype>
#include <functional>

char* strToChar(std::string s);

//Source of url parser: http://stackoverflow.com/a/2616217, author: stackoverflow user wilhelmtell
struct urlParser {
    urlParser(const std::string& url_s); // omitted copy, ==, accessors, ...

public:
    std::string protocol() const;
    void setProtocol(const std::string &protocol);

    std::string host() const;
    void setHost(const std::string &host);

    std::string path() const;
    void setPath(const std::string &path);

    std::string query() const;
    void setQuery(const std::string &query);

private:
    void parse(const std::string& url_s);
private:
    std::string protocol_, host_, path_, query_;
};

class Gadgets
{
public:
    static void printError(std::string msg);
    static bool contains_substring(std::string& input, std::string seq);
    static int find_chunked(std::string& content);
    static std::string find_location_on_redirect(std::string& content);
    void loadFileIntoVector(std::string filename, std::vector<std::string> *urls);
    static std::string redirc;
};

class UrlDetail
{
private:
    int m_port;
    std::string m_path, m_server, m_query;
public:
    UrlDetail(std::string url);
    int port() const;
    std::string path() const;
    std::string server() const;
    std::string query() const;
    std::string file() const;
};


#endif // GADGETS_H


