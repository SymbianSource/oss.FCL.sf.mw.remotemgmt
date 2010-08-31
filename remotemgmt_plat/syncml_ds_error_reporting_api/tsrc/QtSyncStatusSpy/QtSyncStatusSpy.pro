TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
LIBS += -lxqsettingsmanager \
	-lxqserviceutil
INCLUDEPATH += \epoc32\include\platform\mw
HEADERS += qtsyncstatuslog.h \
    qtsyncstatuslogview.h \
    qtsyncstatusmodel.h \
    qtsyncstatusview.h
SOURCES += qtsyncstatuslog.cpp \
    qtsyncstatuslogview.cpp \
    qtsyncstatusmodel.cpp \
    qtsyncstatusview.cpp \
    main.cpp \
    QtSyncStatusSpy_reg.rss
FORMS += 
RESOURCES += 
symbian:TARGET.UID3 = 0x2001FDF3
TARGET.CAPABILITY = ALL \
    -TCB
        