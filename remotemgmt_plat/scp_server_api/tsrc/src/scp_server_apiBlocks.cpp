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
#include "scp_server_api.h"
#include "SCPClientTestClient.h"
#include "SCPClientTestClient_AL.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cscp_server_api::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cscp_server_api::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cscp_server_api::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cscp_server_api::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cscp_server_api::ExampleL ),
        ENTRY( "Constructor", Cscp_server_api::ConstructorL ),
        ENTRY( "Connect", Cscp_server_api::ConnectL ),
        ENTRY( "Version", Cscp_server_api::VersionL ),
        ENTRY( "GetCode", Cscp_server_api::GetCodeL ),
        ENTRY( "StoreCode", Cscp_server_api::SetCodeL ),
        ENTRY( "ChangeCode", Cscp_server_api::ChangeCodeL ),
        ENTRY( "SetPhoneLock", Cscp_server_api::SetPhoneLockL ),
        ENTRY( "GetLockState", Cscp_server_api::GetLockStateL ),
        ENTRY( "QueryAdminCmd", Cscp_server_api::QueryAdminCmdL ),
        ENTRY( "SetParam", Cscp_server_api::SetParamL ),
        ENTRY( "GetParam", Cscp_server_api::GetParamL ),
        ENTRY( "SecCodeQuery", Cscp_server_api::AuthenticateS60L ),
        ENTRY( "ChangeCodeRequest", Cscp_server_api::CodeChangeQueryL ),
        ENTRY( "CheckConfig", Cscp_server_api::CheckConfigL ),
        ENTRY( "PerformCleanUp", Cscp_server_api::PerformCleanUpL )        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cscp_server_api::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cscp_server_api::ExampleL( CStifItemParser& /*aItem*/ )
{
	/*RSCPClientTestClient client;
	client.Connect();
	TInt count = client.testcall();
	client.Close();/
	
	TCertInfo ci;
    RDMCert dmcert;
    TInt ret = dmcert.Get( ci );*/
	
	
	return KErrNone;
}

TInt Cscp_server_api::ConstructorL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.ConstructorCallL();
	client.Close();
	return err;
}

TInt Cscp_server_api::ConnectL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.ConnectCallL();
	client.Close();
	return err; 
}

TInt Cscp_server_api::VersionL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.VersionCallL();
	client.Close();
	return err;
}

TInt Cscp_server_api::GetCodeL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
  err = client.GetCodeCallL();
	client.Close();
	return err;   
}

TInt Cscp_server_api::SetCodeL()
{
	RSCPClientTestClient client;
  TInt err = client.Connect();
  err = client.SetCodeCallL();
	client.Close();
	return err;
}

TInt Cscp_server_api::ChangeCodeL()
{
	RSCPClientTestClient client;
  TInt err = client.Connect();
  err = client.ChangeCodeCallL();
	client.Close();
	return err;
}

TInt Cscp_server_api::SetPhoneLockL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
  err = client.SetPhoneLockCallL();
	client.Close();
	return err;    
}

TInt Cscp_server_api::GetLockStateL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.GetLockStateCallL();
	client.Close();
	return err;    
}

TInt Cscp_server_api::QueryAdminCmdL()
{
	RSCPClientTestClient client;
  TInt err = client.Connect();
	err = client.QueryAdminCmdCallL();
	client.Close();
	
	RSCPClientTestClient_AL client_al;
	err = client_al.Connect();
	err = client_al.QueryAdminCmdCallL();
	client_al.Close();
		
	return err;    
}

TInt Cscp_server_api::SetParamL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.SetParamCallL();
	client.Close();
	return err;    
}

TInt Cscp_server_api::GetParamL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.GetParamCallL();
	client.Close();
	
	RSCPClientTestClient_AL client_al;
	err = client_al.Connect();
	err = client_al.GetParamCallL();
	client_al.Close();
	
	return err;    
}

TInt Cscp_server_api::AuthenticateS60L()
{
    RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.AuthenticateS60CallL();
	client.Close();
	return err;
}

TInt Cscp_server_api::CodeChangeQueryL()
{
	RSCPClientTestClient client;
	TInt err = client.Connect();
	err = client.CodeChangeQueryCallL();
	client.Close();
	return err; 
}

TInt Cscp_server_api::CheckConfigL()
{
	RSCPClientTestClient_AL client;
	TInt err = client.Connect();
	err = client.CheckConfigCallL();
	client.Close();
	return err;	
}

TInt Cscp_server_api::PerformCleanUpL(CStifItemParser& aItem)
{
    RSCPClientTestClient client;
    TInt err = client.Connect();
    if(!err)
    {
        RArray<TUid> uids;
        TInt uid;

        while(!(err = aItem.GetNextInt(uid)))
        {
            uids.Append(TUid::Uid(uid));
        }   
        if(uids.Count())
        {
            TRAPD(lerr, (err = client.PerformCleanUpL(uids)));
            if(lerr)
                err = lerr;
        }
        else
            err = KErrGeneral;
    }
    client.Close();
    return err;
    
}


//  End of File
