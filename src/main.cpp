/****************************************************************
**
** WebGuitarProStore
**
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <QtWidgets/QApplication>

#include "SystemTray.h"

void logToFile( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    QString message	= qFormatLogMessage( type, context, msg );

    static FILE *f	= fopen( "log.txt", "w" );
    fprintf( f, "%s\n", qPrintable( message ) );
    fflush( f );
}

int main( int argc, char **argv )
{
	#ifdef QT_DEBUG
		//qInstallMessageHandler( logToFile );
	#endif
    QApplication app( argc, argv );

	SystemTray sysTray;

	return app.exec();
}
