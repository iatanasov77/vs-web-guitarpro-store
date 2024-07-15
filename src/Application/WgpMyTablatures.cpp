#include "WgpMyTablatures.h"

#include <QDebug>
#include <QString>
#include <QVariant>
#include <QMimeDatabase>

#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"

WgpMyTablatures *WgpMyTablatures::_instance = nullptr;

WgpMyTablatures::WgpMyTablatures( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );
}

WgpMyTablatures *WgpMyTablatures::createInstance()
{
    return new WgpMyTablatures();
}

WgpMyTablatures *WgpMyTablatures::instance()
{
	if ( _instance == nullptr ) {
		_instance = createInstance();
	}

	return _instance;
}

bool WgpMyTablatures::getMyTablatures()
{
	if ( VsAuth::instance()->isLoggedIn() ) {
		//_getMyTablatures();
		_getMyCategories();
		//qDebug() << "WgpMyTablatures::getMyTablatures() Running ...";

		return true;
	}

	return false;
}

void WgpMyTablatures::_getMyTablatures()
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( "/my-tablatures" );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "GET" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYTABLATURES_REQUEST"], headers, true );
}

void WgpMyTablatures::_getMyCategories()
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( "/my-categories" );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "GET" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYCATEGORIES_REQUEST"], headers, true );
}

void WgpMyTablatures::_getMyTablaturesUncategorized()
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( "/my-tablatures-uncategorized" );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "GET" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"], headers, true );
}

void WgpMyTablatures::createTablatureCategory( QString $name, int parentId )
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( "/my-categories/new" );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "POST" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	input->addVar( "parentCategory", QString::number( parentId ) );
	input->addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["CREATE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::updateTablatureCategory( int categoryId, QString $name )
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( QString( "/my-categories/%1" ).arg( categoryId ) );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "PUT" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	input->addVar( "parentCategory", "" );
	input->addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["UPDATE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::deleteTablatureCategory( int categoryId )
{
	if ( categoryId <= 0 ) {
		return;
	}

	QString strUrl			= VsApplication::instance()->apiUrl().append( QString( "/my-categories/%1" ).arg( categoryId ) );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "DELETE" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["DELETE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::createTablature( QString $name, QString filePath, int categoryId )
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( "/tablatures/new" );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "POST" );
	input->requestType		= REQUEST_TYPE_HTTP_MULTIPART;
	QMap<QString, QString> headers;

	input->addVar( "published", "true" );
	input->addVar( "artist", $name );
	input->addVar( "song", $name );
	input->addVar( "category", QString::number( categoryId ) );

	QString mimeType = QMimeDatabase().mimeTypeForFile( filePath ).name();
	input->addFile( filePath, "tablature", mimeType );

	HttpRequestWorker::instance()->execute( input, HttpRequests["CREATE_TABLATURE_REQUEST"], headers, true );
}

void WgpMyTablatures::updateTablature( int tablatureId, QString $name, QString filePath )
{
	QString strUrl			= VsApplication::instance()->apiUrl().append( QString( "/tablatures/%1" ).arg( tablatureId ) );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "PUT" );
	input->requestType		= REQUEST_TYPE_HTTP_MULTIPART;
	QMap<QString, QString> headers;

	input->addVar( "published", "true" );
	input->addVar( "artist", $name );
	input->addVar( "song", $name );

	QString mimeType = QMimeDatabase().mimeTypeForFile( filePath ).name();
	input->addFile( filePath, "tablature", mimeType );

	HttpRequestWorker::instance()->execute( input, HttpRequests["UPDATE_TABLATURE_REQUEST"], headers, true );
}

void WgpMyTablatures::deleteTablature( int tablatureId )
{
	if ( tablatureId <= 0 ) {
		return;
	}

	QString strUrl			= VsApplication::instance()->apiUrl().append( QString( "/tablatures/%1" ).arg( tablatureId ) );
	HttpRequestInput *input	= new HttpRequestInput( strUrl, "DELETE" );
	input->requestType		= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["DELETE_TABLATURE_REQUEST"], headers, true );
}
