#ifndef SRC_APPLICATION_WGPFILESYSTEMMETA_H_
#define SRC_APPLICATION_WGPFILESYSTEMMETA_H_

#include <QString>
#include <QJsonDocument>
#include <QJsonArray>

#include "Model/WgpFileSystemModel.h"

class WgpFileSystemMeta
{
	private:
		WgpFileSystemModel *_model;
		QString _metaPath;
		QJsonArray metaJson;

	public:
		WgpFileSystemMeta( WgpFileSystemModel *model );

		QJsonDocument loadMetaJson();
		void saveMetaJson( QJsonDocument document );
		void appendToMeta( QJsonArray arr );
};

#endif /* SRC_APPLICATION_WGPFILESYSTEMMETA_H_ */
