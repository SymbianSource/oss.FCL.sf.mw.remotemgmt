/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:   Update agent result codes
 *
 */

#ifndef __COMMUNICATION_BETWEEN_NORMAL_AND_UPDATE_MODE_H__
#define __COMMUNICATION_BETWEEN_NORMAL_AND_UPDATE_MODE_H__

typedef enum
    {

    /*
     Everything is ok - update package were installed successfully and 
     newer version of OS is running.
     */
    UPD_OK = 0,
    /*
     Problem: The request sent to update installer were invalid - probable issue in
     Download agent.
     
     Solutions:
     - Probable: Download agent created an invalid file.
     Fix download agent.
     - Inprobable: Corrupted request file, or FAT
     */
    UPD_INSTALL_REQUEST_IS_INVALID = 1,
    /*
     Problem: Update package is absent from specified location.

     Solution: Incorrect user application actions. 
     Read update application document / manual on how to switch to 
     update mode.
     */
    UPD_UPDATE_PACKAGE_IS_ABSENT = 2,
    /*
     Problem: Update package is corrupted. (Integrity or authenticity 
     check failed)
     Solution: End-user should try to download again update package.
     */
    UPD_UPDATE_PACKAGE_IS_CORRUPTED = 3,
    /*
     Problem: Update package contents is invalid for some reason.
     This problem normally refers to the fact that update package
     were created incorrectly.
     
     Solution: Contact update package build team in order to find out the 
     error cause.
     */
    UPD_UPDATE_PACKAGE_CONTENTS_IS_INVALID = 4,
    /*
     Problem: Update package is not compatible with current mobile device.
     Solution: End-user should try to download again update package.
     */

    UPD_UPDATE_PACKAGE_IS_NOT_COMPATIBLE_WITH_CURRENT_MOBILE_DEVICE = 5,
    /*
     Problem: This error code will be used to indicate fatal error 
     code after which phone becomes dead.
     
     Solution: Phone should be taken to maintenance.
     This error code cannot be seen by normal mode applications 
     because OS cannot be started when phone became dead.
     */
    UPD_FATAL_ERROR = 0xFFFFFFFF
    } InstallationResult;

typedef enum
    {
    UPD_InvalidBlockHeaderIdentifier = 1,
    UPD_InvalidUpdatePackageVersion = 2,
    UPD_GenericInvalidFileFormat = 3
    } DetailedInstallationResult;

#endif //__COMMUNICATION_BETWEEN_NORMAL_AND_UPDATE_MODE_H__
