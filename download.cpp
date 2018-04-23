#include "download.h"
#include<iostream>
#include <QMessageBox> 
#include <QTextStream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QDir>
#include "http_get.h"
#include <shlwapi.h>
#include <QProcess>
#include <QCloseEvent>
#include <QMessageBox>
#include <QWebEngineProfile>
#include <QWebEngineHistory>
#include <QThread>
#include <QFileIconProvider>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


download::download(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon("images/logo.ico"));
	setWindowTitle("下载");
	Init();
}

void IncreaseTime() {

}

BOOL SetFileToCurrentTime(HANDLE hFile,int i)
{
	FILETIME ft;
	FILETIME ft2;
	FILETIME ft3;
	SYSTEMTIME st;
	SYSTEMTIME st2;
	SYSTEMTIME st3;
	BOOL f;

	//GetSystemTime(&st);              // Gets the current system time
	//WORD year = st.wYear;
	//WORD month = st.wMonth;
	//WORD minute = st.wMinute;
	//st.wMinute += i;


	QDateTime qdt = QDateTime::fromString("20060101 00:00:00","yyyyMMdd HH:mm:ss");
	static int k = 0;
	qdt = qdt.addSecs(60*k++);

	QDate qd = qdt.date();
	QTime qt = qdt.time();
	int wYear = 0;
	int wMonth = 0;
	int wDay = 0;
	qd.getDate(&wYear,&wMonth,&wDay);

	st.wYear = wYear;
	st.wMonth = wMonth;
	st.wDay = wDay;
	st.wDayOfWeek = qd.dayOfWeek();

	st.wHour = qt.hour();
	st.wMinute = qt.minute();
	st.wSecond = qt.second();
	st.wMilliseconds = 0;
		
	memcpy(&st2,&st,sizeof(SYSTEMTIME));
	memcpy(&st3,&st,sizeof(SYSTEMTIME));
	st.wDay = st.wDay + i;
	st2.wDay = st2.wDay+1+i;
	st3.wDay = st3.wDay+2+i;

	st2.wMinute += 1;
	st3.wMinute += 2;

	SystemTimeToFileTime(&st, &ft);  // Converts the current system time to file time format
	SystemTimeToFileTime(&st2, &ft2);
	SystemTimeToFileTime(&st3, &ft3);

	f = SetFileTime(hFile,           // Sets last-write time of the file 
		&ft,           // to the converted current system time 
		&ft3,
		&ft2);

	return f;
}

void HideItem(const wchar_t* fullName)  //隐藏文件    
{
	//BOOL FileAttribute =SetFileAttributes(dir,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	BOOL FileAttribute = SetFileAttributes(fullName, FILE_ATTRIBUTE_HIDDEN);

}

void ModifiedCreateTime(const wchar_t* dir,bool isfile,int i) {
	HANDLE hFile;

	hFile = CreateFile(dir, // open One.txt
		GENERIC_READ | GENERIC_WRITE,             // open for reading
			0,                        // do not share
			NULL,                     // no security
			OPEN_EXISTING,            // existing file only		
		isfile ? FILE_ATTRIBUTE_HIDDEN:FILE_FLAG_BACKUP_SEMANTICS,
			NULL);                    // no attr. template

	if (hFile == INVALID_HANDLE_VALUE){		
		return;
	}
	SetFileToCurrentTime(hFile,i);
	CloseHandle(hFile);
}

void download::checkDate() {
	QDate ex = QDate::fromString(expire, "yyyyMMdd");
	QDate nw = QDate::currentDate();
	if (nw > ex) {
		QMessageBox::information(NULL, "请支付三期款", "请支付三期款,谢谢", QMessageBox::Yes);
		exit(0);
	}
}
void download::resizeEvent(QResizeEvent* rs)
{	
	//QSize qs2(1024+30,768+5);
	//view->resize(qs2);
	
	//QSize wsize(this->height(),this->width());
	//this->setFixedSize(qs2);
}

