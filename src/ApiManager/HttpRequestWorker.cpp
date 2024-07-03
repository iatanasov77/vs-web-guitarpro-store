#include "HttpRequestWorker.h"

#include <QDebug>
#include <QDateTime>
#include <QBuffer>
#include <QSaveFile>
#include <QMessageBox>

// Needed For Debug
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "GlobalTypes.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"
#include "ApiManager/HttpRequest.h"
#include "ApiManager/JsonRequest.h"

/*
 * This one did the trick!
 * ------------------------
 * But I want to do this: https://stackoverflow.com/questions/46172607/qt-singleton-implementation
 */
HttpRequestWorker *HttpRequestWorker::_instance = 0;

HttpRequestWorker::HttpRequestWorker( QObject *parent ) : QObject( parent )
{
	resetWorker();

    manager = new QNetworkAccessManager( this );
    connect(
    	manager, SIGNAL( finished( QNetworkReply* ) ),
		this, SLOT( onManagerFinished( QNetworkReply* ) )
    );

    connect(
		this, SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( handleRequest() )
	);

	connect(
		this, SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( sendNextRequest() ),
		Qt::QueuedConnection
	);
}

HttpRequestWorker *HttpRequestWorker::createInstance()
{
    return new HttpRequestWorker();
}

HttpRequestWorker *HttpRequestWorker::instance() {
	if ( ! _instance ) {
		_instance = createInstance();
	}

	return _instance;
}

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName )
{
	QMap<QString, QVariant> command;
	resetWorker();
	requestName	= strRequestName;

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	requestWrapper->requestName = requestName;
	requestWrapper->createRequest();

	command["request"]	= QVariant::fromValue( requestWrapper );
	command["executed"]	= QVariant( false );

	if ( commandStack.empty() ) {
		_sendRequest( requestWrapper );
	}
	commandStack << command;

	qDebug() << "HttpRequestWorker Sending Request With Input and RequestName ...";
}

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName, QMap<QString, QString> headers )
{
	QMap<QString, QVariant> command;
	resetWorker();
	requestName	= strRequestName;

	AbstractRequest *requestWrapper;
	if ( input->requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else {
		requestWrapper	= new HttpRequest( input );
	}

	requestWrapper->requestName = requestName;
	QNetworkRequest *request	= requestWrapper->createRequest();
	if ( ! headers.empty() ) {
		foreach ( const QString &key, headers.keys() ) {
			request->setRawHeader( key.toUtf8(), headers.value( key ).toUtf8() );
		}
	}

	command["requestType"]	= QVariant( input->requestType );
	command["request"]		= QVariant::fromValue( requestWrapper );
	command["executed"]		= QVariant( false );

	if ( commandStack.empty() ) {
		_sendRequest( requestWrapper );
	}
	commandStack << command;

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
	/*
	debugNetworkReply( reply );
	return;
	*/

    errorType = reply->error();
    if ( errorType == QNetworkReply::NoError ) {
        response = reply->readAll();
        //debugNetworkReplyResponse( "HttpRequestWorker::onManagerFinished", response );
    }
    else {
        errorStr = reply->errorString();
    }

    reply->deleteLater();
    emit workerFinished( this );
}

void HttpRequestWorker::sendNextRequest()
{
	bool sendNext = false;
	AbstractRequest *requestWrapper;
	HttpRequestType requestType;

	for ( int i = 0; i < commandStack.size(); ++i ) {
		requestType		= commandStack[i]["requestType"].value<HttpRequestType>();
		//qDebug() << "Request Type: " << commandStack[i]["requestType"].value<HttpRequestType>();

		if ( requestType == REQUEST_TYPE_JSON ) {
			requestWrapper 	= commandStack[i]["request"].value<JsonRequest*>();
		} else {
			requestWrapper 	= commandStack[i]["request"].value<HttpRequest*>();
		}

		if ( sendNext && lastFinishedequest != requestWrapper->requestName ) {
			_sendRequest( requestWrapper );
			break;
		}

		if ( requestWrapper->requestName == requestName ) {
			commandStack[i]["executed"].setValue( true );
			sendNext 			= true;
			lastFinishedequest 	= requestName;
			//qDebug() << "Current Request Executed: " << commandStack[i]["executed"].toBool();
		}
	}
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
	//QJsonArray results	= doc.array();
	QJsonObject results	= doc.object();

	//qDebug() << "Debug JSON Response: \n" << doc.toJson( QJsonDocument::Indented );

	QString debugString	= QString( "'%1' Result Size: %2" ).arg( debugFrom ).arg( results.size() );
	qDebug() << debugString;
}

void HttpRequestWorker::_sendRequest( AbstractRequest *requestWrapper )
{
	QNetworkRequest *request	= requestWrapper->request();
	HttpRequestInput *input		= requestWrapper->requestInput();

	/*
	QString requestUrl			= request->url().toString();
	qDebug() << "Request URL: " << requestUrl;
	return;
	*/

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

void HttpRequestWorker::handleRequest()
{
	if ( requestName == HttpRequests["LOGIN_REQUEST"] ) {
		handleLoginCheck();
	} else if( requestName == HttpRequests["GET_MYTABLATURES_REQUEST"] ) {
		handleMyTablaturesResult();
	} else if( requestName == HttpRequests["GET_MYCATEGORIES_REQUEST"] ) {
		handleMyCategoriesResult();
	} else if( requestName == HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"] ) {

	} else {
		qDebug() << "UNDEFINED HTTP REQUEST !!!";
	}
}

void HttpRequestWorker::handleLoginCheck()
{
    QString errorMsg;
	if ( errorType == QNetworkReply::NoError ) {
		emit loginCheckResponseReady( this );
	} else {
		// an error occurred
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

/*
void WgpMyTablatures::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	if ( objectName() == TablaturesRequestTypes[GET_MY_CATEGORIES] ) {
		emit getMyCategoriesFinished( this );
		_getMyTablaturesUncategorized();	// Should To Be Here
	} else if( objectName() == TablaturesRequestTypes[GET_MY_TABLATURES] ) {
		emit getMyTablaturesFinished( this );
		emit serverLoadFinished();
	} else {
		//qDebug() << "UNDEFINED MY TABLATURES REQUEST !!!";
	}
}
*/

void HttpRequestWorker::handleMyCategoriesResult()
{
	QString errorMsg;
	if (errorType == QNetworkReply::NoError ) {
		emit myCategoriesResponseReady( this );
	} else {
		// an error occurred
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablaturesResult()
{

	QString errorMsg;
	if ( errorType == QNetworkReply::NoError ) {
		emit myTablaturesResponseReady( this );
	} else {
		// an error occurred
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}
