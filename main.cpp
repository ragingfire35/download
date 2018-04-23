#include "download.h"
#include <QtWidgets/QApplication>
#include "global.h"
#include <QLibraryInfo>
#include <QDir>

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	QString runPath = QCoreApplication::applicationDirPath();

	download w;
	w.show();
	return a.exec();
}
