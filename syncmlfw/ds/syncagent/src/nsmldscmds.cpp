/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS Sync flow
*
*/

#define SYNCML_V3
// INCLUDE FILES
#include <SyncMLObservers.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmlphoneinfo.h>
#include <nsmlunicodeconverter.h>
// common includes with DM
#include "nsmlcliagconstants.h"
#include "NSmlCmdsBase.h"
#include "NSmlAgentBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResponseController.h"
#include "NSmlURI.h"
#include "WBXMLSyncMLGenerator.h"
#include "WBXMLSyncMLParser.h"
#include "WBXMLDevInfGenerator.h"
#include "WBXMLDevInfParser.h"
#include "WBXMLGeneratorError.h"
#include "WBXMLParser.h"
#include "WBXMLParserError.h"
#include "smldtd.h"
#include "smldef.h"
// DS specific includes
#include "nsmldsagconstants.h"
#include "NSmlDSCmds.h"
#include "nsmldscontent.h"
#include "NSmlDSAgent.h"
#include "nsmldserror.h"
#include "nsmlfilter.h"
#include "nsmldssettings.h"
#include "nsmldsoperatorsettings.h"
#include "nsmldsbatchbuffer.h"
#include "nsmldshostclient.h"
#include <nsmloperatorerrorcrkeys.h> // KCRUidOperatorDatasyncErrorKeys
#include <nsmldevinfextdatacontainerplugin.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// CONSTANTS
_LIT8( KNSmlRoot, "/" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSCmds::CNSmlDSCmds()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSCmds::CNSmlDSCmds( CNSmlDSContent& aDSContent, MSyncMLProgressObserver& aDSObserver ) : iDSContent( aDSContent ), iDSObserver( aDSObserver )
    {
    iAtomicModeOn = EFalse;
    iAtomicId = 0;
    iAtomicHasFailed = EFalse;
    iItemOpened = EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ConstructL
// Symbian 2nd phase constructor. 
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ConstructL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, TBool aVersionCheck )
    {
	CNSmlCmdsBase::ConstructL( aAgent, aSessionID, aVerProto, aPublicId, aSyncMLUserName, aSyncServer, aVersionCheck );
    }

// -----------------------------------------------------------------------------
// CNSmlDSCmds::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSCmds* CNSmlDSCmds::NewL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, CNSmlDSContent& aDSContent, MSyncMLProgressObserver& aDSObserver, TBool aVersionCheck )
    {
	CNSmlDSCmds* self = new ( ELeave ) CNSmlDSCmds( aDSContent, aDSObserver );
	CleanupStack::PushL( self );
    self->ConstructL( aAgent, aSessionID, aVerProto, aPublicId, aSyncMLUserName, aSyncServer, aVersionCheck );
    CleanupStack::Pop();  // self
	return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDSCmds::~CNSmlDSCmds
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSCmds::~CNSmlDSCmds()
    {
	delete iBatchBuffer;	
	delete iDSGetCmdId;
	delete iDSGetMsgId;
	delete iDSLargeObjectCmdName;
	delete iDSDevGenerator;
	delete iDSPreviousSyncTargetURI;
	delete iDataBuffer;	
	delete iLargeUid;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::CommitBatchL
// Commits the batched commands and sets their statuses.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::CommitBatchL()
	{
	_DBG_FILE(_S8("CNSmlDSCmds::CommitBatchL : Begin"));
	if ( !iDSContent.BatchModeOn() )
		{
		return;
		}
		
	RArray<TInt> commitResults;
	TInt resultCode;
	
	iDSContent.CommitBatchL( commitResults, resultCode );

	// if the atomic mode is on and committing the batch fails then everyting will be rolled back
	if ( iAtomicModeOn && resultCode != KErrNone )	
		{
		iAtomicHasFailed = ETrue;
		commitResults.Close();
		iBatchModeOn = EFalse;
		delete iBatchBuffer;
		iBatchBuffer = NULL;
		return;
		}
	
	TInt commitIndex( 0 );

	for ( TInt batchIndex = 0; batchIndex < iBatchBuffer->Count(); batchIndex++ )
		{
		if ( resultCode != KErrNone )
			{
			iDSContent.IncreaseServerItemsFailed();

			iStatusToServer->SetStatusCodeL( iBatchBuffer->StatusEntryId( batchIndex ), TNSmlError::ESmlStatusCommandFailed );
			continue;
			}
			
		TInt statusId( KErrNone );
		
		for ( TInt i = 0; i < iBatchBuffer->NumberOfResults( batchIndex ); i++ )
			{
			if ( commitResults[commitIndex] != KErrNone )
				{
				statusId = commitResults[commitIndex];
				}
			
			++commitIndex;
			}
	
		// map the status code & set the command's status	
		MapStatusCodeL( batchIndex, statusId );
	
		if ( statusId != KErrNone )
			{
			iStatusToServer->SetStatusCodeL( iBatchBuffer->StatusEntryId( batchIndex ), iBatchBuffer->Status( batchIndex ) );
			}
		}
		
	iBatchModeOn = EFalse;
	delete iBatchBuffer;
	iBatchBuffer = NULL;
	_DBG_FILE(_S8("CNSmlDSCmds::CommitBatchL : End"));
	commitResults.Close();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::MapStatusCode
// Performs mapping between datastore operation return values and SyncML error
// codes.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::MapStatusCodeL( const TInt aIndex, const TInt aStatus )
	{
	TBool commandOk( EFalse );

	if ( iBatchBuffer->Command( aIndex ) == KNSmlAgentAdd )
		{
		switch ( aStatus )
			{
			case KErrNone:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusItemAdded );
				iDSContent.IncreaseServerItemsAdded();
				commandOk = ETrue;
				
				if ( !iAtomicModeOn )
					{
					iDSContent.CreateNewMapItemL( iBatchBuffer->Uid( aIndex ), iBatchBuffer->GUid( aIndex ), 0 );
					}
				else
					{
					iDSContent.CreateNewMapItemL( iBatchBuffer->Uid( aIndex ), iBatchBuffer->GUid( aIndex ), iAtomicId );
					}
				break;
				
			case KErrDiskFull:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusDeviceFull );
				iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
				break;
				
			case KErrNotSupported:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
				break;
				
			default:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusCommandFailed );
				break;
			}
		}
	else if ( iBatchBuffer->Command( aIndex ) == KNSmlAgentReplace )
		{
		switch ( aStatus )
			{
			case KErrNone:			
				iDSContent.IncreaseServerItemsChanged();				
				commandOk = ETrue;
				break;
				
			case KErrNotFound:
				{
				TSmlDbItemUid newLUid;
				
				TInt retval = iDSContent.CreateItemL( newLUid, 
													  iBatchBuffer->ItemData( aIndex ).Size(), 
													  iBatchBuffer->MimeType( aIndex ), 
													  iBatchBuffer->MimeVersion( aIndex ),
													  iBatchBuffer->Parent( aIndex ) );
				
				if ( retval == KErrNone )
					{
					TRAP( retval, iDSContent.WriteItemL( iBatchBuffer->ItemData( aIndex ) ) );
					}
					
				if ( retval == KErrNone )
					{
					retval = iDSContent.CommitItemL();
					}

				switch( retval )
					{
					case KErrNone:
						iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusItemAdded );

						iDSContent.IncreaseServerItemsAdded();
						commandOk = ETrue;
														
						if ( !iAtomicModeOn )
							{
							iDSContent.CreateNewMapItemL( newLUid, iBatchBuffer->GUid( aIndex ), 0 );
							}
						else
							{
							iDSContent.CreateNewMapItemL( newLUid, iBatchBuffer->GUid( aIndex ), iAtomicId );
							}
						break;
							
					case KErrDiskFull:
						iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusDeviceFull );
						iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
						break;
							
					case KErrNotSupported:
						iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
						break;
							
					default:
						iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusCommandFailed );
						break;
					}
				}
				break;
				
			case KErrDiskFull:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusDeviceFull );
				iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
				break;
				
			case KErrNotSupported:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
				break;
				
			default:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusCommandFailed );
				break;
			}
		}
	else if ( iBatchBuffer->Command( aIndex ) == KNSmlAgentDelete )
		{
		switch ( aStatus )
			{
			case KErrNone:
				iDSContent.IncreaseServerItemsDeleted();
				commandOk = ETrue;
				break;
				
			case KErrNotFound:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusItemIsNotDeleted );
				break;
				
			case KErrDiskFull:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusDeviceFull );
				iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
				break;
				
			case KErrNotSupported:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
				break;
				
			default:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusCommandFailed );
				break;
			}
		}
	else if ( iBatchBuffer->Command( aIndex ) == KNSmlAgentMove )
		{
		switch ( aStatus )
			{
			case KErrNone:
				iDSContent.IncreaseServerItemsMoved();
				commandOk = ETrue;
				break;
				
			case KErrDiskFull:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusDeviceFull );
				iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
				break;
				
			case KErrNotSupported:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
				break;
				
			default:
				iBatchBuffer->SetStatus( aIndex, TNSmlError::ESmlStatusMoveFailed );
				break;
			}
		}
		
	if ( !commandOk )
	    {
	    iDSContent.IncreaseServerItemsFailed();
	    }
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoAlertL
// Makes an Alert element and calls the parser to generate XML.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoAlertL( const TDesC8& aAlertCode, TTime* aLastSyncAnchor, TTime* aNextSyncAnchor )
	{
	SmlAlert_t* alert = new ( ELeave ) SmlAlert_t; 
	CleanupStack::PushL( alert );
	
	// element type
	alert->elementType = SML_PE_ALERT;
	
	// CmdID element
	DoCmdIdL( alert->cmdID );
	
	// Alert code to Data element 
	PcdataNewL ( alert->data, aAlertCode );
	alert->itemList = new ( ELeave ) SmlItemList_t;
	alert->itemList->item = new ( ELeave ) SmlItem_t;
	
	// Target and Source elements
	if ( aAlertCode == KNSmlDSAgentResultAlert || aAlertCode == KNSmlDSAgentNextMessage || aAlertCode == KNSmlDSAgentNoEndOfData )
		{
		DoTargetL( alert->itemList->item->target, iSentSyncHdrTarget->HostName() );
		DoSourceL( alert->itemList->item->source, iSentSyncHdrSource->HostName() );
		}
	else
		{
		DoTargetWithFilterL( alert->itemList->item->target, iDSContent.RemoteDatabase()->Database() );			
		DoSourceL( alert->itemList->item->source, iDSContent.LocalDatabase()->Database() ); 
		}

	// Anchor/Last and Anchor/Next in the Meta Element
	if ( aLastSyncAnchor && aNextSyncAnchor )
		{
		HBufC8* lastAnchor = HBufC8::NewLC( KNSmlDSAgentAnchorDateFormat.iTypeLength );
		HBufC8* nextAnchor = HBufC8::NewLC( KNSmlDSAgentAnchorDateFormat.iTypeLength );
		
		if ( *aLastSyncAnchor != TTime( 0 ) )
			{
			lastAnchor->Des().Format( KNSmlDSAgentAnchorDateFormat, 
										aLastSyncAnchor->DateTime().Year(),
										aLastSyncAnchor->DateTime().Month() + 1,
										aLastSyncAnchor->DateTime().Day() + 1,
										aLastSyncAnchor->DateTime().Hour(),
										aLastSyncAnchor->DateTime().Minute(),
										aLastSyncAnchor->DateTime().Second() );
			}
			
		if ( *aNextSyncAnchor != TTime( 0 ) )
			{
			nextAnchor->Des().Format(  KNSmlDSAgentAnchorDateFormat, 
										aNextSyncAnchor->DateTime().Year(),
										aNextSyncAnchor->DateTime().Month() + 1,
										aNextSyncAnchor->DateTime().Day() + 1,
										aNextSyncAnchor->DateTime().Hour(),
										aNextSyncAnchor->DateTime().Minute(),
										aNextSyncAnchor->DateTime().Second() );
			}
			
		if ( ( *aLastSyncAnchor != TTime( 0 ) ) || ( *aNextSyncAnchor != TTime( 0 ) ) )
			{
			SmlMetInfMetInf_t* metInf;
			DoMetInfLC( metInf ); 
			DoAnchorsL( *metInf, lastAnchor, nextAnchor );
			DoMetaL( alert->itemList->item->meta, metInf );
			CleanupStack::Pop(); // metInf
			}
			
		CleanupStack::PopAndDestroy( 2 ); // lastAnchor, nextAnchor
		}
		
	TInt ret( iGenerator->smlAlertCmd( alert) );
	
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // alert
	
	// save Status response information for response status controlling
	TInt respID( CreateAndInitResponseItemL( KNSmlAgentAlert ) );
	
	iResponseController->SetAppIndex( respID, iDSContent.CurrentIndex() );
	
	if ( aAlertCode >= KNSmlDSTwoWay && aAlertCode <= KNSmlDSRefreshFromServerByServer )
		{
		iResponseController->SetResponseDetail( respID, CNSmlResponseController::EResponseInitAlert );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoPutL
// Inserts DevInfo into Put element and calls the WBXML generator.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoPutL()
	{
	SmlPut_t* put = new ( ELeave )SmlPut_t; 
	CleanupStack::PushL( put );
	put->elementType = SML_PE_PUT;

	// CmdID element
	DoCmdIdL( put->cmdID );

	// Type element in Meta 
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC( metInf ); 
	PcdataNewL( metInf->type, KNSmlDSAgentDeviceInfoContentType );
	DoMetaL( put->meta, metInf );
	CleanupStack::Pop(); // metinf

	// Item element
	put->itemList = new ( ELeave )SmlItemList_t;
	put->itemList->item = new ( ELeave )SmlItem_t;

	// Item/Source element
	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		DoSourceL( put->itemList->item->source, KNSmlDSAgentDeviceInfo11URI ); 
		PcdataNewL( put->itemList->item->data, DoDeviceInfoL( ETrue ) );
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		DoSourceL( put->itemList->item->source, KNSmlDSAgentDeviceInfo12URI ); 
		PcdataNewL( put->itemList->item->data, DoDeviceInfoL() );
		} 

	// Device Info
	FreeDeviceInfo();
	
	TInt ret( iGenerator->smlPutCmd( put ) );
	
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // put
	
	//save Status response information for response status controlling
	CreateAndInitResponseItemL( KNSmlAgentPut );
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoResultsL
// Generate Device Info to Results element and calls the WBXML generator.
// -----------------------------------------------------------------------------
//
CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoResultsL()
	{
	SmlResults_t* results = new ( ELeave ) SmlResults_t; 
	CleanupStack::PushL( results );
	results->elementType = SML_PE_RESULTS;

	// CmdID element
	DoCmdIdL( results->cmdID );
	
	// MsgRef element
	if ( iDSGetMsgId )
		{
		PcdataNewL ( results->msgRef, *iDSGetMsgId );
		}
		
	// CmdRef element
	if ( iDSGetCmdId )
		{
		PcdataNewL ( results->cmdRef, *iDSGetCmdId );
		}
		
	// Type element in Meta 
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC( metInf ); 
	PcdataNewL( metInf->type, KNSmlDSAgentDeviceInfoContentType );
	DoMetaL( results->meta, metInf );
	CleanupStack::Pop(); // metinf
	
	// Item element
	results->itemList = new( ELeave ) SmlItemList_t;
	results->itemList->item = new( ELeave ) SmlItem_t;
	
	// Item/source element
	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		DoSourceL( results->itemList->item->source, KNSmlDSAgentDeviceInfo11URI ); 
		PcdataNewL( results->itemList->item->data, DoDeviceInfoL(ETrue) );
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		DoSourceL( results->itemList->item->source, KNSmlDSAgentDeviceInfo12URI ); 
		PcdataNewL( results->itemList->item->data, DoDeviceInfoL() );
		} 

	// Device Info
	results->itemList->next = NULL;
	TInt ret( iGenerator->smlResultsCmd( results ) );
	FreeDeviceInfo();

	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // results
	
	CreateAndInitResponseItemL( KNSmlAgentResults );
	
	return CNSmlCmdsBase::EReturnOK;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoGetL
// Generates the Get element used to ask DevInfo from the server.
// -----------------------------------------------------------------------------
//
void  CNSmlDSCmds::DoGetL()
	{
	SmlGet_t* get = new( ELeave ) SmlGet_t; 
	CleanupStack::PushL( get );
	get->elementType = SML_PE_GET;
	
	// CmdID element
	DoCmdIdL( get->cmdID );
	
	// Type element in Meta 
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC( metInf ); 
	PcdataNewL( metInf->type, KNSmlDSAgentDeviceInfoContentType );
	DoMetaL( get->meta, metInf );
	CleanupStack::Pop(); // metinf
	
	// Item element
	get->itemList = new( ELeave ) SmlItemList_t;
	get->itemList->item = new( ELeave ) SmlItem_t;
	
	// Item/Target element
	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		DoTargetL( get->itemList->item->target, KNSmlDSAgentDeviceInfo11URI ); 
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		DoTargetL( get->itemList->item->target, KNSmlDSAgentDeviceInfo12URI ); 
		} 

	TInt ret( iGenerator->smlGetCmd( get ) );
	
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // get
	
	//save Status response information for response status controlling
	CreateAndInitResponseItemL( KNSmlAgentGet );
	}

// -----------------------------------------------------------------------------
// CNSmlCmdsBase::DoStartSyncL
// Generates the Sync element.
// -----------------------------------------------------------------------------
//
CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoStartSyncL() 
	{
	// initialise SyncHdr
	SmlSync_t* sync = new( ELeave ) SmlSync_t; 
	CleanupStack::PushL( sync );

	// element type
	sync->elementType = SML_PE_SYNC_START;

	// CmdID element
	DoCmdIdL( sync->cmdID );

	// Target element
	DoTargetL( sync->target, iDSContent.RemoteDatabase()->Database() ); 

	// Source element
	DoSourceL( sync->source, iDSContent.LocalDatabase()->Database() ); 

	// free memory and max object size in the Meta element 
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC( metInf ); 
	DoMaxObjSizeL( *metInf, iDSContent.MaxObjSizeL() );
	DoMetaL( sync->meta, metInf );
	CleanupStack::Pop(); // metInf
	
	// NumberOfChanges element
	DoNumberOfChangesL( sync->noc );
	
	TInt ret( iGenerator->smlStartSync( sync ) );
	CNSmlCmdsBase::TReturnValue returnValue( CNSmlCmdsBase::EReturnOK );
	
	switch ( ret )
		{
		case KWBXMLGeneratorOk:
			break;
			
		case KWBXMLGeneratorBufferFull:
			returnValue = CNSmlCmdsBase::EReturnBufferFull;
			break;
			
		default:
			User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // sync
	
	// save Status response information for response status controlling
	TInt respID( CreateAndInitResponseItemL( KNSmlAgentSync ) );
	
	iResponseController->SetAppIndex( respID, iDSContent.CurrentIndex() ); 
	
	return returnValue; 
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoEndSyncL
// Generates the end tag of the Sync element and calls the WBXML generator.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoEndSyncL()  
	{
	TInt ret( iGenerator->smlEndSync() );
	
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoAddOrReplaceOrDeleteL
// Generates Add, Replace, Delete and Move elements and calls the parser to 
// generate xml.
// -----------------------------------------------------------------------------
//
CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoAddOrReplaceOrDeleteL() 
	{
	DBG_FILE(_S8("CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoAddOrReplaceOrDeleteL() begins"));
	TInt ret( KErrNone );
	TSmlDbItemUid localUid;
	TSmlDbItemUid parentUid( KNullDataItemId );
	TBuf8<KNSmlMaxCmdNameLength> cmdName;
	CNSmlDbMeta* meta( NULL );
	MSyncMLProgressObserver::TSyncMLDataSyncModifications clientModifications = { 0, 0, 0, 0, 0 };
	MSyncMLProgressObserver::TSyncMLDataSyncModifications serverModifications = { 0, 0, 0, 0, 0 };
	
	if ( iLargeData )
		{
		DBG_FILE(_S8("CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoAddOrReplaceOrDeleteL() iLargeData TRUE"));
		cmdName = *iDSLargeObjectCmdName;
		delete iDSLargeObjectCmdName;
		iDSLargeObjectCmdName = NULL;
		iDSContent.CurrentUID( localUid );
		DBG_FILE(_S8("CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoAddOrReplaceOrDeleteL() iLargeData TRUE ends"));
		}
	else
		{
		if ( iDataBuffer )
			{
			delete iDataBuffer;
			iDataBuffer = 0;
			}
		
		iDataBuffer = CBufFlat::NewL( 1 );
	
		meta =  CNSmlDbMeta::NewLC();
		DBG_FILE(_S8("before OpenNextItemL"));
		ret = iDSContent.OpenNextItemL( localUid, parentUid, cmdName, iItemSizeInStream, *meta );
		
		iBytesSent = 0;

		switch ( ret )
			{
			case KErrNone:
				iItemOpened = ETrue;
				break;
				
			case KErrEof:
				CleanupStack::PopAndDestroy(); // meta
				return CNSmlCmdsBase::EReturnEndData;
				
			case KErrNotFound:
				CleanupStack::PopAndDestroy(); // meta
				iDSContent.SetCurrentItemWritten();
				serverModifications.iNumFailed = 1;
				iDSObserver.OnSyncMLDataSyncModifications( iDSContent.TaskId(), clientModifications, serverModifications );
				return CNSmlCmdsBase::EReturnNotFound;
				
			default:
				User::Leave( ret );
			}

		// check that item's data can be accepted by the server
		TInt maxObjSize( iDSContent.MaxObjSizeInServer() );
		
		if ( maxObjSize > 0 )
			{
			if ( iItemSizeInStream > maxObjSize )
				{
				CleanupStack::PopAndDestroy(); // meta
				iDSContent.SetCurrentItemWritten();
				iDSContent.SetAsReceivedL( localUid );

				if ( iItemOpened )
					{
					iItemOpened = EFalse;
					iDSContent.CloseItemL();
					}

				serverModifications.iNumFailed = 1;
				iDSObserver.OnSyncMLDataSyncModifications( iDSContent.TaskId(), clientModifications, serverModifications );
				return CNSmlCmdsBase::EReturnSkipped;
				}
			}
		}

	// begin to process the data
	CNSmlCmdsBase::TReturnValue returnValue( CNSmlCmdsBase::EReturnOK );

	// initialise command element 
	SmlGenericCmd_t* genericCmd = new ( ELeave ) SmlGenericCmd_t; 
	CleanupStack::PushL( genericCmd );
		
	DBG_FILE(_S8("before DoCmdIdL"));
	// CmdID element
	DoCmdIdL( genericCmd->cmdID );

	// Item element
	genericCmd->itemList = new ( ELeave ) SmlItemList_t;
		
	// if in the middle of a large item then there's no need to recreate it
	if ( (iLargeData) && (iLargeObjectItem) )
		{
		DBG_FILE(_S8("LARGE"));
		genericCmd->itemList->item = iLargeObjectItem;
		iLargeObjectItem = NULL;
		}
	else
		{
		DBG_FILE(_S8("NO LARGE"));
		genericCmd->itemList->item = new ( ELeave ) SmlItem_t;

		// Source element
		TBuf<16> unicodeId;
		unicodeId.Num( localUid );

		DoSourceL( genericCmd->itemList->item->source, unicodeId );
				
		if ( parentUid != KNullDataItemId )
			{
			TBuf8<16> parentDes;

			if ( parentUid == KDbItemUidRoot )
				{
				parentDes.Append( KNSmlRoot );
				}
			else
				{
				parentDes.Num( parentUid );
				}

			genericCmd->itemList->item->sourceParent = new ( ELeave ) sml_source_or_target_parent_s; 
			PcdataNewL ( genericCmd->itemList->item->sourceParent->locURI, parentDes );
			}

		if ( cmdName == KNSmlAgentAdd || cmdName == KNSmlAgentReplace || cmdName == KNSmlAgentPartialReplace )
			{
			if ( meta )
				{
				SmlMetInfMetInf_t* metaInf = meta->MetInfL();
			
				if ( metaInf )
					{
					DoMetaL( genericCmd->itemList->item->meta, metaInf );
					}
				}
			}
		}
		
	// generate command
	if ( cmdName == KNSmlAgentAdd || cmdName == KNSmlAgentReplace || cmdName == KNSmlAgentPartialReplace )
		{
		// set data
		TPtr8 bufPtr( 0, 0 );
		TInt bufPos( iDataBuffer->Size() );

		if ( iItemSizeInStream - iBytesSent > iMaxWorkspaceSize )
			{
			iDataBuffer->ResizeL( iMaxWorkspaceSize );
			}
		else
			{
			iDataBuffer->ResizeL( iItemSizeInStream - iBytesSent );
			}
		
		bufPtr.Set( iDataBuffer->Ptr( bufPos ) );
		bufPtr.SetLength( 0 );
		iDSContent.ReadItemL( bufPtr );

		delete genericCmd->itemList->item->data;
		genericCmd->itemList->item->data = NULL;
		PcdataNewL( genericCmd->itemList->item->data, iDataBuffer->Ptr( 0 ) );
		
		// fill in the correct element structure		
		DBG_FILE_CODE( genericCmd->itemList->item->data->length, _S8("length"));
		if ( genericCmd->itemList->item->data->length >= KNSmlLargeObjectMinSize )
			{
			DBG_FILE(_S8("iLargeData SET TRUE"));
			DBG_FILE_CODE( genericCmd->itemList->item->data->length, _S8("length"));
			iLargeData = ETrue;
			iGenerator->SetContentLength( iItemSizeInStream );
			iGenerator->SetTruncate( ETrue );
			}
			
		if ( cmdName == KNSmlAgentAdd ) // Add
			{
			DBG_FILE(_S8("KNSmlAgentAdd"));
			genericCmd->elementType = SML_PE_ADD;
			ret = iGenerator->smlAddCmd( genericCmd );
			serverModifications.iNumAdded = 1;
			}
		else if ( cmdName == KNSmlAgentReplace ) // Replace
			{
			DBG_FILE(_S8("KNSmlAgentReplace"));
			genericCmd->elementType = SML_PE_REPLACE;
			ret = iGenerator->smlReplaceCmd( genericCmd );
			serverModifications.iNumReplaced = 1;
			}
		else if ( cmdName == KNSmlAgentPartialReplace ) // PartialReplace
			{
			DBG_FILE(_S8("KNSmlAgentPartialReplace"));
			SmlMetInfMetInf_t* metInf;
			DoMetInfLC( metInf );
			PcdataNewL( metInf->fieldLevel, KNullDesC8() );
			DoMetaL( genericCmd->meta, metInf );
			CleanupStack::Pop(); // metInf
			genericCmd->elementType = SML_PE_REPLACE;
			ret = iGenerator->smlReplaceCmd( genericCmd );
			serverModifications.iNumReplaced = 1;
			}
		}
	else if ( cmdName == KNSmlAgentDelete ) // Delete  
		{
		DBG_FILE(_S8("KNSmlAgentDelete"));
		genericCmd->elementType = SML_PE_DELETE;
		ret = iGenerator->smlDeleteCmd( genericCmd );
		serverModifications.iNumDeleted = 1;
		}
	else if ( cmdName == KNSmlAgentMove ) // Move  
		{
		DBG_FILE(_S8("KNSmlAgentMove"));
		genericCmd->elementType = SML_PE_MOVE;
		ret = iGenerator->smlMoveCmd( genericCmd );
		serverModifications.iNumMoved = 1;
		}
	else // SoftDelete
		{
		DBG_FILE(_S8("SoftDelete"));
		genericCmd->elementType = SML_PE_DELETE;
		genericCmd->flags += SmlSftDel_f;
		ret = iGenerator->smlDeleteCmd( genericCmd );
		serverModifications.iNumDeleted = 1;
		}
	DBG_FILE_CODE( ret, _S8("ret"));
		
	switch ( ret )
		{
		case KWBXMLGeneratorOk:
			if ( iGenerator->WasTruncated() )
				{
				DBG_FILE(_S8("WasTruncated TRUE"));
				if ( genericCmd->itemList->item->data->length < iGenerator->DataConsumed() )
					{
					DBG_FILE_CODE( iBytesSent, _S8("iBytesSent"));
					iBytesSent += genericCmd->itemList->item->data->length;
					DBG_FILE_CODE( iBytesSent, _S8("iBytesSent"));
					}
				else
					{
					DBG_FILE_CODE( iBytesSent, _S8("iBytesSent"));
					iBytesSent += iGenerator->DataConsumed();
					DBG_FILE_CODE( iBytesSent, _S8("iBytesSent"));
					}
					
				// change item's ownership from struct to this class
				iLargeObjectItem = genericCmd->itemList->item;
				genericCmd->itemList->item = NULL;
				
				if ( iLargeData )
					{
					delete iDSLargeObjectCmdName;
					iDSLargeObjectCmdName = NULL;
					iDSLargeObjectCmdName = cmdName.AllocL();
					}
					
				if ( iGenerator->DataConsumed() < iDataBuffer->Size() )
					{
					iDataBuffer->Delete( 0, iGenerator->DataConsumed() );
					iDataBuffer->Compress();
					}
				else
					{
					delete iDataBuffer;
					iDataBuffer = NULL;
					}
					
				returnValue = CNSmlCmdsBase::EReturnBufferFull;
				}
			else
				{
				DBG_FILE(_S8("WasTruncated FALSE"));
				iDSContent.SetCurrentItemWritten();
				
				if ( cmdName != KNSmlAgentDelete && cmdName != KNSmlAgentMove )
					{
					iBytesSent += genericCmd->itemList->item->data->length;
					DBG_FILE_CODE( genericCmd->itemList->item->data->length, _S8("length"));
					DBG_FILE_CODE( iBytesSent, _S8("iBytesSent"));
					if ( iBytesSent == iItemSizeInStream )
						{
						DBG_FILE(_S8("iLargeData SET FALSE"));
						iLargeData = EFalse;
						}
					}
				
				}
			break;
				
		case KWBXMLGeneratorBufferFull:
			DBG_FILE(_S8("KWBXMLGeneratorBufferFull"));
			delete iDSLargeObjectCmdName;
			iDSLargeObjectCmdName = NULL;
			iDSLargeObjectCmdName = cmdName.AllocL();
			returnValue = CNSmlCmdsBase::EReturnBufferFull;
			DBG_FILE(_S8("KWBXMLGeneratorBufferFull end"));
			break;
				
		default:
			DBG_FILE_CODE( ret, _S8("ret"));
			User::Leave( ret );
		}
			
	DBG_FILE(_S8("before SetTruncate( EFalse )"));			
	iGenerator->SetTruncate( EFalse );
		
	if ( ret == KWBXMLGeneratorOk )
		{
		DBG_FILE(_S8("KWBXMLGeneratorOk"));
		TInt respID( CreateAndInitResponseItemL( cmdName ) );
		TBuf8<16> localDes;
		localDes.Num( localUid );
		iResponseController->SetLUIDL( respID, localDes );
		iResponseController->SetAppIndex( respID, iDSContent.CurrentIndex() );
			
		if ( returnValue == CNSmlCmdsBase::EReturnBufferFull )
			{
			DBG_FILE(_S8("iLargeData SET TRUE after EReturnBufferFull"));
			iResponseController->SetMoreData( respID );
			iLargeData = ETrue;
			}
		}
		
	if ( !iLargeData )
		{
		DBG_FILE(_S8("!iLargeData"));
		iItemOpened = EFalse;
		iDSContent.CloseItemL();		
		}
		
	CleanupStack::PopAndDestroy(); // genericCmd
		
	if ( meta )
		{
		CleanupStack::PopAndDestroy(); // meta 
		}

	if ( returnValue == CNSmlCmdsBase::EReturnOK )
		{
		iDSObserver.OnSyncMLDataSyncModifications( iDSContent.TaskId(), clientModifications, serverModifications );
		}
		
	DBG_FILE_CODE( returnValue, _S8("returnValue"));	
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoMapL
// Build a structure for the Map command.
// -----------------------------------------------------------------------------
//
CNSmlCmdsBase::TReturnValue CNSmlDSCmds::DoMapL()  
	{
	if ( !iDSContent.MapExists() )
		{
		return EReturnEndData;
		}
		
	SmlMap_t* map = new( ELeave ) SmlMap_t; 
	CleanupStack::PushL( map );
	map->elementType = SML_PE_MAP;
	
	// CmdID element
	DoCmdIdL( map->cmdID );
	
	// Target element
	DoTargetL( map->target, iDSContent.RemoteDatabase()->Database() ); 

	// Source element
	DoSourceL( map->source, iDSContent.LocalDatabase()->Database() ); 

	map->mapItemList = iDSContent.MapItemListL();

	TInt ret( iGenerator->smlMapCmd( map ) );
 	CNSmlCmdsBase::TReturnValue returnValue( CNSmlCmdsBase::EReturnOK );
	
	switch ( ret )
		{
		case KWBXMLGeneratorOk:
			iDSContent.RemoveMap();
			break;
			
		case KWBXMLGeneratorBufferFull:
			iDSContent.SetMapItemList( map->mapItemList ); //map item list is returned 
			map->mapItemList = NULL;                        //to the buffer 
			returnValue = CNSmlCmdsBase::EReturnBufferFull;
			break;
			
		default:
			User::Leave( ret );
		}
		
	CleanupStack::PopAndDestroy(); // map
	
	if ( returnValue != CNSmlCmdsBase::EReturnBufferFull )
	    {
	    // save Status response information for response status controlling
	    TInt respID( CreateAndInitResponseItemL( KNSmlAgentMap ) );	
	    iResponseController->SetAppIndex( respID, iDSContent.CurrentIndex() );
	    }
	
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessResultsCmdL
// Processes DevInfo in the Results command that was received from the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessResultsCmdL( SmlResults_t* aResults )
	{
	TInt statusID( iStatusToServer->CreateNewStatusElementL() );

	iStatusToServer->SetCmdRefL( statusID, aResults->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentResults ); 

	if ( iStatusToServerNoResponse )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
		
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	// Is synchronisation already interrupted?
	if ( iDSContent.AllDatabasesAreInterrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
		
	if ( aResults->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
		
	// content type
	if ( !IsDevInfoContentTypeOkL( aResults->meta, statusID ) )
		{
		return;
		}
		
	// source
	if ( !SourceIsUnderItem( aResults->itemList ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlResultsItemSourceMissing, EFalse, EFalse );
		return;
		}
		
	CNSmlURI* deviceInfoURI( CNSmlURI::NewLC() );
	
	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo11URI );
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo12URI );
		} 

	HBufC* resultsSource;
	PCDATAToUnicodeLC( *aResults->itemList->item->source->locURI, resultsSource );
	resultsSource->Des().TrimRight();
	CNSmlURI* resultsSourceURI = CNSmlURI::NewLC();
	resultsSourceURI->SetDatabaseL( *resultsSource );
	
	if ( !resultsSourceURI->IsEqualL( *deviceInfoURI ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlResultsItemSourceInvalid, EFalse, EFalse );
		CleanupStack::PopAndDestroy( 3 ); // resultsSourceURI, resultsSource, deviceInfoURI
		return;
		}

	CleanupStack::PopAndDestroy( 3 ); // resultsSourceURI, resultsSource, deviceInfoURI

	// Device Info in Data element
	if ( !aResults->itemList->item->data )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoMissing, EFalse, EFalse );
		return;
		}
		
	if ( !aResults->itemList->item->data->content )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoMissing, EFalse, EFalse );
		return;
		}
		
	TPtrC8 deviceInfo( static_cast<TUint8*>( aResults->itemList->item->data->content ), aResults->itemList->item->data->length );

	ParseDeviceInfoL( statusID, deviceInfo );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessPutCmdL
// Handle the Put command from the server. DevInfo element in the command is
// processed.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessPutCmdL( SmlPut_t* aPut )
	{
	TInt statusID( iStatusToServer->CreateNewStatusElementL() );

	iStatusToServer->SetCmdRefL( statusID, aPut->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentPut ); 
	
	if ( iStatusToServerNoResponse || IsFlagSet( aPut->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
		
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	// Is synchronisation already interrupted?
	if ( iDSContent.AllDatabasesAreInterrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
		
	if ( aPut->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
		
	// content type
	if ( !IsDevInfoContentTypeOkL( aPut->meta, statusID ) )
		{
		return;
		}
		
	// source
	if ( !SourceIsUnderItem( aPut->itemList ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlPutItemSourceMissing, EFalse, EFalse );
		return;
		}
		
	CNSmlURI* deviceInfoURI( CNSmlURI::NewLC() );

	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo11URI );
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo12URI );
		} 

	HBufC* putSource;
	PCDATAToUnicodeLC( *aPut->itemList->item->source->locURI, putSource );
	putSource->Des().TrimRight();
	CNSmlURI* putSourceURI = CNSmlURI::NewLC();
	putSourceURI->SetDatabaseL( *putSource );
	
	if ( !putSourceURI->IsEqualL( *deviceInfoURI ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlPutItemSourceInvalid, EFalse, EFalse );
		CleanupStack::PopAndDestroy( 3 ); // putSourceURI, putSource, deviceInfoURI
		return;
		}
		
	CleanupStack::PopAndDestroy( 3 ); // putSourceURI, putSource, deviceInfoURI
	
	// Device Info in Data element
	if ( !aPut->itemList->item->data )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoMissing, EFalse, EFalse );
		return;
		}
		
	if ( !aPut->itemList->item->data->content )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoMissing, EFalse, EFalse );
		return;
		}
		
	TPtrC8 deviceInfo( static_cast<TUint8*>( aPut->itemList->item->data->content ), aPut->itemList->item->data->length );

	ParseDeviceInfoL( statusID, deviceInfo );
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessGetCmdL
// Handles the DevInfo request from the server in the Get command.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessGetCmdL( SmlGet_t* aGet )
	{
	TInt statusID( iStatusToServer->CreateNewStatusElementL() );
 
 	iStatusToServer->SetCmdRefL( statusID, aGet->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentGet ); 

	if ( iAtomicModeOn )
		{
		iStatusToServer->SetAtomicOrSequenceId( statusID, iAtomicId );
		}

	if ( iStatusToServerNoResponse || IsFlagSet( aGet->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
		
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	// Is synchronisation already interrupted?
	if ( iDSContent.AllDatabasesAreInterrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
		
	// cmdID
	if ( aGet->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
		
	// atomic command has failed
	if ( iAtomicHasFailed )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
		
	TPtrC8 cmdID( static_cast<TUint8*>( aGet->cmdID->content ), aGet->cmdID->length );

	delete iDSGetCmdId;
	iDSGetCmdId = NULL;
	iDSGetCmdId = cmdID.AllocL();

	delete iDSGetMsgId;
	iDSGetMsgId = NULL;
	iDSGetMsgId = iCurrServerMsgID->AllocL();

	// content type
	if ( !IsDevInfoContentTypeOkL( aGet->meta, statusID ) )
		{
		return;
		}
		
	// target
	if ( !TargetIsUnderItem( aGet->itemList ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlGetItemTargetMissing, EFalse, EFalse );
		return;
		}
		
	HBufC* getTarget;
	PCDATAToUnicodeLC( *aGet->itemList->item->target->locURI, getTarget );
	getTarget->Des().TrimRight();
	CNSmlURI* deviceInfoURI = CNSmlURI::NewLC();

	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo11URI );
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		deviceInfoURI->SetDatabaseL( KNSmlDSAgentDeviceInfo12URI );
		} 
		
	CNSmlURI* getTargetURI( CNSmlURI::NewLC() );
	
	getTargetURI->SetDatabaseL( *getTarget );
	
	if ( !getTargetURI->IsEqualL( *deviceInfoURI ) )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlGetItemTargetInvalid, EFalse, EFalse );
		CleanupStack::PopAndDestroy( 3 ); // getTargetURI, deviceInfoURI, getTarget
		return;
		}
		
	CleanupStack::PopAndDestroy( 3 ); // getTargetURI, deviceInfoURI, getTarget
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessAlertCmdL
// Handles the Alert command from a server.  
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessAlertCmdL( SmlAlert_t* aAlert, TBool aNextAlert, TBool aServerAlert, TBool aDisplayAlert )
	{
	TBool databaseExists( EFalse );
	TBool initAlert( ETrue );
	
	if ( aNextAlert || aServerAlert || aDisplayAlert )
		{
		initAlert = EFalse;
		}
		
	TInt statusID( iStatusToServer->CreateNewStatusElementL() );

	iStatusToServer->SetCmdRefL( statusID, aAlert->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentAlert );
	
	if ( iStatusToServerNoResponse || IsFlagSet( aAlert->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
		
	if ( iAtomicHasFailed )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
		
	if ( aNextAlert && iDSContent.AllDatabasesAreInterrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
	else
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
		}
		
	if ( aAlert->itemList )
		{
		if ( aAlert->itemList->item )
			{
			iStatusToServer->AddTargetRefL( statusID, aAlert->itemList->item->target );
			iStatusToServer->AddSourceRefL( statusID, aAlert->itemList->item->source );
			}
		else
			{
			iStatusToServer->AddTargetRefL( statusID, NULL );
			iStatusToServer->AddSourceRefL( statusID, NULL );
			}
		}
	else
		{
		iStatusToServer->AddTargetRefL( statusID, NULL );
		iStatusToServer->AddSourceRefL( statusID, NULL );
		}
		
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	// Next Sync anchor in Item/Meta element
	if ( initAlert )
		{
		if ( aAlert->itemList )
			{
			if ( aAlert->itemList->item )
				{
				if ( aAlert->itemList->item->meta )
					{
					if ( aAlert->itemList->item->meta->content && aAlert->itemList->item->meta->contentType == SML_PCDATA_EXTENSION && aAlert->itemList->item->meta->extension == SML_EXT_METINF )
						{
						SmlMetInfMetInf_t* metInf;
						metInf = (SmlMetInfMetInf_t*) aAlert->itemList->item->meta->content;
						if ( metInf->anchor )
							{
							iStatusToServer->AddItemDataL( statusID, aAlert->itemList->item->meta );
							}
						}
					}
				}
			}
		}
		
	TBool errorInCmdID( EFalse );
	
	if ( aAlert->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		errorInCmdID = ETrue; 
		}
		
	if ( aServerAlert )
		{
		databaseExists = MatchServerAlertL( aAlert, statusID );
		
		if ( !databaseExists )
			{
			return;
			}
			
		iDSContent.SetAsServerAlerted();
		iDSContent.InitSyncLogEventL( *iAgent->SyncLog() );
		}

	// DisplayAlert
	if( aDisplayAlert )
		{
		if ( !aAlert->itemList )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->Interrupt( TNSmlError::ESmlAlertInvalid, EFalse, EFalse );
			return;
			}
		if( !aAlert->itemList->item )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->Interrupt( TNSmlError::ESmlAlertInvalid, EFalse, EFalse );
			return;
			}

		// SyncmlNotifier Dependency is removed
		return;
		}

	// Target element
	if ( initAlert )
		{
		if ( !TargetIsUnderItem( aAlert->itemList ) )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->Interrupt( TNSmlError::ESmlAlertTargetLocURIMissing, EFalse, EFalse );
 			}
		else
			{
			HBufC* alertTarget;
			PCDATAToUnicodeLC( *aAlert->itemList->item->target->locURI, alertTarget );
			alertTarget->Des().TrimRight();
			CNSmlURI* alertTargetURI = CNSmlURI::NewLC( iSentSyncHdrSource->HostName() );
			alertTargetURI->SetDatabaseL( *alertTarget );
			databaseExists = iDSContent.SetIndexByLocalDatabaseL( *alertTargetURI );
			CleanupStack::PopAndDestroy(); // alertTargetURI
			
			if ( !databaseExists )     
				{
				iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusNotFound );
				}
				
			CleanupStack::PopAndDestroy(); // alertTarget

			if ( databaseExists && iDSContent.Interrupted() )
				{
				if ( !errorInCmdID )
					{
					iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
					}
				return;
				}
			}
		if ( !databaseExists )
			{
			return;
			}
		}
		
	// Alert Code in Data element
	if ( !aAlert->data )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlAlertCodeMissing, EFalse, EFalse );
		}
	else if ( !aAlert->data->content )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlAlertCodeMissing, EFalse, EFalse );
		}
	else
		{
		TPtr8 alertCode( static_cast<TUint8*>( aAlert->data->content ), aAlert->data->length, aAlert->data->length );

		TrimRightSpaceAndNull( alertCode );

		if ( alertCode.Length() == 0 )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlAlertCodeMissing, EFalse, EFalse );
			}
		if ( aNextAlert )
			{
			if ( alertCode != KNSmlDSAgentNextMessage )
				{
				iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
				iAgent->Interrupt( TNSmlError::ESmlAlertCodeInvalid, EFalse, EFalse );
				}
			}
		else
			{
			if ( ( alertCode != KNSmlDSTwoWay ) && ( alertCode !=  KNSmlDSSlowSync ) && ( alertCode != KNSmlDSTwoWayByServer ) )
				{
				if ( !iDSContent.IsSupportedSyncTypeL( alertCode ) )
					{
					iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOptFeatureNotSupported );
					iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlDSError::ESmlDSUnsupportedSyncType, EFalse, EFalse );
					}
				}
			//RD_SUSPEND_RESUME	
		   	 if(iDSContent.GetSessionResumed())
				 {
			 		TNSmlPreviousSyncType prevalertcode=iDSContent.PreviousSyncTypeL();
			 		TBool refreshrequired=EFalse;
			 		refreshrequired=iDSContent.GetRefreshRequired();
			 		//Checks if the Sync Type of the Resumed session and Suspened session is same
			 	  	if(prevalertcode!=ETypeNotSet && alertCode.Compare(CNSmlDSAgent::MapAlertCode(prevalertcode))!=0)
			        {
			          //Sesion is interrupted if the Resumed command rejected by the Server with  status code 508 &Sync Type other than 201 
			          //or if Resume command accepted by Server with 200 & Sync Type different than the Suspened session
			          if(refreshrequired && alertCode.Compare(KNSmlDSSlowSync)!=0 || !refreshrequired)
			          {
			          	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed);
						iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlDSError::ESmlDSUnsupportedSyncType, EFalse, EFalse );
			          }
			          //If the Resume session is rejected by the Server,Client modifications need to be sent in Pacakge#3
			          else if(refreshrequired && alertCode.Compare(KNSmlDSSlowSync)==0)
			          {
			            iDSContent.SetSessionResumedL(EFalse);
			          }
			        }	
			 	 }
			   //RD_SUSPEND_RESUME	 

			}
			
		if ( databaseExists )
			{
			if ( !iDSContent.Interrupted() )
				{
				if ( initAlert || aServerAlert )
					{
					// MaxObjSize
					TInt maxObjSize( ServerMaxObjSize( aAlert->itemList->item->meta ) );
					
					if ( maxObjSize > 0 )
						{
						iDSContent.SetMaxObjSizeInServer( maxObjSize ); 
						}
					}
					
				// update content specific data
				if ( initAlert )
					{
					iDSContent.SetSyncTypeL( alertCode );
					iDSContent.SetInitAlertReceived();
					}
					
				if ( aServerAlert )
					{
					SwitchAlertCode( alertCode );
					iDSContent.SetSyncTypeL( alertCode );
					}
				}
			}
		StoreSyncType( alertCode );
		}
		
	// status 406 is returned if <Filter> is present BUT the session continues
	if ( aAlert->itemList->item )
	    {
	    if ( aAlert->itemList->item->target )
	        {
    	    if ( aAlert->itemList->item->target->filter )
	            {
	            iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOptFeatureNotSupported );
	            }
	        }
	    }
	}

