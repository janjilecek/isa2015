#include "datadownloader.hpp"

/*
 * initiateConnection();
    my_connect();
    send_request();
    get_headers();
    std::string s =  get_content();

    std::ofstream out("output.xml");
    out << s;
    out.close();
    return 0;
 */

int HTTP::initiateConnection()
{
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

int HTTP::my_connect()
{
    auto a = connect(m_sock, (struct sockaddr*) &m_server, sizeof(sockaddr_in)); // připojení k serveru
    if (a == -1)
    {
        close(m_sock);
        throw ERR_QUERY;
    }
    return 0;
}


int HTTP::send_request()
{
    std::string query = makeHeaders(m_url, m_path);

    auto tmp = send(m_sock, query.c_str(), query.length(), 0);
    if (tmp == -1)
    {
        close(m_sock);
        std::cerr << "error while sending request" << std::endl;
        throw ERR_QUERY;
    }
    return 0;
}

int HTTP::get_headers()
{

    char buffer[1];
    int count;
    std::string endHeaderSequence = "\r\n\r\n";
    std::string endLine = "\r\n";
    std::string firstLine = "";
    bool doOnce = true;
    while (errno == EINTR || (errno = 0, (count = recv(m_sock, buffer, 1, 0)) > 0))

    {
        if (count > 0)
        {
            m_headers.append(buffer, count);
            if (doOnce)
            {
                firstLine.append(buffer, count);
                if (Gadgets::contains_substring(firstLine, endLine))
                {
                    httpResponseCheck(firstLine);
                    doOnce = false;
                }
            }

            if (Gadgets::contains_substring(m_headers, endHeaderSequence))
            {
                int res = Gadgets::find_chunked(m_headers); // determine if chunked
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


std::string HTTP::get_content()
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


int HTTP::download()
{
    initiateConnection();
    my_connect();
    send_request();
    get_headers();
    std::string s =  get_content();

    std::ofstream out("output.xml");
    out << s;
    out.close();

    return 0;
}

int HTTP::httpResponseCheck(std::string& in)
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


std::string HTTP::makeHeaders(std::string server, std::string path)
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

void HTTP::read_bytes(std::vector<char> &buffer, unsigned int size)
{
    if (buffer.size() < size) buffer.resize(size);
    if (recv(m_sock, static_cast<char*>(buffer.data()), size, 0x100) <= 0)
    {
        std::cerr << "read bytes err" << std::endl;
        throw ERR_RECV;
    }
}

std::string HTTP::read_sequence(std::vector<char> &buffer, const std::string &text)
{
    std::string test = "";
    do
    {
        char B;
        if (recv(m_sock, static_cast<char*>(&B), 1, 0x100) <= 0)
        {
            std::cerr  << "read seq err" << std::endl;
            throw ERR_RECV;
        }
        else
        {
            buffer.push_back(B);
        }
        test = std::string(buffer.begin(), buffer.end());
    } while (!Gadgets::contains_substring(test, text));
    return std::string(buffer.begin(), buffer.end());
}

std::string HTTP::get_line()
{
    std::vector<char> buffer;
    std::string a = read_sequence(buffer, "\r\n");
    return a;
}

HTTP::HTTP(std::string inServer, std::string inPath)
{
    m_url = inServer;
    m_path = inPath;
    m_port = 80;
    m_chunked = false;
    m_contentLength = -1;
    m_headers = "";

}


// HTTPS begin -----------------------------------------------------------------------------


int HTTPS::download()
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

HTTPS::HTTPS(std::string inServer, std::string inPath, Arguments *args) : HTTP(inServer, inPath)
{
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    m_port = 443;
    ctx = SSL_CTX_new(TLSv1_2_client_method());
    if (args->getCertfileUsed())
    {
        if (!SSL_CTX_load_verify_locations(ctx, args->sCertFileName().c_str(), NULL))
        {
            throw ERR_SSL_PEM;
        }
    }
    else if (args->getCertfileFolderUsed())
    {
        if (!SSL_CTX_load_verify_locations(ctx, NULL, args->sCertFilesFolder().c_str()))
        {
            throw ERR_SSL_FOLDER;
        }
    }
    else
    {

        if (!SSL_CTX_load_verify_locations(ctx, NULL, "/etc/ssl/certs/"))
        {
            throw ERR_SSL_FOLDER;
        }

    }

    bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    std::ostringstream oss;
    oss << m_url << ":" << m_port;
    bioAddrStr = oss.str();
}
