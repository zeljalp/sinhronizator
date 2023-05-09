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

    enum RequestType {LOGIN, LIST_PROJECTS, LIST_FILES, DOWNLOAD_PICTURE, IDLE};
    void setUserAndPassword(QString user, QString password);
    void download(QUrl fileUrl);
    bool isDownloaded() const;
    bool noError() const;
    QByteArray downloadedData() const;
    QString downloadedUrl() const;
    void login();
    void listProjects();
    void listFilesForProject(QString projectName);
    void downloadFilesForProject(QString projectName);
    bool createHive5Home();
    bool createHive5Project(QString project);
    void startSyncMachine(RequestType startRequest = LOGIN);

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
    QString _httpsPrefix;
    QString _loginApi;
    QString _projectApi;
    QString _filesApi;
    QString _downloadApi;
    RequestType _requestState;
    QString _loginToken;
    QString _loginRedirect;
    QString _usrPics;
    QString _hive5Dir;
    QByteArray _lastPicture;
    QMap<QString, int> _projectMap;
    QMap<QString, int> _filesMap;
    QMultiMap<QString, int> _projectFilesMap;
};
 
#endif // FILEDOWNLOADER_H
