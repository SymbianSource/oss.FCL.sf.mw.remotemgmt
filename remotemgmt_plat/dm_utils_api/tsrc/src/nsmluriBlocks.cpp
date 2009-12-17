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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "nsmluri.h"
#include "nsmldmuri.h"
#include <S32FILE.h>
#include <S32STOR.h>
#include <S32STD.h>
#include "BADESCA.h"
#include <utf.h>
#include <e32base.h>

// Added for nsmliapmatcher

#include "treemoduleapidata.h"

#include <nsmldmiapmatcher.h>

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
// Cnsmluri::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cnsmluri::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cnsmluri::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    	 TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cnsmluri::ExampleL ),
        ENTRY( "ParentURI", Cnsmluri::ParentURI ),
				ENTRY( "LastURISegment", Cnsmluri::LastURISegment ),
				ENTRY( "Removedotslash", Cnsmluri::Removedotslash ),
				ENTRY( "RemoveLastSeg", Cnsmluri::RemoveLastSeg ),
				ENTRY( "NumOfURISegs", Cnsmluri::NumOfURISegs),
				ENTRY( "URISeg", Cnsmluri::URISegL),
				ENTRY( "RemoveProp", Cnsmluri::RemovePropL),
        //ADD NEW ENTRY HERE
        // Added nsmliapmatcher
        ENTRY( "dmMatcherNewL", Cnsmluri::dmMatcherNewL),
        ENTRY( "dmMatcherNewLC", Cnsmluri::dmMatcherNewLC),
        
        ENTRY( "dmMatcherIAPIdFromURIL", Cnsmluri::dmMatcherIAPIdFromURIL),
        ENTRY( "dmMatcherURIFromIAPIdL", Cnsmluri::dmMatcherURIFromIAPIdL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cnsmluri::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Knsmluri, "nsmluri" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Knsmluri, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Knsmluri, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
      
// -----------------------------------------------------------------------------
// Cnsmluri::ParentURI
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::ParentURI(CStifItemParser& aItem )
	{
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	TBuf8<30> buf;
	TInt countval = 0;
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, string);
	
	TLex8 aLex(buf);
	aLex.Mark();
	if(buf[0]=='.')
	{
		   
	
		while( (aLex.Peek() != '\x00'))
		{
		
			if( aLex.Peek() == '/' && countval<3)
			{
				countval++;
				
			}
			if(countval == 2)
			break;
			
			aLex.Inc();
			
		}
	}
	
	else
	{
	
		while( (aLex.Peek() != '/'))
			aLex.Inc();
	}
	
	
		TPtrC8	aPtr = aLex.MarkedToken();
		TPtrC8  aPtr1(NSmlDmURI::ParentURI(buf));
		if(aPtr.Compare(aPtr1))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}
	
// -----------------------------------------------------------------------------
// Cnsmluri::LastURISegment
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::LastURISegment(CStifItemParser& aItem )
	{
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	TBuf8<30> buf;
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, string);
	
	TLex8 aLex(buf);
	TInt off=0;
	
	aLex.Mark();
	while( (aLex.Peek() != '\x00'))
	{
		aLex.Inc();
		if(aLex.Peek() == '/')
		{
			off=aLex.Offset();
			aLex.Inc();
			aLex.Mark();
		}
		
	}
	
	if ( off) ; // to remove warning
	
	TPtrC8 aPtr = aLex.MarkedToken();	
			
	TPtrC8  aPtr1(NSmlDmURI::LastURISeg(buf));
	
		if(aPtr.Compare(aPtr1))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}

