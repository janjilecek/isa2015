#include "datadownloader.hpp"

/**
 * @brief HTTP::initiateConnection
 * @return
 */
int HTTP::initiateConnection()
{
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sock < 0)
    {
        throw ISAException("Error - Socket init. failed.");
    }

    m_info = gethostbyname(m_url.c_str());
    if (m_info == nullptr)
    {
        throw ISAException("Error - IP translation failed.");
    }

    addresses = (struct in_addr **) m_info->h_addr_list; // získání ip
    for (int i = 0; addresses[i] != NULL; ++i) m_ip.assign(inet_ntoa(*addresses[i]));
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(m_port);
    m_addr.s_addr = inet_addr(m_ip.c_str());
    m_server.sin_addr = m_addr;

    auto a = connect(m_sock, (struct sockaddr*) &m_server, sizeof(sockaddr_in)); // připojení k serveru
    if (a == -1)
    {
        close(m_sock);
        throw ISAException("Error - Socket - Failed connecting to server");
    }
    return 0;
}

/**
 * @brief HTTP::send_request
 * @return
 */
int HTTP::send_request()
{
    std::string query = makeHeaders(m_url, m_path);

    auto tmp = send(m_sock, query.c_str(), query.length(), 0);
    if (tmp == -1)
    {
        close(m_sock);
        throw ISAException("Error - Sending request");
    }
    return 0;
}

/**
 * @brief HTTP::get_headers
 * @return
 */
int HTTP::get_headers()
{

    char buffer[1];
    int count;
    std::string endHeaderSequence = "\r\n\r\n";
    std::string endLine = "\r\n";
    std::string firstLine = "";
    bool doOnce = true;
    int resCheck = 0;
    while (errno == EINTR || (errno = 0, (count = receive(buffer, 1)) > 0))

    {
        if (count > 0)
        {
            m_headers.append(buffer, count);
            if (doOnce)
            {
                firstLine.append(buffer, count);
                if (Gadgets::contains_substring(firstLine, endLine))
                {
                    resCheck = httpResponseCheck(firstLine);
                    doOnce = false;
                }
            }

            if (Gadgets::contains_substring(m_headers, endHeaderSequence))
            {
                int res = Gadgets::find_chunked(m_headers); // determine if chunked
                if (res == -1) m_chunked = true;
                else m_contentLength = res; // or to use content-length

                if (resCheck == 301)
                {
                    std::string redirectUrl = Gadgets::find_location_on_redirect(m_headers);
                    UrlDetail urldet(redirectUrl);

                    if (m_maxRedir > 0)
                    {
                        if (urldet.port() == 80)
                        {
                            HTTP downloader(urldet.server(), urldet.file(), m_maxRedir - 1);
                            downloader.download();
                            throw ISAException(Gadgets::redirc);
                        }
                        else
                        {
                            HTTPS downloader(urldet.server(), urldet.file(), m_args, m_maxRedir - 1);
                            downloader.download();
                            throw ISAException(Gadgets::redirc);
                        }
                    }
                    else
                    {
                        throw ISAException("Error - Maximal redirection limit 10 reached");
                    }


                }
                break;
            }
        }
    }
    if (count < 0)
    {
        throw ISAException("Error - Recv bytes");
    }
    return 0;
}

/**
 * @brief HTTP::get_content
 * @return
 */
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
                throw ISAException("Error - Chunked decoding.");
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

/**
 * @brief HTTP::download
 * @return
 */
int HTTP::download()
{
    initiateConnection();
    send_request();
    get_headers();
    std::string s =  get_content();

    std::ofstream out("output.xml");
    out << s;
    out.close();

    return 0;
}

/**
 * @brief HTTP::httpResponseCheck
 * @param in
 * @return
 */
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
        throw ISAException("Error - Not an HTTP response.");
    }
    else if (status != 200)
    {
        if (status == 301)
        {
            return 301;
        }
        else
        {
            throw ISAException("Error - Not a 200 OK response.");
        }
    }
    return 0;
}

/**
 * @brief HTTP::receive
 * @param buf
 * @param size
 * @return
 */
int HTTP::receive(void *buf, int size)
{
    return recv(m_sock, buf, size, 0x100);
}

/**
 * @brief HTTP::makeHeaders
 * @param server
 * @param path
 * @return
 */
std::string HTTP::makeHeaders(std::string server, std::string path)
{
    std::ostringstream oss;
    oss << "GET "
        << path
        << " HTTP/1.1\r\nHost: "
        << server
        << "\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:40.0) Gecko/20100101 Firefox/40.0\r\n"
        << "Accept: */*\r\n\r\n";

    std::string headers = oss.str();
    return headers;
}

/**
 * @brief HTTP::read_bytes
 * @param buffer
 * @param size
 */
