#include "SystemTrayMenu.h"
#include "ui_SystemTrayMenu.h"

#include <QCoreApplication>
#include <QToolButton>

SystemTrayMenu::SystemTrayMenu( QWidget *parent ) :
	QWidget( parent ),
	ui( new Ui::SystemTrayMenu )
{
	ui->setupUi( this );

	init();
}

SystemTrayMenu::~SystemTrayMenu()
{
    delete ui;
}

void SystemTrayMenu::init()
{
	connect(
		ui->btnQuitApplication, &QToolButton::clicked,
		QCoreApplication::instance(), &QCoreApplication::quit,
		Qt::QueuedConnection
	);
}
