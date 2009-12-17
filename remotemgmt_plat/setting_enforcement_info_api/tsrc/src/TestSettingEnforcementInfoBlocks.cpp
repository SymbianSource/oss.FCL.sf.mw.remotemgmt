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
#include "TestSettingEnforcementInfo.h"

#include <SettingEnforcementInfo.h>


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
// CTestSettingEnforcementInfo::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestSettingEnforcementInfo::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestSettingEnforcementInfo::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CTestSettingEnforcementInfo::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        ENTRY( "Destructor", CTestSettingEnforcementInfo::DesCSettingEnforcementInfoL ),
        ENTRY( "NewL", CTestSettingEnforcementInfo::NewL),
        ENTRY( "EnforcementActive", CTestSettingEnforcementInfo::EnforcementActiveL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSettingEnforcementInfo::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KTestSettingEnforcementInfo, "TestSettingEnforcementInfo" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KTestSettingEnforcementInfo, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KTestSettingEnforcementInfo, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSettingEnforcementInfo::DesCSettingEnforcementInfoL ( CStifItemParser& aItem )
{
	CSettingEnforcementInfo* pSettingEnforcementInfo;
//	CleanupStack::PushL(pSettingEnforcementInfo);
	
	pSettingEnforcementInfo = CSettingEnforcementInfo::NewL();
//	CleanupStack::PopAndDestroy(pSettingEnforcementInfo);
	delete pSettingEnforcementInfo;
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSettingEnforcementInfo::NewL( CStifItemParser& aItem )
{
	CSettingEnforcementInfo* pSettingEnforcementInfo;
	//CleanupStack::PushL(pSettingEnforcementInfo);
	
	pSettingEnforcementInfo = CSettingEnforcementInfo::NewL();
	//CleanupStack::PopAndDestroy(pSettingEnforcementInfo);
	delete pSettingEnforcementInfo;
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSettingEnforcementInfo::EnforcementActiveL( CStifItemParser& aItem )
{
	KSettingEnforcements SetEnf;
	TInt err;
	
	CSettingEnforcementInfo* pSettingEnforcementInfo;
	
	pSettingEnforcementInfo = CSettingEnforcementInfo::NewL();
	
	TInt ret;	
	TBool flag = TRUE;
	TInt count = 0;
	TBool breakValue = TRUE;
	TInt Value;
	
	for ( ; breakValue; count++)
	{	
	 	err = aItem.GetNextInt( Value);
	  	if(err)
	  	{
	  		breakValue = FALSE;
	  		break; 
	  	}	
	  	SetEnf = (KSettingEnforcements) Value;
	 	flag = TRUE; 	
		ret = pSettingEnforcementInfo->EnforcementActive( SetEnf, flag);
		if(ret)
		{
			breakValue = FALSE;
		}
		if(breakValue)
		{
			flag = FALSE;
			ret = pSettingEnforcementInfo->EnforcementActive( SetEnf, flag);
			if(ret)
			{
				breakValue = FALSE;
			}
		}
	}
	if(!count)
		return KErrGeneral;
	delete pSettingEnforcementInfo;
	if(breakValue)
	{
		return KErrGeneral;
	}
	else
	{	
		return KErrNone;	
	}
}


// -----------------------------------------------------------------------------
// CTestSettingEnforcementInfo::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CTestSettingEnforcementInfo::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
