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
#include <nsmldmmodule.h>
#include "treemoduleapi.h"
#include "treemoduleapidata.h"

#include <nsmldmiapmatcher.h>
#include <nsmldmdbhandler.h>
#include <utf.h>

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
// Ctreemoduleapi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctreemoduleapi::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctreemoduleapi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Ctreemoduleapi::ExampleL ),
        ENTRY( "TreemoduleNew", Ctreemoduleapi::TreemoduleNewL ),
        ENTRY( "Setserver", Ctreemoduleapi::SetserverL ),
        ENTRY( "AddObject", Ctreemoduleapi::AddObjectL ),
        ENTRY( "UpdateObject", Ctreemoduleapi::UpdateObjectL ),
        ENTRY( "FetchObject", Ctreemoduleapi::FetchObjectL ),
        ENTRY( "DeleteObject", Ctreemoduleapi::DeleteObjectL ),
        ENTRY( "AtomicOperation", Ctreemoduleapi::AtomicOperationL ),
        ENTRY( "CopyObjectL", Ctreemoduleapi::CopyObjectL),
        
        
        ENTRY( "RollBackL", Ctreemoduleapi::RollBackL),
        ENTRY( "RollBackL2", Ctreemoduleapi::RollBackL2),
        ENTRY( "EndMessageL", Ctreemoduleapi::EndMessageL),
        ENTRY( "IsDDFChangedL", Ctreemoduleapi::IsDDFChangedL),
        ENTRY( "MoreDataL", Ctreemoduleapi::MoreDataL),
        ENTRY( "DisconnectFromOtherServers", Ctreemoduleapi::DisconnectFromOtherServersL),
        ENTRY( "MarkGenAlertsSentL", Ctreemoduleapi::MarkGenAlertsSentL),
        ENTRY( "dmMatcherNewL", Ctreemoduleapi::dmMatcherNewL),
        ENTRY( "dmMatcherNewLC", Ctreemoduleapi::dmMatcherNewLC),
        
        ENTRY( "dmMatcherIAPIdFromURIL", Ctreemoduleapi::dmMatcherIAPIdFromURIL),
        ENTRY( "dmMatcherURIFromIAPIdL", Ctreemoduleapi::dmMatcherURIFromIAPIdL),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktreemoduleapi, "treemoduleapi" );
    _LIT( KExample, "In Example" );
    
    TestModuleIf().Printf( 0, Ktreemoduleapi, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ktreemoduleapi, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
// DM-Matcher test methods
TInt Ctreemoduleapi::dmMatcherNewL()
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

TInt Ctreemoduleapi::dmMatcherNewLC()
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

TInt Ctreemoduleapi::dmMatcherIAPIdFromURIL()
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

TInt Ctreemoduleapi::dmMatcherURIFromIAPIdL()
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
	

//	TInt luid = 10;
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

// DM-Module test methods

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::TreemoduleNewL()
    {
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::SetserverL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	_LIT8(KserverId,"funambool");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
    return KErrNone;
	
}

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::AddObjectL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/MyNode900");
  	TBufC8<25> aURI( KaURI );

	_LIT8(KaObject,"");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);
	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);
	status = callBack->iStatuscode;
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	if(status != 418)
    	return KErrGeneral;

	return KErrNone;

}


// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::UpdateObjectL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/MyNode901");
  	TBufC8<40> aURI( KaURI );

	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

 	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

 //	_LIT8(KaURI1,"./DMAcc/MyNode72/PkgName");
 	_LIT8(KaURI1,"./DMAcc/MyNode901/Name");
  	TBufC8<40> aURI1( KaURI1 );
	
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;

}

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::FetchObjectL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/MyNode902");
  	TBufC8<40> aURI( KaURI );

	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

 	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

 	_LIT8(KaURI1,"./DMAcc/MyNode902/Name");
  	TBufC8<40> aURI1( KaURI1 );
	
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	TInt aResultsRef = 1;
	dmModule->FetchObjectL(aURI1,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	if( status != 200 || callBack->iObject.Match(aObject) == KErrNotFound)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;

}

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreemoduleapi::DeleteObjectL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/Node903");
  	TBufC8<40> aURI( KaURI );

	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

 	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

 	//_LIT8(KaURI1,"./DMAcc/Node27/PkgName");
 	_LIT8(KaURI1,"./DMAcc/Node903/Name");
  	TBufC8<40> aURI1( KaURI1 );
	
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	TInt aResultsRef = 1;
	dmModule->DeleteObjectL(aURI,aStatusRef);
	status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

	dmModule->FetchObjectL(aURI1,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	if( status != 404 )
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;

}
// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctreemoduleapi::AtomicOperationL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/Node904");
  	TBufC8<40> aURI( KaURI );

	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->StartTransactionL();
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

//	_LIT8(KaURI1,"./DMAcc/Node137/PkgName");
	_LIT8(KaURI1,"./DMAcc/Node904/Name");
  	TBufC8<40> aURI1( KaURI1 );
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	aStatusRef = 2;
//	dmModule->StartTransactionL();
	dmModule->DeleteObjectL(aURI,aStatusRef);
	TInt aResultsRef = 2;
	dmModule->FetchObjectL(aURI1,aType,aResultsRef,aStatusRef);
//	dmModule->CommitTransactionL();
//	dmModule->EndMessageL();
	status = callBack->iStatuscode;
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;
}
// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctreemoduleapi::CopyObjectL()
{

	// Source
	
	CNSmlDmCallbackTest* callBack = CNSmlDmCallbackTest::NewL();
	CleanupStack::PushL(callBack);
	CNSmlDmModule* dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	_LIT8(KaURISrc, "./DMAcc/Node905");
	TBufC8<40> aURI (KaURISrc);
	
	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);


	//_LIT8(KaURI1,"./DMAcc/Node174/PkgName");
	_LIT8(KaURI1,"./DMAcc/Node905/Name");
  	TBufC8<40> aURI1( KaURI1 );
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

	
	_LIT8(KaURITarget, "./DMAcc/Node909");
	TBufC8<40> aURITarget (KaURITarget);
	
	aStatusRef = 1;


	//dmModule->SetServerL(serverId);
	//dmModule->StartTransactionL();
	dmModule->AddObjectL(aURITarget, aObject, aType,aStatusRef, aLargeItem);

	status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
		
	dmModule->CopyObjectL(aURITarget, aURI, aType, aStatusRef);

	status = callBack->iStatuscode;
	// Expected 405 CommandNotAllowed
	if( status != 405)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	
