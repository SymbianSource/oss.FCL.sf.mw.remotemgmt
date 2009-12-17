/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef SCPLOCKEVENTHANDLER_H
#define SCPLOCKEVENTHANDLER_H

//  INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>
#include <etelmm.h>
#include "SCPSession.h"
#include "SCPServerInterface.h"

// CLASS DECLARATION

/**
*  A base class for the active objects responsible for the lock phone functionality
*/
class CSCPLockEventHandler : public CActive
	{		
	public:  // Methods

        // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CSCPLockEventHandler( CSCPServer* aServer );

        /**
        * Destructor.
        */
        virtual ~CSCPLockEventHandler();
                        
    protected:  // Methods

       // New Methods
        
        /**
        * The base class constuction method, opens the member handles
        */
        void BaseConstructL();     

    protected:  // Data
        /** A ptr to the SCP server */
        CSCPServer* iServer;
        /** A prt to a handle to ETel server, not owned */
        RTelServer*  iTelServ;
        /** A ptr to a handle to the multimode TSY, not owned */
        RMobilePhone* iPhone;                                
    };

#endif      // SCPLOCKEVENTHANDLER_H   
            
// End of File

