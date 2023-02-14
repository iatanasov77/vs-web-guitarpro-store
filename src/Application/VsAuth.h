#ifndef VS_AUTH_H
#define VS_AUTH_H

#include <QString>
#include "ApiManager/HttpRequest.h"
#include "ApiManager/JsonRequest.h"

class VsAuth
{
	private:
		VsAuth();
        static VsAuth *createInstance();
        static VsAuth *_instance;

	public:
        ~VsAuth();
        static VsAuth *instance();

        bool isLoggedIn();
        bool login( QString username, QString password );
        bool testRequestWorker();
};

#endif // VS_AUTH_H
