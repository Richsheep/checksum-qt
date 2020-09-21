#-------------------------------------------------
#
# Project created by QtCreator 2020-04-30T16:42:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = md5sum-qt
TEMPLATE = app

CONFIG += C++11
QMAKE_CXXFLAGS += -std=c++0x

unix {
    QMAKE_LFLAGS += -no-pie
}

SOURCES += main.cpp\
        mainwindow.cpp \
    md5.cpp

HEADERS  += mainwindow.h \
    md5.h

FORMS    +=
