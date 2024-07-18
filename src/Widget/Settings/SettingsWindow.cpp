#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include <QDebug>
#include <QFile>

#include "GlobalTypes.h"
#include "AbstractSettingsWidget.h"
#include "SettingsWidgetGeneral.h"

#include "Model/SettingsTreeModel.h"

SettingsWindow::SettingsWindow( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::SettingsWindow )
{
    ui->setupUi( this );
    setWindowIcon( QIcon( ":/Resources/icons/settings.svg" ) );

	ui->splitter->setSizes( QList<int>() << 200 << 700 );

	initMenu();
    initWidgets();
    showSettings( ui->treeView->model()->index( 0, 1 ) );

    connect( ui->leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterMenu( const QString& ) ) );
    connect( ui->treeView, SIGNAL( clicked( const QModelIndex& ) ), this, SLOT( showSettings( const QModelIndex& ) ) );
	connect( ui->btnApply, SIGNAL( released() ), this, SLOT( applySettings() ) );
    connect( ui->btnSaveAndExit, SIGNAL( released() ), this, SLOT( saveAndExitSettings() ) );
    connect( ui->btnCancel, SIGNAL( released() ), this, SLOT( cancelSettings() ) );
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::initWidgets()
{
	widgets["general"]	= new SettingsWidgetGeneral();

	foreach ( AbstractSettingsWidget* wdg, widgets ) {
		ui->mainWidget->addWidget( wdg );
	}
}

void SettingsWindow::initMenu()
{
	QFile file( ":/Resources/settings_menu/menu.xml" );
	file.open( QIODevice::ReadOnly );
	SettingsTreeModel* sourceModel	= new SettingsTreeModel( file.readAll(), this );
	menuModel				= new QSortFilterProxyModel( this );

	menuModel->setSourceModel( sourceModel );
	menuModel->setFilterKeyColumn( 0 );	// Filter is applyed to the Title Column

	ui->treeView->setModel( menuModel );
	ui->treeView->hideColumn( 1 );	// Id Column for this model
}

void SettingsWindow::showSettings( const QModelIndex &index )
{
	QString title	= ui->treeView->model()->data( index.siblingAtColumn( 0 ) ).toString();
	QString id		= ui->treeView->model()->data( index.siblingAtColumn( 1 ) ).toString();
	//qDebug() << "TreeView ID: " << id;
	showWidget( id, title );
}

void SettingsWindow::showWidget( QString widgetId, QString widgetTitle )
{
	auto wdg	= widgets.find( widgetId );
	if( wdg != widgets.end() ) {
		ui->settingsTitle->setText( widgetTitle );
		ui->mainWidget->setCurrentWidget( wdg.value() );
	} else {
		qDebug() << "Settings Unimpemented";
	}
}

void SettingsWindow::applySettings()
{
	AbstractSettingsWidget*	w	= qobject_cast<AbstractSettingsWidget*>( ui->mainWidget->currentWidget() );
	w->apply();
}

void SettingsWindow::saveAndExitSettings()
{
	applySettings();
	hide();
}

void SettingsWindow::cancelSettings()
{
	hide();
}

void SettingsWindow::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::LanguageChange )
    {
        ui->retranslateUi( this );
    }

    // remember to call base class implementation
    QWidget::changeEvent( event );
}

void SettingsWindow::filterMenu( const QString& filterString )
{
	//	setFilterWildcard(const QString &pattern)
	//	setFilterFixedString(const QString &pattern)
	menuModel->setFilterFixedString( filterString );
}
