#include "WgpFileSystemMeta.h"

#include <QDir>
#include <QFile>
#include <QJsonObject>

#ifdef Q_OS_WIN
	#include <fileapi.h>
#endif

WgpFileSystemMeta::WgpFileSystemMeta( WgpFileSystemModel *model )
{
	_model	= model;

	_metaPath	= model->rootPath() + "/.wgp_store_meta";
	if ( ! QDir( _metaPath ).exists() ) {
		QDir().mkdir( _metaPath );

		#ifdef Q_OS_WIN
			LPCWSTR lstrPath = reinterpret_cast<LPCWSTR>( _metaPath.constData() );
			SetFileAttributes( lstrPath, FILE_ATTRIBUTE_HIDDEN );
		#endif
	}
}

QJsonDocument WgpFileSystemMeta::loadMetaJson()
{
	QString fileName	= _metaPath + "/server_loaded.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::ReadOnly );
    return QJsonDocument().fromJson( jsonFile.readAll() );
}

void WgpFileSystemMeta::saveMetaJson( QJsonDocument document )
{
	QString fileName	= _metaPath + "/server_loaded.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::WriteOnly );
    jsonFile.write( document.toJson() );
}

void WgpFileSystemMeta::appendToMeta( QJsonArray arr )
{
	for( int i = 0; i < arr.size(); i++ ) {
		metaJson.push_back( arr[i].toObject() );
	}

	saveMetaJson( QJsonDocument::fromVariant( metaJson.toVariantList() ) );
}
