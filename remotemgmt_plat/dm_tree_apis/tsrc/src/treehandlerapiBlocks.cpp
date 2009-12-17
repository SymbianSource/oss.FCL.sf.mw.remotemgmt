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
#include "treehandlerapi.h"
#include <nsmldmdbhandler.h>
#include <utf.h>


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
// Ctreehandlerapi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctreehandlerapi::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Ctreehandlerapi::ExampleL ),
        ENTRY( "CreateandDeletedbhandler", Ctreehandlerapi::CreateandDeletedbhandlerL ),
        ENTRY( "DbhandlerNewL", Ctreehandlerapi::DbhandlerNewL ),
        ENTRY( "Setserver", Ctreehandlerapi::SetserverL ),
        ENTRY( "AddMappingInfo", Ctreehandlerapi::AddMappingInfoL ),
        ENTRY( "GetMappingInfo", Ctreehandlerapi::GetMappingInfoL ),
        ENTRY( "RemoveMappingInfo", Ctreehandlerapi::RemoveMappingInfoL ),
        ENTRY( "RenameMapping", Ctreehandlerapi::RenameMappingL ),
        ENTRY( "GetURISegmentList", Ctreehandlerapi::GetURISegmentListL ),
        ENTRY( "UpdateMappingInfo", Ctreehandlerapi::UpdateMappingInfoL ),
        ENTRY( "UpdateAcl", Ctreehandlerapi::UpdateAclL ),
        ENTRY( "CheckAcl", Ctreehandlerapi::CheckAclL ),
        ENTRY( "DeleteAcl", Ctreehandlerapi::DeleteAclL ),
        ENTRY( "DefaultACLsToServer", Ctreehandlerapi::DefaultACLsToServerL ),
        ENTRY( "EraseServerId", Ctreehandlerapi::EraseServerIdL ),
        
         };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktreehandlerapi, "treehandlerapi" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Ktreehandlerapi, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ktreehandlerapi, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
// -----------------------------------------------------------------------------
// Ctreehandlerapi::CreateandDeletedbhandlerL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::CreateandDeletedbhandlerL()
    {
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	CleanupStack::PopAndDestroy();//nsmldmdbhandler
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::DbhandlerNewL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::DbhandlerNewL()
    {
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewL();
	delete nsmldmdbhandler ;//nsmldmdbhandler
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::SetserverL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::SetserverL( CStifItemParser& aItem )
    {
    TPtrC string;
    TInt err = aItem.GetNextString ( string );
    if( err )
    {
    	return KErrNotFound;
    }
	HBufC8* severId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	nsmldmdbhandler->SetServerL( *severId );
	nsmldmdbhandler->SetServerL( *severId );
	CleanupStack::PopAndDestroy();//nsmldmdbhandler
	delete severId;
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::AddMappingInfoL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::AddMappingInfoL( CStifItemParser& aItem )
    {
    TUint adapterId;
    TPtrC string1;
    TPtrC string2;
    TInt err = aItem.GetNextInt( adapterId );
    TInt err1 = aItem.GetNextString( string1 );
    TInt err2 = aItem.GetNextString( string2 );
    if( err || err1 || err2)
    {
    	return KErrNotFound;
    }
    
    HBufC8* aURI = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string1);
	HBufC8* aLuid = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string2);
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, *aURI, *aLuid );
	CleanupStack::PopAndDestroy();//nsmldmdbhandler
	delete aURI;
	delete aLuid;
	if(error)
	{
		return error;
	}
	else
	{
		return KErrNone;	
	}
    

    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::GetMappingInfoL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::GetMappingInfoL( CStifItemParser& aItem )
    {
    TUint adapterId;
    TPtrC string1;
    TPtrC string2;
    TInt err = aItem.GetNextInt( adapterId );
    TInt err1 = aItem.GetNextString( string1 );
    TInt err2 = aItem.GetNextString( string2 );
    if( err || err1 || err2)
    {
    	return KErrNotFound;
    }
    
    HBufC8* aURI = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string1);
	HBufC8* aLuid = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string2);
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, *aURI, *aLuid );
	
	HBufC8* getaLuid = NULL;
	TInt found = nsmldmdbhandler->GetMappingInfoLC( adapterId, *aURI, getaLuid );
	if(error)
	{
		return error;
	}
	
	TPtr8 ptrgetLuid = getaLuid->Des();
	TPtr8 ptraLuid = aLuid->Des();
	if( ptrgetLuid.Compare(ptraLuid))
	{
		CleanupStack::PopAndDestroy();//getaLuid
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		delete aURI;
		delete aLuid;
		return KErrGeneral;
	}
	else
	{
		CleanupStack::PopAndDestroy();//getaLuid
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		delete aURI;
		delete aLuid;
		return KErrNone;
	}

    }


