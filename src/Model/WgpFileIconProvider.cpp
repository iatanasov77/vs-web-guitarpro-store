#include "WgpFileIconProvider.h"

WgpFileIconProvider::WgpFileIconProvider()
{

}

QIcon WgpFileIconProvider::icon( QFileIconProvider::IconType type ) const
{
	Q_UNUSED( type );

	//return QIcon( ":/Resources/icons/logo.png" );
	return QFileIconProvider::icon( type );
}

QIcon WgpFileIconProvider::icon( const QFileInfo &info ) const
{
	Q_UNUSED( info );

	//return QIcon( ":/Resources/icons/logo.png" );
	return QFileIconProvider::icon( info );
}
