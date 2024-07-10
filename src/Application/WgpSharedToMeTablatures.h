#ifndef WGP_SHAREDTOMETABLATURES_H
#define WGP_SHAREDTOMETABLATURES_H

#include <QObject>
#include "ApiManager/HttpRequestWorker.h"

class WgpSharedToMeTablatures : public QObject
{
	Q_OBJECT

	private:
		static WgpSharedToMeTablatures *_instance;

		WgpSharedToMeTablatures( QObject *_parent = nullptr );
        static WgpSharedToMeTablatures *createInstance();

	public:
        static WgpSharedToMeTablatures *instance();

        bool getSharedToMeTablatures();
};

#endif // WGP_SHAREDTOMETABLATURES_H
