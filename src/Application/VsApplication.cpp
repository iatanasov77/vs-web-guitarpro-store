#include "VsApplication.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <QDateTime>
#include <QSettings>
#include <QVersionNumber>

#include "Application/VsSettings.h"

/*
 * This one did the trick!
 * ------------------------
 * But I want to do this: https://stackoverflow.com/questions/46172607/qt-singleton-implementation
 */
VsApplication *VsApplication::_instance = 0;

VsApplication::VsApplication()
{
	m_currLang	= "en";

	m_apiUrl	= "http://wgp.lh/api/";
	m_httpRequestWorker = new HttpRequestWorker();
}

VsApplication *VsApplication::createInstance()
{
    return new VsApplication();
}

VsApplication *VsApplication::instance() {
	if ( ! _instance ) {
		_instance = createInstance();
	}

	return _instance;
	//return Singleton<VsApplication>::instance( VsApplication::createInstance() );
}

QDir VsApplication::dataPath()
{
	//QDir dataPath{ QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) };
	QDir dataPath{ QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation ) };
	//QDir dataPath{ QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) };

	if( ! dataPath.exists() )
		dataPath.mkpath( "." );

	return dataPath;
}

QString VsApplication::appVersion()
{
	QString data;
	QString fileName( ":/VERSION" );

	QFile file( fileName );
	if( ! file.open( QIODevice::ReadOnly ) ) {
		data = QString( "Cannot open VERSION file!" );
	} else {
		data = file.readAll();
	}

	file.close();
	return data;
}

QString VsApplication::appBuildTime()
{
	QString build = QString( "%1 %2" )
					.arg( QLocale( QLocale::C ).toDate( QString( __DATE__ ).simplified(), QLatin1String( "MMM d yyyy" ) ).toString( "dd.MM.yyyy" ) )
					.arg( QString( "%1%2:%3%4" )
						.arg(__TIME__[0])
						.arg(__TIME__[1])
						.arg(__TIME__[3])
						.arg(__TIME__[4])
					);

	return build;
}

QMap<QString, QString> VsApplication::languages()
{
	QMap<QString, QString> languages;

	/*
	 *  Always translated in Bulgarian
	 *
	languages["en"]	= QObject::tr( "English" );
	languages["bg"]	= QObject::tr( "Bulgarian" );
	*/
	languages["en"]	= "English";
	languages["bg"]	= "Bulgarian";

	return languages;
}

void VsApplication::loadLanguage( const QString& rLanguage )
{
	if( m_currLang != rLanguage ) {
		m_currLang		= rLanguage;
		QLocale locale	= QLocale( m_currLang );

		QLocale::setDefault( locale );
		QString languageName	= QLocale::languageToString( locale.language() );
		switchTranslator( m_translator, QString( "QVocabulary_%1.qm" ).arg( rLanguage ) );
		switchTranslator( m_translatorQt, QString( "qt_%1.qm" ).arg( rLanguage ) );

		VsSettings::instance()->setValue( "language", m_currLang, "General" );
	}
}

void VsApplication::switchTranslator( QTranslator& translator, const QString& filename )
{
	// remove the old translator
	qApp->removeTranslator( &translator );
	// load the new translator
	QString path = QApplication::applicationDirPath();
	path.append( "/translations/" );

	//Here Path and Filename has to be entered because the system didn't find the QM Files else
	if( translator.load( path + filename ) ) {
		qDebug() << path + filename;
		qApp->installTranslator( &translator );
	}
}

QString VsApplication::appAboutBody()
{
	QString data;
	QString fileName	= QString( ":/Resources/html/about_%1.html" ).arg( m_currLang );

	QFile file( fileName );
	if( ! file.open( QIODevice::ReadOnly ) ) {
		data = QString( "Cannot open AboutBody file!" );
	} else {
		data = file.readAll();
	}

	file.close();
	return data;
}

QString VsApplication::apiUrl()
{
	return m_apiUrl;
}

HttpRequestWorker *VsApplication::httpRequestWorker()
{
	return m_httpRequestWorker;
}
