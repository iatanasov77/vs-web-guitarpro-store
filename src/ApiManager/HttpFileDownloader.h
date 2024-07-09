#ifndef SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_
#define SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_

#include <QObject>
#include "HttpRequestWorker.h"
#include "WorkerState.h"

class HttpFileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit HttpFileDownloader( QObject *parent = nullptr );
		virtual ~HttpFileDownloader();

		void download( QString url, QString targetPath );

	signals:
		void downloaded( QString targetPath );

	private slots:
		void fileDownloaded( WorkerState state );

	private:
		void writeFile( QString filePath, QByteArray data );
};

#endif /* SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_ */
