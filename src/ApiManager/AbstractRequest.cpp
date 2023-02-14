#include "AbstractRequest.h"

AbstractRequest::AbstractRequest( HttpRequestInput *input )
{
	_input			= input;
	_requestContent	= "";
}

QByteArray AbstractRequest::requestContent()
{
	return _requestContent;
}