QString generateRandomNumber()
{
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	//for (int i = 0; i<10; i++)
	//{
		//int test = qrand() % 10;
		
	//}
	QString num = QString::number( qrand() );
	return num;
}

void download::InitWebEngine() {
	view = new HWebView(this);

	QSize qs2(1024 + 30, 768 + 5);
	view->resize(qs2);

	QString rnd = generateRandomNumber();
	QString url = domain + advuri+ "?ran=" + rnd;
	view->Init(url);
}

void download::Init()
{		
	//Qt::WindowFlags flag = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint;
	//diable scale.	
	//this->setWindowFlags(flag);
	//this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);

	//disable maxbutton
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint);
	

	initfile = startTimer(2000);

	this->ui.update_progressBar->setRange(0, 100);
	this->ui.update_progressBar->setValue(0);
	InitWebEngine();

	//set fixed size.
	QSize qs(this->width(),this->height());
	this->setFixedSize(qs);

	checkDate();
	PasreConf();
	ReadSignatue();

}

QString download::ReadConf()
{
	QFile f("download.conf");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::information(NULL, "打开配置文件", "打开配置文件download.conf失败", QMessageBox::Yes | QMessageBox::No);
		exit(0);
	}
	QTextStream txtInput(&f);
	QString lineStr;
	while (!txtInput.atEnd())
	{
		QString temp = txtInput.readLine();
		lineStr += temp;
	}
	f.close();
	return lineStr;
}

void download::PasreConf() {

	QString conf = ReadConf();
	//QByteArray qb = conf.toUtf8();
	//char* cd = qb.data();

	QJsonParseError complex_json_error;
	QJsonDocument complex_parse_doucment = QJsonDocument::fromJson(conf.toUtf8(), &complex_json_error);
	int i = 0;
	if (complex_json_error.error == QJsonParseError::NoError)
	{
		QJsonObject jsonObject = complex_parse_doucment.object();
		if (jsonObject.contains("subffix"))
		{
			QJsonValue value = jsonObject.value("subffix");
			if (value.isString())
			{
				subffix = value.toString();
			}
		}
	}
	else
	{
		QMessageBox::information(NULL, "配置文件错误", "配置文件错误！请检查配置文件！", QMessageBox::Yes | QMessageBox::No);
		exit(0);
	}
}

bool download::DealResponse(QString str)
{
	QJsonParseError complex_json_error;
	QJsonDocument complex_parse_doucment = QJsonDocument::fromJson(str.toUtf8(), &complex_json_error);
	int i = 0;
	if (complex_json_error.error == QJsonParseError::NoError)
	{
		QJsonObject jsonObject = complex_parse_doucment.object();
		if (jsonObject.contains("code"))
		{
			QJsonValue value = jsonObject.value("code");
			if(value.isDouble() )
			{
				line_code = value.toVariant().toInt();
				if (0 != line_code) {
					return false;
				}
			}
		}
		if (jsonObject.contains("utype"))
		{
			QJsonValue value = jsonObject.value("utype");
			if( value.isString() )
			{
				line_utype = value.toString();
			}

		}
		if (jsonObject.contains("filelist") )
		{
			QJsonValue value = jsonObject.value("filelist");
			if (value.isArray())//判断他是不是json数组  
			{
				QJsonArray arr = value.toArray();

				for (int i = 0; i < arr.size(); i++) {
					QJsonValue value = arr.at(i);
					if (value.isObject())
					{
						QJsonObject obj = value.toObject();

						FileObj* flobj = new FileObj();

						if (obj.contains("rdir")) {
							QJsonValue value = obj.value("rdir");
							if (value.isString()) {
								QString rdir = value.toString();
								flobj->rdir = rdir;
							}
						}
						if (obj.contains("size")) {
							QJsonValue value = obj.value("size");
							if (value.isDouble()) {
								qint64 size = value.toInt();
								flobj->size = size;
							}
						}
						if (obj.contains("hash")) {
							QJsonValue value = obj.value("hash");
							if (value.isString()) {
								QString hash = value.toString();
								flobj->hash = hash;
							}
						}
						
						//line_fileobj.append(qs);
						line_fileobj.append(flobj);
					}
				}
			}
		}

	}
	else
	{
		QMessageBox::information(NULL, "服务器响应错误", "非法json内容！", QMessageBox::Yes | QMessageBox::No);
		exit(0);
		return false;
	}
	return true;
}




