#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
#include <QToolButton>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMessageBox>
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/WgpMyTablatures.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );

	if ( ! VsAuth::instance()->isLoggedIn() ) {
		displayMyTablatures();
	}

	connect(
		VsApplication::instance()->httpRequestWorker(), SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	connect(
		ui->btnQuitApplication, &QToolButton::clicked,
		QCoreApplication::instance(), &QCoreApplication::quit,
		Qt::QueuedConnection
	);
}

SystemTrayMenu::~SystemTrayMenu()
{
    delete ui;
}

void SystemTrayMenu::displayMyTablatures()
{
	VsApplication::instance()->createWaitingSpinner( this );
	bool result = WgpMyTablatures::instance()->getMyTablatures();
	if ( result ) {
		// Accept on handleAuthResult
		// accept();
	}
}

void SystemTrayMenu::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	QString errorMsg;

	VsApplication::instance()->destroyWaitingSpinner();
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		QVariant authPayload	= doc["payload"].toVariant();
		//qDebug() << "API Token: " << authPayload.toHash().value( "token" ).toString();

		QVariant refreshToken	= doc["refresh_token"].toVariant();
		//qDebug() << "API Token: " << refreshToken.toString();

	}
	else {
		// an error occurred
		errorMsg	= "Error: " + worker->errorStr;
		QMessageBox::information( this, "", errorMsg );
	}
}
