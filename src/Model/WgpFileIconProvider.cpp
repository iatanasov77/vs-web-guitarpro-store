#include "WgpFileIconProvider.h"

WgpFileIconProvider::WgpFileIconProvider()
{

}

QIcon WgpFileIconProvider::icon( QFileIconProvider::IconType type ) const
{
	// return QPixmap( ":/Resources/icons/logo.png" );

	return QFileIconProvider::icon( type );
}

QIcon WgpFileIconProvider::icon( const QFileInfo &info ) const
{
	// return QPixmap( ":/Resources/icons/logo.png" );

	return QFileIconProvider::icon( info );
}
