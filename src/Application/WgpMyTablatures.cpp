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
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	HttpRequestWorker::instance()->setObjectName( TablaturesRequestTypes[GET_MY_TABLATURES] );
	HttpRequestWorker::instance()->execute( &input, HttpRequests["GET_MYTABLATURES_REQUEST"], headers );
}

void WgpMyTablatures::_getMyCategories()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-categories" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	HttpRequestWorker::instance()->setObjectName( TablaturesRequestTypes[GET_MY_CATEGORIES] );
	HttpRequestWorker::instance()->execute( &input, HttpRequests["GET_MYCATEGORIES_REQUEST"], headers );
}

void WgpMyTablatures::_getMyTablaturesUncategorized()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures-uncategorized" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	HttpRequestWorker::instance()->setObjectName( TablaturesRequestTypes[GET_MY_TABLATURES] );
	HttpRequestWorker::instance()->execute( &input, HttpRequests["GET_MYTABLATURESUNCATEGORIZED_REQUEST"], headers );
}
