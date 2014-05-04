#-------------------------------------------------
#
# Project created by QtCreator 2013-12-14T14:35:31
#
#-------------------------------------------------

QT += core gui widgets sql

CONFIG += c++11

TARGET = FOE
TEMPLATE = app

QTPLUGIN = qsqlmysql

SOURCES += main.cpp\
        foe_main.cpp \
    foeclan.cpp \
    foedatamanger.cpp \
    foeuser.cpp \
    foegoods.cpp \
    foeusereditdlg.cpp \
    foeages.cpp \
    foeoverviewmodel.cpp \
    foeconnectiondetails.cpp

HEADERS  += foe_main.h \
    foeclan.h \
    foedatamanger.h \
    foeuser.h \
    foegoods.h \
    foeusereditdlg.h \
    foeages.h \
    foeoverviewmodel.h \
    foeconnectiondetails.h

FORMS    += foe_main.ui \
    foeusereditdlg.ui \
    foeconnectiondetails.ui \
    foe_clan.ui

TRANSLATIONS = "foe_da_DK.ts"

RESOURCES += \
    goods.qrc


lupdate.commands = lupdate FOE.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS

lrelease.commands = lrelease FOE.pro
lrelease.depends = lupdate

#ltranslate.commands = $(COPY) *.qm ${DESTDIR}
#ltranslate.depends = lrelease

QMAKE_EXTRA_TARGETS += lupdate lrelease

POST_TARGETDEPS += lrelease
