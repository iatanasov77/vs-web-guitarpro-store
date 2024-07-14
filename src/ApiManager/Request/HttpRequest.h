#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include "AbstractRequest.h"

class HttpRequest : public AbstractRequest
{
	Q_OBJECT

	public:
		HttpRequest( HttpRequestInput *input,  QObject *parent = 0 );
		QNetworkRequest *createRequest();
};

#endif // HTTPREQUEST_H