// -----------------------------------------------------------------------------
// Cnsmluri::Removedotslash
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::Removedotslash(CStifItemParser& aItem )
	{
	
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	TBuf8<30> buf;
	
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, string);
	TLex8 aLex(buf);
	aLex.Mark();	

	if(buf[0]=='.'&& buf[1]=='/')
	{
		   
		aLex.Inc();
		aLex.Inc();
		aLex.Mark();
		while ((aLex.Peek()!= '\x00'))
			aLex.Inc();
		
	}
	
	else
	{
		while ((aLex.Peek()!= '\x00'))
			aLex.Inc();
		
	}
	
	
	TPtrC8 aPtr = aLex.MarkedToken();
			
	TPtrC8  aPtr1(NSmlDmURI::RemoveDotSlash(buf));

		if(aPtr.Compare(aPtr1))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}


// -----------------------------------------------------------------------------
// Cnsmluri::RemoveLastSeg
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::RemoveLastSeg(CStifItemParser& aItem )
	{
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	TBuf8<30> buf;
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, string);
	
	TLex8 aLex(buf);
	TInt off=0;

	while( (aLex.Peek() != '\x00'))
	{
		aLex.Inc();
		if(aLex.Peek() == '/')
		{
			off=aLex.Offset();
			aLex.Inc();
			aLex.Mark();
		}
		
	}
	
	TInt i=0;
	TLex8  aLex1(buf);
	aLex1.Mark();
	while(i!= off)
	{
		aLex1.Inc();
		i++;
		
	}
	
	TPtrC8 aPtr = aLex1.MarkedToken();	
	
	
			
	TPtrC8  aPtr1(NSmlDmURI::RemoveLastSeg(buf));
	
		if(aPtr.Compare(aPtr1))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}
// -----------------------------------------------------------------------------
// Cnsmluri::NumOfURISegs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cnsmluri::NumOfURISegs(CStifItemParser& aItem )
	{
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	TBuf8<30> buf;
	TInt CountNumURI = 1;
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, string);
	TLex8 aLex(buf);
	

	while( (aLex.Peek() != '\x00'))
		{
			aLex.Inc();
			if(aLex.Peek() == '/')
			{
			 CountNumURI++;
			}
		
		}
				
	TInt CountAPINumURI(NSmlDmURI::NumOfURISegs(buf));
	
	
		if(CountAPINumURI != CountNumURI)
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}


 // -----------------------------------------------------------------------------
// Cnsmluri::URISeg
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 
TInt Cnsmluri::URISegL(CStifItemParser& aItem )
	{
	
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	

	TInt aloc = 2;
	
	
	TBuf8<30> buf;

	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf,string );
	
	CArrayFix<TPtrC8>* aURIs = new(ELeave) CArrayFixFlat<TPtrC8>(10);
	
	CleanupStack::PushL(aURIs);
	TLex8 aLex(buf);
	
	TInt Index =0;
	aLex.Mark();
	while( (aLex.Peek() != '\x00'))
		{
			
			aLex.Inc();
			
			
			if(aLex.Peek() == '/')
			{
			 TPtrC8 aPtr = aLex.MarkedToken();
			 aURIs -> InsertL(Index, aPtr);
			 Index++;
			 aLex.Inc();
			 aLex.Mark();
			}
		
		} 
		
	if(aLex.Get()=='\0')
	{
		TPtrC8 aPtr3= aLex.MarkedToken();
		aURIs->InsertL(Index,aPtr3);	
	}
	
	
	TPtrC8 aPtrTest1(aURIs->At(aloc-1)); 
	
	TPtrC8  aPtrTest2(NSmlDmURI::URISeg(buf, aloc));
	
	CleanupStack::PopAndDestroy();
		if(aPtrTest1.Compare(aPtrTest2))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}

// -----------------------------------------------------------------------------
// Cnsmluri::RemoveProp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cnsmluri::RemovePropL(CStifItemParser& aItem )
	{
	
	
	TPtrC string;
	TInt error = aItem.GetNextString ( string );
	if( error != KErrNone )
			return error;
	
	
	
	TBuf8<30> buf;

	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf,string );
	
	CArrayFix<TPtrC8>* aURIs = new(ELeave) CArrayFixFlat<TPtrC8>(10);
	
	CleanupStack::PushL(aURIs);
	TLex8 aLex(buf);
	
	aLex.Mark();
	while( (aLex.Peek() != '\x00'))
		{
			
			aLex.Inc();
			
			
			if(aLex.Peek() == '?')
			  break;
			
		
		} 
		
	TPtrC8 aPtrTest1(aLex.MarkedToken()); 
	
	TPtrC8  aPtrTest2(NSmlDmURI::RemoveProp(buf));
	
	CleanupStack::PopAndDestroy();
		if(aPtrTest1.Compare(aPtrTest2))
			return KErrGeneral;
		else 
			return KErrNone;
    	
      
	}

