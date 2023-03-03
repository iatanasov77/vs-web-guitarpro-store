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
		HttpRequestWorker *m_httpRequestWorker;

		WgpMyTablatures( QObject *_parent = nullptr );
        static WgpMyTablatures *createInstance();

        void _getMyTablatures();
        void _getMyCategories();
        void _getMyTablaturesUncategorized();

	public:
        static WgpMyTablatures *instance();

        bool getMyTablatures();

	public slots:
		void handleMyTablaturesResult( HttpRequestWorker *worker );

	signals:
		void getMyCategoriesFinished( HttpRequestWorker* );
		void getMyTablaturesFinished( HttpRequestWorker* );
		void serverLoadFinished();
};

#endif // WGP_MYTABLATURES_H
