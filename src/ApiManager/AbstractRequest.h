#ifndef ABSTRACTREQUEST_H
#define ABSTRACTREQUEST_H

#include "HttpRequestWorker.h"

class AbstractRequest
{
	protected:
		HttpRequestInput *_input;
		QByteArray _requestContent;

	public:
		AbstractRequest( HttpRequestInput *input );

		QByteArray requestContent();
		virtual QNetworkRequest createRequest() = 0; // pure virtual
};

#endif // ABSTRACTREQUEST_H
