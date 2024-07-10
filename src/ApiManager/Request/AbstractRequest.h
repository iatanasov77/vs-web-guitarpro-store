#ifndef ABSTRACTREQUEST_H
#define ABSTRACTREQUEST_H

#include <QObject>
#include <QString>
#include <QNetworkRequest>
#include "HttpRequestInput.h"
//#include "HttpRequestWorker.h"

/**
 * A Wrapper For Http Requests
 */
class AbstractRequest : public QObject
{
	Q_OBJECT

	protected:
		HttpRequestInput *_input;
		QNetworkRequest *_request;
		QByteArray _requestContent;

	public:
		QString commandId;
		QString requestName;

		AbstractRequest( HttpRequestInput *input,  QObject *parent = 0  );

		QNetworkRequest *request();
		HttpRequestInput *requestInput();
		QByteArray requestContent();
		virtual QNetworkRequest *createRequest() = 0; // pure virtual
};

#endif // ABSTRACTREQUEST_H
