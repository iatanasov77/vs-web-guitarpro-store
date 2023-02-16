#include "SystemTray.h"

#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QGuiApplication>
#include <QScreen>
#include <QDialog>
#include "Application/VsAuth.h"
#include "Dialog/UserLoginDialog.h"

SystemTray::SystemTray( QObject *parent )
{
	Q_UNUSED( parent );

	if ( ! VsAuth::instance()->isLoggedIn() ) {
		loginToWebGuitarPro();
	}

	createSystemTrayApplication();
}

void SystemTray::loginToWebGuitarPro()
{
	UserLoginDialog *dlg	= new UserLoginDialog();
	dlg->setModal( true );

	if ( dlg->exec() == QDialog::Accepted )
	{
		//qDebug() << "Open DB: " << dbPath;
	}
}

void SystemTray::createSystemTrayApplication()
{
	QPixmap oPixmap( 32,32 );
	oPixmap.load ( ":/Resources/icons/logo.png" );
	QIcon oIcon( oPixmap );

	trayIcon = new QSystemTrayIcon( oIcon );
	trayIcon->setVisible( true );
	trayIcon->showMessage( tr( "WebGuitarPro Store" ), tr( "WebGuitarPro Store is Started !!!" ), QSystemTrayIcon::Information, 3000 );
	trayIcon->setToolTip( tr( "WebGuitarPro Store" ) );

	sysTrayMenu	= new SystemTrayMenu();
	sysTrayMenu->setWindowFlags( Qt::Popup );

	connect(
		trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		this, SLOT( onActivated( QSystemTrayIcon::ActivationReason ) )
	);
}

void SystemTray::onActivated( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason );

	QRect rect	= trayIcon->geometry();
	if ( rect.x() == 0 && rect.y() == 0 ) {
		QRect screenrect = QGuiApplication::primaryScreen()->availableGeometry();
		sysTrayMenu->move( screenrect.right() - sysTrayMenu->width(), screenrect.bottom() - sysTrayMenu->height() );
	} else {
		sysTrayMenu->move( rect.x(), rect.y() );
	}

	sysTrayMenu->show();
}
