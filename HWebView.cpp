#include <HWebView.h>
#include <QDesktopServices>

HWebView::HWebView(QWidget *parent)
	: QWebEngineView(parent) {
	mParent = parent;
}

HWebView::~HWebView() {

}

void HWebView::Init(QUrl url) {
		
	this->load(QUrl(url));
	this->show();
	connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
	//connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}


void HWebView::onUrlChanged(QUrl url) {
	QString str = url.toString();

	if (-1 == str.indexOf("adv.php")) {
		mOldUrl = url;
		this->back();
		QDesktopServices::openUrl(url);
	}
}

QWebEngineView *HWebView::createWindow(QWebEnginePage::WebWindowType type) {
	//HWebView* view = new HWebView(mParent);
	//view->setAttribute(Qt::WA_DeleteOnClose);
	//view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//
	//return view;

	Q_UNUSED(type);
	return this;
}

void HWebView::loadFinished(bool ok) {
	int k = 0;
	k++;
}