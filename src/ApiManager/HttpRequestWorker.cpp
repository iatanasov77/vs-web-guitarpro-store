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
	working = false;
	resetWorker();
	commandStack = new QCache<int, QMap<QString, QVariant>>();

    _manager = new QNetworkAccessManager( this );
    connect(
    	_manager, SIGNAL( finished( QNetworkReply* ) ),
		this, SLOT( onManagerFinished( QNetworkReply* ) )
    );

    connect(
		this, SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( handleRequest() )
	);

    /* */
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

void HttpRequestWorker::execute( HttpRequestInput input, QString strRequestName, bool needAuthorization )
{
	QMap<QString, QVariant> *command	= new QMap<QString, QVariant>();
	resetWorker();

	AbstractRequest *requestWrapper;
	if ( input.requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( &input );
	} else {
		requestWrapper	= new HttpRequest( &input );
	}

	requestWrapper->requestName = strRequestName;
	requestWrapper->createRequest();

	command->insert( "requestType", QVariant( input.requestType ) );
	command->insert( "request", QVariant::fromValue( requestWrapper ) );
	command->insert( "executed", QVariant( false ) );

	if ( ! working ) {
		//qDebug() << "HttpRequestWorker Sending Request With Input and RequestName ...";
		_sendRequest( requestWrapper, needAuthorization );
	}

	//debugCommand( *command );
	commandStack->insert( commandStack->size(), command );
}

void HttpRequestWorker::execute( HttpRequestInput input, QString strRequestName, QMap<QString, QString> headers, bool needAuthorization )
{
	QMap<QString, QVariant> *command	= new QMap<QString, QVariant>();
	resetWorker();

	AbstractRequest *requestWrapper;
	if ( input.requestType	== REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( &input );
	} else {
		requestWrapper	= new HttpRequest( &input );
	}

	requestWrapper->requestName = strRequestName;
	QNetworkRequest *request	= requestWrapper->createRequest();
	if ( ! headers.empty() ) {
		foreach ( const QString &key, headers.keys() ) {
			request->setRawHeader( key.toUtf8(), headers.value( key ).toUtf8() );
		}
	}

	command->insert( "requestType", QVariant( input.requestType ) );
	command->insert( "request", QVariant::fromValue( requestWrapper ) );
	command->insert( "executed", QVariant( false ) );

	if ( ! working ) {
		//qDebug() << "HttpRequestWorker Sending Request With Input, RequestName and Headers ...";
		_sendRequest( requestWrapper, needAuthorization );
	}

	//debugCommand( *command );
	commandStack->insert( commandStack->size(), command, commandStack->size() );
}

void HttpRequestWorker::resetWorker()
{
	response 	= "";
	errorType	= QNetworkReply::NoError;
	errorStr 	= "";
	//requestName	= "";
}

