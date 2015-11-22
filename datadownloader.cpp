#include "datadownloader.hpp"

/**
 * @brief HTTP::initiateConnection
 * @return int with state of error
 */
int HTTP::initiateConnection()
{
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // initiate socket
    if (m_sock < 0)
    {
        throw ISAException("Error - Socket init. failed.");
    }

    m_info = gethostbyname(m_url.c_str()); // convert url
    if (m_info == nullptr)
    {
        throw ISAException("Error - IP translation failed.");
    }

    addresses = (struct in_addr **) m_info->h_addr_list; // get ip
    for (int i = 0; addresses[i] != NULL; ++i) m_ip.assign(inet_ntoa(*addresses[i]));
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(m_port);
    m_addr.s_addr = inet_addr(m_ip.c_str());
    m_server.sin_addr = m_addr;

    auto a = connect(m_sock, (struct sockaddr*) &m_server, sizeof(sockaddr_in)); // connection to the server
    if (a == -1)
    {
        close(m_sock);
        throw ISAException("Error - Socket - Failed connecting to server");
    }
    return 0;
}

/**
 * @brief HTTP::send_request
 * @return int
 */
int HTTP::send_request()
{
    std::string query = makeHeaders(m_url, m_path); // prepare the headers

    auto tmp = send(m_sock, query.c_str(), query.length(), 0); // send request
    if (tmp == -1)
    {
        close(m_sock);
        throw ISAException("Error - Sending request");
    }
    return 0;
}

/**
 * @brief HTTP::get_headers
 * @return int
 */
