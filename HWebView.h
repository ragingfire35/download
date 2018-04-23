#pragma once

#include "global.h"
#include <QWebEngineView>


class HWebView : public QWebEngineView
{
	Q_OBJECT
public:
	HWebView(QWidget *parent);
	~HWebView();
	void Init(QUrl url);
public slots :
	void onUrlChanged(QUrl url);
	void loadFinished(bool ok);
protected:
	virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
private:
	QUrl mOldUrl;
	QWidget *mParent;
};