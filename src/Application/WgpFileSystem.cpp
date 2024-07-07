#include "WgpFileSystem.h"

#include <QDir>
#include <QDirIterator>
#include <QFileIconProvider>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMimeDatabase>

#include "VsApplication.h"
#include "VsSettings.h"
#include "WgpMyTablatures.h"

WgpFileSystem *WgpFileSystem::_instance = nullptr;

WgpFileSystem::WgpFileSystem( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );

	allowedMimeTypes = QStringList{ "application/gpx+xml", "application/octet-stream" };
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
		HttpRequestWorker::instance(), SIGNAL( myCategoryUpdateResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleUpdateCategoryResult( HttpRequestWorker* ) )
	);

	connect(
		HttpRequestWorker::instance(), SIGNAL( myTablatureUploadResponseReady( HttpRequestWorker* ) ),
		this, SLOT( handleUploadTablatureResult( HttpRequestWorker* ) )
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
	//qDebug() << "Create FileSystem Model";
	_model			= new WgpFileSystemModel;
	iconProvider	= new WgpFileIconProvider();
	watcher			= new QFileSystemWatcher( { _model->rootPath() } ) ;
	meta			= new WgpFileSystemMeta( _model );

	_model->setIconProvider( iconProvider );
	initWatcher();

	connect(
		_model, SIGNAL( fileRenamed( const QString&, const QString&, const QString& ) ),
		this, SLOT( fileRenamed( QString, QString, QString ) )
	);

	connect(
		watcher, SIGNAL( directoryChanged( QString ) ),
		this, SLOT( directoryModified( QString ) )
	);

	connect(
		watcher, SIGNAL( fileChanged( QString ) ),
		this, SLOT( fileModified( QString ) )
	);
}

/**
 * https://doc.qt.io/qt-6/qdiriterator.html
 */
void WgpFileSystem::initWatcher()
{
	QDirIterator it( _model->rootPath(), QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
	    QString categoryPath = it.next();
	    //qDebug() << "'WgpFileSystem::initWatcher' Category Path: " << categoryPath;

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
	QString categoryPath	= QString( "%1/%2" ).arg( path, jc["name"].toString() );
	if ( ! QDir( categoryPath ).exists() ) {
		//qDebug() << "PATH NOT EXISTS: " << categoryPath;
		createCategory( jc["name"].toString(), path );
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
			downloadTablature( jt["id"].toInt(), tablatureFile["originalName"].toString(), tablaturePath );
		}
	}
}

void WgpFileSystem::handleUpdateCategoryResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject result	= doc.object();
	meta->appendToServerObjects( result );
	meta->appendToLocalObjects( result );
}

void WgpFileSystem::handleUploadTablatureResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject result	= doc.object();
	meta->appendToServerObjects( result );
	meta->appendToLocalObjects( result );
}

void WgpFileSystem::handleMyCategoriesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject results	= doc.object();
	meta->refreshServerObjects( results );

	foreach( const QString& key, results.keys() ) {
		QJsonObject jc	= results.value( key ).toObject();

		if ( jc.contains( "parent" ) )
			continue;

		_createCategories( jc, _model->rootPath() );
	}
	//serverLoadFinished();
}

void WgpFileSystem::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
	QJsonObject results	= doc.object();
	meta->refreshServerObjects( results );

	foreach( const QString& key, results.keys() ) {
		QJsonObject jt	= results.value( key ).toObject();

		// Tablature Original File Name
		QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
		QString tablaturePath		= _model->rootPath() + "/" + tablatureFile["originalName"].toString();

		if ( ! QFile::exists( tablaturePath ) ) {
			//qDebug() << "FILE NOT EXISTS: " << tablaturePath;
			downloadTablature( jt["id"].toInt(), tablatureFile["originalName"].toString(), tablaturePath );
		}
	}
	//serverLoadFinished();
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

void WgpFileSystem::metaDifferences()
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

void WgpFileSystem::fileRenamed( QString path, QString oldName, QString newName )
{
	qDebug() << "'WgpFileSystem::fileRenamed' Path Renamed: " << path;
	qDebug() << "'WgpFileSystem::fileRenamed' Old Name: " << oldName;
	qDebug() << "'WgpFileSystem::fileRenamed' New Name: " << newName;
}

void WgpFileSystem::fileModified( QString path )
{
	qDebug() << "'WgpFileSystem::fileModified' Path Modified: " << path;
	metaDifferences();
	QFileInfo fi( path );

	QDateTime lastModified	= fi.lastModified();
	QString mimeType		= QMimeDatabase().mimeTypeForFile( path ).name();
	//qDebug() << "Mime type:" << mimeType;
	if ( allowedMimeTypes.contains( mimeType ) ) {
		//WgpMyTablatures::instance()->createTablature( path );
		//WgpMyTablatures::instance()->updateTablature( 1, path );
	}
}

void WgpFileSystem::directoryModified( QString path )
{
	qDebug() << "'WgpFileSystem::directoryModified' Path Modified: " << path;
	QString categoryPath;
	metaDifferences();
	QFileInfo fi( path );

	QDateTime lastModified		= fi.lastModified();
	QStringList newCategories	= meta->findNewCategories( _model->rootPath() );
	for ( int i = 0; i < newCategories.size(); ++i ) {
		//qDebug() << "'WgpFileSystem::directoryModified' Creating New Category ...";

		//WgpMyTablatures::instance()->createTablatureCategory( newCategories[i] );
		//WgpMyTablatures::instance()->updateTablatureCategory( 1, newCategories[i] );

		categoryPath	= QString( "%1/%2" ).arg( path, newCategories[i] );
		watcher->addPath( categoryPath );
	}
}

QMap<QString, QString> WgpFileSystem::authHeaders()
{
	QMap<QString, QString> headers;
	QVariant authToken	= VsSettings::instance()->value( SettingsKeys["AUTH_PAYLOAD"], SettingsGroups["authentication"] ).toHash().value( "token" );

	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	return headers;
}

WgpFileSystemModel *WgpFileSystem::model()
{
	return _model;
}

void WgpFileSystem::createCategory( QString name, QString path )
{
	QString categoryPath	= QString( "%1/%2" ).arg( path, name );

	_model->mkdir( _model->index( path ), name );
	watcher->addPath( categoryPath );
}

void WgpFileSystem::downloadTablature( int tabId, QString originalName, QString tablaturePath )
{
	QString fileUrl	= QString( "%1/download/%2-%3" )
						.arg( VsApplication::instance()->apiUrl() )
						.arg( tabId )
						.arg( originalName );

	downloader->download( fileUrl, tablaturePath, authHeaders() );
}
