#include <QMessageBox>

#include "filedownloader.h"
 
FileDownloader::FileDownloader(QObject *parent) :
 QObject(parent), m_downloaded(false), m_noError(false)
{
    connect(&m_networkManager, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
}

FileDownloader::FileDownloader(QUrl fileUrl, QObject *parent) :
 QObject(parent), m_downloaded(false), m_noError(false)
{
    m_url = fileUrl.toString();

    connect(&m_networkManager, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
 
    QNetworkRequest request(fileUrl);
    request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
    m_networkManager.get(request);
}
 
FileDownloader::~FileDownloader() { }
 
void FileDownloader::download(QUrl fileUrl)
{
    if(m_url.isEmpty())
    {
        m_url = fileUrl.toString();

        QNetworkRequest request(fileUrl);
        request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
        m_networkManager.get(request);
    }
}

QString FileDownloader::downloadedUrl() const
{
    return m_url;
}

void FileDownloader::fileDownloaded(QNetworkReply* reply) {

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 302)
    {
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        QNetworkRequest newRequest(newUrl);
        m_networkManager.get(newRequest);
        return;
    }

    m_downloadedData = reply->readAll();
    reply->deleteLater();
    if(reply->error() == QNetworkReply::NoError)
    {
         m_noError = true;
        //emit a signal
        emit downloaded(this);
    }
    else
    {
        //QMessageBox::information(0, "Error", reply->errorString(), QMessageBox::Ok);
        m_noError = false;
        emit downloadError(this);
    }

    m_downloaded = true;
}
 
bool FileDownloader::isDownloaded() const
{
    return m_downloaded;
}

bool FileDownloader::noError() const
{
    return m_noError;
}

QByteArray FileDownloader::downloadedData() const
{
    return m_downloadedData;
}
