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
* Description:  DCMO Notifier Aob
*
*/


#include "dcmonotifieraob.h"
#include "dcmoserver.h"
#include "dcmodebug.h"


// --------------------------------------------------------------------------
// CDCMONotifierAob* CDCMONotifierAob::NewL( CAknGlobalMsgQuery& aGlobalMsgQuery )
// --------------------------------------------------------------------------
//
CDCMONotifierAob* CDCMONotifierAob::NewL( )
	{
		RDEBUG("CDCMONotifierAob::NewL begin");
		CDCMONotifierAob* self = new (ELeave) CDCMONotifierAob( );
		CleanupStack::PushL(self);
  	self->ConstructL();
  	CleanupStack::Pop(); // self
  	RDEBUG("CDCMONotifierAob::NewL end");
		return self;		
	}

// --------------------------------------------------------------------------
// CDCMONotifierAob::CDCMONotifierAob( CAknGlobalMsgQuery& aGlobalMsgQuery )
// --------------------------------------------------------------------------
//
CDCMONotifierAob::CDCMONotifierAob( ) 
	:CActive( EPriorityStandard ) //EPriorityNormal )  
	{
		RDEBUG("CDCMONotifierAob::constructor");
	}


// --------------------------------------------------------------------------
// void CDCMONotifierAob::ConstructL()
// --------------------------------------------------------------------------
//
void CDCMONotifierAob::ConstructL()
    {   
    	RDEBUG("CDCMONotifierAob::ConstructL begin");		
    	iGlobalMsgQuery = NULL;
    	CActiveScheduler::Add(this);	
    	RDEBUG("CDCMONotifierAob::ConstructL end");    	
    }

// --------------------------------------------------------------------------
// CDCMONotifierAob::~CDCMONotifierAob()
// --------------------------------------------------------------------------
//
CDCMONotifierAob::~CDCMONotifierAob()
	{
		RDEBUG("CDCMONotifierAob::~CDCMONotifierAob begin");   
		if( iGlobalMsgQuery )
		{
			delete iGlobalMsgQuery ;
    	iGlobalMsgQuery = NULL; 
		}	
		RDEBUG("CDCMONotifierAob::~CDCMONotifierAob end");    
  }

// --------------------------------------------------------------------------
// void CDCMONotifierAob::RunL()
// --------------------------------------------------------------------------
//
void CDCMONotifierAob::RunL()
    {
    RDEBUG("CDCMONotifierAob::RunL() : Begin");   

    if(iStatus == EAknSoftkeyOk)
    {
    	RDEBUG("CDCMONotifierAob::RunL() : EAknSoftkeyOk");     	
    	delete iGlobalMsgQuery ;
    	iGlobalMsgQuery = NULL; 	
    	if( CDCMOServer::iSessionCount == 0)
    		CActiveScheduler::Stop();
    }
        
    RDEBUG("CDCMONotifierAob::RunL() : End");
    }

// --------------------------------------------------------------------------
// void CDCMONotifierAob::DoCancel()
// --------------------------------------------------------------------------
//
void CDCMONotifierAob::DoCancel()
   {
   		RDEBUG("CDCMONotifierAob::DoCancel() : Begin");
   		iGlobalMsgQuery->CancelMsgQuery();
    	delete iGlobalMsgQuery ;
    	iGlobalMsgQuery = NULL; 	
			RDEBUG("CDCMONotifierAob::DoCancel() : End");
	 }

// --------------------------------------------------------------------------
// void CDCMONotifierAob::ShowNotifierL( TDesC& aString )
// --------------------------------------------------------------------------
//
void CDCMONotifierAob::ShowNotifierL( TDesC& aString )
{
		RDEBUG("CDCMONotifierAob::ShowNotifierL() : Begin");
		if( !iGlobalMsgQuery )
			iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
		if (!IsActive())
    {
			iStatus = KRequestPending;		
		     
			iGlobalMsgQuery->ShowMsgQueryL(
		                    iStatus, 
		                    aString,
		                    R_AVKON_SOFTKEYS_OK_EMPTY, //R_AVKON_SOFTKEYS_OK_EMPTY, //R_AVKON_SOFTKEYS_OK_CANCEL,
		                    KNullDesC,
		                    KNullDesC);  
			SetActive();                    
		}
		RDEBUG("CDCMONotifierAob::ShowNotifierL() : End");	
}

// -----------------------------------------------------------------------------
// CDCMONotifierAob::RunError
// Called when RunL leaves
// This method can't leave
// -----------------------------------------------------------------------------

TInt CDCMONotifierAob::RunError(TInt /* aError */)
	{
	RDEBUG("CDCMONotifierAob::RunError >>");
			
	RDEBUG("CDCMONotifierAob::RunError <<");
	return KErrNone;
	}
	
//  End of File
