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
* Description:  Status response buffering
*
*/





#include "NSmlResponseController.h"
#include "nsmlcliagconstants.h"


// ---------------------------------------------------------
// CNSmlResponseController::CNSmlResponseController()
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
CNSmlResponseController::CNSmlResponseController()
	{
	}

// ---------------------------------------------------------
// CNSmlResponseController::ConstructL()
// Two-way construction. Constructor may leave in EPOC.
// ---------------------------------------------------------
//
void CNSmlResponseController::ConstructL()
	{
	iResponse = new ( ELeave ) CArrayFixFlat<TResponseData>(1);
	iEntryID = 0;
	}
// ---------------------------------------------------------
// CNSmlResponseController::~CNSmlResponseController()
// 
// ---------------------------------------------------------
CNSmlResponseController::~CNSmlResponseController()
	{
	if ( iResponse )
		{
		for ( TInt i = 0; i < iResponse->Count(); i++)
			{
			FreeResources( i );
			}
		delete iResponse;
		}
	}

// ---------------------------------------------------------
// CNSmlResponseController::NewL()
// Creates new instance of CNSmlResponseController. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlResponseController* CNSmlResponseController::NewL()
	{
	CNSmlResponseController* self = CNSmlResponseController::NewLC();
	CleanupStack::Pop();
	return( self );
	}

// ---------------------------------------------------------
// CNSmlResponseController::NewLC()
// Creates new instance of CNSmlResponseController 
// Leaves instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlResponseController* CNSmlResponseController::NewLC()
	{
	CNSmlResponseController* self = new (ELeave) CNSmlResponseController();
	CleanupStack::PushL( self );
	self->ConstructL();
	return( self );
	}

// ---------------------------------------------------------
// CNSmlResponseController::CreateNewResponseItemL()
// Creates new response item to the array
// ---------------------------------------------------------
//
TInt CNSmlResponseController::CreateNewResponseItemL()
	{
	TResponseData responseData;
	responseData.responseMsgID = 0;
	responseData.responseCmd = NULL;
	responseData.responseCmdID = 0;
	responseData.responseLUID = NULL;
	responseData.responseAppIndex = -1;
	responseData.responseStatusCode = 0;
	responseData.responseStatusReceived = EFalse;
	responseData.responseChal = NULL;
	responseData.responseDetail = EResponseNoDetail;
	responseData.responseMoreData = EFalse;
	iResponse->AppendL( responseData );
	return iResponse->Count();
	}

// ---------------------------------------------------------
// CNSmlResponseController::RemoveResponseItem
// Removes Response item
// ---------------------------------------------------------
void CNSmlResponseController::RemoveResponseItem( TInt aMsgID, TInt aCmdID )
	{
	for ( TInt i = 0; i < iResponse->Count(); i++)
		{
		if ( (*iResponse)[i].responseMsgID == aMsgID &&
			 (*iResponse)[i].responseCmdID == aCmdID )
			{
			FreeResources( i );
			iResponse->Delete( i );
			break;
			}
		}
	}

// ---------------------------------------------------------
// CNSmlResponseController::SetMsgID()
// Sets Message ID 
// ---------------------------------------------------------
void CNSmlResponseController::SetMsgID( TInt aEntryID, TInt aMsgID )
	{
	(*iResponse)[aEntryID-1].responseMsgID = aMsgID;
	}

