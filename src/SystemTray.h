#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QObject>
#include <QtWidgets/QSystemTrayIcon>
#include "Widget/SystemTrayMenu.h"

class SystemTray : public QObject
{
    Q_OBJECT

	private:
		QSystemTrayIcon *trayIcon;
		SystemTrayMenu *sysTrayMenu;

		void loginToWebGuitarPro();
		void createSystemTrayApplication();

	public:
		SystemTray( QObject *_parent = nullptr );

	public slots:
		void onActivated( QSystemTrayIcon::ActivationReason reason );
};

#endif // SYSTEMTRAY_H
