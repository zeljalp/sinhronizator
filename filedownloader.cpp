#include <QMessageBox>
#include "filedownloader.h"
#include <QThread>
#include <QEventLoop>
#include <QJsonArray>
#include <QDir>

FileSyncMonster::FileSyncMonster(QUrl url, QObject *parent) :
 QObject(parent), _downloaded(false), _noError(false),
  _httpsPrefix("https://"), _loginApi("/api/login"), _projectApi("/api/project"),
  _filesApi("/api/asset?filter[project][operator]=eq&filter[project][value]="),
  _downloadApi("/api/assetDownload")
{
    _url = url.toString();
    _networkManager = new QNetworkAccessManager(this);
    _requestState = LOGIN;

    _usrPics = qgetenv("USERPROFILE").append("/Pictures/");
    _hive5Dir = _usrPics + QString("/hive5");
}

FileSyncMonster::~FileSyncMonster() {}

void FileSyncMonster::setUserAndPassword(QString user, QString password)
{
    _user = user;
    _password = password;
}

void FileSyncMonster::login()
{
    // if this is false then check run environment
    // run environment should contain C:\Program Files\OpenSSL-Win64\bin
    qDebug () << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    connect(_networkManager, &QNetworkAccessManager::finished, this, &FileSyncMonster::processResponse);

    QJsonObject json;
    json.insert("email",_user);
    json.insert("password",_password);
    QJsonDocument jsonDoc(json);
    QByteArray jsonData= jsonDoc.toJson();
    QNetworkRequest request(QUrl("https://api.staging.hive5.app/api/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QNetworkReply *rep = _networkManager->post(request, jsonData);

    QEventLoop loop;
    connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

void FileSyncMonster::listProjects()
{
    QString locationString = _httpsPrefix + _loginRedirect + _projectApi;
    QUrl location(locationString);
    qDebug() << locationString;
    QNetworkRequest request(location);
    request.setRawHeader("Authorization", QString("Bearer " + _loginToken).toStdString().c_str());
    qDebug() << request.rawHeader("Authorization");

    QNetworkReply *rep = _networkManager->get(request);

    // in qt5 this is a method to ingore asynchrone logic in signal/slot
    QEventLoop loop;
    connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

void FileSyncMonster::listFilesForProject(QString projectName)
{
    const int id = _projectMap.value(projectName, -1);
    if (id < 0)
    {
        qDebug() << tr("Could not find id for project %1").arg(projectName);
        return;
    }

    QString locationString = _httpsPrefix + _loginRedirect + _filesApi + QString("%1").arg(id);
    qDebug() << locationString;

    QUrl location(locationString);
    QNetworkRequest request(location);
    request.setRawHeader("Authorization", QString("Bearer " + _loginToken).toStdString().c_str());
    qDebug() << request.rawHeader("Authorization");

    QNetworkReply *rep = _networkManager->get(request);

    // in qt5 this is a method to ingore asynchrone logic in signal/slot
    QEventLoop loop;
    connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

void FileSyncMonster::downloadFilesForProject(QString projectName)
{
    QList<int> ids = _projectFilesMap.values(projectName);
    if (ids.count() == 0)
    {
        qDebug() << tr("Could not find assets for project %1").arg(projectName);
        return;
    }

    QString locationString = _httpsPrefix + _loginRedirect + _downloadApi;
    qDebug() << locationString;

    QUrl location(locationString);
    QNetworkRequest request(location);
    request.setRawHeader("Authorization", QString("Bearer " + _loginToken).toStdString().c_str());
    request.setRawHeader("Content-Type", "application/json");

    foreach(int assetId, ids)
    {
        QJsonObject json;
        json.insert("assets", QString("%1").arg(assetId));
        QJsonDocument jsonDoc(json);
        QByteArray jsonData = jsonDoc.toJson();

        QNetworkReply *rep = _networkManager->put(request, jsonDoc.toJson());

        // in qt5 this is a method to ingore asynchrone logic in signal/slot
        QEventLoop loop;
        connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        QDir pDir(_hive5Dir + "/" + projectName);
        QFile *file = new QFile(pDir.path() + "/" + _filesMap.key(assetId));
        qDebug() << tr("Downloading file: ") << file->fileName();
        if(file->open(QFile::WriteOnly))
        {
            file->write(_lastPicture);
            file->flush();
            file->close();
            qDebug() << tr("File written");
        }
    }
}


bool FileSyncMonster::createHive5Home()
{
    QDir dir(_usrPics);
    QDir h5(_hive5Dir);
    if (h5.exists())
    {
        return true;
    } else {
        if (dir.exists())
        {
            return dir.mkdir("hive5");
        } else {
            return false;
        }
    }
}

bool FileSyncMonster::createHive5Project(QString project)
{
    QDir h5(_hive5Dir);
    QDir pDir(_hive5Dir + "/" + project);
    qDebug() << tr("Creating dir %1 in path %2").arg(project).arg(h5.path());

    if (pDir.exists())
    {
        qDebug() << tr("Dir %1 already exists").arg(pDir.path());
        return true;
    } else {
        return h5.mkdir(_hive5Dir + "/" + project);
    }
}

void FileSyncMonster::startSyncMachine(RequestType startRequest)
{
    bool ok = createHive5Home();
    if (!ok)
    {
        qDebug() << tr("Failed to create hive5 homedir...");
        return;
    }

    _requestState = startRequest;
    while (true)
    {
        switch (_requestState)
        {
        case LOGIN:
            login();
            break;
        case LIST_PROJECTS:
            listProjects();
            break;
        case LIST_FILES:
            foreach(QString pName, _projectMap.keys())
            {
                listFilesForProject(pName);
            }
            _requestState = DOWNLOAD_PICTURE;
            break;
        case DOWNLOAD_PICTURE:
            foreach(QString pName, _projectMap.keys())
            {
                downloadFilesForProject(pName);
            }
            _requestState = IDLE;
            break;
        case IDLE:
            return;
            break;
        default:
            break;
        }
    }

}
 

QString FileSyncMonster::downloadedUrl() const
{
    return _url;
}

void FileSyncMonster::processResponse(QNetworkReply* reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->request().header(QNetworkRequest::ContentTypeHeader);
        qDebug() << reply->request().rawHeader("Authorization");
        qDebug() << reply->request().url();
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QByteArray serverResponse = reply->readAll();
        qDebug() << serverResponse;
        QJsonDocument doc = QJsonDocument::fromJson(serverResponse);

        switch (_requestState)
        {
        case LOGIN:
        {
            QJsonObject obj = doc.object();
            _loginRedirect = obj.value("redirect").toString();
            _loginToken = obj.value("token").toString();
            _requestState = LIST_PROJECTS;
        }
            break;
        case LIST_PROJECTS:
        {
            QJsonObject data = doc.object();
            QJsonArray arr = data["data"].toArray();
            QJsonObject ar1 = arr.at(0).toObject();
            int idx = 0;
            for(const QJsonValue& val: arr) {
                QJsonObject loopObj = val.toObject();
                QString idS = loopObj["id"].toString();
                QJsonObject attr = loopObj["attributes"].toObject();
                QString projectName = attr["name"].toString();

                qDebug() << "[" << idx << "] id    : " << idS;
                qDebug() << "[" << idx << "] name: " << projectName;

                bool ok = createHive5Project(projectName);
                if (!ok)
                {
                    qDebug() << tr("Failed to create dir ").arg(attr["name"].toString());
                } else {
                    _projectMap.insert(projectName, idS.toInt());
                }
                ++idx;
            }
            _requestState = LIST_FILES;
        }
            break;
        case LIST_FILES:
        {
            QJsonObject data = doc.object();
            QJsonArray arr = data["data"].toArray();
            int idx = 0;
            qDebug() << tr("Files ..............................................");
            for(const QJsonValue& val: arr) {
                QJsonObject loopObj = val.toObject();
                QString idS = loopObj["id"].toString();
                QJsonObject attr = loopObj["attributes"].toObject();
                QString fileName = attr["name"].toString();
                QString projectName = attr["projectName"].toString();
                qDebug() << "[" << idx << "] id    : " << idS;
                qDebug() << "[" << idx << "] name: " << fileName;
                qDebug() << "[" << idx << "] project: " << projectName;

                _filesMap.insert(fileName, idS.toInt());
                _projectFilesMap.insertMulti(projectName, idS.toInt());

                ++idx;
            }
        }
            break;
        case DOWNLOAD_PICTURE:
            _lastPicture = serverResponse;
            break;
        default:
            break;
        }

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
