#include <QMessageBox>
#include "filedownloader.h"
 

FileSyncMonster::FileSyncMonster(QUrl url, QString user, QString password, QObject *parent) :
 QObject(parent), _downloaded(false), _noError(false), httpsPrefix("https://"), loginApi("/api/login")
{

    _url = url.toString();
    _user = user;
    _password = password;
    _networkManager = new QNetworkAccessManager(this);


}

FileSyncMonster::~FileSyncMonster() { }

void FileSyncMonster::login()
{
    QString tokenRequest = QString("{\"email\":\"%1\",\"password\":\"%2\"}").arg(_user).arg(_password);
    QByteArray data = tokenRequest.toUtf8().toBase64();
    qDebug() << data;
    QNetworkRequest request(httpsPrefix + _url + loginApi);
    qDebug() << "Url:"  << request.url().toString();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qDebug() << "Header:" << request.header(QNetworkRequest::ContentTypeHeader).toString();

    connect(_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processResponse(QNetworkReply*)));
    _networkManager->post(request, data);
}
 

QString FileSyncMonster::downloadedUrl() const
{
    return _url;
}

void FileSyncMonster::processResponse(QNetworkReply* reply)
{
    qDebug() << "Processing response";
    if(reply->error() == QNetworkReply::NoError)
    {
        _downloadedData = reply->readAll();
        qDebug() << _downloadedData;

        emit downloaded(this);
    } else{
        QString err = reply->errorString();
        qDebug() << err;
    }

    reply->deleteLater();
}
 
bool FileSyncMonster::isDownloaded() const
{
    return _downloaded;
}

bool FileSyncMonster::noError() const
{
    return _noError;
}

QByteArray FileSyncMonster::downloadedData() const
{
    return _downloadedData;
}
