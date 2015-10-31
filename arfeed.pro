TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
CONFIG -= qt
LIBS += -L/usr/local/lib -lssl -lcrypto -lxml2 -I/usr/include/libxml2 -lm

SOURCES += main.cpp \
    downloader.cpp

HEADERS += \
    downloader.hpp \
    gadgets.h \
    test.h

