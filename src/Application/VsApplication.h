#ifndef VS_APPLICATION_H
#define VS_APPLICATION_H

#include <QDir>
#include <QLocale>
#include <QTranslator>

class VsApplication
{
	private:
		VsApplication();
	    static VsApplication* createInstance();
	    static VsApplication *_instance;

	    QTranslator m_translator; // contains the translations for this application
		QTranslator m_translatorQt; // contains the translations for qt
		QLocale defaultLocale; // contains the currently loaded language
		QString m_langPath; // Path of language files. This is always fixed to /languages.

		QString m_apiUrl;

		void switchTranslator( QTranslator& translator, const QString& filename );

	public:
	    static VsApplication* instance();

		static QDir dataPath();
		static QString appVersion();
		static QString appBuildTime();

		QMap<QString, QString> languages();
		void loadLanguage( const QLocale& locale );
		QString appAboutBody();

		void makeStartupApp();
		void removeStartupApp();
		QString apiUrl();
};

#endif // VS_APPLICATION_H
