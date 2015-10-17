TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    interpreter.cpp \
    api.cpp \
    catalog_manager.cpp \
    record_manager.cpp \
    index_manager.cpp \
    buffer_manager.cpp \
    dbms.cpp \
    exception.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    interpreter.h \
    api.h \
    catalog_manager.h \
    record_manager.h \
    index_manager.h \
    buffer_manager.h \
    dbms.h \
    exception.h

