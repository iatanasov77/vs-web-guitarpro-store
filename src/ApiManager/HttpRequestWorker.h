#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QCache>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>

#include "CommandState.h"
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
		void execute( HttpRequestInput *input, QString strRequestName, bool needAuthorization = false );
		void execute( HttpRequestInput *input, QString strRequestName, QMap<QString, QString> headers, bool needAuthorization = false );

	signals:
		void workerFinished( CommandState *state );
		void loginCheckResponseReady( CommandState *state );
		void myTablatureDownloadResponseReady( CommandState *state );
		void myCategoriesResponseReady( CommandState *state );
		void myTablaturesResponseReady( CommandState *state );
		void myCategoryUpdateResponseReady( CommandState *state );
		void myTablatureUploadResponseReady( CommandState *state );

	private:
		static HttpRequestWorker *_instance;

		QNetworkAccessManager *_manager;
		QMap<QString, CommandState*> commandState;
		QCache<int, QMap<QString, QVariant>> *commandStack;
		bool working;
		QString currentCommandId;

		HttpRequestWorker( QObject *parent = 0 );
		static HttpRequestWorker* createInstance();

		QString generateCommandId() const;
		void _authorizeRequest( QNetworkRequest *request );
		void _sendRequest( AbstractRequest *requestWrapper, bool needAuthorization );
		void debugNetworkReply( QNetworkReply *reply );
		void debugNetworkReplyResponse( QString debugFrom, QByteArray response );
		void debugCommand( QMap<QString, QVariant> command );
		void debugCommandStack();
		void debugRequest( QNetworkRequest *request );
		void debugAuthorizationSettings();

		void handleLoginCheck( CommandState *state );
		void handleMyTablatureDownload( CommandState *state );
		void handleMyCategoriesResult( CommandState *state );
		void handleMyTablaturesResult( CommandState *state );
		void handleMyTablaturesUncategorizedResult( CommandState *state );
		void handleUpdateCategoryResult( CommandState *state );
		void handleUploadTablatureResult( CommandState *state );

	private slots:
		void onManagerFinished( QNetworkReply *reply );
		void handleRequest( CommandState *state );
		void sendNextRequest( CommandState *state );

};

#endif // HTTPREQUESTWORKER_H
