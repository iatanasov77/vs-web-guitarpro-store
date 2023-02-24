#ifndef WGP_MYTABLATURES_H
#define WGP_MYTABLATURES_H

#include "ApiManager/HttpRequest.h"
#include "ApiManager/JsonRequest.h"

class WgpMyTablatures
{
	private:
		WgpMyTablatures();
        static WgpMyTablatures *createInstance();
        static WgpMyTablatures *_instance;

        void _getMyTablatures();

	public:
        ~WgpMyTablatures();
        static WgpMyTablatures *instance();

        bool getMyTablatures();
};

#endif // WGP_MYTABLATURES_H
