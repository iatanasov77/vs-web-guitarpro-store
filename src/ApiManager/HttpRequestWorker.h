#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QCache>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>

#include "WorkerState.h"
#include "Request/AbstractRequest.h"
#include "Request/HttpRequestInput.h"

/**
 * https://www.creativepulse.gr/en/blog/2014/restful-api-requests-using-qt-cpp-for-linux-mac-osx-ms-windows
 *
 * https://stackoverflow.com/questions/51816213/restful-api-requests-using-qt
 */
class HttpRequestWorker : public QObject
{
    Q_OBJECT

	public:
		static HttpRequestWorker* instance();
		QNetworkAccessManager *manager();
		void execute( HttpRequestInput input, QString strRequestName, bool needAuthorization = false );
		void execute( HttpRequestInput input, QString strRequestName, QMap<QString, QString> headers, bool needAuthorization = false );

	signals:
		void workerFinished( WorkerState state );
		void loginCheckResponseReady( WorkerState state );
		void myTablatureDownloadResponseReady( WorkerState state );
		void myCategoriesResponseReady( WorkerState state );
		void myTablaturesResponseReady( WorkerState state );
		void myCategoryUpdateResponseReady( WorkerState state );
		void myTablatureUploadResponseReady( WorkerState state );

	private:
		static HttpRequestWorker *_instance;

		QNetworkAccessManager *_manager;
		WorkerState _state;
		QCache<int, QMap<QString, QVariant>> *commandStack;

		HttpRequestWorker( QObject *parent = 0 );
		static HttpRequestWorker* createInstance();

		QString generateCommandId() const;
		void resetWorker();
		void _authorizeRequest( QNetworkRequest *request );
		void _sendRequest( AbstractRequest *requestWrapper, bool needAuthorization );
		void debugNetworkReply( QNetworkReply *reply );
		void debugNetworkReplyResponse( QString debugFrom, QByteArray response );
		void debugCommand( QMap<QString, QVariant> command );
		void debugCommandStack();
		void debugRequest( QNetworkRequest *request );
		void debugAuthorizationSettings();

		void handleLoginCheck( WorkerState state );
		void handleMyTablatureDownload( WorkerState state );
		void handleMyCategoriesResult( WorkerState state );
		void handleMyTablaturesResult( WorkerState state );
		void handleMyTablaturesUncategorizedResult( WorkerState state );
		void handleUpdateCategoryResult( WorkerState state );
		void handleUploadTablatureResult( WorkerState state );

	private slots:
		void onManagerFinished( QNetworkReply *reply );
		void handleRequest( WorkerState state );
		void sendNextRequest( WorkerState state );

};

#endif // HTTPREQUESTWORKER_H
