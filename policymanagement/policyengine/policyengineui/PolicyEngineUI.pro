# Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Implementation of policymanagement components


TEMPLATE = lib
TARGET = PolicyEngUINotifiersplugin
CONFIG += hb\
          plugin

DESTDIR = $${HB_BUILD_DIR}/plugins/devicedialogs
          
QT += core \
    gui
HEADERS += 	./inc/PolicyEngUINotifierplugin.h \
    	   	./inc/PolicyEngineUI.h
SOURCES += 	./src/PolicyEngNotifierplugin.cpp \
   			./src/PolicyEngineUI.cpp
RESOURCES += PolicyEngineUI.qrc
LIBS += -lflogger

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x10207817

    hblib.sources = Hb.dll
    hblib.path = /sys/bin
    hblib.depends = "(0xEEF9EA38), 1, 0, 0, {\"Hb\"}"

    pluginstub.sources = PolicyEngUINotifiersplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
 }
 !local {
    target.path = $${HB_PLUGINS_DIR}/devicedialogs
    INSTALLS += target
}

