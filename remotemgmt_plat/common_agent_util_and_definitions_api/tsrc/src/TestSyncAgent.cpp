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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "TestSyncAgent.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestSyncAgent::CTestSyncAgent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestSyncAgent::CTestSyncAgent( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestSyncAgent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestSyncAgent::ConstructL()
    {
    iLog = CStifLogger::NewL( KTestSyncAgentLogPath, 
                          KTestSyncAgentLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

		//Creating Object for "CNSmlPhoneInfo" Class
		iPhoneInfo = CNSmlPhoneInfo::NewL();
    }

// -----------------------------------------------------------------------------
// CTestSyncAgent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestSyncAgent* CTestSyncAgent::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTestSyncAgent* self = new (ELeave) CTestSyncAgent( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CTestSyncAgent::~CTestSyncAgent()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

		if( iPhoneInfo )
		{
			delete iPhoneInfo;
    }

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

    return ( CScriptBase* ) CTestSyncAgent::NewL( aTestModuleIf );

    }


//  End of File
