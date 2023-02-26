#include "UserLoginDialog.h"
#include "ui_UserLoginDialog.h"

#include <QNetworkReply>
#include <QMessageBox>
#include <QPushButton>
#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"

UserLoginDialog::UserLoginDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::UserLoginDialog )
{
    ui->setupUi( this );

    waitingSpinner	= new WgpWaitingSpinner( this );

    QPushButton *saveButton = ui->buttonBox->button( QDialogButtonBox::Save );
    saveButton->setText( tr( "Login" ) );

    // May be there is other way to translate Core Strings
    QPushButton *cancelButton = ui->buttonBox->button( QDialogButtonBox::Cancel );
    cancelButton->setText( tr( "Cancel" ) );

    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    connect(
		VsAuth::instance(), SIGNAL( loginCheckFinished( HttpRequestWorker* ) ),
		this, SLOT( handleAuthResult( HttpRequestWorker* ) )
	);
}

UserLoginDialog::~UserLoginDialog()
{
    delete ui;
}

void UserLoginDialog::save()
{
	waitingSpinner->start();
	setDisabled( true );

	QString username	= ui->leUsername->text();
	QString password	= ui->lePassword->text();

	bool result = VsAuth::instance()->login( username, password );
	if ( result ) {
		// Accept on handleAuthResult
		// accept();
	}
}

void UserLoginDialog::handleAuthResult( HttpRequestWorker *worker )
{
	waitingSpinner->stop();
	setEnabled( true );

    if ( worker->errorType == QNetworkReply::NoError ) {
    	accept();
    } else {
    	QString errorMsg	= "Error: " + worker->errorStr;
        QMessageBox::information( this, "", errorMsg );
    }
}