void HttpRequestWorker::onManagerFinished( QNetworkReply *reply )
{
	working = false;
	QNetworkRequest request	= reply->request();

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

	//debugCommandStack();
	for ( int i = 0; i < commandStack->size(); ++i ) {
		requestType		= commandStack->object( i )->value( "requestType" ).value<HttpRequestType>();

		if ( requestType == REQUEST_TYPE_JSON ) {
			requestWrapper 	= commandStack->object( i )->value( "request" ).value<JsonRequest*>();
		} else {
			requestWrapper 	= commandStack->object( i )->value( "request" ).value<HttpRequest*>();
		}

		if ( sendNext && lastFinishedRequest != requestWrapper->requestName ) {
			//qDebug() << "Next Request: " << requestWrapper->requestName;
			_sendRequest( requestWrapper, true );
			break;
		}

		//qDebug() << "Worker Request Name: " << requestName;
		if ( requestWrapper && requestWrapper->requestName == requestName ) {
			commandStack->object( i )->insert( "executed", QVariant( true ) );
			sendNext 			= true;
			lastFinishedRequest = requestName;

			//qDebug() << "Current Request Executed: " << commandStack->object( i )->value( "executed" ).toBool();
			//qDebug() << "Last Finished Request: " << lastFinishedRequest;
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

void HttpRequestWorker::debugCommand( QMap<QString, QVariant> command )
{
	AbstractRequest *requestWrapper;
	HttpRequestType requestType	= command["requestType"].value<HttpRequestType>();

	if ( requestType == REQUEST_TYPE_JSON ) {
		requestWrapper 	= command["request"].value<JsonRequest*>();
	} else {
		requestWrapper 	= command["request"].value<HttpRequest*>();
	}

	qDebug() << "Command Stack Size: " << commandStack->size();
	//qDebug() << "Command Stack Size At " << commandStack->size() << ": " << size_t( commandStack->size() );
	//qDebug() << "Command Stack Command Size: " << sizeof( command );

	qDebug() << "Wrapper Request: " << requestWrapper;
	qDebug() << "Wrapper Request Type: " << requestType;
	qDebug() << "Wrapper Request Name: " << requestWrapper->requestName;
}

void HttpRequestWorker::debugCommandStack()
{
	QMap<QString, QVariant> *command;

	for ( int i = 0; i < commandStack->size(); ++i ) {
		command	= commandStack->object( i );
		debugCommand( *command );
	}
}

void HttpRequestWorker::debugRequest( QNetworkRequest *request )
{
	QString requestUrl			= request->url().toString();
	qDebug() << "Request URL: " << requestUrl;
}

void HttpRequestWorker::debugAuthorizationSettings()
{
	QString authorizationHeaderKey		= "Authorization";
	VsSettings *oSettings				= VsSettings::instance();
	QVariant authToken					= oSettings->value(
		SettingsKeys["AUTH_PAYLOAD"],
		SettingsGroups["authentication"]
	).toHash().value( "token" );

	qDebug() << "Authorization Token: " << authToken.toString();
}

void HttpRequestWorker::_authorizeRequest( QNetworkRequest *request )
{
	QString authorizationHeaderKey		= "Authorization";
	VsSettings *oSettings				= VsSettings::instance();
	QVariant authToken					= oSettings->value(
		SettingsKeys["AUTH_PAYLOAD"],
		SettingsGroups["authentication"]
	).toHash().value( "token" );

	QString authorizationHeaderValue	= QString( "Bearer " ).append( authToken.toString() );
	request->setRawHeader( authorizationHeaderKey.toUtf8(), authorizationHeaderValue.toUtf8() );
}

void HttpRequestWorker::_sendRequest( AbstractRequest *requestWrapper, bool needAuthorization )
{
	working 					= true;
	requestName					= requestWrapper->requestName;
	QNetworkRequest *request	= requestWrapper->request();
	HttpRequestInput *input		= requestWrapper->requestInput();
	//debugRequest( request );
	//return;

	if ( needAuthorization ) {
		//debugAuthorizationSettings();
		//return;

		_authorizeRequest( request );
	}

	if ( input->httpMethod == "GET" ) {
	    _manager->get( *request );
	}
	else if ( input->httpMethod == "POST" ) {
		_manager->post( *request, requestWrapper->requestContent() );
	}
	else if ( input->httpMethod == "PUT" ) {
		_manager->put( *request, requestWrapper->requestContent() );
	}
	else if ( input->httpMethod == "HEAD" ) {
		_manager->head( *request );
	}
	else if ( input->httpMethod == "DELETE" ) {
		_manager->deleteResource( *request );
	}
	else {
		QByteArray requestContent	= requestWrapper->requestContent();
		QBuffer buff( & requestContent );
		_manager->sendCustomRequest( *request, input->httpMethod.toLatin1(), &buff );
	}
}

void HttpRequestWorker::handleRequest()
{
	if ( requestName == HttpRequests["LOGIN_REQUEST"] ) {
		//return;
		handleLoginCheck();
	} else if( requestName == HttpRequests["GET_MYTABLATURES_REQUEST"] ) {
		//return;
		handleMyTablaturesResult();
	} else if( requestName == HttpRequests["GET_MYCATEGORIES_REQUEST"] ) {
		//return;
		handleMyCategoriesResult();
	} else if( requestName == HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"] ) {
		//return;
		handleMyTablaturesUncategorizedResult();
	} else if( requestName == HttpRequests["CREATE_TABLATURE_CATEGORY_REQUEST"] || requestName == HttpRequests["UPDATE_TABLATURE_CATEGORY_REQUEST"] ) {
		//return;
		handleUpdateCategoryResult();
	} else if( requestName == HttpRequests["CREATE_TABLATURE_REQUEST"] || requestName == HttpRequests["UPDATE_TABLATURE_REQUEST"] ) {
		//return;
		handleUploadTablatureResult();
	} else {
		qDebug() << "UNDEFINED HTTP REQUEST !!!";
	}
}

void HttpRequestWorker::handleLoginCheck()
{
    QString errorMsg;

    //qDebug() << "Worker Error Type: " << errorType;
	//return;

	if ( errorType == QNetworkReply::NoError ) {
		emit loginCheckResponseReady( this );
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyCategoriesResult()
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << errorType;
	//return;

	if ( errorType == QNetworkReply::NoError ) {
		emit myCategoriesResponseReady( this );
		//_getMyTablaturesUncategorized();	// Should To Be Here
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablaturesResult()
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << errorType;
	//return;

	if ( errorType == QNetworkReply::NoError ) {
		emit myTablaturesResponseReady( this );
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablaturesUncategorizedResult()
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << errorType;
	//return;

	if ( errorType == QNetworkReply::NoError ) {
		emit myTablaturesResponseReady( this );
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleUpdateCategoryResult()
{
	QString errorMsg;

	if ( errorType == QNetworkReply::NoError ) {
		emit myCategoryUpdateResponseReady( this );
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleUploadTablatureResult()
{
	QString errorMsg;

	if ( errorType == QNetworkReply::NoError ) {
		emit myTablatureUploadResponseReady( this );
	} else {
		errorMsg	= "Error: " + errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

QNetworkAccessManager *HttpRequestWorker::manager()
{
	return _manager;
}
