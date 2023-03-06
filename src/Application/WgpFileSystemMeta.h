#ifndef SRC_APPLICATION_WGPFILESYSTEMMETA_H_
#define SRC_APPLICATION_WGPFILESYSTEMMETA_H_

#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>

#include "Model/WgpFileSystemModel.h"

class WgpFileSystemMeta
{
	private:
		WgpFileSystemModel *_model;
		QString _metaPath;
		QJsonArray metaServerJson;
		QJsonArray metaLocalJson;
		QStringList m_differences;

		void compareObjects( QStringList keyStack, const QJsonObject obj1, const QJsonObject obj2 );
		void compareArrays( QStringList keyStack, const QJsonArray arr1, const QJsonArray arr2 );
		void compareValues( QStringList keyStack, const QJsonValue val1, const QJsonValue val2 );

	public:
		WgpFileSystemMeta( WgpFileSystemModel *model );

		QJsonDocument loadMetaJson();
		void saveMetaJson( QJsonDocument document );
		void appendToServerMeta( QJsonArray arr );
		void clearMeta();
		QStringList compareMeta();
};

#endif /* SRC_APPLICATION_WGPFILESYSTEMMETA_H_ */
