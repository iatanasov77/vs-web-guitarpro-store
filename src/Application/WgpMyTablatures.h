#ifndef WGP_MYTABLATURES_H
#define WGP_MYTABLATURES_H

#include <QObject>
#include <QString>
#include <QMap>

#include "ApiManager/HttpRequestWorker.h"

enum TablaturesRequest{GET_MY_CATEGORIES, GET_MY_TABLATURES};
static const QMap<TablaturesRequest, QString> TablaturesRequestTypes {
	{ GET_MY_CATEGORIES, "GetMyCategories" },
	{ GET_MY_TABLATURES, "GetMyTablatures" },
};

class WgpMyTablatures : public QObject
{
	Q_OBJECT

	private:
		static WgpMyTablatures *_instance;

		WgpMyTablatures( QObject *_parent = nullptr );
        static WgpMyTablatures *createInstance();

        void _getMyTablatures();
        void _getMyCategories();
        void _getMyTablaturesUncategorized();

	public:
        static WgpMyTablatures *instance();

        bool getMyTablatures();

        void createTablatureCategory( QString $name, int parentId );
        void updateTablatureCategory( int categoryId, QString $name );
        void deleteTablatureCategory( int categoryId );

        void createTablature( QString $name, QString filePath, int categoryId );
		void updateTablature( int tablatureId, QString $name, QString filePath );
		void deleteTablature( int tablatureId );
};

#endif // WGP_MYTABLATURES_H
