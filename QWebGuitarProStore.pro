# Running in Debug Mode: qmake CONFIG+=debug
TARGET = WebGuitarProStore
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += core gui network

# Compile with:
# qmake-mingw CONFIG+=debug
CONFIG(debug, debug|release) {
    # For qDebug to work
	CONFIG += console
}

# QtWaitingSpinner
##########################################################
INCLUDEPATH += /usr/include/QtWaitingSpinner/include
LIBS += -L/usr/lib -lqtwaitingspinner

##########################################################
    
INCLUDEPATH += /projects/VS_WebGuitarPro_Store/src
DEFINES += QT_DEPRECATED_WARNINGS

###############################
# SOURCES
###############################
SOURCES += $$files(src/*.cpp, true)
HEADERS += $$files(src/*.h, true)
FORMS += $$files(Forms/*.ui, true)

RC_ICONS = logo.ico
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
