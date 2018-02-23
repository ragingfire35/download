#include "http_get.h"  
#include <shlwapi.h>
#include<windows.h>
#include <tchar.h>
#include "global.h"

Http::Http()
{
}

Http::~Http()
{
	
}

BOOL CreateDirTree(LPCTSTR lpPath)
{

	if (NULL == lpPath || _tcslen(lpPath) == 0)
	{
		return FALSE;
	}

	if (::PathFileExists(lpPath) || ::PathIsRoot(lpPath))
		return TRUE;

	TCHAR szParentpath[MAX_PATH] = _T("");
	::lstrcpy(szParentpath, lpPath);

	::PathRemoveBackslash(szParentpath);//ȥ��·�����ķ�б��  
	::PathRemoveFileSpec(szParentpath);//��·��ĩβ���ļ������ļ��кͷ�б��ȥ��  

	if (0 == _tcscmp(lpPath, szParentpath))
		return FALSE;

	//assert(0 != _tcscmp(lpPath, szParentpath));
	if (CreateDirTree(szParentpath))//�ݹ鴴��ֱ����һ����ڻ��Ǹ�Ŀ¼  
	{
		return ::CreateDirectory(lpPath, NULL);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void Http::httpDownload(QObject* parent,QString& strurl, QString& dir, QString& rdir) {

	mparent = parent;
	mdir = rdir;
	//dir = "G:\\1.mp3";
	QString tmp = dir.replace("/", "\\");;

	QString onlyeDir = tmp.left( tmp.lastIndexOf("\\")+1 );

	QString fileName = tmp.right(tmp.length() - tmp.lastIndexOf("\\") - 1);

	bool res = CreateDirTree(onlyeDir.toStdWString().c_str());

	//str = "http://biaoqian.cn/t.php";
	
	//leInfo = url.path();
	//dir = "g:/1.txt";
	file = new QFile(dir);
	QString hash;	
	if (file->exists()) {
		QString hash = GetMd5(dir);
		hash = hash.toLower();
		strurl = QString("%1?hash=%2&rdir=%3").arg(strurl).arg(hash).arg(rdir);
		//res = file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::ReadOnly);//ֻд��ʽ���ļ�  		
	}
	else {
		res = file->open(QIODevice::WriteOnly | QIODevice::ReadOnly);//ֻд��ʽ���ļ�  
	}
	QUrl url(strurl);

	accessManager = new QNetworkAccessManager(this);
	request.setUrl(url);
	/******************����http��header***********************/
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");  
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");  
	// request.setRawHeader("Content-Disposition","form-data;name='doc';filename='a.txt'");  
	//request.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());
	  
	connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finishΪmanager�Դ����źţ�replyFinished���Զ����  
	
	reply = accessManager->get(request);//ͨ���������ݣ�����ֵ������replyָ����.

	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));//download�ļ�ʱ����
	connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));	
	connect((QObject *)reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
	
}
/***************��Ӧ����**************************/
void Http::replyFinished(QNetworkReply *reply) {
	//��ȡ��Ӧ����Ϣ��״̬��Ϊ200��ʾ����  
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	//QByteArray bytes = reply->readAll();  //��ȡ�ֽ�
	//QString result(bytes);  //ת��Ϊ�ַ���
	//qDebug() << result;

	//�޴��󷵻�  
	if (reply->error() == QNetworkReply::NoError)
	{		
		//return ok
		file->close();
		QString retVal;
		QMetaObject::invokeMethod(mparent, "HttpSuccessCallBack", Qt::DirectConnection,
			Q_RETURN_ARG(QString, retVal),
			Q_ARG(QString, mdir));
		//QByteArray bytes = reply->readAll();  //��ȡ�ֽ�
		//QString result(bytes);  //ת��Ϊ�ַ���
		//qDebug() << result;
	}
	else
	{
		//�������
		if (400 == status_code.toInt()) {

		}
	}

	reply->deleteLater();//Ҫɾ��reply�����ǲ�����repyfinished��ֱ��delete��Ҫ����deletelater;
	this->deleteLater();
}

void Http::metaDataChanged() {
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int res = status_code.toInt();
	if ( 200 == res && false == hasInit) {
		int k = 0;
		k++;
	}
}

/***********���½�����*************/
void Http::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal) {
	int k = 0;
	k++;
}

void Http::onReadyRead() {

	QByteArray ar = reply->readAll();
	qint64 size = ar.size(); 
	int tmp = ar.length();

	QString retVal;
	QMetaObject::invokeMethod(mparent, "DownloadSize", Qt::DirectConnection,
		Q_RETURN_ARG(QString, retVal),
		Q_ARG(qint64, size));

	file->write(ar);	
	file->flush();
}
