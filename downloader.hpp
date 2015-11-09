#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include "shared.h"
#include "gadgets.h"
#include "cmdArguments.hpp"
#include "xmlviewer.h"

//cd /usr/include/
//ln -s libxml2/libxml libxml



typedef enum
{
    ERR_SOCKET,
    ERR_IP,
    ERR_BIO_CREATE,
    ERR_BIO_CONN,
    ERR_CONN,
    ERR_QUERY,
    ERR_RECV,
    NOT_HTTP,
    ERR_CHUNKED,
    ERR_SSL_PEM,
    ERR_SSL_FOLDER,
    SSL_CONN_FAILED,
    SSL_CERTCHECK_FAIL,
    SSL_FAILED_SEND
} errors;

class Downloader {
private:
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *bio;
    std::string m_url;
    std::string m_url_server;
    std::string m_url_path;
    int m_contentLength;
    int m_chunked;
    std::string m_headers;
    std::string m_content;
    struct sockaddr_in *m_location;
    int m_sock;
    int m_port;
    char *m_host;
    std::string m_ip;
    std::string m_path;
    struct hostent *m_info;
    int m_connection;
    std::string bioAddrStr;
    //copied from IPK
    struct in_addr **addresses;
    struct sockaddr_in m_server;
    struct in_addr m_addr;
    Arguments* m_args;
public:
    Downloader(std::string url, Arguments *args) : m_args(args)
    {
        SSL_library_init();
        SSL_load_error_strings();
        ERR_load_BIO_strings();
        OpenSSL_add_all_algorithms();
        urlParser up(url);
        m_url = up.host();
        m_path = up.path();
        m_port = 80;


        if (up.protocol() == "https")
        {
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
        }

        std::ostringstream oss;
        oss << m_url << ":" << m_port;
        bioAddrStr = oss.str();

        m_chunked = false;
        m_contentLength = -1;
        m_headers = "";

    }

    int send_request();
    int my_connect();
    int initiateConnection();
    int get_headers();
    std::string get_content();
    int splitUrl();
    int read_data(void *str, const std::string& end);
    int MakeUnsecuredConn();
    int MakeSecureConn();
    int httpResponseCheck(std::string& in);
    std::string makeHeaders(std::string server, std::string path);
    bool contains_substring(std::string& input, std::string seq);
    int find_chunked(std::string& content);
    void printError(std::string msg);
    void download();

    void read_bytes(std::vector<char> &buffer, unsigned int size)
    {
        if (buffer.size() < size) buffer.resize(size);
        if (BIO_read(bio, static_cast<char*>(buffer.data()), size) <= 0)
        {
            std::cerr << "read bytes err" << std::endl;
            throw ERR_RECV;
        }
    }

    std::string read_sequence(std::vector<char> &buffer, const std::string &text)
    {
        std::string test = "";
        do
        {
            char B;
            if (BIO_read(bio, static_cast<char*>(&B), 1) <= 0)
            {
                std::cerr  << "read seq err" << std::endl;
                throw ERR_RECV;
            }
            else
            {
                buffer.push_back(B);
            }
            test = std::string(buffer.begin(), buffer.end());
        } while (!contains_substring(test, text));
        return std::string(buffer.begin(), buffer.end());
    }

    std::string get_line()
    {
        std::vector<char> buffer;
        std::string a = read_sequence(buffer, "\r\n");
        return a;
    }

    int http_read(int fd, void *buf, int size, int flags, BIO *b)
    {
        return recv(fd, buf, size, flags);
    }

    int ssl_read(int fd, void *buf, int size, int flags, BIO *b)
    {

        return BIO_read(b, buf, size);
    }
};

#endif // DOWNLOADER_HPP
