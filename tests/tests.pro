QT          += testlib gui
CONFIG      += qt console c++11
CONFIG      -= app_bundle

TEMPLATE    = app

INCLUDEPATH += ../src

SOURCES     += \
    TestCMLP.cpp \
    ../src/CMLP.cpp \
    ../src/CNeurone.cpp \
    ../src/CNeuroneRelu.cpp \
    ../src/CNeuroneLineaire.cpp \
    ../src/CCapteur.cpp

HEADERS     += \
    TestCMLP.h
