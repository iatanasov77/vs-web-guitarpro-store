#include "AbstractRequest.h"

#include <QUrl>

AbstractRequest::AbstractRequest( HttpRequestInput *input,  QObject *parent  ) : QObject( parent )
{
	_input			= input;
	_requestContent	= "";

	_request = new QNetworkRequest( QUrl( _input->httpUrl ) );
	_request->setRawHeader( "User-Agent", "VS WebGuitarPro Store" );
	//_request->setRawHeader( "Accept", "*/*" );
	//_request->setRawHeader( "Accept-Encoding", "gzip, deflate, br" );
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
