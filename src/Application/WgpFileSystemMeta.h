#ifndef SRC_APPLICATION_WGPFILESYSTEMMETA_H_
#define SRC_APPLICATION_WGPFILESYSTEMMETA_H_

#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>

#include "GlobalTypes.h"
#include "Model/WgpFileSystemModel.h"

class WgpFileSystemMeta
{
	private:
		WgpFileSystemModel *_model;
		QString _metaPath;
		QJsonArray metaServerJson;
		QJsonArray metaLocalJson;
		QStringList m_differences;
		QJsonArray metaFileSystemFilesJson;

		void clearMeta();
		void initServerObjects();
		void initLocalObjects();

		void compareObjects( QStringList keyStack, const QJsonObject obj1, const QJsonObject obj2 );
		void compareArrays( QStringList keyStack, const QJsonArray arr1, const QJsonArray arr2 );
		void compareValues( QStringList keyStack, const QJsonValue val1, const QJsonValue val2 );

	public:
		WgpFileSystemMeta( WgpFileSystemModel *model );

		QJsonObject createMetaObject( QMap<QString, QVariant> data, FileSystemObject objectType );
		bool inLocalObjects( QString categoryName, FileSystemObject objectType );
		QJsonDocument loadLocalObjects();
		void saveLocalObjects( QJsonDocument document );
		void appendToLocalObjects( QJsonObject jc );

		QJsonDocument loadServerObjects();
		void saveServerObjects( QJsonDocument document );
		void appendToServerObjects( QJsonObject jc );
		void refreshServerObjects( QJsonObject jc );
		QStringList compareMeta();

		QJsonDocument loadFileSystemFiles();
		void saveFileSystemFiles( QJsonDocument document );
		void appendToFileSystemFiles( QString path );
};

#endif /* SRC_APPLICATION_WGPFILESYSTEMMETA_H_ */
