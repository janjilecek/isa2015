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
   // m_path = "/dailydose/dailydose_atom.xml";
}

int Downloader::send_request()
{
    std::string query = httpRequest(m_url, m_path);

    auto tmp = send(m_sock, query.c_str(), query.length(), 0);
    if (tmp == -1)
    {
        close(m_sock);
        std::cerr << "error while sending request" << std::endl;
        throw ERR_QUERY;
    }
}

int Downloader::my_connect()
{
    auto a = connect(m_sock, (struct sockaddr*) &m_server, sizeof(sockaddr_in)); // připojení k serveru
    if (a == -1)
    {
        close(m_sock);
        throw ERR_QUERY;
    }
}

std::string Downloader::httpRequest(std::string server, std::string path)
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

int Downloader::read_data(void *str, const std::string& end)
{
}

 bool Downloader::contains_substring(std::string& input, std::string seq)
 {
     auto result = std::search(input.begin(), input.end(), seq.begin(), seq.end());
     return (result != input.end()) ? true : false;
 }


int Downloader::initiateConnection(int port)
{
    m_port = port;
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sock < 0)
    {
        std::cerr << "Socket init. failed." << std::endl;
        throw ERR_SOCKET;
    }

    std::cout << m_url<< std::endl;
    m_info = gethostbyname(m_url.c_str());
    if (m_info == nullptr)
    {
        std::cerr << "IP translation failed." << std::endl;
        throw ERR_IP;
    }

    addresses = (struct in_addr **) m_info->h_addr_list; // získání ip
    for (int i = 0; addresses[i] != NULL; ++i) m_ip.assign(inet_ntoa(*addresses[i]));
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(m_port);
    m_addr.s_addr = inet_addr(m_ip.c_str());
    m_server.sin_addr = m_addr;

    std::cout << m_ip << std::endl;
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

int Downloader::get_headers()
{
    my_connect();
    send_request();

    char buffer[1];
    int count;
    std::string endHeaderSequence = "\r\n\r\n";
    std::string endLine = "\r\n";
    std::string firstLine = "";
    bool doOnce = true;
    while (errno == EINTR || (errno = 0, (count = recv(m_sock, buffer, sizeof(buffer), 0)) > 0))
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
//std::cout << m_headers;
    return 0;
}

std::string Downloader::get_content()
{
    std::string line;
    if (m_chunked)
    {
        std::vector<char> data;
        unsigned long chunkSize = 1;
        while (chunkSize - 2)
        {
            line = get_line();
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); // EDIT THIS
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            try
            {
                chunkSize = std::stol(line, nullptr, 16) + 2;
            }
            catch (std::invalid_argument& e)
            {
                std::cerr << e.what() << std::endl;
            }

            if (chunkSize - 2)
            {
                std::vector<char> tmp;
                read_bytes(tmp, chunkSize);
                data.insert(data.end(), tmp.begin(), tmp.end());
            }
        }
        return std::string(data.begin(), data.end());
    }
    else
    {
        std::string out = "";
        std::vector<char> tmp;
        read_bytes(tmp, m_contentLength);
        out.insert(out.end(), tmp.begin(), tmp.end());
        return out;
    }
}

int Downloader::MakeUnsecuredConn()
{
    splitUrl();
    initiateConnection(80);
    get_headers();
    std::string s =  get_content();
    std::cout << s;
    return 0;
}
