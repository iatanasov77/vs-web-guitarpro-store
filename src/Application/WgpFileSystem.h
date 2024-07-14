#ifndef SRC_APPLICATION_WGPFILESYSTEM_H_
#define SRC_APPLICATION_WGPFILESYSTEM_H_

#include <QObject>
#include <QFileSystemWatcher>
#include <QJsonObject>

#include "ApiManager/CommandState.h"
#include "Model/WgpFileSystemModel.h"
#include "Model/WgpFileIconProvider.h"
#include "WgpFileSystemMeta.h"

class WgpFileSystem : public QObject
{
	Q_OBJECT

	private:
		static WgpFileSystem *_instance;
		QStringList allowedMimeTypes;
		QStringList excludePaths;
		QStringList reservedNames;

		WgpFileSystemModel *_model;
		WgpFileIconProvider *iconProvider;
		QFileSystemWatcher *watcher;
		WgpFileSystemMeta *meta;

		WgpFileSystem( QObject *_parent = nullptr );
        static WgpFileSystem *createInstance();

        void fixLocalMetaObjects();
        QStringList findNewCategories( QString path );
        void removeDeletedFiles();
        void createModel();
        void initWatcher();
        void _createCategories( QJsonObject jc, QString path );

	public:
        static WgpFileSystem *instance();

        WgpFileSystemModel *model();
        void metaDifferences();
        void sync();
        void createCategory( QString name, QString path );
        void downloadTablature( int tabId, QString originalName, QString tablaturePath );

    public slots:
		void handleSharedToMeTablaturesResult( CommandState *state );
        void handleMyCategoriesResult( CommandState *state );
        void handleMyTablaturesResult( CommandState *state );
        void handleUpdateCategoryResult( CommandState *state );
        void handleUploadTablatureResult( CommandState *state );
        void handleDeleteResourceResult( CommandState *state );
        void serverLoadFinished();
        void handleDownloadedTablature( QString targetPath );
        void directoryLoaded( QString path );
        void fileRenamed( QString path, QString oldName, QString newName );
        void fileModified( QString path );
        void directoryModified( QString path );

};

#endif /* SRC_APPLICATION_WGPFILESYSTEM_H_ */
