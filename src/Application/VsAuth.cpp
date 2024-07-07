#include "VsAuth.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsSettings.h"

VsAuth *VsAuth::_instance = 0;

VsAuth::VsAuth( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );

	connect(
		HttpRequestWorker::instance(), SIGNAL( loginCheckResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleAuthResult( HttpRequestWorker* ) )
	);
}

VsAuth *VsAuth::createInstance()
{
    return new VsAuth();
}

VsAuth *VsAuth::instance()
{
	if ( ! _instance ) {
		_instance = createInstance();
	}

	return _instance;
}

bool VsAuth::isLoggedIn()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authExpireTime	= oSettings->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "tokenExpired" );

	return QDateTime::currentSecsSinceEpoch() <= authExpireTime.toInt();
}

bool VsAuth::login( QString username, QString password )
{
	QString strUrlLogin	= VsApplication::instance()->apiUrl().append( "/login_check" );
	//qDebug() << strUrlLogin;

	HttpRequestInput input( strUrlLogin, "POST" );
	input.requestType	= REQUEST_TYPE_JSON;

	input.addVar( "username", username );
	input.addVar( "password", password );

	//HttpRequestWorker::instance()->setObjectName( RequestTypes[LOGIN_CHECK] );
	HttpRequestWorker::instance()->execute( input, HttpRequests["LOGIN_REQUEST"] );

	return true;
}

void VsAuth::logout()
{
	VsSettings::instance()->remove( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] );
}

QString VsAuth::userFullName()
{
	if ( ! isLoggedIn() ) {
		return "Not Logged In";
	}

	QString userFullName	= VsSettings::instance()->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash()
													.value( "userFullName" ).toString();
	return userFullName;
}

void VsAuth::handleAuthResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

	QVariant authPayload	= doc["payload"].toVariant();
	//qDebug() << "API Token: " << authPayload.toHash().value( "token" ).toString();

	QVariant refreshToken	= doc["refresh_token"].toVariant();
	//qDebug() << "API Token: " << refreshToken.toString();

	VsSettings::instance()->setValue( SettingsKeys["AUTH_PAYLOAD"], authPayload, SettingsGroups["authentication"] );
	VsSettings::instance()->setValue( SettingsKeys["REFRESH_TOKEN"], refreshToken, SettingsGroups["authentication"] );

	emit loginCheckFinished( worker );
}
