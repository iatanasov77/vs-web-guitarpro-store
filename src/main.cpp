/****************************************************************
**
** WebGuitarProStore
**
****************************************************************/
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QSystemTrayIcon>

#include "Widget/SystemTrayMenu.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QPixmap oPixmap( 32,32 );
	oPixmap.load ( ":/Resources/icons/logo.png" );
	QIcon oIcon( oPixmap );

	QSystemTrayIcon *trayIcon = new QSystemTrayIcon( oIcon );
	qDebug() << trayIcon->isSystemTrayAvailable();
	trayIcon->setVisible( true );

	trayIcon->showMessage( "Test Message", "Text", QSystemTrayIcon::Information, 1000 );

	SystemTrayMenu sysTrayMenu;

	QObject::connect( trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
	        		&sysTrayMenu, SLOT( onActivated( QSystemTrayIcon::ActivationReason ) ) );

	return app.exec();
}
