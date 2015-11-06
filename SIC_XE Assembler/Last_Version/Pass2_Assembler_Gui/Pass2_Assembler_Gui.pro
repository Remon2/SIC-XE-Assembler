#-------------------------------------------------
#
# Project created by QtCreator 2014-05-31T15:03:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pass2_Assembler_Gui
TEMPLATE = app


SOURCES += \
    textedit.cpp \
    Tables.cpp \
    SICXEPass2.cpp \
    SICXEPass1.cpp \
    main.cpp \
    highlighter.cpp \
    Conversions.cpp \
    mainwindow.cpp

HEADERS  += \
    textedit.h \
    Tables.h \
    SICXEPass2.h \
    SICXEPass1.h \
    highlighter.h \
    Conversions.h \
    mainwindow.h

FORMS    += mainwindow.ui
