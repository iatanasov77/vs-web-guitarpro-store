#include "WgpFileSystemMeta.h"

#include <QDir>
#include <QFile>
#include <QJsonObject>

#include "WgpFileSystem.h"

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

	initServerObjects();
	initLocalObjects();
}

void WgpFileSystemMeta::initServerObjects()
{
	QJsonDocument document 	= loadServerObjects();
	metaServerJson			= document.array();
}

void WgpFileSystemMeta::initLocalObjects()
{
	QJsonDocument document 	= loadLocalObjects();
	metaLocalJson			= document.array();

	if ( ! metaLocalJson.isEmpty() ) {
		return;
	}

	QString fileName	= _metaPath + "/local_objects.json";
	metaLocalJson		= metaServerJson;
	document.setArray( metaLocalJson );

	QFile jsonFile( fileName );
	jsonFile.open( QFile::WriteOnly );
	jsonFile.write( document.toJson() );
}

QJsonObject WgpFileSystemMeta::createMetaObject( QMap<QString, QVariant> data, FileSystemObject objectType )
{
	QJsonObject object;

	if ( objectType == OBJECT_CATEGORY ) {
		object["id"]			= 0;
		object["code"]			= "";
		object["name"]			= data["name"].toString();
		object["children"]		= QJsonArray();
		object["tablatures"]	= QJsonObject();
	} else {
		object["id"]			= 0;
		object["enabled"]		= true;
		object["artist"]		= data["artist"].toString();
		object["song"]			= data["song"].toString();
		object["createdAt"]		= QDateTime::currentDateTime().toString( "yyyy-MM-dd" );
		object["updatedAt"]		= QDateTime::currentDateTime().toString( "yyyy-MM-dd" );

		QJsonObject tablatureFile;
		tablatureFile["originalName"]	= data["originalName"].toString();
		tablatureFile["path"]			= data["path"].toString();

		object["tablatureFile"]	= tablatureFile;
	}

	return object;
}

QJsonDocument WgpFileSystemMeta::loadLocalObjects()
{
	QString fileName	= _metaPath + "/local_objects.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::ReadOnly );

    return QJsonDocument().fromJson( jsonFile.readAll() );
}

void WgpFileSystemMeta::saveLocalObjects( QJsonDocument document )
{
	QString fileName	= _metaPath + "/local_objects.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::WriteOnly );
    jsonFile.write( document.toJson() );
}

void WgpFileSystemMeta::appendToLocalObjects( QJsonObject jc )
{
	metaLocalJson.append( jc );

	QJsonDocument document 	= loadLocalObjects();
	document.setArray( metaLocalJson );
	saveLocalObjects( document );
}

/**
 * @TODO Should be Pass 'code' Not 'name'
 */
bool WgpFileSystemMeta::inLocalObjects( QString name, FileSystemObject objectType )
{
	for ( auto v : metaLocalJson ) {
	    QJsonObject jc	= v.toObject();

	    if ( objectType == OBJECT_CATEGORY ) {
			if ( name == jc["name"].toString() ) {
				return true;
			}
		} else {
			QJsonObject tabs	= jc.value( "tablatures" ).toObject();
			foreach( const QString& key, tabs.keys() ) {
				QJsonObject jt	= tabs.value( key ).toObject();

				QJsonObject tablatureFile	= jt.value( "tablatureFile" ).toObject();
				if ( name == tablatureFile["originalName"].toString() ) {
					return true;
				}
			}
		}
	}

	return false;
}

QJsonDocument WgpFileSystemMeta::loadServerObjects()
{
	QString fileName	= _metaPath + "/server_objects.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::ReadOnly );

    return QJsonDocument().fromJson( jsonFile.readAll() );
}

void WgpFileSystemMeta::saveServerObjects( QJsonDocument document )
{
	QString fileName	= _metaPath + "/server_objects.json";

    QFile jsonFile( fileName );
    jsonFile.open( QFile::WriteOnly );
    jsonFile.write( document.toJson() );
}

void WgpFileSystemMeta::appendToServerObjects( QJsonObject jc )
{
	metaLocalJson.append( jc );

	QJsonDocument document 	= loadLocalObjects();
	document.setArray( metaLocalJson );
	saveLocalObjects( document );
}

void WgpFileSystemMeta::refreshServerObjects( QJsonObject jc )
{
	clearMeta();

	foreach( const QString& key, jc.keys() ) {
		QJsonObject meta	= jc.value( key ).toObject();

		metaServerJson.push_back( meta );
	}

	QJsonDocument newMetaObjects	= QJsonDocument::fromVariant( metaServerJson.toVariantList() );
	saveServerObjects( newMetaObjects );
	saveLocalObjects( newMetaObjects );
}

void WgpFileSystemMeta::clearMeta()
{
	while( metaServerJson.count() ) {
		metaServerJson.pop_back();
	}
	/*
	while( metaLocalJson.count() ) {
		metaLocalJson.pop_back();
	}
	*/
}

QStringList WgpFileSystemMeta::compareMeta()
{
	m_differences.clear();

	QJsonDocument metaServer	= QJsonDocument::fromVariant( metaServerJson.toVariantList() );
	QJsonDocument metaLocal		= QJsonDocument::fromVariant( metaLocalJson.toVariantList() );

	QJsonObject obj1 = metaServer.object();
	QJsonObject obj2 = metaLocal.object();

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
		int arr1Size = static_cast<int>( arr1.size() );

		for ( int i = minSize; i < arr1Size; i++ ) {
			m_differences << keyStack.join( ", " ) + ", " + "[" + QString::number( i ) + "]" + ": deleted";
		}
	}

	if ( arr1.size() < arr2.size() ) {
		int arr2Size = static_cast<int>( arr2.size() );

		for ( int i = minSize; i < arr2Size; i++ ) {
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
