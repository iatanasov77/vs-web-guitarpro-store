#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "AbstractRequest.h"

class HttpRequest : public AbstractRequest
{
	public:
		HttpRequest( HttpRequestInput *input );

		QNetworkRequest *createRequest();
		QString httpAttributeEncode( QString attributeName, QString input );
};

#endif // HTTPREQUEST_H
