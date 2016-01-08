#-------------------------------------------------
#
# Project created by QtCreator 2015-04-27T16:39:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Zrewidowany_simplex
TEMPLATE = app

CONFIG += c++11 static

SOURCES += main.cpp\
        rootwidget.cpp \
    stepsdialog.cpp \
    examplesdialog.cpp \
    matrix.cpp \
    qcustomplot.cpp

HEADERS  += rootwidget.h \
    stepsdialog.h \
    qvarlabel.h \
    qvaredit.h \
    examplesdialog.h \
    exercises.h \
    qseparator.h \
    matrix.h \
    qcustomplot.h