//	dmModule->CommitTransactionL();
//	dmModule->EndMessageL();
	

	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt Ctreemoduleapi::RollBackL( )
{
   CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/MyNode906");
  	TBufC8<25> aURI( KaURI );

	_LIT8(KaObject,"");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->StartTransactionL();
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

	TInt status = callBack->iStatuscode;
	dmModule->RollBackL();
	
	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	
	TInt aResultsRef = 1;
	dmModule->FetchObjectL(aURI,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;
}

// -----------------------------------------------------------------------------
// Ctreemoduleapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt Ctreemoduleapi::RollBackL2( )
{
/*
   CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/MyNode233");
  	TBufC8<25> aURI( KaURI );

	_LIT8(KaObject,"");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->StartTransactionL();
	dmModule->AddObjectL(aURI, aObject, aType, aStatusRef, aLargeItem);

  	_LIT8(KaURI2,"./DMAcc/MyNode234");
  	TBufC8<25> aURI2( KaURI2 );

	dmModule->AddObjectL(aURI2, aObject, aType, aStatusRef, aLargeItem);
	
  	_LIT8(KaURI3,"./DMAcc/MyNode235");
  	TBufC8<25> aURI3( KaURI3 );

	dmModule->AddObjectL(aURI3, aObject, aType, aStatusRef, aLargeItem);

	dmModule->RollBackL();

	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	
	TInt aResultsRef = 1;
	dmModule->FetchObjectL(aURI,aType,aResultsRef,aStatusRef);
	TInt status = callBack->iStatuscode;
	
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}

	dmModule->FetchObjectL(aURI2,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	
	dmModule->FetchObjectL(aURI3,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}


	_LIT8(KaURI4,"./DMAcc/MyNode238");
  	TBufC8<25> aURI4( KaURI4 );

	
	dmModule->AddObjectL(aURI4, aObject, aType, aStatusRef, aLargeItem);
	
	
	
	dmModule->FetchObjectL(aURI4,aType,aResultsRef,aStatusRef);
	status = callBack->iStatuscode;
	
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
*/
	return KErrNone;
}

TInt Ctreemoduleapi::EndMessageL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
  	_LIT8(KaURI,"./DMAcc/Node907");
  	TBufC8<40> aURI( KaURI );

	_LIT8(KaObject,"Software");
  	TBufC8<25> aObject( KaObject );

	_LIT8(KaType,"text/plain");
  	TBufC8<25> aType( KaType );
  	
  	TInt aStatusRef = 1;
  	TBool aLargeItem = EFalse;
  	
  	_LIT8(KserverId,"funambool1");
  	TBufC8<25> serverId( KserverId );

	dmModule->SetServerL(serverId);
	dmModule->StartTransactionL();
	dmModule->AddObjectL(aURI,aObject,aType,aStatusRef, aLargeItem);

	_LIT8(KaURI1,"./DMAcc/Node907/Name");
  	TBufC8<40> aURI1( KaURI1 );
	dmModule->UpdateObjectL(aURI1,aObject,aType,aStatusRef, aLargeItem);
	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	TInt status = callBack->iStatuscode;
	if( status != 200)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
	/*
	Question: Do I need to remove the object (URI)
	
	dmModule->StartTransactionL();
	dmModule->DeleteObjectL(aURI,aStatusRef);
	TInt aResultsRef = 1;
	dmModule->FetchObjectL(aURI1,aType,aResultsRef,aStatusRef);
	dmModule->CommitTransactionL();
	dmModule->EndMessageL();
	status = callBack->iStatuscode;
	if( status != 404)
	{	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return KErrGeneral;
	}
*/
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);

	return KErrNone;
}

TInt Ctreemoduleapi::IsDDFChangedL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	TRAPD(res, dmModule->IsDDFChangedL());
	
	if( res ) 
	{
		
	
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return res;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
	
	return KErrNone;
}

        
        // void MoreDataL(CBufBase*& aData);
TInt Ctreemoduleapi::MoreDataL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	CBufBase *data = CBufFlat::NewL(16);

	TRAPD(res, dmModule->MoreDataL(data));
	
	if (res)
	{
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return res;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

TInt Ctreemoduleapi::DisconnectFromOtherServersL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	TRAPD(res, dmModule->DisconnectFromOtherServers());
	
	if (res)
	{
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return res;
	}
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

TInt Ctreemoduleapi::MarkGenAlertsSentL()
{
    CNSmlDmCallbackTest *callBack =CNSmlDmCallbackTest::NewL(); 
	CleanupStack::PushL(callBack);
	CNSmlDmModule *dmModule = NULL;
	dmModule = CNSmlDmModule::NewL(callBack);
	CleanupStack::PushL(dmModule);
	
	TRAPD(res, dmModule->MarkGenAlertsSentL());
	
	if (res)
	{
		dmModule->DisconnectFromOtherServers();
		CleanupStack::PopAndDestroy(dmModule);
		CleanupStack::PopAndDestroy(callBack);
		return res;
	}
	
	dmModule->DisconnectFromOtherServers();
	CleanupStack::PopAndDestroy(dmModule);
	CleanupStack::PopAndDestroy(callBack);
	return KErrNone;
}

        
        
// -----------------------------------------------------------------------------
// Ctreemoduleapi::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Ctreemoduleapi::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
