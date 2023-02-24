#include "UserLoginDialog.h"
#include "ui_UserLoginDialog.h"

#include <QNetworkReply>
#include <QMessageBox>
#include <QPushButton>
#include <QJsonDocument>
#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"

UserLoginDialog::UserLoginDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::UserLoginDialog )
{
    ui->setupUi( this );

    QPushButton *saveButton = ui->buttonBox->button( QDialogButtonBox::Save );
    saveButton->setText( tr( "Login" ) );

    // May be there is other way to translate Core Strings
    QPushButton *cancelButton = ui->buttonBox->button( QDialogButtonBox::Cancel );
    cancelButton->setText( tr( "Cancel" ) );

    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    connect(
		VsApplication::instance()->httpRequestWorker(), SIGNAL( workerFinished( HttpRequestWorker* ) ),
		this, SLOT( handleAuthResult( HttpRequestWorker* ) )
	);
}

UserLoginDialog::~UserLoginDialog()
{
    delete ui;
}

void UserLoginDialog::save()
{
	QString username	= ui->leUsername->text();
	QString password	= ui->lePassword->text();

	VsApplication::instance()->createWaitingSpinner( this );
	bool result = VsAuth::instance()->login( username, password );
	if ( result ) {
		// Accept on handleAuthResult
		// accept();
	}
}

void UserLoginDialog::handleAuthResult( HttpRequestWorker *worker )
{
	if ( worker->requestName != "LoginCheck" )
		return;

	// don't use it
	//VsApplication::instance()->destroyWaitingSpinner();

	QString errorMsg;
	VsSettings *oSettings	= VsSettings::instance();
    if ( worker->errorType == QNetworkReply::NoError ) {
        // communication was successful
    	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

    	QVariant authPayload	= doc["payload"].toVariant();
    	//qDebug() << "API Token: " << authPayload.toHash().value( "token" ).toString();

    	QVariant refreshToken	= doc["refresh_token"].toVariant();
    	//qDebug() << "API Token: " << refreshToken.toString();

    	oSettings->setValue( "authPayload", authPayload, SettingsGroups["authentication"] );
    	oSettings->setValue( "refreshToken", refreshToken, SettingsGroups["authentication"] );

    	accept();
    }
    else {
        // an error occurred
    	errorMsg	= "Error: " + worker->errorStr;
        QMessageBox::information( this, "", errorMsg );
    }
}
