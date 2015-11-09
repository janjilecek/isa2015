#include "downloader.hpp"
#include "gadgets.h"


int Downloader::splitUrl()
{
    //regex
   /* std::regex expr(".*\/.*");
    if (std::regex_match(m_url, expr))
    {
        std::cout << "Its an URL\n";
    }*/

    m_path = "/httpgallery/chunked/chunkedimage.aspx";
    m_path = "/dailydose/dailydose_atom.xml";
    m_path = "/atom.xml";
    m_path = "/data_centre/headlines.atom";
    return 0;
}

int Downloader::send_request()
{
    std::string query = makeHeaders(m_url, m_path);

    auto tmp = BIO_write(bio, query.c_str(), query.length());
    if (tmp <= 0)
    {
        std::cerr << "error while sending request" << std::endl;
        throw ERR_QUERY;
    }
    return 0;
}

int Downloader::my_connect()
{
    auto a = connect(m_sock, (struct sockaddr*) &m_server, sizeof(sockaddr_in)); // připojení k serveru
    if (a == -1)
    {
        close(m_sock);
        throw ERR_QUERY;
    }
    return 0;
}

std::string Downloader::makeHeaders(std::string server, std::string path)
{
    std::string query;
    std::ostringstream oss;
    oss << "GET "
        << path
        << " HTTP/1.1\r\nHost: "
        << server
        << "\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:40.0) Gecko/20100101 Firefox/40.0\r\n"
        << "Accept: */*\r\n"
        << "Referer: \r\n"
        << "Connection: close\r\n\r\n";

    std::string headers = oss.str();
    return headers;
}

 bool Downloader::contains_substring(std::string& input, std::string seq)
 {
     auto result = std::search(input.begin(), input.end(), seq.begin(), seq.end());
     return (result != input.end()) ? true : false;
 }


int Downloader::initiateConnection()
{
    char* cpTemp = (char *)bioAddrStr.c_str();
    bio = BIO_new_connect(cpTemp);
    if (bio == NULL)
    {
        throw ERR_BIO_CREATE;
    }

    if (BIO_do_connect(bio) <= 0)
    {
        throw ERR_BIO_CONN;
    }
    return 0;
}



int Downloader::httpResponseCheck(std::string& in)
{
    std::stringstream sstream;
    std::string http;
    short status;
    sstream << in;
    sstream >> http;
    sstream >> status;
    if (status == 0)
    {
        throw NOT_HTTP;
    }
    return 0;
}

int Downloader::find_chunked(std::string& content)
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

void Downloader::download()
{
    if (m_port == 443)
    {
        MakeSecureConn();
    }
    else
    {
        MakeUnsecuredConn();
    }
    xmlViewer xmlView(m_args);
    //std::cout << xmlView.dumpXML() << std::endl;
    xmlView.loadTree();
}

int Downloader::get_headers()
{

    char buffer[1];
    int count;
    std::string endHeaderSequence = "\r\n\r\n";
    std::string endLine = "\r\n";
    std::string firstLine = "";
    bool doOnce = true;
    while (errno == EINTR || (errno = 0, (count = BIO_read(bio, buffer, 1)) > 0))
    {
        if (count > 0)
        {
            m_headers.append(buffer, count);
            if (doOnce)
            {
                firstLine.append(buffer, count);
                if (contains_substring(firstLine, endLine))
                {
                    httpResponseCheck(firstLine);
                    doOnce = false;
                }
            }

            if (contains_substring(m_headers, endHeaderSequence))
            {
                int res = find_chunked(m_headers); // determine if chunked
                if (res == -1) m_chunked = true;
                else m_contentLength = res; // or to use content-length
                break;
            }
        }
    }
    if (count < 0)
    {
        std::cerr << "recv error" << std::endl;
        throw ERR_RECV;
    }
    return 0;
}

std::string Downloader::get_content()
{
    std::string line;
    if (m_chunked)
    {
        short endLnSize = 2;
        std::vector<char> data;
        unsigned long chunkSize = 1;
        while (chunkSize - endLnSize)
        {
            line = get_line();
            try
            {
                chunkSize = std::stol(line, nullptr, 16) + endLnSize;
            }
            catch (std::invalid_argument& e)
            {
                std::cerr << e.what() << std::endl;
                throw ERR_CHUNKED;
            }

            if (chunkSize - endLnSize)
            {
                std::vector<char> tmp;
                read_bytes(tmp, chunkSize);
                data.insert(data.end(), tmp.begin(), tmp.end() - endLnSize);
            }
        }
        return std::string(data.begin(), data.end());
    }
    else
    {
        std::string out = "";
        std::vector<char> tmp;
        read_bytes(tmp, m_contentLength);
        out.append(tmp.begin(), tmp.end());
        return out;
    }
    return "";
}

int Downloader::MakeUnsecuredConn()
{
    initiateConnection();
   // my_connect();
    send_request();
    get_headers();
    std::string s =  get_content();

    std::ofstream out("output.xml");
    out << s;
    out.close();
    return 0;
}

int Downloader::MakeSecureConn()
{
    BIO_set_conn_hostname(bio, bioAddrStr.c_str()); // bioAddrStr in format hostname:port
    if (BIO_do_connect(bio) <= 0)
    {
        throw SSL_CONN_FAILED;
    }
    if (SSL_get_verify_result(ssl) != X509_V_OK)
    {
        throw SSL_CERTCHECK_FAIL;
    }

    std::string headers = makeHeaders(m_url, m_path);

    if (BIO_write(bio, headers.c_str(), headers.length()) <= 0)
    {
        throw SSL_FAILED_SEND;
    }
    else
    {
        get_headers();

    }

    return 0;
}
