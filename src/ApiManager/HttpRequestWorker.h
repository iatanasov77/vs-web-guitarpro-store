#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QCache>
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

		static HttpRequestWorker* instance();
		QNetworkAccessManager *manager();
		void execute( HttpRequestInput input, QString strRequestName, bool needAuthorization = false );
		void execute( HttpRequestInput input, QString strRequestName, QMap<QString, QString> headers, bool needAuthorization = false );

	signals:
		void workerFinished( HttpRequestWorker *worker );
		void loginCheckResponseReady( HttpRequestWorker *worker );
		void myCategoriesResponseReady( HttpRequestWorker *worker );
		void myTablaturesResponseReady( HttpRequestWorker *worker );

	private:
		static HttpRequestWorker *_instance;

		QNetworkAccessManager *_manager;
		QCache<int, QMap<QString, QVariant>> *commandStack;

		QString errorStr;
		QString requestName;
		QString lastFinishedRequest;

		HttpRequestWorker( QObject *parent = 0 );
		static HttpRequestWorker* createInstance();

		void resetWorker();
		void _authorizeRequest( QNetworkRequest *request );
		void _sendRequest( AbstractRequest *requestWrapper, bool needAuthorization );
		void debugNetworkReply( QNetworkReply *reply );
		void debugNetworkReplyResponse( QString debugFrom, QByteArray response );
		void debugCommand( QMap<QString, QVariant> command );
		void debugCommandStack();
		void debugRequest( QNetworkRequest *request );
		void debugAuthorizationSettings();

		void handleLoginCheck();
		void handleMyCategoriesResult();
		void handleMyTablaturesResult();
		void handleMyTablaturesUncategorizedResult();

	private slots:
		void onManagerFinished( QNetworkReply *reply );
		void handleRequest();
		void sendNextRequest();

};

#endif // HTTPREQUESTWORKER_H
