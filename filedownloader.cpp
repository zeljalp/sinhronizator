#include <QMessageBox>
#include "filedownloader.h"
 

FileSyncMonster::FileSyncMonster(QUrl url, QObject *parent) :
 QObject(parent), _downloaded(false), _noError(false), httpsPrefix("https://"), loginApi("/api/login")
{

    _url = url.toString();
}

FileSyncMonster::~FileSyncMonster() { }

void FileSyncMonster::setUserAndPassword(QString user, QString password)
{
    _user = user;
    _password = password;
}

void FileSyncMonster::login()
{
    qDebug () << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    _networkManager = new QNetworkAccessManager(this);
    connect(_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processResponse(QNetworkReply*)));

    /*QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_1);*/
    //request.setSslConfiguration(config);

    QJsonObject json;
    json.insert("email","zeljko.acimovic@gmail.com");
    json.insert("password","12061986az");
    QJsonDocument jsonDoc(json);
    QByteArray jsonData= jsonDoc.toJson();
    QNetworkRequest request(QUrl("https://api.staging.hive5.app/api/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    _networkManager->post(request, jsonData);
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
