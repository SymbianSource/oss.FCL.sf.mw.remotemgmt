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
* Description:  definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "testengineapis.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctestengineapis::Ctestengineapis
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Ctestengineapis::Ctestengineapis( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// Ctestengineapis::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Ctestengineapis::ConstructL()
    {
    iLog = CStifLogger::NewL( KtestengineapisLogPath, 
                          KtestengineapisLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    iProxies = new (ELeave) CDesC16ArrayFlat(1);

    }

// -----------------------------------------------------------------------------
// Ctestengineapis::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Ctestengineapis* Ctestengineapis::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    Ctestengineapis* self = new (ELeave) Ctestengineapis( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
Ctestengineapis::~Ctestengineapis()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
    
    //delete iEngine
    if (iEngine)
        {
        delete iEngine;
        }
    
    //delete iObserver
    if (iObserver)
        {
        delete iObserver;
        }
    
    //delete iProxies
    delete iProxies;
    
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

    return ( CScriptBase* ) Ctestengineapis::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// CWPContextObserver::CWPContextObserver
// Default constructor.
// -----------------------------------------------------------------------------
//
CWPContextObserver::CWPContextObserver()
    {

    }

// -----------------------------------------------------------------------------
// CWPContextObserver::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CWPContextObserver::ConstructL(TInt data)
    {
    this->data = data;
    }

// -----------------------------------------------------------------------------
// CWPContextObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPContextObserver* CWPContextObserver::NewL(TInt data)
    {
    CWPContextObserver* self = new (ELeave) CWPContextObserver();
    CleanupStack::PushL(self);
    self->ConstructL(data);
    CleanupStack::Pop();

    return self;

    }

// -----------------------------------------------------------------------------
// CWPContextObserver::~CWPContextObserver
// Dummy Destructor.
// -----------------------------------------------------------------------------
//
CWPContextObserver::~CWPContextObserver()
    {

    }
// -----------------------------------------------------------------------------
// CWPContextObserver::ContextChangeL
// Dummy Implementation of Pure virtual function.
// -----------------------------------------------------------------------------
//
void CWPContextObserver::ContextChangeL(RDbNotifier::TEvent aEvent)
    {
    // Do nothing
    }
//  End of File
