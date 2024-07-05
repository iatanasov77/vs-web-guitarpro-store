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
