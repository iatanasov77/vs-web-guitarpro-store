#ifndef SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_
#define SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_

#include <QObject>
#include "HttpRequestWorker.h"
#include "CommandState.h"

class HttpFileDownloader : public QObject
{
	Q_OBJECT

	public:
		static HttpFileDownloader* instance();
		void download( QString url, QString targetPath );

	signals:
		void downloaded( QString targetPath );

	private slots:
		void fileDownloaded( CommandState *state );

	private:
		static HttpFileDownloader *_instance;
		QMap<QString, HttpRequestInput*> downloadFiles;

		static HttpFileDownloader* createInstance();
		HttpFileDownloader( QObject *parent = nullptr );

		void writeFile( QString filePath, QByteArray data );
};

#endif /* SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_ */
