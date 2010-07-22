/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   interprocess messages
*
*/



// INCLUDES
#include "FotaIPCTypes.h"
#include <centralrepository.h>
#include "fotaserverPrivateCRKeys.h"

// ======================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// TDownloadIPCParams::TDownloadIPCParams 
// ---------------------------------------------------------------------------
TDownloadIPCParams::TDownloadIPCParams () : iPkgId(-1),iProfileId(0)
        , iSendAlert(ETrue)
        , iIapId(-1), iPkgSize(0), iSessionType(0), iUpdateLtr (0)
{
    iPkgName        = KNullDesC8;
    iPkgVersion     = KNullDesC8;
}

// ---------------------------------------------------------------------------
// TDownloadIPCParams::TDownloadIPCParams 
// ---------------------------------------------------------------------------
TDownloadIPCParams::TDownloadIPCParams (const TDownloadIPCParams& p )
                           : iPkgId ( p.iPkgId ), iProfileId (p.iProfileId )
                           , iSendAlert( p.iSendAlert )
                           , iIapId(p.iIapId), iPkgSize(p.iPkgSize), iSessionType(p.iSessionType), iUpdateLtr (p.iUpdateLtr)
    {
    iPkgName        = p.iPkgName;
    iPkgVersion     = p.iPkgVersion;
    }



// ---------------------------------------------------------------------------
// TPackageState::TPackageState
// ---------------------------------------------------------------------------
TPackageState::TPackageState( RFotaEngineSession::TState aState, TInt aResult)
    : TDownloadIPCParams (),  iState(aState), iResult(aResult)
    						,iSmlTryCount( KDefaultSmlTryCount )
    {
    }


// ---------------------------------------------------------------------------
// TPackageState::TPackageState
// ---------------------------------------------------------------------------
TPackageState::TPackageState( RFotaEngineSession::TState aState) 
    : TDownloadIPCParams (), iState(aState), iResult(-1) 
    						,iSmlTryCount( KDefaultSmlTryCount )
    {
	SetSmlTryCount();
    }


// ---------------------------------------------------------------------------
// TPackageState::TPackageState
// ---------------------------------------------------------------------------
TPackageState::TPackageState( TDownloadIPCParams p) : 
    TDownloadIPCParams(p),iState(RFotaEngineSession::EIdle), iResult(-1)
    					 ,iSmlTryCount( KDefaultSmlTryCount )
    {  
    SetSmlTryCount();
    
    }


// ---------------------------------------------------------------------------
// TPackageState::TPackageState
// ---------------------------------------------------------------------------
TPackageState::TPackageState() : TDownloadIPCParams () 
            ,iState(RFotaEngineSession::EIdle), iResult(-1)
            ,iSmlTryCount( KDefaultSmlTryCount )
    {
    SetSmlTryCount();
    }


// ---------------------------------------------------------------------------
// TPackageState::SetSmlTryCount()
// ---------------------------------------------------------------------------
void TPackageState::SetSmlTryCount()
{
	 TInt err = KErrNone;
	 TInt retry = 0;
	 CRepository *centrep = NULL;
      TRAP(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    	    if ( centrep ) 
        	{
        	err = centrep->Get( KGenericAlertRetries, retry );
        	delete centrep; centrep = NULL;
        	}
        	if(err == KErrNone)
        	{
        		
            	    if(retry < 0 )
            	    {
            	    iSmlTryCount = KDefaultSmlTryCount;
            	    }
                  	else if( retry == 0 )
               	    {
               	     iSmlTryCount = 2 ;
               	    }
                	else if (retry > KMaximumSmlTryCount )
               	    {
               	     iSmlTryCount = KMaximumSmlTryCount + 1;
               	    }
                   	else
                    {
                	  iSmlTryCount = retry + 1;
                    }
        	    }
        	 else
        	    {
        	    iSmlTryCount = KDefaultSmlTryCount;
        	    }
}


// ---------------------------------------------------------------------------
// TPackageState::operator=
// ---------------------------------------------------------------------------
TPackageState& TPackageState::operator= ( const TDownloadIPCParams& a )
    {
    iPkgId      = a.iPkgId;
    iProfileId  = a.iProfileId;
    iPkgName    = a.iPkgName;
    iPkgVersion = a.iPkgVersion;
    iIapId		= a.iIapId;
    iPkgSize	= a.iPkgSize;
    iSessionType= a.iSessionType;
    iUpdateLtr 	= a.iUpdateLtr;

    return *this;
    }


// ---------------------------------------------------------------------------
// TPackageState::operator=
// ---------------------------------------------------------------------------
TPackageState& TPackageState::operator=(const TPackageState& a)
    {
    iPkgId      = a.iPkgId; 
    iProfileId  = a.iProfileId; 
    iPkgName    = a.iPkgName; 
    iPkgVersion = a.iPkgVersion;
    iIapId		= a.iIapId;
    iPkgSize	= a.iPkgSize;
    iSessionType= a.iSessionType;
    iUpdateLtr	= a.iUpdateLtr;
    iState      = a.iState;  
    iResult     = a.iResult;
    iSmlTryCount= a.iSmlTryCount;
    return *this;
    }
 
            
// End of File
