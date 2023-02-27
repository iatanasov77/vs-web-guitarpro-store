#include "HttpFileDownloader.h"

HttpFileDownloader::HttpFileDownloader( QObject *parent ) : QObject( parent )
{
	connect(
		&m_WebCtrl, SIGNAL ( finished( QNetworkReply* ) ),
		this, SLOT ( fileDownloaded( QNetworkReply* ) )
	);
}

HttpFileDownloader::~HttpFileDownloader() { }

void HttpFileDownloader::fileDownloaded( QNetworkReply* pReply )
{
	for( auto key : downloadFiles.keys() ) {
		if ( downloadFiles.value( key ) == pReply->request().url().toString() ) {
			m_DownloadedData[key]	= pReply->readAll();
			writeFile( key, m_DownloadedData[key] );

			//emit a signal
			pReply->deleteLater();
			emit downloaded( key );

			//qDebug() << "FILE DOWNLOADED: " << key << "   (" << m_DownloadedData[key].length() << " BITES)";
		}
	}
}

QByteArray HttpFileDownloader::downloadedData( QString target ) const
{
	return m_DownloadedData[target];
}

void HttpFileDownloader::download( QString url, QString targetPath )
{
	downloadFiles[targetPath] = url;

	QUrl fileUrl( url );
	QNetworkRequest request( fileUrl );
	m_WebCtrl.get( request );
}

void HttpFileDownloader::writeFile( QString filePath, QByteArray data )
{
	files << new QSaveFile( filePath );
	files.last()->open( QIODevice::WriteOnly );
	files.last()->write( data );

	files.last()->commit();	// Calling commit() is mandatory, otherwise nothing will be written.
}
