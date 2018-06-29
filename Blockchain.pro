#-------------------------------------------------
#
# Project created by QtCreator 2018-06-27T09:28:50
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Blockchain
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
QMAKE_LFLAGS = -static

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    base64.cpp \
    sha256.cpp \
    server.cpp \
    client.cpp \
    thread.cpp

HEADERS += \
        mainwindow.h \
    base64.h \
    Block.h \
    Blockchain.h \
    File.h \
    sha256.h \
    server.h \
    client.h \
    thread.h

FORMS += \
        mainwindow.ui
