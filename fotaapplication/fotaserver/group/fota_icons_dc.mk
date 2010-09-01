#
# Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Implementation of fotaserver component
# 	This is part of fotaapplication.
#


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\fotaserver.mif
HEADERFILENAME=$(HEADERDIR)\fotaserver.mbg


do_nothing : 
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	/c8,8 qgn_prop_sml_http.bmp \
	/c8,8 qgn_prop_sml_http_off.bmp \
	/c8,8 qgn_prop_sml_bt.bmp \
	/c8,8 qgn_prop_sml_bt_off.bmp \
	/c8,8 qgn_prop_sml_usb.bmp \
	/c8,8 qgn_prop_sml_usb_off.bmp \
	/c8,8 qgn_menu_dm_cxt.bmp \
	/c8,8 qgn_menu_dm_disabled_cxt.bmp \
	/c8,8 qgn_note_sml.bmp \
	/c8,8 qgn_note_sml_server.bmp	
	

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
