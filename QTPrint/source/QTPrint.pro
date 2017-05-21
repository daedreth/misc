QT += core
QT -= gui

CONFIG += c++11

TARGET = qtprint
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

target.path = /usr/bin
INSTALLS += target
