#ifndef VS_AUTH_H
#define VS_AUTH_H

#include <QObject>
#include <QString>
#include <QMap>

#include "ApiManager/CommandState.h"

enum AuthRequest{LOGIN_CHECK, UNDEFINED};
static const QMap<AuthRequest, QString> RequestTypes {
	{ LOGIN_CHECK, "LoginCheck" },
};

class VsAuth : public QObject
{
	Q_OBJECT

	private:
		static VsAuth *_instance;

		explicit VsAuth( QObject *_parent = nullptr );
        static VsAuth *createInstance();

	public:
        static VsAuth *instance();

        bool isLoggedIn();
        bool login( QString username, QString password );
        void logout();
        QString userFullName();

	public slots:
		void handleAuthResult( CommandState *state );

	signals:
		void loginCheckFinished( CommandState* );
};

#endif // VS_AUTH_H
