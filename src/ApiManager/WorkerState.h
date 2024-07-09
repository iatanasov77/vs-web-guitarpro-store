#ifndef WORKERSTATE_H
#define WORKERSTATE_H

#include <QString>
#include <QByteArray>
#include <QNetworkReply>

class WorkerState
{
	public:
		WorkerState();

		QByteArray response;
		QNetworkReply::NetworkError errorType;
		QString errorStr;
		QString commandId;
		QString requestName;
		QString lastFinishedRequest;
		QString downloadingFile;
		QString downloadedFile;
		bool working;
};

#endif // WORKERSTATE_H
