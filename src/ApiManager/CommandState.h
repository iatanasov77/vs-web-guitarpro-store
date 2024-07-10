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

		QString commandId;
		QString requestName;

		QNetworkReply::NetworkError errorType;
		QString errorStr;

		QByteArray response;
		QString downloadedFile;
};

#endif // WORKERSTATE_H