QString download::DelPanFuAndReplaceSlack(QString fullname)
{
	QString tmp =  fullname.replace("\\", "/");
	//int i = fullname.indexOf("\\");
	if (-1 != tmp.indexOf("/")) {
		tmp = tmp.right((tmp.length() - tmp.indexOf("/") - 1));
	}
	return tmp;
}

void download::recusion(QString path)
{
	QDir dir(path);
	dir.setFilter(QDir::AllEntries|QDir::Hidden);

	foreach(QFileInfo mfi, dir.entryInfoList())
	{
		QString dirName = mfi.baseName();
		if (mfi.isFile())
		{			
			QString kr = mfi.absoluteFilePath();// path +"\\" + mfi.fileName();

			QFileInfo fileinfo;
			fileinfo = QFileInfo(kr);

			QString ex = fileinfo.suffix();
			QString tmp = DelPanFuAndReplaceSlack(kr);
			
			QString baseName = fileinfo.baseName();

			if (-1 != subffix.indexOf(ex) &&  ( -1!=baseName.indexOf("-")   || true==IsDigitStr(baseName) )  ) {

				FileObj* obj = new FileObj();
				obj->rdir = tmp;
				QString hsh = GetMd5(kr);				
				obj->hash = hsh.toLower();
				obj->size = fileinfo.size();
				
				local_fileList.append(obj);
				qDebug() << "File :" << tmp;
			}
		}
		else if( -1 != mfi.baseName().indexOf("-") )
		{
			if (mfi.fileName() == "." || mfi.fileName() == "..")continue;
			//qDebug() << "Entry Dir" << mfi.absoluteFilePath();
			QString abdir = mfi.absoluteFilePath();
			recusion(abdir);
		}
	}
}

void download::EnumlocalFile() {
	recusion(cur_ms);
}

void download::mergeupdatelist() {
	if (line_fileobj.size() > 0) {
		QList<FileObj*>::iterator it = line_fileobj.begin();
		for (;it!= line_fileobj.end();it++) {
			FileObj* line_item = *it;

			QList<FileObj*>::iterator itlocal = local_fileList.begin();
			bool has = false;
			
			for (; itlocal != local_fileList.end(); itlocal++) {
				FileObj* local_item = *itlocal;
				if ( line_item->hash == local_item->hash  ) {
					if (line_item->rdir == local_item->rdir ) {
						has = true;
						break;
					}
				}
			}
			//not  in local
			if (false == has) {
				if (-1 == update_fileList.indexOf(line_item)) {
					update_fileList.append(line_item);
					//AddToUpdateList2norootdir(line_item);
				}
			}
		}
	}
	//StartDownload();
}

//void download::StartDownload() {
//	if (0==update_fileList.size()) {
//		ui.update_progressBar->setRange(0,100);
//		ui.update_progressBar->setValue(100);
//		orderfile();
//		QMessageBox::information(NULL, "更新", "成功更新完毕！", QMessageBox::Yes);
//		return;
//	}	
//
//	QList<FileObj*>::iterator it = update_fileList.begin();
//	for (;it!= update_fileList.end();it++) {
//		FileObj* item = *it;
//		total_size += item->size;
//	}
//	
//	this->ui.update_progressBar->setRange(0,total_size);
//	this->ui.update_progressBar->setValue(0);
//	UpdateFile();
//}


