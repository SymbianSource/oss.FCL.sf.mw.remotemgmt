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
* Description:  Contains definitions for some functions of CTestHistoryLog class
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "TestHistoryLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestHistoryLog::CTestHistoryLog
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTestHistoryLog::CTestHistoryLog( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestHistoryLog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestHistoryLog::ConstructL()
    {
    iLog = CStifLogger::NewL( KTestHistoryLogLogPath, 
                          KTestHistoryLogLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// -----------------------------------------------------------------------------
// CTestHistoryLog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestHistoryLog* CTestHistoryLog::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTestHistoryLog* self = new (ELeave) CTestHistoryLog( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self; 
    }

// -----------------------------------------------------------------------------
// CTestHistoryLog::~CTestHistoryLog
// Destructor, to clean all resources
// -----------------------------------------------------------------------------
// 
CTestHistoryLog::~CTestHistoryLog()
    {  
    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iLog;      
    }    

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    return ( CScriptBase* ) CTestHistoryLog::NewL( aTestModuleIf );
        
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);
    
    }
#endif // EKA2
    
//  End of File
