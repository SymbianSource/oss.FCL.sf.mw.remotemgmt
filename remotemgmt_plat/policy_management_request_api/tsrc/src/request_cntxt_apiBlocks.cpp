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
#include <PolicyEngineXACML.h>
#include <PolicyEngineClient.h>

#include "request_cntxt_api.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Crequest_cntxt_api::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Crequest_cntxt_api::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Crequest_cntxt_api::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Crequest_cntxt_api::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Crequest_cntxt_api::ExampleL ),
        ENTRY( "Constructor", Crequest_cntxt_api::ConstructorL ),
        ENTRY( "Destructor", Crequest_cntxt_api::DestructorL ),
        ENTRY( "AddSub_Certificate", Crequest_cntxt_api::AddSub_CertificateL ),
        ENTRY( "AddSub_SID", Crequest_cntxt_api::AddSub_SidL ),
        ENTRY( "AddSubjectAttribute", Crequest_cntxt_api::AddSubjectAttributeL ),
        ENTRY( "AddActionAttribute", Crequest_cntxt_api::AddActionAttributeL ),
        ENTRY( "AddResourceAttribute", Crequest_cntxt_api::AddResourceAttributeL ),
        ENTRY( "AddEnvAttr", Crequest_cntxt_api::AddEnvAttrL ),
        ENTRY( "SetResponseValue", Crequest_cntxt_api::SetResponseValueL ),
        ENTRY( "GetResponseValue", Crequest_cntxt_api::GetResponseValueL )

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Crequest_cntxt_api::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Crequest_cntxt_api::ExampleL( CStifItemParser& /*aItem*/ )
{
	TInt err = KErrNone;
	return err;
}

TInt Crequest_cntxt_api::ConstructorL()
{
	TInt err = KErrNone;
 	TRequestContext request;
	return err;
}

TInt Crequest_cntxt_api::DestructorL()
{
	TInt err = KErrNone;
 	TRequestContext request;
	return err; 
}

TInt Crequest_cntxt_api::AddSub_CertificateL()
{
	TInt err = KErrNone;
	TRequestContext request;
 	
	TCertInfo info;
	CreateCertificate( info ); 	
	
 	request.AddSubjectAttributeL( PolicyEngineXACML::KTrustedSubject, info);

	return err;
}

TInt Crequest_cntxt_api::AddSub_SidL()
{
	TInt err = KErrNone;
	TRequestContext request;
 	
 	//_LIT(KUID,"0x101FB3E3");
 	TInt32 intSid = 270513123;
	TUid sid;// = intSid;
	sid.Uid(intSid);
	//sid = intSid;
		
 	request.AddSubjectAttributeL( PolicyEngineXACML::KThirdPartySecureIds, sid);
	return err;   
}

TInt Crequest_cntxt_api::AddSubjectAttributeL(CStifItemParser& aItem)
{
	TInt err = KErrNone;
    TPtrC param;
   	TBuf8<100> att_id;
   	TBuf8<100> value;
   	TBuf8<100> type;
   	TPtrC ptr;
   	
   	aItem.GetNextString ( param );
   	aItem.GetNextString( ptr);
   	att_id.Copy(ptr);
   	aItem.GetNextString( ptr);
   	value.Copy(ptr);
   	aItem.GetNextString( ptr);
   	type.Copy(ptr);
   
   	TBuf8<10> stringtype;
   	stringtype = _L8("string");
   	
   	if ( type == stringtype )
   	{
   		type = PolicyEngineXACML::KStringDataType;
   	}
   	else
   	{
   		type = PolicyEngineXACML::KBooleanDataType;
   	}
   	
   	TRequestContext request;
 	request.AddSubjectAttributeL( att_id, value, type);
	return err;
}

TInt Crequest_cntxt_api::AddActionAttributeL(CStifItemParser& aItem)
{
	TInt err = KErrNone;
	TPtrC param;
   	TBuf8<100> att_id;
   	TBuf8<100> value;
   	TBuf8<100> type;
   	TPtrC ptr;
   	
   	aItem.GetNextString ( param );
   	aItem.GetNextString( ptr);
   	att_id.Copy(ptr);
   	aItem.GetNextString( ptr);
   	value.Copy(ptr);
   	aItem.GetNextString( ptr);
   	type.Copy(ptr);
   
   	TBuf8<10> stringtype;
   	stringtype = _L8("string");
   	
   	if ( type == stringtype )
   	{
   		type = PolicyEngineXACML::KStringDataType;
   	}
   	else
   	{
   		type = PolicyEngineXACML::KBooleanDataType;
   	}
   	TRequestContext request;

 	request.AddActionAttributeL( att_id, value, type);
	return err;
}

