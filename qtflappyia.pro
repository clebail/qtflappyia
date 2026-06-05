QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Icône de l'exécutable Windows (l'icône de la fenêtre est définie dans mainwindow.ui)
win32:RC_ICONS = icon.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CCapteur.cpp \
    CGenetic.cpp \
    CNeurone.cpp \
    common.cpp \
    flappy.cpp \
    main.cpp \
    mainwindow.cpp \
    scenewidget.cpp \
    tuyau.cpp

HEADERS += \
    CCapteur.h \
    CGenetic.h \
    CNeurone.h \
    common.h \
    flappy.h \
    mainwindow.h \
    scenewidget.h \
    tuyau.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    qtflappyia_fr_FR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qtflappyia.qrc