// -----------------------------------------------------------------------------
// Ctreehandlerapi::RemoveMappingInfoL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::RemoveMappingInfoL( )
    {
	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID001");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
	_LIT8(KChildnode1,"AP/APID001/NAP");
    TBufC8<25> childnode1( KChildnode1 );
    _LIT8(KChildnode1Luid,"2");
    TBufC8<25> childnode1Luid( KChildnode1Luid );
	
	_LIT8(KChildnode2,"AP/APID001/NAP/NAP001");
    TBufC8<25> childnode2( KChildnode2 );
    _LIT8(KChildnode2Luid,"3");
    TBufC8<25> childnode2Luid( KChildnode2Luid );
	
	_LIT8(KChildnode3,"AP/APID001/NAP/NAP002");
    TBufC8<25> childnode3( KChildnode3 );
    _LIT8(KChildnode3Luid,"4");
    TBufC8<25> childnode3Luid( KChildnode3Luid );

 

	_LIT8(KChildnode4,"AP/APID001/Px");
    TBufC8<25> childnode4( KChildnode4 );
    _LIT8(KChildnode4Luid,"5");
    TBufC8<25> childnode4Luid( KChildnode4Luid );
	
	_LIT8(KChildnode5,"AP/APID001/Px/Px01");
    TBufC8<25> childnode5 (KChildnode5);
    _LIT8(KChildnode5uid,"6");
    TBufC8<25> childnode5Luid( KChildnode5uid );
    
	_LIT8(KChildnode6,"AP/APID001/Px/Px02");
    TBufC8<25> childnode6 (KChildnode6);
    _LIT8(KChildnode6Luid,"7");
    TBufC8<25> childnode6Luid( KChildnode6Luid );



    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	TInt error1 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode1, childnode1Luid );
	TInt error2 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode2, childnode2Luid );
	TInt error3 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode3, childnode3Luid );
	TInt error4 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode4, childnode4Luid );
	TInt error5 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode5, childnode5Luid );
	TInt error6 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode6, childnode6Luid );

	nsmldmdbhandler->WriteMappingInfoToDbL();
	if(error || error1 || error2 || error3)
	{
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		return KErrGeneral;
	}
	
	TInt errorremove = nsmldmdbhandler->RemoveMappingInfoL(adapterId, childnode1, ETrue );

	if(errorremove)
	{
		CleanupStack::PopAndDestroy();
		return KErrGeneral;
	}

	nsmldmdbhandler->WriteMappingInfoToDbL();
	
	CArrayFix<TSmlDmMappingInfo>* aURISegList =	new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);

	TInt founduri1 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode1, *aURISegList );
	TInt founduri2 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode2, *aURISegList );
	TInt founduri3 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode3, *aURISegList );
	
	if(founduri1 != KErrNotFound || founduri2 != KErrNotFound || founduri3 != KErrNotFound)
	{
		CleanupStack::PopAndDestroy();
		delete  aURISegList;
		return KErrNotFound;
		
	}
	else
	{
		CleanupStack::PopAndDestroy();
		delete  aURISegList;
		return KErrNone;
		
	}

