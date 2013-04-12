INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/singleapplication.h \
    $$PWD/core/qtlocalpeer.h \
    $$PWD/core/qtlockedfile.h \
    $$PWD/core/qtsingleapplication.h \
    $$PWD/core/qtsinglecoreapplication.h

SOURCES += \
    $$PWD/core/qtlocalpeer.cpp \
    $$PWD/core/qtlockedfile.cpp \
    $$PWD/core/qtlockedfile_unix.cpp \
    $$PWD/core/qtlockedfile_win.cpp \
    $$PWD/core/qtsingleapplication.cpp \
    $$PWD/core/qtsinglecoreapplication.cpp
