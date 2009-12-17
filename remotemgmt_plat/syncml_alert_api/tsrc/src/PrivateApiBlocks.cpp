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
#include <SyncMLDef.h>
#include <SyncMLClientDM.h>
#include <e32svr.h>


#include "PrivateApi.h"
#include "NSmlDMAuthInfo.h"
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPrivateApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CPrivateApi::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CPrivateApi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPrivateApi::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CPrivateApi::ExampleL ),
        ENTRY( "StartSession", CPrivateApi::StartSessionL ),
        ENTRY( "CloseSession", CPrivateApi::CloseSession ),
        ENTRY( "SendData", CPrivateApi::SendDataL ),
        ENTRY( "SendDMAuthData", CPrivateApi::SetDMAuthDataL),
        ENTRY( "AddDMGenericAlert", CPrivateApi::AddDMGenericAlertL)
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// -----------------------------------------------------------------------------
// CPrivateApi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPrivateApi::ExampleL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("PrivateApi"), _L("In Example") );
    // Print to log file
    iLog->Log(_L("In Example"));

    TInt i = 0;
    TPtrC string;
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, _L("PrivateApi"), 
                                _L("Param[%i]: %S"), i, &string );
        i++;
        }

    return KErrNone;
    
    }
    
TInt CPrivateApi::StartSessionL( CStifItemParser& /*aItem*/ )
	{
	// Print to UI
    TestModuleIf().Printf( 0, _L("PrivateApi"), _L("Start session") );
    // Print to log file
    iLog->Log(_L("CPrivateApi::StartSession"));

    iPrivateApi.OpenL();

    return KErrNone;
	}

TInt CPrivateApi::CloseSession( CStifItemParser& /*aItem*/ )
	{
	// Print to UI
    TestModuleIf().Printf( 0, _L("PrivateApi"), _L("Close session") );
    // Print to log file
    iLog->Log(_L("CPrivateApi::CloseSession"));

	iPrivateApi.Close();
	return KErrNone;
	}
	
TInt CPrivateApi::SendDataL( CStifItemParser& /*aItem*/ )
	{
	TestModuleIf().Printf( 0, _L("PrivateApi"), _L("Send data") );
    // Print to log file
    iLog->Log(_L("CPrivateApi::SendData"));

	_LIT(KServerAlertFile,"C:\\TestFrameWork\\privateAPI_11.wbxml");	
	RFs fsSession;
	User::LeaveIfError ( fsSession.Connect() );
	RFile alertFile;
	TInt err = alertFile.Open ( fsSession, KServerAlertFile, EFileShareAny );
	if ( err == KErrNone )
	{
		HBufC8* package = HBufC8::NewLC(5000);
		TPtr8 p = package->Des();
		alertFile.Read( p, 1000 );
		alertFile.Close();
		fsSession.Close();
    
    iPrivateApi.SendL(p, ESmlDataSync, ESmlVersion1_1_2);
    
    CleanupStack::PopAndDestroy(); //data    
  }  
	return KErrNone;
	}

TInt CPrivateApi::SetDMAuthDataL( CStifItemParser& /*aItem*/ )
	{
	TestModuleIf().Printf( 0, _L("PrivateApi"), _L("Send data") );
    // Print to log file
    iLog->Log(_L("CPrivateApi::SendData"));
	
	RSyncMLSession syncSession;
	syncSession.OpenL();
	CleanupClosePushL(syncSession);
	
	RSyncMLDevManProfile dmProfile;
	dmProfile.CreateL( syncSession );
	CleanupClosePushL( dmProfile );

    dmProfile.SetDisplayNameL(_L("####´51"));
    dmProfile.SetServerIdL(_L8("ServerID"));
    dmProfile.SetCreatorId(85);
    dmProfile.SetPasswordL(_L8("Password"));
    dmProfile.SetServerPasswordL(_L8("ServerPassword"));
    dmProfile.SetUserNameL(_L8("Username"));
	
	dmProfile.UpdateL();
	
	TSmlProfileId id = dmProfile.Identifier();
	
	CleanupStack::PopAndDestroy(); // dmProfile


	_LIT8(KServerNonce, "ServerNonce");
	_LIT8(KClientNonce, "ClientNonce");
	
	CNSmlDMAuthInfo info;
	
	info.iProfileId = id;
	info.iAuthPref = EFalse;
	info.iServerNonce = KServerNonce().AllocL();
	info.iClientNonce = KClientNonce().AllocL();

	iPrivateApi.SetDMAuthInfoL(info);
    
    CNSmlDMAuthInfo info2;
    info2.iProfileId = id;
    iPrivateApi.GetDMAuthInfoL(info2);
    
    
    
    
    
    CleanupStack::PopAndDestroy(); //syncsession
    
    
	return KErrNone;
	}
	
	
TInt CPrivateApi::AddDMGenericAlertL( CStifItemParser& /*aItem*/ )
	{
	_LIT8( KEmptyString, "");
	_LIT8( KLongString, "thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!thisislongdata!data!");
	_LIT8( KMgmtUri, "./FUMO/fwObject/Update");
	_LIT8( KMetaType, "org.fumo.this.is.a.test.meta.type");
	_LIT8( KMetaFormat, "text/plain");
	_LIT8( KCorrelator, "a1B2c3D4");
	
	// empty data
	TInt res = iPrivateApi.AddDMGenericAlertRequestL( KEmptyString, KEmptyString, KEmptyString, 0, KEmptyString );
	if ( res != KErrNotFound )
		{
		return KErrGeneral;
		}
	
	// normal data
	res = iPrivateApi.AddDMGenericAlertRequestL( KMgmtUri, KMetaType, KMetaFormat, 500, KCorrelator );
	if ( res != KErrNotFound )
		{
		return KErrGeneral;
		}

	// large data
	res = iPrivateApi.AddDMGenericAlertRequestL( KLongString, KLongString, KLongString, 1500000000, KLongString );
	if ( res != KErrNotFound )
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}
	



//  End of File
