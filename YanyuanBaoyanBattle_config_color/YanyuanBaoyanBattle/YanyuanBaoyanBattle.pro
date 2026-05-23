QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = YanyuanBaoyanBattle

INCLUDEPATH += . \
    libcore \
    ui

SOURCES += \
    main.cpp \
    libcore/student.cpp \
    libcore/buff.cpp \
    libcore/event.cpp \
    libcore/examsystem.cpp \
    libcore/gamemanager.cpp \
    ui/eventlog.cpp \
    ui/buffdialog.cpp \
    ui/recruitdialog.cpp \
    ui/examdialog.cpp \
    ui/mainwindow.cpp

HEADERS += \
    libcore/student.h \
    libcore/buff.h \
    libcore/event.h \
    libcore/gameconfig.h \
    libcore/examsystem.h \
    libcore/gamemanager.h \
    ui/eventlog.h \
    ui/buffdialog.h \
    ui/recruitdialog.h \
    ui/examdialog.h \
    ui/mainwindow.h

RESOURCES += resources.qrc
