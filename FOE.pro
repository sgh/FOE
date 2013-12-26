#-------------------------------------------------
#
# Project created by QtCreator 2013-12-14T14:35:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FOE
TEMPLATE = app
QT += core sql gui

QTPLUGIN = qsqlmysql

SOURCES += main.cpp\
        foe_main.cpp \
    foedatamanger.cpp \
    foeuser.cpp \
    foeproduct.cpp \
    foeusereditdlg.cpp \
    foeages.cpp \
    foeoverviewmodel.cpp \
    foeconnectiondetails.cpp

HEADERS  += foe_main.h \
    foedatamanger.h \
    foeuser.h \
    foeproduct.h \
    foeusereditdlg.h \
    foeages.h \
    foeoverviewmodel.h \
    foeconnectiondetails.h

FORMS    += foe_main.ui \
    foeusereditdlg.ui \
    foeconnectiondetails.ui

RESOURCES += \
    products.qrc
