QT += core dbus
QT -= gui
QT += widgets
CONFIG += c++11

TARGET = ClientService
TEMPLATE = app
SOURCES += client.cpp

LIBS += -L/home/lilycherly/Work/SharingService -lSharingService  
INCLUDEPATH += /home/lilycherly/Work/SharingService
