#include "CommandState.h"

CommandState::CommandState( QObject *parent ) : QObject( parent )
{
	commandId		= "";
	requestName		= "";

	response 		= "";
	downloadedFile	= "";
	errorType		= QNetworkReply::NoError;
	errorStr 		= "";
}
