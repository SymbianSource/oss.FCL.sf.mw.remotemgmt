/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of common client api.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include <SyncMLObservers.h>

#include "NSmlClientAPIDefs.h"
#include "NSmlClientAPIUtils.h"
#include "NSmlClientAPIActiveCallback.h"
#include "nsmlsosserverdefs.h"
#include "NSmlErrorCodeConversion.h"
#include <SyncMLContactSuiteProgressClient.h>
#include "NSmlClientContactSuiteAPIActiveCallback.h"

#define KSmlContactSuiteAppUID 0x101f4CCE
  
EXPORT_C void RContactSuiteSyncMLSession::CloseContactSuiteSession()
    {
    delete iContactSuiteCallBack;
    iContactSuiteCallBack = NULL;    
    Close();
    }

EXPORT_C void RContactSuiteSyncMLSession::RequestContactSuiteProgressL( TInt aProfileId, MSyncMLProgressObserver& aProgressObserver )
    {
    RProcess loadedProcess;
    if( loadedProcess.SecureId() != KSmlContactSuiteAppUID )
        {
        return ;
        }
    
   if ( iContactSuiteCallBack == NULL )
        {
        // callback not yet created, create
        iContactSuiteCallBack = new (ELeave) CSmlContactSuiteActiveCallback( *this );            
        }

    Send( ECmdContactSuiteRegisterObserversWithProfileId, TIpcArgs (aProfileId) );
    iContactSuiteCallBack->SetProgressObserverL( aProgressObserver );               
    };
