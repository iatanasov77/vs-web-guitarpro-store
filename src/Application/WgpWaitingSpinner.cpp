#include "WgpWaitingSpinner.h"

WgpWaitingSpinner::WgpWaitingSpinner( QWidget *parent ) : WaitingSpinnerWidget( parent )
{
	this->setRoundness( 70.0 );
	this->setMinimumTrailOpacity( 15.0 );
	this->setTrailFadePercentage( 70.0 );
	this->setNumberOfLines( 12 );
	this->setLineLength( 10 );
	this->setLineWidth( 5 );
	this->setInnerRadius( 10 );
	this->setRevolutionsPerSecond( 1 );
	this->setColor( QColor( 81, 4, 71 ) );
}
