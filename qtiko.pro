TARGET = qtiko
VERSION = 0.1.2 #major.minor.patch
APPNAME = $$TARGET

DEFINES += \
    VERSION=\\\"$$VERSION\\\" \
    APPNAME=\\\"$$APPNAME\\\"

DEFINES += FORCE_AMBIANCE

TEMPLATE = app

QT += core gui network phonon dbus sql

INCLUDEPATH += $$PWD

include($$PWD/solutions/singleapplication/singleapplication.pri)

SOURCES += \
    main.cpp \
    $$PWD/core/*.cpp \
    $$PWD/models/*.cpp \
    $$PWD/widgets/*.cpp \
    $$PWD/tabs/*.cpp \
    $$PWD/base/*.cpp

HEADERS += \
    $$PWD/core/*.h \
    $$PWD/interfaces/*.h \
    $$PWD/models/*.h \
    $$PWD/widgets/*.h \
    $$PWD/tabs/*.h \
    $$PWD/types/*.h \
    $$PWD/base/*.h

unix {
    SOURCES += $$PWD/linux/*.cpp
    HEADERS += $$PWD/linux/*.h
}

FORMS += \
    $$PWD/core/*.ui \
    $$PWD/tabs/*.ui

RESOURCES += \
    resources/qtiko.qrc

OTHER_FILES += \
    $$PWD/resources/extras-qtiko.desktop  \
    $$PWD/resources/stylesheets/*.css

unix {
    INCLUDEPATH += /usr/include/taglib
    LIBS += -ltag
} else {
    INCLUDEPATH += /usr/include/taglib
    LIBS += -ltag
}

unix {
    target.path = /opt/extras.ubuntu.com/qtiko/bin
    INSTALLS += target

    desktop.path = /usr/share/applications
    desktop.files += resources/extras-qtiko.desktop
    INSTALLS += desktop

    DATADIR = /opt/extras.ubuntu.com/qtiko/share

    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += resources/icons/64/qtiko.png
    INSTALLS += icon64

    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += resources/icons/32/qtiko.png
    INSTALLS += icon32
}