void HTTP::read_bytes(std::vector<char> &buffer, unsigned int size)
{
    if (buffer.size() < size) buffer.resize(size);
    if (receive(static_cast<char*>(buffer.data()), size) <= 0)
    {        
        throw ISAException("Error - reading bytes");
    }
}

/**
 * @brief HTTP::read_sequence
 * @param buffer
 * @param text
 * @return
 */
std::string HTTP::read_sequence(std::vector<char> &buffer, const std::string &text)
{
    std::string test = "";
    do
    {
        char B;
        if (receive(static_cast<char*>(&B), 1) <= 0)
        {
            throw ISAException("Error - reading sequence");
        }
        else
        {
            buffer.push_back(B);
        }
        test = std::string(buffer.begin(), buffer.end());
    } while (!Gadgets::contains_substring(test, text));
    return std::string(buffer.begin(), buffer.end());
}

/**
 * @brief HTTP::get_line
 * @return
 */
std::string HTTP::get_line()
{
    std::vector<char> buffer;
    std::string a = read_sequence(buffer, "\r\n");
    return a;
}

/**
 * @brief HTTP::HTTP
 * @param inServer
 * @param inPath
 * @param max_redir
 */
HTTP::HTTP(std::string inServer, std::string inPath, int max_redir)
{
    m_maxRedir = max_redir;
    m_url = inServer;
    m_path = inPath;
    m_port = 80;
    m_chunked = false;
    m_contentLength = -1;
    m_headers = "";
}

HTTP::~HTTP()
{
    close(m_sock);
}


// HTTPS begin -----------------------------------------------------------------------------

/**
 * @brief HTTPS::HTTPS
 * @param inServer
 * @param inPath
 * @param args
 * @param max_redir
 */
HTTPS::HTTPS(std::string inServer, std::string inPath, Arguments *args, int max_redir) : HTTP(inServer, inPath, max_redir)
{
    m_args = args;
    ctx = NULL;
    ssl = NULL;
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    m_port = 443;
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (args->getCertfileUsed())
    {
        if (!SSL_CTX_load_verify_locations(ctx, args->sCertFileName().c_str(), NULL))
        {
            throw ISAException("Error - Loading Certificate File");
        }
    }
    else if (args->getCertfileFolderUsed())
    {
        if (!SSL_CTX_load_verify_locations(ctx, NULL, args->sCertFilesFolder().c_str()))
        {
            throw ISAException("Error - Loading Certificates from a folder");
        }
    }
    else
    {

        if (!SSL_CTX_load_verify_locations(ctx, NULL, "/etc/ssl/certs/")) // default folder for certs
        {
            throw ISAException("Error - Loading cerificates from /etc/ssl/certs");
        }

    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_get_cert_store(ctx);
    certstore = X509_STORE_new();
    if (m_args->getCertfileUsed()) X509_STORE_load_locations(certstore, m_args->sCertFileName().c_str(), NULL);
    else if (m_args->getCertfileFolderUsed()) X509_STORE_load_locations(certstore, NULL, m_args->sCertFilesFolder().c_str());
    else X509_STORE_load_locations(certstore, NULL, "/etc/ssl/certs/");
    if (certstore == NULL) throw ISAException("Error - Certificate could not be loaded.");
    SSL_CTX_set_cert_store(ctx, certstore);
}

HTTPS::~HTTPS()
{
    if (ctx != NULL) SSL_CTX_free(ctx);
    if (ssl != NULL)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
}

/**
 * @brief HTTPS::initiateConnection
 * @return
 */
int HTTPS::initiateConnection()
{
    HTTP::initiateConnection();

    ssl = SSL_new(ctx);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_fd(ssl, m_sock);

    int sslresult = SSL_connect(ssl);
    if (sslresult != 1)
    {
        ssl_print_error();
        throw ISAException("Error - Start SSL connection error.");
    }

    ssl_print_error();

    return 0;
}

/**
 * @brief HTTPS::ssl_print_error
 */
void HTTPS::ssl_print_error()
{
    if (SSL_get_peer_certificate(ssl) != NULL)
    {
        if (SSL_get_verify_result(ssl) != X509_V_OK)
        {
            throw ISAException("Error - Invalid peer certificate.");
        }
    }
    else
    {
        throw ISAException("Error - Invalid peer certificate.");
    }
}

/**
 * @brief HTTPS::send_request
 * @return
 */
int HTTPS::send_request()
{
    std::string headers = makeHeaders(m_url, m_path);

    if (SSL_write(ssl, headers.c_str(), headers.length()) <= 0)
    {
        throw ISAException("Error - SSL sending failed.");
    }
    return 0;
}

/**
 * @brief HTTPS::receive
 * @param buf
 * @param size
 * @return
 */
int HTTPS::receive(void *buf, int size)
{
    int rec = 0;
    while (rec != size) rec +=  SSL_read(ssl, static_cast<char*>(buf) + rec, size - rec);
    return rec;
}