void download::StartDownloadNew() {
	if (0 == newupdate_list.size()) {
		ui.update_progressBar->setRange(0, 100);
		ui.update_progressBar->setValue(100);
		startWorkInAThread();		
		return;
	}
	//init progress again.
	this->ui.update_progressBar->setRange(0, newupdate_list.size());
	this->ui.update_progressBar->setValue(0);
	UpdateFileNew();
}



void download::UpdateFileNew()
{
	if (0 != newupdate_list.size()) {
		FileObj* item = newupdate_list.at(0);
		newupdate_list.pop_front();

		Http* pd = new Http();//need delete.
		//QString rurl = QString(domain) + line_utype + "/" + item->rdir;
		QString durl = QString(domain) + DOWN_FILE;
		QString fdir = cur_ms +"\\"+ item->rdir;
		pd->httpDownload(this, durl, fdir, item->rdir, line_utype);
	}
	//	qDebug() << item->rdir;
	//}
}


enum {
	SRC_UNHIDE,
	DES_HIDED
};

void HideItem(const wchar_t* fullName,int hide)  //隐藏文件    
{
	if (SRC_UNHIDE == hide) {
		BOOL FileAttribute = SetFileAttributes(fullName, FILE_ATTRIBUTE_NORMAL);
	}
	if (DES_HIDED == hide) {
		BOOL FileAttribute = SetFileAttributes(fullName, FILE_ATTRIBUTE_HIDDEN);
	}
}

void CreateDir(QString path) {
	path.replace("/", "\\");

	QDir dir(path);
	//if (dir.exists()) {		
	//	QProcess p(0);
	//	QString cmd = QString("rd  /S /Q %1").arg(path);
	//	p.start("cmd", QStringList() << "/c" << cmd);

	//	p.waitForStarted();
	//	p.waitForFinished(-1);
	//}

	QProcess p(0);
	QString cmd = QString("md %1").arg(path);
	p.start("cmd", QStringList() << "/c" << cmd);

	p.waitForStarted();
	p.waitForFinished(-1);
	QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());
}
void WorkerThread::MoveOneFile(QString src,QString to, int hide) {
	src.replace("/","\\");
	to.replace("/", "\\");
	QProcess p(0);
	QString cmd = QString("move %1 %2").arg(src).arg(to);
	p.start("cmd", QStringList() << "/c" << cmd);

	p.waitForStarted();
	p.waitForFinished(-1);
	QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());

	if (DES_HIDED == hide) {
		std::wstring str = to.toStdWString();
		HideItem(str.c_str(), hide);
	}
}


void WorkerThread::ExicuteBatCmd(QString moveStr) {
	QProcess p(0);		
	LogText(LogFileName, moveStr);

	p.start("cmd", QStringList() << "/c" << moveStr);	
	p.waitForStarted();
	p.waitForFinished(-1);

	QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());

	LogText(LogFileName, strTemp);
}

