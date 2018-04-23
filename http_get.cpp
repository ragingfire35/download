#include "http_get.h"  
#include <shlwapi.h>
#include<windows.h>
#include <tchar.h>
#include "global.h"
#include <QMessageBox>

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

	::PathRemoveBackslash(szParentpath);//去除路径最后的反斜杠  
	::PathRemoveFileSpec(szParentpath);//将路径末尾的文件名或文件夹和反斜杠去掉  

	if (0 == _tcscmp(lpPath, szParentpath))
		return FALSE;

	//assert(0 != _tcscmp(lpPath, szParentpath));
	if (CreateDirTree(szParentpath))//递归创建直到上一层存在或是根目录  
	{
		return ::CreateDirectory(lpPath, NULL);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void Http::httpDownload(QObject* parent,QString& strurl, QString& dir, QString& rdir, QString& uType) {

	mparent = parent;
	mrdir = rdir;
	//dir = "G:\\1.mp3";
	QString tmp = dir.replace("/", "\\");;

	QString onlyeDir = tmp.left( tmp.lastIndexOf("\\")+1 );

	QString fileName = tmp.right(tmp.length() - tmp.lastIndexOf("\\") - 1);

	bool res = CreateDirTree(onlyeDir.toStdWString().c_str());

	//str = "http://biaoqian.cn/t.php";
	
	//leInfo = url.path();
	//dir = "g:/1.txt";
	savedir = dir;
	file = new QFile(dir);
	QString hash;
	if (file->exists()) {
		QString hash = GetMd5(dir);
		hash = hash.toLower();
		strurl = QString("%1?hash=%2&rdir=%3&uType=%4&XDEBUG_SESSION_START=PHPSTORM").arg(strurl).arg(hash).arg(rdir).arg(uType);
		//res = file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::ReadOnly);//只写方式打开文件  		
	}
	else {
		strurl = QString("%1?hash=%2&rdir=%3&uType=%4&XDEBUG_SESSION_START=PHPSTORM").arg(strurl).arg("NEED_DOWN_IMEDIAE").arg(rdir).arg(uType);
		
	}
	finalUrl = strurl;
	QUrl url(strurl);

	accessManager = new QNetworkAccessManager(this);
	request.setUrl(url);
	/******************设置http的header***********************/
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");  
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");  
	// request.setRawHeader("Content-Disposition","form-data;name='doc';filename='a.txt'");  
	//request.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());
	request.setHeader(QNetworkRequest::CookieHeader, "XDEBUG_SESSION=PHPSTORM");
	  
	//connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finish为manager自带的信号，replyFinished是自定义的  
	
	reply = accessManager->get(request);//通过发送数据，返回值保存在reply指针里.

	//connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));//download文件时进度
	//connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));	
	connect((QObject *)reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
	
}
/***************响应结束**************************/
void Http::replyFinished(QNetworkReply *reply) {
	//获取响应的信息，状态码为200表示正常  
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	//QByteArray bytes = reply->readAll();  //获取字节
	//QString result(bytes);  //转化为字符串
	//qDebug() << result;

	//无错误返回  
	if (reply->error() == QNetworkReply::NoError)
	{		
		//return ok
		file->close();
		QString retVal;
		QMetaObject::invokeMethod(mparent, "HttpSuccessCallBack", Qt::DirectConnection,
			Q_RETURN_ARG(QString, retVal),
			Q_ARG(QString, mrdir));
		//QByteArray bytes = reply->readAll();  //获取字节
		//QString result(bytes);  //转化为字符串
		//qDebug() << result;
	}
	else
	{
		IsContinue( QString(QString::fromLocal8Bit("下载错误%1")).arg(status_code.toString()), reply);
		//处理错误
		if (400 == status_code.toInt()) {
		}
	}

	reply->deleteLater();//要删除reply，但是不能在repyfinished里直接delete，要调用deletelater;
	this->deleteLater();
}

void Http::Finished(QNetworkReply *reply) {

	QString retVal;
	QMetaObject::invokeMethod(mparent, "HttpSuccessCallBack", Qt::DirectConnection,
		Q_RETURN_ARG(QString, retVal),
		Q_ARG(QString, mrdir));
	//disconnect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	//disconnect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	reply->deleteLater();
}

void Http::IsContinue(QString title, QNetworkReply *reply) {
	QMessageBox msgBox;
	msgBox.setInformativeText(title);	
	QPushButton *yesButton = msgBox.addButton(QString::fromLocal8Bit("忽略错误，继续下载?"), QMessageBox::ActionRole);
	QPushButton *cancelButton = msgBox.addButton(QString::fromLocal8Bit("退出?"), QMessageBox::ActionRole);
	//QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
	if (msgBox.clickedButton() == (QAbstractButton*)yesButton) {
		Finished(reply);
	}
	else if (msgBox.clickedButton() == (QAbstractButton*)cancelButton) {
		exit(0);
	}
}

void Http::metaDataChanged() {
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int res = status_code.toInt();
	if ( 200 == res && false == hasInit) {
		QByteArray qb = QString("code").toUtf8();
		bool ishas = reply->hasRawHeader(qb);
		if (ishas) {
			QByteArray code = reply->rawHeader(qb);
			QString str(code);
			int ncode = code.toInt();

			switch (ncode)
			{
			case HASH_PARAM_ERROR:
			case HASH_SERVER_NO_FILE:
				{
					QString title;
					if (HASH_PARAM_ERROR == ncode) {
						title = QString(QString::fromLocal8Bit("下载错误，参数错误%1")).arg(finalUrl);
					}
					else {
						title = QString( QString::fromLocal8Bit("服务器没有这个文件！") + mrdir );
					}
					IsContinue( title, reply);
				}
				break;
			case HASH_EQUAL:
				{
					Finished(reply);
				}
				break;
			case HASH_WILL_DOWNLOAD:
				{
					bool res = file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::ReadOnly);
					if (!res) {
						IsContinue(QString(QString::fromLocal8Bit("无法打开文件,dir:%1")).arg(savedir), reply);
					}
					connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
					connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
				}
				break;
			default:
				break;
			}
			int k = 0;
			k++;
		}
		else {
			IsContinue(QString(QString::fromLocal8Bit("没有code头,dir:%1")).arg(mrdir), reply);
		}
	}
}

/***********更新进度条*************/
void Http::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal) {
	int k = 0;
	k++;
}

void Http::onReadyRead() {

	QByteArray ar = reply->readAll();
	qint64 size = ar.size(); 
	int tmp = ar.length();
	
	file->write(ar);	
	file->flush();
}
