#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#
# ============================================================================
#  Name	 : Icons_aif_scalable_dc.mk
#  Part of  : SyncFwCustomizer
#
#  Description:
# 
# ============================================================================


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\SyncFwCustomizer_aif.mif
HEADERDIR=$(EPOCROOT)epoc32\include
HEADERFILENAME=$(HEADERDIR)\SyncFwCustomizer_aif.mbg

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	@echo ...Deleting $(ICONTARGETFILENAME)
	del /q /f $(ICONTARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : $(ICONTARGETFILENAME)

$(ICONTARGETFILENAME) : $(ICONDIR)\qgn_menu_SyncFwCustomizer.svg
	mifconv $(ICONTARGETFILENAME) \
		/H$(HEADERFILENAME) \
		/c32,8 $(ICONDIR)\qgn_menu_SyncFwCustomizer.svg
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

