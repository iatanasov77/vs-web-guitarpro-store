#include "WgpFileIconProvider.h"

QIcon WgpFileIconProvider::icon( QFileIconProvider::IconType type ) const
{
	if ( type == QFileIconProvider::Folder ) {
		return QPixmap( ":/Resources/icons/logo.png" );
	}

	return QFileIconProvider::icon( type );
}

QIcon WgpFileIconProvider::icon( const QFileInfo &info ) const
{
	if ( info.isDir() ) {
		return QPixmap( ":/Resources/icons/logo.png" );
	}

	return QFileIconProvider::icon( info );
}
