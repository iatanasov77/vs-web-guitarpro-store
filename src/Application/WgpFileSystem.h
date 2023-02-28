#ifndef SRC_APPLICATION_WGPFILESYSTEM_H_
#define SRC_APPLICATION_WGPFILESYSTEM_H_

#include <QObject>
#include <QFileSystemWatcher>

#include "ApiManager/HttpRequestWorker.h"
#include "ApiManager/HttpFileDownloader.h"
#include "Model/WgpFileSystemModel.h"
#include "Model/WgpFileIconProvider.h"

class WgpFileSystem : public QObject
{
	Q_OBJECT

	private:
		static WgpFileSystem *_instance;
		HttpFileDownloader *downloader;

		WgpFileSystemModel *model;
		WgpFileIconProvider *iconProvider;
		QFileSystemWatcher *watcher;

		WgpFileSystem( QObject *_parent = nullptr );
        static WgpFileSystem *createInstance();

        void createModel();
        QMap<QString, QString> authHeaders();

	public:
        static WgpFileSystem *instance();

        void sync();

    public slots:
        void handleMyCategoriesResult( HttpRequestWorker *worker );
        void handleMyTablaturesResult( HttpRequestWorker *worker );
        void handleDownloadedTablature( QString targetPath );
        void fileModified( QString path );

};

#endif /* SRC_APPLICATION_WGPFILESYSTEM_H_ */
