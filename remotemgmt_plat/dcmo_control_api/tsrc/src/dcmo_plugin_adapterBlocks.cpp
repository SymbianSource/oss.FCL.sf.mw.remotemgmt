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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "dcmo_plugin_adapter.h"
#include "dcmoclient.h"
#include "dcmoconst.h"

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
// Cdcmo_plugin_adapter::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cdcmo_plugin_adapter::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cdcmo_plugin_adapter::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cdcmo_plugin_adapter::ExampleL ),
        ENTRY( "GetIntValue", Cdcmo_plugin_adapter::GetIntValueL ),        
        ENTRY( "GetStrValue", Cdcmo_plugin_adapter::GetStrValueL ),
        ENTRY( "SetIntValue", Cdcmo_plugin_adapter::SetIntValueL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdcmo_plugin_adapter::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kdcmo_plugin_adapter, "dcmo_plugin_adapter" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kdcmo_plugin_adapter, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kdcmo_plugin_adapter, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::GetIntValueL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdcmo_plugin_adapter::GetIntValueL( CStifItemParser& aItem )
    {
		
		TPtrC category;
		TInt node;
  	if( aItem.GetNextString ( category ) == KErrNone )
  	{   		
  		if( aItem.GetNextInt ( node ) == KErrNone )
  		{ 
				TBuf<50> categoryValue;
				categoryValue.Copy(category);
  			RDCMOClient* dcmoClient = RDCMOClient::NewL();
   			if( dcmoClient)
   			{
   		  TInt value;
   			dcmoClient->GetDCMOIntAttributeValue(categoryValue, (TDCMONode)node, value);
   			delete dcmoClient;
   			return KErrNone;
   			}
			}
		}    

    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::GetStrValueL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdcmo_plugin_adapter::GetStrValueL( CStifItemParser& aItem )
    {
		
		TPtrC category;
		TInt node;
  	if( aItem.GetNextString ( category ) == KErrNone )
  	{ 
  		if( aItem.GetNextInt ( node ) == KErrNone )
  		{ 
  			TBuf<50> categoryValue;
				categoryValue.Copy(category);
  			RDCMOClient* dcmoClient = RDCMOClient::NewL();
   			if( dcmoClient)
   			{
   		  TBuf<255> strValue;
   			dcmoClient->GetDCMOStrAttributeValue(categoryValue, (TDCMONode)node, strValue);
   			delete dcmoClient;
   			return KErrNone;
   			}
			}
		}    

    return KErrNone;

    }    

// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::SetIntValueL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdcmo_plugin_adapter::SetIntValueL( CStifItemParser& aItem )
    {
		
		TPtrC category;
		TInt node;
  	if( aItem.GetNextString ( category ) == KErrNone )
  	{ 
  		if( aItem.GetNextInt ( node ) == KErrNone )
  		{ 
  			TBuf<50> categoryValue;
				categoryValue.Copy(category);
  			RDCMOClient* dcmoClient = RDCMOClient::NewL();
   			if( dcmoClient)
   			{
   		  TInt value;
   		  if( aItem.GetNextInt ( value ) == KErrNone )
  			{
   		  	dcmoClient->SetDCMOIntAttributeValue(categoryValue, (TDCMONode)node, value);
   				delete dcmoClient;
   				return KErrNone;
   			}
   			}
			}
		}    

    return KErrNone;

    }


// -----------------------------------------------------------------------------
// Cdcmo_plugin_adapter::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cdcmo_plugin_adapter::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
