#include "WgpSharedToMeTablatures.h"

#include <QDebug>

#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"

WgpSharedToMeTablatures *WgpSharedToMeTablatures::_instance = nullptr;

WgpSharedToMeTablatures::WgpSharedToMeTablatures( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );
}

WgpSharedToMeTablatures *WgpSharedToMeTablatures::createInstance()
{
    return new WgpSharedToMeTablatures();
}

WgpSharedToMeTablatures *WgpSharedToMeTablatures::instance()
{
	if ( _instance == nullptr ) {
		_instance = createInstance();
	}

	return _instance;
}

bool WgpSharedToMeTablatures::getSharedToMeTablatures()
{
	if ( VsAuth::instance()->isLoggedIn() ) {
		//qDebug() << "WgpSharedToMeTablatures::getSharedToMeTablatures() Running ...";

		QString strUrl			= VsApplication::instance()->apiUrl().append( "/shared-to-me" );
		HttpRequestInput *input	= new HttpRequestInput( strUrl, "GET" );
		input->requestType		= REQUEST_TYPE_JSON;
		QMap<QString, QString> headers;

		HttpRequestWorker::instance()->execute( input, HttpRequests["GET_SHAREDTOMETABLATURES_REQUEST"], headers, true );

		return true;
	}

	return false;
}
