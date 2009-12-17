#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#
EPOCROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(EPOCROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

$(TARGET_DIR) $(TARGET_DIR_WINSCW_UDEB) $(TARGET_DIR_WINSCW_UREL) $(TARGET_DIR_ARMV5_UDEB) $(TARGET_DIR_ARMV5_UREL):
	-$(MKDIR) $@

RESOURCE: $(TARGET_DIR) $(TARGET_DIR_WINSCW_UDEB) $(TARGET_DIR_WINSCW_UREL) $(TARGET_DIR_ARMV5_UDEB) $(TARGET_DIR_ARMV5_UREL)
	-$(RM) $(TARGET_DIR)/$(FILE_EXT) \
	        $(TARGET_DIR_WINSCW_UREL)/$(FILE_EXT) $(TARGET_DIR_WINSCW_UDEB)/$(FILE_EXT) \
	        $(TARGET_DIR_ARMV5_UREL)/$(FILE_EXT) $(TARGET_DIR_ARMV5_UDEB)/$(FILE_EXT)
	@echo policy dir = $(SRC_DIR)
	-$(CP) $(SRC_DIR)/$(FILE_EXT) $(TARGET_DIR)/
	-$(CP) $(SRC_DIR)/$(FILE_EXT) $(TARGET_DIR_WINSCW_UDEB)/
	-$(CP) $(SRC_DIR)/$(FILE_EXT) $(TARGET_DIR_WINSCW_UREL)/
	-$(CP) $(SRC_DIR)/$(FILE_EXT) $(TARGET_DIR_ARMV5_UDEB)/
	-$(CP) $(SRC_DIR)/$(FILE_EXT) $(TARGET_DIR_ARMV5_UREL)/

RELEASABLES CLEAN REALLYCLEAN LIB BLD MAKEFILE LIBRARY FINAL TARGET SAVESPACE MAKMAKE FREEZE: do_nothing

do_nothing :
	@echo do_nothing