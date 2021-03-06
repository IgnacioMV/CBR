#-------------------------------------------------
#
# Project created by QtCreator 2018-02-13T06:29:07
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBR
TEMPLATE = app

LIBS += -L/usr/local/lib -larchive
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cbzcomic.cpp \
    jpeg.cpp \
    png.cpp \
    bmp.cpp \
    thumbnailworker.cpp \
    cbzextractworker.cpp \
    scaleimageworker.cpp \
    image.cpp \
    downsamplingworker.cpp \
    bilinearworker.cpp \
    bicubicworker.cpp

HEADERS += \
        mainwindow.h \
    image.h \
    comic.h \
    cbzcomic.h \
    jpeg.h \
    png.h \
    bmp.h \
    thumbnailworker.h \
    cbzextractworker.h \
    scaleimageworker.h \
    downsamplingworker.h \
    bilinearworker.h \
    scalingalgorithms.h \
    displaymode.h \
    extractworker.h \
    bicubicworker.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    images.qrc