// ---------------------------------------------------------
// CNSmlResponseController::SetCmdL()
// Sets SyncML command 
// ---------------------------------------------------------
void CNSmlResponseController::SetCmdL( TInt aEntryID, const TDesC8& aCmd )
	{
	(*iResponse)[aEntryID-1].responseCmd = HBufC8::NewL( aCmd.Length() );
	*(*iResponse)[aEntryID-1].responseCmd = aCmd;
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetCmdID()
// Sets Command ID 
// ---------------------------------------------------------
void CNSmlResponseController::SetCmdID( TInt aEntryID, TInt aCmdID )
	{
	(*iResponse)[aEntryID-1].responseCmdID = aCmdID;
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetLUIDL()
// Sets LUID 
// ---------------------------------------------------------
EXPORT_C void CNSmlResponseController::SetLUIDL( TInt aEntryID, const TDesC8& aLUID )
	{
	(*iResponse)[aEntryID-1].responseLUID = HBufC8::NewL( aLUID.Length() );
	*(*iResponse)[aEntryID-1].responseLUID = aLUID;	
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetAppIndex()
// Set Index of application in Sync 
// ---------------------------------------------------------
EXPORT_C void CNSmlResponseController::SetAppIndex( TInt aEntryID, TInt aAppIndex )
	{
	(*iResponse)[aEntryID-1].responseAppIndex = aAppIndex;
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetChalTypeL()
// Sets Challenge Type 
// ---------------------------------------------------------
void CNSmlResponseController::SetChalTypeL( TInt aEntryID, const TDesC8& aType )
	{
	if ( !(*iResponse)[aEntryID-1].responseChal )
		{
		CreateResponseChalL( aEntryID-1 );
		}
	if ( (*iResponse)[aEntryID-1].responseChal->chalType )
		{
		delete (*iResponse)[aEntryID-1].responseChal->chalType;
        (*iResponse)[aEntryID-1].responseChal->chalType = NULL;
		}
	(*iResponse)[aEntryID-1].responseChal->chalType = HBufC8::NewL( aType.Length() );	
	*(*iResponse)[aEntryID-1].responseChal->chalType = aType;	
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetChalFormatL()
// Sets Challenge Format
// ---------------------------------------------------------
void CNSmlResponseController::SetChalFormatL( TInt aEntryID, const TDesC8& aFormat )
	{
	if ( !(*iResponse)[aEntryID-1].responseChal )
		{
		CreateResponseChalL( aEntryID-1 );
		}
	if ( (*iResponse)[aEntryID-1].responseChal->chalFormat )
		{
		delete (*iResponse)[aEntryID-1].responseChal->chalFormat;
		(*iResponse)[aEntryID-1].responseChal->chalFormat = NULL;
		}
	(*iResponse)[aEntryID-1].responseChal->chalFormat = HBufC8::NewL( aFormat.Length() );	
	*(*iResponse)[aEntryID-1].responseChal->chalFormat = aFormat;	
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetChalNextNonceL()
// Sets Challenge Format
// ---------------------------------------------------------
void CNSmlResponseController::SetChalNextNonceL( TInt aEntryID, const TDesC8& aNextNonce )
	{
	if ( !(*iResponse)[aEntryID-1].responseChal )
		{
		CreateResponseChalL( aEntryID-1 );
		}
	if ( (*iResponse)[aEntryID-1].responseChal->chalNextNonce )
		{
		delete (*iResponse)[aEntryID-1].responseChal->chalNextNonce;
        (*iResponse)[aEntryID-1].responseChal->chalNextNonce = NULL;
		}
	(*iResponse)[aEntryID-1].responseChal->chalNextNonce = HBufC8::NewL( aNextNonce.Length() );	
	*(*iResponse)[aEntryID-1].responseChal->chalNextNonce = aNextNonce;	
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetResponseDetail()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlResponseController::SetResponseDetail( TInt aEntryID, TResponseDetail aResponseDetail )
	{
	(*iResponse)[aEntryID-1].responseDetail= aResponseDetail;
	}
// ---------------------------------------------------------
// CNSmlResponseController::SetMoreData()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlResponseController::SetMoreData( TInt aEntryID )
	{
	(*iResponse)[aEntryID-1].responseMoreData = ETrue;
	}
// ---------------------------------------------------------
// CNSmlResponseController::RemoveAllReceivedOnes()
// All items which Status is received are deleted 
// ---------------------------------------------------------
void CNSmlResponseController::RemoveAllReceivedOnes()
	{
	for ( TInt i = iResponse->Count() - 1; i >= 0; i--)
		{
		if ( (*iResponse)[i].responseStatusReceived )
			{
			FreeResources( i );
			iResponse->Delete( i );
			}
		}
	iResponse->Compress();
	iEntryID = 0;
	}
// ---------------------------------------------------------
// CNSmlResponseController::ResetL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlResponseController::ResetL()
	{
	for ( TInt i = 0; i < iResponse->Count(); i++)
		{
		FreeResources( i );
		}
	delete iResponse;
	iResponse = NULL;
	ConstructL();
	}

// ---------------------------------------------------------
// CNSmlResponseController::MatchStatusElement
// Searches matching Response data and updates status code and status-received flag
// ---------------------------------------------------------
TBool CNSmlResponseController::MatchStatusElement( const TDesC8& aMsgID, const TDesC8& aCmdID, TInt aStatusCode, TInt& aEntryID )
	{
	TLex8 lexicalStatusMsgID( aMsgID );
	TInt numericMsgID;
	if ( lexicalStatusMsgID.Val (numericMsgID ) != KErrNone )
		{
		return EFalse;
		}
	TLex8 lexicalStatusCmdID( aCmdID );
	TInt numericCmdID;
	if ( lexicalStatusCmdID.Val (numericCmdID ) != KErrNone )
		{
		return EFalse;
		}
	for ( TInt i = 0; i < iResponse->Count(); i++)
		{
		if ( numericMsgID == (*iResponse)[i].responseMsgID  && numericCmdID == (*iResponse)[i].responseCmdID )
			{
			(*iResponse)[i].responseStatusReceived = ETrue;
			(*iResponse)[i].responseStatusCode = aStatusCode;
			aEntryID = ++i;
			return ETrue;
			}
		}
	return EFalse;
	}
// ---------------------------------------------------------
// CNSmlResponseController::Begin()
// 
// ---------------------------------------------------------
void CNSmlResponseController::Begin()
	{
	iEntryID = 0;
	}
// ---------------------------------------------------------
// CNSmlResponseController::NextResponseInfo()
// Gives entry ID for the next response info item
// ---------------------------------------------------------
TBool CNSmlResponseController::NextResponseInfo( TInt& aEntryID ) 
	{
	if ( iResponse->Count() <= iEntryID )
		{
		return EFalse;
		}
	iEntryID++;
	aEntryID = iEntryID;
	return ETrue;
	}

// ---------------------------------------------------------
// CNSmlResponseController::ItemResponseInfo()
// Gives entry ID for the response info of the item 
// ---------------------------------------------------------
TBool CNSmlResponseController::ItemResponseInfo( const TDesC8& aLUID, TInt& aEntryID ) const
	{
	TBool itemFound = EFalse;
	for ( TInt i = 0; i < iResponse->Count(); i++)
		{
		if ( (*iResponse)[i].responseLUID )
			{
			if ( *(*iResponse)[i].responseLUID == aLUID )
				{
				itemFound = ETrue;
				aEntryID = ++i;
				break;
				}
			}
		}
	return itemFound;
	}
// ---------------------------------------------------------
// CNSmlResponseController::Cmd()
// Gives name of the SyncML command 
// ---------------------------------------------------------
EXPORT_C TDesC8& CNSmlResponseController::Cmd( TInt aEntryID ) const
	{
	return *(*iResponse)[aEntryID-1].responseCmd; 
	}
// ---------------------------------------------------------
// CNSmlResponseController::LUID()
// Gives name of the SyncML command 
// ---------------------------------------------------------
EXPORT_C TPtrC8 CNSmlResponseController::LUID( TInt aEntryID ) const
	{
	TPtrC8 empty;
	if ( (*iResponse)[aEntryID-1].responseLUID )
		{
		return *(*iResponse)[aEntryID-1].responseLUID;
		}
	else
		{
		return empty;
		}
	}
// ---------------------------------------------------------
// CNSmlResponseController::ChalType()
// Gives name of the authentication type 
// ---------------------------------------------------------
EXPORT_C TPtrC8 CNSmlResponseController::ChalType( TInt aEntryID ) const
	{
	TPtrC8 type;
	if ( (*iResponse)[aEntryID-1].responseChal )
		{ 
		if ( (*iResponse)[aEntryID-1].responseChal->chalType )
			{
			type.Set( *(*iResponse)[aEntryID-1].responseChal->chalType ); 
			}
		}
	return type;
	}
// ---------------------------------------------------------
// CNSmlResponseController::ChalFormat()
// Gives Format of NextNonce  
// ---------------------------------------------------------
TPtrC8 CNSmlResponseController::ChalFormat( TInt aEntryID ) const
	{
	TPtrC8 format;
	if ( (*iResponse)[aEntryID-1].responseChal )
		{
		if ( (*iResponse)[aEntryID-1].responseChal->chalFormat )
			{
			format.Set( *(*iResponse)[aEntryID-1].responseChal->chalFormat ); 
			}
		}
	return format;
	}
// ---------------------------------------------------------
// CNSmlResponseController::ChalNextNonce()
// Gives NextNonce  
// ---------------------------------------------------------
TPtrC8 CNSmlResponseController::ChalNextNonce( TInt aEntryID ) const
	{
	TPtrC8 nextNonce;
	if ( (*iResponse)[aEntryID-1].responseChal )
		{
		if ( (*iResponse)[aEntryID-1].responseChal->chalNextNonce )
			{
			nextNonce.Set( *(*iResponse)[aEntryID-1].responseChal->chalNextNonce ); 
			}
		}
	return nextNonce;
	}
// ---------------------------------------------------------
// CNSmlResponseController::ResponseDetail
// 
// ---------------------------------------------------------
EXPORT_C CNSmlResponseController::TResponseDetail CNSmlResponseController::ResponseDetail( TInt aEntryID ) const
	{
	return (*iResponse)[aEntryID-1].responseDetail;
	}
// ---------------------------------------------------------
// CNSmlResponseController::BusyStatus()
// Returns True if 101 statuscode in SyncHdr in given message  
// ---------------------------------------------------------
TBool CNSmlResponseController::BusyStatus( TInt aMsgID ) const
	{
	TBool busyStatus = EFalse;
	for ( TInt i = 0; i < iResponse->Count(); i++)
		{
		if ( ( (*iResponse)[i].responseMsgID == aMsgID ) && ( (*iResponse)[i].responseCmd ) )
			{
			if ( *(*iResponse)[i].responseCmd == KNSmlAgentSyncHdr )
				{
				if ( (*iResponse)[i].responseStatusCode == 101 )
					{
					busyStatus = ETrue;
					}
				break;
				}
			}
		}
	return busyStatus;
	}

// ---------------------------------------------------------
// CNSmlResponseController::MoreData()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlResponseController::MoreData( TInt aEntryID ) const
	{
	return (*iResponse)[aEntryID-1].responseMoreData;
	}
// ---------------------------------------------------------
// CNSmlResponseController::AppIndex()
// Gives index of the application 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlResponseController::AppIndex( TInt aEntryID ) const
	{
	return (*iResponse)[aEntryID-1].responseAppIndex;
	}
// ---------------------------------------------------------
// CNSmlResponseController::StatusCode()
// Gives status code 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlResponseController::StatusCode( TInt aEntryID ) const
	{
	return (*iResponse)[aEntryID-1].responseStatusCode;
	}

// ---------------------------------------------------------
// CNSmlResponseController::EntryID()
// Returns EntryID by Cmd ID and Msg ID
// ---------------------------------------------------------
EXPORT_C TInt CNSmlResponseController::EntryID( TInt aCmdID, TInt aMsgID) const
	{
	TInt count = iResponse->Count();
	for( TInt i = 0; i < count; i++ )
		{
		// aCmdID and aMsgID have been retrieved from the received
		// status command for deletion. 
		if( iResponse->At( i ).responseCmdID == aCmdID &&
			iResponse->At( i ).responseMsgID == aMsgID )
			{
			// Entry ID of the current item.
			return i+1;
			}
		}
	// Entry ID not found
	return 0;
	}

// ---------------------------------------------------------
// CNSmlResponseController::StatusReceived()
// Returns True, if the has returned Status for the command 
// ---------------------------------------------------------
TBool CNSmlResponseController::StatusReceived( TInt aEntryID ) const
	{
	return (*iResponse)[aEntryID-1].responseStatusReceived;
	}
// ---------------------------------------------------------
// CNSmlResponseController::CreateResponseChalL
// Creates and initiates responseChal structure 
// ---------------------------------------------------------
void CNSmlResponseController::CreateResponseChalL( TInt aIndex)
	{
	(*iResponse)[aIndex].responseChal = new( ELeave ) TChal;
	(*iResponse)[aIndex].responseChal->chalType = NULL;
	(*iResponse)[aIndex].responseChal->chalFormat = NULL;
	(*iResponse)[aIndex].responseChal->chalNextNonce = NULL;
	}
// ---------------------------------------------------------
// CNSmlResponseController::FreeResources
// Frees resources of an item
// ---------------------------------------------------------
void CNSmlResponseController::FreeResources( TInt aI )
	{
	delete ( (*iResponse)[aI].responseCmd );
	delete ( (*iResponse)[aI].responseLUID );
	if ( (*iResponse)[aI].responseChal )
		{
		delete (*iResponse)[aI].responseChal->chalType;
		delete (*iResponse)[aI].responseChal->chalFormat;
		delete (*iResponse)[aI].responseChal->chalNextNonce;
		delete (*iResponse)[aI].responseChal;	
		}	
	}
