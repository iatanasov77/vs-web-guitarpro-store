#include "WgpFileIconProvider.h"

WgpFileIconProvider::WgpFileIconProvider( QFileSystemModel *fileSystemModel )
{
	_fileSystemModel	= fileSystemModel;
}

QIcon WgpFileIconProvider::icon( QFileIconProvider::IconType type ) const
{
	/*
	if ( type == QFileIconProvider::Folder ) {
		return QPixmap( ":/Resources/icons/logo.png" );
	}
	*/

	return QFileIconProvider::icon( type );
}

QIcon WgpFileIconProvider::icon( const QFileInfo &info ) const
{
	/*
	if ( info.absolutePath() == _fileSystemModel->rootPath() ) {
		return QPixmap( ":/Resources/icons/logo.png" );
	}
	*/

	return QFileIconProvider::icon( info );
}
