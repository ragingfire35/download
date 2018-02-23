/********************************************************************************
** Form generated from reading UI file 'download.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOAD_H
#define UI_DOWNLOAD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_downloadClass
{
public:
    QWidget *centralWidget;
    QProgressBar *update_progressBar;
    QLabel *label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *downloadClass)
    {
        if (downloadClass->objectName().isEmpty())
            downloadClass->setObjectName(QStringLiteral("downloadClass"));
        downloadClass->resize(1057, 868);
        centralWidget = new QWidget(downloadClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        update_progressBar = new QProgressBar(centralWidget);
        update_progressBar->setObjectName(QStringLiteral("update_progressBar"));
        update_progressBar->setGeometry(QRect(20, 790, 991, 23));
        update_progressBar->setValue(24);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 770, 191, 16));
        downloadClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(downloadClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1057, 23));
        downloadClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(downloadClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        downloadClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(downloadClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        downloadClass->setStatusBar(statusBar);

        retranslateUi(downloadClass);

        QMetaObject::connectSlotsByName(downloadClass);
    } // setupUi

    void retranslateUi(QMainWindow *downloadClass)
    {
        downloadClass->setWindowTitle(QApplication::translate("downloadClass", "\344\270\213\350\275\275", Q_NULLPTR));
        label->setText(QApplication::translate("downloadClass", "\344\270\213\350\275\275\350\277\233\345\272\246:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class downloadClass: public Ui_downloadClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOAD_H
