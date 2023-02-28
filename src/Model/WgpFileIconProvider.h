#ifndef SRC_MODEL_WGPFILEICONPROVIDER_H_
#define SRC_MODEL_WGPFILEICONPROVIDER_H_

#include <QFileIconProvider>
#include <QFileInfo>
#include <QFileSystemModel>

/**
 * https://www.makeuseof.com/windows-rebuild-icon-cache/
 */
class WgpFileIconProvider : public QFileIconProvider
{
    public:
		WgpFileIconProvider( QFileSystemModel *fileSystemModel );

		virtual QIcon icon( QFileIconProvider::IconType type ) const;
		virtual QIcon icon( const QFileInfo &info ) const;

	private:
		QFileSystemModel *_fileSystemModel;

};

#endif /* SRC_MODEL_WGPFILEICONPROVIDER_H_ */
