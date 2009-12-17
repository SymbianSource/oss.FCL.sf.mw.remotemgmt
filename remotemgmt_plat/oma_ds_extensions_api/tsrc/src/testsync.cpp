/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

//SyncML headers
#include <SyncMLDef.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLTransportProperties.h>
#include <commdb.h> 
#include "nsmlconstants.h"
#include "ClientAPIAdvanced.h"





// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CSync::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CSync* CSync::NewL(TSmlProfileId aProfileId)
{

    CSync* self = new (ELeave) CSync;
    CleanupStack::PushL(self);
    self->ConstructL(aProfileId);
    CleanupStack::Pop(self);
    return self;
}

// ----------------------------------------------------------------------------
// CSync::CSync
// Constructor
// ----------------------------------------------------------------------------
CSync::CSync() : CActive(CActive::EPriorityHigh)
{	
   CActiveScheduler::Add(this);
}

// ----------------------------------------------------------------------------
// CSync::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CSync::ConstructL(TSmlProfileId aProfileId)
{  
    iProfileId=aProfileId;
    iState = ERegister;
    IssueRequest();
}

// ----------------------------------------------------------------------------
// CSync::~CSync
// Destructor*
// ----------------------------------------------------------------------------
CSync::~CSync()
{
}

// ----------------------------------------------------------------------------
// CSync::RunL
// ---------------------------------------------------------------------------
void CSync::RunL()
{
    switch ( iState ) 
    {
		case ERegister:
		     RegisterL();
		     break;
		case ESyncOk:
    		 StartSyncL();
    	     break;
    	case EInitSync:
    		 break;
		case EComplete:
			 Complete();
		     break;
    	case ECancel:
    	     SyncCancel();
		default:
		    break;
    }
}

// ----------------------------------------------------------------------------
// CSync::RunError
// ----------------------------------------------------------------------------
TInt CSync::RunError ( TInt aError )
{
      return aError;
}

// ----------------------------------------------------------------------------
// CSync::DoCancel
// ----------------------------------------------------------------------------
void CSync::DoCancel()
{
    // Cleanup sync session
    CloseSyncSession();
}

// ----------------------------------------------------------------------------
// CSync::RegisterL
// ----------------------------------------------------------------------------
void CSync::RegisterL()
{
    // Wait for events
    iState = ESyncOk;
    IssueRequest();
}



// ----------------------------------------------------------------------------
// CSync::RegisterL
// ----------------------------------------------------------------------------
void CSync::StartSyncL()
{
    TBool doSync = EFalse;
    TSmlProfileId profileId;
    
    OpenSyncSessionL();
         
    // Trigger sync here	    
	// Register for sync events    
	iSyncSession.RequestEventL(*this);     // for MSyncMLEventObserver events      
	iSyncSession.RequestProgressL(*this);  // for MSyncMLProgressObserver events
	
	iSyncJob.CreateL(iSyncSession,iProfileId );
	iState = EInitSync;
	IssueRequest();
	    
}

// ----------------------------------------------------------------------------
// CSync::SyncCancel
// ----------------------------------------------------------------------------

void CSync::SyncCancel()
{
	iSyncSession.CancelEvent();
	iSyncSession.Close();
	
	Cancel();
    // Close the server
    CActiveScheduler::Stop();
}


// ----------------------------------------------------------------------------
// CSync::OpenSyncSessionL
// ----------------------------------------------------------------------------
void CSync::OpenSyncSessionL()
{
	TRAPD(err, iSyncSession.OpenL());
	    
	    if (err != KErrNone)
    	{
	      	User::Leave(err);
    	}
	
}

// ----------------------------------------------------------------------------
// CSync::Complete
// ----------------------------------------------------------------------------
void CSync::Complete()
{
    
	Cancel();

    // Close the server
    CActiveScheduler::Stop();
    
}

// ----------------------------------------------------------------------------
// CSync::CloseSyncSession
// ----------------------------------------------------------------------------
void CSync::CloseSyncSession()
{
		iSyncSession.CancelEvent();
		iSyncSession.Close();
}

// ----------------------------------------------------------------------------
// CSync::EventL
// ----------------------------------------------------------------------------
void CSync::IssueRequest()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}

// ----------------------------------------------------------------------------
// CSync::OnSyncMLSessionEvent
// ----------------------------------------------------------------------------
void CSync::OnSyncMLSessionEvent(TEvent aEvent, 
                                   TInt /*aIdentifier*/, 
                                   TInt /*aError*/, 
                                   TInt /*aAdditionalData*/)
{    
	
    switch ( aEvent )
    {
    	case EJobStart:
    	     break;
    	case EJobStartFailed:
		case EJobRejected:
		case ETransportTimeout:
		case EServerSuspended:
		case EServerTerminated:	 
	  	case EJobStop:
			 TInt err(KErrNone);
		     iState = EComplete;
	     	 IssueRequest();	
    	     break;
    }
}

// -----------------------------------------------------------------------------
// CSync::OnSyncMLSyncError (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CSync::OnSyncMLSyncError(TErrorLevel aErrorLevel, TInt aError, TInt /*aTaskId*/, TInt /*aInfo1*/, TInt /*aInfo2*/)
{
   //nothing
}


// -----------------------------------------------------------------------------
// CSync::OnSyncMLSyncProgress (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CSync::OnSyncMLSyncProgress(TStatus aStatus, TInt aInfo1, TInt /*aInfo2*/)
{

	//nothing
}

void CSync::OnSyncMLDataSyncModifications(TInt aTaskId, 
                      const TSyncMLDataSyncModifications& aClientModifications,
                      const TSyncMLDataSyncModifications& aServerModifications)
{
	//Do Nothing
}



