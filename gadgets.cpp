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
std::string urlParser::query() const
{
    return query_;
}

void urlParser::setQuery(const std::string &query)
{
    query_ = query;
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


void Gadgets::printError(std::string msg)

{
    std::cerr << msg << std::endl;
}

bool Gadgets::contains_substring(std::string& input, std::string seq)
{
    auto result = std::search(input.begin(), input.end(), seq.begin(), seq.end());
    return (result != input.end()) ? true : false;
}

int Gadgets::find_chunked(std::string& content)
{
    int retVal = 0;
    if (contains_substring(content, "Transfer-Encoding: chunked"))
    {
        retVal = -1;
        return retVal;
    }
    else
    {
        // find content length
        std::string seq = "Content-Length: ";
        std::string endl = "\r\n";
        if (contains_substring(content, seq))
        {
            auto start = std::search(content.begin(), content.end(), seq.begin(), seq.end());
            auto end = std::search(start, content.end(), endl.begin(), endl.end());
            std::string num(start+seq.length(), end);
            std::stringstream ss(num);
            ss >> retVal;
            return retVal;
        }
    }
    // returns content length if available, else returns -1 (chunked)
    return retVal;
}

void Gadgets::loadFileIntoVector(string filename, std::vector<string> *urls)
{
    string line;
    ifstream feedfile(filename);
    if (feedfile.is_open())
    {
        while (getline(feedfile, line))
        {
            if (line.find("#") != 0) // if string doesnt start with hash
            {
                (*urls).push_back(line);
            }
        }
        feedfile.close();
    }
    else
    {
        throw std::runtime_error("Non existent feedfile");
    }
}



int UrlDetail::port() const
{
    return m_port;
}

std::string UrlDetail::path() const
{
    return m_path;
}

std::string UrlDetail::server() const
{
    return m_server;
}


std::string UrlDetail::query() const
{
    return m_query;
}

string UrlDetail::file() const
{
    ostringstream oss;
    oss << m_path << "?" << m_query;
    return oss.str();
}

UrlDetail::UrlDetail(std::string url)
{
    urlParser up(url);
    m_server = up.host();
    m_query = up.query();
    m_path = up.path();
    m_port = 80;
    if (up.protocol() == "https")
    {
        m_port = 443;
    }
}
