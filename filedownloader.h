#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
 
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

class FileSyncMonster : public QObject
{
    Q_OBJECT
public:
    explicit FileSyncMonster(QUrl url, QObject *parent = 0);
    virtual ~FileSyncMonster();

    void setUserAndPassword(QString user, QString password);
    void download(QUrl fileUrl);
    bool isDownloaded() const;
    bool noError() const;
    QByteArray downloadedData() const;
    QString downloadedUrl() const;
    void login();

signals:
    void downloaded(FileSyncMonster* downloader);
    void downloadError(FileSyncMonster* downloader);

public slots:
    void processResponse(QNetworkReply* pReply);

private:
    QNetworkAccessManager *_networkManager;
    QByteArray _downloadedData;
    bool _downloaded;
    bool _noError;
    QString _url;
    QString _password;
    QString _user;
    QString httpsPrefix;
    QString loginApi;

};
 
#endif // FILEDOWNLOADER_H
