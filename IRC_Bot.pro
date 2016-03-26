#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T17:45:59
#
#-------------------------------------------------

QT       += core network widgets sql

#QT       -= gui

TARGET = IRC_Bot
#CONFIG   += console
CONFIG   -= app_bundle
QMAKE_LFLAGS += /FS
QMAKE_CXXFLAGS +=/FS
INCLUDEPATH += J:/Installs/development/c++/IRC_BOT
TEMPLATE = app


SOURCES += main.cpp \
    irc.cpp \
    ../LoadoutEditor/MainLibrary/HTTPdownloader.cpp \
    twitchhelper.cpp \
    eventscheduler.cpp \
    event.cpp \
    database.cpp \
    streamer.cpp \
    controller.cpp

HEADERS += \
    irc.h \
    ../LoadoutEditor/MainLibrary/HTTPdownloader.h \
    twitchhelper.h \
    eventscheduler.h \
    event.h \
    database.h \
    streamer.h \
    localdata.h \
    controller.h
