#ifndef SRC_MODEL_WGPFILESYSTEMMODEL_H_
#define SRC_MODEL_WGPFILESYSTEMMODEL_H_

#include <QFileSystemModel>

class WgpFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

	public:
		WgpFileSystemModel( QObject *parent = nullptr );
		virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

};


#endif /* SRC_MODEL_WGPFILESYSTEMMODEL_H_ */
