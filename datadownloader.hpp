#ifndef DATADOWNLOADER_HPP
#define DATADOWNLOADER_HPP

#include "shared.h"
#include "gadgets.h"

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

#endif // DATADOWNLOADER_HPP


typedef enum
{
    ERR_SOCKET,
    ERR_IP,
    ERR_CONN,
    ERR_QUERY,
    ERR_RECV,
    NOT_HTTP,
    ERR_CHUNKED,
    ERR_SSL_PEM,
    ERR_SSL_FOLDER,
    SSL_CONN_FAILED,
    SSL_CERTCHECK_FAIL,
    SSL_FAILED_SEND,
    ERR_DIFFERENT_CODE,
    SSL_CONN_ERROR,
    BAD_CERT,
    NO_URLS,
    NON_EXISTENT_FEEDFILE
} errors;

class IDownload
{
    virtual int receive(void *buf, int size)=0;
    virtual int send_request()=0;
    virtual int initiateConnection()=0;
    virtual int get_headers()=0;
    virtual std::string get_content()=0;
    virtual int httpResponseCheck(std::string& in)=0;
    virtual std::string makeHeaders(std::string server, std::string path)=0;
    virtual int download()=0;
    virtual void read_bytes(std::vector<char> &buffer, unsigned int size)=0;
    virtual std::string read_sequence(std::vector<char> &buffer, const std::string &text)=0;
    virtual std::string get_line()=0;

};

class HTTP : virtual IDownload
{
protected:
    int m_maxRedir;
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

    //copied from my IPK proj.1
    struct in_addr **addresses;
    struct sockaddr_in m_server;
    struct in_addr m_addr;
    Arguments* m_args;
public:
    HTTP(std::string inServer, std::string inPath, int max_redir = 10);
    virtual ~HTTP();
    virtual int receive(void *buf, int size);
    virtual int send_request();
    virtual int initiateConnection();
    virtual int get_headers();
    virtual std::string get_content();
    virtual int httpResponseCheck(std::string& in);
    virtual std::string makeHeaders(std::string server, std::string path);
    virtual int download();
    virtual void read_bytes(std::vector<char> &buffer, unsigned int size);
    virtual std::string read_sequence(std::vector<char> &buffer, const std::string &text);
    virtual std::string get_line();
};

class HTTPS : virtual IDownload, public HTTP
{
private:
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *bio;
    std::string bioAddrStr;
public:
    HTTPS(std::string inServer, std::string inPath, Arguments *args, int max_redir = 10);
    virtual ~HTTPS();
    virtual int receive(void *buf, int size);
    virtual int send_request();
    virtual int initiateConnection();
};
