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
* Description:  SyncML DM NetMon 
*
*/

// INCLUDE FILES
#include <e32base.h>

#include "DMNetworkMon.h"

// ----------------------------------------------------------------------------
// Initialize and run the server
// ----------------------------------------------------------------------------
static void RunTheServerL()
{
    // First create and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);


	// Create the server
	CDMNetworkMon* server = CDMNetworkMon::NewL();  
	CleanupStack::PushL(server);

	// Signal the client the startup is complete
	RProcess::Rendezvous(KErrNone);

	// Enter the wait loop
	CActiveScheduler::Start();

	// Exited cleanup scheduler and server
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(scheduler);
	
    
}

// ----------------------------------------------------------------------------
// Main entry-point for the server thread/process
// ----------------------------------------------------------------------------
static TInt RunTheServer()
{    
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
    {
		TRAP(r,RunTheServerL());
		delete cleanup;
    }
    
    return (r);
}

// ----------------------------------------------------------------------------
// Process entry point 
// ----------------------------------------------------------------------------
TInt E32Main()
{
    return RunTheServer();
}
