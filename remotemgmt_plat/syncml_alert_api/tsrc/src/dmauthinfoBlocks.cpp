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



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "dmauthinfo.h"

// From NsmlDMAuthInfo.cpp
#include <nsmldmauthinfo.h>
//#include "nsmlsosserverdefs.h"

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
// Cdmauthinfo::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cdmauthinfo::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cdmauthinfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cdmauthinfo::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cdmauthinfo::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        ENTRY( "CNSmlDMAuthInfoCons", Cdmauthinfo::CNSmlDMAuthInfoCons),
        ENTRY( "CNSmlDMAuthInfoDes", Cdmauthinfo::CNSmlDMAuthInfoDes ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cdmauthinfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdmauthinfo::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kdmauthinfo, "dmauthinfo" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kdmauthinfo, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kdmauthinfo, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


 TInt Cdmauthinfo::CNSmlDMAuthInfoCons()
 { 	
 	CNSmlDMAuthInfo* dmauth = new CNSmlDMAuthInfo;
 	if(dmauth)
 		delete dmauth; 	
 	return KErrNone;	
 }
 
 TInt Cdmauthinfo::CNSmlDMAuthInfoDes()
 {
  CNSmlDMAuthInfo* dmauth;
 	dmauth = new CNSmlDMAuthInfo; 	
 	
 	delete dmauth;
 	return KErrNone; 	
 }
 
// -----------------------------------------------------------------------------
// Cdmauthinfo::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cdmauthinfo::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
