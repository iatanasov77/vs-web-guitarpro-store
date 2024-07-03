#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>

#include "AbstractRequest.h"
#include "HttpRequestInput.h"

/**
 * https://www.creativepulse.gr/en/blog/2014/restful-api-requests-using-qt-cpp-for-linux-mac-osx-ms-windows
 *
 * https://stackoverflow.com/questions/51816213/restful-api-requests-using-qt
 */
class HttpRequestWorker : public QObject
{
    Q_OBJECT

	public:
		QByteArray response;
		QNetworkReply::NetworkError errorType;
		QString errorStr;
		QString requestName;
		QString lastFinishedequest;

		static HttpRequestWorker* instance();
		void execute( HttpRequestInput *input, QString strRequestName );
		void execute( HttpRequestInput *input, QString strRequestName, QMap<QString, QString> headers );

	signals:
		void workerFinished( HttpRequestWorker *worker );
		void loginCheckResponseReady( HttpRequestWorker *worker );
		void myCategoriesResponseReady( HttpRequestWorker *worker );
		void myTablaturesResponseReady( HttpRequestWorker *worker );

	private:
		HttpRequestWorker( QObject *parent = 0 );
		static HttpRequestWorker* createInstance();
		static HttpRequestWorker *_instance;
		QObject *handler;

		QNetworkAccessManager *manager;
		QList<QMap<QString, QVariant>> commandStack;
		void resetWorker();
		void _sendRequest( AbstractRequest *requestWrapper );
		void debugNetworkReply( QNetworkReply *reply );
		void debugNetworkReplyResponse( QString debugFrom, QByteArray response );

		void handleLoginCheck();
		void handleMyCategoriesResult();
		void handleMyTablaturesResult();

	private slots:
		void onManagerFinished( QNetworkReply *reply );
		void handleRequest();
		void sendNextRequest();

};

#endif // HTTPREQUESTWORKER_H
