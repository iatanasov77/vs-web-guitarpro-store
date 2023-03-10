#include "JsonRequest.h"

#include <QUrl>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

JsonRequest::JsonRequest( HttpRequestInput *input ) : AbstractRequest( input ) { }

QNetworkRequest *JsonRequest::createRequest()
{
	QJsonObject object;

	foreach ( const QString &key, _input->vars.keys() ) {
		object.insert( key, _input->vars.value( key ) );
	}

	QJsonDocument doc	= QJsonDocument( object );
	_requestContent		= doc.toJson();

	// prepare connection
	_request = new QNetworkRequest( QUrl( _input->httpUrl ) );
	_request->setRawHeader( "Content-Type", "application/json" );

	return _request;
}
