/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#include <f32file.h>
#include <s32mem.h>

#include <nsmldebug.h>
#include <nsmldmauthinfo.h>
#include <NSmlPrivateAPI.h>
#include "NSmlClientAPIDefs.h"
#include "nsmlsosserverdefs.h"

//------------------------------------------------------------
// Launches the server.
//------------------------------------------------------------
//
TInt LaunchServer()
	{	
	DBG_BEGIN();
	_DBG_FILE("RNSmlPrivateAPI LaunchServer(): begin");
	TInt res = KErrNone;

	// DLL launch
	RProcess server;
	res = server.Create(KServerExeName, KNullDesC);

	// Loading failed.
	if ( res != KErrNone )
		{
		return res;
		}
	
	TRequestStatus status;
	server.Rendezvous(status);

	if (status != KRequestPending)
		{
		server.Kill(0);		// abort startup
		server.Close();
		return KErrGeneral;   // status can be KErrNone: don't return status.Int()
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
		
	User::WaitForRequest(status);
	
	server.Close();

	_DBG_FILE("RNSmlPrivateAPI LaunchServer(): end");
	
	return status.Int();
	}


// ---------------------------------------------------------
// RNSmlPrivateAPI::RNSmlPrivateAPI()
// Constructor
// ---------------------------------------------------------
//
EXPORT_C RNSmlPrivateAPI::RNSmlPrivateAPI()
	{

	}

// ---------------------------------------------------------
// RNSmlPrivateAPI::OpenL()
// Creates connection to server
// ---------------------------------------------------------
//
EXPORT_C void RNSmlPrivateAPI::OpenL()
	{
	_DBG_FILE("RNSmlPrivateAPI::OpenL(): begin");
	TVersion version(KNSmlSOSServerVersionMajor, KNSmlSOSServerVersionMinor, 0);
	
	TInt res = CreateSession( KSOSServerName, version, KDefaultMessageSlots );
	
	if ( res != KErrNone )
		{
		res = LaunchServer();
		User::LeaveIfError( res );
		res = CreateSession( KSOSServerName, version, KDefaultMessageSlots );
		}

	User::LeaveIfError( res );
	
	_DBG_FILE("RNSmlPrivateAPI::OpenL(): end");
	}

// ---------------------------------------------------------
// RNSmlPrivateAPI::SendL( const TDesC8& aData, TSmlUsageType aType, 
// TSmlProtocolVersion aVersion )
// Passes message to server
// ---------------------------------------------------------
//
EXPORT_C void RNSmlPrivateAPI::SendL(
    const TDesC8& aData, 
    TSmlUsageType aType, 
    TSmlProtocolVersion aVersion, 
    TUid aTransportId )
	{
	_DBG_FILE("RNSmlPrivateAPI::SendL(): begin");
	
	TIpcArgs args( &aData, aType, aVersion, aTransportId.iUid );    
    User::LeaveIfError( SendReceive( ECmdServerAlert, args ) );
    
    _DBG_FILE("RNSmlPrivateAPI::SendL(): end");
	}

// ---------------------------------------------------------
// RNSmlPrivateAPI::Close()
// Passes message to server
// ---------------------------------------------------------
//		
EXPORT_C void RNSmlPrivateAPI::Close()
	{
	_DBG_FILE("RNSmlPrivateAPI::Close(): begin");
	
	// close session	
	RSessionBase::Close();
	
	_DBG_FILE("RNSmlPrivateAPI::Close(): end");
	}

// ---------------------------------------------------------
// RNSmlPrivateAPI::SetDMAuthInfoL( const CNSmlDMAuthInfo& aAuthInfo )
// Sends DM auhentication info to be saved
// ---------------------------------------------------------
//	
EXPORT_C void RNSmlPrivateAPI::SetDMAuthInfoL( const CNSmlDMAuthInfo& aAuthInfo )
	{
	CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( buffer );
	
	RBufWriteStream stream( *buffer );	
	CleanupClosePushL( stream );
	
	aAuthInfo.ExternalizeL( stream );
	
	TPtr8 pBuffer(0, NULL, 0);
	pBuffer.Set( buffer->Ptr(0) );
	
	CleanupStack::PopAndDestroy(); // stream
	
	TIpcArgs args(aAuthInfo.iProfileId, &pBuffer);
	User::LeaveIfError( SendReceive( ECmdSetDMAuthInfo, args) );
	
	CleanupStack::PopAndDestroy(); // buffer
	}

// ---------------------------------------------------------
// RNSmlPrivateAPI::GetDMAuthInfoL( CNSmlDMAuthInfo& aAuthInfo )
// Retrieves DM auhentication info from settings
// ---------------------------------------------------------
//	
EXPORT_C void RNSmlPrivateAPI::GetDMAuthInfoL( CNSmlDMAuthInfo& aAuthInfo )
	{
	TPckgBuf<TInt> buf;
	TIpcArgs args(aAuthInfo.iProfileId, &buf);
	
	User::LeaveIfError( SendReceive( ECmdInitDMAuthInfo, args) );
	
	CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( buffer );
	buffer->ResizeL( buf() );
	
	TPtr8 pBuffer(0, NULL, 0);
	pBuffer.Set( buffer->Ptr(0) );
	                
	TIpcArgs argsGet( &pBuffer );
	User::LeaveIfError( SendReceive( ECmdGetDMAuthInfo, argsGet) );
	
	RDesReadStream stream;
	stream.Open( pBuffer );
	CleanupClosePushL( stream );
	
	aAuthInfo.InternalizeL( stream );
	
	CleanupStack::PopAndDestroy(2); // stream, buffer	
	}


// FOTA

// ---------------------------------------------------------
// RNSmlPrivateAPI::AddDMGenericAlertRequestL()
// Adds a Generic Alert to DM Agent if it is found in the 
// server.
// ---------------------------------------------------------
//	
EXPORT_C TInt RNSmlPrivateAPI::AddDMGenericAlertRequestL( const TDesC8& aMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aCorrelator ) const
	{
	TInt err = KErrNone;
	
	// create buffer
	CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( buffer );
	
	// open stream to buffer
	RBufWriteStream stream( *buffer );	
	CleanupClosePushL( stream );
	
    // externalize data to stream
	stream.WriteInt32L( aMgmtUri.Length() );
	stream.WriteL( aMgmtUri );

	stream.WriteInt32L( aMetaType.Length() );
	stream.WriteL( aMetaType );
	
	stream.WriteInt32L( aMetaFormat.Length() );
	stream.WriteL( aMetaFormat );
	
	stream.WriteInt32L( aCorrelator.Length() );
	stream.WriteL( aCorrelator );
	
	stream.WriteInt32L( aFinalResult );
	
	// get modifiable pointer to buffer
	TPtr8 bufPtr( buffer->Ptr(0) );
	
	CleanupStack::PopAndDestroy( &stream );
	
	// send the data to sos server
	TIpcArgs args( &bufPtr );
	err = SendReceive( ECmdAddDMGenericAlert, args );
	
	CleanupStack::PopAndDestroy( buffer );
	
	return err;
	}

// FOTA end


// SCOMO

// ---------------------------------------------------------
// RNSmlPrivateAPI::AddDMGenericAlertRequestL()
// Adds a Generic Alert to DM Agent if it is found in the 
// server.
// ---------------------------------------------------------
//	
EXPORT_C TInt RNSmlPrivateAPI::AddDMGenericAlertRequestL( const TDesC8& aCorrelator, RArray<CNSmlDMAlertItem>& aItemList ) const
	{
	TInt err = KErrNone;
	
	// create buffer
	CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( buffer );
	
	// open stream to buffer
	RBufWriteStream stream( *buffer );	
	CleanupClosePushL( stream );
	
    // externalize data to stream
	stream.WriteInt32L( aCorrelator.Length() );
	stream.WriteL( aCorrelator );

	TInt count = aItemList.Count();
	stream.WriteInt32L( count );
	
	if(count > 0)
	{		
		for(TInt i =0 ; i <count; i++)
		{
			stream.WriteInt32L( (aItemList[i].iSource)->Length() );
			stream.WriteL( (aItemList[i].iSource)->Des() );
	
			stream.WriteInt32L( (aItemList[i].iTarget)->Length() );
			stream.WriteL( aItemList[i].iTarget->Des() );
	
			stream.WriteInt32L( (aItemList[i].iMetaType)->Length() );
			stream.WriteL( aItemList[i].iMetaType->Des() );

			stream.WriteInt32L( (aItemList[i].iMetaFormat)->Length()  );
			stream.WriteL( aItemList[i].iMetaFormat->Des());

			stream.WriteInt32L( (aItemList[i].iMetaMark)->Length() );
			stream.WriteL( aItemList[i].iMetaMark->Des() );
			
			stream.WriteInt32L( (aItemList[i].iData)->Length() );
			stream.WriteL( aItemList[i].iData->Des() );
		}		
	}
	
	// get modifiable pointer to buffer
	TPtr8 bufPtr( buffer->Ptr(0) );
	
	CleanupStack::PopAndDestroy( &stream );
	
	// send the data to sos server
	TIpcArgs args( &bufPtr );
	err = SendReceive( ECmdAddGenericAlert, args );
	
	CleanupStack::PopAndDestroy( buffer );
	
	return err;
	}

// SCOMO


//End of File
