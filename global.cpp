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


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	return;
	static QMutex mutex;
	mutex.lock();

	QString text;
	switch (type)
	{
	case QtDebugMsg:
		text = QString("Debug:");
		break;

	case QtWarningMsg:
		text = QString("Warning:");
		break;

	case QtCriticalMsg:
		text = QString("Critical:");
		break;

	case QtFatalMsg:
		text = QString("Fatal:");
	}

	QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	//QString current_date = QString("(%1)").arg(current_date_time);
	QString message = QString("%1 %2 %3| %4").arg(text).arg(current_date_time).arg(context_info).arg(msg);

	QFile file("log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << message << "\r\n";
	file.flush();
	file.close();

	mutex.unlock();
}