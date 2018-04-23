#include "global.h"
#include <QFile>
#include <QCryptographicHash>
#include <QMutex >
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageLogContext>

const QString GetMd5(const QString& filePath){
	QString sMd5;
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)){
		QByteArray bArray = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
		sMd5 = QString(bArray.toHex()).toUpper();
	}
	file.close();
	return sMd5;
}


bool IsDigitStr(QString src)
{
	QByteArray ba = src.toLatin1();//QString 转换为 char*
	const char *s = ba.data();
	while (*s && *s >= '0' && *s <= '9') s++;
	if (*s){ //不是纯数字
		return false;
	}else{ //纯数字
		return true;
	}
}


void LogText(const QString fileName, const QString &msg)
{	
	//return;//for release
	static QMutex mutex;
	mutex.lock();
		
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");	
	QString message = QString("%1 | %2").arg(current_date_time).arg(msg);

	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << message << "\r\n";
	file.flush();
	file.close();

	mutex.unlock();
}