TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    v4l2serial.c

HEADERS += \
    v4l2serial.h

