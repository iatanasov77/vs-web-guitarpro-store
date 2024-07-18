#ifndef WIDGET_SETTINGS_WIDGET_GENERAL_H
#define WIDGET_SETTINGS_WIDGET_GENERAL_H

#include <QWidget>

#include "AbstractSettingsWidget.h"

namespace Ui {
	class SettingsWidgetGeneral;
}

class SettingsWidgetGeneral : public AbstractSettingsWidget
{
    Q_OBJECT

	public:
		explicit SettingsWidgetGeneral( QWidget *parent = 0 );
		~SettingsWidgetGeneral();

		virtual void apply();

	protected:
		// this event is called, when a new translator is loaded or the system language is changed
		void changeEvent( QEvent* );

	private:
		void init();

	    Ui::SettingsWidgetGeneral* ui;
};

#endif // WIDGET_SETTINGS_WIDGET_GENERAL_H
