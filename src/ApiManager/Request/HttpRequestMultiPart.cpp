#include "HttpRequestMultiPart.h"

#include <QDebug>
#include <QUrl>
#include <QFileInfo>
#include <QDateTime>

#include "GlobalTypes.h"

HttpRequestMultiPart::HttpRequestMultiPart( HttpRequestInput *input,  QObject *parent ) : AbstractRequest( input, parent ) { }

QNetworkRequest *HttpRequestMultiPart::createRequest()
{
	_multiPart = new QHttpMultiPart( QHttpMultiPart::FormDataType );

	// add variables
	foreach ( QString key, _input->vars.keys() ) {
		QHttpPart textPart;

		textPart.setHeader( QNetworkRequest::ContentDispositionHeader, QVariant( QString( "form-data; name=\"%1\"" ).arg( key ) ) );
		textPart.setBody( _input->vars.value( key ).toUtf8() );

		_multiPart->append( textPart );
	}

	// add files
	for ( QList<HttpRequestInputFileElement>::iterator fileInfo = _input->files.begin(); fileInfo != _input->files.end(); fileInfo++ ) {
		qDebug() << "HttpRequestMultiPart Add File: " << fileInfo->requestFilename;

		QHttpPart filePart;
		_files[fileInfo->requestFilename] = new QFile( fileInfo->localFilename );
		_files[fileInfo->requestFilename]->open( QIODevice::ReadOnly );

		filePart.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( fileInfo->mimeType ) );
		filePart.setHeader( QNetworkRequest::ContentLengthHeader, _files[fileInfo->requestFilename]->size() );
		filePart.setHeader(
			QNetworkRequest::ContentDispositionHeader,
			QVariant(
				QString( "form-data; name=\"%1\"; filename=\"%2\"" )
					.arg( fileInfo->requestFilename, _files[fileInfo->requestFilename]->fileName() )
			)
		);

		filePart.setBodyDevice( _files[fileInfo->requestFilename] );
		//filePart.setBodyDevice( 0 );
		//filePart.setBody( _files[fileInfo->requestFilename]->readAll() );

		_files[fileInfo->requestFilename]->setParent( _multiPart ); // we cannot delete the file now, so delete it with the multiPart
		_multiPart->append( filePart );
	}

	return _request;
}