// -----------------------------------------------------------------------------
// Cnsmluri::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cnsmluri::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// DM-Matcher test methods
TInt Cnsmluri::dmMatcherNewL()
{
	CMSmlDmCallbackTest*  callBack = CMSmlDmCallbackTest::NewL();
	CleanupStack::PushL(callBack);
	CNSmlDMIAPMatcher* dmMatcher = NULL;	
	dmMatcher = CNSmlDMIAPMatcher::NewL(callBack);
	CleanupStack::PushL(dmMatcher);
		
	CleanupStack::PopAndDestroy(dmMatcher);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

TInt Cnsmluri::dmMatcherNewLC()
{
	CMSmlDmCallbackTest*  callBack = CMSmlDmCallbackTest::NewL();
	CleanupStack::PushL(callBack);
	CNSmlDMIAPMatcher* dmMatcher = NULL;	
	dmMatcher = CNSmlDMIAPMatcher::NewLC(callBack);
	//CleanupStack::PushL(dmMatcher);
	
	CleanupStack::PopAndDestroy(dmMatcher);
	//CleanupStack::Pop(); //
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

TInt Cnsmluri::dmMatcherIAPIdFromURIL()
{

	CMSmlDmCallbackTest*  callBack = CMSmlDmCallbackTest::NewL();
	CleanupStack::PushL(callBack);
	CNSmlDMIAPMatcher* dmMatcher = NULL;	
	dmMatcher = CNSmlDMIAPMatcher::NewLC(callBack);
	
  	//_LIT8(KaURI,"./AP/NTMSAP2194660/NAPDef");
  	_LIT8(KaURI,"AP/NTMSAP2194660");
  	TBufC8<25> aURI( KaURI );

	TInt luid = 10;
	HBufC8* retURI = dmMatcher->URIFromIAPIdL( luid );
	
	TPtr8 prturi = retURI->Des(); 
	TInt retluid = dmMatcher->IAPIdFromURIL(prturi);
	
	if ( luid != retluid)
	{	
		CleanupStack::PopAndDestroy(dmMatcher);
		CleanupStack::PopAndDestroy(callBack);	
		return KErrGeneral;
	}
	CleanupStack::PopAndDestroy(dmMatcher);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;	
}

TInt Cnsmluri::dmMatcherURIFromIAPIdL()
{	
	CMSmlDmCallbackTest*  callBack = CMSmlDmCallbackTest::NewL();
	CleanupStack::PushL(callBack);
	CNSmlDMIAPMatcher* dmMatcher = NULL;	
	dmMatcher = CNSmlDMIAPMatcher::NewLC(callBack);		
	
 // 	_LIT8(KaURI,"./AP/NTMSAP2194660/NAPDef");
  	_LIT8(KaURI,"AP/NTMSAP2194660");
  	TBufC8<30> aURI( KaURI );

	//TPtr8 ptruri = aURI->Des(); 
	TInt retluid = dmMatcher->IAPIdFromURIL( aURI ); //ptruri);
	
	HBufC8* retURI = dmMatcher->URIFromIAPIdL( retluid );
	
	
	if ( !retURI->Compare(aURI))
	{	
		CleanupStack::PopAndDestroy(dmMatcher);
		CleanupStack::PopAndDestroy(callBack);	
		return KErrGeneral;
	}
	
	CleanupStack::PopAndDestroy(dmMatcher);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}


//  End of File
