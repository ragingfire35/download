#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_download.h"
#include <QMap>
#include "getnumbypanfu.h"
#include <QNetworkReply>
#include "global.h"
#include <HWebView.h>
#include <QThread>


void CreateDir(QString path);

class WorkerThread : public QThread
{
public:
	WorkerThread(QString panfu, QString ms, QString mb, QString sub) {
		QThread::QThread(0);
		cur_panfu = panfu;
		cur_ms = ms;
		cur_mb = mb;
		subffix = sub;
	}

public:
	QString cur_panfu;
	QString cur_ms;
	QString cur_mb;
	QString subffix;
	void orderfile();

	void move_ms_to_mb();
	void move_mb_to_ms();
	
	void ExicuteBatCmd(QString moveStr);
	void MoveEachMS_MB(QString src, QString to);
	void MoveOneFile(QString src, QString to, int hide);
	void MoveBackAllFile(QString src, QString to);
	void DelBackMb(QString path);

	Q_OBJECT
		void run() override {
		QString result;
		/* ... here is the expensive or blocking operation ... */
		orderfile();
		emit OrderFinished(result);
	}
	signals:
		void OrderFinished(const QString &s);
};

class DelThread : public QThread
{
public:
	DelThread(QString panfu, QString ms, QString mb, QString sub, QList<QString>& dls) {
		QThread::QThread(0);
		cur_panfu = panfu;
		cur_ms = ms;
		cur_mb = mb;
		subffix = sub;
		del_lst = dls;
	}

public:
	QString cur_panfu;
	QString cur_ms;
	QString cur_mb;
	QString subffix;
	QList<QString> del_lst;
	void DelFile();
	Q_OBJECT
		void run() override {
		DelFile();
		QString result;
	}
signals:

};


class download : public QMainWindow
{
	Q_OBJECT

public:
	download(QWidget *parent = Q_NULLPTR);
private:
	void startWorkInAThread();
	void startDelThread();
private:
	void checkDate();
	void InitWebEngine();
	void Init();
	void ReadSignatue();
	QString GetSignatureJson();
	void GetuType();
	//void DownloadFile();
	//void UpdateFile();
	void UpdateFileNew();
	bool DealResponse(QString str);
	void DealUpdateTxt(QString str);
	void GetUpdateTxt();
	//void recusion_order(QString path);
	void recusion(QString path);
	void EnumlocalFile();
	QString ReadConf();
	void PasreConf();
	
	//void StartDownload();
	void StartDownloadNew();
	//void delList(QString rdir);
	//bool IsInLst(QString rdir);
	bool IsInLst(QList<FileObj*>& lst,QString dir);
	bool RemoveLst(QList<FileObj*>& lst, QString dir);
	//void AddToUpdateList(QString rdir);
	QString DelPanFuAndReplaceSlack(QString fullname);
	void mergeupdatelist();
	//void AddToUpdateList2norootdir(QString rdir);
	void resizeEvent(QResizeEvent*);
	
	//void DelFile(QString path, bool isfile);
	//void BackFile(QString path, bool isfile);
	//void copyback(QString path);
	//void ResueAllFile(QString path);
	//void ResueBackOneFile(QString path, bool isfile);
	//void DelBackMb(QString path);
	//void MdMB(QString path);

	void closeEvent(QCloseEvent *event);
protected:
	int initfile;
	void timerEvent(QTimerEvent *event);
public slots:	
	
	void OrderFinished(const QString &);
	//QString FinishedOneDownload(QString rdir);
	QString HttpSuccessCallBack(QString dir);
	//QString DownloadSize(qint64 size);
	void GetuTypeResponse(QNetworkReply* reply);
	void GetUpdateTxtRes_callback(QNetworkReply* reply);
private:
	Ui::downloadClass ui;

	//totalnum
	int totalNum = 0;

	//webview
	//QWebEngineView* view;
	HWebView* view;
	//end
	QString subffix;
	//res
	int line_code;//wehter response true;
	QString line_utype;
	//QList<QString> line_fileLst;
	QList<FileObj*> line_fileobj;
	//del list
	QList<QString> del_lst;
	//end
	//get,update.txt
	QString cur_panfu;
	QString cur_ms;//f:\ms
	QString cur_mb;//f:\mb
	QNetworkAccessManager* mGetupdatetxt;

	QList<FileObj*> newupdate_list;
	//
	QList<FileObj*> update_fileList;
	QList<FileObj*> local_fileList;
	//end
	QNetworkAccessManager* namUpload;
	ULONG cur_signature;	
	QMap<int, QString> m_KeyMap;
	std::map<std::wstring, ULONG> sigmap;
};
