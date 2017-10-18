SOURCES += main.cpp \
           LifetimeMainWindow.cpp \
           lifetimecore.cpp \
           lifetimeconfig.cpp \
           lifetimeprotocol.cpp \
           lifetimethememanager.cpp \
 lifetimeimportexportdialog.cpp
TEMPLATE = app
QT += widgets
CONFIG += thread \
          qt \
	  warn_off
DESTDIR = ../bin
TARGET = lifetime
RESOURCES = application.qrc
HEADERS += LifetimeMainWindow.h \
lifetimecore.h \
lifetimeconfig.h \
lifetimeprotocol.h \
lifetimethememanager.h \
 lifetimeimportexportdialog.h
FORMS += lifetimemainwidget.ui \
lifetimeprotocolview.ui \
lifetimesettingsview.ui \
 lifetimeimportexportdialog.ui
SUBDIRS += img

QMAKE_CXXFLAGS_DEBUG += -g \
-O0
CONFIG -= warn_on

