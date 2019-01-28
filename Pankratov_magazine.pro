#-------------------------------------------------
#
# Project created by QtCreator 2018-09-21T10:05:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pankratov_magazine
TEMPLATE = app

CONFIG += sdk_no_version_check

SOURCES += \
    FlyObjects/MathClasses/angle.cpp \
    FlyObjects/la.cpp \
    FlyObjects/rocket.cpp \
    FlyObjects/runnabletask.cpp \
    GUI/drawingobject.cpp \
    GUI/graphics_view_zoom.cpp \
    GUI/graphicsview.cpp \
    GUI/mainwindow.cpp \
    GUI/object_tracer.cpp \
    GUI/runnablegraphictask.cpp \
    Phys/physic.cpp \
    main.cpp

HEADERS  += \
    FlyObjects/MathClasses/angle.h \
    FlyObjects/la.h \
    FlyObjects/mathstructure.h \
    FlyObjects/rocket.h \
    FlyObjects/runnabletask.h \
    GUI/drawingobject.h \
    GUI/graphics_view_zoom.h \
    GUI/graphicsview.h \
    GUI/mainwindow.h \
    GUI/object_tracer.h \
    GUI/runnablegraphictask.h \
    Phys/physic.h

INCLUDEPATH += $$PWD/FlyObjects \
    $$PWD/Phys \
    $$PWD/GUI  \
    $$PWD/FlyObjects/MathClasses

FORMS    += mainwindow.ui

