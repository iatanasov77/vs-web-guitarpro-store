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

	m_httpRequestWorker = new HttpRequestWorker();

	connect(
		m_httpRequestWorker, SIGNAL( workerFinished( HttpRequestWorker* ) ),
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
	QVariant authExpireTime	= oSettings->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "tokenExpired" );

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

	m_httpRequestWorker->setObjectName( RequestTypes[LOGIN_CHECK] );
	m_httpRequestWorker->execute( &input );

	return true;
}

void VsAuth::handleAuthResult( HttpRequestWorker *worker )
{
    if ( worker->objectName() == RequestTypes[LOGIN_CHECK] ) {
    	handleLoginCheck( worker );
    } else {
    	qDebug() << "UNDEFINED AUTH REQUEST !!!";
    }
}

void VsAuth::handleLoginCheck( HttpRequestWorker *worker )
{
    if ( worker->errorType == QNetworkReply::NoError ) {
        // communication was successful
    	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

    	QVariant authPayload	= doc["payload"].toVariant();
    	//qDebug() << "API Token: " << authPayload.toHash().value( "token" ).toString();

    	QVariant refreshToken	= doc["refresh_token"].toVariant();
    	//qDebug() << "API Token: " << refreshToken.toString();

    	VsSettings::instance()->setValue( "authPayload", authPayload, SettingsGroups["authentication"] );
    	VsSettings::instance()->setValue( "refreshToken", refreshToken, SettingsGroups["authentication"] );
    }

    emit loginCheckFinished( worker );
}
