#include "WgpFileSystem.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "WgpMyTablatures.h"

WgpFileSystem *WgpFileSystem::_instance = nullptr;

WgpFileSystem::WgpFileSystem( QObject *parent ) : QObject( parent )
{
	Q_UNUSED( parent );

	downloader	= new HttpFileDownloader();

	rootPath	= QDir::homePath() + "/WebGuitarPro";
	if ( ! QDir( rootPath ).exists() ) {
		QDir().mkdir( rootPath );
	}

	createWatcher();

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyCategoriesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyTablaturesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
	);

	connect(
		downloader, SIGNAL( downloaded( HttpRequestWorker* ) ),
		this, SLOT( handleDownloadedTablature( HttpRequestWorker* ) )
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

void WgpFileSystem::createWatcher()
{
	watcher	= new QFileSystemWatcher( { rootPath } ) ;

	connect(
		watcher, SIGNAL( directoryChanged( QString ) ),
		this, SLOT( fileModified( QString ) )
	);
}

void WgpFileSystem::sync()
{
	//WgpMyTablatures::instance()->getMyTablatures();
}

void WgpFileSystem::handleMyCategoriesResult( HttpRequestWorker *worker )
{
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		if ( doc.isArray() ) {
			QJsonArray results	= doc.array();

			for( int i = 0; i < results.size(); i++ ) {
				QJsonObject jc	= results[i].toObject();

				// Category Name
				QJsonObject categoryTaxon	= jc["taxon"].toObject();

				QString categoryPath	= rootPath + "/" + categoryTaxon["name"].toString();
				if ( ! QDir( categoryPath ).exists() ) {
					//qDebug() << "PATH NOT EXISTS: " << categoryPath;
					QDir().mkdir( categoryPath );
				}

				QJsonArray tabs	= jc["tablatures"].toArray();
				for( int j = 0; j < tabs.size(); j++ ) {
					QJsonObject jt	= tabs[j].toObject();

					// Tablature Original File Name
					QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
					QString tablaturePath		= categoryPath + "/" + tablatureFile["originalName"].toString();
					if ( ! QDir( tablaturePath ).exists() ) {
						qDebug() << "PATH NOT EXISTS: " << tablaturePath;

						QString fileUrl	= QString( "http://wgp.lh/tablatures-ext/%1/read" ).arg( jt["id"].toString() );
						downloader->download( fileUrl, tablaturePath );
					}
				}
			}
		}
	}
}

void WgpFileSystem::handleMyTablaturesResult( HttpRequestWorker *worker )
{
	if ( worker->errorType == QNetworkReply::NoError ) {
		// communication was successful
		QJsonDocument doc	= QJsonDocument::fromJson( worker->response );

		if ( doc.isArray() ) {
			QJsonArray results	= doc.array();

			for( int i = 0; i < results.size(); i++ ) {
			    QJsonObject jt	= results[i].toObject();

			}
		}
	}
}

void WgpFileSystem::fileModified( QString path )
{
	qDebug() << "Directory Modified: " << path;
}
