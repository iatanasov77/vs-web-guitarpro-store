#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
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
#include <QDialog>

#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/WgpMyTablatures.h"
#include "Application/VsSettings.h"

#include "Dialog/UserLoginDialog.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );

	toolBar	= new QToolBar( ui->widget );
	ui->menubarLayout->addWidget( toolBar );

	ui->treeWidget->setColumnCount( 3 );

	if ( VsAuth::instance()->isLoggedIn() ) {
		createToolBar();
		displayMyTablatures();
	} else {
		loginToWebGuitarPro();
	}
}

SystemTrayMenu::~SystemTrayMenu()
{
    delete ui;
}

void SystemTrayMenu::loginToWebGuitarPro()
{
	UserLoginDialog *dlg	= new UserLoginDialog();
	dlg->setModal( true );

	if ( dlg->exec() == QDialog::Accepted )
	{
		createToolBar();
		displayMyTablatures();
	}
}

void SystemTrayMenu::createToolBar()
{
	QMenu *profileMenu 	= new QMenu( "Profile" );

	QAction *logoutAct = new QAction( tr("&Sign Out" ), this );
	logoutAct->setStatusTip( tr( "Sign Out From API" ) );
	connect( logoutAct, SIGNAL( triggered() ), this, SLOT( logout() ) );
	profileMenu->addAction( logoutAct );

	QAction *quitAct = new QAction( tr("&Quit" ), this );
	quitAct->setStatusTip( tr( "Quit Application" ) );
	connect( quitAct, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection );
	profileMenu->addAction( quitAct );

	QToolButton *profileButton	= new QToolButton( toolBar );
	profileButton->setIcon( createProfileIcon() );
	profileButton->setMenu( profileMenu );
	profileButton->setPopupMode( QToolButton::InstantPopup );
	toolBar->addWidget( profileButton  );
}

QIcon SystemTrayMenu::createProfileIcon()
{
	QString profileIconText	= QString( "" );
	QString userFullName	= VsAuth::instance()->userFullName();
	auto parts = userFullName.split( ' ' );
	for ( const auto& i : parts  )
	{
		if ( ! i.isEmpty() ) {
			profileIconText.append( i.at( 0 ).toUpper() );
		}
	}

	QImage image( 100, 100, QImage::Format_ARGB32_Premultiplied );
	QPainter painter( &image );
	QFont font	= painter.font();

	font.setPixelSize( 58 );
	painter.setFont( font );

	painter.setBrush( Qt::red );
	painter.drawEllipse( image.rect() );

	painter.setPen( Qt::white );
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
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyTablaturesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);
}

void SystemTrayMenu::handleMyCategoriesResult( HttpRequestWorker *worker )
{
	QString errorMsg;
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		if ( doc.isArray() ) {
			QJsonArray results	= doc.array();
			QList<QTreeWidgetItem *> items;
			QTreeWidgetItem *treeItem;
			QTreeWidgetItem *childItem;

			for( int i = 0; i < results.size(); i++ ) {
				QJsonObject jc	= results[i].toObject();
				treeItem = new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );

				// Category Name
				QJsonObject categoryTaxon	= jc["taxon"].toObject();
				treeItem->setText( 0, categoryTaxon["name"].toString() );

				QJsonArray tabs	= jc["tablatures"].toArray();
				for( int j = 0; j < tabs.size(); j++ ) {
					QJsonObject jt	= tabs[j].toObject();
					childItem = new QTreeWidgetItem( treeItem );

					// Tablature Name
					childItem->setText( 0, jt["artist"].toString() + " - " + jt["song"].toString() );

					// Tablature Original File Name
					QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
					childItem->setText( 1, tablatureFile["originalName"].toString() );

					// Tablature Is Public
					QPixmap oPixmap( 32,32 );
					if ( jt["enabled"] == true ) {
						oPixmap.load ( ":/Resources/icons/Symbol_OK.svg" );
					} else {
						oPixmap.load ( ":/Resources/icons/Symbol_NO.svg" );
					}
					QIcon oIcon( oPixmap );
					childItem->setIcon( 2, oIcon );
				}

				items.append( treeItem );
			}
			//ui->treeWidget->insertTopLevelItems( 0, items );
			ui->treeWidget->addTopLevelItems( items );
		}
	}
	else {
		// an error occurred
		errorMsg	= "Error: " + worker->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void SystemTrayMenu::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	QString errorMsg;
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		if ( doc.isArray() ) {
			QJsonArray results	= doc.array();
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
			//ui->treeWidget->insertTopLevelItems( 0, items );
			ui->treeWidget->addTopLevelItems( items );
		}
	}
	else {
		// an error occurred
		errorMsg	= "Error: " + worker->errorStr;
		QMessageBox::information( nullptr, "", errorMsg );
	}
}

void SystemTrayMenu::logout()
{
	VsAuth::instance()->logout();
	toolBar->clear();
	ui->treeWidget->clear();
	loginToWebGuitarPro();
}