/*	TInt errorremove1 = nsmldmdbhandler->RemoveMappingInfoL(adapterId, childnode4, EFalse );
	if(errorremove1)
	{
		CleanupStack::PopAndDestroy();
		delete  aURISegList;
		return KErrGeneral;
	}
	
	nsmldmdbhandler->WriteMappingInfoToDbL();
	TInt founduri4 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode4, *aURISegList );
	TInt founduri5 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode5, *aURISegList );
	TInt founduri6 = nsmldmdbhandler->GetURISegmentListL( adapterId, childnode6, *aURISegList );
	
	if(founduri4 != KErrNotFound || founduri5 == KErrNotFound || founduri6 == KErrNotFound)
	{
		CleanupStack::PopAndDestroy();
		delete  aURISegList;
		return KErrGeneral;
		
	}

	CleanupStack::PopAndDestroy();
	delete  aURISegList;
	return KErrNone;		
    */
    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::RenameMappingL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::RenameMappingL()
    {
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID001/NAP/NAP001");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );


	_LIT8(Kreplacenode,"NAP004");
    TBufC8<25> replacenode( Kreplacenode );
    
    _LIT8(Kgetnode,"AP/APID001/NAP/NAP004");
    TBufC8<25> getnode(Kgetnode);
	HBufC8* getaLuid = NULL;
    
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	if( error )
	{
		return error;
	}
	TInt ret = nsmldmdbhandler->RenameMappingL( adapterId, rootnode, replacenode);
	TInt found = nsmldmdbhandler->GetMappingInfoLC( adapterId, getnode, getaLuid );
	
	
	TPtr8 ptrgetaLuid = getaLuid->Des();
	
	if(ret && ptrgetaLuid.Compare(rootnodeLuid) && (found == -1) )
	{
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		CleanupStack::PopAndDestroy();//getaLuid
		return KErrGeneral;
	}
	else
	{
		CleanupStack::PopAndDestroy();//getaLuid
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		return KErrNone;
	}

    }
 
// -----------------------------------------------------------------------------
// Ctreehandlerapi::GetURISegmentListL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::GetURISegmentListL()
    {
   	const TInt KGranularity = 8;
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID001");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
	_LIT8(KChildnode1,"AP/APID001/NAP");
    TBufC8<25> childnode1( KChildnode1 );
    _LIT8(KChildnode1Luid,"2");
    TBufC8<25> childnode1Luid( KChildnode1Luid );
	
	_LIT8(KChildnode2,"AP/APID001/Px");
    TBufC8<25> childnode2( KChildnode2 );
    _LIT8(KChildnode2Luid,"3");
    TBufC8<25> childnode2Luid( KChildnode2Luid );
	
	_LIT8(KChildnode3,"AP/APID001/Ax");
    TBufC8<25> childnode3( KChildnode3 );
    _LIT8(KChildnode3Luid,"4");
    TBufC8<25> childnode3Luid( KChildnode3Luid );


    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	TInt error1 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode1, childnode1Luid );
	TInt error2 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode2, childnode2Luid );
	TInt error3 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode3, childnode3Luid );

	if(error || error1 || error2 || error3)
	{
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		return KErrGeneral;
	}

	CArrayFix<TSmlDmMappingInfo>* aURISegList =	new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);
	
	TInt found = nsmldmdbhandler->GetURISegmentListL( adapterId, rootnode, *aURISegList );
	
	if( found != KErrNone)
	{
		CleanupStack::PopAndDestroy();
		return found;
	}
	TBufC8<25> getchildnode1 = aURISegList->At(0).iURISeg;
	TBufC8<25> getchildnode2 = aURISegList->At(1).iURISeg;
	TBufC8<25> getchildnode3 = aURISegList->At(2).iURISeg;
	
	_LIT8(KChildnode4,"NAP");
    TBufC8<25> childnode4( KChildnode4 );

	_LIT8(KChildnode5,"Px");
    TBufC8<25> childnode5( KChildnode5 );

	_LIT8(KChildnode6,"Ax");
    TBufC8<25> childnode6( KChildnode6 );
	TInt retValue;
	if( getchildnode1.Compare(childnode4) && getchildnode1.Compare(childnode5) && getchildnode1.Compare(childnode6))
	{		
		retValue = KErrGeneral;
	}
	else
	{		
		retValue = KErrNone;	
	}
	CleanupStack::PopAndDestroy();
	return retValue;
    }
    
