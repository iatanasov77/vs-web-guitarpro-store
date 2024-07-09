#ifndef DOWNLOADREQUEST_H
#define DOWNLOADREQUEST_H

#include <QObject>
#include "AbstractRequest.h"

class DownloadRequest : public AbstractRequest
{
	Q_OBJECT

	public:
		DownloadRequest( HttpRequestInput *input,  QObject *parent = 0 );

		QNetworkRequest *createRequest();
};

#endif // DOWNLOADREQUEST_H
