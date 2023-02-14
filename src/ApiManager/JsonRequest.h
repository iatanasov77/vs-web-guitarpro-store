#ifndef JSONREQUEST_H
#define JSONREQUEST_H

#include "AbstractRequest.h"

class JsonRequest : public AbstractRequest
{
	public:
		JsonRequest( HttpRequestInput *input );

		QNetworkRequest createRequest();
};

#endif // JSONREQUEST_H
