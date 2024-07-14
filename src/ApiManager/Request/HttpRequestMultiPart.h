#ifndef HTTPREQUESTMULTIPART_H
#define HTTPREQUESTMULTIPART_H

#include <QObject>
#include "AbstractRequest.h"

class HttpRequestMultiPart : public AbstractRequest
{
	Q_OBJECT

	public:
		HttpRequestMultiPart( HttpRequestInput *input,  QObject *parent = 0 );
		QNetworkRequest *createRequest();
};

#endif // HTTPREQUESTMULTIPART_H
