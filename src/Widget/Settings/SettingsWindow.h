#ifndef WIDGET_SETTINGS_WINDOW_H
#define WIDGET_SETTINGS_WINDOW_H

#include <QWidget>
#include <QSortFilterProxyModel>

#include "AbstractSettingsWidget.h"

namespace Ui {
	class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

	public:
		explicit SettingsWindow( QWidget *parent = 0 );
		~SettingsWindow();

	signals:
		void speakerSettingsUpdated();

	protected:
		// this event is called, when a new translator is loaded or the system language is changed
		void changeEvent( QEvent* );

	private:
		void initWidgets();
		void initMenu();
		void showWidget( QString widgetId, QString widgetTitle );

	    Ui::SettingsWindow* ui;
	    QMap<QString, AbstractSettingsWidget*> widgets;
	    QSortFilterProxyModel* menuModel;

	private slots:
		void showSettings( const QModelIndex& index );
		void applySettings();
		void saveAndExitSettings();
		void cancelSettings();
		void filterMenu( const QString& filterString );
};

#endif // WIDGET_SETTINGS_WINDOW_H
