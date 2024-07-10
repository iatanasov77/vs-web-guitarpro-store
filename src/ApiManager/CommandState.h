#ifndef WORKERSTATE_H
#define WORKERSTATE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QNetworkReply>

class CommandState : public QObject
{
	Q_OBJECT

	public:
		CommandState( QObject *parent = 0 );

		QByteArray response;
		QNetworkReply::NetworkError errorType;
		QString errorStr;
		QString commandId;
		QString requestName;
		QString lastFinishedRequest;
		QString downloadingFile;
		QString downloadedFile;
};

#endif // WORKERSTATE_H