// ---------------------------------------------------------
// CNSmlDSCmds::AlertParameter()
// 
// ---------------------------------------------------------
TInt CNSmlDSCmds::AlertParameter( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
	{
	TInt valueNum = 0;
	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				if ( parameters.Length() > startPos + aParamID.Length() )
					{
					TPtrC8 strPart = parameters.Right( parameters.Length() - ( startPos + aParamID.Length() ) );
					TInt length = 0;
					while ( length < strPart.Length() )
						{
						TChar character = strPart[length];
						if ( character.IsDigit() )
							{
							++length;
							}
						else
							{
							break;
							}
						}
					TPtrC8 valueStr = strPart.Left(	length );
					TLex8 lexicalValue( valueStr );
					if ( lexicalValue.Val( valueNum ) != KErrNone )
						{
						valueNum = 0;
						}
					}
				}
			}
		}
	return valueNum; 
	}
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessSyncL
// Handles the Sync command from a server.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessSyncL( SmlSync_t* aSync )
	{
	TBool databaseExists( EFalse );
	TInt statusID( iStatusToServer->CreateNewStatusElementL() );
	
	iBatchModeOn = EFalse;
	
	iStatusToServer->SetCmdRefL( statusID, aSync->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentSync ); 
	
	if ( iStatusToServerNoResponse || IsFlagSet( aSync->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
		
	iStatusToServer->AddTargetRefL( statusID, aSync->target );
	iStatusToServer->AddSourceRefL( statusID, aSync->source );
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	if ( iAtomicModeOn )
		{
		iStatusToServer->SetAtomicOrSequenceId( statusID, iAtomicId );
		}
		
	// cmdID
	if ( aSync->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		}
		
	// Local Database in Target element
	if ( !aSync->target )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlSyncTargetLocURIMissing, EFalse, EFalse );
		}
	else
		{
		if ( !aSync->target->locURI )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->Interrupt( TNSmlError::ESmlSyncTargetLocURIMissing, EFalse, EFalse );
			}
		else
			{
			HBufC* syncTarget( NULL );
			
			PCDATAToUnicodeLC( *aSync->target->locURI, syncTarget );
			syncTarget->Des().TrimRight();
			
			if ( syncTarget->Length() == 0 )
				{
				iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
				iAgent->Interrupt( TNSmlError::ESmlSyncTargetLocURIMissing, EFalse, EFalse );
				}
			else
				{
				CNSmlURI* syncTargetURI = CNSmlURI::NewLC( iSentSyncHdrSource->HostName() );
				
				syncTargetURI->SetDatabaseL( *syncTarget );
				databaseExists = iDSContent.SetIndexByLocalDatabaseL( *syncTargetURI );
				
				if ( iMoreData )
					{
					if ( !syncTargetURI->IsEqualL( *iDSPreviousSyncTargetURI ) )
						{
						ResetLargeObjectBuffer();
						iAgent->SetEndOfDataAlertRequest();
						}
					}
					
				delete iDSPreviousSyncTargetURI;			
				iDSPreviousSyncTargetURI = syncTargetURI;
				CleanupStack::Pop(); // syncTargetURI
				}
				
			CleanupStack::PopAndDestroy(); // syncTarget
			}
		}
		
	if ( !databaseExists )     
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusNotFound );
		iAgent->Interrupt( TNSmlError::ESmlSyncTargetLocURIInvalid, EFalse, EFalse );
		
		if ( iAtomicModeOn )
			{
			iAtomicHasFailed = ETrue;
			}
		}
	
	// Is synchronisation already interrupted?
	// Is sync type one-way from client?
	if ( databaseExists )
		{
		if ( iDSContent.Interrupted() || iDSContent.SyncType() == KNSmlDSOneWayFromClient 
									  || iDSContent.SyncType() == KNSmlDSRefreshFromClient
									  || iDSContent.SyncType() == KNSmlDSOneWayFromClientByServer
									  || iDSContent.SyncType() == KNSmlDSRefreshFromClientByServer )
											
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
			return;
			}
		}
		
	// ensure that transactions are supported if the atomic mode is on
	if ( databaseExists && iAtomicModeOn )
		{
		if ( !iDSContent.SupportsOperationL( KUidSmlSupportTransaction ) )
			{
			iAtomicHasFailed = ETrue;
			}
		else
			{
			iDSContent.BeginTransActionL();
			}
		}

	// Format
	TPtrC8 format;
	
	if ( !IsChrFormat( aSync->meta, format ) ) 
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		SmlPcdata_t* data( NULL );
		PcdataNewL( data, format );
		CleanupStack::PushL( data );
		iStatusToServer->AddItemDataL( statusID, data );
		CleanupStack::PopAndDestroy(); // data
		iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlFormatNotSupported, EFalse, EFalse );
		}
		
	// MaxObjSize
	if ( databaseExists )
		{
		TInt maxObjSize( ServerMaxObjSize( aSync->meta ) );
		
		if ( maxObjSize > 0 )
			{
			iDSContent.SetMaxObjSizeInServer( maxObjSize ); 
			}
		}
		
	// NumberOfChanges
	if ( databaseExists )
		{
		TInt nocNum( -1 );
		TBool NocNumReceived( EFalse );
		if ( !iDSContent.ServerItemCountAsked() )
			{
			// If new number of changes info is received
			if ( aSync->noc )
				{
				if ( aSync->noc->content )
					{
					TPtr8 nocStr( static_cast<TUint8*>( aSync->noc->content ),
					              aSync->noc->length, aSync->noc->length );
					TrimRightSpaceAndNull( nocStr );
					TLex8 lexicalValue( nocStr );
					
					if ( lexicalValue.Val( nocNum ) == KErrNone )
						{
						NocNumReceived = ETrue;
						iDSContent.SetServerItemCount( nocNum );
						}
					}
				}
			// else number of changes information is not received
	        if ( !NocNumReceived )
	            {
	            iDSContent.SetServerItemCount( nocNum );
	            }
			iDSObserver.OnSyncMLSyncProgress(
            	MSyncMLProgressObserver::ESmlReceivingModificationsFromServer,
	        	iDSContent.ServerItemCount(), iDSContent.TaskId() );
			}

        // If number of changes is more than batch limit then batch is used if
        // plug in adapter supports batch mode
	    if ( iDSContent.ServerItemCount() >= KNSmlBatchLimit && !iMoreData )
	        {
			if ( iDSContent.SupportsOperationL( KUidSmlSupportBatch ) )
				{
				iBatchModeOn = ETrue;
				iDSContent.BeginBatchL();
				
				if ( !iBatchBuffer )
					{
					iBatchBuffer = new ( ELeave ) CNSmlDSBatchBuffer;
					}
				else
					{
					iBatchBuffer->Reset();
					}
				}
			}
	    }
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessEndSyncL
// Handles the end of the Sync command.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessEndSyncL()
	{
	if ( iDSContent.ContentIndexIsSet() )
		{
		CommitBatchL();	
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessUpdatesL
// Processes received Add, Replace, Move and Delete commands.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessUpdatesL( const TDesC8& aCmd, SmlGenericCmd_t* aContent )
	{
	// Is the sync previously interrupted?
	TBool alreadyInterrupted ( EFalse );
	
	if ( iDSContent.AllDatabasesAreInterrupted() )
		{
		alreadyInterrupted = ETrue;
		}
	else
		{
		if ( !iDSContent.ContentIndexIsSet() )
			{
			alreadyInterrupted = ETrue;
			}
		else
			{
			if ( iDSContent.Interrupted() )
				{
				alreadyInterrupted = ETrue;
				}
			}
		}
		
    if ( !aContent->itemList )
		{
		return;
		}
	else
		{
		if ( !aContent->itemList->item )
			{
			return;
			}
		}
		
	SmlItemList_t* itemList( aContent->itemList );
	SmlItem_t* item( aContent->itemList->item );
	
	do
		{
		TBool processUpdate( EFalse );
		
		if ( alreadyInterrupted )
			{
			StatusDataToGenericCommandL( aCmd, aContent, item, TNSmlError::ESmlStatusCommandFailed );
			}
		else if ( iAtomicHasFailed )
			{
			StatusDataToGenericCommandL( aCmd, aContent, item, TNSmlError::ESmlStatusCommandFailed );
			}
		else
			{
			TBool typeAndFormatOK( EFalse );
			
			if ( iMoreData )
				{
				typeAndFormatOK = ETrue;
				}
			else
				{
				if ( CheckAndSetReceivedMediaTypeL( aCmd, aContent, item ) )
					{
					if ( IsFormatOkL( aCmd, aContent, item ) )
						{
						typeAndFormatOK = ETrue;
						}
					}
				}
				
			if ( typeAndFormatOK )
				{
				TSmlDbItemUid parent;
				TBool resumeBatchMode( EFalse );
				HBufC8* sourceParent( NULL );
				
				processUpdate = ETrue;
				
				// check if SourceParent is received
				if ( !IsParentOkL( item->targetParent, parent ) )
					{
					if ( IsParentOkLC( item->sourceParent, sourceParent ) )
						{
						if ( iBatchModeOn )
							{
							CommitBatchL();
							resumeBatchMode = ETrue;
							}
								
						if ( !iDSContent.MapSourceParent( *sourceParent, parent ) )
							{
							TInt statusId;
							
							if ( aCmd == KNSmlAgentMove )
								{
								statusId = StatusDataToGenericCommandL( aCmd, aContent, item, TNSmlError::ESmlStatusMoveFailed );							
								}
							else
								{
								statusId = StatusDataToGenericCommandL( aCmd, aContent, item, TNSmlError::ESmlStatusCommandFailed );							
								}
								
							processUpdate = EFalse;
							
							if ( iAtomicModeOn )
								{
								iStatusToServer->SetAtomicOrSequenceId( statusId, iAtomicId );
								iAtomicHasFailed = ETrue;
								}
							}
						}
					// Move MUST include either TargetParent or SourceParent
					else if ( aCmd == KNSmlAgentMove )
						{
						processUpdate = EFalse;
						StatusDataToGenericCommandL( aCmd, aContent, item, TNSmlError::ESmlStatusIncompleteCommand );
						}
					}

				if ( processUpdate ) 
					{
					HBufC8* cmdLUID( NULL );
					HBufC8* cmdGUID( NULL );
	
					if ( aCmd == KNSmlAgentReplace || aCmd == KNSmlAgentDelete || aCmd == KNSmlAgentMove )
						{
						if ( IsLUIDOkLC( aCmd, aContent, item, cmdLUID ) )
							{
							UpdateL( aCmd, aContent, item, *cmdLUID, parent );
							}
						else
							{
							processUpdate = EFalse;
							
							if ( iAtomicModeOn )
								{
								iAtomicHasFailed = ETrue;
								}
							}
						}
					else  // Add
						{
						if ( IsGUIDOkLC( aCmd, aContent, item, cmdGUID ) )
							{
							UpdateL( aCmd, aContent, item, *cmdGUID, parent );
							}
						else
							{
							processUpdate = EFalse;
							
							if ( iAtomicModeOn )
								{
								iAtomicHasFailed = ETrue;
								}
							}
						}
						
						if ( cmdGUID )
							{
							CleanupStack::PopAndDestroy(); // cmdGUID;
							}
							
						if ( cmdLUID )
							{
							CleanupStack::PopAndDestroy(); // cmdLUID;
							}
					}
				
				if ( sourceParent )
					{
					CleanupStack::PopAndDestroy(); // sourceParent
					}
					
				if ( resumeBatchMode )
					{
					iDSContent.BeginBatchL();
					}
				}
			}
			
		// if no updates were done then the item has failed
		if ( !processUpdate )
			{
			MSyncMLProgressObserver::TSyncMLDataSyncModifications clientModifications = { 0, 0, 0, 0, 0 };
			MSyncMLProgressObserver::TSyncMLDataSyncModifications serverModifications = { 0, 0, 0, 0, 0 };
			
			clientModifications.iNumFailed = 1;
			iDSObserver.OnSyncMLDataSyncModifications( iDSContent.TaskId(), clientModifications, serverModifications );

			if ( iDSContent.ContentIndexIsSet() )
				{
				iDSContent.IncreaseServerItemsFailed();
				}
			}

		// next item
		item = NULL;
		
		if ( itemList->next )
			{
			itemList = itemList->next;
			
			if ( itemList->item )
				{
				item = itemList->item;
				}
			}
		} while ( item );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessAtomicL
// Processes the Atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessAtomicL( SmlAtomic_t* aAtomic )
	{
	TInt statusId( iStatusToServer->CreateNewStatusElementL() );
	
	iStatusToServer->SetCmdRefL( statusId, aAtomic->cmdID );
	iStatusToServer->SetCmdL( statusId, KNSmlAgentAtomic );
	
	if ( iStatusToServerNoResponse || IsFlagSet( aAtomic->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusId, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusId, EFalse );
		}
		
	if ( iDSContent.AllDatabasesAreInterrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusId, TNSmlError::ESmlStatusAtomicFailed );
		return;
		}
		
	++iAtomicId;
	
	if ( iDSContent.ContentIndexIsSet() )
		{
		if ( iDSContent.SupportsOperationL( KUidSmlSupportTransaction) )
			{
			iAtomicHasFailed = EFalse;
			iDSContent.BeginTransActionL();
			iStatusToServer->SetStatusCodeL( statusId, TNSmlError::ESmlStatusOK );
			iStatusToServer->SetAtomicOrSequenceId( statusId, iAtomicId );
			}
		else
			{
			iAtomicHasFailed = ETrue;		
			iStatusToServer->SetStatusCodeL( statusId, TNSmlError::ESmlStatusAtomicFailed );
			}
		}
	else
		{
		iAtomicHasFailed = EFalse;
		iStatusToServer->SetStatusCodeL( statusId, TNSmlError::ESmlStatusOK );
		iStatusToServer->SetAtomicOrSequenceId( statusId, iAtomicId );
		}
		
	iAtomicModeOn = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessEndAtomicL
// Handles the end of the Atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessEndAtomicL()
	{
	TInt resultCode;

	iDSContent.SaveCurrentIndex();
	iDSContent.SetToFirst();
		
	do
		{
		if ( iDSContent.BatchModeOn() )
			{
			CommitBatchL();
			}
		}
	while ( iDSContent.SetToNext() );
		
	iDSContent.SetIndexToSaved();
	
	if ( iAtomicHasFailed )
		{
		iDSContent.SaveCurrentIndex();
		iDSContent.SetToFirst();
		
		do
			{
			if ( iDSContent.AtomicModeOn() )
				{
				iDSContent.RemoveFailedMappingsL( iAtomicId );
				iDSContent.RevertTransactionL( resultCode );
				}
			}
		while ( iDSContent.SetToNext() );
				
		iDSContent.SetIndexToSaved();
		
		iStatusToServer->SetStatusCodesInAtomicL( iAtomicId, TNSmlError::ESmlStatusRollBackOK, EFalse );
		iStatusToServer->SetStatusCodeToAtomicOrSequenceCmdL( iAtomicId, TNSmlError::ESmlStatusAtomicFailed, KNSmlAgentAtomic );
		}
	else
		{
		TBool atomicSucceeded( ETrue );
		iDSContent.SaveCurrentIndex();
		iDSContent.SetToFirst();
		
		do
			{
			if ( iDSContent.AtomicModeOn() )
				{
				iDSContent.CommitTransactionL( resultCode );
			
				if ( resultCode != KErrNone )
					{
					atomicSucceeded = EFalse;
					}
				}
			}
		while ( iDSContent.SetToNext() );
		
		iDSContent.SetIndexToSaved();
		
		if ( !atomicSucceeded )
			{
			iStatusToServer->SetStatusCodesInAtomicL( iAtomicId, TNSmlError::ESmlStatusRollBackOK, EFalse );
			iStatusToServer->SetStatusCodeToAtomicOrSequenceCmdL( iAtomicId, TNSmlError::ESmlStatusAtomicFailed, KNSmlAgentAtomic );
			}
		}
		
	iAtomicModeOn = EFalse;
	iAtomicHasFailed = EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessSequenceL
// Processes the Sequence command. Currently this command is not supported.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessSequenceL( SmlSequence_t* /*aSequence*/ )
	{
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessEndSequence
// Handles the end of the Sequence command. Currently this command is not
// supported.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessEndSequence()
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoEndMessageL
// Generates end tag of a SyncML element and possible final flag in SyncBody.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoEndMessageL( TBool aFinal )  
	{
	TInt ret;
	
	if ( aFinal )
		{
		ret = iGenerator->smlEndMessage( ETrue );
		}
	else
		{
		ret = iGenerator->smlEndMessage( EFalse );
		}
		
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::MatchServerAlertL
// Constructs URIs for local and remote databases received from a server. 
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::MatchServerAlertL( const SmlAlert_t* aAlert, TInt aStatusID )
	{
	TBool databaseExists( EFalse );
	HBufC* imeiCode = HBufC::NewLC( 50 );
	HBufC* alertTarget;
	TPtr imeiCodePtr( imeiCode->Des() );
	iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, imeiCodePtr );
	CNSmlURI* localURI = CNSmlURI::NewLC( *imeiCode );
	
	if ( !TargetIsUnderItem( aAlert->itemList ) )
		{
		alertTarget = HBufC::NewLC( 0 );
 		}
	else
		{
		PCDATAToUnicodeLC( *aAlert->itemList->item->target->locURI, alertTarget );
		alertTarget->Des().TrimRight();
		}
		
	localURI->SetDatabaseL( *alertTarget );

	HBufC* alertSource;
	CNSmlURI* remoteURI = CNSmlURI::NewLC();

	*remoteURI = *iSyncServer;
	
	if ( !SourceIsUnderItem( aAlert->itemList ) )
		{
		alertSource = HBufC::NewLC( 0 );
 		}
	else
		{
		PCDATAToUnicodeLC( *aAlert->itemList->item->source->locURI, alertSource );
		alertSource->Des().TrimRight();
		}
		
	remoteURI->SetDatabaseL( *alertSource );
	
	SmlMetInfMetInf_t* metInf;
	TPtr8 mediaType( NULL, 0, 0 );
	
	if ( aAlert->itemList->item->meta )
		{
		if ( aAlert->itemList->item->meta->content && 
			aAlert->itemList->item->meta->contentType == SML_PCDATA_EXTENSION && 
			aAlert->itemList->item->meta->extension == SML_EXT_METINF )
			{
			metInf = (SmlMetInfMetInf_t*) aAlert->itemList->item->meta->content;
			
			if ( metInf->type )
				{
				mediaType.Set( static_cast<TUint8*>( metInf->type->content ), metInf->type->length, metInf->type->length );
				TrimRightSpaceAndNull( mediaType );
				}
			}
		}
	
	if( iDSContent.SetIndexByRemoteDatabaseL( *remoteURI,ETrue ) )
		{
		databaseExists = ETrue;
		}
	else
		{		
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusNotFound );    		 
		}
	
	if ( iUnknownDevice && databaseExists )	
	    {	    
	    databaseExists = EFalse;
	    iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusNotFound );	        	    
	    }	
	CleanupStack::PopAndDestroy( 5 ); // alertSource, remoteURI, alertTarget, localURI, imeiCode
	
	return databaseExists;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoDeviceInfoL
// Builds DevInfo structure.
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSCmds::DoDeviceInfoL( TBool aConvert )
	{
	// create generator instance first
	if ( !iDSDevGenerator )
		{
		iDSDevGenerator = new( ELeave ) CWBXMLDevInfGenerator;
		}
		
	// create workspace
	iDSDevGenerator->CreateWorkspaceL();
	
	//initialise DevInf
	SmlDevInfDevInf_t* devInf = new( ELeave ) SmlDevInfDevInf_t; 
	CleanupStack::PushL( devInf );
	
	// VerDTD element
	PcdataNewL ( devInf->verdtd, KNSmlAgentVerDTD12 );

    TBool isOperator = EFalse;
    TInt profileId( iAgent->ProfileId() );
    CNSmlDSOperatorSettings* settings = CNSmlDSOperatorSettings::NewLC();

    CNSmlDSSettings* dsSettings = CNSmlDSSettings::NewLC();
    CNSmlDSProfile* profile = dsSettings->ProfileL( profileId );

    if( profile )
        {
        CleanupStack::PushL( profile );
        isOperator = settings->IsOperatorProfileL( 
            profile->StrValue( EDSProfileServerId ) );
        CleanupStack::PopAndDestroy( profile );
        }
    
	// Man element (manufacturer)
	HBufC* manufacturer = HBufC::NewLC( 50 );
	TPtr manufacturerPtr( manufacturer->Des() );
	HBufC8* manufacturerInUTF8( NULL );
    if ( isOperator )
        {
        manufacturerInUTF8 = settings->CustomManValueLC();
        if ( manufacturerInUTF8->Length() == 0 )
            {
            CleanupStack::PopAndDestroy();// manufacturerInUTF8
            iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneManufacturer, manufacturerPtr );
            NSmlUnicodeConverter::HBufC8InUTF8LC( *manufacturer, manufacturerInUTF8 );
            }
        }
    else
        {
        iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneManufacturer, manufacturerPtr );
        NSmlUnicodeConverter::HBufC8InUTF8LC( *manufacturer, manufacturerInUTF8 );
        }
	PcdataNewL ( devInf->man, *manufacturerInUTF8 );
	CleanupStack::PopAndDestroy( 2 ); // manufacturerInUTF8, manufacturer
	
	PcdataNewL ( devInf->fwv, KNullDesC8() );
	
	// Mod element (model name)
	HBufC* model = HBufC::NewLC( 50 );
	TPtr modelPtr = model->Des();
	HBufC8* modelInUTF8 = NULL;
	if ( isOperator )
	    {
	    modelInUTF8 = settings->CustomModValueLC();
	    if ( modelInUTF8->Length() == 0 )
	        {
	        CleanupStack::PopAndDestroy();// modelInUTF8
	        iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneModelId, modelPtr );
	        NSmlUnicodeConverter::HBufC8InUTF8LC( *model, modelInUTF8 );
	        }
	    }
	else
	    {
	    iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneModelId, modelPtr );
	    NSmlUnicodeConverter::HBufC8InUTF8LC( *model, modelInUTF8 );
	    }
	PcdataNewL ( devInf->mod, *modelInUTF8 );
	CleanupStack::PopAndDestroy( 2 );   // modelInUTF8, model
	// SwV element (software version)
	if ( isOperator )
	    {
	    HBufC8* swv = settings->CustomSwvValueLC();
	    if ( swv->Length() > 0 )
	        {
	        PcdataNewL ( devInf->swv, *swv );
	        }
	    else
	        {
	        PcdataNewL ( devInf->swv, iPhoneInfo->SwVersionL() );
	        }
	    CleanupStack::PopAndDestroy( swv );
	    }
	else
	    {
	    PcdataNewL ( devInf->swv, iPhoneInfo->SwVersionL() );
	    }

	CleanupStack::PopAndDestroy( 2, settings );

	PcdataNewL ( devInf->hwv, KNullDesC8() );

	// DevId element  
	HBufC* devId = HBufC::NewLC( 50 );
	TPtr devIdPtr( devId->Des() );
	iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, devIdPtr );
	HBufC8* devIdInUTF8( NULL );
	NSmlUnicodeConverter::HBufC8InUTF8LC( *devId, devIdInUTF8 );
	PcdataNewL ( devInf->devid, *devIdInUTF8 );
	CleanupStack::PopAndDestroy( 2 ); // devIdInUTF8, devId
	
	// DevTyp element
	_LIT8( KDevTyp, "phone" );
	PcdataNewL ( devInf->devtyp, KDevTyp );

	// SupportLargeObjs element
	devInf->flags += SmlSupportLargeObjects_f;

	// SupportNumberOfChanges element
	devInf->flags += SmlSupportNumberOfChanges_f;
	
	// SupportUTC element
	devInf->flags += SmlUTC_f;

	// DataStore elements
	SmlDevInfDatastoreList_t** currDatastorePtr = &devInf->datastore;

	// Operator specific Device info extensions (XNam, XVal)
	if( isOperator )
		{
		TRAPD( err, InsertOperatorExtensionDevInfFieldsL( devInf ) );
		if( err != KErrNone )
			{
			DBG_FILE( _S8( "CNSmlDSCmds::DoDeviceInfoL(): Leave in InsertOperatorExtensionDevInfFieldsL()" ) );
			DBG_FILE_CODE( err, _S8("Error code") );
			}
		}

    iDSContent.SetToFirst();

	do
		{
		if ( iDSContent.Interrupted() )
			{
			continue;
			}

		CDesCArray* storeNames = iDSContent.StoreNamesL();
		CleanupStack::PushL( storeNames );

		for ( TInt i = 0; i < storeNames->Count(); i++ )
			{
			CNSmlDbCaps* dbCaps;
			TInt result = iDSContent.DbCapabilitiesL( dbCaps );
			
			if ( result != KErrNone )
			    {
			    iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlStatusCommandFailed, EFalse, EFalse );
			    continue;
			    }
			
			CleanupStack::PushL( dbCaps );

			// DataStore begins
			*currDatastorePtr = new( ELeave ) SmlDevInfDatastoreList_t;
			(*currDatastorePtr)->data = dbCaps->DatastoreL(); // ownership is changed
			
			CNSmlURI *databaseURI = CNSmlURI::NewLC();
			databaseURI->SetDatabaseL( (*storeNames)[i], ETrue );
			HBufC8* databaseURIInUTF8;
			NSmlUnicodeConverter::HBufC8InUTF8LC( databaseURI->Database(), databaseURIInUTF8 );
			delete (*currDatastorePtr)->data->sourceref;
			PcdataNewL ( (*currDatastorePtr)->data->sourceref,*databaseURIInUTF8 );
			CleanupStack::PopAndDestroy( 2 ); // databaseURI, databaseURIInUTF8
 
			(*currDatastorePtr)->data->filtercap = dbCaps->FilterCaps();
			 
			// SourceRef element
			// MaxGUIDSize
			PcdataNewL ( (*currDatastorePtr)->data->maxguidsize, KNSmlDSAgentMaxGUIDSize );
			// RxPref, Rx, TxPref and Tx elements are already filled by an adpater
			// SyncCap begins
			(*currDatastorePtr)->data->synccap = new( ELeave ) SmlDevInfSyncCap_t;
			(*currDatastorePtr)->data->synccap->synctype = new( ELeave ) SmlPcdataList_t;
			SmlPcdataList_t** currSynctypePtr =	&(*currDatastorePtr)->data->synccap->synctype;
			// Sync Type elements
					
			TBuf8<1> stringSyncType; 
			
			stringSyncType.Num( (TInt) CNSmlDbCaps::ETwoWaySync );
			PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
			(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
			currSynctypePtr = &(*currSynctypePtr)->next;
			stringSyncType.Num( (TInt) CNSmlDbCaps::ESlowTwoWaySync );
			PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
			
			if ( iDSContent.IsSupportedSyncTypeL( CNSmlDbCaps::EOneWaySyncFromClient ) )
				{
				(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
				currSynctypePtr = &(*currSynctypePtr)->next;
				stringSyncType.Num( (TInt) CNSmlDbCaps::EOneWaySyncFromClient );
				PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
				}
				
			if ( iDSContent.IsSupportedSyncTypeL( CNSmlDbCaps::ERefreshSyncFromClient ) )
				{
				(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
				currSynctypePtr = &(*currSynctypePtr)->next;
				stringSyncType.Num( (TInt) CNSmlDbCaps::ERefreshSyncFromClient );
				PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
				}
				
			if ( iDSContent.IsSupportedSyncTypeL( CNSmlDbCaps::EOneWaySyncFromServer ) )
				{
				(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
				currSynctypePtr = &(*currSynctypePtr)->next;
				stringSyncType.Num( (TInt) CNSmlDbCaps::EOneWaySyncFromServer );
				PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
				}
				
			if ( iDSContent.IsSupportedSyncTypeL( CNSmlDbCaps::ERefreshSyncFromServer ) )
				{
				(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
				currSynctypePtr = &(*currSynctypePtr)->next;
				stringSyncType.Num( (TInt) CNSmlDbCaps::ERefreshSyncFromServer );
				PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );
				}
				
			(*currSynctypePtr)->next = new( ELeave ) SmlPcdataList_t;
			currSynctypePtr = &(*currSynctypePtr)->next;
			stringSyncType.Num( (TInt) CNSmlDbCaps::EServerAlertedSync );
			PcdataNewL ( (*currSynctypePtr)->data, stringSyncType );

			// SyncCap ends
			currDatastorePtr = &(*currDatastorePtr)->next;
			
			CleanupStack::PopAndDestroy(); // dbCaps
			}

			CleanupStack::PopAndDestroy(); // storeNames
		} while ( iDSContent.SetToNext() ); 

	if ( aConvert )
		{
		CNSmlDbCaps* dbcaps = CNSmlDbCaps::NewLC();
		dbcaps->ConvertDeviceInfoL( devInf );
		CleanupStack::PopAndDestroy(); // dbcaps
		}	
	
	TInt ret( iDSDevGenerator->smlDeviceInfo( devInf ) );
	
	CleanupStack::PopAndDestroy(); // devInf 
	
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
		
	return iDSDevGenerator->Document();
	}

// -----------------------------------------------------------------------------
// CNSmlCmdsBase::DoTargetWithFilterL
// Makes Source or Target element with Filter.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoTargetWithFilterL( sml_target_s*& aTarget, const TDesC& aLocURIData) const
	{
    DoTargetL( aTarget, aLocURIData );
    aTarget->locName = NULL;
    
    if ( !iDSContent.HasFilters() )
        {
        return;
        }

    CNSmlFilter* filter( CNSmlFilter::NewLC() );
    
    TInt retval( iDSContent.GetFilterL( filter ) );
    
    if ( retval == KErrNone )
        {
        aTarget->filter = filter->FilterL();
        }
    else if ( retval != KErrNotSupported )
        {
        User::Leave( retval );
        }
    
    CleanupStack::PopAndDestroy(); // filter
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::FreeDeviceInfo
// Frees resources allocated by the DevInfo generator.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::FreeDeviceInfo() 
	{
	delete iDSDevGenerator;
	iDSDevGenerator = NULL;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ParseDeviceInfoL
// Parses the given DevInfo structure.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ParseDeviceInfoL( TInt aStatusID, const TPtrC8& aDeviceInfo )
	{
	CWBXMLDevInfDocHandler* devDocHandler = CWBXMLDevInfDocHandler::NewL( this );
	CleanupStack::PushL( devDocHandler );
	
	CWBXMLParser* devParser = CWBXMLParser::NewL();
	CleanupStack::PushL( devParser );
	
	devParser->SetDocumentHandler( devDocHandler );
	devParser->SetExtensionHandler( devDocHandler );
	RDesReadStream devReadStream( aDeviceInfo );
	devParser->SetDocumentL( devReadStream );
	TWBXMLParserError ret( KWBXMLParserErrorOk );
	iDSDeviceInfoParsed = EFalse;
	
	do
		{
		ret = devParser->ParseL();
		
		if ( ret != KWBXMLParserErrorOk && ret != KWBXMLParserErrorEof )
			{
			iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoInvalid, EFalse, EFalse );
			iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusCommandFailed );
			}
		} while ( ret == KWBXMLParserErrorOk && iDSDeviceInfoParsed == EFalse );   
		
	CleanupStack::PopAndDestroy( 2 ); // devParser, devDocHandler
	}


// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessDeviceInfoL
// Processes the DevInfo structure received from a server.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessDeviceInfoL( const SmlDevInfDevInf_t& aContent )	
	{
	iDSDeviceInfoParsed = ETrue;	
	TInt statusID( iStatusToServer->LastEntryID() );
	
	// VerDTD
	if ( iVersionCheck )
		{
		if ( !IsVersionOkL( aContent, statusID ) )
			return;
		}
		
	if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		if ( !UpdateServerIdInSettingsL( aContent, statusID ) )
			return;
		}

	// DataStore
	SmlDevInfDatastoreList_t* currDatastore( aContent.datastore );
	SmlDevInfCtCapList_t* currCtCapList( aContent.ctcap );

	RPointerArray<CNSmlDbCaps> dbCapArray;
	CleanupStack::PushL( TCleanupItem( DeleteRPointerArray, &dbCapArray ) );
    // check if all remote databases are present in devinf
    SmlDevInfDatastoreList_t* tempDatastore = currDatastore;
    RPointerArray<CNSmlURI> srcRefArray;
    TInt count = 0;
    
    while( tempDatastore )
    {
    	if ( !tempDatastore->data )
			{
			tempDatastore = tempDatastore->next;
			continue;
			}
		HBufC8* srcRef = tempDatastore->data->sourceref->Data().AllocLC();
		TPtr8 srcRefPtr( srcRef->Des() );
		TrimRightSpaceAndNull( srcRefPtr );
		CNSmlURI* srcRefURI = CNSmlURI::NewLC( iSyncServer->HostName() );
		count = count + 2;
		
		srcRefURI->SetDatabaseL( *srcRef );
		srcRefArray.Append( srcRefURI );		
		tempDatastore = tempDatastore->next;
    }
    
    TBool resp( EFalse );
    if ( iRespURI )
	{
		resp = ETrue;
	}
    if( !iDSContent.CheckValidRemoteDbL( srcRefArray, resp ))
    {
        // interrupt sync session, Invalid remote database error shown
    	iAgent->Interrupt( TNSmlError::ESmlStatusNotFound, EFalse, EFalse );
    }
    
    CleanupStack::PopAndDestroy( count );
	while ( currDatastore )
		{
		if ( !currDatastore->data )
			{
			currDatastore = currDatastore->next;
			continue;
			}
		
		CNSmlDbCaps* dbcaps = CNSmlDbCaps::NewLC();
		// If device info from server is accordant with 1.1 standard,
		// copy it under correct datastore (like in 1.2)
		AppendCTCapsL( *dbcaps, currDatastore->data, currCtCapList );
	
		dbCapArray.Append( dbcaps );
		CleanupStack::Pop(); // dbcaps

		// SourceRef
		TBool databaseExists( EFalse );
		HBufC8* sourceRef = currDatastore->data->sourceref->Data().AllocLC();
		TPtr8 sourceRefPtr( sourceRef->Des() );
		TrimRightSpaceAndNull( sourceRefPtr );
		
		if ( sourceRefPtr.Length() == 0 )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
			iAgent->Interrupt( TNSmlDSError::ESmlDSDatastoreSourceRefMissing, EFalse, EFalse );
			CleanupStack::PopAndDestroy(); // sourceRef
			break;
			}
		else
			{
			CNSmlURI* sourceRefURI = CNSmlURI::NewLC( iSyncServer->HostName() );
			sourceRefURI->SetDatabaseL( *sourceRef );
			TBool respURIExists( EFalse );
			
			if ( iRespURI )
				{
				respURIExists = ETrue;
				}
				
			databaseExists = iDSContent.SetIndexByRemoteDatabaseL( *sourceRefURI, respURIExists );
			CleanupStack::PopAndDestroy(); // sourceRefURI
			}
			
		CleanupStack::PopAndDestroy(); // sourceRef
		
		if ( !databaseExists )     
			{
			currDatastore = currDatastore->next;
			continue;
			}
		
		if ( !IsRxTxOkL( currDatastore, statusID, dbCapArray ) )
			{
			// Something wrong with Rx or Tx
			break;
			}

		// Sync Types
		CArrayFix<HBufC8*>* syncTypeArray = new( ELeave ) CArrayFixFlat<HBufC8*>(7);
		CleanupStack::PushL( syncTypeArray );
		TInt pushed( 1 );
		SmlPcdataList_t* syncTypeList( NULL );
		
		if ( currDatastore->data->synccap )
			{
			syncTypeList = currDatastore->data->synccap->synctype;
			}
			
		while ( syncTypeList )
			{
			if ( syncTypeList->data )
				{
				HBufC8* syncType = syncTypeList->data->Data().AllocLC();
				++pushed;
				TPtr8 syncTypePtr( syncType->Des() );
				TrimRightSpaceAndNull( syncTypePtr );
				syncTypeArray->AppendL( syncType );
				}
				
			syncTypeList = syncTypeList->next;
			}
			
		iDSContent.UpdateSyncTypeL( *syncTypeArray );	
		CleanupStack::PopAndDestroy( pushed ); // syncType, syncTypeArray

		// Support HierarchicalSync
		TInt support( 0 );
		
		if ( currDatastore->data->supportHierarchicalSync )
			{
			support = 1;
			}

		iDSContent.UpdateSupportHierarchicalSyncL( support );
		
		// Filter-rx
		iDSContent.RemoveFilterCapsL();

		if ( currDatastore->data->filterrx )
			{
			SmlDevInfXmitList_t* filterRxList( currDatastore->data->filterrx );
			
			while ( filterRxList )
				{
				if ( filterRxList->data )
					{
					HBufC8* filterRxCTType = filterRxList->data->cttype->Data().AllocLC();
					TPtr8 filterRxCTTypePtr( filterRxCTType->Des() );
					TrimRightSpaceAndNull( filterRxCTTypePtr );
					ProcessFilterCapL ( *currDatastore->data->filtercap, *filterRxCTType );
					CleanupStack::PopAndDestroy(); // filterRxCTType
					}
					
				filterRxList = filterRxList->next;
				}
			}
			
		currDatastore = currDatastore->next;
		}
		
	CleanupStack::PopAndDestroy(); // dbCapArray
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsVersionOkL
// Checks the verDTD field in DevInfo.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsVersionOkL( const SmlDevInfDevInf_t& aContent, TInt aStatusID )
	{
	TBool versionOK( ETrue );
	HBufC8* verDTD = aContent.verdtd->Data().AllocLC();
	TPtr8 verDTDPtr( verDTD->Des() );
	TrimRightSpaceAndNull( verDTDPtr );
	
	if ( verDTDPtr.Length() == 0 || verDTDPtr != *iVerDTD )  
		{
		versionOK = EFalse;
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusVersionNotSupported );	
		SmlPcdata_t* data( NULL );
		PcdataNewL( data, *iVerDTD );
		CleanupStack::PushL( data );
		
		iStatusToServer->AddItemDataL( aStatusID, data );
		CleanupStack::PopAndDestroy(); // data
		
		if ( verDTDPtr.Length() == 0 )
			{
			iAgent->Interrupt( TNSmlError::ESmlVerDTDMissing, EFalse, EFalse );
			}
		else
			{
			iAgent->Interrupt( TNSmlError::ESmlVerDTDNotSupported, EFalse, EFalse );
			}
		}
		
	CleanupStack::PopAndDestroy(); // verDTD
	
	return versionOK;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::UpdateServerIdInSettingsL
// ServerId is dragged out of device info because OMA settings are currently 
// lacking ServerId.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::UpdateServerIdInSettingsL( const SmlDevInfDevInf_t& aContent, TInt aStatusID )
	{
	TBool serverIdOK( ETrue );
	HBufC8* serverId = aContent.devid->Data().AllocLC();
	TPtr8 serverIdPtr( serverId->Des() );
	TrimRightSpaceAndNull( serverIdPtr );
	
	if ( serverIdPtr.Length() == 0 )  
		{
		serverIdOK = EFalse;
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDeviceInfoInvalid, EFalse, EFalse );
		}
	else
		{
		TInt profileId( iAgent->ProfileId() );
		CNSmlDSProfile* profile = CNSmlDSSettings::NewLC()->ProfileL( profileId );
		
		if ( !profile )
			{
			User::Leave( TNSmlError::ESmlErrorInSettings );
			}
		else
			{
			CleanupStack::PushL( profile );
			HBufC* unicode( NULL );
			NSmlUnicodeConverter::HBufC16InUnicodeLC( serverIdPtr, unicode );
			
			// If this is a first sync with this server, there is a default
			// ServerId (='None') in the AgentLog and Authentication tables.
			// It must be updated.
			TInt status = iDSContent.UpdateServerIdL( *unicode );
			
			if ( status == KErrNone )
				{
				profile->SetStrValue( EDSProfileServerId, *unicode );
				profile->SaveL();
				}
			else
				{
				iAgent->Interrupt( status, EFalse, EFalse );
				serverIdOK = EFalse;
				}
				
			CleanupStack::PopAndDestroy( 2 ); //profile, uniPtr
			}
		CleanupStack::PopAndDestroy();
		}
		
	CleanupStack::PopAndDestroy(); // serverId
	
	return serverIdOK;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsRxTxOkL
// Checks that the types proposed by the server in DevInfo are supported both 
// for receiving and sending.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsRxTxOkL( SmlDevInfDatastoreList_t* aCurrDatastore, TInt aStatusID, RPointerArray<CNSmlDbCaps>& aDbCapArray )
	{
	iDSContent.RemoveCtCapsL();
		
	// RxPref
	TBool clientCanTransmitType( EFalse );
	HBufC8* rxPrefCTType( NULL );
	
	if (  aCurrDatastore->data->rxpref )
		{
		if ( aCurrDatastore->data->rxpref->cttype )
			{
			rxPrefCTType = aCurrDatastore->data->rxpref->cttype->Data().AllocLC();
			TPtr8 rxPrefCTTypePtr( rxPrefCTType->Des() );
			TrimRightSpaceAndNull( rxPrefCTTypePtr );
			
			if ( rxPrefCTTypePtr.Length() == 0 )
				{
				CleanupStack::PopAndDestroy(); // rxPrefCTType
				rxPrefCTType = NULL;
				}
			}
		}
		
	if ( !rxPrefCTType )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDatastoreRxPrefCTTypeMissing, EFalse, EFalse );
		return EFalse;
		}
	
	TInt status ( KErrNone );
	TRAPD( error, status = iDSContent.IsSupportedTxL( *rxPrefCTType ) );
	
	if ( error != KErrNone )
	    {
	    iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusCommandFailed );
		iAgent->Interrupt( error, EFalse, EFalse );
		CleanupStack::PopAndDestroy(); // rxPrefCTType
		return EFalse;
	    }
	    	
	if ( status )
		{
		// if client supports RxPref it is used
		clientCanTransmitType = ETrue;
		}
		
	CleanupStack::PopAndDestroy(); // rxPrefCTType
		
	// Rx elements
	// Read only when RxPref is not supported
	if ( !clientCanTransmitType )
		{
		SmlDevInfXmitList_t* rxList = aCurrDatastore->data->rx;
		while ( rxList )
			{
			HBufC8* rxCTType = rxList->data->cttype->Data().AllocLC();
			TPtr8 rxCTTypePtr( rxCTType->Des() );
			TrimRightSpaceAndNull( rxCTTypePtr );
			
			TInt status (KErrNone );

        	TRAP( error, status = iDSContent.IsSupportedTxL( *rxCTType ) );
        	if ( error != KErrNone )
        	    {        	    
        		CleanupStack::PopAndDestroy(); // rxCTType
        		break;
        	    }
	    
			if ( status )
				{
				clientCanTransmitType = ETrue;
				CleanupStack::PopAndDestroy(); // rxCTType
				break;
				}
				
			CleanupStack::PopAndDestroy(); //rxCTType
			rxList = rxList->next;
			}
		}
	
	if ( error != KErrNone )	
	    {
	    iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusCommandFailed );
		iAgent->Interrupt( error, EFalse, EFalse );
		return EFalse;     
	    }
	else
	if ( !clientCanTransmitType )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDatastoreRxCTTypeNotMatching, EFalse, EFalse );
		return EFalse;
		}

	//txPref
	TBool clientCanReceiveType( EFalse );
	HBufC8* txPrefCTType( NULL );
	
	if ( aCurrDatastore->data->txpref )
		{
		if ( aCurrDatastore->data->txpref->cttype )
			{
			txPrefCTType = aCurrDatastore->data->txpref->cttype->Data().AllocLC();
			TPtr8 txPrefCTTypePtr( txPrefCTType->Des() );
			TrimRightSpaceAndNull( txPrefCTTypePtr );
			
			if ( txPrefCTTypePtr.Length() == 0 )
				{
				CleanupStack::PopAndDestroy(); // txPrefCTType
				txPrefCTType = NULL;
				}
			}
		}
		
	if ( !txPrefCTType )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDatastoreTxPrefCTTypeMissing, EFalse, EFalse );
		return EFalse;  // break the loop
		}
		
	
	TRAP( error, status = iDSContent.IsSupportedRxL( *txPrefCTType ));
	if ( error != KErrNone )
	    {
	    iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusCommandFailed );
		iAgent->Interrupt( error, EFalse, EFalse );
		CleanupStack::PopAndDestroy(); // txPrefCTType
		return EFalse;
	    }
	else
	if ( status )
		{
		iDSContent.UpdateCtCapsL( aDbCapArray );
		clientCanReceiveType = ETrue;
		}
		
	CleanupStack::PopAndDestroy(); // txPrefCTType
	
	
	// Tx elements
	if ( !clientCanReceiveType )
		{
		SmlDevInfXmitList_t* txList = aCurrDatastore->data->tx;
		while ( txList )
			{
			if ( txList->data )
				{
				HBufC8* txCTType = txList->data->cttype->Data().AllocLC();
				TPtr8 txCTTypePtr( txCTType->Des() );
				TrimRightSpaceAndNull( txCTTypePtr );
				
				TInt status ( EFalse );
	            TRAP( error, status = iDSContent.IsSupportedRxL( *txCTType ));
	            if ( error != KErrNone )
            	    {
            	    CleanupStack::PopAndDestroy(); // txCTType            	    
            		break;
            	    }
            	else    
				if ( status )
					{
					clientCanReceiveType = ETrue;
					iDSContent.UpdateCtCapsL( aDbCapArray );
					CleanupStack::PopAndDestroy(); // txCTType
					break;
					}
					
				CleanupStack::PopAndDestroy(); // txCTType
				}
				
			txList = txList->next;
			}
		}
		
	if ( error != KErrNone )	
	    {
	    iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusCommandFailed );
		iAgent->Interrupt( error, EFalse, EFalse );
		return EFalse;     
	    }
	else
	if ( !clientCanReceiveType )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlDSError::ESmlDSDatastoreTxCTTypeNotMatching, EFalse, EFalse );
		return EFalse; 
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessCTCapL
// Reads CTCap elements and stores them to the Agent log.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessCTCapL( SmlDevInfCtCapList_t& aCapList, const TDesC8& aCTType ) const
	{
	CNSmlCtCapsHandler* ctCapsHandler = new(ELeave) CNSmlCtCapsHandler( &aCapList );
	CleanupStack::PushL( ctCapsHandler );
	CArrayFix<TNSmlCtCapData>* ctCapArray = new( ELeave ) CArrayFixFlat<TNSmlCtCapData>(8);
	CleanupStack::PushL( ctCapArray );
	ctCapsHandler->GetCtCapsL( *ctCapArray, aCTType );
	
	if ( ctCapArray->Count() > 0 )
		{
		CArrayFix<TNSmlCtCapData>* currCtCapArray = new( ELeave ) CArrayFixFlat<TNSmlCtCapData>(8);
		CleanupStack::PushL( currCtCapArray );

		for ( TInt i = 0; i < currCtCapArray->Count(); i++ )
			{
			if ( (*currCtCapArray)[i].iTag == TNSmlCtCapData::ECtType )
				{
				if ( (*currCtCapArray)[i].iValue == aCTType )
					{
					// CtCaps of the given content type are already stored
					CleanupStack::PopAndDestroy( 3 ); // currCtCapArray, ctCapArray, ctCapsHandler
					return;
					}
				}
			}
			
		for ( TInt j = 0; j < ctCapArray->Count(); j++ )
			{
			TNSmlCtCapData ctCapData( (*ctCapArray)[j].iTag, (*ctCapArray)[j].iValue );
			currCtCapArray->AppendL( ctCapData );
			}
			
		CleanupStack::PopAndDestroy(); // currCtCapArray
		}
    
	CleanupStack::PopAndDestroy( 2 ); // ctCapArray, ctCapsHandler
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ProcessFilterCapL()
// Reads FilterCap elements and stores them to the Agent log.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::ProcessFilterCapL ( SmlDevInfFilterCapList_t& aFilterCapList, const TDesC8& aCTType ) const
	{
	CNSmlFilterCapsHandler* filterCapsHandler = new ( ELeave ) CNSmlFilterCapsHandler ( &aFilterCapList );
	CleanupStack::PushL( filterCapsHandler );
	CArrayFix<TNSmlFilterCapData>* filterCapArray = new ( ELeave ) CArrayFixFlat<TNSmlFilterCapData>(8);
	CleanupStack::PushL( filterCapArray );
	filterCapsHandler->GetFilterCapsL( *filterCapArray, aCTType );

	if ( filterCapArray->Count() > 0 )
	{
		CArrayFix<TNSmlFilterCapData>* currFilterCapArray = new ( ELeave ) CArrayFixFlat<TNSmlFilterCapData>(8);
		CleanupStack::PushL( currFilterCapArray );
		iDSContent.GetSavedFilterCapsL( currFilterCapArray );
		
		for ( TInt i = 0; i < currFilterCapArray->Count(); i++ )
			{
			if ( (*currFilterCapArray)[i].iTag == TNSmlFilterCapData::ECtType )
				{
				if ( (*currFilterCapArray)[i].iValue == aCTType )
					{
					// FilterCaps of given content type are already stored
					CleanupStack::PopAndDestroy( 3 ); //currFilterCapArray, filtarCapArray, filterCapsHandler
					return;
					}
				}
			}
			
		for ( TInt j = 0; j < filterCapArray->Count(); j++ )
			{
			TNSmlFilterCapData filterCapData ( (*filterCapArray)[j].iTag, (*filterCapArray)[j].iValue );
			currFilterCapArray->AppendL ( filterCapData );
			}
			
		iDSContent.UpdateFilterCapsL ( currFilterCapArray );
		
		CleanupStack::PopAndDestroy( ); // currFilterCapArray
		}
		
	CleanupStack::PopAndDestroy( 2 ); // filterCapArray, filterCapsHandler
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::DoNumberOfChangesL
// Generates the NumberOfChanges element.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::DoNumberOfChangesL( SmlPcdata_t*& aNoc ) const
	{
	HBufC8* stringNoc = HBufC8::NewLC( 16 );	
	//RD_SUSPEND_RESUME	- Code commented for Usability issue.
	/*if(iDSContent.SuspendedState() == EStateServerModification && iDSContent.GetSessionResumed() )
		stringNoc->Des().Num( 0 );
	else*/
	//RD_SUSPEND_RESUME
	stringNoc->Des().Num( iDSContent.ClientItemCount() );
	PcdataNewL ( aNoc, *stringNoc );
	CleanupStack::PopAndDestroy(); // stringNoc
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::GetChunkL
// Buffer the received item and handles large objects.
// -----------------------------------------------------------------------------
//
TNSmlError::TNSmlSyncMLStatusCode CNSmlDSCmds::GetChunkL( const TDesC8& aUid, const SmlItem_t* aCurrentItem, const SmlPcdata_t* aMetaInCommand, CBufBase*& aBuffer )
	{
	iItemSizeInStream = 0;
	TNSmlError::TNSmlSyncMLStatusCode status( TNSmlError::ESmlStatusOK );
	iMoreData = IsFlagSet( aCurrentItem->flags, SmlMoreData_f );
	DBG_ARGS(_S("iMoreData flag (GetChunk): %d"), iMoreData);
	// check that the previously received chunk belongs to the same item
	if ( iAmountReceived != 0 )
		{
		if ( aUid != *iLargeUid )
			{
			iAgent->SetEndOfDataAlertRequest();
			delete iRecBuf;
			iRecBuf = NULL;
			delete iLargeUid;
			iLargeUid = NULL;
			iServerLargeObjectSize = 0;
			}
		}
		
	if ( iMoreData && ( iAmountReceived == 0 ) )
		{
		iServerLargeObjectSize = ServerObjectSize( aCurrentItem->meta );
		
		if ( iServerLargeObjectSize == 0 )
			{
			iServerLargeObjectSize = ServerObjectSize( aMetaInCommand );
			}
		
		iItemSizeInStream = iServerLargeObjectSize;
		delete iLargeUid;
		iLargeUid = NULL;
		iLargeUid = aUid.AllocL();
		}
		
	iRecBuf = CBufFlat::NewL( 1 );
	
	// buffer data
	if ( aCurrentItem->data )
		{
		if ( iItemSizeInStream == 0 )
			{
			iItemSizeInStream = aCurrentItem->data->length;
			}
			
		if ( aCurrentItem->data->content )
			{
			TPtr8 data( static_cast<TUint8*>( aCurrentItem->data->content ), aCurrentItem->data->length, aCurrentItem->data->length );
			iRecBuf->InsertL( iRecBuf->Size(), data );
			}
		}
	else
		{
		aBuffer = iRecBuf;
		iRecBuf = NULL;
		return status;
		}
		
	// last / only chunk
	if ( !iMoreData )
		{
		TBool notFirst( iAmountReceived != 0 );
		iAmountReceived += aCurrentItem->data->length;
		
		if ( notFirst && ( iAmountReceived != iServerLargeObjectSize ) )
			{
			status = TNSmlError::ESmlStatusSizeMismatch;
			delete iRecBuf;
		    iRecBuf = NULL;	
			}
        aBuffer = iRecBuf;
		iRecBuf = NULL;
		iAmountReceived = 0;
		}
	// middle chunk
	else
		{
		iAmountReceived += aCurrentItem->data->length;
		aBuffer = iRecBuf;
		status = TNSmlError::ESmlStatusItemAccepted;
		}
		
	return status;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::UpdateL
// Performs updates to local databases.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::UpdateL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, const TDesC8& aUID, const TSmlDbItemUid aParent )
	{	
	TInt returnCode( KErrNone );
	TInt statusId( -1 );
	TBool resumeBatchMode( EFalse );
	TBool backToBatchMode( EFalse );
	MSyncMLProgressObserver::TSyncMLDataSyncModifications clientModifications = { 0, 0, 0, 0, 0 };
	MSyncMLProgressObserver::TSyncMLDataSyncModifications serverModifications = { 0, 0, 0, 0, 0 };

	if ( aCmd == KNSmlAgentAdd || aCmd == KNSmlAgentReplace )
		{
		TPtr8 data( 0, 0 );
		CBufBase* dataBuffer( NULL );
		TBool canGo( EFalse );
		if ( iMoreData )
    	    {
    	    backToBatchMode = ETrue;
    	    }
		TNSmlError::TNSmlSyncMLStatusCode statusCode( GetChunkL( aUID, aItem, aContent->meta, dataBuffer ) );
    	if ( !iMoreData && backToBatchMode )
    	    {
    	    resumeBatchMode = ETrue;
    	    }

		if ( dataBuffer )
			{
			data.Set( dataBuffer->Ptr( 0 ) );
			CleanupStack::PushL( dataBuffer );
			}
			
		// if size of large item does not match
		if ( statusCode == TNSmlError::ESmlStatusSizeMismatch )
		    {
		    statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusSizeMismatch );
		    }
        // check that some data was actually received		    
		else if ( data.Length() == 0 )
			{
			statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
			}
		else
			{
			if ( statusCode == TNSmlError::ESmlStatusOK )
				{
				canGo = ETrue;
				}
			else if ( statusCode == TNSmlError::ESmlStatusItemAccepted )
				{
				// large items are not allowed during batch mode
				if ( iBatchModeOn )
					{
					CommitBatchL();
					resumeBatchMode = ETrue;
					}
					
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusItemAccepted );
				canGo = ETrue;
				}
			}

		if ( canGo )
			{
			SmlMetInfMetInf_t* metInf( NULL );
				
			if ( aItem->meta )
				{
				if ( ( aItem->meta->content ) && ( aItem->meta->contentType == SML_PCDATA_EXTENSION ) && ( aItem->meta->extension == SML_EXT_METINF ) )
					{
					if ( ((SmlMetInfMetInf_t*) aItem->meta->content)->type )
						{
						metInf = (SmlMetInfMetInf_t*) aItem->meta->content;
						}
					}
				}
					
			if ( !metInf )
				{
				if ( (aContent->meta) && ( aContent->meta->content ) && ( aContent->meta->contentType == SML_PCDATA_EXTENSION ) && ( aContent->meta->extension == SML_EXT_METINF ) )
					{
					metInf = (SmlMetInfMetInf_t*) aContent->meta->content;
					}
				}
					
			CNSmlDbMetaHandler* metaHandler;
					
			if ( !metInf )
				{
				metaHandler = new ( ELeave ) CNSmlDbMetaHandler;
				}
			else
				{
				metaHandler = new ( ELeave ) CNSmlDbMetaHandler( metInf );
				}
					
			CleanupStack::PushL( metaHandler );

			if ( aCmd == KNSmlAgentAdd )
				{
				if ( !iItemOpened )
					{
					iItemOpened = ETrue;
					
					if ( !iBatchModeOn && !iMoreData )
						{
						returnCode = iDSContent.CreateItemL( iNewUid, data.Length(), metaHandler->Type(), metaHandler->Version(), aParent );
						}
					else if ( !iBatchModeOn )
					    {
						returnCode = iDSContent.CreateItemL( iNewUid, iServerLargeObjectSize, metaHandler->Type(), metaHandler->Version(), aParent );
						}
					else
						{
						iBatchBuffer->CreateNewItemL( aCmd );
						iBatchBuffer->SetGUidL( aUID );
						returnCode = iDSContent.CreateItemL( iBatchBuffer->Uid(), data.Length(), metaHandler->Type(), metaHandler->Version(), aParent );
						}
					}
					
				if ( returnCode == KErrNone )
					{
					TRAPD( error, iDSContent.WriteItemL( data ) );
					
					if ( error != KErrNone )
						{
						iMoreData = EFalse;
						iItemOpened = EFalse;
						}
						
					returnCode = error;
					}
					
				// if this is the last or the only chunk then commit it
				if ( !iMoreData )
					{
					if ( returnCode == KErrNone )
						{
						returnCode = iDSContent.CommitItemL();
						
						if ( iBatchModeOn )
							{
							iBatchBuffer->Increase();
							}
						}
						
					iItemOpened = EFalse;
					
					switch ( returnCode )
						{
						case KErrNone:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusItemAdded );

							if ( !iBatchModeOn )
								{
								iDSContent.IncreaseServerItemsAdded();
								clientModifications.iNumAdded = 1;
								
								if ( !iAtomicModeOn )
									{
									iDSContent.CreateNewMapItemL( iNewUid, aUID, 0 );
									}
								else
									{
									iDSContent.CreateNewMapItemL( iNewUid, aUID, iAtomicId );
									}
								}
							else
								{
								clientModifications.iNumAdded = 1;
								iBatchBuffer->SetStatusEntryId( statusId );
								}
							break;
							
						case KErrDiskFull:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusDeviceFull );
							iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
							break;
							
						case KErrNotSupported:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
							break;
							
						default:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusCommandFailed );
							break;
						}
					}
				}
			else  // KNSmlAgentReplace
				{
				TBool partialUpdate( EFalse );
				iNewUid = KNullDataItemId;
				
				if ( (aContent->meta) && ( aContent->meta->content ) && ( aContent->meta->contentType == SML_PCDATA_EXTENSION ) && ( aContent->meta->extension == SML_EXT_METINF ) )
					{
					SmlMetInfMetInf_t* metInfForPartial( (SmlMetInfMetInf_t*)aContent->meta->content );
						
					if ( metInfForPartial->fieldLevel )
						{
						partialUpdate = ETrue;
						}
					}

				if ( !iItemOpened )
					{
					iItemOpened = ETrue;

					TSmlDbItemUid uid;
					
					returnCode = ConvertUid( aUID, uid );
					
					if ( returnCode == KErrNone && !iMoreData )
						{
						returnCode = iDSContent.ReplaceItemL( uid, data.Length(), aParent, partialUpdate );
						}
					else if ( returnCode == KErrNone )
						{
						returnCode = iDSContent.ReplaceItemL( uid, iServerLargeObjectSize, aParent, partialUpdate );
						}

					if ( returnCode == KErrNotFound )
						{
						if ( iBatchModeOn )
							{
							iBatchBuffer->CreateNewItemL( aCmd );
							iBatchBuffer->SetGUidL( aUID );
							returnCode = iDSContent.CreateItemL( iBatchBuffer->Uid(), data.Length(), metaHandler->Type(), metaHandler->Version(), aParent );
							}
						else
							{
							if ( !iMoreData )
							    {
							     returnCode = iDSContent.CreateItemL( iNewUid, data.Length(), metaHandler->Type(), metaHandler->Version(), aParent );   
							    }
							else
							    {
							    returnCode = iDSContent.CreateItemL( iNewUid, iServerLargeObjectSize, metaHandler->Type(), metaHandler->Version(), aParent );   
							    }
							}
						}
					else
						{
						if ( iBatchModeOn )
							{
							iBatchBuffer->CreateNewItemL( aCmd );
							iBatchBuffer->SetGUidL( aUID );
							iBatchBuffer->StoreItemDataL( data );
							iBatchBuffer->SetMetaInformationL( metaHandler );
							iBatchBuffer->SetParent( aParent );
							}
						}
					}
						
				if ( returnCode == KErrNone )
					{
					TRAPD( error, iDSContent.WriteItemL( data ) );
					
					if ( error != KErrNone )
						{
						iMoreData = EFalse;
						iItemOpened = EFalse;
						}
						
					returnCode = error;
					}
					
				// if this is the last or the only chunk then commit it
				if ( !iMoreData )
					{
					if ( returnCode == KErrNone )
						{
						returnCode = iDSContent.CommitItemL();
						
						if ( iBatchModeOn )
							{
							iBatchBuffer->Increase();
							}
						}
						
					iItemOpened = EFalse;
					
					switch ( returnCode )
						{
						case KErrNone:
							if ( iNewUid == KNullDataItemId ) // item to be replaced was found
								{
								statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOK );
								
								if ( !iBatchModeOn )
									{
									iDSContent.IncreaseServerItemsChanged();
									clientModifications.iNumReplaced = 1;
									}
								else
									{
									clientModifications.iNumReplaced = 1;
									iBatchBuffer->SetStatusEntryId( statusId );
									}
								}
							else // Replace was converted to Add
								{
								statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusItemAdded );

								if ( !iBatchModeOn )
									{
									iDSContent.IncreaseServerItemsAdded();
									clientModifications.iNumAdded = 1;
									
									if ( !iAtomicModeOn )
										{
										iDSContent.CreateNewMapItemL( iNewUid, aUID, 0 );
										}
									else
										{
										iDSContent.CreateNewMapItemL( iNewUid, aUID, iAtomicId );
										}
									}
								else
									{
									clientModifications.iNumAdded = 1;
									iBatchBuffer->SetStatusEntryId( statusId );
									}
								}

							break;
							
						case KErrNotFound:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusNotFound );
							break;
							
						case KErrDiskFull:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusDeviceFull );
							iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
							break;
							
						case KErrNotSupported:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
							break;
							
						default:
							statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusCommandFailed );
							break;
						}
					}
				}
				
				CleanupStack::PopAndDestroy(); // metaHandler
			}
			
		if ( dataBuffer )
			{
			CleanupStack::PopAndDestroy(); // dataBuffer
			iRecBuf = NULL;
			}
			
		if ( statusCode != TNSmlError::ESmlStatusItemAccepted )
			{
			iNewUid = KNullDataItemId;
			}
		}
	else if ( aCmd == KNSmlAgentDelete )
		{
		TSmlDbItemUid uid;
		
		returnCode = ConvertUid( aUID, uid );
		
		if ( returnCode == KErrNone )
			{
			returnCode = iDSContent.DeleteItemL( uid, IsFlagSet( aContent->flags, SmlSftDel_f ) );
			}
			
		switch ( returnCode )
			{
			case KErrNone:
				if ( !iBatchModeOn )
					{
					iDSContent.IncreaseServerItemsDeleted();
					clientModifications.iNumDeleted = 1;
					
					if ( IsFlagSet( aContent->flags, SmlArchive_f ) )
						{
						statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusDeleteWithoutArchive );
						}
					else
						{
						statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOK );
						}
					}
				else
					{
					iBatchBuffer->CreateNewItemL( aCmd );
					iBatchBuffer->Increase();
					clientModifications.iNumDeleted = 1;	
					if ( IsFlagSet( aContent->flags, SmlArchive_f ) )
						{
						statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusDeleteWithoutArchive );
						}
					else
						{
						statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOK );
						}

					iBatchBuffer->SetStatusEntryId( statusId );
					}
				break;
				
			case KErrNotFound:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusItemIsNotDeleted );
				break;
					
			case KErrDiskFull:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusDeviceFull );
				iAgent->InterruptL( iDSContent.CurrentIndex(), TNSmlError::ESmlLowMemory, EFalse, EFalse );
				break;
					
			case KErrNotSupported:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
				break;
					
			default:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusCommandFailed );
				break;
			}
		}
	else if ( aCmd == KNSmlAgentMove )
		{
		TSmlDbItemUid uid;
		
		returnCode = ConvertUid( aUID, uid );
		
		if ( returnCode == KErrNone )
			{
			returnCode = iDSContent.MoveItemL( uid, aParent );
			}
		
		switch ( returnCode )
			{
			case KErrNone:
				if ( !iBatchModeOn )
					{
					iDSContent.IncreaseServerItemsMoved();
					statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOK );
					clientModifications.iNumMoved = 1;
					}
				else
					{
					iBatchBuffer->CreateNewItemL( aCmd );
					iBatchBuffer->Increase();
					statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOK );
					iBatchBuffer->SetStatusEntryId( statusId );
					clientModifications.iNumMoved = 1;
					}
				break;
				
			case KErrNotSupported:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusCommandFailed );
				break;
				
			default:
				statusId = StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusMoveFailed );
				break;
			}
		}
		
	if ( iDSContent.PartiallyUpdated() )
		{
		iDSContent.SetSyncStatus( CNSmlDSAgent::EServerDataPartiallyUpdated );
		}
		
	if ( iAtomicModeOn )
		{
		iStatusToServer->SetAtomicOrSequenceId( statusId, iAtomicId );

		if ( returnCode != KErrNone )
			{
			iAtomicHasFailed = ETrue;
			}
		}
	
	if ( !iMoreData )
		{
		// build the modifications struct
		TInt num( clientModifications.iNumAdded );
		num += clientModifications.iNumReplaced;
		num += clientModifications.iNumMoved;
		num += clientModifications.iNumDeleted;
	
		if ( num == 0 && !iBatchModeOn )
			{
			// If items are handled in Batch mode then it never goes here
			// Status of batch item is handled in CommitBatchL method
			clientModifications.iNumFailed = 1;
			iDSContent.IncreaseServerItemsFailed();
			}
		
		iDSObserver.OnSyncMLDataSyncModifications( iDSContent.TaskId(), clientModifications, serverModifications );
		iDSContent.SetServerItemCount( iDSContent.ServerItemCount() - 1 );
		}
		
	if ( resumeBatchMode && !iMoreData )
		{
		if ( iDSContent.SupportsOperationL( KUidSmlSupportBatch ) )
    		{
    		iBatchModeOn = ETrue;
    		if ( !iBatchBuffer )
    			{
    			iBatchBuffer = new ( ELeave ) CNSmlDSBatchBuffer;
    			}
        	else
    		    {
    			iBatchBuffer->Reset();
    	        }
    		
    		iDSContent.BeginBatchL();
    		}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsDevInfoContentTypeOkL
// Validates the content type in server's DevInfo.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsDevInfoContentTypeOkL( const SmlPcdata_t* aMeta, TInt aStatusID ) 
	{
	if ( !aMeta )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	if ( ( !aMeta->content ) || ( aMeta->contentType != SML_PCDATA_EXTENSION ) || ( aMeta->extension != SML_EXT_METINF ) )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	SmlMetInfMetInf_t* metInf( static_cast<SmlMetInfMetInf_t*>( aMeta->content ) );
	
	if ( !metInf->type )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	if ( !metInf->type->content )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	TPtr8 deviceInfoContentType( static_cast<TUint8*>( metInf->type->content ), metInf->type->length, metInf->type->length );
	TrimRightSpaceAndNull( deviceInfoContentType );

	if ( deviceInfoContentType.Length() == 0 )  
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	if ( deviceInfoContentType != KNSmlDSAgentDeviceInfoContentType )
		{
		iStatusToServer->SetStatusCodeL( aStatusID, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat );
		iAgent->Interrupt( TNSmlError::ESmlTypeInvalid, EFalse, EFalse );
		return EFalse;
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsLUIDOkLC
// Checks that LUID is present in the Target element and returns it.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsLUIDOkLC( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, HBufC8*& aLUID )
	{
	if ( !aItem->target )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		return EFalse;
		}
	
	if ( !aItem->target->locURI )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		return EFalse;
		}
	
	TPtr8 LUID( static_cast<TUint8*>( aItem->target->locURI->content ), aItem->target->locURI->length, aItem->target->locURI->length );
	TrimRightSpaceAndNull( LUID );
	aLUID = LUID.AllocLC();
	
	if ( aLUID->Length() == 0 )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		CleanupStack::PopAndDestroy(); // aLUID
		return EFalse;
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsGUIDOkLC
// Checks that GUID is present in the Source element and returns it.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsGUIDOkLC( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, HBufC8*& aGUID )
	{
	if ( !aItem->source )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		return EFalse;
		}

	if ( !aItem->source->locURI )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		return EFalse;
		}

	TPtr8 GUID( static_cast<TUint8*>( aItem->source->locURI->content ), aItem->source->locURI->length, aItem->source->locURI->length );
	TrimRightSpaceAndNull( GUID );
	aGUID = GUID.AllocLC();
	
	if ( aGUID->Length() == 0 )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusIncompleteCommand );
		CleanupStack::PopAndDestroy(); // aGUID
		return EFalse;
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsParentOkL
// Checks that GUID is present in the Parent element and returns GUID. If GUID 
// is not present then KNullDataItemId is returned.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsParentOkL( const sml_source_or_target_parent_s* aParent, TSmlDbItemUid& aGUID )
	{
	if ( !aParent )
		{
		aGUID = KNullDataItemId;
		return EFalse;
		}

	if ( !aParent->locURI )
		{
		aGUID = KNullDataItemId;
		return EFalse;
		}

	TPtr8 GUID( static_cast<TUint8*>( aParent->locURI->content ), aParent->locURI->length, aParent->locURI->length );
	TrimRightSpaceAndNull( GUID );
	
	if ( GUID == KNSmlRoot )
		{
		aGUID = KDbItemUidRoot;
		return ETrue;
		}
		
	TLex8 lex( GUID );
	
	if ( lex.Val( aGUID ) != KErrNone )
		{
		aGUID = KNullDataItemId;
		return EFalse;
		}

	return ETrue ;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsParentOkLC
// Checks that GUID is present in the Parent element and returns GUID. If GUID 
// is not present then an empty descriptor is returned.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsParentOkLC( const sml_source_or_target_parent_s* aParent, HBufC8*& aGUID )
	{
	if ( !aParent )
		{
		aGUID = KNullDesC8().AllocLC();
		return EFalse;
		}

	if ( !aParent->locURI )
		{
		aGUID = KNullDesC8().AllocLC();
		return EFalse;
		}

	TPtr8 GUID( static_cast<TUint8*>( aParent->locURI->content ), aParent->locURI->length, aParent->locURI->length );
	TrimRightSpaceAndNull( GUID );
	
	aGUID = GUID.AllocLC();

	return ETrue ;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsFormatOkL
// Validates the Format element received from the server.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsFormatOkL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem )
	{
	TPtrC8 format;

	if ( ( !IsChrFormat( aContent->meta, format ) ) || ( !IsChrFormat( aItem->meta, format ) ) )
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat, &format );
		return EFalse;
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::IsChrFormat
// Checks if the Format elements value is chr. 
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::IsChrFormat( const SmlPcdata_t* aMeta, TPtrC8& aFormat ) const
	{
	if ( aMeta )
		{
		if ( aMeta->content && aMeta->contentType == SML_PCDATA_EXTENSION && aMeta->extension == SML_EXT_METINF )
			{
			SmlMetInfMetInf_t* metInf( static_cast<SmlMetInfMetInf_t*>( aMeta->content ) );
			
			if ( metInf->format )
				{
				if ( metInf->format->content )
					{
					TPtr8 format( static_cast<TUint8*>( metInf->format->content ), metInf->format->length, metInf->format->length );
					TrimRightSpaceAndNull( format );
					
					if ( format.Length() > 0 )
						{
						if ( format != KNSmlAgentChrFormat )
							{
							aFormat.Set( format );
							return EFalse;
							}
						}
					}
				}
			}
		}
		
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSCmds:::CheckAndSetReceivedMediaTypeL
// Validates the received media (content) type.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::CheckAndSetReceivedMediaTypeL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem )
	{
	if ( aCmd == KNSmlAgentDelete )
		{
		return ETrue;
		}

	TPtr8 mediaType( NULL, 0, 0 );
	TBool found( EFalse );
	
	// first check item level media type
	if ( MediaTypeSetOkL( aItem->meta, found, mediaType, aCmd ) )
		{
		return ETrue;
		}

	// check under command if item level media type was not found
	if ( !found )
		{
		if ( MediaTypeSetOkL( aContent->meta, found, mediaType,aCmd ) )
			{
			return ETrue;
			}
		}
		
	if ( found )
		{
		//mediatype found but does not suppport hierarchical sync (move)
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusOptFeatureNotSupported );
		}
	else
		{
		StatusDataToGenericCommandL( aCmd, aContent, aItem, TNSmlError::ESmlStatusUnsupportedMediaTypeOrFormat, &mediaType );
		}
		
	return EFalse;
	}			

// -----------------------------------------------------------------------------
// CNSmlDSCmds:::MediaTypeSetOkL
// If the media (content) type is OK its devcaps are set to the corresponding 
// DataStore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSCmds::MediaTypeSetOkL( const SmlPcdata_t* aMeta, TBool& aFound, TPtr8& aMediaType,const TDesC8& aCmd ) const
	{
	aFound = EFalse;
	
	if ( aMeta )
		{
		if ( aMeta->content && aMeta->contentType == SML_PCDATA_EXTENSION && aMeta->extension == SML_EXT_METINF )
			{
			SmlMetInfMetInf_t* metInf( (SmlMetInfMetInf_t*)aMeta->content );
			
			if ( metInf->type )
				{
				aMediaType.Set( static_cast<TUint8*>( metInf->type->content ), metInf->type->length, metInf->type->length );
				TrimRightSpaceAndNull( aMediaType );
				
				if ( aMediaType.Length() > 0 )
					{
					aFound = ETrue;
		
					if ( iDSContent.SetSavedPartnerDevCapsL() == KErrNone )
						{
						TInt status( KErrNone );
						TRAPD( error, status = iDSContent.IsHierarchicalSyncSupportedL( aMediaType ) );
						
						if ( error != KErrNone )
						    {
						    return EFalse;    
						    }
						    
						if ( aCmd == KNSmlAgentMove && !status )
							{
							return EFalse;
							}
																		
						return ETrue;
						}
					}
				}
			}	
		}
		
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::smlDeviceInfoL
// Callback function used by the DevInfo parser.
// -----------------------------------------------------------------------------
//
Ret_t CNSmlDSCmds::smlDeviceInfoL( SmlDevInfDevInfPtr_t aContent )
	{
	ProcessDeviceInfoL( *aContent );	
	return 0;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::AlertDataLC()
// 
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDSCmds::AlertDataLC( const SmlItemList_t* aItemList ) const
	{
	HBufC* unicodeData( NULL );
	
	if ( aItemList )
		{
		if ( aItemList->item->data->content )
			{
			TPtr8 data( static_cast<TUint8*>( aItemList->item->data->content ), aItemList->item->data->length, aItemList->item->data->length );
			TrimRightSpaceAndNull( data );
			NSmlUnicodeConverter::HBufC16InUnicodeLC( data, unicodeData ); 
			}
		}
		
	if ( !unicodeData )
		{
		unicodeData = HBufC::NewLC( 0 );
		}
		
	return unicodeData;
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::AppendCTCapsL
// 
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::AppendCTCapsL( CNSmlDbCaps& aDbCaps, const SmlDevInfDatastore_t* aDataStore, SmlDevInfCtCapList_t* aCtCapList ) const
	{
	if ( aDataStore->sourceref )
		{
		aDbCaps.SetSourceRefL( aDataStore->sourceref->Data() );
		}

	if ( aDataStore->displayname )
		{
		aDbCaps.SetDisplayNameL( aDataStore->displayname->Data() );
		}

	if ( aDataStore->maxguidsize )
		{
		aDbCaps.SetMaxGuidSizeL( aDataStore->maxguidsize->Data() );
		}

	if ( aDataStore->rxpref )
		{
		aDbCaps.SetRxPrefL( aDataStore->rxpref->cttype->Data(), aDataStore->rxpref->verct->Data() );
		}

	if ( aDataStore->rx )
		{
		SmlDevInfXmitListPtr_t rx( aDataStore->rx );

		while ( rx )
			{
			aDbCaps.AddRxL( rx->data->cttype->Data(), rx->data->verct->Data() );
			rx = rx->next;
			}
		}

	if ( aDataStore->txpref )
		{
		aDbCaps.SetTxPrefL( aDataStore->txpref->cttype->Data(), aDataStore->txpref->verct->Data() );
		}

	if ( aDataStore->tx )
		{
		SmlDevInfXmitListPtr_t tx( aDataStore->tx );

		while ( tx )
			{
			aDbCaps.AddTxL( tx->data->cttype->Data(), tx->data->verct->Data() );
			tx = tx->next;
			}
		}

	if ( aDataStore->supportHierarchicalSync )
		{
		aDbCaps.SetSupportHierarchicalSyncL();
		}

	if ( aDataStore->synccap )
		{
		SmlPcdataListPtr_t syncType( aDataStore->synccap->synctype );

		while ( syncType )
			{
			TLex8 lex( syncType->data->Data() );
			TInt type;
			lex.Val( type );
			aDbCaps.SetSyncType( (CNSmlDbCaps::ENSmlSyncTypes)type );
			syncType = syncType->next;
			}
		}

	TInt pc( 0 );
	SmlDevInfCtCapListPtr_t ctcap( NULL );

	if ( iVersionPublicId == KNSmlSyncMLPublicId )
		{
		ctcap = aCtCapList;
		}
	else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
		{
		ctcap = aDataStore->ctcap;
		} 

	while ( ctcap )
		{
		if ( ctcap->data )
			{
			CNSmlCtCap* cap = aDbCaps.AddCtCapLC();
			++pc;
			
			if ( ctcap->data->cttype )
				{
				cap->SetCtTypeL( ctcap->data->cttype->Data() );
				}
				
			if ( ctcap->data->verct )
				{
				cap->SetVerCtL( ctcap->data->verct->Data() );
				}
				
			if ( ctcap->data->fieldlevel )
				{
				cap->SetFieldLevelL();
				}
				
			if ( iVersionPublicId == KNSmlSyncMLPublicId )
				{
				if( ctcap->data->prop )
					{
					SmlDevInfCTDataPropListPtr_t prop( ctcap->data->prop );
					
					while ( prop )
						{
						if( prop->data )
							{
							CNSmlDevInfProp* pr( cap->AddDevInfPropLC( prop->data->prop->name->Data() ) );
							++pc;
							
							if ( prop->data->prop->datatype )
								{
								pr->SetDataTypeL( prop->data->prop->datatype->Data() );
								}
								
							if ( prop->data->prop->dname )
								{
								pr->SetDisplayNameL( prop->data->prop->dname->Data() );
								}
								
							if ( prop->data->prop->size )
								{
								pr->SetMaxSizeL( prop->data->prop->size->Data() );
								}

							if ( prop->data->prop->valenum )
								{
								SmlPcdataListPtr_t propValEnum( prop->data->prop->valenum );
								
								while ( propValEnum )
									{
									if ( propValEnum->data )
										{
										pr->AddValEnumL( propValEnum->data->Data() );
										}
										
									propValEnum = propValEnum->next;
									}
								}					
		
							if ( prop->data->param )
								{
								SmlDevInfCTDataListPtr_t param( prop->data->param );
								
								while ( param )
									{
									if ( param->data )
										{
										CNSmlPropParam* par( pr->AddParamLC( param->data->name->Data() ) );
										++pc;
										
										if ( param->data->datatype )
											{
											par->SetDataTypeL( param->data->datatype->Data() );
											}
											
										if ( param->data->dname )
											{
											par->SetDisplayNameL( param->data->dname->Data() );
											}
											
										if ( param->data->valenum )
											{
											SmlPcdataListPtr_t valenum( param->data->valenum );
											
											while (valenum)
												{
												if ( valenum->data )
													{
													par->AddValEnumL( valenum->data->Data() );
													}
													
												valenum = valenum->next;
												}
											}
										}
										
									param = param->next;
									}
								}
							}
							
						prop = prop->next;
						}
					} 
				}
			else if ( iVersionPublicId == KNSmlSyncMLPublicId12 )
				{
				if ( ctcap->data->property )
					{
					SmlDevInfPropertyListPtr_t prop( ctcap->data->property );
					
					while ( prop )
						{
						if ( prop->data )
							{
							
							if ( prop->data->propname == NULL )
							    {
							    User::Leave( KErrArgument );
							    }
							    
							CNSmlDevInfProp* pr( cap->AddDevInfPropLC(prop->data->propname->Data() ) );
							++pc;
							
							if ( prop->data->datatype )
								{
								pr->SetDataTypeL( prop->data->datatype->Data() );
								}
								
							if ( prop->data->displayname )
								{
								pr->SetDisplayNameL( prop->data->displayname->Data() );
								}
								
							if ( prop->data->maxoccur )
								{
								pr->SetMaxOccurL( prop->data->maxoccur->Data() );
								}
								
							if ( prop->data->maxsize )
								{
								pr->SetMaxSizeL( prop->data->maxsize->Data() );
								}
								
							if ( prop->data->notruncate )
								{
								pr->SetNoTruncateL();
								}
						
							if ( prop->data->propparam )
								{
								SmlDevInfPropParamListPtr_t param( prop->data->propparam );
								
								while ( param )
									{
									if ( param->data )
										{
										CNSmlPropParam* par( pr->AddParamLC( param->data->paramname->Data() ) );
										++pc;
										
										if ( param->data->datatype )
											{
											par->SetDataTypeL( param->data->datatype->Data() );
											}
											
										if ( param->data->displayname )
											{
											par->SetDisplayNameL( param->data->displayname->Data() );
											}
											
										if ( param->data->valenum )
											{
											SmlPcdataListPtr_t valenum( param->data->valenum );
											
											while ( valenum )
												{
												if ( valenum->data )
													{
													par->AddValEnumL( valenum->data->Data() );
													}
												valenum = valenum->next;
												}
											}
										}
										
									param = param->next;
									}
								}
							}
							
						prop = prop->next;
						}
					} 
				}
			}
			
		ctcap = ctcap->next;
		}

	CleanupStack::PopAndDestroy( pc );
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::SwitchAlertCodeL
// Does mapping between OMA DS 1.1 and 1.2 alert codes.
// -----------------------------------------------------------------------------
//
void CNSmlDSCmds::SwitchAlertCode( TDes8& aAlertCode ) 
	{
	if ( aAlertCode == KNSmlDSTwoWayByServer )
		{
		aAlertCode = KNSmlDSTwoWay;
		}
	else if ( aAlertCode == KNSmlDSOneWayFromClientByServer )
		{
		aAlertCode = KNSmlDSOneWayFromClient;
		}
	else if ( aAlertCode == KNSmlDSRefreshFromClientByServer )
		{
		aAlertCode = KNSmlDSRefreshFromClient;
		}
	else if ( aAlertCode == KNSmlDSOneWayFromServerByServer )
		{
		aAlertCode = KNSmlDSOneWayFromServer;
		}
	else if ( aAlertCode == KNSmlDSRefreshFromServerByServer )
		{
		aAlertCode = KNSmlDSRefreshFromServer;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSCmds::ConvertUid
// Converts UID from literal to numeric presentation.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSCmds::ConvertUid( const TDesC8& aLiteralUid, TSmlDbItemUid& aNumericUid )
	{
	TLex8 lexer( aLiteralUid );
	
	aNumericUid = KNullDataItemId;

	return lexer.Val( aNumericUid );
	}

//-----------------------------------------------------------------------------
// CNSmlDSCmds::StoreSyncType
// Checks if received Alert Code is a sync type and tries to convert
// it to Sync Type (TSmlSyncType).
//-----------------------------------------------------------------------------
//
void CNSmlDSCmds::StoreSyncType( const TDes8& aAlertCode )
    {
    TInt syncType = KErrNotFound;

    if ( aAlertCode == KNSmlDSTwoWay )
        {
        syncType = ESmlTwoWay;
        }
    else if ( aAlertCode == KNSmlDSOneWayFromServer )
        {
        syncType = ESmlOneWayFromServer;
        }
    else if ( aAlertCode == KNSmlDSOneWayFromClient )
        {
        syncType = ESmlOneWayFromClient;
        }
    else if ( aAlertCode == KNSmlDSSlowSync )
        {
        syncType = ESmlSlowSync;
        }
    else if ( aAlertCode == KNSmlDSRefreshFromServer )
        {
        syncType = ESmlRefreshFromServer;
        }
    else if ( aAlertCode == KNSmlDSRefreshFromClient )
        {
        syncType = ESmlRefreshFromClient;
        }

    if ( syncType != KErrNotFound )
        {
        CRepository* rep = NULL;
        TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys ) );
        if ( err == KErrNone )
            {
            rep->Set( KNsmlOpDsSyncType, syncType );
            delete rep;
            }
        }
    }

// ----------------------------------------------------------------------------
// CNSmlDSCmds::InsertOperatorExtensionDevInfFieldsL
// Adds operator specific extension fields <XNam> and <XVal> to Device info
// ----------------------------------------------------------------------------
//
void CNSmlDSCmds::InsertOperatorExtensionDevInfFieldsL( 
    SmlDevInfDevInfPtr_t& aDevInf )
    {
    // Instantiate Extension Data container ECom plugin 
    CNSmlDevInfExtDataContainerPlugin* extensionPlugin = NULL;
    TRAPD( err, extensionPlugin = CNSmlDevInfExtDataContainerPlugin::NewL() );
    if( err == KErrNone )
        {
        CleanupStack::PushL( extensionPlugin );

        if( extensionPlugin->GetExtensionCountL() > 0 )
            {
            // Create a list for extensions
            SmlDevInfExtListPtr_t extList = new ( ELeave ) SmlDevInfExtList_t;
            CleanupStack::PushL( extList );
            extList->data = NULL;
            extList->next = NULL;

            for( TInt i = 0; i < extensionPlugin->GetExtensionCountL(); i++ )
                {
                // Create new <Ext> element and insert it to extension list
                SmlDevInfExtPtr_t extElement = new ( ELeave ) SmlDevInfExt_t;
                CleanupStack::PushL( extElement );

                // Handle <XNam>
                PcdataNewL( extElement->xnam, extensionPlugin->GetExtNameL( i ) );

                // Handle <XVal>
                if( extensionPlugin->GetExtValueCountL( i ) > 0 )
                    {
                    SmlPcdataListPtr_t xValList = new ( ELeave ) SmlPcdataList_t;
                    CleanupStack::PushL( xValList );
                    xValList->data = NULL;
                    xValList->next = NULL;

                    for( TInt j = 0; j < extensionPlugin->GetExtValueCountL( i ); j++ )
                        {
                        AppendToXValListL( xValList, 
                            extensionPlugin->GetExtValueL( i, j ) );
                        }
                    extElement->xval = xValList;
                    CleanupStack::Pop( xValList );
                    }
                else
                    {
                    extElement->xval = NULL;
                    }
                AppendToExtensionListL( extList, extElement );
                CleanupStack::Pop( extElement );
                }

            // Add extensions as a part of DevInfo structure
            aDevInf->ext = extList;
            CleanupStack::Pop( extList );
            }

        // Do the cleanup
        CleanupStack::PopAndDestroy( extensionPlugin );
        REComSession::FinalClose();
        }
    }

// ----------------------------------------------------------------------------
// CNSmlDSCmds::AppendToExtensionListL
// ----------------------------------------------------------------------------
//
void CNSmlDSCmds::AppendToExtensionListL( SmlDevInfExtListPtr_t aExtList, 
    SmlDevInfExtPtr_t aExtItem )
    {
    if( !aExtList->data )
        {
        // This is the first item to be added to the list
        aExtList->data = aExtItem;
        aExtList->next = NULL;
        }
    else
        {
        // List is not empty
        SmlDevInfExtListPtr_t newExtListItem = new ( ELeave ) SmlDevInfExtList_t;
        newExtListItem->data = aExtItem;
        newExtListItem->next = NULL;
    
        // Search the end of the extension list & add new item
        SmlDevInfExtListPtr_t tmp = aExtList;
        while( tmp->next )
            {
            tmp = tmp->next;
            }
        tmp->next = newExtListItem;
        }
    }

// ----------------------------------------------------------------------------
// CNSmlDSCmds::AppendToXValListL
// ----------------------------------------------------------------------------
//
void CNSmlDSCmds::AppendToXValListL( SmlPcdataListPtr_t aXValList,
    const TDesC8& aXVal )
    {
    if( !aXValList->data )
        {
        // This is the first item to be added to the list
        PcdataNewL( aXValList->data, aXVal );
        aXValList->next = NULL;
        }
    else
        {
        // List is not empty
        SmlPcdataListPtr_t newXValItem = new ( ELeave ) SmlPcdataList_t;
        CleanupStack::PushL( newXValItem );
        PcdataNewL( newXValItem->data, aXVal );
        CleanupStack::Pop( newXValItem );
        newXValItem->next = NULL;
    
        // Search the end of the extension list & add new item
        SmlPcdataListPtr_t tmp = aXValList;
        while( tmp->next )
            {
            tmp = tmp->next;
            }
        tmp->next = newXValItem;
        }
    }

// End of File
