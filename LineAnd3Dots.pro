QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

UI_DIR = $$PWD/build/ui

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/GraphicsViewEx/graphicsviewex.cpp \
    src/Point/point.cpp \
    src/MainWindow/mainwindow.cpp

HEADERS += \
    src/GraphicsViewEx/graphicsviewex.h \
    src/MainWindow/const.h \
    src/Point/point.h \
    src/MainWindow/mainwindow.h

FORMS += \
    src/MainWindow/mainwindow.ui

TRANSLATIONS += \
    translations/LineAnd3Dots_ru.ts \
    translations/LineAnd3Dots_en.ts

RESOURCES += \
    src/MainWindow/resources.qrc

INCLUDEPATH += $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

