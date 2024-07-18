#ifndef ABSTRACT_SETTINGS_WIDGET_H
#define ABSTRACT_SETTINGS_WIDGET_H

#include <QWidget>

#include "Application/VsSettings.h"

class AbstractSettingsWidget : public QWidget
{
    Q_OBJECT

	public:
		explicit AbstractSettingsWidget( QWidget *parent = 0 );

		virtual void apply() = 0;

	protected:
		VsSettings* settings;
};

#endif // ABSTRACT_SETTINGS_WIDGET_H
