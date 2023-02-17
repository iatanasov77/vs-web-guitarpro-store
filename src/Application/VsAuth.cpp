#include "VsAuth.h"

#include <QDateTime>
#include <QDebug>
#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsSettings.h"

VsAuth *VsAuth::_instance = 0;

VsAuth::VsAuth() { }

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
	QVariant authExpireTime	= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "tokenExpired" );

	return QDateTime::currentSecsSinceEpoch() <= authExpireTime.toInt();
}

bool VsAuth::login( QString username, QString password )
{
	QString strUrlLogin	= VsApplication::instance()->apiUrl().append( "login_check" );
	//qDebug() << strUrlLogin;

	HttpRequestInput input( strUrlLogin, "POST" );
	input.requestType	= REQUEST_TYPE_JSON;

	input.addVar( "username", username );
	input.addVar( "password", password );

	VsApplication::instance()->httpRequestWorker()->execute( &input );

	return true;
}

bool VsAuth::testRequestWorker()
{
	QString strUrl	= "http://google.com";
	HttpRequestInput input( strUrl, "GET" );

	qDebug() << "Test Request Work GET: " << strUrl;
	qDebug() << "====================================================";
	VsApplication::instance()->httpRequestWorker()->execute( &input );

	return true;
}
