#ifndef SRC_MODEL_WGPFILEICONPROVIDER_H_
#define SRC_MODEL_WGPFILEICONPROVIDER_H_

#include <QFileIconProvider>
#include <QFileInfo>

class WgpFileIconProvider : public QFileIconProvider
{
    public:
		virtual QIcon icon( QFileIconProvider::IconType type ) const;
		virtual QIcon icon( const QFileInfo &info ) const;

};

#endif /* SRC_MODEL_WGPFILEICONPROVIDER_H_ */