//move cmd.
void WorkerThread::MoveBackAllFile(QString src,QString to) {
	src.replace("/", "\\");
	to.replace("/", "\\");

	QDir dir(src);
	dir.setFilter(QDir::AllEntries | QDir::Hidden);
	QMap<int, QString> map;
	foreach(QFileInfo mfi, dir.entryInfoList())
	{
		if (mfi.isFile()) {
			QString kr = mfi.absoluteFilePath();
			kr = kr.replace("/", "\\");

			QFileInfo fileinfo;
			fileinfo = QFileInfo(kr);
			QString ex = fileinfo.suffix();
			QString name = fileinfo.baseName();

			if (-1 != subffix.indexOf(ex)) {				
				if (-1 != name.indexOf("-")) {
					QString strnum = name.left(name.indexOf("-"));
					if (IsDigitStr(strnum)) {
						int index = strnum.toInt();

						QMap<int,QString>::const_iterator i = map.find(index);
						if (i == map.end()) {
							map[index] = kr;
						}
						else {
							map[-1*index] = kr;
						}						
					}
				}
				else if (IsDigitStr(name)) {
					int index = name.toInt();

					QMap<int, QString>::const_iterator i = map.find(index);
					if (i == map.end()) {
						map[index] = kr;
					}
					else {
						map[-1 * index] = kr;
					}					
				}
				//if (hasadd = true&&SRC_UNHIDE == hide) {
				//	std::wstring str = kr.toStdWString();
				//	HideItem(str.c_str(), hide);
				//}
			}
		}
		else {
			if (mfi.fileName() == "." || mfi.fileName() == "..") {
				continue;
			}
			QString kr = mfi.absoluteFilePath();
			kr = kr.replace("/", "\\");

			QDir curdir(kr);
			QString name = curdir.dirName();

			if (-1 != name.indexOf("-")) {
				QString strnum = name.left(name.indexOf("-"));
				if (IsDigitStr(strnum)) {
					int index = strnum.toInt();
					
					QMap<int, QString>::const_iterator i = map.find(index);
					if (i == map.end()) {
						map[index] = kr;
					}
					else {
						map[-1 * index] = kr;
					}
				}
			}
			else if (IsDigitStr(name)) {
				int index = name.toInt();			

				QMap<int, QString>::const_iterator i = map.find(index);
				if (i == map.end()) {
					map[index] = kr;
				}
				else {
					map[-1 * index] = kr;
				}
			}
			//if (hasadd) {
			//	if (SRC_UNHIDE == hide) {
			//		std::wstring str = kr.toStdWString();
			//		HideItem(str.c_str(), hide);
			//	}
			//	//create dir
			//	//CreateDir(to + name + "\\");
			//}
		}
	}//end forreach
	 //modified creatime.
	QMap<int, QString>::iterator itmap = map.begin();
	int i = 0;
	for (; itmap != map.end(); itmap++, i++) {
		int first = itmap.key();
		QString sec = itmap.value();
		QFileInfo mfi(sec);
		bool isfile = mfi.isFile();
		QString kr = mfi.absoluteFilePath();
		kr = kr.replace("/", "\\");

		QString NewToFullfileName = to + "\\" + mfi.fileName();

		if (isfile) {

			std::wstring srcstd = kr.toStdWString();
			std::wstring tostd = NewToFullfileName.toStdWString();
			MoveFile(srcstd.c_str(),tostd.c_str() );
			//HideItem(tostd.c_str(), DES_HIDED);
		}
		else {
			QDir curdir(kr);
			QString name = curdir.dirName();
			QString newTo = to + "\\" + name;
			
			QString cmd = QString("md %1").arg(newTo);
			ExicuteBatCmd( cmd );

			MoveBackAllFile(kr, newTo);
						
			QString cmd1 = QString("rd /s /q %2").arg(kr);
			ExicuteBatCmd(cmd1);
			//if (DES_HIDED == hide) {
			//	QString tmp = to + name + "\\";
			//	std::wstring str = tmp.toStdWString();
			//	HideItem(str.c_str(), hide);
			//}
		}
	}
}

void WorkerThread::DelBackMb(QString path) {
	QProcess p(0);
	QString cmd = QString("rd  /S /Q %1").arg(path);	
	ExicuteBatCmd( cmd );
}

