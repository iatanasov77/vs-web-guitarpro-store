#include "HttpRequest.h"

#include <QUrl>
#include <QFileInfo>
#include <QDateTime>

#include "GlobalTypes.h"

HttpRequest::HttpRequest( HttpRequestInput *input ) : AbstractRequest( input ) { }

QNetworkRequest *HttpRequest::createRequest()
{
	// decide on the variable layout
	if ( _input->files.length() > 0 ) {
		_input->varLayout = MULTIPART;
	}

	if ( _input->varLayout == NOT_SET ) {
		_input->varLayout = _input->httpMethod == "GET" || _input->httpMethod == "HEAD" ? ADDRESS : URL_ENCODED;
	}

	// prepare request content
	QString boundary = "";

	if ( _input->varLayout == ADDRESS || _input->varLayout == URL_ENCODED ) {
		// variable layout is ADDRESS or URL_ENCODED

		if ( _input->vars.count() > 0 ) {
			bool first = true;
			foreach ( QString key, _input->vars.keys() ) {
				if (!first) {
					_requestContent.append( "&" );
				}
				first = false;

				_requestContent.append( QUrl::toPercentEncoding( key ) );
				_requestContent.append( "=" );
				_requestContent.append( QUrl::toPercentEncoding( _input->vars.value( key ) ) );
			}

			if ( _input->varLayout == ADDRESS ) {
				_input->httpUrl += "?" + _requestContent;
				_requestContent = "";
			}
		}
	}
	else {
		// variable layout is MULTIPART

		boundary = "__-----------------------"
			+ QString::number( QDateTime::currentDateTime().toMSecsSinceEpoch() )
			+ QString::number( rand() );
		QString boundary_delimiter = "--";
		QString new_line = "\r\n";

		// add variables
		foreach ( QString key, _input->vars.keys() ) {
			// add boundary
			_requestContent.append( boundary_delimiter.toUtf8() );
			_requestContent.append( boundary.toUtf8() );
			_requestContent.append( new_line.toUtf8() );

			// add header
			_requestContent.append( "Content-Disposition: form-data; " );
			_requestContent.append( httpAttributeEncode( "name", key ).toUtf8() );
			_requestContent.append( new_line.toUtf8() );
			_requestContent.append( "Content-Type: text/plain" );
			_requestContent.append( new_line.toUtf8() );

			// add header to body splitter
			_requestContent.append( new_line.toUtf8() );

			// add variable content
			_requestContent.append( _input->vars.value( key ).toUtf8() );
			_requestContent.append( new_line.toUtf8() );
		}

		// add files
		for ( QList<HttpRequestInputFileElement>::iterator fileInfo = _input->files.begin(); fileInfo != _input->files.end(); fileInfo++ ) {
			QFileInfo fi( fileInfo->localFilename );

			// ensure necessary variables are available
			if ( fileInfo->localFilename.isEmpty() || ! fi.exists() || ! fi.isFile() || ! fi.isReadable() ) {
				// silent abort for the current file
				continue;
			}

			QFile file( fileInfo->localFilename );
			if ( ! file.open( QIODevice::ReadOnly ) ) {
				// silent abort for the current file
				continue;
			}

			// ensure filename for the request
			if ( fileInfo->requestFilename.isEmpty() ) {
				fileInfo->requestFilename = "file";
			}

			// add boundary
			_requestContent.append( boundary_delimiter.toUtf8() );
			_requestContent.append( boundary.toUtf8() );
			_requestContent.append( new_line.toUtf8() );

			// add header
			// Example Header: Content-Disposition: form-data; name="file2"; filename="a.html"
			_requestContent.append( QString( "Content-Disposition: form-data; %1; %2" ).arg(
				httpAttributeEncode( "name", fileInfo->requestFilename ),
				httpAttributeEncode( "filename", fi.fileName() )
			).toUtf8() );
			_requestContent.append( new_line.toUtf8() );

			if ( ! fileInfo->mimeType.isEmpty() ) {
				_requestContent.append( "Content-Type: " );
				_requestContent.append( fileInfo->mimeType.toUtf8() );
				_requestContent.append( new_line.toUtf8() );
			}

			_requestContent.append( "Content-Transfer-Encoding: binary" );
			_requestContent.append( new_line.toUtf8() );

			// add header to body splitter
			_requestContent.append( new_line.toUtf8() );

			// add file content
			_requestContent.append( file.readAll() );
			_requestContent.append( new_line.toUtf8() );

			file.close();
		}

		// add end of body
		_requestContent.append( boundary_delimiter.toUtf8() );
		_requestContent.append( boundary.toUtf8() );
		_requestContent.append( boundary_delimiter.toUtf8() );
	}


	// prepare connection
	_request = new QNetworkRequest( QUrl( _input->httpUrl ) );
	_request->setRawHeader( "User-Agent", "Agent name goes here" );

	if ( _input->varLayout == URL_ENCODED ) {
		_request->setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
	}
	else if ( _input->varLayout == MULTIPART ) {
		_request->setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary );
	}

	return _request;
}

QString HttpRequest::httpAttributeEncode( QString attributeName, QString input )
{
    // result structure follows RFC 5987
    bool need_utf_encoding = false;
    QString result = "";
    QByteArray input_c = input.toLocal8Bit();
    char c;

    for ( int i = 0; i < input_c.length(); i++ ) {
        c = input_c.at( i );
        if ( c == '\\' || c == '/' || c == '\0' || c < ' ' || c > '~' ) {
            // ignore and request utf-8 version
            need_utf_encoding = true;
        }
        else if ( c == '"' ) {
            result += "\\\"";
        }
        else {
            result += c;
        }
    }

    if ( result.length() == 0 ) {
        need_utf_encoding = true;
    }

    if ( ! need_utf_encoding ) {
        // return simple version
        return QString( "%1=\"%2\"" ).arg( attributeName, result );
    }

    QString result_utf8 = "";
    for ( int i = 0; i < input_c.length(); i++ ) {
        c = input_c.at( i );
        if (
            ( c >= '0' && c <= '9' )
            || ( c >= 'A' && c <= 'Z' )
            || ( c >= 'a' && c <= 'z' )
        ) {
            result_utf8 += c;
        }
        else {
            result_utf8 += "%" + QString::number( static_cast<unsigned char>( input_c.at( i ) ), 16 ).toUpper();
        }
    }

    // return enhanced version with UTF-8 support
    return QString( "%1=\"%2\"; %1*=utf-8''%3" ).arg( attributeName, result, result_utf8 );
}
