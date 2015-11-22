#include "gadgets.h"

/**
 * @brief strToChar
 * @param s
 * @return char*
 */
char* strToChar(std::string s)
{
    char *cstr = &s[0u];
    return cstr;
}


using namespace std;

/**
 * @brief urlParser::urlParser
 * @param url_s
 */
urlParser::urlParser(const string &url_s)
{
    parse(url_s);
}

//Source of url parser: http://stackoverflow.com/a/2616217, author: stackoverflow user wilhelmtell
/**
 * @brief urlParser::parse
 * @param url_s
 */
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
/**
 * @brief urlParser::query
 * @return string with query
 */
std::string urlParser::query() const
{
    return query_;
}

/**
 * @brief urlParser::setQuery
 * @param query
 */
void urlParser::setQuery(const std::string &query)
{
    query_ = query;
}

/**
 * @brief urlParser::path
 * @return string with path
 */
std::string urlParser::path() const
{
    return path_;
}
/**
 * @brief urlParser::setPath
 * @param path
 */
void urlParser::setPath(const std::string &path)
{
    path_ = path;
}
/**
 * @brief urlParser::host
 * @return string with host
 */
std::string urlParser::host() const
{
    return host_;
}
/**
 * @brief urlParser::setHost
 * @param host
 */
void urlParser::setHost(const std::string &host)
{
    host_ = host;
}
/**
 * @brief urlParser::protocol
 * @return string with protocol
 */
std::string urlParser::protocol() const
{
    return protocol_;
}
/**
 * @brief urlParser::setProtocol
 * @param protocol
 */
void urlParser::setProtocol(const std::string &protocol)
{
    protocol_ = protocol;
}

/**
 * @brief Gadgets::printError
 * @param msg
 */
void Gadgets::printError(std::string msg)

{
    std::cerr << msg << std::endl;
}
/**
 * @brief Gadgets::redirc
 */
std::string Gadgets::redirc = "redirection complete";
/**
 * @brief Gadgets::contains_substring
 * @param input
 * @param seq
 * @return true if we found the desired substring
 */
bool Gadgets::contains_substring(std::string& input, std::string seq)
{
    auto result = std::search(input.begin(), input.end(), seq.begin(), seq.end());
    return (result != input.end()) ? true : false;
}
/**
 * @brief Gadgets::find_chunked
 * @param content
 * @return int, size in bytes if static, -1 if chunked
 */
int Gadgets::find_chunked(std::string& content)
{
    int retVal = 0;
    std::string backup = content;
    std::transform(content.begin(), content.end(), content.begin(), ::tolower);
    if (contains_substring(content, "transfer-encoding: chunked"))
    {
        retVal = -1;
        content = backup; // renew the backed-up copy of the header string
        return retVal;
    }
    else
    {
        // find content length
        std::string seq = "content-length: ";
        std::string endl = "\r\n";
        if (contains_substring(content, seq))
        {
            auto start = std::search(content.begin(), content.end(), seq.begin(), seq.end()); // beginning of the sequence
            auto end = std::search(start, content.end(), endl.begin(), endl.end()); // end of it
            std::string num(start+seq.length(), end); // we need only the value after the found text
            std::stringstream ss(num);
            ss >> retVal; // convert to int
            content = backup; // renew
            return retVal;
        }
    }
    // returns content length if available, else returns -1 (chunked)
    content = backup;
    return retVal;
}
/**
 * @brief Gadgets::find_location_on_redirect
 * @param content
 * @return string with address of the redirection
 */
string Gadgets::find_location_on_redirect(string &content)
{
    std::string seq = "Location: ";
    std::string endl = "\r\n";
    if (contains_substring(content, seq))
    {
        auto start = std::search(content.begin(), content.end(), seq.begin(), seq.end()); // begininning
        auto end = std::search(start, content.end(), endl.begin(), endl.end()); // the end
        std::string retStr(start+seq.length(), end); // only the location
        std::stringstream ss(retStr);
        return ss.str();
    }

    return "";
}
/**
 * @brief Gadgets::loadFileIntoVector
 * @param filename
 * @param urls
 */
void Gadgets::loadFileIntoVector(string filename, std::vector<string> *urls)
{
    string line;
    ifstream feedfile(filename.c_str());
    if (feedfile.is_open())
    {
        while (getline(feedfile, line))
        {
            if (line.find("#") != 0) // if string doesnt start with hash
            {
                (*urls).push_back(line); // add the line
            }
        }
        feedfile.close();
    }
    else
    {
        throw ISAException("Non existent feedfile");
    }
}


/**
 * @brief UrlDetail::port
 * @return int with port
 */
int UrlDetail::port() const
{
    return m_port;
}
/**
 * @brief UrlDetail::path
 * @return string with path
 */
std::string UrlDetail::path() const
{
    return m_path;
}
/**
 * @brief UrlDetail::server
 * @return string with server
 */
std::string UrlDetail::server() const
{
    return m_server;
}

/**
 * @brief UrlDetail::query
 * @return string with query
 */
std::string UrlDetail::query() const
{
    return m_query;
}
/**
 * @brief UrlDetail::file
 * @return string with filename
 */
string UrlDetail::file() const
{
    ostringstream oss;
    oss << m_path << "?" << m_query;
    return oss.str();
}
/**
 * @brief UrlDetail::UrlDetail
 * @param url
 */
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
