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
* Description: Observer for Set System Locked event
*/
#include    <bldvariant.hrh>
#include    <e32property.h>
#include	<PSVariables.h>
#include	<coreapplicationuisdomainpskeys.h>
#include <ctsydomainpskeys.h>
//#include    <SecurityUIsPrivatePSKeys.h>
#include "secui.hrh"
#include "SCPLockObserver.h"
#include "SCPQueryDialog.h"
#include "SCPDebug.h"


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSCPLockObserver::NewL()
// Constructs a new entry with given values.
// ----------------------------------------------------------
//
CSCPLockObserver* CSCPLockObserver::NewL(CSCPQueryDialog* aDialog, TInt aType)
    {
    
    Dprint(_L("CSCPLockObserver::NewL() BEGIN"));
    
    CSCPLockObserver* self = new (ELeave) CSCPLockObserver(aDialog, aType);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    Dprint(_L("CSCPLockObserver::NewL() END"));
    
    return self;
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::CSCPLockObserver()
// Destructor
// ----------------------------------------------------------
//
CSCPLockObserver::~CSCPLockObserver()
    {
    
    Dprint(_L("CSCPLockObserver::~CSCPLockObserver"));
    
    Cancel();
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::Start()
// Starts listening an event 
// ----------------------------------------------------------
//
TInt CSCPLockObserver::Start()
    {
    
    Dprint(_L("CSCPLockObserver::Start() BEGIN"));
    
    if (IsActive())
    {
    
    Dprint(_L("CSCPLockObserver::Start() In use!!!"));
    
    return KErrInUse;
    }
        
    iStatus = KRequestPending;
    
    switch(iType)
        {
            case ESecUiDeviceLockObserver:
                
                Dprint(_L("CSCPLockObserver::Start() Device Lock Observer"));
                
                iProperty.Attach(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus); 
                break;
            case ESecUiCallStateObserver:
                
                Dprint(_L("CSCPLockObserver::Start() Call State Observer"));
                
                iProperty.Attach(KPSUidCtsyCallInformation, KCTsyCallState); 
                break;

            default:
                break;
        }
    
    iProperty.Subscribe(iStatus);
    SetActive();
    iSubscribedToEvent = ETrue;
    
    Dprint(_L("CSCPLockObserver::Start() END"));
    
    return KErrNone;
    }
//
// ----------------------------------------------------------
// CLockObserver::CLockObserver()
// C++ constructor
// ----------------------------------------------------------
// 
CSCPLockObserver::CSCPLockObserver(CSCPQueryDialog* aDialog, TInt aType) : CActive(0), iDialog(aDialog), iSubscribedToEvent(EFalse), iType(aType)
	{                            
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::ConstructL()
// Symbian OS default constructor
// ----------------------------------------------------------
// 
void CSCPLockObserver::ConstructL()
    {
    
    Dprint(_L("CSCPLockObserver::ConstructL() BEGIN"));
    
    // Add this active object to the scheduler.
	CActiveScheduler::Add(this);
	// Begin obsering PubSub event  
	Start();
	
    Dprint(_L("CSCPLockObserver::ConstructL() END"));
    	
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::RunL()
// Called by Active Scheduler
// ----------------------------------------------------------
// 
void CSCPLockObserver::RunL()
	{
	
    Dprint(_L("CSCPLockObserver::RunL() BEGIN"));
    
    
    switch(iType)
        {
            case ESecUiDeviceLockObserver:
                 TInt autolockState;
                 iProperty.Get( autolockState );
                 if (autolockState > EAutolockOff)
                    {
                    
                    Dprint(_L("CSCPLockObserver::RunL() TryCancelQueryL Device Lock"));
                    
                	iDialog->TryCancelQueryL(ESecUiDeviceLocked);
                	iSubscribedToEvent = EFalse;
                    }
                break;
            case ESecUiCallStateObserver:
                TInt callState;
                iProperty.Get( callState );
				 
				 Dprint( (_L("CSCPLockObserver::RunL() callState : %d"),callState ));
                if(callState == EPSCTsyCallStateDisconnecting)
                    {
                     
                    Dprint(_L("CSCPLockObserver::RunL() TryCancelQueryL Active Call"));
                    
                	iDialog->TryCancelQueryL(EPSCTsyCallStateDisconnecting);
                	iSubscribedToEvent = EFalse;   
                    }
                break;

            default:
                break;
        }

   
	
    Dprint(_L("CSCPLockObserver::RunL() END"));
    
	}
// ----------------------------------------------------------------------------
// CSCPLockObserver::RunError
// ----------------------------------------------------------------------------
TInt CSCPLockObserver::RunError ( TInt /*aError*/ )
    {
        return KErrNone;
    }	
//
// ----------------------------------------------------------
// CSCPLockObserver::DoCancel()
// Cancels event listening
// ----------------------------------------------------------
// 
void CSCPLockObserver::DoCancel()
    {
    
    Dprint(_L("CSCPLockObserver::DoCancel() BEGIN"));
    
    if(iSubscribedToEvent)
    	iProperty.Cancel();
    iStatus = KErrNone;
    
    Dprint(_L("CSCPLockObserver::DoCancel() END"));
    
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::StopObserver()
// Cancels event listening
// ----------------------------------------------------------
//     
void CSCPLockObserver::StopObserver()
	{
	
    Dprint(_L("CSCPLockObserver::StopObserver() BEGIN"));
    
    Cancel();
    
    Dprint(_L("CSCPLockObserver::StopObserver() END"));
    
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::StartObserver()
// Start event listening
// ----------------------------------------------------------
//     
void CSCPLockObserver::StartObserver()
	{
	
    Dprint(_L("CSCPLockObserver::StartObserver() BEGIN"));
    
    Start();
    
    Dprint(_L("CSCPLockObserver::StartObserver() END"));
    
    }
//
// ----------------------------------------------------------
// CSCPLockObserver::SetAddress()
// Start event listening
// ----------------------------------------------------------
//  
void CSCPLockObserver::SetAddress(CSCPQueryDialog* aDialog)
{
	
    Dprint(_L("CSCPLockObserver::SetAddress() BEGIN"));
    
	iDialog = aDialog;
	
    Dprint(_L("CSCPLockObserver::SetAddress() END"));
    
}
// End of file
