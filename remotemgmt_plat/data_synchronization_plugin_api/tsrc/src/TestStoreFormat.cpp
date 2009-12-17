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



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "TestStoreFormat.h"
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestStoreFormat::CTestStoreFormat
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestStoreFormat::CTestStoreFormat( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestStoreFormat::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestStoreFormat::ConstructL()
    {
      //do nothing
    }

// -----------------------------------------------------------------------------
// CTestStoreFormat::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestStoreFormat* CTestStoreFormat::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTestStoreFormat* self = new (ELeave) CTestStoreFormat( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CTestStoreFormat::~CTestStoreFormat()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    // delete iLog; 

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CTestStoreFormat::NewL( aTestModuleIf );

    }


//  End of File
