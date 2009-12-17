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
#include <ApSelect.h>
#include <ApUtils.h>
#include <collate.h>
#include <featmgr.h>
#include <fotaserver.rsg>
#include "FotaSrvDocument.h"
#include "FotaSrvUI.h"
#include "FotaSrvDebug.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Desctructor
// ---------------------------------------------------------------------------
//
CFotaSrvDocument::~CFotaSrvDocument()
    {
    }


// ---------------------------------------------------------------------------
// CFotaSrvDocument::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CFotaSrvDocument::ConstructL()
    {
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
