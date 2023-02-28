#include "WgpFileSystemModel.h"

#include <QDebug>

#include "WgpFileIconProvider.h"

/**
 * https://forum.qt.io/topic/102074/icons-in-qfilesystemmodel/9
 * https://stackoverflow.com/questions/27587035/qfilesystemmodel-custom-icons
 */

WgpFileSystemModel::WgpFileSystemModel( QObject *parent ) : QFileSystemModel( parent )
{
	QString _rootPath	= QDir::homePath() + "/WebGuitarPro";
	if ( ! QDir( _rootPath ).exists() ) {
		QDir().mkdir( _rootPath );
	}
	setRootPath( _rootPath );
	setIconProvider( new WgpFileIconProvider );
}

QVariant WgpFileSystemModel::data( const QModelIndex& index, int role ) const
{
	qDebug() << "WgpFileSystemModel Data !!!";
	QFileInfo info = WgpFileSystemModel::fileInfo( index );

	//if( info.isDir() ) {
	//if( index == this->index( rootPath() ) ) {
		switch ( role ) {
			case QFileSystemModel::FileIconRole:
			case QFileSystemModel::FilePathRole:
			case Qt::DisplayRole:
				qDebug() << "WgpFileSystemModel Path Data !!!";
				return QPixmap( ":/Resources/icons/logo.png" );
				break;
		}
	//}

	return QFileSystemModel::data( index, role );
}
