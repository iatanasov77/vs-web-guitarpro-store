#ifndef SYSTEMTRAYMENU_H
#define SYSTEMTRAYMENU_H

#include <QObject>
#include <QtWidgets/QSystemTrayIcon>

class SystemTrayMenu : public QObject
{
    Q_OBJECT

	public:
		SystemTrayMenu( QObject *_parent = nullptr );

	public slots:
		void onActivated( QSystemTrayIcon::ActivationReason reason );

	private:
		void init();
};

#endif // SYSTEMTRAYMENU_H
