QT       += core gui widgets
CONFIG   += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    libcore/student.cpp \
    libcore/gamemanager.cpp \
    libcore/examsystem.cpp \
    ui/eventlog.cpp \
    ui/buffdialog.cpp \
    ui/recruitdialog.cpp \
    ui/examdialog.cpp

HEADERS += \
    mainwindow.h \
    libcore/student.h \
    libcore/buff.h \
    libcore/event.h \
    libcore/gamemanager.h \
    libcore/examsystem.h \
    ui/eventlog.h \
    ui/buffdialog.h \
    ui/recruitdialog.h \
    ui/examdialog.h

INCLUDEPATH += . ./libcore ./ui

TRANSLATIONS += \
    CampusWar_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target