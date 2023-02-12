#ifndef VS_AUTH_H
#define VS_AUTH_H

#include <QSettings>

class VsAuth
{
	protected:
		VsAuth();
        static VsAuth *createInstance();

        static VsAuth* _instance;

	public:
        ~VsAuth();
        static VsAuth *instance();

        QVariant value( QString key, QString group, QVariant defaultValue = QVariant() );
        void setValue( QString key, QVariant value, QString group );
};

#endif // VS_AUTH_H
