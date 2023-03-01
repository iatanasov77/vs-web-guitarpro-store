#include "WgpFileSystemModel.h"

#include <QDebug>


/**
 * https://forum.qt.io/topic/102074/icons-in-qfilesystemmodel/9
 * https://stackoverflow.com/questions/27587035/qfilesystemmodel-custom-icons
 * https://stackoverflow.com/questions/68941080/update-folder-icon-with-desktop-ini-instantly-change-c
 */
WgpFileSystemModel::WgpFileSystemModel( QObject *parent ) : QFileSystemModel( parent )
{
	QString _rootPath	= QDir::homePath() + "/WebGuitarPro/";
	if ( ! QDir( _rootPath ).exists() ) {
		QDir().mkdir( _rootPath );
	}
	setRootPath( _rootPath );
}

QVariant WgpFileSystemModel::data( const QModelIndex& index, int role ) const
{
/*
	QFileInfo info = WgpFileSystemModel::fileInfo( index );

	if( index == this->index( rootPath() ) ) {
		switch ( role ) {
			case QFileSystemModel::FileIconRole:
			case QFileSystemModel::FilePathRole:
				{
					if( info.isDir() ) {
						QPixmap oPixmap( 32,32 );
						oPixmap.load ( ":/Resources/icons/logo.png" );
						return QIcon( oPixmap );
					}
				}
				break;
		}
	}
*/
	return QFileSystemModel::data( index, role );
}
