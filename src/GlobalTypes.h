#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

enum HttpRequestVarLayout {NOT_SET, ADDRESS, URL_ENCODED, MULTIPART};
enum HttpRequestType { REQUEST_TYPE_HTTP , REQUEST_TYPE_JSON };

#include <QMap>
#include <QString>
#include <QMetaType>

Q_DECLARE_METATYPE( HttpRequestType )

static QMap<QString, QString> SettingsGroups {
	{"authentication", "Authentication"},
};

#endif	// GLOBALTYPES_H
