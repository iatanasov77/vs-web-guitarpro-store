#include "HttpRequestWorker.h"

#include <QDebug>
#include <QDateTime>
#include <QBuffer>
#include <QSaveFile>

// Needed For Debug
#include <QJsonDocument>
#include <QJsonArray>

#include "GlobalTypes.h"
#include "ApiManager/HttpRequest.h"
#include "ApiManager/JsonRequest.h"

HttpRequestWorker::HttpRequestWorker( QObject *parent )
    : QObject( parent )
{
	resetWorker();

    manager = new QNetworkAccessManager( this );
    connect(
    	manager, SIGNAL( finished( QNetworkReply* ) ),
		this, SLOT( onManagerFinished( QNetworkReply* ) )
    );
}

void HttpRequestWorker::execute( HttpRequestInput *input )
{
	resetWorker();

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	requestWrapper->createRequest();
	_sendRequest( requestWrapper );

	qDebug() << "HttpRequestWorker Sending Request With Input ...";
}

void HttpRequestWorker::execute( HttpRequestInput *input, QMap<QString, QString> headers )
{
	resetWorker();

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	QNetworkRequest *request	= requestWrapper->createRequest();
	if ( ! headers.empty() ) {
		foreach ( const QString &key, headers.keys() ) {
			request->setRawHeader( key.toUtf8(), headers.value( key ).toUtf8() );
		}
	}

	_sendRequest( requestWrapper );

	qDebug() << "HttpRequestWorker Sending Request With Input and Headers ...";
}

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName )
{
	resetWorker();
	requestName	= strRequestName;

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	requestWrapper->createRequest();
	_sendRequest( requestWrapper );
}

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName, QMap<QString, QString> headers )
{
	resetWorker();
	requestName	= strRequestName;

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	QNetworkRequest *request	= requestWrapper->createRequest();
	if ( ! headers.empty() ) {
		foreach ( const QString &key, headers.keys() ) {
			request->setRawHeader( key.toUtf8(), headers.value( key ).toUtf8() );
		}
	}

	_sendRequest( requestWrapper );

	qDebug() << "HttpRequestWorker Sending Request With Input, RequestName and Headers ...";
}

void HttpRequestWorker::resetWorker()
{
	response 	= "";
	errorType	= QNetworkReply::NoError;
	errorStr 	= "";
	requestName	= "";
}

void HttpRequestWorker::onManagerFinished( QNetworkReply *reply )
{
	//debugNetworkReply( reply );
	//return;

    errorType = reply->error();
    if ( errorType == QNetworkReply::NoError ) {
        response = reply->readAll();

        debugNetworkReplyResponse( "HttpRequestWorker::onManagerFinished", response );
    }
    else {
        errorStr = reply->errorString();
    }

    reply->deleteLater();
    emit workerFinished( this );
}

void HttpRequestWorker::debugNetworkReply( QNetworkReply *reply )
{
	qDebug() << "REPLY ERROR TYPE: " << reply->error();

	qDebug() << "REPLY READ ALL: /vagrant/Temp/error.html";
	QSaveFile file( "/vagrant/Temp/error.html" );
	file.open( QIODevice::WriteOnly );
	file.write( reply->readAll() );
	file.commit(); // Calling commit() is mandatory, otherwise nothing will be written.
}

void HttpRequestWorker::debugNetworkReplyResponse( QString debugFrom, QByteArray baResponse )
{
	QJsonDocument doc	= QJsonDocument::fromJson( baResponse );
	QJsonArray results	= doc.array();

	QString debugString	= QString( "'%1' Result Size: %2" ).arg( debugFrom ).arg( results.size() );
	qDebug() << debugString;
}

void HttpRequestWorker::_sendRequest( AbstractRequest *requestWrapper )
{
	QNetworkRequest *request	= requestWrapper->request();
	HttpRequestInput *input		= requestWrapper->requestInput();

	if ( input->httpMethod == "GET" ) {
	    manager->get( *request );
	}
	else if ( input->httpMethod == "POST" ) {
		manager->post( *request, requestWrapper->requestContent() );
	}
	else if ( input->httpMethod == "PUT" ) {
		manager->put( *request, requestWrapper->requestContent() );
	}
	else if ( input->httpMethod == "HEAD" ) {
		manager->head( *request );
	}
	else if ( input->httpMethod == "DELETE" ) {
		manager->deleteResource( *request );
	}
	else {
		QByteArray requestContent	= requestWrapper->requestContent();
		QBuffer buff( & requestContent );
		manager->sendCustomRequest( *request, input->httpMethod.toLatin1(), &buff );
	}
}
