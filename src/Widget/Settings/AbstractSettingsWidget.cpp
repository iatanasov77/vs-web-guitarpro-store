#include "AbstractSettingsWidget.h"

#include "Application/VsSettings.h"

AbstractSettingsWidget::AbstractSettingsWidget( QWidget *parent ) : QWidget( parent )
{
	settings	= VsSettings::instance();
}
