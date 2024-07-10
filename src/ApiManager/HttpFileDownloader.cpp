#include "HttpFileDownloader.h"

#include <QDebug>
#include <QSaveFile>

/*
 * This one did the trick!
 * ------------------------
 * But I want to do this: https://stackoverflow.com/questions/46172607/qt-singleton-implementation
 */
HttpFileDownloader *HttpFileDownloader::_instance = 0;

HttpFileDownloader::HttpFileDownloader( QObject *parent ) : QObject( parent )
{
	//qDebug() << "FILE DOWNLOADER INITIALIZED !!!";
	connect(
		HttpRequestWorker::instance(), SIGNAL ( myTablatureDownloadResponseReady( CommandState* ) ),
		this, SLOT ( fileDownloaded( CommandState* ) )
	);
}

HttpFileDownloader *HttpFileDownloader::createInstance()
{
    return new HttpFileDownloader();
}

HttpFileDownloader *HttpFileDownloader::instance() {
	if ( ! _instance ) {
		_instance = createInstance();
	}

	return _instance;
}

void HttpFileDownloader::fileDownloaded( CommandState *state )
{
	QString fileTarget	= state->downloadedFile;
	QByteArray fileData	= state->response;

	qDebug() << "FILE DOWNLOADED: " << fileTarget << "   (" << fileData.length() << " BITES )";
	writeFile( fileTarget, fileData );
	emit downloaded( fileTarget );
}

void HttpFileDownloader::download( QString url, QString targetPath )
{
	qDebug() << "'HttpFileDownloader::download' Download URL: " << url;

	downloadFiles[targetPath]				= new HttpRequestInput( url, "GET", targetPath );
	downloadFiles[targetPath]->requestType	= REQUEST_TYPE_DOWNLOAD;

	HttpRequestWorker::instance()->execute( downloadFiles[targetPath], HttpRequests["DOWNLOAD_TABLATURE_REQUEST"], true );
}

void HttpFileDownloader::writeFile( QString filePath, QByteArray data )
{
	QSaveFile *flie	= new QSaveFile( filePath );

	flie->open( QIODevice::WriteOnly );
	flie->write( data );
	flie->commit();	// Calling commit() is mandatory, otherwise nothing will be written.
}
