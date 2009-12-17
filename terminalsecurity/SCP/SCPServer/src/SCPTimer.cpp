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
#include <e32debug.h>

#include "SCPTimer.h"
#include "SCPServer.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSCPTimer::CSCPTimer( TAny* aParam, MSCPTimeoutHandler* aHandler )
	: CActive( EPriorityNormal ),
	iParam( aParam ),
	iHandler( aHandler )
	{
	Dprint( (_L("--> CSCPTimer::CSCPTimer()") ));
 
    // No implementation required    
	
	Dprint( (_L("<-- CSCPTimer::CSCPTimer()") ));
	}


// Static constructor.
CSCPTimer* CSCPTimer::NewL( TUint aTimeout, TAny* aParam, MSCPTimeoutHandler* aHandler )
	{
	Dprint( (_L("--> CSCPTimer::NewL()") ));

	CSCPTimer* self = CSCPTimer::NewLC( aTimeout, aParam, aHandler );
	CleanupStack::Pop( self ); 
	
	Dprint( (_L("<-- CSCPTimer::NewL()") ));
	return self;	
	}
	

// Static constructor, leaves object pointer to the cleanup stack.
CSCPTimer* CSCPTimer::NewLC( TUint aTimeout, TAny* aParam, MSCPTimeoutHandler* aHandler )
	{
	Dprint( (_L("--> CSCPTimer::NewLC()") ));

	CSCPTimer* self = new (ELeave) CSCPTimer( aParam, aHandler );

    CleanupStack::PushL( self );
    self->ConstructL( aTimeout );	
    
    Dprint( (_L("<-- CSCPTimer::NewLC()") ));
    return self;	
    }


// Symbian 2nd phase constructor can leave.
void CSCPTimer::ConstructL( TUint aTimeout )
    {
    Dprint( (_L("--> CSCPTimer::ConstructL()") ));        
    CActiveScheduler::Add( this );
    
    iTimer.CreateLocal();
    
    // Start the timer
    iTimer.After( iStatus, aTimeout );
    // No need to check for isActive, this is the constructor
    SetActive();
   	
    Dprint( (_L("<-- CSCPTimer::ConstructL()") ));
    }

    
// Destructor
CSCPTimer::~CSCPTimer()
	{
	Dprint( (_L("--> CSCPTimer::~CSCPTimer()") ));    
    
    if ( IsActive() )
        {
        Cancel();
        }
        
    iParam = NULL;
    iHandler = NULL;
    
    iTimer.Close();
                  
	Dprint( (_L("<-- CSCPTimer::~CSCPTimer()") ));
	}


// ---------------------------------------------------------
// void CSCPTimer::RunL()
// Handle the timeout by calling the callback method
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPTimer::RunL()
    {
    Dprint( (_L("--> CSCPTimer::RunL()") ));
    
    Dprint( (_L("--> CSCPTimer::RunL(): TIMEOUT occured, signalling the handler") ));
    iHandler->Timeout( iParam );
   
    Dprint( (_L("<-- CSCPTimer::RunL()") ));
    }   


// ---------------------------------------------------------
// void CSCPTimer::DoCancel()
// Cancel the call using the RTimer member
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPTimer::DoCancel()
    {
    Dprint( (_L("--> CSCPTimer::DoCancel()") ));
        
    iTimer.Cancel();
   
    Dprint( (_L("<-- CSCPTimer::DoCancel()") ));
    }   

//  End of File  

