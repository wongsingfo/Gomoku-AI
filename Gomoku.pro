#-------------------------------------------------
#
# Project created by QtCreator 2016-03-19T16:32:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gomoku
TEMPLATE = app


SOURCES += main.cpp\
    AI/aigo.cpp \
        mainwindow.cpp \
    chessman.cpp \
    chessboard.cpp \
    ai_thread.cpp \
    AI/aisample.cpp \
    highlight.cpp \
    AI/aialphabeta.cpp

HEADERS  += mainwindow.h \
    AI/aigo.h \
    constants.h \
    chessman.h \
    chessboard.h \
    ai_thread.h \
    AI/aisample.h \
    highlight.h \
    AI/aialphabeta.h

RESOURCES += \
    image.qrc