// -----------------------------------------------------------------------------
// Ctreehandlerapi::UpdateMappingInfoL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::UpdateMappingInfoL()
    {
    
    const TInt KGranularity = 8;
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
    _LIT8(KChildnode1,"AP/APID002/NAP");
    TBufC8<25> childnode1( KChildnode1 );
    _LIT8(KChildnode1Luid,"2");
    TBufC8<25> childnode1Luid( KChildnode1Luid );
	
	_LIT8(KChildnode2,"AP/APID002/Px");
    TBufC8<25> childnode2( KChildnode2 );
    _LIT8(KChildnode2Luid,"3");
    TBufC8<25> childnode2Luid( KChildnode2Luid );
	
	_LIT8(KChildnode3,"AP/APID002/Ax");
    TBufC8<25> childnode3( KChildnode3 );
    _LIT8(KChildnode3Luid,"4");
    TBufC8<25> childnode3Luid( KChildnode3Luid );
    
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	TInt error1 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode1, childnode1Luid );
	TInt error2 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode2, childnode2Luid );
	TInt error3 = nsmldmdbhandler->AddMappingInfoL( adapterId, childnode3, childnode3Luid );
	nsmldmdbhandler->WriteMappingInfoToDbL();

	if(error || error1 || error2 || error3)
	{
		CleanupStack::PopAndDestroy();//nsmldmdbhandler
		return KErrGeneral;
	}
	

 _LIT8(KChildnode4,"NAP/Px");
    TBufC8<25> childnode4( KChildnode4 );
    
	CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);
    childList->InsertL(0,childnode4);
    
    
    nsmldmdbhandler->UpdateMappingInfoL(adapterId, rootnode, *childList); 
    nsmldmdbhandler->WriteMappingInfoToDbL();
    
   	CArrayFix<TSmlDmMappingInfo>* aURISegList =	new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);
	
	TInt found = nsmldmdbhandler->GetURISegmentListL( adapterId, rootnode, *aURISegList );
	
	if( found != KErrNone)
	{
		delete aURISegList;
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
		return found;
	}

	TBufC8<25> getchildnode1 = aURISegList->At(0).iURISeg;
	TBufC8<25> getchildnode2 = aURISegList->At(1).iURISeg;

	_LIT8(Knapnode,"NAP");
    TBufC8<25> napnode( Knapnode );

	_LIT8(KPxnode,"Px");
    TBufC8<25> Pxnode( KPxnode );

	
	if(getchildnode1.Compare(napnode) || getchildnode2.Compare(Pxnode) || aURISegList->Count() != 2)
	{
		delete aURISegList;
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
	 	return KErrGeneral;		
	}
	delete aURISegList;
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::UpdateAclL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::UpdateAclL()
    {
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
    _LIT8(KServerID,"funambool");
    TBufC8<25> serverID( KServerID );
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
   	nsmldmdbhandler->SetServerL( serverID );

	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );

	_LIT8(Kacl,"Add=funambool&Get=funambool");
    TBufC8<50> aACL( Kacl );
    
   	CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);

	nsmldmdbhandler->UpdateAclL(rootnode, aACL);
	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<27>getacl;
    childList->Read(0, getacl);
    TInt foundfun = getacl.Find(serverID);
 	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
    
	if(foundfun != KErrNotFound)
	{
		return KErrNone;	
	}
	else
	{
		return KErrNotFound;
	}
    
    }

// -----------------------------------------------------------------------------
// Ctreehandlerapi::CheckAclL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::CheckAclL( CStifItemParser& aItem )
	{
    TPtrC string;
    TInt err = aItem.GetNextString ( string );
    if( err )
    {
    	return KErrNotFound;
    }
	HBufC8* severId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	nsmldmdbhandler->SetServerL( *severId );
	
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	
	_LIT8(Kacl,"Add=funambool&Get=funambool");
    TBufC8<100> aACL( Kacl );

	nsmldmdbhandler->UpdateAclL(rootnode, aACL);
	TNSmlDmCmdType aCmdType = EAclGet;
	TBool isCMDtype1 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );
	aCmdType = EAclAdd;
	TBool isCMDtype2 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );
	aCmdType = EAclReplace;
	TBool isCMDtype3 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );
	aCmdType = EAclDelete;
	TBool isCMDtype4 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );

	_LIT8(Kacl1,"Add=hi&Get=hi");
    TBufC8<100> aACL1( Kacl1 );
    
   	nsmldmdbhandler->UpdateAclL(rootnode, aACL1);
	aCmdType = EAclGet;
	TBool isCMDtype5 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );
	aCmdType = EAclAdd;
	TBool isCMDtype6 = nsmldmdbhandler->CheckAclL( rootnode, aCmdType );

	CleanupStack::PopAndDestroy();//nsmldmdbhandler
	delete severId;

	if(isCMDtype1 == 1 && isCMDtype2 == 1 &&isCMDtype3 == 0 &&isCMDtype4 == 0 &&
	   isCMDtype5 == 0 && isCMDtype6 == 0 )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
    
	}

