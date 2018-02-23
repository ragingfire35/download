#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_download.h"
#include <QMap>
#include "getnumbypanfu.h"
#include <QNetworkReply>
#include "global.h"
#include <HWebView.h>

class download : public QMainWindow
{
	Q_OBJECT

public:
	download(QWidget *parent = Q_NULLPTR);
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
	void orderfile();
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


	void CreateDir(QString path);
	void MoveOneFile(QString src, QString to, int hide);
	void MoveBackAllFile(QString cur_panfu, QString src, QString to, int hide);

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
	QString HttpSuccessCallBack(QString dir);
	QString DownloadSize(qint64 size);
	void GetuTypeResponse(QNetworkReply* reply);
	void GetUpdateTxtRes_callback(QNetworkReply* reply);
private:
	Ui::downloadClass ui;

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
	//end
	//get,update.txt
	QString cur_panfu;
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
