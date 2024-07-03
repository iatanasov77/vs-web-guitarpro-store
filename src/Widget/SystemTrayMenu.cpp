#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
#include <QMenu>
#include <QToolButton>
#include <QPainter>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtGui/QIcon>
#include <QDialog>

#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"
#include "Application/WgpMyTablatures.h"
#include "Application/WgpFileSystem.h"

#include "Dialog/UserLoginDialog.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );

	toolBar	= new QToolBar( ui->widget );
	ui->menubarLayout->addWidget( toolBar );

	ui->treeWidget->setColumnCount( 3 );

	connect(
		HttpRequestWorker::instance(), SIGNAL( myCategoriesResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		HttpRequestWorker::instance(), SIGNAL( myTablaturesResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	if ( VsAuth::instance()->isLoggedIn() ) {
		createToolBar();
		displayMyTablatures();
		syncFileSystem();
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
		syncFileSystem();
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
	font.setBold( true );
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
	dirModel	= new WgpFileSystemModel();
	ui->treeView->setModel( dirModel );
	ui->treeView->setRootIndex( dirModel->index( dirModel->rootPath() ) );

	WgpMyTablatures::instance()->getMyTablatures();
}

void SystemTrayMenu::setTopLevelItems( QList<QTreeWidgetItem *> items )
{
	//ui->treeWidget->insertTopLevelItems( 0, items );
	ui->treeWidget->addTopLevelItems( items );
}

QTreeWidgetItem *SystemTrayMenu::createTreeWidgetItems( QJsonObject jc, QTreeWidgetItem *parentItem )
{
	//QTreeWidgetItem *treeItem	= new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );
	QTreeWidgetItem *treeItem	= new QTreeWidgetItem( parentItem );

	// Category Name
	treeItem->setText( 0, jc["name"].toString() );

	QJsonObject children	= jc.value( "children" ).toObject();
	//qDebug() << "'SystemTrayMenu::createTreeWidgetItems' Categories Size: " << children.size();
	foreach( const QString& key, children.keys() ) {
		QJsonObject child	= children.value( key ).toObject();

		createTreeWidgetItems( child, treeItem );
	}

	QJsonObject tabs	= jc.value( "tablatures" ).toObject();
	//qDebug() << "'SystemTrayMenu::createTreeWidgetItems' Tablatures Size: " << tabs.size();
	foreach( const QString& key, tabs.keys() ) {
		QJsonObject jt	= tabs.value( key ).toObject();
		QTreeWidgetItem *childItem	= new QTreeWidgetItem( treeItem );

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

	return treeItem;
}

void SystemTrayMenu::logout()
{
	VsAuth::instance()->logout();
	toolBar->clear();
	ui->treeWidget->clear();
	loginToWebGuitarPro();
}

void SystemTrayMenu::syncFileSystem()
{
	WgpFileSystem::instance()->sync();
}

void SystemTrayMenu::handleMyCategoriesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject results	= doc.object();
	//qDebug() << "'SystemTrayMenu::handleMyCategoriesResult' Result Size: " << results.size();

	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *treeItem;

	foreach( const QString& key, results.keys() ) {
		QJsonObject jc	= results.value( key ).toObject();
		if ( jc.contains( "parent" ) )
			continue;

		treeItem	= createTreeWidgetItems( jc );
		if ( ! treeItem->parent() ) {
			items.append( treeItem );
		}
	}

	setTopLevelItems( items );
}

void SystemTrayMenu::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject results	= doc.object();
	//qDebug() << "'SystemTrayMenu::handleMyTablaturesResult' Result Size: " << results.size();

	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *treeItem;

	foreach( const QString& key, results.keys() ) {
		QJsonObject jt	= results.value( key ).toObject();

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

	setTopLevelItems( items );
}
