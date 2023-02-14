#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

		explicit HttpRequestWorker( QObject *parent = 0 );
		void execute( HttpRequestInput *input );

	signals:
		void workerFinished( HttpRequestWorker *worker );

	private:
		QNetworkAccessManager *manager;
		void resetWorker();
		void debugNetworkReply( QNetworkReply *reply );

	private slots:
		void onManagerFinished( QNetworkReply *reply );

};

#endif // HTTPREQUESTWORKER_H
