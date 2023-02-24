#include "WgpMyTablatures.h"

#include <QString>
#include <QVariant>
#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"

WgpMyTablatures *WgpMyTablatures::_instance = nullptr;

WgpMyTablatures::WgpMyTablatures() { }

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
		_getMyTablatures();

		return true;
	}

	return false;
}

void WgpMyTablatures::_getMyTablatures()
{
	VsSettings *oSettings	= VsSettings::instance();
	QVariant authToken		= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	QString strUrl	= VsApplication::instance()->apiUrl().append( "my-tablatures" );
	HttpRequestInput input( strUrl, "GET" );
	input.requestType	= REQUEST_TYPE_JSON;

	QMap<QString, QString> headers;
	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	VsApplication::instance()->httpRequestWorker()->execute( &input, "GetMyTablatures", headers );
}
