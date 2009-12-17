/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SCPEventHandler.h - This header file is used by swhandler.
*               This checks for uninstallation (presently only for sisx uninstallation) or 
*								removal of memory card. Whenever there is uninstallation the database has 
*								to be updated for the parameters changes.
*								This depends on Application installer (For Java: Java registry) & the SCPDatabase.
*
*/

#ifndef SCPEVENTHANDLER_H
#define SCPEVENTHANDLER_H

//For Swi (Sis registry)
#include <e32base.h>
#include <swi/launcher.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <swi/sisregistryentry.h>
// Sw handler for ENUM
#include "DmEventNotifierCommon.h"
/* Java registry
#include <javadomainpskeys.h>
#include <javaregistryincludes.h>
*/

const TUint32 KNSmlDMHostServer1ID = 270506498;  // 0x101F9A02
const TUint32 KNSmlDMHostServer2ID = 270506499;  // 0x101F9A03
const TUint32 KNSmlDMHostServer3ID = 270506500;  // 0x101F9A04
const TUint32 KNSmlDMHostServer4ID = 270506501;  // 0x101F9A05

/**
 * This checks for uninstallation (presently only for sisx uninstallation) or 
 * removal of memory card. Whenever there is uninstallation the database has 
 * to be updated for the parameters changes.
 * This depends Application installer & the SCPDatabase.
 *
 * @lib SCPEventHandler.lib
 */

// Clean Comments

class CSCPEventHandler : public CBase
    {
    public: 
        
       /**
        * Two-phased constructor.
        */
        IMPORT_C static CSCPEventHandler* NewL();

       /**
        * Two-phased constructor.
        */
        IMPORT_C static CSCPEventHandler* NewLC();

       /**
        * Destructor.
        */
        ~CSCPEventHandler();
    
        /** 
         * Get the event from SWHandler. 
         * Query the db by calling the 'db api'.
         * For each of the db entry make a call to Application Installer/ Java registry & 
         * check for the state, collect all the uid's that has to change.
         * If there is need to update the database, then call SCPServer!
         * 
         * @param  Event - An event that has occured, has to be passed from the Sw-handler
         *                  May be, Memory card removal or manual uninstall etc.
         * @return None
         */
         
         IMPORT_C void NotifyChangesL(THandlerServiceId aEvent, THandlerOperation aOperation);
                
      private:
        /**
         * C++ default constructor.
         */
          CSCPEventHandler();

        /** 
         *By default Symbian 2nd phase constructor is private.
         */
         void ConstructL();
        
         /* Since the applications is of Symbian, => only sis files needs uid check. The Java related are commented
         inline void GetInstalledJavaUidsL(RArray<TUid>& aUids);
         inline TBool isInstalledJava(const TUid& aUid, const RArray<TUid>& aUids ) const;         
          */

				inline void NotifyCleanupL(RArray<TUid>& aUids);
    };
#endif SCPEVENTHANDLER_H
