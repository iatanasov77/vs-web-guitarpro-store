#include "AbstractRequest.h"

AbstractRequest::AbstractRequest( HttpRequestInput *input,  QObject *parent  )
{
	_input			= input;
	_request		= nullptr;
	_requestContent	= "";
}

QNetworkRequest *AbstractRequest::request()
{
	return _request;
}

HttpRequestInput *AbstractRequest::requestInput()
{
	return _input;
}

QByteArray AbstractRequest::requestContent()
{
	return _requestContent;
}