void WorkerThread::MoveEachMS_MB(QString src,QString to)
{
	src.replace("/", "\\");
	to.replace("/", "\\");

ReDir:
	QDir dir(src);
	dir.setFilter(QDir::AllEntries | QDir::Hidden);
	QMap<int, QString> map;
	QList<QString> dirlist;
	int size = dir.entryInfoList().size();
	
	foreach(QFileInfo mfi, dir.entryInfoList())
	{
		if (mfi.isFile()) {
			QString kr = mfi.absoluteFilePath();
			kr = kr.replace("/", "\\");

			QFileInfo fileinfo;
			fileinfo = QFileInfo(kr);
			QString fileName = fileinfo.fileName(); //fileinfo.baseName();
			
			QString cmd =  QString("move /y %1\\* %2").arg(src).arg(to);
			ExicuteBatCmd(cmd);
			goto ReDir;
		}
		else {
			if (mfi.fileName() == "." || mfi.fileName() == "..") {
				continue;
			}
			QString kr = mfi.absoluteFilePath();
			kr = kr.replace("/", "\\");

			QDir curdir(kr);
			QString name = curdir.dirName();

			QDir dir(to + "\\" + name);
			if ( dir.exists() ) {
				MoveEachMS_MB( kr, to + "\\" + name);
				
				QString cmd = QString("rd /s /q %1").arg(kr);
				ExicuteBatCmd(cmd);
			}
			else {				
				QString cmd = QString("move /y %1 %2").arg(kr).arg(to + "\\" + name);
				ExicuteBatCmd(cmd);
			}
		}
	}//end forreach
}
void WorkerThread::move_ms_to_mb()
{
	QDir dir(cur_mb);
	if ( dir.exists( ) ) {
		MoveEachMS_MB(cur_ms,cur_mb);
		return;
	}
	QProcess p(0);
	QString cmd = QString("move /Y %1 %2").arg(cur_ms).arg(cur_mb);
	ExicuteBatCmd(cmd);

}

void WorkerThread::move_mb_to_ms(){	
	MoveBackAllFile(cur_mb, cur_ms);	
}

void WorkerThread::orderfile()
{		
	std::wstring ms = cur_ms.toStdWString();
	HideItem(ms.c_str(), SRC_UNHIDE);
	move_ms_to_mb();
	CreateDir(cur_ms);
	move_mb_to_ms();
	DelBackMb(cur_mb);
	HideItem(ms.c_str(), DES_HIDED);
}

void DelThread::DelFile() {
	QList<QString>::iterator it = del_lst.begin();
	for (; it != del_lst.end(); it++) {
		QString item = *it;
		QString fullName = cur_ms + "\\" + item;
		std::wstring str = fullName.toStdWString();
		bool rs = DeleteFile(str.c_str());
	}
}

bool download::IsInLst(QList<FileObj*>& lst,QString dir) {
	QList<FileObj*>::iterator it = lst.begin();
	for (;it!=lst.end();it++) {
		FileObj* item = *it;
		if (item->rdir == dir) {
			return true;
		}
	}
	return false;
}

bool download::RemoveLst(QList<FileObj*>& lst, QString dir) {
	QList<FileObj*>::iterator it = lst.begin();
	for (; it != lst.end();) {
		FileObj* item = *it;
		if (item->rdir == dir) {			
			it == lst.erase(it++);
		}
		else {
			it++;
		}
	}
	return true;
}

//QString download::DownloadSize(qint64 size) {
//	has_down_size += size;
//	ui.update_progressBar->setValue( has_down_size );
//	return "";
//}

//QString download::FinishedOneDownload(QString rdir) {
//
//	return "";
//}

void download::startWorkInAThread()
{
	WorkerThread *workerThread = new WorkerThread(cur_panfu,cur_ms,cur_mb,subffix);
	connect(workerThread, &WorkerThread::OrderFinished, this, &download::OrderFinished);
	connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
	workerThread->start();
}

void download::startDelThread()
{
	DelThread *workerThread = new DelThread(cur_panfu, cur_ms, cur_mb, subffix, del_lst);
	workerThread->start();
}



void download::OrderFinished(const QString &) {
	ui.update_progressBar->setRange(0, 100);
	ui.update_progressBar->setValue(100);
	QMessageBox::information(NULL, "更新", "成功更新完毕！", QMessageBox::Yes);
}
QString download::HttpSuccessCallBack(QString dir) {

	if (true == IsInLst(newupdate_list,dir)) {
		RemoveLst(newupdate_list,dir);
	}
	if (0 == newupdate_list.size()) {		
		startWorkInAThread();
	}
	else {
		//change progress
		ui.update_progressBar->setValue( totalNum - newupdate_list.size() );
		UpdateFileNew();
	}
	return "";
}


