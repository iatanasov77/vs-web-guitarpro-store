#include "HttpFileDownloader.h"

#include <QDebug>
#include <QSaveFile>

HttpFileDownloader::HttpFileDownloader( QObject *parent ) : QObject( parent )
{
	connect(
		HttpRequestWorker::instance(), SIGNAL ( myTablatureDownloadResponseReady( CommandState ) ),
		this, SLOT ( fileDownloaded( CommandState ) )
	);
}

HttpFileDownloader::~HttpFileDownloader() { }

void HttpFileDownloader::fileDownloaded( CommandState state )
{
	QString fileTarget	= state.downloadedFile;
	QByteArray fileData	= state.response;

	qDebug() << "FILE DOWNLOADED: " << fileTarget << "   (" << fileData.length() << " BITES )";
	writeFile( fileTarget, fileData );
	emit downloaded( fileTarget );
}

void HttpFileDownloader::download( QString url, QString targetPath )
{
	qDebug() << "'HttpFileDownloader::download' Download URL: " << url;

	HttpRequestInput input( url, "GET", targetPath );
	input.requestType	= REQUEST_TYPE_DOWNLOAD;

	HttpRequestWorker::instance()->execute( input, HttpRequests["DOWNLOAD_TABLATURE_REQUEST"], true );
}

void HttpFileDownloader::writeFile( QString filePath, QByteArray data )
{
	QSaveFile *flie	= new QSaveFile( filePath );

	flie->open( QIODevice::WriteOnly );
	flie->write( data );
	flie->commit();	// Calling commit() is mandatory, otherwise nothing will be written.
}
