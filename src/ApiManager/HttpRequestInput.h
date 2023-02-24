#ifndef HTTPREQUESTINPUT_H
#define HTTPREQUESTINPUT_H

#include <QMap>
#include <QList>
#include <QString>

#include "GlobalTypes.h"
#include "HttpRequestInputFileElement.h"

class HttpRequestInput
{
	public:
		QString httpUrl;
		QString httpMethod;
		HttpRequestType requestType;
		HttpRequestVarLayout varLayout;
		QMap<QString, QString> vars;
		QList<HttpRequestInputFileElement> files;

		HttpRequestInput();
		HttpRequestInput( QString v_httpUrl, QString v_httpMethod );
		void initialize();
		void addVar( QString key, QString value );
		void addFile( QString localFilename, QString requestFilename, QString mimeType );
};

#endif // HTTPREQUESTINPUT_H
