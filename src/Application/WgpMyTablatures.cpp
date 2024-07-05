#include "WgpMyTablatures.h"

#include <QString>
#include <QVariant>
#include <QDebug>

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
	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYTABLATURES_REQUEST"], headers, true );
}

void WgpMyTablatures::_getMyCategories()
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-categories" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYCATEGORIES_REQUEST"], headers, true );
}

void WgpMyTablatures::_getMyTablaturesUncategorized()
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures-uncategorized" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"], headers, true );
}

void WgpMyTablatures::createTablatureCategory( QString $name )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-categories/new" );
	HttpRequestInput input( strUrl, "POST" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	input.addVar( "parentCategory", "" );
	input.addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["CREATE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::updateTablatureCategory( int categoryId, QString $name )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( QString( "/my-categories/%1" ).arg( categoryId ) );
	HttpRequestInput input( strUrl, "PUT" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	input.addVar( "parentCategory", "" );
	input.addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["UPDATE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::deleteTablatureCategory( int categoryId )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( QString( "/my-categories/%1" ).arg( categoryId ) );
	HttpRequestInput input( strUrl, "DELETE" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["DELETE_TABLATURE_CATEGORY_REQUEST"], headers, true );
}

void WgpMyTablatures::createTablature( QString $name )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( "/tablatures/new" );
	HttpRequestInput input( strUrl, "POST" );
	input.requestType	= REQUEST_TYPE_HTTP;
	QMap<QString, QString> headers;

	input.addVar( "parentCategory", "" );
	input.addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["CREATE_TABLATURE_REQUEST"], headers, true );
}

void WgpMyTablatures::updateTablature( int tablatureId, QString $name )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( QString( "/tablatures/%1" ).arg( tablatureId ) );
	HttpRequestInput input( strUrl, "PUT" );
	input.requestType	= REQUEST_TYPE_HTTP;
	QMap<QString, QString> headers;

	input.addVar( "parentCategory", "" );
	input.addVar( "name", $name );

	HttpRequestWorker::instance()->execute( input, HttpRequests["UPDATE_TABLATURE_REQUEST"], headers, true );
}

void WgpMyTablatures::deleteTablature( int tablatureId )
{
	QString strUrl	= VsApplication::instance()->apiUrl().append( QString( "/tablatures/%1" ).arg( tablatureId ) );
	HttpRequestInput input( strUrl, "DELETE" );
	input.requestType	= REQUEST_TYPE_JSON;
	QMap<QString, QString> headers;

	HttpRequestWorker::instance()->execute( input, HttpRequests["DELETE_TABLATURE_REQUEST"], headers, true );
}
