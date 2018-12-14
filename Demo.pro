#-------------------------------------------------
#
# Project created by QtCreator 2018-05-31T12:05:10
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo
TEMPLATE = app


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
    glsheet.cpp \
    vertex2D.cpp \
    transform.cpp \
    camera.cpp \
    optiondialog.cpp \
    layer.cpp \
    centralcontainer.cpp \
    paint.cpp \
    tool.cpp \
    layerbutton.cpp

HEADERS += \
        mainwindow.h \
    glsheet.h \
    vertex2D.h \
    transform.h \
    camera.h \
    optiondialog.h \
    layer.h \
    centralcontainer.h \
    paint.h \
    tool.h \
    layerbutton.h

FORMS += \
        mainwindow.ui \
    optiondialog.ui

LIBS += \
        -lOpengl32

DISTFILES += \
    square.vert \
    square.frag \
    squareOutline.vert \
    squareOutline.frag \
    circle.vert \
    circle.frag \
    background.vert \
    background.frag

RESOURCES += \
    resources.qrc