void download::DealUpdateTxt(QString str) {
	QJsonParseError complex_json_error;
	QJsonDocument complex_parse_doucment = QJsonDocument::fromJson(str.toUtf8(), &complex_json_error);
	int i = 0;
	if (complex_json_error.error == QJsonParseError::NoError){
		QJsonObject jsonObject = complex_parse_doucment.object();
		if (jsonObject.contains("update")) {
			QJsonValue value = jsonObject.value("update");
			if (value.isArray()) {
				QJsonArray ar = value.toArray();
				for (int i = 0; i<ar.size(); i++) {
					QJsonValue job = ar.at(i);  // 遍历 jsonarray 数组，把每一个对象转成 json 对象
					if (job.isString()) {
						QString str = job.toString();
						FileObj *obj = new FileObj();
						obj->rdir = str;						
						newupdate_list.append(obj);
					}
					else if (job.isObject()) {
						QJsonObject jobj =	job.toObject();
						QString refdir, subfix;
						int startnum, endnum=0;

						if (jobj.contains("refdir")) {
							QJsonValue value = jobj.value("refdir");
							if (value.isString()) {
								refdir = value.toString();
							}	
						}
						if (jobj.contains("startnum")) {
							QJsonValue value = jobj.value("startnum");
							if (value.isDouble()) {
								startnum = value.toInt();
							}
						}
						if (jobj.contains("endnum")) {
							QJsonValue value = jobj.value("endnum");
							if ( value.isDouble() ) {
								endnum = value.toInt();
							}
						}
						if (jobj.contains("subfix")) {
							QJsonValue value = jobj.value("subfix");
							if (value.isString()) {
								subfix = value.toString();
							}
						}
						//add to list
						for (int j = startnum; j <= endnum;j++) {
							QString dir = QString("%1%2%3").arg(refdir).arg(j).arg(subfix);
							FileObj *obj = new FileObj();
							obj->rdir = dir;
							newupdate_list.append(obj);
						}
					}
				}
			}			
		}
		if (jsonObject.contains("deletelist")) {
			QJsonValue value = jsonObject.value("deletelist");
			if (value.isArray()) {
				QJsonArray ar = value.toArray();
				for (int i = 0; i < ar.size(); i++) {
					QJsonValue job = ar.at(i);  // 遍历 jsonarray 数组，把每一个对象转成 json 对象
					if (job.isString()) {
						QString str = job.toString();
						del_lst.append(str);
					}
				}
			}
		}
		if (del_lst.size() > 0) {
			startDelThread();
		}
	}
}
void download::GetUpdateTxt() {
	QString js = GetSignatureJson();

	namUpload = new QNetworkAccessManager(this);
	QObject::connect(namUpload, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(GetUpdateTxtRes_callback(QNetworkReply*)));

	QString rurl = QString(domain) + line_utype + "/"+ UPDATE_TXT;
	QUrl url(rurl);
	qDebug() << "querurl:" << rurl;
	//QNetworkReply* reply = namUpload->post(QNetworkRequest(url), js.toUtf8());
	QNetworkReply* reply = namUpload->get(QNetworkRequest(url));
}

