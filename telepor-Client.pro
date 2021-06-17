#-------------------------------------------------
#
# Project created by QtCreator 2021-05-29T18:10:20
#
#-------------------------------------------------

QT       += core gui sql network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = telepor-Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
            clientStuff.cpp
HEADERS  += mainwindow.h \ \
    data.h\
   sms.h\
   clientStuff.h

FORMS    += mainwindow.ui
