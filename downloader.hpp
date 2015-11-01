#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sstream>
#include <libxml2/libxml/nanohttp.h>
//cd /usr/include/
//ln -s libxml2/libxml libxml

#include <regex>


typedef enum
{
    ERR_SOCKET,
    ERR_IP,
    ERR_CONN,
    ERR_QUERY,
    ERR_RECV,
    NOT_HTTP
} errors;

class Downloader {
private:
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

    //copied from IPK
    struct in_addr **addresses;
    struct sockaddr_in m_server;
    struct in_addr m_addr;
public:
    Downloader(std::string url) : m_url(url)
    {
        m_chunked = false;
        m_contentLength = -1;
        m_headers = "";
        SSL_load_error_strings();
        ERR_load_BIO_strings();
        OpenSSL_add_all_algorithms();
        std::cout << "OpenSLL initialized" << std::endl;
    }

    int send_request();
    int my_connect();
    int get_line();
    int initiateConnection(int port);
    int get_headers();
    int get_content();
    int splitUrl();
    int read_data(void *str, const std::string& end);
    int MakeUnsecuredConn();
    int httpResponseCheck(std::string& in);
    int initSocketAndAssignIP();
    std::string httpRequest(std::string server, std::string path);
    bool read_bytes(std::string& input, std::string seq);
    int find_chunked(std::string& content);
    void printError(std::string msg);
};

#endif // DOWNLOADER_HPP
