# Running in Debug Mode: qmake CONFIG+=debug
TARGET = WebGuitarProStore
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += core gui

# Compile with:
# qmake-mingw CONFIG+=debug
CONFIG(debug, debug|release) {
    # For qDebug to work
	CONFIG += console
}

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /projects/VS_WebGuitarPro_Store/src

###############################
# SOURCES
###############################
SOURCES += $$files(src/*.cpp, true)
HEADERS += $$files(src/*.h, true)
FORMS += $$files(Forms/*.ui, true)

RC_ICONS = dictionary_transparent.ico
RESOURCES = WebGuitarProStore.qrc
TRANSLATIONS += $$files(translations/*.ts, true)

###############################
# BUILD
###############################
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
}
CONFIG(release, debug|release) {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
