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

void WgpFileSystemMeta::appendToServerMeta( QJsonArray arr )
{
	for( int i = 0; i < arr.size(); i++ ) {
		metaServerJson.push_back( arr[i].toObject() );
	}

	saveMetaJson( QJsonDocument::fromVariant( metaServerJson.toVariantList() ) );
}

void WgpFileSystemMeta::clearMeta()
{
	while( metaServerJson.count() ) {
		metaServerJson.pop_back();
	}

	while( metaLocalJson.count() ) {
		metaLocalJson.pop_back();
	}
}

QStringList WgpFileSystemMeta::compareMeta()
{
	if ( metaLocalJson.isEmpty() ) {
		metaLocalJson	= metaServerJson;
	}

	m_differences.clear();

	QJsonDocument metaServer	= QJsonDocument::fromVariant( metaServerJson.toVariantList() );
	QJsonDocument metaLocal		= QJsonDocument::fromVariant( metaLocalJson.toVariantList() );

	QJsonObject obj1 = metaServer.object();
	QJsonObject obj2 = metaServer.object();

	compareObjects( *( new QStringList() ), obj1, obj2 );

	return m_differences;
}

void WgpFileSystemMeta::compareObjects( QStringList keyStack, const QJsonObject obj1, const QJsonObject obj2 )
{
	QStringList keysObj1 = obj1.keys();
	QStringList keysObj2 = obj2.keys();

	for ( QString key : keysObj1 ) {
		if ( ! keysObj2.contains( key ) ) {
			m_differences << keyStack.join( ", " ) + ", " + key + ": deleted";
		}
	}

	for ( QString key : keysObj2 ) {
		if ( ! keysObj1.contains( key ) ) {
			m_differences << keyStack.join( ", " ) + ", " + key + ": created";
		}
	}

	for ( QString key : keysObj1 ) {
		if ( keysObj2.contains( key ) ) {
			keyStack.append( key );

			if ( obj1[key].isArray() ) {
				compareArrays( keyStack, obj1[key].toArray(), obj2[key].toArray() );
			} else if ( obj1[key].isObject() ) {
				compareObjects( keyStack, obj1[key].toObject(), obj2[key].toObject() );
			} else {
				if ( obj1[key] != obj2[key] ) {
					compareValues( keyStack, obj1[key], obj2[key] );
				}
			}

			keyStack.removeLast();
		}
	}
}

void WgpFileSystemMeta::compareArrays( QStringList keyStack, const QJsonArray arr1, const QJsonArray arr2 )
{
	quint32 minSize = qMin<int>( arr1.size(), arr2.size() );

	for ( quint32 i = 0; i < minSize; i++ ) {
		keyStack.append("[" + QString::number(i) + "]");

		if ( arr1[i].isArray() ) {
			compareArrays( keyStack, arr1[i].toArray(), arr2[i].toArray() );
		} else if ( arr1[1].isObject() ) {
			compareObjects( keyStack, arr1[i].toObject(), arr2[i].toObject() );
		}

		keyStack.removeLast();
	}

	if ( arr1.size() > arr2.size() ) {
		for ( quint32 i = minSize; i < arr1.size(); i++ ) {
			m_differences << keyStack.join( ", " ) + ", " + "[" + QString::number( i ) + "]" + ": deleted";
		}
	}

	if ( arr1.size() < arr2.size() ) {
		for ( quint32 i = minSize; i < arr2.size(); i++ ) {
			m_differences << keyStack.join( ", " ) + ", " + "[" + QString::number( i ) + "]" + ": created";
		}
	}
}

void WgpFileSystemMeta::compareValues( QStringList keyStack, const QJsonValue val1, const QJsonValue val2 )
{
	if ( val1.isString() ) {
		m_differences << keyStack.join( ", " ) + ": changed from " + val1.toString() + " to " + val2.toString();
	} else if ( val1.isDouble() ) {
		m_differences << keyStack.join( ", " ) + ": changed from " + QString::number( val1.toDouble() ) + " to " + QString::number( val2.toDouble() );
	}
}
