#include "WgpFileSystemModel.h"


WgpFileSystemModel::WgpFileSystemModel( QObject *parent ) : QFileSystemModel( parent )
{
	QString _rootPath	= QDir::homePath() + "/WebGuitarPro";
	if ( ! QDir( _rootPath ).exists() ) {
		QDir().mkdir( _rootPath );
	}
	setRootPath( _rootPath );
}