void download::GetUpdateTxtRes_callback(QNetworkReply* reply) {

	QVariant statusCodeV =
		reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if (200 != statusCodeV) {
		QString str = QString::number(statusCodeV.toInt());
		QMessageBox::information(NULL, "失败", "获取update.txt失败,httperr:" + str, QMessageBox::Yes);
		return;
	}

	// Or the target URL if it was a redirect:  
	QVariant redirectionTargetUrl =
		reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	if (reply->error() == QNetworkReply::NoError)
	{

		QByteArray bytes = reply->readAll();
		QString string = QString::fromUtf8(bytes);
		qDebug() << string;
		DealUpdateTxt(string);
		
		totalNum = newupdate_list.size();
		StartDownloadNew();
		//StartDownload();
	}
	// Some http error received  
	else
	{
		// handle errors here  
	}

	// We receive ownership of the reply object  
	// and therefore need to handle deletion.
	reply->deleteLater();
}
void download::GetuTypeResponse(QNetworkReply* reply)
{
	QVariant statusCodeV =
		reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if (200 != statusCodeV) {
		QString str = QString::number(statusCodeV.toInt());
		QMessageBox::information(NULL, QString::fromLocal8Bit("查询"), QString::fromLocal8Bit("查询类型失败,httperr:") + str, QMessageBox::Yes | QMessageBox::No);
		return;
	}

	// Or the target URL if it was a redirect:  
	QVariant redirectionTargetUrl =
		reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	if (reply->error() == QNetworkReply::NoError)
	{

		QByteArray bytes = reply->readAll(); 											  
		QString string = QString::fromUtf8(bytes);
		qDebug() << string;
		//std::string tmp = string.toStdString();
		//qDebug() << tmp;
		//QJsonParseError complex_json_error;
		//QJsonDocument complex_parse_doucment = QJsonDocument::fromJson(string.toUtf8(), &complex_json_error);
		//if (complex_json_error.error == QJsonParseError::NoError){
		//	
		//	qDebug() << QString(complex_parse_doucment.toJson());
		//}
		//
		DealResponse(string);
		
		if (0 == line_code) {//success.
			//mergeupdatelist();
			GetUpdateTxt();
		}	
	}
	// Some http error received  
	else
	{
		// handle errors here  
	}

	// We receive ownership of the reply object  
	// and therefore need to handle deletion.
	reply->deleteLater();
}



void download::GetuType()
{
	QString js = GetSignatureJson();

	namUpload = new QNetworkAccessManager(this);
	QObject::connect(namUpload, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(GetuTypeResponse(QNetworkReply*)));
	
	QString rurl = QString(domain) + "only_query_uType.php";
	QUrl url(rurl);
	qDebug() << "querurl:"<<rurl;
	QNetworkReply* reply = namUpload->post(QNetworkRequest(url), js.toUtf8());
}



void download::ReadSignatue()
{
	QString runPath = QCoreApplication::applicationDirPath();
	QChar curpf = runPath.at(0);
	cur_panfu = curpf;
	cur_ms = cur_panfu+":\\music";
	cur_mb = cur_panfu + ":\\musictls";
	FindAllDrivers(sigmap);
	for (std::map<std::wstring, ULONG>::iterator it = sigmap.begin(); it != sigmap.end(); it++)
	{
		std::wstring key = it->first;
		ulong signatrue = it->second;

		QString ky = QString::fromStdWString(key);
		QChar kpf = ky.at(0);
		if (curpf == kpf){
			cur_signature = signatrue;
			break;
		}
	}
}
QString download::GetSignatureJson()
{
	QJsonObject json;
	json.insert("signature", (qint64)cur_signature);

	QJsonDocument document;
	document.setObject(json);
	QByteArray byte_array = document.toJson(QJsonDocument::Compact);
	QString json_str(byte_array);
	return json_str;
}

void download::closeEvent(QCloseEvent *event)
{
	QMessageBox msgBox;
	msgBox.setInformativeText("退出?");
	QPushButton *yesButton = msgBox.addButton("是的", QMessageBox::ActionRole);
	QPushButton *cancelButton = msgBox.addButton("取消", QMessageBox::ActionRole);
	//QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
	if ( msgBox.clickedButton() == (QAbstractButton*)yesButton ) {
		// connect
		event->accept();
	}
	else if (msgBox.clickedButton() == (QAbstractButton*)cancelButton) {
		// abort
		event->ignore();
	}	
	//event->ignore();
}


void download::timerEvent(QTimerEvent *event)
{
	if (initfile == event->timerId()) {
		//startWorkInAThread();		
		//killTimer(initfile);
		//return;
		GetuType();
		killTimer(initfile);
	}	
}