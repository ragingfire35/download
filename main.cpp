#include "download.h"
#include <QtWidgets/QApplication>
#include "global.h"
#include <QLibraryInfo>

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	QString runPath = QCoreApplication::applicationDirPath();

	//qApp->addLibraryPath(runPath);
	//QString apdir = QCoreApplication::applicationFilePath();
	//QString rn = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	//QString dp = QLibraryInfo::location(QLibraryInfo::DataPath);

	qInstallMessageHandler(outputMessage);


	//qDebug("%s", runPath);
	//qDebug("%s", apdir);
	//qDebug("%s", dp);

	download w;
	w.show();
	return a.exec();
}
