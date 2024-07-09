#include "HttpRequestInput.h"

/**
 * This is a Request Wrapper for Sending Request to Request Worker
 */

HttpRequestInput::HttpRequestInput()
{
    initialize();
}

HttpRequestInput::HttpRequestInput( QString v_httpUrl, QString v_httpMethod, QString v_targetPath )
{
    initialize();

    httpUrl		= v_httpUrl;
    httpMethod	= v_httpMethod;
    targetPath  = v_targetPath;
}

void HttpRequestInput::initialize()
{
    varLayout 	= NOT_SET;
    httpUrl 	= "";
    httpMethod	= "GET";
    targetPath  = "";
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
