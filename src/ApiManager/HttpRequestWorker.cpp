#include "HttpRequestWorker.h"

#include <QDebug>
#include <QDateTime>
#include <QBuffer>
#include <QSaveFile>
#include <QMessageBox>
#include <QRandomGenerator>

// Needed For Debug
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "GlobalTypes.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"
#include "ApiManager/Request/HttpRequest.h"
#include "ApiManager/Request/JsonRequest.h"
#include "ApiManager/Request/DownloadRequest.h"

/*
 * This one did the trick!
 * ------------------------
 * But I want to do this: https://stackoverflow.com/questions/46172607/qt-singleton-implementation
 */
HttpRequestWorker *HttpRequestWorker::_instance = 0;

HttpRequestWorker::HttpRequestWorker( QObject *parent ) : QObject( parent )
{
	commandStack = new QCache<int, QMap<QString, QVariant>>();

    _manager = new QNetworkAccessManager( this );
    connect(
    	_manager, SIGNAL( finished( QNetworkReply* ) ),
		this, SLOT( onManagerFinished( QNetworkReply* ) )
    );

    connect(
		this, SIGNAL( workerFinished( CommandState* ) ),
		this, SLOT( handleRequest( CommandState* ) )
	);

    /* */
	connect(
		this, SIGNAL( workerFinished( CommandState* ) ),
		this, SLOT( sendNextRequest( CommandState* ) )
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

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName, bool needAuthorization )
{
	QMap<QString, QVariant> *command	= new QMap<QString, QVariant>();
	CommandState *state	= new CommandState();

	AbstractRequest *requestWrapper;
	if ( input->requestType == REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else if( input->requestType == REQUEST_TYPE_HTTP ) {
		requestWrapper	= new HttpRequest( input );
	} else if( input->requestType == REQUEST_TYPE_DOWNLOAD ) {
		requestWrapper	= new DownloadRequest( input );
	}

	QString commandId			= generateCommandId();
	requestWrapper->commandId 	= commandId;
	requestWrapper->requestName = strRequestName;
	requestWrapper->createRequest();

	command->insert( "commandId", QVariant( commandId ) );
	command->insert( "requestType", QVariant( input->requestType ) );
	command->insert( "request", QVariant::fromValue( requestWrapper ) );
	command->insert( "executed", QVariant( false ) );

	//debugCommand( *command );
	commandState[commandId]	= state;
	commandStack->insert( commandStack->size(), command );
	//debugCommandStack();

	if ( ! working ) {
		//qDebug() << "HttpRequestWorker Sending Request With Input and RequestName ...";
		_sendRequest( requestWrapper, needAuthorization );
	}
}

void HttpRequestWorker::execute( HttpRequestInput *input, QString strRequestName, QMap<QString, QString> headers, bool needAuthorization )
{
	QMap<QString, QVariant> *command	= new QMap<QString, QVariant>();
	CommandState *state	= new CommandState();

	AbstractRequest *requestWrapper;
	if ( input->requestType == REQUEST_TYPE_JSON ) {
		requestWrapper	= new JsonRequest( input );
	} else if( input->requestType == REQUEST_TYPE_HTTP ) {
		requestWrapper	= new HttpRequest( input );
	} else if( input->requestType == REQUEST_TYPE_DOWNLOAD ) {
		requestWrapper	= new DownloadRequest( input );
	}

	QString commandId			= generateCommandId();
	requestWrapper->commandId 	= commandId;
	requestWrapper->requestName = strRequestName;
	QNetworkRequest *request	= requestWrapper->createRequest();
	if ( ! headers.empty() ) {
		foreach ( const QString &key, headers.keys() ) {
			request->setRawHeader( key.toUtf8(), headers.value( key ).toUtf8() );
		}
	}

	command->insert( "commandId", QVariant( commandId ) );
	command->insert( "requestType", QVariant( input->requestType ) );
	command->insert( "request", QVariant::fromValue( requestWrapper ) );
	command->insert( "executed", QVariant( false ) );

	//debugCommand( *command );
	commandState[commandId]	= state;
	commandStack->insert( commandStack->size(), command );
	//debugCommandStack();

	if ( ! working ) {
		//qDebug() << "HttpRequestWorker Sending Request With Input, RequestName and Headers ...";
		_sendRequest( requestWrapper, needAuthorization );
	}
}

void HttpRequestWorker::onManagerFinished( QNetworkReply *reply )
{
	working = false;
	QNetworkRequest request	= reply->request();

	commandState[currentCommandId]->errorType = reply->error();
    if ( commandState[currentCommandId]->errorType == QNetworkReply::NoError ) {
    	commandState[currentCommandId]->response = reply->readAll();
        debugNetworkReplyResponse( "HttpRequestWorker::onManagerFinished", commandState[currentCommandId]->response );
    }
    else {
    	commandState[currentCommandId]->errorStr = reply->errorString();
    }

    reply->deleteLater();
    emit workerFinished( commandState[currentCommandId] );
}

void HttpRequestWorker::sendNextRequest( CommandState *state )
{
	bool sendNext = false;
	AbstractRequest *requestWrapper;
	HttpRequestType requestType;

	debugCommandStack();
	for ( int i = 0; i < commandStack->size(); ++i ) {
		requestType		= commandStack->object( i )->value( "requestType" ).value<HttpRequestType>();

		if ( requestType == REQUEST_TYPE_JSON ) {
			requestWrapper	= commandStack->object( i )->value( "request" ).value<JsonRequest*>();
		} else if( requestType == REQUEST_TYPE_HTTP ) {
			requestWrapper	= commandStack->object( i )->value( "request" ).value<HttpRequest*>();;
		} else if( requestType == REQUEST_TYPE_DOWNLOAD ) {
			requestWrapper	= commandStack->object( i )->value( "request" ).value<DownloadRequest*>();
		}

		if ( sendNext && state->lastFinishedRequest != requestWrapper->commandId ) {
			qDebug() << "Next Request: " << requestWrapper->commandId;
			_sendRequest( requestWrapper, true );
			break;
		}

		//qDebug() << "Worker Request Name: " << state.requestName;
		if ( requestWrapper && requestWrapper->commandId == state->commandId ) {
			commandStack->object( i )->insert( "executed", QVariant( true ) );
			sendNext 			= true;
			state->lastFinishedRequest = state->commandId;

			qDebug() << "Current Request Executed: " << commandStack->object( i )->value( "executed" ).toBool();
			qDebug() << "Last Finished Request: " << state->lastFinishedRequest;
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
		requestWrapper	= command["request"].value<JsonRequest*>();
	} else if( requestType == REQUEST_TYPE_HTTP ) {
		requestWrapper	= command["request"].value<HttpRequest*>();
	} else if( requestType == REQUEST_TYPE_DOWNLOAD ) {
		requestWrapper	= command["request"].value<DownloadRequest*>();
	}

	qDebug() << "Command Stack Size: " << commandStack->size();
	//qDebug() << "Command Stack Size At " << commandStack->size() << ": " << size_t( commandStack->size() );
	//qDebug() << "Command Stack Command Size: " << sizeof( command );

	qDebug() << "Wrapper Request: " << requestWrapper;
	qDebug() << "Wrapper Request Type: " << requestType;
	qDebug() << "Wrapper Request Name: " << requestWrapper->requestName;
	qDebug() << "Wrapper Command ID: " << requestWrapper->commandId;
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
	QNetworkRequest *request	= requestWrapper->request();
	HttpRequestInput *input		= requestWrapper->requestInput();

	currentCommandId											= requestWrapper->commandId;
	commandState[requestWrapper->commandId]->commandId			= currentCommandId;
	commandState[requestWrapper->commandId]->requestName		= requestWrapper->requestName;
	commandState[requestWrapper->commandId]->downloadingFile	= input->targetPath;

	if ( needAuthorization ) {
		_authorizeRequest( request );
	}

	/*
	if ( requestWrapper->requestName == HttpRequests["DOWNLOAD_TABLATURE_REQUEST"] ) {
		debugRequest( request );
		debugAuthorizationSettings();
		return;
	}
	*/

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

void HttpRequestWorker::handleRequest( CommandState *state )
{
	if ( state->requestName == HttpRequests["LOGIN_REQUEST"] ) {
		//return;
		handleLoginCheck( state );
	} else if( state->requestName == HttpRequests["DOWNLOAD_TABLATURE_REQUEST"] ) {
		//return;
		handleMyTablatureDownload( state );
	} else if( state->requestName == HttpRequests["GET_MYTABLATURES_REQUEST"] ) {
		//return;
		handleMyTablaturesResult( state );
	} else if( state->requestName == HttpRequests["GET_MYCATEGORIES_REQUEST"] ) {
		//return;
		handleMyCategoriesResult( state );
	} else if( state->requestName == HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"] ) {
		//return;
		handleMyTablaturesUncategorizedResult( state );
	} else if( state->requestName == HttpRequests["CREATE_TABLATURE_CATEGORY_REQUEST"] || state->requestName == HttpRequests["UPDATE_TABLATURE_CATEGORY_REQUEST"] ) {
		//return;
		handleUpdateCategoryResult( state );
	} else if( state->requestName == HttpRequests["CREATE_TABLATURE_REQUEST"] || state->requestName == HttpRequests["UPDATE_TABLATURE_REQUEST"] ) {
		//return;
		handleUploadTablatureResult( state );
	} else {
		qDebug() << "UNDEFINED HTTP REQUEST !!!";
	}
}

void HttpRequestWorker::handleLoginCheck( CommandState *state )
{
    QString errorMsg;

    //qDebug() << "Worker Error Type: " << state->errorType;
	//return;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit loginCheckResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablatureDownload( CommandState *state )
{
	QString errorMsg;

	qDebug() << "Worker Error Type: " << state->errorType;
	return;

	if ( state->errorType == QNetworkReply::NoError ) {
		state->downloadedFile	= state->downloadingFile;
		emit myTablatureDownloadResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyCategoriesResult( CommandState *state )
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << state->errorType;
	//return;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit myCategoriesResponseReady( state );
		//_getMyTablaturesUncategorized();	// Should To Be Here
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablaturesResult( CommandState *state )
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << state->errorType;
	//return;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit myTablaturesResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleMyTablaturesUncategorizedResult( CommandState *state )
{
	QString errorMsg;

	//qDebug() << "NetworkReply NoErrorType: " << QNetworkReply::NoError;
	//qDebug() << "Worker Error Type: " << state->errorType;
	//return;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit myTablaturesResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleUpdateCategoryResult( CommandState *state )
{
	QString errorMsg;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit myCategoryUpdateResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void HttpRequestWorker::handleUploadTablatureResult( CommandState *state )
{
	QString errorMsg;

	if ( state->errorType == QNetworkReply::NoError ) {
		emit myTablatureUploadResponseReady( state );
	} else {
		errorMsg	= "Error: " + state->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

QNetworkAccessManager *HttpRequestWorker::manager()
{
	return _manager;
}

QString HttpRequestWorker::generateCommandId() const
{
   const QString possibleCharacters( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" );
   const int randomStringLength = 12; // assuming you want random strings of 12 characters

   QString randomString;
   for( int i=0; i < randomStringLength; ++i )
   {
	   quint32 index = QRandomGenerator::global()->generate() % possibleCharacters.length();;
       //int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at( index );
       randomString.append( nextChar );
   }

   return randomString;
}
