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
* Description:
* Implementation of CDCMOCustCmdAllReasons class.
*
*/

#include <schtime.h>      
#include <csch_cli.h>    
#include <centralrepository.h>
#include <dcmoclient.h>
#include <featmgr.h>
#include <DevManInternalCRKeys.h>
#include "dcmocustcmdallreasons.h"
#include "dcmostartupDebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::NewL
// ---------------------------------------------------------------------------
//
CDCMOCustCmdAllReasons* CDCMOCustCmdAllReasons::NewL()
	{
  FLOG( _L( "[CDCMOCustCmdAllReasons::NewL() BEGIN " ) );
	return new ( ELeave ) CDCMOCustCmdAllReasons;
	}


// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::~CDCMOCustCmdAllReasons
// ---------------------------------------------------------------------------
//
CDCMOCustCmdAllReasons::~CDCMOCustCmdAllReasons()
    {
      FLOG( _L( "[CDCMOCustCmdAllReasons::~CDCMOCustCmdAllReasons()  " ) );
    }


// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::Initialize
// ---------------------------------------------------------------------------
//
TInt CDCMOCustCmdAllReasons::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FLOG( _L( "[CDCMOCustCmdAllReasons::~Initialize()  " ) );    
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::Execute
// ---------------------------------------------------------------------------
//
void CDCMOCustCmdAllReasons::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    	FLOG( _L( "[CDCMOCustCmdAllReasons::~Execute() BEGIN " ) );
      aRequest = KRequestPending;			
    	TRAP_IGNORE(ExecuteL());			
    	TRequestStatus* request = &aRequest;
    	User::RequestComplete( request, KErrNone );
 			FLOG( _L( "[CDCMOCustCmdAllReasons::~Execute() END " ) ); 
    }

// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::ExecuteL
// ---------------------------------------------------------------------------
//
void CDCMOCustCmdAllReasons::ExecuteL( )
    {
      FLOG( _L( "[CDCMOStartupExtensionPlugIn]::ExecuteL() Boot reason AllReason BEGIN " ) );
      FeatureManager::InitializeLibL();
			if ( FeatureManager::FeatureSupported ( KFeatureIdFfRuntimeDeviceCapabilityConfiguration  ) )
			{
      	CRepository *rep = NULL;  
    		TInt value(KErrNotFound);
				TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys )) ;
				if(!err1) 
	   	 		rep->Get( KDevManMemCardCtrl, value );  
				delete rep; 
				rep = NULL;
				if(!value)
				{
					RDCMOClient* dcmoClient = NULL;
					TRAPD( err2, dcmoClient = RDCMOClient::NewL()); 
					if(!err2)
					{
        		TBuf<20> xValue;
        		xValue.Copy(_L("ExternalMemory"));
        		TInt err = dcmoClient->SetDCMOIntAttributeValue(xValue, EEnable, value);
        		delete dcmoClient;
        		dcmoClient = NULL;        
					}
				}
			}
      // uninitialize feature manager
			FeatureManager::UnInitializeLib();      
      FLOG( _L( "[CDCMOStartupExtensionPlugIn]::ExecuteL() Boot reason AllReason END " ) );      
    }

// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CDCMOCustCmdAllReasons::ExecuteCancel()
    {
   
    }


// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::Close
// ---------------------------------------------------------------------------
//
void CDCMOCustCmdAllReasons::Close()
    {
     FLOG( _L( "[CDCMOStartupExtensionPlugIn]::Close() " ) );
    }


// ---------------------------------------------------------------------------
// CDCMOCustCmdAllReasons::Release
// ---------------------------------------------------------------------------
//
void CDCMOCustCmdAllReasons::Release()
    {
    
     FLOG( _L( "[CDCMOStartupExtensionPlugIn]::Close() " ) );
	   delete this;
    }
