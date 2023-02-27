#ifndef SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_
#define SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSaveFile>

class HttpFileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit HttpFileDownloader( QObject *parent = nullptr );
		virtual ~HttpFileDownloader();
		QByteArray downloadedData( QString target ) const;
		void download( QString url, QString targetPath );

	signals:
		void downloaded( QString targetPath );

	private slots:
		void fileDownloaded( QNetworkReply* pReply );

	private:
		QNetworkAccessManager m_WebCtrl;
		QMap<QString, QByteArray> m_DownloadedData;

		QMap<QString, QString> downloadFiles;
		QList<QSaveFile*> files;
		void writeFile( QString filePath, QByteArray data );
};

#endif /* SRC_APIMANAGER_HTTPFILEDOWNLOADER_H_ */
