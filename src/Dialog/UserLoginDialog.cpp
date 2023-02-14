#include "UserLoginDialog.h"
#include "ui_UserLoginDialog.h"

#include <QNetworkReply>
#include <QMessageBox>
#include <QPushButton>
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"

UserLoginDialog::UserLoginDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::UserLoginDialog )
{
    ui->setupUi( this );

    QPushButton *saveButton = ui->buttonBox->button( QDialogButtonBox::Save );
    saveButton->setText( tr( "Login" ) );

    // May be there is other way to translate Core Strings
    ui->buttonBox->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );

    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
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

	bool result = VsAuth::instance()->login( username, password );
}

void UserLoginDialog::handleAuthResult( HttpRequestWorker *worker )
{
    QString msg;

    if ( worker->errorType == QNetworkReply::NoError ) {
        // communication was successful
        msg = "Success - Response: " + worker->response;
    }
    else {
        // an error occurred
        msg = "Error: " + worker->errorStr;
        QMessageBox::information( this, "", msg );
    }
}