int HTTP::get_headers()
{

    char buffer[1];
    int count;
    std::string endHeaderSequence = "\r\n\r\n"; // end headers string
    std::string endLine = "\r\n";
    std::string firstLine = "";
    bool doOnce = true; // for first line check
    int resCheck = 0;
    while (errno == EINTR || (errno = 0, (count = receive(buffer, 1)) > 0))

    {
        if (count > 0)
        {
            m_headers.append(buffer, count); // append to headers
            if (doOnce)
            {
                firstLine.append(buffer, count);
                if (Gadgets::contains_substring(firstLine, endLine))
                {
                    resCheck = httpResponseCheck(firstLine); // check if we have 200 or 301, or error
                    doOnce = false;
                }
            }

            if (Gadgets::contains_substring(m_headers, endHeaderSequence))
            {
                int res = Gadgets::find_chunked(m_headers); // determine if chunked
                if (res == -1) m_chunked = true;
                else m_contentLength = res; // or to use content-length

                if (resCheck == 301) // if we found 301 state
                {
                    std::string redirectUrl = Gadgets::find_location_on_redirect(m_headers); // we find location which to redirect to
                    UrlDetail urldet(redirectUrl);

                    if (m_maxRedir > 0) // check if we still can redirect
                    {
                        if (urldet.port() == 80)
                        {
                            HTTP downloader(urldet.server(), urldet.file(), m_maxRedir - 1); // http
                            downloader.download();
                            throw ISAException(Gadgets::redirc);
                        }
                        else
                        {
                            HTTPS downloader(urldet.server(), urldet.file(), m_args, m_maxRedir - 1); // ssl
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
 * @return string with content
 */
std::string HTTP::get_content()
{
    std::string line;
    if (m_chunked)
    {
        short endLnSize = 2; // overlapping characters
        std::vector<char> data;
        unsigned long chunkSize = 1;
        while (chunkSize - endLnSize)
        {
            line = get_line(); // get one line
            try
            {
                chunkSize = std::stol(line, nullptr, 16) + endLnSize; // convert chunk size from string to long int
            }
            catch (std::invalid_argument& e)
            {
                throw ISAException("Error - Chunked decoding.");
            }

            if (chunkSize - endLnSize)
            {
                std::vector<char> tmp;
                read_bytes(tmp, chunkSize); // read bytes
                data.insert(data.end(), tmp.begin(), tmp.end() - endLnSize); // we append the retrieved byte
            }
        }
        return std::string(data.begin(), data.end()); // after we are done we convert vector of bytes to a string
    }
    else
    {
        std::string out = "";
        std::vector<char> tmp;
        read_bytes(tmp, m_contentLength); // else the same thing, but without finding chunk size
        out.append(tmp.begin(), tmp.end()); // append downloaded bytes
        return out; // return string
    }
    return "";
}

/**
 * @brief HTTP::download
 * @return int
 */
int HTTP::download()
{
    initiateConnection(); // connection init
    send_request(); // send request with headers
    get_headers(); // get headers, find if chunked, redirection etc.
    std::string s =  get_content(); // save the content

    std::ofstream out("output.xml");
    out << s; // save to file
    out.close();

    return 0;
}

/**
 * @brief HTTP::httpResponseCheck
 * @param in
 * @return int, 0 not specified, 200 if ok and 301 if redirection
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
 * @return int of received bytes
 */
int HTTP::receive(void *buf, int size)
{
    return recv(m_sock, buf, size, 0x100); // read and wait for all
}

/**
 * @brief HTTP::makeHeaders
 * @param server
 * @param path
 * @return string with created headers
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
    if (buffer.size() < size) buffer.resize(size); // resize if needed
    if (receive(static_cast<char*>(buffer.data()), size) <= 0) // receive the bytes
    {        
        throw ISAException("Error - reading bytes");
    }
}

/**
 * @brief HTTP::read_sequence
 * @param buffer
 * @param text
 * @return string with read sequence
 */
std::string HTTP::read_sequence(std::vector<char> &buffer, const std::string &text)
{
    std::string test = "";
    do
    {
        char B;
        if (receive(static_cast<char*>(&B), 1) <= 0) // receive one char byte
        {
            throw ISAException("Error - reading sequence");
        }
        else
        {
            buffer.push_back(B); // push back into vector of bytes
        }
        test = std::string(buffer.begin(), buffer.end()); // if we found what we needed
    } while (!Gadgets::contains_substring(test, text)); // end
    return std::string(buffer.begin(), buffer.end());
}

/**
 * @brief HTTP::get_line
 * @return string with retrieved line
 */
std::string HTTP::get_line()
{
    std::vector<char> buffer;
    std::string a = read_sequence(buffer, "\r\n"); // reads until endline sequence
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
    OpenSSL_add_all_algorithms(); // requited initialization functions

    m_port = 443; // port for SSL
    ctx = SSL_CTX_new(SSLv23_client_method()); // for better compatibility
    if (args->getCertfileUsed()) // if we are using certfile
    {
        if (!SSL_CTX_load_verify_locations(ctx, args->sCertFileName().c_str(), NULL)) // test the file
        {
            throw ISAException("Error - Loading Certificate File");
        }
    }
    else if (args->getCertfileFolderUsed()) // if we are using a folder
    {
        if (!SSL_CTX_load_verify_locations(ctx, NULL, args->sCertFilesFolder().c_str())) // test the folder
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
    SSL_CTX_get_cert_store(ctx); // set up a store
    certstore = X509_STORE_new(); // create a new one
    if (m_args->getCertfileUsed()) X509_STORE_load_locations(certstore, m_args->sCertFileName().c_str(), NULL); // if we use file, test again
    else if (m_args->getCertfileFolderUsed()) X509_STORE_load_locations(certstore, NULL, m_args->sCertFilesFolder().c_str()); // test folder
    else X509_STORE_load_locations(certstore, NULL, "/etc/ssl/certs/"); // else load default location
    if (certstore == NULL) throw ISAException("Error - Certificate could not be loaded."); // error, throw ISAException
    SSL_CTX_set_cert_store(ctx, certstore); // set up the store with ctx
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
 * @return int
 */
int HTTPS::initiateConnection()
{
    HTTP::initiateConnection();

    ssl = SSL_new(ctx);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_fd(ssl, m_sock);

    int sslresult = SSL_connect(ssl); // new ssl connection
    if (sslresult != 1)
    {
        ssl_print_error(); // print the error, try certs
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
        if (SSL_get_verify_result(ssl) != X509_V_OK) // if the cert was not ok
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
    std::string headers = makeHeaders(m_url, m_path); // make header string

    if (SSL_write(ssl, headers.c_str(), headers.length()) <= 0) // write the data
    {
        throw ISAException("Error - SSL sending failed.");
    }
    return 0;
}

/**
 * @brief HTTPS::receive
 * @param buf
 * @param size
 * @return int with received bytes
 */
int HTTPS::receive(void *buf, int size)
{
    int rec = 0;
    while (rec != size) rec +=  SSL_read(ssl, static_cast<char*>(buf) + rec, size - rec); // read until there is something to read
    return rec;
}
