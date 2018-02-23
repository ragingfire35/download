#ifndef HTTP_DOWNLOAD_H  
#define HTTP_DOWNLOAD_H  
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Http : public QObject
{
	Q_OBJECT
public:
	Http();
	~Http();
public slots:
	void metaDataChanged();
	void httpDownload(QObject* parent,QString& strurl, QString& dir, QString& rdir);
	void replyFinished(QNetworkReply*reply);
	void onDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
	void onReadyRead();
private:
	bool hasInit=false;
	QString mdir;
	QObject * mparent;
	QFile *file;
	QFileInfo fileInfo;
	QNetworkAccessManager *accessManager;
	QNetworkRequest request;
	QNetworkReply *reply;
};



#endif