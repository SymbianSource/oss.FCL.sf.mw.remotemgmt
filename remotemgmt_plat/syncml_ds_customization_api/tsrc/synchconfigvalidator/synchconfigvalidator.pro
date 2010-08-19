TEMPLATE = app
TARGET = synchconfigvalidator
QT += core \
    gui
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
symbian:LIBS += -letel3rdparty \
    -lsysutil \
    -lcentralrepository \
    -lsyncmlclientapi \
    -lecom \
    -ledbms \
    -lefsrv \
    -lcharconv
HEADERS += synchconfigvalidator.loc \
    customdatasynchconfigvalidator.h \
    customdatasynchconfigvalidatorenginewrapper.h \
    customdatasynchconfigvalidatorengine_p.h \
    customdatasynchdeviceinfo.h
SOURCES += synchconfigvalidator_reg.rss \
    main.cpp \
    customdatasynchconfigvalidator.cpp \
    customdatasynchconfigvalidatorenginewrapper.cpp \
    customdatasynchconfigvalidatorengine_p.cpp \
    customdatasynchdeviceinfo.cpp
FORMS += synchconfigvalidator.ui
RESOURCES += 
symbian:TARGET.UID3 = 0x2002DC7B
symbian:TARGET.CAPABILITY = All \
    -TCB
symbian { 
    SYNCHCONFIGTOOL_IBY_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    BLD_INF_RULES.prj_exports += "$$SYNCHCONFIGTOOL_IBY_DIR/synchconfigvalidator.iby CUSTOMER_APP_LAYER_IBY_EXPORT_PATH(synchconfigvalidator.iby)"
}
