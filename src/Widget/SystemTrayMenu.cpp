#include "SystemTrayMenu.h"

#include <QtWidgets/QMenu>
#include <QAction>

SystemTrayMenu::SystemTrayMenu( QObject *_parent )
{
	//init();
}

void SystemTrayMenu::init()
{
	QMenu* menu = new QMenu(); // want to get a context menu from system tray
	QAction * viewWindow = new QAction( "Another Action" );
	QAction * quitAction = new QAction( "Exit" );

	connect( viewWindow, SIGNAL( triggered() ), this, SLOT( show() ) );
	connect( quitAction, SIGNAL( triggered() ), this, SLOT( close() ) );

	menu->addAction( viewWindow );
	menu->addAction( quitAction );
}

void SystemTrayMenu::onActivated( QSystemTrayIcon::ActivationReason reason )
{

}
