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
#    Makefile for building bif resource 
#
#

PROJECT=provisioningbc
SOURCEDIR=..\bif
LANGUAGE=sc


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGETDIR=\Epoc32\Release\$(PLATFORM)\$(CFG)\z\system\bif
else
TARGETDIR=\Epoc32\release\$(PLATFORM)\$(CFG)
endif

$(TARGETDIR) :
	@perl \epoc32\tools\emkdir.pl $(TARGETDIR)


SOURCERESOURCE=$(SOURCEDIR)\provisioningbif.rss
TEMPRESOURCE=$(TARGETDIR)\provisioningbif.rss
TARGETRESOURCE=$(TARGETDIR)\prov.r$(LANGUAGE)

$(TARGETRESOURCE) : $(TARGETDIR) $(SOURCERESOURCE)
	@copy $(SOURCERESOURCE) $(TEMPRESOURCE)
	@epocrc.bat -u -I. -I\epoc32\include -I$(SOURCEDIR)\..\..\Group $(TEMPRESOURCE) -o$(TARGETRESOURCE)
	@del $(TEMPRESOURCE)

do_nothing:
	rem do nothing


MAKMAKE : do_nothing

RESOURCE : $(TARGETRESOURCE)

SAVESPACE : BLD

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

CLEAN : 
	@erase $(TARGETRESOURCE)

RELEASABLES : 
	@echo $(TARGETRESOURCE)

# End of file
