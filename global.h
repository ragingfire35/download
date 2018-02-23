#ifndef GLOBAL_H  
#define GLOBAL_H  
#include <QString>
#include <QtGlobal>
#include <QDebug>

//const QString domain = "http://tian.cow8.cn/";
const QString domain = "http://biaoqian.cn/";
const QString expire = "20180301";
const QString advuri = "adv/adv.php";
const QString DOWN_FILE = "/download.php";
const QString UPDATE_TXT = "update.txt";



class FileObj {
public:
	QString rdir;
	qint64 size;
	QString hash;
};


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

const QString GetMd5(const QString& filePath);
bool IsDigitStr(QString src);

static qint64 total_size = 0;
static qint64 has_down_size = 0;

#endif