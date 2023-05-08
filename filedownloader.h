#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
 
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
 
class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  explicit FileDownloader(QObject *parent = 0);
  explicit FileDownloader(QUrl fileUrl, QObject *parent = 0);
  virtual ~FileDownloader();

  void download(QUrl fileUrl);
  bool isDownloaded() const;
  bool noError() const;
  QByteArray downloadedData() const;
 QString downloadedUrl() const;

 signals:
  void downloaded(FileDownloader* downloader);
  void downloadError(FileDownloader* downloader);

 private slots:
  void fileDownloaded(QNetworkReply* pReply);
 
 private:
  QNetworkAccessManager m_networkManager;
  QByteArray m_downloadedData;
  bool m_downloaded;
  bool m_noError;
  QString m_url;
};
 
#endif // FILEDOWNLOADER_H
