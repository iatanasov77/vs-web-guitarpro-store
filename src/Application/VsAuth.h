#ifndef VS_AUTH_H
#define VS_AUTH_H

#include <QObject>
#include <QString>
#include <QMap>

#include "ApiManager/HttpRequestWorker.h"

enum AuthRequest{LOGIN_CHECK, UNDEFINED};
static const QMap<AuthRequest, QString> RequestTypes {
	{ LOGIN_CHECK, "LoginCheck" },
};

class VsAuth : public QObject
{
	Q_OBJECT

	private:
		static VsAuth *_instance;
	    HttpRequestWorker *m_httpRequestWorker;

		explicit VsAuth( QObject *_parent = nullptr );
        static VsAuth *createInstance();

        void handleLoginCheck( HttpRequestWorker *worker );

	public:
        static VsAuth *instance();

        bool isLoggedIn();
        bool login( QString username, QString password );

	public slots:
		void handleAuthResult( HttpRequestWorker *worker );

	signals:
		void loginCheckFinished( HttpRequestWorker* );
};

#endif // VS_AUTH_H
