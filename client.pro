QT += core dbus
QT -= gui
QT += widgets
CONFIG += c++11

TARGET = ClientService
TEMPLATE = app
SOURCES += client.cpp

LIBS += -L/path/to/SharingService -lSharingService  
INCLUDEPATH += /path/to/SharingService
