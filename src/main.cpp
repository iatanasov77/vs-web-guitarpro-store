/****************************************************************
**
** WebGuitarProStore
**
****************************************************************/
#include <QtWidgets/QApplication>

#include "SystemTray.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

	SystemTray sysTray;

	return app.exec();
}
