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
#include "dmcertapi.h"
#include "dmcertapiClient.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cdmcertapi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cdmcertapi::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cdmcertapi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cdmcertapi::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cdmcertapi::ExampleL ),
        ENTRY( "GetCertificate", Cdmcertapi::GetCertificateL )
        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cdmcertapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdmcertapi::ExampleL( CStifItemParser& /*aItem*/ )
{
	/*RdmcertapiClient client;
	client.Connect();
	TInt count = client.testcall();
	client.Close();/
	
	TCertInfo ci;
    RDMCert dmcert;
    TInt ret = dmcert.Get( ci );*/
	
	
	return KErrNone;
}

TInt Cdmcertapi::GetCertificateL()
{
	RdmcertapiClient client;
	User::LeaveIfError(client.Connect());
	TInt err = client.GetCertificateL();
	client.Close();
	return err;
}

//  End of File
