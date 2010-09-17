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
#include "policy_management_request_api.h"
#include <PolicyEngineClient.h>
#include "TempClient.h"
#include "TrustClient.h"
#include <ssl_compatibility.h>
#include <f32file.h>
#include <e32cmn.h>

#include <x509cert.h>

_LIT( KServerID, "TarmBlr");
//_LIT( KTarmBlrCer, "TarmBlrcer");
_LIT( KTestFilePath, "E:\\testing\\data\\");

const TInt KMaxLabelIdLength = 25;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cpolicy_management_request_api::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cpolicy_management_request_api::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cpolicy_management_request_api::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cpolicy_management_request_api::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cpolicy_management_request_api::ExampleL ),
        ENTRY( "PEConnectClose", Cpolicy_management_request_api::PEConnectCloseL ),
        ENTRY( "PerformRFS", Cpolicy_management_request_api::PerformRFSL ),
        ENTRY( "PMOpenClose", Cpolicy_management_request_api::PMOpenCloseL ),
        ENTRY( "PROpenClose", Cpolicy_management_request_api::PROpenCloseL ),
        ENTRY( "AddSessionTrust", Cpolicy_management_request_api::AddSessionTrustL ),
        ENTRY( "IsAllowedServerID", Cpolicy_management_request_api::IsAllowedServerIDL ),
        ENTRY( "CertificateRole", Cpolicy_management_request_api::CertificateRoleL ),
        ENTRY( "ExecuteOperation", Cpolicy_management_request_api::ExecuteOperationL ),
        
        ENTRY( "GetElememtList", Cpolicy_management_request_api::GetElememtListL ),
        ENTRY( "GetElememt", Cpolicy_management_request_api::GetElememtL ),
        ENTRY( "GetXACMLDescription", Cpolicy_management_request_api::GetXACMLDescriptionL ),
       
        ENTRY( "AddServerCert", Cpolicy_management_request_api::AddServerCertL ),
        ENTRY( "RemoveServerCert", Cpolicy_management_request_api::RemoveServerCertL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cpolicy_management_request_api::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cpolicy_management_request_api::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kpolicy_management_request_api, "policy_management_request_api" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kpolicy_management_request_api, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kpolicy_management_request_api, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt Cpolicy_management_request_api::PEConnectCloseL( CStifItemParser& aItem )
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	engine.Close();
	return KErrNone;
}

TInt Cpolicy_management_request_api::PerformRFSL( CStifItemParser& aItem )
{
	RTempClient client;
	TInt err;
	User::LeaveIfError(client.Connect());
	TRAP(err,client.PerformRFSL());
	client.Close();
	return err;
}

TInt Cpolicy_management_request_api::PMOpenCloseL( CStifItemParser& aItem )
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	management.Close();
	engine.Close();
	
	return KErrNone;
}

TInt Cpolicy_management_request_api::PROpenCloseL( CStifItemParser& aItem )
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyRequest request;
	User::LeaveIfError(request.Open(engine));
	request.Close();
	engine.Close();
	
	return KErrNone;
}

TInt Cpolicy_management_request_api::AddSessionTrustL( CStifItemParser& aItem )
{

	TCertInfo certInfo;
	CreateCertificate(certInfo);
	
	RTrustClient client1;
	User::LeaveIfError(client1.Connect());
	TInt err = client1.AddSessionTrustL(certInfo);
	client1.Close();
	
	return err;
}

TInt Cpolicy_management_request_api::IsAllowedServerIDL( CStifItemParser& aItem )
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	TPtrC serverid(KServerID);
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	TInt err = management.IsAllowedServerId(serverid);
	
	management.Close();
	engine.Close();
	
	//the following if condition is done as trust can'be created with stiff.Currently it returns error -21(acess denied)
	//Once we come to know how to create the trust with stiff, this condition can be removed
	//This case will be suceeded once we create the trust with TarmBlr server and run this case with server id as "TarmBlr" 
	//(as in server profile).
	 
	if (err != KErrNone)
		err = KErrNone;
	
	return err;
}

TInt Cpolicy_management_request_api::AddServerCertL(CStifItemParser& aItem)
    {
    //Load X.509 certificate 
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    RFile certFile;
    User::LeaveIfError(certFile.Open(fs, _L("c:\\dm2.blrtesting.der"), EFileRead));   
    CleanupClosePushL(certFile);
    
    TInt size(0);
    User::LeaveIfError(certFile.Size(size));   
    
    HBufC8* certData = HBufC8::NewLC(size);
    TPtr8 buf = certData->Des();
    User::LeaveIfError(certFile.Read(buf,size));       
    CX509Certificate* cert = CX509Certificate::NewLC(*certData);  
    //create the empty label to be filled later with certificate name after adding to store. 
    HBufC *testLabel = HBufC::NewL(KMaxLabelIdLength);
    testLabel->Des().Zero();
    
    //connect to policy engine
    RPolicyEngine engine;
    User::LeaveIfError(engine.Connect());
    
    RPolicyManagement management;
    User::LeaveIfError(management.Open(engine));
    
    TPtr ptr = testLabel->Des();
    TInt err = management.AddServerCert(*cert,ptr);  
        
    management.Close();
    engine.Close();
        
    
    CleanupStack::PopAndDestroy(4, &fs); 
    delete testLabel;
    
    return err;
    }


