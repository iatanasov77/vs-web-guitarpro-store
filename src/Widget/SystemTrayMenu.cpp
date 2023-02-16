#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
#include <QToolButton>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QtGui/QIcon>
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/WgpMyTablatures.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );
	ui->treeWidget->setColumnCount( 3 );

	if ( VsAuth::instance()->isLoggedIn() ) {
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

		if ( doc.isArray() ) {
			QJsonArray results				= doc.array();
			QList<QTreeWidgetItem *> items;
			QTreeWidgetItem *treeItem;

			for( int i = 0; i < results.size(); i++ ) {
			    QJsonObject jt	= results[i].toObject();
			    treeItem = new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );

				// Tablature Name
				treeItem->setText( 0, jt["artist"].toString() + " - " + jt["song"].toString() );

				// Tablature Original File Name
				QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
				treeItem->setText( 1, tablatureFile["originalName"].toString() );

				// Tablature Is Public
				QPixmap oPixmap( 32,32 );
				if ( jt["enabled"] == true ) {
					oPixmap.load ( ":/Resources/icons/Symbol_OK.svg" );
				} else {
					oPixmap.load ( ":/Resources/icons/Symbol_NO.svg" );
				}
				QIcon oIcon( oPixmap );
				treeItem->setIcon( 2, oIcon );

				items.append( treeItem );
			}
			ui->treeWidget->insertTopLevelItems( 0, items );
		}
	}
	else {
		// an error occurred
		errorMsg	= "Error: " + worker->errorStr;
		QMessageBox::information( this, "", errorMsg );
	}
}