TInt Crequest_cntxt_api::AddResourceAttributeL(CStifItemParser& aItem)
{
	TInt err = KErrNone;
	TPtrC param;
   	TBuf8<100> att_id;
   	TBuf8<100> value;
   	TBuf8<100> type;
   	TPtrC ptr;
   	
   	aItem.GetNextString ( param );
   	aItem.GetNextString( ptr);
   	att_id.Copy(ptr);
   	aItem.GetNextString( ptr);
   	value.Copy(ptr);
   	aItem.GetNextString( ptr);
   	type.Copy(ptr);
   
   	TBuf8<10> stringtype;
   	stringtype = _L8("string");
   	
   	if ( type == stringtype )
   	{
   		type = PolicyEngineXACML::KStringDataType;
   	}
   	else
   	{
   		type = PolicyEngineXACML::KBooleanDataType;
   	}
   	TRequestContext request;

 	request.AddResourceAttributeL( att_id, value, type);
	return err;    
}

TInt Crequest_cntxt_api::AddEnvAttrL(CStifItemParser& aItem)
{
	TInt err = KErrNone;
	TPtrC param;
   	TBuf8<100> att_id;
   	TBuf8<100> value;
   	TBuf8<100> type;
   	TPtrC ptr;
   	
   	aItem.GetNextString ( param );
   	aItem.GetNextString( ptr);
   	att_id.Copy(ptr);
   	aItem.GetNextString( ptr);
   	value.Copy(ptr);
   	aItem.GetNextString( ptr);
   	type.Copy(ptr);
   
   	TBuf8<10> stringtype;
   	stringtype = _L8("string");
   	
   	if ( type == stringtype )
   	{
   		type = PolicyEngineXACML::KStringDataType;
   	}
   	else
   	{
   		type = PolicyEngineXACML::KBooleanDataType;
   	}
	TRequestContext request;

 	request.AddEnvironmentAttributeL( att_id, value, type);
	return err;    
}

TInt Crequest_cntxt_api::SetResponseValueL(CStifItemParser& aItem)
{
    TPtrC param;
    TResponse response;
	while ( aItem.GetNextString ( param ) == KErrNone )
    {	
       	if ( param[0] == 'd')
       	{
       		response.SetResponseValue(EResponseDeny);
       	}
       	else if  ( param[0] == 'p')
       	{
       		response.SetResponseValue(EResponsePermit);
       	}
       	else if  ( param[0] == 'n')
       	{
       		response.SetResponseValue(EResponseNotApplicable);
       	}
       	else if  ( param[0] == 'i')
       	{
       		response.SetResponseValue(EResponseIndeterminate);
       	}
       	  	
    }
	TInt err = KErrNone;
	return err;    
}

TInt Crequest_cntxt_api::GetResponseValueL(CStifItemParser& aItem)
{
	TInt err = KErrNone;
	TPtrC param;
    TResponse response;
    aItem.GetNextString ( param );
	if ( param[0] == 'd')
    {
    	response.SetResponseValue(EResponseDeny);
    }
    else if  ( param[0] == 'p')
    {
    	response.SetResponseValue(EResponsePermit);
    }
    else if  ( param[0] == 'n')
    {
    	response.SetResponseValue(EResponseNotApplicable);
    }
    else if  ( param[0] == 'i')
    {
    	response.SetResponseValue(EResponseIndeterminate);
    }
	switch ( response.GetResponseValue())
    {
    	case EResponseDeny:
    		err = KErrNone;
    	break;
		case EResponsePermit:
    		err = KErrNone;
    	break;
		case EResponseNotApplicable:
    		err = KErrNone;
    	break;
		case EResponseIndeterminate:
    		err = KErrNone;
    	break;
    	default:
    		err = KErrGeneral;
	   	break;
   	}	
	return err;    
}

void Crequest_cntxt_api::CreateCertificate( TCertInfo& aCertInfo )
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


//  End of File
