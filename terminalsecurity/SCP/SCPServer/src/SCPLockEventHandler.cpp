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


// INCLUDE FILES
#include <e32base.h>
#include <e32debug.h>

#include <etelmm.h>
#include <rmmcustomapi.h>



#include    <e32property.h>
#include    <PSVariables.h>   // Property values


 
#include <mmtsy_names.h>

#include "SCPLockEventHandler.h"
#include "SCPServer.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSCPLockEventHandler::CSCPLockEventHandler( CSCPServer* aServer )
	: CActive( EPriorityHigh ),
	iServer( aServer )
	// Assign a high priority to bypass all other possible outstanding requests
	{
	Dprint( (_L("--> CSCPLockEventHandler::CSCPLockEventHandler()")) );
 
    // No implementation required  
	
	Dprint( (_L("<-- CSCPLockEventHandler::CSCPLockEventHandler()")) );
	}


// Symbian 2nd phase constructor can leave.
void CSCPLockEventHandler::BaseConstructL()
    {
    Dprint( (_L("--> CSCPLockEventHandler::BaseConstructL()") ));

    iServer->GetEtelHandlesL( &iTelServ, &iPhone );
    
    CActiveScheduler::Add( this );
   	
    Dprint( (_L("<-- CSCPLockEventHandler::BaseConstructL()") ));
    }

    
// Destructor
CSCPLockEventHandler::~CSCPLockEventHandler()
    {
    Dprint( (_L("--> CSCPLockEventHandler::~CSCPLockEventHandler()") ));
           
    Dprint( (_L("<-- CSCPLockEventHandler::~CSCPLockEventHandler()") ));
    }

//  End of File  

