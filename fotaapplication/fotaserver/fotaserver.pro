# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: This is the project file defnition for fotaserver.

TEMPLATE = app
TARGET = fotaserver

DEPENDPATH += . \
    ./inc \
    ./src \
    ../inc \

INCLUDEPATH += . \
    ./inc \
    ../inc \
    ../cenrep \
    ../fmsclient/inc \

HEADERS +=     FotaServer.h \ 
    FotaDlMgrClient.h \
    FotaDlClient.h \
    FotaDB.h \
    FotaNetworkRegStatus.h \
    fotaserverPrivatePSKeys.h \
    FotaSrvDebug.h \
    FotasrvSession.h \
    fotaupdate.h \
    fotaUpdateAgentComms.h	\
    fotanotifiers.h	\
    fotafullscreendialog.h	\
    fsview.h
    
    
RESOURCES = fotaservernotifier.qrc

TRANSLATIONS += deviceupdates.ts

SOURCES += FotaDlMgrClient.cpp \
    FotaDlClient.cpp \
    FotaServer.cpp \
    fotaupdate.cpp \
    fotaSrvSession.cpp \
    FotaDB.cpp \
    FotaNetworkRegStatus.cpp \
    FotaIPCTypes.cpp	\
    fotanotifiers.cpp	\
    fotafullscreendialog.cpp \
    main.cpp	\
    fsview.cpp
    
CONFIG(release, debug|release):LIBS += -L. \
    -L./../../../../WrtBuild/Release/bin \
    -lWrtDownloadMgr \
    -lwrtserviceipcclient
CONFIG(debug, debug|release):LIBS += -L. \
    -L./../../../../WrtBuild/Debug/bin \
    -lWrtDownloadMgr \
    -lwrtserviceipcclient
symbian { 
    SYMBIAN_PLATFORMS = WINSCW
    SYMBIAN_PLATFORMS += ARMV5
    TARGET.VID = VID_DEFAULT
    TARGET.UID3 = 0x102072C4
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_SERVER \
        NetworkControl \
        PowerMgmt \
        DiskAdmin \
        All \
        -Tcb
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
        /epoc32/include/platform/mw/cwrt \
        /epoc32/include/cshelp \
        /epoc32/include/ecom \
        /epoc32/include/libc \
        /epoc32/include/mw/Qt \
        /epoc32/include/mw/QtCore \
        /epoc32/include/mw/QtGui \
        /epoc32/include/mw \
        ../../sf/mw/qt/mkspecs/common/symbian \
        ../../sf/mw/qt/mkspecs/common/symbian/tmp \
        /epoc32/include \
        /epoc32/include/stdapis \
        /epoc32/include/stdapis/sys \
        /epoc32/include/platform/mw \
        /epoc32/include/platform \
        /epoc32/include/platform/loc \
        /epoc32/include/platform/mw/loc \
        /epoc32/include/platform/loc/sc \
        /epoc32/include/platform/mw/loc/sc \
        /epoc32/include/mw/cwrt \
        /epoc32/include/app \
        /epoc32/include/platform/app \
        /epoc32/include/platform/app/loc \
        /epoc32/include/platform/app/loc/sc \
    	/epoc32/include/stdapis/stlportv5	\
		/epoc32/include/mw/hb/hbcore	\
		/epoc32/include/mw/hb/hbwidgets	\
		/epoc32/include/mw/hb/hbutils \
		tmp

RSS_RULES += "hidden = KAppIsHidden;"
	
    LIBS += -laknskins \
        -lapgrfx \
        -lapparc \
        -lavkon \
        -lbafl \
        -lcone \
        -lcommdb \
        -lcommonengine \
        -ledbms \
        -lefsrv \
        -leikctl \
        -leikcore \
        -leikcoctl \
        -leikdlg \
        -lestor \
        -leuser \
        -lfeatmgr \
        -lhlplch \
        -lws32 \
        -lcommonui \
        -lapmime \
        -lsyncmlclientapi \
        -lecom \
        -lbitgdi \
        -lfbscli \
        -lgdi \
        -letelmm \
        -letel \
        -lcentralrepository \
        -lesock \
        -lschsvr \
        -lsysversioninfo \
        -lnsmldmtreedbclient \
        -lakncapserverclient \
        -lstarterclient \
        -lconnmon \
        -lfeatmgr \
        -lfmsclient \
        -lcmmanager \
        -lsysutil \
        -lQtGui \
        -lQtCore \
        -lapgrfx \
        -lgdi \
        -leiksrv \
        -lhal \
        -llibc \
        -llibm \
        -llibdl \
        -lmediaclientaudio \
        -lcharconv \
        -lws32 \
        -laknnotify \
		-lHbCore	\
		-lHbWidgets	\
		-lHbUtils	\
		-lQtSvg		\
		-lQtNetwork \
		-lxqutils \
		-linetprotutil \
		-lusbman \
		-lxqserviceutil \
		-lflogger
	
CONFIG += hb
MOC_DIR = moc
}
