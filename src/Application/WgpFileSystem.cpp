#include "WgpFileSystem.h"

#include <QDir>
#include <QDirIterator>
#include <QFileIconProvider>
#include <QJsonDocument>
#include <QJsonArray>

#include "VsApplication.h"
#include "VsSettings.h"
#include "WgpMyTablatures.h"

WgpFileSystem *WgpFileSystem::_instance = nullptr;

WgpFileSystem::WgpFileSystem( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );

	downloader	= new HttpFileDownloader();

	createModel();

	connect(
		HttpRequestWorker::instance(), SIGNAL( myCategoriesResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		HttpRequestWorker::instance(), SIGNAL( myTablaturesResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	connect(
		downloader, SIGNAL( downloaded( QString ) ),
		this, SLOT( handleDownloadedTablature( QString ) )
	);

	/*
	connect(
		WgpMyTablatures::instance(), SIGNAL( serverLoadFinished() ),
		this, SLOT( serverLoadFinished() )
	);
	*/
}

WgpFileSystem *WgpFileSystem::createInstance()
{
    return new WgpFileSystem();
}

WgpFileSystem *WgpFileSystem::instance()
{
	if ( _instance == nullptr ) {
		_instance = createInstance();
	}

	return _instance;
}

void WgpFileSystem::createModel()
{
	model			= new WgpFileSystemModel;
	iconProvider	= new WgpFileIconProvider();
	watcher			= new QFileSystemWatcher( { model->rootPath() } ) ;
	meta			= new WgpFileSystemMeta( model );

	model->setIconProvider( iconProvider );
	initWatcher();

	connect(
		watcher, SIGNAL( directoryChanged( QString ) ),
		this, SLOT( fileModified( QString ) )
	);

	connect(
		watcher, SIGNAL(  fileChanged( QString ) ),
		this, SLOT( fileModified( QString ) )
	);
}

/**
 * https://doc.qt.io/qt-6/qdiriterator.html
 */
void WgpFileSystem::initWatcher()
{
	QDirIterator it( model->rootPath(), QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
	    QString categoryPath = it.next();
	    qDebug() << "'WgpFileSystem::initWatcher' Category Path: " << categoryPath;

	    watcher->addPath( categoryPath );
	}
}

void WgpFileSystem::sync()
{
	WgpMyTablatures::instance()->getMyTablatures();
}

/**
 * QJsonObject jc - Parent Category
 */
void WgpFileSystem::_createCategories( QJsonObject jc, QString path )
{
	//qDebug() << "Create Category: " << jc["name"];

	QString categoryPath	= path + "/" + jc["name"].toString();
	if ( ! QDir( categoryPath ).exists() ) {
		//qDebug() << "PATH NOT EXISTS: " << categoryPath;

		model->mkdir( model->index( path ), jc["name"].toString() );
		watcher->addPath( categoryPath );
	}

	QJsonObject children	= jc.value( "children" ).toObject();
	foreach( const QString& key, children.keys() ) {
		QJsonObject child	= children.value( key ).toObject();

		_createCategories( child, categoryPath );
	}

	QJsonObject tabs	= jc.value( "tablatures" ).toObject();
	foreach( const QString& key, tabs.keys() ) {
		QJsonObject jt	= tabs.value( key ).toObject();

		// Tablature Original File Name
		QJsonObject tablatureFile	= jt.value( "tablatureFile" ).toObject();
		QString tablaturePath		= categoryPath + "/" + tablatureFile["originalName"].toString();

		if ( ! QFile::exists( tablaturePath ) ) {
			//qDebug() << "FILE NOT EXISTS: " << tablaturePath;

			QString fileUrl	= QString( "%1/download/%2-%3" )
								.arg( VsApplication::instance()->apiUrl() )
								.arg( jt["id"].toInt() )
								.arg( tablatureFile["originalName"].toString() );
			//qDebug() << "TAB URL: " << fileUrl;
			downloader->download( fileUrl, tablaturePath, authHeaders() );
		}
	}
}

void WgpFileSystem::handleMyCategoriesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	meta->clearMeta();

	QJsonObject results	= doc.object();
	//qDebug() << "'WgpFileSystem::handleMyCategoriesResult' Result Size: " << results.size();
	//return;
	meta->appendToServerMeta( results );

	foreach( const QString& key, results.keys() ) {
		QJsonObject jc	= results.value( key ).toObject();

		if ( jc.contains( "parent" ) )
			continue;

		_createCategories( jc, model->rootPath() );
	}
}

void WgpFileSystem::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

	QJsonObject results	= doc.object();
	//qDebug() << "'WgpFileSystem::handleMyTablaturesResult' Result Size: " << results.size();
	//return;
	meta->appendToServerMeta( results );

	foreach( const QString& key, results.keys() ) {
		QJsonObject jt	= results.value( key ).toObject();

		// Tablature Original File Name
		QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
		QString tablaturePath		= model->rootPath() + "/" + tablatureFile["originalName"].toString();

		if ( ! QFile::exists( tablaturePath ) ) {
			//qDebug() << "FILE NOT EXISTS: " << tablaturePath;

			QString fileUrl	= QString( "%1/download/%2-%3" )
								.arg( VsApplication::instance()->apiUrl() )
								.arg( jt["id"].toInt() )
								.arg( tablatureFile["originalName"].toString() );
			//qDebug() << "TAB URL: " << fileUrl;
			downloader->download( fileUrl, tablaturePath, authHeaders() );
		}
	}
}

void WgpFileSystem::serverLoadFinished()
{
	qDebug() << "META DIFFERENCES \n=============================\n";

	QStringList list	= meta->compareMeta();
	for ( const auto& i : list  )
	{
	    qDebug() << i;
	}

}

void WgpFileSystem::handleDownloadedTablature( QString targetPath )
{
	watcher->addPath( targetPath );
}

void WgpFileSystem::fileModified( QString path )
{
	qDebug() << "'WgpFileSystem::fileModified' Path Modified: " << path;
}

QMap<QString, QString> WgpFileSystem::authHeaders()
{
	QMap<QString, QString> headers;
	QVariant authToken	= VsSettings::instance()->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "token" );

	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	return headers;
}
