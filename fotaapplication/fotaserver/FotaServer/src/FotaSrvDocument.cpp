/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Fotaserver document class
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include <collate.h>
#include <featmgr.h>
#include <fotaserver.rsg>
#include "FotaSrvDocument.h"
#include "FotaSrvUI.h"
#include "FotaSrvDebug.h"
#include <e32property.h>
#include "FotaServer.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Desctructor
// ---------------------------------------------------------------------------
//
CFotaSrvDocument::~CFotaSrvDocument()
    {
    TInt err = RProperty::Delete(TUid::Uid(KFotaServerUid), KFotaDownloadActive);
    }


// ---------------------------------------------------------------------------
// CFotaSrvDocument::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CFotaSrvDocument::ConstructL()
    {
    TInt err1(KErrNone);
    _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
    _LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityDiskAdmin );
    TInt err  = RProperty::Define( TUid::Uid(KFotaServerUid),
                KFotaDownloadActive,
                RProperty::EInt,KReadPolicy,KWritePolicy); 
    if(err==0)
        {
        err1 =  RProperty::Set( TUid::Uid(KFotaServerUid),KFotaDownloadActive,KErrNotFound ); 
        }
        FLOG(_L(" [FotaServer] CFotaSrvDocument::ConstructL  : error in setting rproperty: %d %d"), err,err1);
	//iEikEnv = CEikonEnv::Static();	
    
    }

// ---------------------------------------------------------------------------
// CFotaSrvDocument::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFotaSrvDocument* CFotaSrvDocument::NewL( CAknApplication& aApp )
    {
    FLOG( _L( "[FotaServer] CFotaSrvDocument::NewL:" ) );

	CFotaSrvDocument* self = new (ELeave) CFotaSrvDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  //self
    return self;
	}

 
// ---------------------------------------------------------------------------
// CFotaSrvDocument::CreateAppUiL
// ---------------------------------------------------------------------------
//
CEikAppUi* CFotaSrvDocument::CreateAppUiL()
    {
    FLOG( _L( "[FotaServer] CFotaSrvDocument::CreateAppUiL:" ) );
    return new (ELeave) CFotaSrvUi;
	}


// End of File
