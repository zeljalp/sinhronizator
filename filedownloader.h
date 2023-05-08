#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
 
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
 
class FileSyncMonster : public QObject
{
    Q_OBJECT
public:
    explicit FileSyncMonster(QUrl url, QString user, QString password, QObject *parent = 0);
    virtual ~FileSyncMonster();

    void download(QUrl fileUrl);
    bool isDownloaded() const;
    bool noError() const;
    QByteArray downloadedData() const;
    QString downloadedUrl() const;
    void login();

signals:
    void downloaded(FileSyncMonster* downloader);
    void downloadError(FileSyncMonster* downloader);

private slots:
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
