#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QPainter>
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

#include "GlobalTypes.h"
#include "Application/VsSettings.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );
	createToolBar();

	if ( VsAuth::instance()->isLoggedIn() ) {
		displayMyTablatures();
	}
}

SystemTrayMenu::~SystemTrayMenu()
{
    delete ui;
}

void SystemTrayMenu::createToolBar()
{
	QToolBar *toolBar	= new QToolBar( ui->widget );

	QMenu *profileMenu 	= new QMenu( "Profile" );

	profileMenu->addAction( "Sign Out" );

	QAction *quitAct = new QAction( tr("&Quit" ), this );
	quitAct->setStatusTip( tr( "Quit Application" ) );
	connect( quitAct, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection );
	profileMenu->addAction( quitAct );

	QToolButton *profileButton	= new QToolButton( toolBar );
	profileButton->setIcon( createProfileIcon() );
	profileButton->setMenu( profileMenu );
	profileButton->setPopupMode( QToolButton::InstantPopup );
	toolBar->addWidget( profileButton  );

	ui->menubarLayout->addWidget( toolBar );
}

QIcon SystemTrayMenu::createProfileIcon()
{
	QString profileIconText	= QString( "" );
	QString userFullName	= VsAuth::instance()->userFullName();
	auto parts = userFullName.split( ' ' );
	for ( const auto& i : parts  )
	{
	    //qDebug() << i;
		if ( ! i.isEmpty() ) {
			profileIconText.append( i.at( 0 ).toUpper() );
		}
	}

	QImage image( 100, 100, QImage::Format_ARGB32_Premultiplied );
	QPainter painter( &image );
	QFont font	= painter.font();

	font.setPixelSize( 48 );
	painter.setFont( font );
	painter.fillRect( image.rect(), Qt::yellow );
	painter.drawText( image.rect(), Qt::AlignCenter | Qt::AlignVCenter, profileIconText );

	QIcon profileIcon	= QIcon( QPixmap::fromImage( image ) );

	return profileIcon;
}

void SystemTrayMenu::displayMyTablatures()
{
	bool result = WgpMyTablatures::instance()->getMyTablatures();
	if ( result ) {
		// Accept on handleAuthResult
		// accept();
	}

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyCategoriesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyTablaturesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);
}

void SystemTrayMenu::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	ui->treeWidget->setColumnCount( 3 );

	QString errorMsg;
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
		QMessageBox::information( nullptr, "", errorMsg );
	}
}
