#include "WgpFileSystem.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "VsApplication.h"
#include "VsSettings.h"
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

	// TEST
	/*
	downloader->download( "http://wgp.lh/api/download/4-SoundOfHeaven.gp3", rootPath + "/SoundOfHeaven.gp3", authHeaders() );
	downloader->download( "http://wgp.lh/api/download/9-vital_remains_dechristianize.gp4", rootPath + "/vital_remains_dechristianize.gp4", authHeaders() );
	*/

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyCategoriesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyCategoriesResult( HttpRequestWorker* ) )
	);

	connect(
		WgpMyTablatures::instance(), SIGNAL( getMyTablaturesFinished( HttpRequestWorker* ) ),
		this, SLOT( handleMyTablaturesResult( HttpRequestWorker* ) )
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

			for( int i = 0; i < results.size(); i++ ) {
			    QJsonObject jt	= results[i].toObject();

			    // Tablature Original File Name
				QJsonObject tablatureFile	= jt["tablatureFile"].toObject();
				QString tablaturePath		= rootPath + "/" + tablatureFile["originalName"].toString();

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

void WgpFileSystem::handleDownloadedTablature( QString targetPath )
{
	Q_UNUSED( targetPath );
}

void WgpFileSystem::fileModified( QString path )
{
	qDebug() << "Directory Modified: " << path;
}

QMap<QString, QString> WgpFileSystem::authHeaders()
{
	QMap<QString, QString> headers;
	QVariant authToken	= VsSettings::instance()->value( "authPayload", SettingsGroups["authentication"] ).toHash().value( "token" );

	headers.insert( "Authorization", QString( "Bearer " ).append( authToken.toString() ) );

	return headers;
}
