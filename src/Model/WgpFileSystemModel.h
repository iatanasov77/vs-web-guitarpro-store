#ifndef SRC_MODEL_WGPFILESYSTEMMODEL_H_
#define SRC_MODEL_WGPFILESYSTEMMODEL_H_

#include <QFileSystemModel>

class WgpFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

	public:
		WgpFileSystemModel( QObject *parent = nullptr );

};


#endif /* SRC_MODEL_WGPFILESYSTEMMODEL_H_ */
