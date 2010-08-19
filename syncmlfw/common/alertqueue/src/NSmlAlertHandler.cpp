/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Alert queue and handler
*
*/

#include <nsmldebug.h>

#include "NSmlAlertQueue.h"
#include "nsmlsosserverdefs.h"

//Fix to Remove the Bad Compiler Warnings
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNSmlAlertHandler::NewL(MNSmlMessageHandler* aMsgHandler)
// Two phase constructor
// ---------------------------------------------------------
//
CNSmlAlertHandler* CNSmlAlertHandler::NewL( MNSmlMessageHandler* aMsgHandler )
	{
	CNSmlAlertHandler* self = new (ELeave) CNSmlAlertHandler;
	CleanupStack::PushL( self );
	self->ConstructL( aMsgHandler );
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::CNSmlAlertHandler()
// Constructor
// ---------------------------------------------------------
CNSmlAlertHandler::CNSmlAlertHandler()
: CActive(0), iNewEntry( EFalse )
	{
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::~CNSmlAlertHandler()
// Destructor
// ---------------------------------------------------------
CNSmlAlertHandler::~CNSmlAlertHandler()
	{

	Cancel();
	if( iHistoryArray )
	{
	iHistoryArray->SetOwnerShip( ETrue );
	delete iHistoryArray;
	}
	delete iAlertParser;			
		
	if ( ! iNewEntry )
		{
		delete iAlertInfo;
		delete iHistoryInfo;
		}
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::DoCancel()
// Method from base class
// ---------------------------------------------------------
void CNSmlAlertHandler::DoCancel()
	{
	
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::ProcessAlert()
// Activates alert handler
// ---------------------------------------------------------
void CNSmlAlertHandler::ProcessAlert()
	{
	if ( IsActive() )
		{
		return;
		}
	SetActive();
	TRequestStatus* pStatus = &iStatus;
	User::RequestComplete(pStatus, KErrNone);
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::ConstructL( MNSmlMessageHandler* aMsgHandler )
// Second phase constructor
// ---------------------------------------------------------
void CNSmlAlertHandler::ConstructL( MNSmlMessageHandler* aMsgHandler )
	{
	User::LeaveIfNull(aMsgHandler);
	iMsgHandler = aMsgHandler;
	
    iHistoryInfo = CSyncMLHistoryPushMsg::NewL();

	iAlertInfo = new ( ELeave ) CSmlAlertInfo;

	iHistoryArray = CNSmlHistoryArray::NewL();
	iHistoryArray->SetOwnerShip( EFalse ); // move ownership to history array
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::RunL()
// Method from base class
// ---------------------------------------------------------
void CNSmlAlertHandler::RunL()
    {
    TRAP_IGNORE( DoRunL() );    
    };

// ---------------------------------------------------------
// CNSmlAlertHandler::DoRunL()
// Method from base class
// ---------------------------------------------------------
void CNSmlAlertHandler::DoRunL()
	{

    // Delete member variables
    iHistoryArray->SetOwnerShip( ETrue );
	delete iHistoryArray;
	iHistoryArray = NULL;
	delete iAlertParser;			
	iAlertParser = NULL;

    // These are deleted by iHistoryArray
	iAlertInfo = NULL;
	iHistoryInfo = NULL;

    // Create member variables	
	iHistoryInfo = CSyncMLHistoryPushMsg::NewL();
	iAlertInfo = new ( ELeave ) CSmlAlertInfo;
	iHistoryArray = CNSmlHistoryArray::NewL();
	iHistoryArray->SetOwnerShip( EFalse ); // move ownership to history array
	
	
	// Continue DoRunL method
	TBool message( EFalse );
	TBool quit( ETrue );
	TBool doPop( EFalse );
	
	TPtrC8 pPackage;
	TSmlUsageType type;
	TSmlProtocolVersion version;
	TSmlTransportId bearerType;
	
	//check message
	iMsgHandler->CheckMessage( message, type, version, bearerType );
	iAlertInfo->SetJobControl( CSmlAlertInfo::EDoNotCreateJob );
	iAlertInfo->SetTransportId( bearerType );
	#ifdef __NSML_DEBUG__
		DBG_ARGS(_S("CNSmlAlertHandler::RunL(): TransportId : '%d'"), bearerType );
	#endif // __NSML_DEBUG__

	TInt err( KErrNone );
	
	if ( message )
	    {
	    		
    	iAlertParser = NSmlParserFactory::CreateAlertParserL( type, version, *iAlertInfo, *iHistoryInfo );
    	iAlertParser->CreateBufferL( iMsgHandler->MessageSize() );
    	iMsgHandler->AlertMessage( iAlertParser->Message() );
    					
    	TRAP(err, iAlertParser->ParseMessageL());
    	DBG_FILE_CODE(err, _S8("CNSmlAlertHandler::RunL() : Alert result"));
    	iAlertInfo->SetErrorCode( err );
    	
    	iNewEntry = ETrue;
    	
    	if ( err != KErrNone)
    		{
    		iNewEntry = EFalse;
    		iAlertInfo->SetJobControl( CSmlAlertInfo::EDoNotCreateJob );
    		
    		// Close local connection
    		if ( bearerType != KUidNSmlMediumTypeInternet.iUid )
    			{
    			iMsgHandler->DoDisconnect();	
    			}
    	   		
    		if ( err == KErrCorrupt )
    			{
    			return; 
    			}
    		}
    	else
    		{
    		CheckDigestL( iAlertInfo->Profile(), iHistoryInfo->MsgDigest() );
    					
    		CheckProtocolAndChangeL( version );
    		    					
    		if ( ( type == ESmlDataSync )  && ( version == ESmlVersion1_1_2 ) )
    			{
    			iAlertInfo->SetConfirmation( ETrue );
    			pPackage.Set( iAlertParser->DoMessageCopyLC() );
    			doPop = ETrue;
    			}
    		}

    	delete iAlertParser;
    	iAlertParser = NULL;
	    }
	    
	iAlertInfo->SetConfirmation( FinalizeBeforeJobCreationL() );
	iMsgHandler->CreateJobL( *iAlertInfo, quit, pPackage);                
		
	if ( doPop )
		{
		CleanupStack::PopAndDestroy(); // DoMessageCopyLC()
		}
	
	
	if (! quit)
		{
		ProcessAlert();
		}
	
	}


// ---------------------------------------------------------
// CNSmlAlertHandler::FinalizeBeforeJobCreationL()
// Connects to notifier plug in to show confirmation note and
// waits for the response.
// ---------------------------------------------------------
TBool CNSmlAlertHandler::FinalizeBeforeJobCreationL()
	{
	
	if ( iAlertInfo->Profile() < KMaxDataSyncID )		
		{
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		
		CNSmlDSProfile* profile = settings->ProfileL( iAlertInfo->Profile() );
		
		if ( !profile )
			{
			CleanupStack::PopAndDestroy(); //settings
			return EFalse;
			}
			
		CleanupStack::PushL( profile );
		
		iAlertInfo->SetTransportId( profile->IntValue( EDSProfileTransportId) );
		iAlertInfo->SetConnectionId( profile->IntValue( EDSProfileTransportId) );
		
		CleanupStack::PopAndDestroy(2); //profile, settings
		}
	else
		{
		CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
		
		CNSmlDMProfile* profile = settings->ProfileL( iAlertInfo->Profile() );
		
		if ( !profile )
			{
			CleanupStack::PopAndDestroy(); //settings
			return EFalse;
			}
		
		CleanupStack::PushL( profile );
        profile->SetIntValue( EDMProfileSessionId, iAlertInfo->SessionId() );
        profile->SaveL();

		iAlertInfo->SetTransportId( profile->IntValue( EDMProfileTransportId) );
		iAlertInfo->SetConnectionId( profile->IntValue( EDMProfileTransportId) );
		
		CleanupStack::PopAndDestroy(2); //profile, settings
		}
	
	SaveAlertInfoL();
	
	if ( ! iAlertInfo->CreateSession() )
		{
		return ETrue;
		}
	
	return ETrue;
	}

// ---------------------------------------------------------
// CheckDigestL( const TInt aProfileId, const TDesC8& aDigest )
// Checks whether alert has already been handled. Receive count
// is updated if handled before.
// ---------------------------------------------------------
void CNSmlAlertHandler::CheckDigestL( const TInt aProfileId, const TDesC8& aDigest )
	{
	
	if ( aProfileId < KMaxDataSyncID )		
		{
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		
		CNSmlDSProfile* profile = settings->ProfileL(aProfileId);
		CleanupStack::PushL(profile);
		
		if ( ! profile->HasLogL() )
			{
			CleanupStack::PopAndDestroy(2); //profile, settings
			return;
			}
		
		RReadStream& readStream = profile->LogReadStreamL();
		CleanupClosePushL( readStream );
		
		iHistoryArray->InternalizeL(readStream);
		
		CleanupStack::PopAndDestroy(); //readStream
		
		for (TInt i = 0; i < iHistoryArray->Count(); i++)
			{
			CSyncMLHistoryEntry& hEntry = iHistoryArray->Entry(i);
			CSyncMLHistoryPushMsg * pushMsg = CSyncMLHistoryPushMsg::DynamicCast(&hEntry);
			
			if (pushMsg == NULL)
				continue;
			
			if ( aDigest.Compare( pushMsg->MsgDigest() ) == 0)
				{
				pushMsg->IncReceivedCount();
				iNewEntry = EFalse;
				iAlertInfo->SetConfirmation( iNewEntry );
				}
			}
	
		CleanupStack::PopAndDestroy(2); //profile, settings
		}
	else
		{
		CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
		
		CNSmlDMProfile* profile = settings->ProfileL(aProfileId);
		CleanupStack::PushL(profile);
		
		if ( ! profile->HasLogL() )
			{
			CleanupStack::PopAndDestroy(2); //profile, settings
			return;
			}
			
		RReadStream& readStream = profile->LogReadStreamL();
		CleanupClosePushL( readStream );

		iHistoryArray->InternalizeL( readStream );
		
		CleanupStack::PopAndDestroy(); //readStream
		
		for (TInt i = 0; i < iHistoryArray->Count(); i++)
			{
			CSyncMLHistoryEntry& hEntry = iHistoryArray->Entry(i);
			CSyncMLHistoryPushMsg * pushMsg = CSyncMLHistoryPushMsg::DynamicCast(&hEntry);
			
			if (pushMsg == NULL)
				continue;
			
			if ( aDigest.Compare( pushMsg->MsgDigest() ) == 0)
				{
				pushMsg->IncReceivedCount();
				iNewEntry = EFalse;
				iAlertInfo->SetConfirmation( iNewEntry );
				}
			}
		CleanupStack::PopAndDestroy(2); //profile, settings
		}
	}

// ---------------------------------------------------------
// CNSmlAlertHandler::SaveAlertInfoL( )
// Saves the result to profile history log and adds new entry
// ---------------------------------------------------------
void CNSmlAlertHandler::SaveAlertInfoL( )
	{
	
	TInt profileId( iAlertInfo->Profile() );
	
	if ( profileId == KNSmlNullId )
		{
		return;
		}
		
	if ( profileId < KMaxDataSyncID )
		{
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		
		CNSmlDSProfile* profile = settings->ProfileL( profileId );
		CleanupStack::PushL( profile );
		RWriteStream& stream = profile->LogWriteStreamL();
		CleanupClosePushL( stream );
		
		if ( iNewEntry )
			{
			RPointerArray<CSmlAlertInfo> alertInfoArray;
			CleanupClosePushL( alertInfoArray );
			
			alertInfoArray.AppendL( iAlertInfo );
			
			iHistoryInfo->AddAlertsL( alertInfoArray );
			iHistoryArray->AppendEntryL( iHistoryInfo );
			CleanupStack::PopAndDestroy(); //alertInfoArray	
			}
			
		iHistoryArray->ExternalizeL(stream);
		
		CleanupStack::PopAndDestroy(); //stream
		
		profile->WriteStreamCommitL();
	
		CleanupStack::PopAndDestroy(2); // profile, settings
		}
	else
		{
		CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
		
		CNSmlDMProfile* profile = settings->ProfileL( profileId );
		CleanupStack::PushL( profile );
		
		RWriteStream& stream = profile->LogWriteStreamL();
		CleanupClosePushL( stream );
		
		if ( iNewEntry )
			{
			RPointerArray<CSmlAlertInfo> alertInfoArray;
			CleanupClosePushL( alertInfoArray );
			
			alertInfoArray.AppendL( iAlertInfo );
		
			iHistoryInfo->AddAlertsL( alertInfoArray );
			iHistoryArray->AppendEntryL( iHistoryInfo );
			
			CleanupStack::PopAndDestroy(); //alertInfoArray	
			}
			
		iHistoryArray->ExternalizeL( stream );
		CleanupStack::PopAndDestroy(); //stream
		
		profile->WriteStreamCommitL();
		
		CleanupStack::PopAndDestroy(2); // profile, settings
		}
	}


// ---------------------------------------------------------
// CheckProtocolAndChange(TSmlProtocolVersion& aVersion)
// Checks if profile has different version and chages to alert's
// version if different.
// ---------------------------------------------------------
void CNSmlAlertHandler::CheckProtocolAndChangeL( TSmlProtocolVersion& aVersion ) const
	{
	TInt profileId( iAlertInfo->Profile() );
	
	if ( profileId == KNSmlNullId )
		{
		return;
		}
		
	if ( profileId < KMaxDataSyncID )
		{
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		CNSmlDSProfile* profile = settings->ProfileL( profileId );
		CleanupStack::PushL( profile );
		
		if ( profile->IntValue( EDSProfileProtocolVersion ) != aVersion )
			{
			profile->SetIntValue( EDSProfileProtocolVersion, (TInt) aVersion );
			profile->SaveL();
			}
			
		CleanupStack::PopAndDestroy(2); // profile, settings
		}
	}

//End of File
