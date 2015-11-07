#ifndef CMDARGUMENTS_HPP
#define CMDARGUMENTS_HPP

#include <unistd.h>
#include <cstdlib>
#include <iostream>

class Arguments
{
public:
    Arguments() : aktualizace(false), autor(false), url(false), latest(false), feedfileUsed(false), certfileUsed(false), certfileFolderUsed(false), urlUsed(false) {}
    bool getAktualizace() const;
    void setAktualizace(bool value);

    bool getAutor() const;
    void setAutor(bool value);

    bool getUrl() const;
    void setUrl(bool value);

    bool getLatest() const;
    void setLatest(bool value);

    bool getFeedfileUsed() const;
    void setFeedfileUsed(bool value);

    void parseArgs(int argc, char **argv);
    void printHelp();

    bool getCertfileUsed() const;
    void setCertfileUsed(bool value);

    bool getCertfileFolderUsed() const;
    void setCertfileFolderUsed(bool value);

    bool getUrlUsed() const;
    void setUrlUsed(bool value);

    std::string sUrl() const;
    void setSUrl(const std::string &sUrl);

    std::string sCertFileName() const;
    void setSCertFileName(const std::string &sCertFileName);

    std::string sCertFilesFolder() const;
    void setSCertFilesFolder(const std::string &sCertFilesFolder);

    std::string sFeedFile() const;
    void setSFeedFile(const std::string &sFeedFile);

private:
    std::string m_sUrl;
    std::string m_sCertFileName;
    std::string m_sCertFilesFolder;
    std::string m_sFeedFile;

    int opt;
    bool aktualizace;
    bool autor;
    bool url;
    bool latest;
    bool feedfileUsed;
    bool certfileUsed;
    bool certfileFolderUsed;
    bool urlUsed;

};


#endif // CMDARGUMENTS_HPP



