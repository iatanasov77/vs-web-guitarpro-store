#ifndef JSONREQUEST_H
#define JSONREQUEST_H

#include <QObject>
#include "AbstractRequest.h"

class JsonRequest : public AbstractRequest
{
	Q_OBJECT

	public:
		JsonRequest( HttpRequestInput *input,  QObject *parent = 0 );

		QNetworkRequest *createRequest();
};

#endif // JSONREQUEST_H
