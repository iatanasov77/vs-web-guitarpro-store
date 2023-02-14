#ifndef VS_APPLICATION_H
#define VS_APPLICATION_H

#include <QDir>
#include <QTranslator>
#include "ApiManager/HttpRequestWorker.h"

class VsApplication
{
	private:
		VsApplication();
	    static VsApplication* createInstance();
	    static VsApplication *_instance;

	    QTranslator m_translator; // contains the translations for this application
		QTranslator m_translatorQt; // contains the translations for qt
		QString m_currLang; // contains the currently loaded language
		QString m_langPath; // Path of language files. This is always fixed to /languages.

		QString m_apiUrl;
		HttpRequestWorker *m_httpRequestWorker;

		void switchTranslator( QTranslator& translator, const QString& filename );

	public:
	    static VsApplication* instance();

		static QDir dataPath();
		static QString appVersion();
		static QString appBuildTime();

		QMap<QString, QString> languages();
		void loadLanguage( const QString& rLanguage );
		QString appAboutBody();

		QString apiUrl();
		HttpRequestWorker *httpRequestWorker();
};

#endif // VS_APPLICATION_H
