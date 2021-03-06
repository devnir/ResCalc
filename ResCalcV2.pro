#-------------------------------------------------
#
# Project created by QtCreator 2014-10-02T09:33:26
#
#-------------------------------------------------

QT       += core gui
QT += network
QT +=xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ResCalcV2
TEMPLATE = app


SOURCES += main.cpp\
        rescalc.cpp \
    resourcedata.cpp \
    bonuses.cpp \
    corporation.cpp \
    settingsadd.cpp \
    trainsrepair.cpp \
    statistic.cpp \
    trainsheduler.cpp \
    getxmldata.cpp

HEADERS  += rescalc.h \
    resourcedata.h \
    bonuses.h \
    corporation.h \
    settingsadd.h \
    trainsrepair.h \
    statistic.h \
    trainsheduler.h \
    getxmldata.h

FORMS    += rescalc.ui \
    settingsadd.ui \
    trainsheduler.ui

RESOURCES += \
    res.qrc
RC_FILE += icon.rc

OTHER_FILES += \
    icon.rc
