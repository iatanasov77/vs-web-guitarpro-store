#ifndef SRC_MODEL_WGPFILEICONPROVIDER_H_
#define SRC_MODEL_WGPFILEICONPROVIDER_H_

#include <QFileIconProvider>
#include <QFileInfo>

/**
 * https://www.makeuseof.com/windows-rebuild-icon-cache/
 */
class WgpFileIconProvider : public QFileIconProvider
{
    public:
		WgpFileIconProvider();

		virtual QIcon icon( QFileIconProvider::IconType type ) const;
		virtual QIcon icon( const QFileInfo &info ) const;
};

#endif /* SRC_MODEL_WGPFILEICONPROVIDER_H_ */
