#include "gadgets.h"

char* strToChar(std::string s)
{
    char *cstr = &s[0u];
    return cstr;
}


using namespace std;

urlParser::urlParser(const string &url_s)
{
    parse(url_s);
}

//Source of url parser: http://stackoverflow.com/a/2616217, author: stackoverflow user wilhelmtell
void urlParser::parse(const string& url_s)
{
    const string prot_end("://");
    string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
                                           prot_end.begin(), prot_end.end());
    protocol_.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i,
              back_inserter(protocol_),
              ptr_fun<int,int>(tolower)); // protocol is icase
    if( prot_i == url_s.end() )
        return;
    advance(prot_i, prot_end.length());
    string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    host_.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i,
              back_inserter(host_),
              ptr_fun<int,int>(tolower)); // host is icase
    string::const_iterator query_i = find(path_i, url_s.end(), '?');
    path_.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    query_.assign(query_i, url_s.end());
}
std::string urlParser::path() const
{
    return path_;
}

void urlParser::setPath(const std::string &path)
{
    path_ = path;
}

std::string urlParser::host() const
{
    return host_;
}

void urlParser::setHost(const std::string &host)
{
    host_ = host;
}

std::string urlParser::protocol() const
{
    return protocol_;
}

void urlParser::setProtocol(const std::string &protocol)
{
    protocol_ = protocol;
}


