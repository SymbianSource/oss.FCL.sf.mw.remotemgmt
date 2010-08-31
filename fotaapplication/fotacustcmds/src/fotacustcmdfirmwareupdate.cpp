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
* Implementation of CFotaCustCmdFirmwareUpdate class.
*
*/

#include "fotacustcmdfirmwareupdate.h"
#ifdef __SYNCML_DM_FOTA
#include <fotaengine.h>
#include "fmsclient.h"
#include "fotaserverPrivateCRKeys.h"
#include "FotaIPCTypes.h"
#include "fmsclientserver.h"
#endif
#include "fotastartupDebug.h"
#include <centralrepository.h>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::NewL
// ---------------------------------------------------------------------------
//
CFotaCustCmdFirmwareUpdate* CFotaCustCmdFirmwareUpdate::NewL()
	{
    FLOG( _L( "CFotaCustCmdFirmwareUpdate::NewL()  " ) );
	return new ( ELeave ) CFotaCustCmdFirmwareUpdate;
	}


// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::~CFotaCustCmdFirmwareUpdate
// ---------------------------------------------------------------------------
//
CFotaCustCmdFirmwareUpdate::~CFotaCustCmdFirmwareUpdate()
    {
     FLOG( _L( "CFotaCustCmdFirmwareUpdate::~CFotaCustCmdFirmwareUpdate()  " ) );
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::Initialize
// ---------------------------------------------------------------------------
//
TInt CFotaCustCmdFirmwareUpdate::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    
     FLOG( _L( "CFotaCustCmdFirmwareUpdate::Initialize()  " ) );
    
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::Execute
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirmwareUpdate::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
     aRequest = KRequestPending;
       FLOG( _L( "CFotaCustCmdFirmwareUpdate::Execute() BEGIN " ) );
     #ifdef __SYNCML_DM_FOTA
     TRAP_IGNORE(ExecuteL());
     #endif
     TRequestStatus* request = &aRequest;
     User::RequestComplete( request, KErrNone );
     FLOG( _L( "CFotaCustCmdFirmwareUpdate::Execute() END " ) );
     

    }
// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::ExecuteL
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirmwareUpdate::ExecuteL( )
    
   {
   	FLOG( _L( "CFotaCustCmdFirmwareUpdate::ExecuteL( ) Boot reason KFirmwareUpdateReason BEGIN " ) );

    CRepository*    centrep( NULL);
    RFotaEngineSession fotaEngine;
    TInt err= KErrNone;
    TRAP( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    TInt sendGAAfterrebootfeature = 0;
    	if( err == KErrNone && centrep )
    	{
    	  err = centrep->Get(  KGenericAlertResendAfterBoot, sendGAAfterrebootfeature );
    	}
    	 FTRACE(RDebug::Print(_L("[CFotaCustCmdFirmwareUpdate] feature sendGenericAlert is %d"), sendGAAfterrebootfeature));        
        if( sendGAAfterrebootfeature != 1 )
        {
   	        FLOG( _L( "[CFotaCustCmdFirmwareUpdate] FirmwareUpdateReason: Starting fota server  " ) );
            TRAPD(oError,fotaEngine.OpenL());
            if(oError == KErrNone)
            {
            fotaEngine.Close();
            FLOG( _L( "[CFotaCustCmdFirmwareUpdate] FirmwareUpdateReason: Closing fota server  " ) );
            }
       
        }
        
        if(centrep)
        {
        	delete centrep;
        }
    
  	FLOG( _L( "CFotaCustCmdFirmwareUpdate::ExecuteL( ) Boot reason KFirmwareUpdateReason END " ) );

   }
// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirmwareUpdate::ExecuteCancel()
    {
  
      FLOG( _L( "CFotaCustCmdFirmwareUpdate::ExecuteCancel() " ) );
    
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::Close
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirmwareUpdate::Close()
    {
    
       FLOG( _L( "CFotaCustCmdFirmwareUpdate::Close() " ) );
    
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirmwareUpdate::Release
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirmwareUpdate::Release()
    {
    FLOG( _L( "CFotaCustCmdFirmwareUpdate::Release() " ) );

	delete this;
    }
