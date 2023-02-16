#ifndef ABSTRACTREQUEST_H
#define ABSTRACTREQUEST_H

#include <QNetworkRequest>
#include "HttpRequestInput.h"
//#include "HttpRequestWorker.h"

/**
 * A Wrapper For Http Requests
 */
class AbstractRequest
{
	protected:
		HttpRequestInput *_input;
		QNetworkRequest *_request;
		QByteArray _requestContent;

	public:
		AbstractRequest( HttpRequestInput *input );

		QNetworkRequest *request();
		HttpRequestInput *requestInput();
		QByteArray requestContent();
		virtual QNetworkRequest *createRequest() = 0; // pure virtual
};

#endif // ABSTRACTREQUEST_H
