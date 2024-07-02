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
		WgpMyTablatures::instance(), SIGNAL( getMyCategoriesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyTablaturesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( serverLoadFinished() ),
		this, SLOT( serverLoadFinished() )
	);

	connect(
		downloader, SIGNAL( downloaded( QString ) ),
		this, SLOT( handleDownloadedTablature( QString ) )
	);
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
	    qDebug() << categoryPath;

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
	// Category Name
	QJsonObject categoryTaxon	= jc["taxon"].toObject();
	qDebug() << "Create Category: " << categoryTaxon["name"];

	QString categoryPath	= path + "/" + categoryTaxon["name"].toString();
	if ( ! QDir( categoryPath ).exists() ) {
		//qDebug() << "PATH NOT EXISTS: " << categoryPath;

		model->mkdir( model->index( path ), categoryTaxon["name"].toString() );
		watcher->addPath( categoryPath );
	}

	QJsonArray children	= jc["children"].toArray();
	for( int i = 0; i < children.size(); i++ ) {
		QJsonObject child	= children[i].toObject();

		_createCategories( child, categoryPath );
	}

	QJsonArray tabs	= jc["tablatures"].toArray();
	for( int j = 0; j < tabs.size(); j++ ) {
		QJsonObject jt	= tabs[j].toObject();

		// Tablature Original File Name
		QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
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
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );
		meta->clearMeta();

		QJsonArray results	= doc.array();
		meta->appendToServerMeta( results );

		for( int i = 0; i < results.size(); i++ ) {
			QJsonObject jc	= results[i].toObject();
			if ( jc.contains( "parent" ) )
				continue;

			_createCategories( jc, model->rootPath() );
		}
	}
}

/**
 * Triggered By: WgpMyTablatures::_getMyTablaturesUncategorized()
 */
void WgpFileSystem::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		if ( doc.isArray() ) {
			QJsonArray results	= doc.array();
			meta->appendToServerMeta( results );

			for( int i = 0; i < results.size(); i++ ) {
			    QJsonObject jt	= results[i].toObject();

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
	qDebug() << "Path Modified: " << path;
}

QMap<QString, QString> WgpFileSystem::authHeaders()
{
	QMap<QString, QString> headers;
	QVariant authToken	= VsSettings::instance()->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	return headers;
}
