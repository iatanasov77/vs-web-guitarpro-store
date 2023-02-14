#include "VsAuth.h"

#include "GlobalTypes.h"
#include "VsApplication.h"
#include "VsSettings.h"
#include "ApiManager/HttpRequestWorker.h"

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
	// VsSettings::instance()->value( "language", "General" ).toString();
	return false;
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
