#include "JsonRequest.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

JsonRequest::JsonRequest( HttpRequestInput *input,  QObject *parent ) : AbstractRequest( input, parent ) { }

QNetworkRequest *JsonRequest::createRequest()
{
	QJsonObject object;

	foreach ( const QString &key, _input->vars.keys() ) {
		object.insert( key, _input->vars.value( key ) );
	}

	QJsonDocument doc	= QJsonDocument( object );
	_requestContent		= doc.toJson();

	// prepare connection
	_request->setRawHeader( "Content-Type", "application/json" );

	return _request;
}
