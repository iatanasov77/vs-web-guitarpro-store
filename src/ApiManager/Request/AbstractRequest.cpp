#include "AbstractRequest.h"

#include <QUrl>

AbstractRequest::AbstractRequest( HttpRequestInput *input,  QObject *parent  ) : QObject( parent )
{
	_input			= input;
	_requestContent	= "";

	_request = new QNetworkRequest( QUrl( _input->httpUrl ) );
	_request->setRawHeader( "User-Agent", "VS WebGuitarPro Store" );
	//_request->setRawHeader( "Accept", "*/*" );
	//_request->setRawHeader( "Accept-Encoding", "gzip, deflate, br" );
}

QNetworkRequest *AbstractRequest::request()
{
	return _request;
}

HttpRequestInput *AbstractRequest::requestInput()
{
	return _input;
}

QByteArray AbstractRequest::requestContent()
{
	return _requestContent;
}

QHttpMultiPart *AbstractRequest::multiPart()
{
	return _multiPart;
}

QString AbstractRequest::httpAttributeEncode( QString attributeName, QString input )
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