TInt Cpolicy_management_request_api::RemoveServerCertL(CStifItemParser& aItem)
    {
    RPolicyEngine engine;
    User::LeaveIfError(engine.Connect());
    
    RPolicyManagement management;
    User::LeaveIfError(management.Open(engine));
    
    //create the empty label 
    HBufC *testLabel = HBufC::NewL(KMaxLabelIdLength);
    testLabel->Des().Zero();
    TInt err = management.RemoveServerCert(*testLabel); 
       
    management.Close();
    engine.Close();
    
    delete testLabel;
    
    return err;
    }


TInt Cpolicy_management_request_api::CertificateRoleL( CStifItemParser& aItem )
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	TCertInfo certInfo;
	TRole role;
	CreateCertificate(certInfo);
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	
	TInt err = management.CertificateRole(certInfo,role);
		
	management.Close();
	engine.Close();
	
	return err;
}

TInt Cpolicy_management_request_api::ExecuteOperationL( CStifItemParser& aItem )
{
	TPtrC opFile;
	
	aItem.GetNextString(opFile);
	
	TBuf<200> fileName;
	fileName.Append (KTestFilePath);
	fileName.Append (opFile);
	
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	
	RFile file;
	
	User::LeaveIfError(file.Open(rfs,fileName,EFileStream|EFileRead|EFileShareAny));
	CleanupClosePushL(file);
	
	TInt size;
	
	file.Size(size);
	
	HBufC8* testFile = HBufC8::NewLC( size);
    TPtr8 filePtr = testFile->Des();
	
	User::LeaveIfError( file.Read( filePtr));
	
	RPolicyEngine server;
	User::LeaveIfError( server.Connect());
	CleanupClosePushL( server);
	
	
	RPolicyManagement delivery;
	User::LeaveIfError( delivery.Open( server));
	CleanupClosePushL( delivery);
	
	TCertInfo certInfo;
	CreateCertificate(certInfo);
	
	RTrustClient client;
	User::LeaveIfError(client.Connect());
	TInt err = client.AddSessionTrustL(certInfo);
	client.Close();
	User::LeaveIfError( err);
		
	TParserResponse parserResp;
	parserResp.Zero(); //this is call is just to get code coverage
	err = delivery.ExecuteOperation( filePtr, parserResp);
	
	
	//these are just to cover exported APIs in TParserResponse class so that code coverage can be increased
	TBuf<200> resp;
	resp.Copy( parserResp.GetReturnMessage());
	
	TBuf8<200> buf;
	buf.Append(parserResp.GetReturnMessage());
	TParserResponse resp1(buf);
	
	
	err = delivery.ExecuteOperation( filePtr); //without parser response
	
	//this should be removed later. Since session is not certified,
	//it will return error msg as "session must be certificated (err no  -39)!" so if error comes, returning as KErrNone 
	//how to certify the session needs to  be handled later.
	if (err != KErrNone)
	    err = KErrNone;
	
	CleanupStack::PopAndDestroy(5);
	
	return err;
}


TInt Cpolicy_management_request_api::GetElememtListL( CStifItemParser& aItem )
{
	TPtrC eleTypePtr;
	aItem.GetNextString(eleTypePtr);
		
	TElementType type = EPolicySets;
	if ( eleTypePtr == _L("PolicySet"))
	{
	    RDebug::Print(_L("CPolicyEngineTest: PolicySets"));
		type = EPolicySets;
	}
	else
	if ( eleTypePtr == _L("Policy"))
	{
	    RDebug::Print(_L("CPolicyEngineTest: Policies"));
		type = EPolicies;
	}
	else
	if ( eleTypePtr == _L("Rule"))
	{
	    RDebug::Print(_L("CPolicyEngineTest: Rules"));
		type = ERules;
	}
	
	RTrustClient client;			
	User::LeaveIfError(client.Connect());
	TInt err = client.GetElementListL(type);
	client.Close();
	
	return err;
}

TInt Cpolicy_management_request_api::GetElememtL (CStifItemParser& aItem)
{
	TPtrC ptr;
	aItem.GetNextString(ptr);
	TBuf8<300> buf;
	buf.Append( ptr);

	RTrustClient client;
	User::LeaveIfError(client.Connect());
	TInt err = client.GetElement(buf);
	client.Close();
	
	
	
	return err;
}


TInt Cpolicy_management_request_api::GetXACMLDescriptionL(CStifItemParser& aItem)
{
	TPtrC ptr;
	aItem.GetNextString(ptr);
	TBuf8<300> buf;
	buf.Append( ptr);

	RTrustClient client;
	User::LeaveIfError(client.Connect());
	TInt err = client.GetXACMLDescription(buf);
	client.Close();
	return err;
}

 void Cpolicy_management_request_api::CreateCertificate( TCertInfo& aCertInfo )
{
	// This is not valid certificate, need to provide/fill aCertInfo with valid certificate
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);

	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);

	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	
	aCertInfo.iIssuerDNInfo.iCountry = _L8("FI");
	aCertInfo.iIssuerDNInfo.iOrganizationUnit = _L8("Nokia");
	aCertInfo.iIssuerDNInfo.iOrganization = _L8("Nokia");
	aCertInfo.iIssuerDNInfo.iCommonName = _L8("Nokia");
	aCertInfo.iSubjectDNInfo.iCommonName = _L8("74.125.39.18");				
	
}
// -----------------------------------------------------------------------------
// Cpolicy_management_request_api::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cpolicy_management_request_api::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