// -----------------------------------------------------------------------------
// Ctreehandlerapi::DeleteAclL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::DeleteAclL( )
	{
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
    _LIT8(KServerID,"funambool");
    TBufC8<25> serverID( KServerID );
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
   	nsmldmdbhandler->SetServerL( serverID );

	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	if( error )
	{
	CleanupStack::PopAndDestroy();	
	return error;
	}

	_LIT8(Kacl,"Add=funambool&Get=funambool&Replace=funambool&Delete=funambool&Exec=funambool");
    TBufC8<100> aACL( Kacl );
    
 
	nsmldmdbhandler->UpdateAclL(rootnode, aACL);

   	
    nsmldmdbhandler->DeleteAclL( rootnode );
	CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);
	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<18>getacl;
    childList->Read(0, getacl);
    childList->Reset();
    TInt foundfun = getacl.Find(serverID);

	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();

	if( foundfun == KErrNotFound )
	{
		return KErrNone;	
	}
	else
	{
		return KErrGeneral;
	}

	}

// -----------------------------------------------------------------------------
// Ctreehandlerapi::DefaultACLsToServerL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::DefaultACLsToServerL( )
	{
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    
    _LIT8(KServerID,"funambool");
    TBufC8<25> serverID( KServerID );
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
   	nsmldmdbhandler->SetServerL( serverID );

	TInt error = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	if( error )
	{
	CleanupStack::PopAndDestroy();	
	return error;
	}
	
	nsmldmdbhandler->DefaultACLsToServerL(rootnode);
	
	CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);
	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<77>getacl;
    childList->Read(0, getacl);
    childList->Reset();
    TInt foundfun = getacl.Find(serverID);

	
    _LIT8(KServerID1,"ServName");
    TBufC8<25> serverID1( KServerID1 );
   	nsmldmdbhandler->SetServerL( serverID1 );

	nsmldmdbhandler->DefaultACLsToServerL(rootnode);
	nsmldmdbhandler->GetAclL(rootnode, *childList);

    TBuf8<62>getacl1;
    childList->Read(0, getacl1);
    childList->Reset();
    TInt foundash = getacl1.Find(serverID1);
	
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	
	if(foundfun == KErrNotFound || foundash == KErrNotFound  )
	{
		return KErrNotFound;
	}
	else
	{
		return KErrNone;
	}
	}


// -----------------------------------------------------------------------------
// Ctreehandlerapi::EraseServerIdL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreehandlerapi::EraseServerIdL( )
	{
   	TUint32 adapterId = 10;
	_LIT8(KRootnode,"AP/APID002");
    TBufC8<25> rootnode( KRootnode );
	_LIT8(KRootnodeLuid,"1");
    TBufC8<25> rootnodeLuid( KRootnodeLuid );
    CNSmlDmDbHandler* nsmldmdbhandler = CNSmlDmDbHandler::NewLC();
	TInt error  = nsmldmdbhandler->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
	TInt error1 = nsmldmdbhandler->WriteMappingInfoToDbL();
	if( error || error1 )
	{
	CleanupStack::PopAndDestroy();	
	return error;
	}
	CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);

    _LIT8(KServerID1,"ServName");
    TBufC8<25> serverID1( KServerID1 );

    _LIT8(KServerID,"funambool");
    TBufC8<25> serverID( KServerID );


	_LIT8(Kacl,"Add=funambool&Get=funambool&Replace=ServName&Delete=ServName");
    TBufC8<100> aACL( Kacl );
    nsmldmdbhandler->UpdateAclL(rootnode, aACL);
    TInt error3 = nsmldmdbhandler->WriteAclInfoToDbL();
	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<56>getacl;
    childList->Read(0, getacl);
    childList->Reset();
    
    TInt foundfun = getacl.Find(serverID);
    TInt foundash = getacl.Find(serverID1);
    
    if( foundfun == KErrNotFound || foundash == KErrNotFound  )
    {
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
		return KErrNotFound;
    	
    }

	nsmldmdbhandler->EraseServerIdL( serverID1 );

	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<27>getacl1;
    childList->Read(0, getacl1);
    childList->Reset();
    
    foundfun = getacl1.Find(serverID);
   	foundash = getacl1.Find(serverID1);
   
    if( foundfun == KErrNotFound || foundash != KErrNotFound)
    {
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
		return KErrNotFound;
    	
    }
	nsmldmdbhandler->EraseServerIdL( serverID );
	nsmldmdbhandler->GetAclL(rootnode, *childList);
    TBuf8<18>getacl2;
    childList->Read(0, getacl2);
    childList->Reset();

    foundfun = getacl2.Find(serverID);
   	foundash = getacl2.Find(serverID1);
	

    if( foundfun != KErrNotFound || foundash != KErrNotFound)
    {
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
		return KErrNotFound;
    	
    }
	
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();

	return KErrNone;
	}


//  End of File
