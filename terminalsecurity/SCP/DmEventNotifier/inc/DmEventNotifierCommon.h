/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This header contains all the common constants that are used by the application.
*
*/
#ifndef __DMEVENTNOTIFIERCOMMON_H__
#define __DMEVENTNOTIFIERCOMMON_H__

//UID3 of this executable
#define KAppUidDmEventNotifier 0x20026F5E

// Definition for various states of the handler
enum TScheduleHandlerState
    {
    EHandlerNotRegistered = 0,//Schedule handler not registered to Symbian scheduler
    EHandlerRegistered,       //Schedule handler registered to Symbian scheduler
    EHandlerNeedRegister      //To register the handler after the ongoing operation is complete
    };

// Definition of Service Ids. Must not exceed KMaxServices numbers
enum THandlerServiceId
    {
    ESoftwareService = 0,  //Software application service. Begin with 0 always. 
    EJavaService = 1,      //Java application service
    EMmcService = 2      //MMC card service
    };

// Definiton of event operations
enum THandlerOperation
    {
    ENoOpn = 200,             //No operation
    EOpnInstall = 201,        //used by ESwAppService & EJavaAppService services
    EOpnUninstall = 202,      //used by ESwAppService & EJavaAppService services
    EOpnRestore = 203,        //used by ESwAppService & EJavaAppService services
    EOpnInserted = 204,       //used by EMmcService
    EOpnRemoved = 205,        //used by EMmcService
    EOpnUnknown = 206         //unknown operation
    };

#endif /* __DMEVENTNOTIFIERCOMMON_H__ */
// End of File
