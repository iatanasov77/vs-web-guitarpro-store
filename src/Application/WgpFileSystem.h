#ifndef SRC_APPLICATION_WGPFILESYSTEM_H_
#define SRC_APPLICATION_WGPFILESYSTEM_H_

#include <QObject>
#include <QFileSystemWatcher>

#include "ApiManager/HttpRequestWorker.h"
#include "ApiManager/HttpFileDownloader.h"

class WgpFileSystem : public QObject
{
	Q_OBJECT

	private:
		static WgpFileSystem *_instance;
		QString rootPath;
		HttpFileDownloader *downloader;
		QFileSystemWatcher *watcher;

		WgpFileSystem( QObject *_parent = nullptr );
        static WgpFileSystem *createInstance();

        void createWatcher();

	public:
        static WgpFileSystem *instance();

        void sync();

    public slots:
        void handleMyCategoriesResult( HttpRequestWorker *worker );
        void handleMyTablaturesResult( HttpRequestWorker *worker );
        void fileModified( QString path );

};

#endif /* SRC_APPLICATION_WGPFILESYSTEM_H_ */
