

TEMPLATE = app
TARGET = 
DEPENDPATH += .
CONFIG += hb
LIBS += -lxqsettingsmanager
INCLUDEPATH += \epoc32\include\platform\mw

# Input
SOURCES += customlistview.cpp \
    main.cpp
HEADERS = customlistview.h

symbian:TARGET.UID3 = 0x2001FDF4
symbian:LIBS += -lsyncmlclientapi \
	-lcentralrepository
TARGET.CAPABILITY = ALL \
    -TCB
