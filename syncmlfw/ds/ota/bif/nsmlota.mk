#
# Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
# Makefile for building bif resource and bitmaps.
#

PROJECT=nsmlsync
SOURCEDIR= .
LANGUAGE=sc

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGETDIR=\Epoc32\Release\$(PLATFORM)\$(CFG)\Z\resource\messaging\bif
else
TARGETDIR=\Epoc32\data\Z\resource\messaging\bif
endif

$(TARGETDIR) :
	@perl \epoc32\tools\emkdir.pl $(TARGETDIR)

SOURCERESOURCE=nsmlsp_sms_no.rss
TEMPRESOURCE=$(TARGETDIR)\nsmlsp_sms_no.rss
TARGETRESOURCE=$(TARGETDIR)\nsmlsp_sms_no.r$(LANGUAGE)

$(TARGETRESOURCE) : $(TARGETDIR) $(SOURCERESOURCE)
	@copy $(SOURCERESOURCE) $(TEMPRESOURCE)
	@epocrc.bat -u -I. -I..\loc  -I\epoc32\include $(TEMPRESOURCE) -o$(TARGETRESOURCE)
	@del $(TEMPRESOURCE)
	
do_nothing:
	rem do nothing

MAKMAKE : do_nothing

RESOURCE : $(TARGETRESOURCE) $(TARGETRESOURCE2)

SAVESPACE : BLD

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

clear_ro:
	if exist $(TARGETRESOURCE) attrib -r $(TARGETRESOURCE)


CLEAN : clear_ro
	if exist $(TARGETRESOURCE) erase $(TARGETRESOURCE)

RELEASABLES : 
	@echo $(TARGETRESOURCE)


