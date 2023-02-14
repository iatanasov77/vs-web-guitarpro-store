#include "HttpRequestInput.h"

HttpRequestInput::HttpRequestInput()
{
    initialize();
}

HttpRequestInput::HttpRequestInput( QString v_httpUrl, QString v_httpMethod )
{
    initialize();

    httpUrl		= v_httpUrl;
    httpMethod	= v_httpMethod;
}

void HttpRequestInput::initialize()
{
    varLayout 	= NOT_SET;
    httpUrl 	= "";
    httpMethod	= "GET";
}

void HttpRequestInput::addVar( QString key, QString value )
{
    vars[key] = value;
}

void HttpRequestInput::addFile( QString localFilename, QString requestFilename, QString mimeType )
{
    HttpRequestInputFileElement file;

    file.localFilename = localFilename;
    file.requestFilename = requestFilename;
    file.mimeType = mimeType;

    files.append( file );
}
