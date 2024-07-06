#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

#include <QMap>
#include <QString>
#include <QMetaType>

enum FileSystemObject { OBJECT_TABLATURE, OBJECT_CATEGORY };
enum HttpRequestVarLayout {NOT_SET, ADDRESS, URL_ENCODED, MULTIPART};
enum HttpRequestType { REQUEST_TYPE_HTTP , REQUEST_TYPE_JSON };
Q_DECLARE_METATYPE( HttpRequestType )

static QMap<QString, QString> SettingsGroups {
	{"authentication", "Authentication"},
};

static QMap<QString, QString> SettingsKeys {
	{"LANGUAGE", "language"},
	{"AUTH_PAYLOAD", "authPayload"},
	{"REFRESH_TOKEN", "refreshToken"}
};

static QMap<QString, QString> HttpRequests {
	{"LOGIN_REQUEST", "Login Request"},
	{"GET_MYTABLATURES_REQUEST", "Get MyTablatures Request"},
	{"GET_MYCATEGORIES_REQUEST", "Get MyCategories Request"},
	{"GET_MYTABLATURESUNCATEGORIZED_REQUEST", "Get MyTablaturesUncategorized Request"},

	{"CREATE_TABLATURE_CATEGORY_REQUEST", "Create TablatureCategory Request"},
	{"UPDATE_TABLATURE_CATEGORY_REQUEST", "Update TablatureCategory Request"},
	{"DELETE_TABLATURE_CATEGORY_REQUEST", "Delete TablatureCategory Request"},

	{"CREATE_TABLATURE_REQUEST", "Create Tablature Request"},
	{"UPDATE_TABLATURE_REQUEST", "Update Tablature Request"},
	{"DELETE_TABLATURE_REQUEST", "Delete Tablature Request"}
};

#endif	// GLOBALTYPES_H
