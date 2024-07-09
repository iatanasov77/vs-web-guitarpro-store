#include "DownloadRequest.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

DownloadRequest::DownloadRequest( HttpRequestInput *input,  QObject *parent ) : AbstractRequest( input, parent ) { }

QNetworkRequest *DownloadRequest::createRequest()
{
	return _request;
}
