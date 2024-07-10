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
#include <QDesktopServices>
#include <QUrl>

#include "GlobalTypes.h"
#include "Application/VsApplication.h"
#include "Application/VsAuth.h"
#include "Application/VsSettings.h"
#include "Application/WgpMyTablatures.h"
#include "Application/WgpSharedToMeTablatures.h"
#include "Application/WgpFileSystem.h"
#include "ApiManager/HttpRequestWorker.h"

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );

	/**
	 * Directory Tab is To Show Directory With Files Not Category Tree Got From API
	 */
	//ui->tabWidget->removeTab( 1 );

	toolBar	= new QToolBar( ui->widget );
	toolBar->setStyleSheet( "QToolBar{spacing:10px;}" );
	ui->menubarLayout->addWidget( toolBar );

	ui->treeWidget->setColumnCount( 3 );

	connect(
		HttpRequestWorker::instance(), SIGNAL( myCategoriesResponseReady( CommandState* ) ),
		this, SLOT( handleMyCategoriesResult( CommandState* ) )
	);

	connect(
		HttpRequestWorker::instance(), SIGNAL( myTablaturesResponseReady( CommandState* ) ),
		this, SLOT( handleMyTablaturesResult( CommandState* ) )
	);

	connect(
		HttpRequestWorker::instance(), SIGNAL( sharedToMeTablaturesResponseReady( CommandState* ) ),
		this, SLOT( handleSharedToMeTablaturesResult( CommandState* ) )
	);

	if ( VsAuth::instance()->isLoggedIn() ) {
		_createToolBar();
		_displayMyTablatures();
		//_syncFileSystem();
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
	loginDialog	= new UserLoginDialog();
	loginDialog->setModal( true );

	if ( loginDialog->exec() == QDialog::Accepted )
	{
		//qDebug() << "'SystemTrayMenu::loginToWebGuitarPro' UserLoginDialog: ACCEPTED";
		_createToolBar();
		_displayMyTablatures();
		//_syncFileSystem();
	}
}

void SystemTrayMenu::_createToolBar()
{
	QAction *openFolderAct = new QAction( tr("&Open WebGuitarPro Folder" ), this );
	//openFolderAct->setStatusTip( tr( "Open WebGuitarPro Folder" ) );
	connect( openFolderAct, SIGNAL( triggered() ), this, SLOT( openWebGuitarProFolder() ) );

	QToolButton *folderButton	= new QToolButton( toolBar );
	folderButton->setDefaultAction( openFolderAct );
	folderButton->setIcon( QIcon( ":/Resources/icons/folder.png" ) );
	toolBar->addWidget( folderButton  );

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
	profileButton->setIcon( _createProfileIcon() );
	profileButton->setMenu( profileMenu );
	profileButton->setPopupMode( QToolButton::InstantPopup );
	toolBar->addWidget( profileButton  );
}

QIcon SystemTrayMenu::_createProfileIcon()
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

void SystemTrayMenu::_displayMyTablatures()
{

	dirModel	= WgpFileSystem::instance()->model();
	ui->treeView->setModel( dirModel );
	ui->treeView->setRootIndex( dirModel->index( dirModel->rootPath() ) );

	//return;
	WgpMyTablatures::instance()->getMyTablatures();
	WgpSharedToMeTablatures::instance()->getSharedToMeTablatures();
}

void SystemTrayMenu::_setTopLevelItems( QList<QTreeWidgetItem *> items )
{
	//ui->treeWidget->insertTopLevelItems( 0, items );
	ui->treeWidget->addTopLevelItems( items );
}

QTreeWidgetItem *SystemTrayMenu::_createTreeWidgetItems( QJsonObject jc, QTreeWidgetItem *parentItem )
{
	//QTreeWidgetItem *treeItem	= new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );
	QTreeWidgetItem *treeItem	= new QTreeWidgetItem( parentItem );

	// Category Name
	treeItem->setText( 0, jc["name"].toString() );

	QJsonObject children	= jc.value( "children" ).toObject();
	//qDebug() << "'SystemTrayMenu::_createTreeWidgetItems' Categories Size: " << children.size();
	foreach( const QString& key, children.keys() ) {
		QJsonObject child	= children.value( key ).toObject();

		_createTreeWidgetItems( child, treeItem );
	}

	QJsonObject tabs	= jc.value( "tablatures" ).toObject();
	//qDebug() << "'SystemTrayMenu::_createTreeWidgetItems' Tablatures Size: " << tabs.size();
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

void SystemTrayMenu::_syncFileSystem()
{
	WgpFileSystem::instance()->sync();
}

void SystemTrayMenu::handleMyCategoriesResult( CommandState *state )
{
	QJsonDocument doc	= QJsonDocument::fromJson( state->response );
	QJsonObject results	= doc.object();
	//qDebug() << "'SystemTrayMenu::handleMyCategoriesResult' Result Size: " << results.size();

	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *treeItem;

	foreach( const QString& key, results.keys() ) {
		QJsonObject jc	= results.value( key ).toObject();
		if ( jc.contains( "parent" ) )
			continue;

		treeItem	= _createTreeWidgetItems( jc );
		if ( ! treeItem->parent() ) {
			items.append( treeItem );
		}
	}

	_setTopLevelItems( items );
}

void SystemTrayMenu::handleMyTablaturesResult( CommandState *state )
{
	QJsonDocument doc	= QJsonDocument::fromJson( state->response );
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

	_setTopLevelItems( items );
}

void SystemTrayMenu::handleSharedToMeTablaturesResult( CommandState *state )
{
	QJsonDocument doc	= QJsonDocument::fromJson( state->response );
	QJsonObject results	= doc.object();
	//qDebug() << "'SystemTrayMenu::handleSharedToMeTablaturesResult' Result Size: " << results.size();

	QList<QTreeWidgetItem *> items;
	QList<QTreeWidgetItem *> children;
	QTreeWidgetItem *treeItem;

	// Create Root Item
	QTreeWidgetItem *rootItem = new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );
	rootItem->setText( 0, "Shared To Me Tablatures" );
	items.append( rootItem );

	foreach( const QString& key, results.keys() ) {
		QJsonObject jc	= results.value( key ).toObject();

		treeItem = new QTreeWidgetItem( static_cast<QTreeWidget *>(nullptr) );

		// Tablature Name
		treeItem->setText( 0, jc["name"].toString() );
		children.append( treeItem );

		_createSharedToMeTablaturesItems( jc, treeItem );
	}
	rootItem->addChildren( children );

	_setTopLevelItems( items );
}

void SystemTrayMenu::_createSharedToMeTablaturesItems( QJsonObject jc, QTreeWidgetItem *parentItem )
{
	QJsonObject tabs	= jc.value( "tablatures" ).toObject();
	//qDebug() << "'SystemTrayMenu::_createSharedToMeTablaturesItems' Tablatures Size: " << tabs.size();

	foreach( const QString& key, tabs.keys() ) {
		QJsonObject jt	= tabs.value( key ).toObject();
		QTreeWidgetItem *childItem	= new QTreeWidgetItem( parentItem );

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
}

void SystemTrayMenu::openWebGuitarProFolder()
{
	QString WebGuitarProFolder	= QDir::homePath() + "/WebGuitarPro";
	//qDebug() << WebGuitarProFolder;

	QDesktopServices::openUrl( QUrl::fromLocalFile( WebGuitarProFolder ) );
}
