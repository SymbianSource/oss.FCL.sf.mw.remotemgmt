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
* Description: Implementation of policymanagement components
*
*/
/* PolicyEngineClient.cpp
*/

#include <eikenv.h>
#include <eikappui.h>
#include <e32svr.h>
#include <featmgr.h>
#include <bldvariant.hrh>

#include "PolicyEngineClientServer.h"
#include "PolicyEngineClientServerDefs.h"
#include "PolicyEngineClient.h"
#include "ErrorCodes.h"
#include "debug.h"

#include <f32file.h>
#include <s32file.h>

const TInt KMaxLabelIdLength = 25; //take 25 from cert store or some header file


// Standard server startup code
// 
static TInt StartServer()
	{
	RDEBUG("PolicyEngineServer: Starting server...");
	
	const TUidType serverUid(KNullUid,KNullUid,KPolicyEngineServerUid);

	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	//TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
	TInt r=server.Create(KPolicyEngineServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("PolicyEngineClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("PolicyEngineClient: Started");
	
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}


// This is the standard retry pattern for server connection
EXPORT_C TInt RPolicyEngine::Connect()
	{
	TRAPD( e, FeatureManager::InitializeLibL() );
	if( e != KErrNone )
		{
		return e;
		}
				
	TBool iSupported = FeatureManager::FeatureSupported( KFeatureIdSapPolicyManagement);
	FeatureManager::UnInitializeLib();	    
		
	if( !iSupported )
		{
		return KErrNotSupported;
		}	

	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KPolicyEngineServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}
	
EXPORT_C void RPolicyEngine::Close()
	{
	RSessionBase::Close();  //basecall
	}


EXPORT_C TInt RPolicyEngine::PerformRFS()
{
   	RDEBUG("PolicyEngineClient: Perform RFS");
	return SendReceive( EPerformPMRFS);		
}

	
	

// ----------------------------------------------------------------------------------------
// RPolicyManagement subsessions
// ----------------------------------------------------------------------------------------

//  create new subsession
EXPORT_C TInt RPolicyManagement::Open(RPolicyEngine &aServer )
    {
	RDEBUG("PolicyEngineClient: Create new managemenet subsession");
	return CreateSubSession(aServer,ECreateManagementSubSession,TIpcArgs(NULL,NULL));
	}

// close a subsession
EXPORT_C void RPolicyManagement::Close()
	{
	RDEBUG("PolicyEngineClient: Close managemenet subsession");
	RSubSessionBase::CloseSubSession(ECloseManagementSubSession);
	}
	
	
EXPORT_C TInt RPolicyManagement::ExecuteOperation( const TDesC8& aOperationDescription, 
		          					  	TParserResponse &aResponse)
{
	RDEBUG("PolicyEngineClient: Execute operation");

	TPckg<TParserResponse> pack(aResponse);
	TInt err = SendReceive( EExecuteOperation, TIpcArgs(&aOperationDescription, &pack));	
	
	return err;	
}

EXPORT_C TInt RPolicyManagement::ExecuteOperation( const TDesC8& aOperationDescription )
{
	TParserResponse response;
	TInt err = ExecuteOperation( aOperationDescription, response);
	
	if ( err == KErrNone )
	{
		if ( response.GetReturnMessage().Compare( ManagementErrors::OperationOk) != 0)
		{
			err = KErrAbort;
		}
	}
	
	return err;
}



EXPORT_C TInt RPolicyManagement::GetElementL( TElementInfo& aElementInfo)
{
	RDEBUG("PolicyEngineClient: Get element");

	TElementInfoHelpPack helper;
	helper.iDescriptionLength = 0;
	helper.iChildListLength = 0;

	TPckg<TElementInfoHelpPack> helperPack(helper);
	
	TInt err = SendReceive( EGetElementDescriptionAndChildListLength, TIpcArgs( &aElementInfo.iElementId, &helperPack));	

	if ( err == KErrNone)
	{
		HBufC8 * childs = HBufC8::NewLC( helper.iChildListLength);	

		aElementInfo.iDescription = HBufC8::NewL( helper.iDescriptionLength);	

		TPtr8 descriptionArgs = aElementInfo.iDescription->Des();
		TPtr8 childsArgs = childs->Des();

		err =	SendReceive( EReadElementAndChildList, 
		   		TIpcArgs( &descriptionArgs, &childsArgs));	
	
		if ( err == KErrNone )
		{
			aElementInfo.iChildElements.SetListL( *childs); 
		}
		   
		CleanupStack::PopAndDestroy();
	}
	
	return err;
}

EXPORT_C TInt RPolicyManagement::GetXACMLDescriptionL( TElementInfo& aElementInfo)
{
	RDEBUG("PolicyEngineClient: Get XACML description");

	TInt XACMLLength;
	TPckg<TInt> lengthPack(XACMLLength);

	TInt err = SendReceive( EGetElementXACMLLength, TIpcArgs( &aElementInfo.iElementId, &lengthPack));	

	if ( err == KErrNone)
	{
		aElementInfo.iXACMLContent = HBufC8::NewL( XACMLLength);	
		TPtr8 contentArgs = aElementInfo.iXACMLContent->Des();
	
		err = SendReceive( EReadElementXACML, 
			   TIpcArgs( &contentArgs));	
	}
	
	return err;	
}

EXPORT_C TInt RPolicyManagement::GetElementListL( const TElementType& aElementType, RElementIdArray& aElementIdArray)
{
	RDEBUG("PolicyEngineClient: Get element list");


	//Create packages for data
	TInt listLength;
	TPckg<TInt> lengthPack(listLength);
	TPckg<TElementType> typePack(aElementType);

	//get element list length from server side
	TInt err = SendReceive( EGetElementListLength, TIpcArgs( &typePack, &lengthPack));	

	//and if operation ok, get also element list
	if ( err == KErrNone)
	{
		//create buffer for list
		TPtr8 ptr = HBufC8::NewLC( listLength)->Des();	

		//get element list descriptor
		err = SendReceive( EReadElementList, TIpcArgs( &ptr));	

		//feed list descriptor for IDArray element, which decode descriptor to list format
		if ( err == KErrNone)
		{
			aElementIdArray.SetListL( ptr);
		}

		//HBufC8
		CleanupStack::PopAndDestroy();		
	}
	
	return err;	
}


EXPORT_C TInt RPolicyManagement::AddSessionTrust( TCertInfo& aCertInfo)
{
	RDEBUG("PolicyEngineClient: Add session trust");


	//Create package for data
	TPckg<TCertInfo> certInfoPck( aCertInfo);
	
	//Send CertInfo to server
	return SendReceive( EAddSessionTrust, TIpcArgs( &certInfoPck));	
}

EXPORT_C TInt RPolicyManagement::AddServerCert(const CX509Certificate& aCert, TDes& aLabel)
{
    RDEBUG("RPolicyManagement::AddServerCert()" );
    
    TPtrC8 ptr(aCert.Encoding());   
    TInt size =ptr.Length();
          
    TBuf<1024> certData;
    certData.Copy(ptr);
    //label buffer to be written by server
    TBuf<KMaxLabelIdLength> cLabel;
    
    TIpcArgs args(&certData,size,&cLabel);       
    
    TInt err = SendReceive(EServerCertAddRequest, args); 
    // copy returned label 
    aLabel.Append(cLabel);    
    
    return err; 
 
}


EXPORT_C TInt RPolicyManagement:: RemoveServerCert(const TDesC& aLabel)
{  
    RDEBUG("RPolicyManagement:: RemoveServerCert");

    TBuf<KMaxLabelIdLength> labelData;
    labelData.Copy(aLabel);    
    
    TInt err = SendReceive(EServerCertRemoveRequest, TIpcArgs(&labelData));    
    return err;    
}


EXPORT_C TInt RPolicyManagement::IsAllowedServerId( const TDesC& aServerID)
{
	RDEBUG_2("RPolicyManagement::IsAllowedServerId( %S )", &aServerID );

	//Create package for data
	TBool response;
	TPckg<TBool> respPck( response);

	//make 8-bit descriptor
	HBufC8* buf = NULL;
	TRAPD( e, buf = HBufC8::NewL( aServerID.Length()) );
	if( e != KErrNone )
		{
		return e;
		}
	
	buf->Des().Append( aServerID);
	
	//Send parameters to server
	TInt err = SendReceive( EIsServerIdValid, TIpcArgs( buf, &respPck));	
	
	if ( err == KErrNone )
	{
		if ( !response )
		{
			err = KErrAccessDenied;
		}
	}

	delete buf;
	buf = NULL;

	return err;
}


EXPORT_C TInt RPolicyManagement::CertificateRole( TCertInfo& aCertInfo, TRole& aRole)
{
	RDEBUG("PolicyEngineClient: Certificate role");

	//Create package for data
	TPckg<TCertInfo> certInfoPck( aCertInfo);
	TPckg<TRole> rolePck( aRole);

	//Send parameters to server
	return SendReceive( ECertificateRole, TIpcArgs( &certInfoPck, &rolePck));	
}


// ----------------------------------------------------------------------------------------
// RPolicyRequest subsessions
// ----------------------------------------------------------------------------------------

//  create new subsession 
EXPORT_C TInt RPolicyRequest::Open(RPolicyEngine &aServer )
    {
   	RDEBUG("PolicyEngineClient: Open policy request subsession");
	return CreateSubSession(aServer,ECreateRequestSubSession,TIpcArgs(NULL,NULL));
	}
	
// close a subsession
EXPORT_C void RPolicyRequest::Close()
	{
   	RDEBUG("PolicyEngineClient: Close policy request subsession");
	RSubSessionBase::CloseSubSession(ECloseRequestSubSessio);
	}

EXPORT_C TInt RPolicyRequest::MakeRequest( TRequestContext& aRequestContext, TResponse& aResponse)
{
   	RDEBUG("PolicyEngineClient: Make policy request");
	TPckg<TResponse> pack( aResponse);
	return SendReceive( EPolicyRequest, TIpcArgs( &aRequestContext.RequestDescription(), &pack));		
}


