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

	m_httpRequestWorker = new HttpRequestWorker();

	connect(
		m_httpRequestWorker, SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) ),
		Qt::QueuedConnection
	);
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

void WgpMyTablatures::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	if ( worker->objectName() == TablaturesRequestTypes[GET_MY_CATEGORIES] ) {
		emit getMyCategoriesFinished( worker );
		_getMyTablaturesUncategorized();	// Should To Be Here
	} else if( worker->objectName() == TablaturesRequestTypes[GET_MY_TABLATURES] ) {
		emit getMyTablaturesFinished( worker );
		emit serverLoadFinished();
	} else {
		//qDebug() << "UNDEFINED MY TABLATURES REQUEST !!!";
	}
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
	QVariant authToken		= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	m_httpRequestWorker->setObjectName( TablaturesRequestTypes[GET_MY_TABLATURES] );
	m_httpRequestWorker->execute( &input, "Get MyTablatures Request", headers );
}

void WgpMyTablatures::_getMyCategories()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-categories" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	m_httpRequestWorker->setObjectName( TablaturesRequestTypes[GET_MY_CATEGORIES] );
	m_httpRequestWorker->execute( &input, "Get MyCategories Request", headers );
}

void WgpMyTablatures::_getMyTablaturesUncategorized()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "/my-tablatures-uncategorized" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	m_httpRequestWorker->setObjectName( TablaturesRequestTypes[GET_MY_TABLATURES] );
	m_httpRequestWorker->execute( &input, "Get MyTablaturesUncategorized Request", headers );
}
