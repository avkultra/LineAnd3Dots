QT -= gui
QT += core testlib

TARGET = geometry_tests
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++17

TEMPLATE = app

GTEST_PATH = $$PWD/../googletest


INCLUDEPATH += $$GTEST_PATH/googletest/include
INCLUDEPATH += $$GTEST_PATH/googletest
INCLUDEPATH += $$GTEST_PATH/googlemock/include
INCLUDEPATH += $$PWD/../src

SOURCES += \
    $$PWD/../src/Point/point.cpp

HEADERS += \
    test_utils.h \
    $$PWD/../src/Point/point.h \
    $$PWD/../src/const.h

SOURCES += \
    main.cpp \
    test_geometry.cpp \
    test_simciclepath.cpp \
    test_findline.cpp


SOURCES += \
    $$GTEST_PATH/googletest/src/gtest-all.cc


unix
{
    LIBS += -lpthread
}
