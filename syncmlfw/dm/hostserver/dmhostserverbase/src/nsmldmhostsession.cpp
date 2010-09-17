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
* Description:  A common session class for all the specialized 
*				 DM Host Servers.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
//
#include <s32mem.h>
#include <s32strm.h>
#include <implementationinformation.h>
#include <ecom.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <smldmadapter.h>

// FOTA
#include <nsmldmconst.h>
// FOTA end
#include <nsmldmuri.h>
#include <e32property.h>
#include "nsmldminternalpskeys.h"
#include "nsmldmclientandserver.h"
#include "nsmldmhostserver.h"
#include "nsmldmhostsession.h"
#include "nsmldmcmdarghandler.h"
#include "nsmldmimpluids.h"
#include <featmgr.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
//
// Granularity of the lists; mapping info and prevUriSegList
const TInt KNSmlDmGranularity = 8;

// This means the dm adapter doesn't support streaming
const TInt KNSmlDmNotSupported = -1;

// During the fetch link serving the callbacks are handled differently
const TInt KNSmlDmNoRef = -1; 

// A safety margin (in bytes) which is left unreserved in the chunk
const TInt KNSmlDmChunkMarginal = 128;

// A cross-reference to the interner adapter is recognized using these strings
_LIT8 ( KNSmlDmApAdapterURI1, "AP" );
_LIT8 ( KNSmlDmApAdapterURI2, "./AP" );

// Identifies the server which owns the internet DM plug-in adapter
const TInt KNSmlDmApAdapterServerId = 1;

// Tells to the session class that it is acting as a fetch link server
const TInt KNSmlDmFetchLinkServerId = 99;

// The constant part of the result item size in bytes: 	
//  iResultRef => 2 bytes
//  iResult->Size => 4 bytes
//  iMimeType->Length => 2 bytes
const TInt KNSmlDmConstItemSize = 8;

// FOTA
const TInt KNSmlDmLastPackage = 1;
// FOTA end


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CNSmlDMHostSession
//	C++ constructor.
//	@param aServer. The server instance, owner of session.
// ------------------------------------------------------------------------------------------------
//
CNSmlDMHostSession::CNSmlDMHostSession ( CNSmlDMHostServer& aServer ) : 
    iServer(aServer)
	{
	_DBG_FILE("CNSmlDMHostSession::CNSmlDMHostSession(): begin");
	iServer.IncSessionCount();
	_DBG_FILE("CNSmlDMHostSession::CNSmlDMHostSession(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::~CNSmlDMHostSession
//  C++ destructor.
// ------------------------------------------------------------------------------------------------
//
CNSmlDMHostSession::~CNSmlDMHostSession()
	{	
	_DBG_FILE("CNSmlDMHostSession::~CNSmlDMHostSession(): begin");
	if ( iCbSessConnected )
		{
		iCbSession.Close();		
		}
	iServer.DecSessionCount();
	delete iMgmtTree;
	if ( iOwnId != KNSmlDmFetchLinkServerId )
		{
		iChunk.Close(); 		
		}
	
	for ( TInt i=0; i<iResultList.Count(); i++ )
		{
		delete iResultList[i];
		}
	iResultList.Close();
	iStatusCodeList.Close();
	iCallbackList.Close();
	
	if ( iLargeWriteStream.streamPtr )
		{
		iLargeWriteStream.streamPtr->Close();		
		}
	if ( iResultReadStream )
		{
		iResultReadStream->Close();		
		}
	
	// delete all loaded adapters
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		delete iAdapters[i].adapterPtr;
		}
	iAdapters.Close();
	REComSession::FinalClose();	
	if ( iMapUris )
		{
		iMapUris->ResetAndDestroy();
		delete iMapUris;	
		}
	if ( iMapLuids )
		{
		iMapLuids->ResetAndDestroy();
		delete iMapLuids;
		}
	delete iSemaphoreName;
	delete iResultOtherAdapter;
	delete iLargeObject;
	DeletePrevParams();
	FeatureManager::UnInitializeLib();
	_DBG_FILE("CNSmlDMHostSession::~CNSmlDMHostSession(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::NewL
//	Creates a new session object.
//	@param aServer. Reference to server is needed in order to increase and decrease session counts.
//	@return CNSmlDMHostSession*. New instance of this class.
// ------------------------------------------------------------------------------------------------
//
CNSmlDMHostSession* CNSmlDMHostSession::NewL ( CNSmlDMHostServer& aServer )
	{
	_DBG_FILE("CNSmlDMHostSession::NewL(): begin");
	CNSmlDMHostSession* self= new (ELeave) CNSmlDMHostSession ( aServer );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	_DBG_FILE("CNSmlDMHostSession::NewL(): end");
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ConstructL
//	Symbian 2nd phase constructor
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ConstructL()
	{
	_DBG_FILE("CNSmlDMHostSession::ConstructL(): begin");
	iMgmtTree = CNSmlDmMgmtTree::NewL();
	iCommitStatus.iStreamingOngoing = EFalse;
	iCommitStatus.iStreamCommitted = EFalse;
	FeatureManager::InitializeLibL();
	_DBG_FILE("CNSmlDMHostSession::ConstructL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ServiceL
//	Handles the servicing of client requests.
//	Entry point for arriving messages.
//	@param aMessage. An object which encapsulates a client request.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ServiceL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::ServiceL(): begin");
	if(!aMessage.HasCapability(ECapabilityDiskAdmin))
	    {
	    _DBG_FILE("CNSmlDMHostSession::ServiceL(): capability fail");
	    aMessage.Complete( KErrPermissionDenied );
	    return;
	    }
	TRAPD ( err, DispatchMessageL ( aMessage ) );
	aMessage.Complete( err );
	_DBG_FILE("CNSmlDMHostSession::ServiceL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::DispatchMessageL
//	Extracts the operation code from the message. 
//	Based on that calls the right private function.
//	@param aMessage. An object which encapsulates a client request.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::DispatchMessageL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::DispatchMessageL(): begin");
	switch( aMessage.Function() )
        {
		case ENSmlDMCloseSession:
        	CloseSessionL();
        	break;
		case ENSmlDMGetDDF:
        	GetDDFStructureL ( aMessage );
        	break;
        case ENSmlDMUpdateLeaf:
        	UpdateLeafOrExecuteL ( aMessage, ENSmlDMUpdateLeaf );
        	break;
        case ENSmlDMExecute:
        	UpdateLeafOrExecuteL ( aMessage, ENSmlDMExecute );
        	break;
        case ENSmlDMUpdateLeafLarge:
        	UpdateLeafOrExecuteLargeL ( aMessage, ENSmlDMUpdateLeafLarge );
        	break;
        case ENSmlDMExecuteLarge:
        	UpdateLeafOrExecuteLargeL ( aMessage, ENSmlDMExecuteLarge );
        	break;        	
        case ENSmlDMAddNode:
        	AddCopyOrDeleteL ( aMessage, ENSmlDMAddNode );
        	break;
        case ENSmlDMCopy:
        	AddCopyOrDeleteL ( aMessage, ENSmlDMCopy );
        	break;
        case ENSmlDMDelete:
        	AddCopyOrDeleteL ( aMessage, ENSmlDMDelete );
        	break;        
        case ENSmlDMStartAtomic:
        	StartAtomicL ( aMessage );
        	break;
        case ENSmlDMCommitAtomic:
        	CommitAtomicL ( aMessage );
        	break;
        case ENSmlDMRollbackAtomic:
        	RollbackAtomicL ( aMessage );
        	break;
        case ENSmlDMCompleteCommands:
        	CompleteCommandsL ( aMessage );
        	break;
        case ENSmlDMFetchLeaf:
        	FetchLeafChildUrisOrSizeL ( aMessage, ENSmlDMFetchLeaf );
        	break;
        case ENSmlDMFetchLink:
        	FetchLinkL ( aMessage );
        	break;
        case ENSmlDMFetchLeafSize:
        	FetchLeafChildUrisOrSizeL ( aMessage, ENSmlDMFetchLeafSize );
        	break;
        case ENSmlDMChildUriList:
        	FetchLeafChildUrisOrSizeL ( aMessage, ENSmlDMChildUriList );
        	break;
    	case ENSmlDMChunkHandle:
        	SetChunkHandleL ( aMessage ); 
        	break;
    	case ENSmlDMGetMappings:
        	GetMappingsL ( aMessage ); 
        	break;
    	case ENSmlDMGetStatuses:
        	GetStatusesL ( aMessage ); 
        	break;
    	case ENSmlDMGetResults:
        	GetResultsL ( aMessage ); 
        	break;
    	case ENSmlDMGetMore:
        	GetResultsL ( aMessage ); 
        	break;
        // FOTA
    	case ENSmlDMGenericAlertsSent:
    	if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
    	{
    	_DBG_FILE(" CNSmlDMHostSession::DispatchMessageL(): calling new MarkGenAlertsSentL(message)");
        	MarkGenAlertsSentL(aMessage); 	
    	}
    	else
    	{
    	_DBG_FILE(" CNSmlDMHostSession::DispatchMessageL(): calling old  MarkGenAlertsSentL ");
        MarkGenAlertsSentL();	
    	}
        	break;
        // FOTA end
		default:
			PanicClient ( aMessage, KErrNotSupported );
        }
	_DBG_FILE("CNSmlDMHostSession::DispatchMessageL(): end");
	}


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::PanicClient
//	Panics the client
//	@param aMessage. Message from client to panic.
//	@param aReason. Reason code.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::PanicClient ( const RMessage2& aMessage, TInt aReason ) const
	{
	_DBG_FILE("CNSmlDMHostSession::PanicClient(): begin");
	aMessage.Panic ( _L( "CNSmlDMHostSession" ), aReason );
	_DBG_FILE("CNSmlDMHostSession::PanicClient(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CloseSessionL
//	Empty function.
//	Kept here for possible future needs. 
//	Freeing of resources is done in the destructor.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CloseSessionL() const
	{
	_DBG_FILE("CNSmlDMHostSession::CloseSessionL(): begin");
	_DBG_FILE("CNSmlDMHostSession::CloseSessionL(): end");
	}

void CNSmlDMHostSession::MarkGenAlertsSentL(const RMessage2& aMessage)
	{

	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): begin");
	
	HBufC8* tempPtr = HBufC8::NewLC(aMessage.GetDesLength(0));
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): crating hBufC for length of URI ");
	TPtr8 uRI = tempPtr->Des();
   _DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): TPrt got from HBufC ");
	aMessage.ReadL(0, uRI, 0);
	TUint32 adId(0);
	TNSmlDmDDFFormat nodeType;
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): Before calling adapterIdFromUriL  ");
	TBool adapterFound = AdapterIdFromUriL ( uRI, adId, nodeType );
	
	_LIT8(KSCOMONODE, "SCOMO");
	
	// For AM Adapter check for Node "SCOMO" if present in Uri then use Adapter Implementation UID
	
	if(uRI.Compare(KSCOMONODE) == KErrNone)
	{
		adId = KNSmlDMAMAdapterImplUid;
		adapterFound = ETrue;
	}
	
	
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): After calling adapterIdFromUriL ");
	
	if ( adapterFound )
		{
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): adapter is found ! ");	  
	
        CSmlDmAdapter* adapter = NULL;
        adapterFound = EFalse;
	    for ( TInt i(0); i<iAdapters.Count(); i++ )
		{
		
		if ( iAdapters[i].adapterId == adId )
			{
			adapterFound = ETrue;
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, adId );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );
					}
				}
			// Call overloaded function of the  adapter to request
			// the  adapter to mark Generic Alerts sent.
		
			TInt resetGenAlerts ( KNSmlDMResetGenAlerts );
			iAdapters[i].adapterPtr->StreamingSupport ( resetGenAlerts );
		
			}
		}
	}
	CleanupStack::PopAndDestroy(); //tempPtr
	if ( !adapterFound )
	    {
	    _DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(RMessage2): adapter NOT :-( found ! ");	  
		User::Leave ( KErrNotFound );	    
	    }
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(): end");
	}

// FOTA
// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::MarkGenAlertsSentL
//	This function is called only for the FOTA adapter.
//	Makes a request for the FOTA adapter to mark generic alerts sent.
//	In other words, the written generic alerts are issued without
//	errors to the remote server.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::MarkGenAlertsSentL()
	{
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(): begin");
	CSmlDmAdapter* adapter = NULL;
	TBool adapterFound ( EFalse );
	for ( TInt i(0); i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == KNSmlDMFotaAdapterImplUid )
			{
			adapterFound = ETrue;
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, KNSmlDMFotaAdapterImplUid );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );
					}
				}
			// Call overloaded function of the FOTA adapter to request
			// the FOTA adapter to mark Generic Alerts sent.
			TInt resetGenAlerts ( KNSmlDMResetGenAlerts );
			iAdapters[i].adapterPtr->StreamingSupport ( resetGenAlerts );
			}
		else if ( iAdapters[i].adapterId == KNSmlDMAMAdapterImplUid )
			{
			adapterFound = ETrue;
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, KNSmlDMAMAdapterImplUid );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );
					}
				}
			// Call overloaded function of the AM adapter to request
			// the AM adapter to mark Generic Alerts sent.
			TInt resetGenAlerts ( KNSmlDMResetGenAlerts );
			iAdapters[i].adapterPtr->StreamingSupport ( resetGenAlerts );
			}
		
			
		}
	if ( !adapterFound )
	    {
		User::Leave ( KErrNotFound );	    
	    }
	_DBG_FILE("CNSmlDMHostSession::MarkGenAlertsSentL(): end");
	}
// FOTA end

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::GetDDFStructureL
//	Gets DDF structure from the dm adapters.
//	aMessage parameter contains a list of adapters which are already loaded.
//	Writes the combined DDF structure to the chunk in the WBXML format.
//	Returns also a checksum of the DDF structure to the client. 
//	@param aMessage. A list of already loaded adapters, and place for checksum.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::GetDDFStructureL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::GetDDFStructureL(): begin");
	RImplInfoPtrArray allDmAdapters;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( CImplementationInformation, &allDmAdapters ) );

	RArray<TUint32> dontLoadThese;
	CleanupClosePushL ( dontLoadThese );
	ReadUidsL ( aMessage, dontLoadThese );
		
	TUid ifUid = { KSmlDMInterfaceUid };
	REComSession::ListImplementationsL ( ifUid, allDmAdapters );
	RemoveAlreadyLoadedL ( allDmAdapters, dontLoadThese );
	CleanupStack::PopAndDestroy();  // dontLoadThese

	RPointerArray<HBufC8> versions;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( HBufC8, &versions ) );
	RArray<TInt32> uids;
	CleanupClosePushL ( uids );
	
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i=0; i<allDmAdapters.Count(); i++ )
		{
		TRAPD ( err, ( adapter = CSmlDmAdapter::NewL ( allDmAdapters[i]->ImplementationUid(), *this ) ) );
		if ( err == KErrNone && adapter )
			{
			CleanupStack::PushL(adapter);
			AskInfoFromAdapterL ( adapter, versions, uids, allDmAdapters[i]->ImplementationUid().iUid );		
			CleanupStack::PopAndDestroy(); //adapter
			}
		} 
	TUint16 crc = CalcCheckSumL ( versions, uids );
	CleanupStack::PopAndDestroy(3); // uids, versions, allDmAdapters
	WriteTreeToChunkL();
	aMessage.WriteL ( 2, TPckgBuf<TInt> (crc) );
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );	
	_DBG_FILE("CNSmlDMHostSession::GetDDFStructureL(): end");
	}


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ReadUidsL
//	Auxiliary function called by GetDDFStructureL.
//	Reads the UIDs of the loaded adapters from the message, 
//	and stores those to internal list. 
//	@param aMessage. An object which encapsulates a client request.
//	@param aUids. A reference parameter, a place for extracted list of UIDs.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ReadUidsL ( const RMessage2& aMessage, RArray<TUint32>& aUids ) const
	{
	_DBG_FILE("CNSmlDMHostSession::ReadUidsL(): begin");
	HBufC8* buffer = HBufC8::NewLC ( aMessage.GetDesLengthL(0) );
    TPtr8 bufPtr = buffer->Des();
    aMessage.ReadL ( 0, bufPtr, 0 );
    
    RDesReadStream readStream;
	readStream.Open ( *buffer );
	CleanupClosePushL ( readStream );
	TUint8 nOfuids = readStream.ReadUint8L();
    	
	for ( TInt i=0; i<nOfuids; i++ )
		{
		TUint32 uidTemp = readStream.ReadUint32L();
		aUids.Append ( uidTemp );		
		}
	
	CleanupStack::PopAndDestroy(2); // readStream, buffer
	_DBG_FILE("CNSmlDMHostSession::ReadUidsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::RemoveAlreadyLoadedL
//	Auxiliary function called by GetDDFStructureL.
//	Gets a list of all the potential dm adapters.
//	Removes from it those which are alreay loaded.  
//	@param aAllAds. A reference parameter, after this function call 
//	                contains the acceptable adapters.
//	@param aUids. A list of adapters which are already loaded.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::RemoveAlreadyLoadedL ( RImplInfoPtrArray& aAllAds, RArray<TUint32>& aUids ) const
	{
	_DBG_FILE("CNSmlDMHostSession::RemoveAlreadyLoadedL(): begin");
	for ( TInt i=0; i<aAllAds.Count(); i++ )
		{
		for ( TInt u=0; u<aUids.Count(); u++ )
			{
			if ( aUids[u] == (TUint32)aAllAds[i]->ImplementationUid().iUid )
				{
				delete aAllAds[i];
				aAllAds[i] = NULL;
				aAllAds.Remove(i);
				i--;
				break;								
				}
			}
		}
	_DBG_FILE("CNSmlDMHostSession::RemoveAlreadyLoadedL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::AskInfoFromAdapterL
//	Auxiliary function called by GetDDFStructureL.
//	Asks initial information from the loaded dm adapter.
//	E.g. whether the adapter supports streaming or not.
//	@param aAdapter. A pointer to the just loaded dm plug-in adapter. 
//	@param aVersions. A list of DDF versions collected from the adapters. 
//	@param aUids. A list of adapter UIDs. Note that aVersions and aUids
//	              are always ordered to ascending order in order to prevent
//	              a checksum to change value if ECom framework gives a list
//	    	      of potential plug-in adapters in different order.
//	@param aAdapterId. Identifier of the dm plug-in adapter.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::AskInfoFromAdapterL ( CSmlDmAdapter* aAdapter, RPointerArray<HBufC8>& aVersions, 
												RArray<TInt32>& aUids, TInt32 aAdapterId )
	{
	_DBG_FILE("CNSmlDMHostSession::AskInfoFromAdapterL(): begin");
	TNSmlDmAdapter newElement;
	newElement.adapterPtr = NULL;
	newElement.adapterId = aAdapterId;
	newElement.startAtomicCalled = EFalse;
	newElement.commandsCalled = EFalse;
	TInt limit ( KNSmlDmNotSupported );
	newElement.streamingLimit = ( aAdapter->StreamingSupport ( limit ) ) ? limit : KNSmlDmNotSupported;	
	iAdapters.Append ( newElement );

	CBufBase* version = CBufFlat::NewL(16);
	CleanupStack::PushL ( version );
	aAdapter->DDFVersionL ( *version );
	HBufC8* versionHBufC = HBufC8::NewLC ( version->Size() );
	*versionHBufC = version->Ptr(0);
	TInt j=0;
	// Sort uids and versions to ascending order in arrays
	while ( j<aUids.Count() &&  aAdapterId < aUids[j] )  
		{
		j++;
		}
	aUids.InsertL ( aAdapterId, j );
	aVersions.InsertL ( versionHBufC, j );
	CleanupStack::Pop(); //versionHBufC
	CleanupStack::PopAndDestroy(); //version
		
	aAdapter->DDFStructureL ( *iMgmtTree );
	iMgmtTree->SetAdapterUid ( aAdapterId );

	_DBG_FILE("CNSmlDMHostSession::AskInfoFromAdapterL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::WriteTreeToChunkL
//	Auxiliary function called by GetDDFStructureL.
//	Writes a combined DDF stucture to the chunk using DM Utils services.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::WriteTreeToChunkL()
	{
	_DBG_FILE("CNSmlDMHostSession::WriteTreeToChunkL(): begin");
	CBufBase* buffer = CBufFlat::NewL ( 64 );
	CleanupStack::PushL ( buffer );

	RBufWriteStream writeStream ( *buffer );
	CleanupClosePushL ( writeStream );
	iMgmtTree->GenerateWBXMLL ( writeStream );
	CleanupStack::PopAndDestroy();  // writeStream

	RBufReadStream readStream ( *buffer );
	CleanupClosePushL ( readStream );
	iChunk.Adjust ( buffer->Size() );

	RMemWriteStream chunkStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( chunkStream );
	chunkStream.WriteL ( readStream );
	CleanupStack::PopAndDestroy(3);  // chunkStream, readStream, buffer
	_DBG_FILE("CNSmlDMHostSession::WriteTreeToChunkL(): end");
	}


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CalcCheckSumL
//	Auxiliary function called by GetDDFStructureL.
//	Calculates a CCITT CRC checksum from the collected DDF versions and adapter UIDs 
//	@param aVersions. A collected list from adapters containing versions of the DDF structure. 
//	@param aUids. A list of adapter UIDs. 
//	@return TUint16. Calculated checksum value.
// ------------------------------------------------------------------------------------------------
//
TUint16 CNSmlDMHostSession::CalcCheckSumL ( RPointerArray<HBufC8>& aVersions, RArray<TInt32>& aUids ) const
	{
	_DBG_FILE("CNSmlDMHostSession::CalcCheckSumL(): begin");
	TUint16 crc(0);	
	CBufBase* checkData = CBufFlat::NewL(32);
	CleanupStack::PushL(checkData);
	for ( TInt i(0); i<aUids.Count(); i++ )
		{
		checkData->InsertL ( checkData->Size(), aVersions[i]->Des() );
		checkData->InsertL ( checkData->Size(), &aUids[i], sizeof ( aUids[i] ) );
		}
	Mem::Crc ( crc, checkData->Ptr(0).Ptr(), checkData->Size() );
	CleanupStack::PopAndDestroy(); //checkData
	_DBG_FILE("CNSmlDMHostSession::CalcCheckSumL(): end");
	return crc;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::UpdateLeafOrExecuteL
//	Common function for handling UpdateLeafObject and Execute DM commands.
//	Reads parameters from the chunk.
//	Loads the requested adapter if this is a first command to it in this session.
//	Checks the need to inform adapter at atomic command situation.
//	Decides whether or not use streaming for passing data to dm plug-in adapter.
//	Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aCommand. Needed to choose between Update and Execute commands. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::UpdateLeafOrExecuteL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand )
	{
	_DBG_FILE("CNSmlDMHostSession::UpdateLeafOrExecuteL(): begin");
	ReadCmdParamsL ( aMessage );
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == iAdapterId )
			{
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, iAdapterId );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );					
					}
				}
			if ( iStartAtomic && !iAdapters[i].startAtomicCalled )
				{
				iAdapters[i].startAtomicCalled = ETrue;
				iAdapters[i].adapterPtr->StartAtomicL();
				}				
			if ( iAdapters[i].streamingLimit == KNSmlDmNotSupported || iData->Size() < iAdapters[i].streamingLimit )
				{
				CallAdapterCommandL ( aCommand, i );
				}
			else 
				{
				RWriteStream* writeStream = NULL;
				// FOTA
				if ( iAdapterId == KNSmlDMFotaAdapterImplUid && aCommand == ENSmlDMUpdateLeaf )
					{
					// Call overloaded function of the FOTA adapter to tell the 
					// total size of incoming large object.
					TInt sizeOfData ( iData->Size() );
					iAdapters[i].adapterPtr->StreamingSupport ( sizeOfData );
					if ( sizeOfData == KErrNoMemory )
						{
						User::Leave ( KErrTooBig );						
						}
					}
				// FOTA end
				CallAdapterCommandL ( aCommand, i, writeStream );
				if ( !writeStream )
					{
					User::Leave ( KErrGeneral );						
					}
				writeStream->WriteL ( *iData );
				writeStream->Close();
				iAdapters[i].adapterPtr->StreamCommittedL();
				}
			TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
			aMessage.WriteL ( 3, cb );				
			return;
			}
		}
	User::Leave ( KErrArgument );
	_DBG_FILE("CNSmlDMHostSession::UpdateLeafOrExecuteL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::UpdateLeafOrExecuteLargeL
//	Common function for handling UpdateLeafObject and Execute DM commands for large objects.
//	Reads parameters from the chunk.
//	Loads the requested adapter if this is a first command to it in this session.
//	Checks the need to inform adapter at atomic command situation.
//	Decides whether or not use streaming for passing data to dm plug-in adapter.
//	In case this is last package of the large object, closes and commits stream,
//	provided streaming was used for passing a data.
//	Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aCommand. Needed to choose between Update and Execute commands. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::UpdateLeafOrExecuteLargeL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand )
	{
	_DBG_FILE("CNSmlDMHostSession::UpdateLeafOrExecuteLargeL(): begin");
	ReadCmdParamsL ( aMessage );
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == iAdapterId )
			{
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, iAdapterId );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );								
					}
				}
			if ( iStartAtomic && !iAdapters[i].startAtomicCalled )
				{
				iAdapters[i].startAtomicCalled = ETrue;
				iAdapters[i].adapterPtr->StartAtomicL();
				}				
			if ( iAdapters[i].streamingLimit != KNSmlDmNotSupported )
				{
				CheckIfCorrectAdapterL();
				if ( !iLargeWriteStream.streamPtr )
					{
					// FOTA
					if ( iAdapterId == KNSmlDMFotaAdapterImplUid && aCommand == ENSmlDMUpdateLeafLarge )
						{
						// Call overloaded function of the FOTA adapter to tell the 
						// total size of incoming large object.
						// Note that in this context the third parameter holds the size
						// information. Otherwise it contains the 'last package' information.
						TInt totSizeOfLarge ( aMessage.Int2() );
						iAdapters[i].adapterPtr->StreamingSupport ( totSizeOfLarge );
						if ( totSizeOfLarge == KErrNoMemory )
							{
							User::Leave ( KErrTooBig );						
							}
						}					
					// FOTA end
					iLargeWriteStream.adapterId = iAdapterId;
					CallAdapterCommandL ( aCommand, i, iLargeWriteStream.streamPtr );
					if ( !iLargeWriteStream.streamPtr )
						{
						User::Leave ( KErrGeneral );						
						}
					iCommitStatus.iStreamingOngoing=ETrue;
					iCommitStatus.iOldStatusRef = iStatusRef;
					}
				iLargeWriteStream.streamPtr->WriteL ( *iData );
				iCommitStatus.iNewStatusRef = iStatusRef;
				// FOTA
				if ( aMessage.Int2() == KNSmlDmLastPackage )  //  Last package (TBool)
				// FOTA end
					{
					iLargeWriteStream.streamPtr->Close();
					iLargeWriteStream.streamPtr = NULL;									
					iCommitStatus.iStreamCommitted = ETrue;
					iAdapters[i].adapterPtr->StreamCommittedL();
				    iCommitStatus.iStreamCommitted=EFalse;
        		    iCommitStatus.iStreamingOngoing=EFalse;
					}
				TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
				aMessage.WriteL ( 3, cb );
				}
			else 
				{
				UpdateOrExecuteLargeNoStreamL ( aMessage, i, aCommand );
				}
			return;
			}
		}
	User::Leave ( KErrArgument );
	_DBG_FILE("CNSmlDMHostSession::UpdateLeafOrExecuteLargeL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::UpdateOrExecuteLargeNoStreamL
//	Auxiliary function of UpdateLeafOrExecuteLargeL.
//	Provided the dm plug-in adapter doesn't support streaming, this function
//	takes care of appending large object piece by piece.
//	When the last package arrives, calls the adapter and gives the built-up 
//	large object as a parameter.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aAdIndex. Identifies the assigned adapter. 
//	@param aCommand. Needed to choose between Update and Execute commands. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::UpdateOrExecuteLargeNoStreamL ( const RMessage2& aMessage, TInt aAdIndex, TNSmlDmHostOperationCodes aCommand )
	{
	_DBG_FILE("CNSmlDMHostSession::UpdateOrExecuteLargeNoStreamL(): begin");

	if ( !iLargeObject )
		{
		iLargeObject = HBufC8::NewL(0);
		}
	iLargeObject = iLargeObject->ReAllocL ( iLargeObject->Length() + iData->Length() );
	TPtr8 tmpPtr = iLargeObject->Des();	
	tmpPtr.Append ( *iData );
	
	if ( aMessage.Int2() )  //  Last package (TBool)
		{					
		CallAdapterCommandL ( aCommand, aAdIndex, ETrue );
		delete iLargeObject;
		iLargeObject = NULL;
		}		
	_DBG_FILE("CNSmlDMHostSession::UpdateOrExecuteLargeNoStreamL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::AddCopyOrDeleteL
//	Common function for handling Add Interior Node, Copy Node, or Delete Object DM commands.
//	Reads parameters from the chunk.
//	Loads the requested adapter if this is a first command to it in this session.
//	Checks the need to inform adapter at atomic command situation.
//	Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aCommand. Needed to choose between Add, Copy, and Delete commands. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::AddCopyOrDeleteL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand )
	{
	_DBG_FILE("CNSmlDMHostSession::AddCopyOrDeleteL(): begin");
	
	if ( aCommand == ENSmlDMCopy )
		{
		ReadCopyCmdParamsL ( aMessage );			
		}
	else
		{
		ReadCmdParamsL ( aMessage );		
		}
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == iAdapterId )
			{
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, iAdapterId );
				if ( !adapter )
					{
					User::Leave ( KErrArgument );					
					}
				}
			if ( iStartAtomic && !iAdapters[i].startAtomicCalled )
				{
				iAdapters[i].startAtomicCalled = ETrue;
				iAdapters[i].adapterPtr->StartAtomicL();
				}				
			CallAdapterCommandL ( aCommand, i );
			TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
			aMessage.WriteL ( 3, cb );		
			return;
			}
		}
	User::Leave ( KErrArgument );
	_DBG_FILE("CNSmlDMHostSession::AddCopyOrDeleteL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::FetchLeafChildUrisOrSizeL
//	Common function for handling Fetch Leaf Object, Get Child URI List, or Fetch Leaf Object Size 
//	DM commands.
//	Reads parameters from the chunk.
//	Loads the requested adapter if this is a first command to it in this session.
//	Checks the need to inform adapter at atomic command situation.
//	Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aCommand. Needed to choose between Fetch, Get Child URI List, and Fetch Size commands. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::FetchLeafChildUrisOrSizeL ( const RMessage2& aMessage,  TNSmlDmHostOperationCodes aCommand )
	{
	_DBG_FILE("CNSmlDMHostSession::FetchLeafChildUrisOrSizeL(): begin");
	RPointerArray<HBufC8> prevLuids;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( HBufC8, &prevLuids ) );
	ReadCmdFetchParamsL ( aMessage, prevLuids );
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == iAdapterId )
			{
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, iAdapterId );
				if ( !adapter )
					{
					CleanupStack::PopAndDestroy();  // prevLuids
					User::Leave ( KErrArgument );
					}
				}
			if ( iStartAtomic && !iAdapters[i].startAtomicCalled )
				{
				iAdapters[i].startAtomicCalled = ETrue;
				iAdapters[i].adapterPtr->StartAtomicL();
				}
			CallAdapterCommandL ( aCommand, i );
			CleanupStack::PopAndDestroy();  // prevLuids					
			TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
			aMessage.WriteL ( 3, cb );
			return;
			}
		}
	CleanupStack::PopAndDestroy();  // prevLuids			
	User::Leave ( KErrNotFound );
	_DBG_FILE("CNSmlDMHostSession::FetchLeafChildUrisOrSizeL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::FetchLinkL
//	Called only from other DM Host Servers when they need to cross-reference AP -adapter. 
//	Loads the AP dm plug in adapter (i.e. Internet adapter).
//	Asks the DDF structure from the AP adapter.
//	Checks that the given URI is found from the DDF structure.
//	If the above actions are successful, calls the Fetch Leaf Object or Get Child URI List 
//	commands of AP adapter.
//	Writes the returned result to the chunk.
//	@param aMessage. An object which encapsulates a client (other DM Host Server in this case) request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::FetchLinkL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::FetchLinkL(): begin");
	
	RPointerArray<HBufC8> versions;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( HBufC8, &versions ) );
	RArray<TInt32> uids;
	CleanupClosePushL ( uids );
	
	CSmlDmAdapter* adapter = NULL;
	TUid uid = { GetAccessPointImplUid() };
	
	adapter = CSmlDmAdapter::NewL ( uid, *this );
	CleanupStack::PushL(adapter);
	AskInfoFromAdapterL ( adapter, versions, uids, uid.iUid );		
	CleanupStack::PopAndDestroy(3); // adapter, uids, versions 
	
	HBufC8* uri = HBufC8::NewLC ( aMessage.GetDesLengthL(0) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL ( 0, uriPtr, 0 );    
	
	CBufBase* buf = CBufFlat::NewL ( 16 );
	CleanupStack::PushL ( buf );
	MSmlDmAdapter::TError status = MSmlDmAdapter::EOk;

	TUint32 adId(0);
	TNSmlDmDDFFormat nodeType;
	TBool adapterFound = AdapterIdFromUriL ( *uri, adId, nodeType );
	if ( adapterFound )
		{
		FetchLinkFromAdapterL ( *uri, adId, *buf, status, nodeType );
		}
	if ( !adapterFound || status != MSmlDmAdapter::EOk )
		{
		CleanupStack::PopAndDestroy(2); // buf, uri	
		User::Leave ( KErrNotFound );
		}
	
	// Write answer
	iChunk.Adjust ( 2 + buf->Size() );
	RMemWriteStream writeStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( writeStream );
	writeStream.WriteUint16L ( buf->Size() );
	writeStream.WriteL ( buf->Ptr(0) );
	CleanupStack::PopAndDestroy(3); // writestream, buf, uri
	_DBG_FILE("CNSmlDMHostSession::FetchLinkL(): end");
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::StartAtomicL()
//	Sets a general atomic command state. 
//	When this state is set, StartAtomicL function of the adapters
//	is called when the first 'normal' command arrives.
//	In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::StartAtomicL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::StartAtomicL(): begin");	
	iStartAtomic = ETrue;
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );
	_DBG_FILE("CNSmlDMHostSession::StartAtomicL(): end");
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::CommitAtomicL()
//	Resets the general atomic command state (When atomic commands were successful). 
//	This command is forwarded only to those adapters which have received StartAtomicL call.
//	In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CommitAtomicL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::CommitAtomicL(): begin");
	iStartAtomic = EFalse;
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].startAtomicCalled && iAdapters[i].adapterPtr )
			{
			iAdapters[i].startAtomicCalled = EFalse;	
			iAdapters[i].adapterPtr->CommitAtomicL();	
			}
		}
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );		
	_DBG_FILE("CNSmlDMHostSession::CommitAtomicL(): end");
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::RollbackAtomicL()
//	Resets the general atomic command state (When one or more atomic commands failed). 
//	This command is forwarded only to those adapters which have received StartAtomicL call.
//	In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::RollbackAtomicL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::RollbackAtomicL(): begin");
	iStartAtomic = EFalse;	
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].startAtomicCalled && iAdapters[i].adapterPtr )
			{
			iAdapters[i].startAtomicCalled = EFalse;	
			iAdapters[i].adapterPtr->RollbackAtomicL();	
			}
		}
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );		
	_DBG_FILE("CNSmlDMHostSession::RollbackAtomicL(): end");
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::CompleteCommandsL()
//	Calls CompleteOutstandingCmdsL -function of the DM plug-in adapters.
//	This command is issued only to those adapters which have received some adapter 
//	commands during the session.
//	In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CompleteCommandsL ( const RMessage2& aMessage )
	{	
	_DBG_FILE("CNSmlDMHostSession::CompleteCommandsL(): begin");
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].commandsCalled && iAdapters[i].adapterPtr )
			{
			iAdapters[i].commandsCalled = EFalse;	
			iAdapters[i].adapterPtr->CompleteOutstandingCmdsL();
			}
		}
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );		
	_DBG_FILE("CNSmlDMHostSession::CompleteCommandsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SetChunkHandleL
//	Sets a handle to the global memory chunk. 
//	The chunk is a common data area between the client (DM Tree Module) and the server
//	(DM Host Server).
//	Additionally, a server id is sent in the message.
//	This id tells to the (common) session class which server session is serving.  
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::SetChunkHandleL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::SetChunkHandleL(): begin");
	iOwnId = aMessage.Int1();
	iChunk.Close();
	TInt err=iChunk.Open ( aMessage, 0, EFalse );	
	if(err != KErrNone)
	{
		User::Leave (err);		
	}
	if ( iChunk.MaxSize() <= KNSmlDmChunkMarginal )
		{
		User::Leave ( KErrGeneral );		
		}
	_DBG_FILE("CNSmlDMHostSession::SetChunkHandleL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::GetMappingsL
//	Gets the URI / Luid mappings which have been received from the dm adapters 
//	via callback function calls. 
//	These mappings are buffered by the session class, and - during this call,
//	all written to the chunk at once.
//	Additionally informs the client (DM Tree Module) if there are also unread statuses or results.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::GetMappingsL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::GetMappingsL(): begin");
	if ( !iMapUris || !iMapLuids )
		{
		User::Leave ( KErrNotFound );		
		}
	
	iChunk.Adjust ( SizeOfMappings() );
	RMemWriteStream writeStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( writeStream );
	NSmlDmCmdArgHandler::PacketMappingsL ( writeStream, *iMapUris, *iMapLuids );			
	
	iMapUris->ResetAndDestroy();
	delete iMapUris;
	iMapUris = NULL;
	iMapLuids->ResetAndDestroy();
	delete iMapLuids;
	iMapLuids = NULL;
	for ( TInt i=0; i<iCallbackList.Count(); i++ )
		{
		if ( iCallbackList[i].callBack == ENSmlDmCallbackMappings )
			{
			iCallbackList.Remove(i);
			i--;				
			}
		}
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );	
	CleanupStack::PopAndDestroy();  // writeStream	
	_DBG_FILE("CNSmlDMHostSession::GetMappingsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::GetStatusesL
//	Gets the DM command statuses received from the dm adapters via callback function calls. 
//	These statuse are buffered by the session class, and - during this call,
//	all written to the chunk at once.
//	Additionally informs the client (DM Tree Module) if there are also unread mappings or results.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::GetStatusesL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::GetStatusesL(): begin");
	if ( !iStatusCodeList.Count() )
		{
		User::Leave ( KErrNotFound );		
		}

	iChunk.Adjust ( SizeOfStatuses() );
	RMemWriteStream writeStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( writeStream );
	NSmlDmCmdArgHandler::PacketStatusesL ( writeStream, iStatusCodeList );			
	
	iStatusCodeList.Close();
	
	for ( TInt i=0; i<iCallbackList.Count(); i++ )
		{
		if ( iCallbackList[i].callBack == ENSmlDmCallbackStatuses )
			{
			iCallbackList.Remove(i);
			i--;			
			}
		}
	
	TPckgBuf<TInt> cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;
	aMessage.WriteL ( 3, cb );	
	CleanupStack::PopAndDestroy();  // writeStream
	_DBG_FILE("CNSmlDMHostSession::GetStatusesL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::GetResultsL
//	Gets the DM command results received from the dm adapters via callback function calls.
//	In practice, these are the results from the 'FetchLeafObject', 'FecthLeafObjectSize', 
//	and 'GetChildURIList' commands.
//	The results are all written to the chunk at once if they just fit there.
//	In case of large objects the result item is cut into pieces by writing one 'chunkfull' at time.
//	Informs the client (DM Tree Module) if the result is cut up and needs thus additional 
//	GetResultsL -function calls. Additionally, informs the client if there are unread mappings 
//	or statuses.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::GetResultsL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::GetResultsL(): begin");
	TInt okToWrite(0);
	TInt adjustChunkSize(0);	
	TInt largeTotSize(0);
	TNSmlDmResultAnalysis conclusion = AnalyzeResultList ( okToWrite, adjustChunkSize );
	
	iChunk.Adjust ( adjustChunkSize );
	RMemWriteStream writeStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( writeStream );
	
	if ( conclusion == ENSmlDmAllWillFit )
		{
		NSmlDmCmdArgHandler::PacketResultsL ( writeStream, iResultList );			
		RemoveResultCallbacksL ( iResultList.Count() );
		}
	else if ( conclusion == ENSmlDmSomeWillFit )
		{
		RPointerArray<CNSmlDmResultElement> someResults;
		CleanupClosePushL ( someResults );
		for ( TInt i(0); i<okToWrite; i++ )
			{
			someResults.AppendL ( iResultList[i] );			
			}
		NSmlDmCmdArgHandler::PacketResultsL ( writeStream, someResults );
		CleanupStack::PopAndDestroy();  // someResults
		RemoveResultCallbacksL ( okToWrite );
		}
	else if ( conclusion == ENSmlDmStreamed )
		{
		if ( !iResultList[0]->iStreamed )
			{
			User::Leave ( KErrGeneral );			
			}
		
		RPointerArray<CNSmlDmResultElement> result;
		CleanupClosePushL ( result );
		result.AppendL ( iResultList[0] );			
		NSmlDmCmdArgHandler::PacketResultsL ( writeStream, result );

		largeTotSize = iResultReadStream->Source()->SizeL();
		TInt leftToRead = largeTotSize - iResultOffset;
		TInt available = RoomForLargeData ( *iResultList[0]->iMimeType );
		delete iResultList[0]->iResult;
		iResultList[0]->iResult = NULL;

		RBufWriteStream writeBufStream;
		CleanupClosePushL ( writeBufStream );
		if ( leftToRead > available )
			{
			iResultList[0]->iResult = CBufFlat::NewL ( available );
			writeBufStream.Open ( *iResultList[0]->iResult );
			iResultReadStream->ReadL ( writeBufStream, available );
			iResultOffset += available;
			}
		else
			{
			iResultList[0]->iResult = CBufFlat::NewL ( leftToRead );
			writeBufStream.Open ( *iResultList[0]->iResult );
			iResultReadStream->ReadL ( writeBufStream, leftToRead );
			iResultReadStream->Close();
			iResultList[0]->iStreamed = EFalse;
			iResultReadStream = NULL;
			iResultOffset = 0;
			}
		CleanupStack::PopAndDestroy ( 2 );  // writeBufStream, result
		}
	else if ( conclusion == ENSmlDmLargeDoesntFit )
		{
		largeTotSize = iResultList[0]->iResult->Size();
		TInt available = iChunk.Size() - KNSmlDmChunkMarginal;
		CBufBase* newBuffer = CBufFlat::NewL ( available );
		CleanupStack::PushL ( newBuffer );
		newBuffer->InsertL ( 0, iResultList[0]->iResult->Ptr(0).Left ( available ) );
		
		CBufBase* oldBuffer = iResultList[0]->iResult;
		iResultList[0]->iResult = newBuffer;
				
		RPointerArray<CNSmlDmResultElement> truncResult;
		CleanupClosePushL ( truncResult );
		truncResult.AppendL ( iResultList[0] );			
		NSmlDmCmdArgHandler::PacketResultsL ( writeStream, truncResult );
		
		CleanupStack::PopAndDestroy(2);  // truncResult, newBuffer
		oldBuffer->Delete ( 0, available );
		iResultList[0]->iResult = oldBuffer;
		}
	
	CleanupStack::PopAndDestroy();  // writeStream
	TPckgBuf<TInt> cb;
	if ( conclusion == ENSmlDmStreamed || conclusion == ENSmlDmLargeDoesntFit )
		{
		cb = ENSmlDmCallbackChunkFull;
		TPckgBuf<TInt> totalSize ( largeTotSize );
		aMessage.Write ( 2, totalSize );		
		}
	else 
		{
		cb = ( iCallbackList.Count() ) ? iCallbackList[0].callBack : ENSmlDmCallbackNone;		
		}
	aMessage.WriteL ( 3, cb );	
		
	_DBG_FILE("CNSmlDMHostSession::GetResultsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::AnalyzeResultList
//	Auxiliary function called by GetResultsL.
//	This function analyzes whether all the results will fit into the chunk
//	or not. 
//	@param aOkToWrite. A reference parameter which is used only in case of
//	                  'some will fit' for telling how many results can be
//	                   written at once.
//	@param aAdjustChunkSize. A reference parameter for adjusting the chunk to 
//	    				     adequate size before writing takes place.
//	@return TNSmlDmResultAnalysis. Possible return values are: 1) 'all will fit',
//	                               2) 'some will fit', 3) 'large object (streamed) -
//	     						   won't fit', and 4) 'large object (not streamed)
//	 							   - won't fit'.
// ------------------------------------------------------------------------------------------------
//
CNSmlDMHostSession::TNSmlDmResultAnalysis CNSmlDMHostSession::AnalyzeResultList ( TInt& aOkToWrite, TInt& aAdjustChunk  )
	{
	TInt occupied(2);  // 2 = count 16bits
	for ( TInt i(0); i<iResultList.Count(); i++ )
		{
		occupied += ResultItemSize ( i );
		TInt maximum ( KNSmlDmHostChunkMaxSize - KNSmlDmChunkMarginal );
		if (  occupied <= maximum )
			{
			aOkToWrite++;
			if ( iResultList[i]->iStreamed )
				{
				aAdjustChunk = KNSmlDmHostChunkMaxSize;				
				return ENSmlDmStreamed;
				}
			}
		else 
			{
			if ( aOkToWrite )
				{
				aAdjustChunk = occupied - ResultItemSize(i);				
				return ENSmlDmSomeWillFit;
				}
			else
				{
				aAdjustChunk = KNSmlDmHostChunkMaxSize;				
				return ENSmlDmLargeDoesntFit;			
				}
			}
		}
	aAdjustChunk = occupied;
	return ENSmlDmAllWillFit;
	}


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::RemoveResultCallbacksL
//	Auxiliary function for removing the 'read' results from the internal list.
//	This function is called from GetResultsL after some (or all) results 
//	are written to the chunk. 
//	@param aNumber. The number of the results which are needed to be 
//	  			    removed from the internal list.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::RemoveResultCallbacksL ( TInt aNumber )
	{
	if ( aNumber > iCallbackList.Count() || aNumber > iResultList.Count() || aNumber < 0 )
		{
		User::Leave ( KErrGeneral );		
		}
	
	for ( TInt i(0); i<aNumber; i++ )
		{
		for ( TInt u(0); u<iCallbackList.Count(); u++ )
			{
			if ( iCallbackList[u].callBack == ENSmlDmCallbackResults )
				{
				iCallbackList.Remove(u);
				break;				
				}
			}
		delete iResultList[0];
		iResultList.Remove(0);
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::LoadNewAdapterL
//	Auxiliary function called by the command handling functions (e.g. AddCopyOrDeleteL).
//	Loads the ECom plug-in Dm Adapter.
//	The adapter is identified with the given implementation UID.
//	@param aIndex. A place in the internal list where the adpater pointer is stored. 
//	@param aAdapterId. The implementation UID of the requested DM adapter. 
//	@return CSmlDmAdapter*. New instance of the ECom plug-in DM adapter.
// ------------------------------------------------------------------------------------------------
//
CSmlDmAdapter* CNSmlDMHostSession::LoadNewAdapterL ( TInt aIndex, TUint32 aAdapterId )
	{
	_DBG_FILE("CNSmlDMHostSession::LoadNewAdapterL(): begin");	
	CSmlDmAdapter* adapter = NULL;
	TUid uid = { aAdapterId };
	adapter = CSmlDmAdapter::NewL ( uid, *this );
	iAdapters[aIndex].adapterPtr = adapter;
	_DBG_FILE("CNSmlDMHostSession::LoadNewAdapterL(): end");	
	return adapter;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ReadCmdParamsL
//	Auxiliary function called by the command handling functions (e.g. UpdateLeafOrExecuteL).
//	Reads the DM command parameters from the message and from the chunk.
//	Utilizes the DM Utils services for parsing data in the chunk.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ReadCmdParamsL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::ReadCmdParamsL(): begin");
	DeletePrevParams();
	RMemReadStream readStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( readStream );
	NSmlDmCmdArgHandler::ParseDataL ( readStream, iLuid, iData, iUri, iType );
	iAdapterId = aMessage.Int0();
	iStatusRef = aMessage.Int1();
	CleanupStack::PopAndDestroy();	// readStream	
	_DBG_FILE("CNSmlDMHostSession::ReadCmdParamsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ReadCmdFetchParamsL
//	Auxiliary function called by the FetchLeafChildUrisOrSizeL.
//	Reads the DM command parameters from the message and from the chunk.
//	Utilizes the DM Utils services for parsing data in the chunk.
//	@param aMessage. An object which encapsulates a client request. 
//	@param aPreviousLuids. A reference parameter, needed for freeing the reserved 
//	                       heap memory when the luids can be destroyed. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ReadCmdFetchParamsL ( const RMessage2& aMessage, RPointerArray<HBufC8>& aPreviousLuids )
	{
	_DBG_FILE("CNSmlDMHostSession::ReadCmdFetchParamsL(): begin");	
	DeletePrevParams();
	RMemReadStream readStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( readStream );
	iPrevSegURIList = new (ELeave) CArrayFixFlat<TSmlDmMappingInfo>(KNSmlDmGranularity);
	NSmlDmCmdArgHandler::ParseFetchArgumentsL ( readStream, iLuid, iUri, iType, *iPrevSegURIList, aPreviousLuids );
	iAdapterId = aMessage.Int0();
	iStatusRef = aMessage.Int1();
	iResultRef = aMessage.Int2();
	CleanupStack::PopAndDestroy();	// readStream	
	_DBG_FILE("CNSmlDMHostSession::ReadCmdFetchParamsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ReadCopyCmdParamsL
//	Auxiliary function called by the AddCopyOrDeleteL.
//	Reads the DM command parameters from the message and from the chunk.
//	Utilizes the DM Utils services for parsing data in the chunk.
//	@param aMessage. An object which encapsulates a client request. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::ReadCopyCmdParamsL ( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDMHostSession::ReadCopyCmdParamsL(): begin");
	DeletePrevParams();
	RMemReadStream readStream ( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL ( readStream );
	NSmlDmCmdArgHandler::ParseCopyArgumentsL ( readStream, iTargetLuid, iTargetURI, iLuid, iUri, iType );
	iAdapterId = aMessage.Int0();
	iStatusRef = aMessage.Int1();
	CleanupStack::PopAndDestroy();	// readStream	
	_DBG_FILE("CNSmlDMHostSession::ReadCopyCmdParamsL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::DeletePrevParams
//	Auxiliary function for deleting previous set of DM command parameters.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::DeletePrevParams()
	{
	_DBG_FILE("CNSmlDMHostSession::DeletePrevParams(): begin");	
	delete iLuid;
	iLuid = NULL;
	delete iUri;
	iUri = NULL;
	delete iData;
	iData = NULL;
	delete iType;
	iType = NULL;
	delete iTargetLuid;
	iTargetLuid = NULL;
	delete iTargetURI;
	iTargetURI = NULL;
    if ( iPrevSegURIList )
    	{
    	iPrevSegURIList->Reset();    	
    	}
    delete iPrevSegURIList;
    iPrevSegURIList = NULL;
	_DBG_FILE("CNSmlDMHostSession::DeletePrevParams(): end");	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CheckIfCorrectAdapterL
//	Auxiliary function called by the UpdateLeafOrExecuteLargeL.
//	Checks that the arrived command is not to different adapter,
//	during the large object update to the other. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CheckIfCorrectAdapterL() const
	{
	_DBG_FILE("CNSmlDMHostSession::CheckIfCorrectAdapterL(): begin");	
	if ( iLargeWriteStream.streamPtr && iLargeWriteStream.adapterId != iAdapterId )
		{
		User::Leave ( KErrArgument );		
		}
	_DBG_FILE("CNSmlDMHostSession::CheckIfCorrectAdapterL(): end");	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CallAdapterCommandL
//	Auxiliary function called by the the command handling functions, 
//	when the streaming is not used.
//	Calls the chosen DM adapter function of the given adapter.
//	The function parameters are set from the internal member variables.
//	@param aCommand. Identifies the chosen adapter command. 
//	@param aAdIndex. Identifies the chosen DM plug-in adapter. 
//	@param aLargeObject. If the data is a large object, a different 
//	                     internal variable is chosen. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CallAdapterCommandL ( TNSmlDmHostOperationCodes aCommand, TInt aAdIndex, TBool aLargeObject )
	{
	_DBG_FILE("CNSmlDMHostSession::CallAdapterCommandL(): begin");
	iAdapters[aAdIndex].commandsCalled = ETrue;
	switch ( aCommand ) 
		{
		case ENSmlDMAddNode:
			iAdapters[aAdIndex].adapterPtr->AddNodeObjectL ( *iUri, *iLuid, iStatusRef );				
			break;
		case ENSmlDMCopy:
			iAdapters[aAdIndex].adapterPtr->CopyCommandL ( *iTargetURI, *iTargetLuid, *iUri, *iLuid, *iType, iStatusRef );
			break;
		case ENSmlDMDelete:
			iAdapters[aAdIndex].adapterPtr->DeleteObjectL ( *iUri, *iLuid, iStatusRef );			
			break;
		case ENSmlDMFetchLeaf:
			iAdapters[aAdIndex].adapterPtr->FetchLeafObjectL ( *iUri, *iLuid, *iType, iResultRef, iStatusRef );
			break;	
		case ENSmlDMChildUriList:
			iAdapters[aAdIndex].adapterPtr->ChildURIListL ( *iUri, *iLuid, *iPrevSegURIList, iResultRef, iStatusRef );
			break;
		case ENSmlDMFetchLeafSize:
			iAdapters[aAdIndex].adapterPtr->FetchLeafObjectSizeL ( *iUri, *iLuid, *iType, iResultRef, iStatusRef );
			break;
		case ENSmlDMUpdateLeaf:
		case ENSmlDMUpdateLeafLarge:
			if ( !aLargeObject )
				{
				iAdapters[aAdIndex].adapterPtr->UpdateLeafObjectL ( *iUri, *iLuid, *iData, *iType, iStatusRef );				
				}
			else
				{
				iAdapters[aAdIndex].adapterPtr->UpdateLeafObjectL ( *iUri, *iLuid, *iLargeObject, *iType, iStatusRef );				
				}			
			break;
		case ENSmlDMExecute:
		case ENSmlDMExecuteLarge:
			if ( !aLargeObject )
				{
				iAdapters[aAdIndex].adapterPtr->ExecuteCommandL ( *iUri, *iLuid, *iData, *iType, iStatusRef );
				}
			else
				{
				iAdapters[aAdIndex].adapterPtr->ExecuteCommandL ( *iUri, *iLuid, *iLargeObject, *iType, iStatusRef );				
				}
			break;
		default:
			User::Leave ( KErrNotSupported );			
		}
	_DBG_FILE("CNSmlDMHostSession::CallAdapterCommandL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::CallAdapterCommandL
//	Auxiliary function called by the the command handling functions, 
//	when the streaming is used.
//	Calls the chosen DM adapter function of the given adapter.
//	The function parameters are set from the internal member variables.
//	@param aCommand. Identifies the chosen adapter command. 
//	@param aAdIndex. Identifies the chosen DM plug-in adapter. 
//	@param aWriteStream. A reference parameter. The adapter sets this parameter to point
//	                     to the correct place. 
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::CallAdapterCommandL ( TNSmlDmHostOperationCodes aCommand, TInt aAdIndex, RWriteStream*& aWriteStream )
	{
	_DBG_FILE("CNSmlDMHostSession::CallAdapterCommandL( writeStream ): begin");
	iAdapters[aAdIndex].commandsCalled = ETrue;	
	switch ( aCommand )
		{
		case ENSmlDMUpdateLeaf:
		case ENSmlDMUpdateLeafLarge:
			iAdapters[aAdIndex].adapterPtr->UpdateLeafObjectL ( *iUri, *iLuid, aWriteStream, *iType, iStatusRef );
			break;
		case ENSmlDMExecute:
		case ENSmlDMExecuteLarge:
			iAdapters[aAdIndex].adapterPtr->ExecuteCommandL ( *iUri, *iLuid, aWriteStream, *iType, iStatusRef );
			break;		
		default:
			User::Leave ( KErrNotSupported );
		}

	_DBG_FILE("CNSmlDMHostSession::CallAdapterCommandL( writeStream ): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::ResultItemSize
//	Auxiliary function for calculating how much memory does one particular result item reserve.
//	This function is called from AnalyzeResultList. 
//	@param aIndex. The place of the item in the internal result list.
//	@return TInt. Needed memory area in bytes for the given result item.
// ------------------------------------------------------------------------------------------------
//
inline TInt CNSmlDMHostSession::ResultItemSize ( TInt aIndex )
	{
	_DBG_FILE("CNSmlDMHostSession::ResultItemSize: begin");
	// (iResultRef=2bytes) + (iResult->Size=4) + (iResult) + (iMimeType->Lenght=2) + (iMimeType)
	TInt bytes = iResultList[aIndex]->iResult->Size() + iResultList[aIndex]->iMimeType->Size() + KNSmlDmConstItemSize;
	_DBG_FILE("CNSmlDMHostSession::ResultItemSize: end");
	return bytes;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SizeOfMappings
//	Auxiliary function for calculating how much memory do the mappings need.
//	This function is called from GetMappingsL in order to adjust the chunk 
//	to adequate size before the mappings are written to the chunk. 
//	@return TInt. Needed memory area in bytes for all the unread mappings.
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDMHostSession::SizeOfMappings()
	{
	_DBG_FILE("CNSmlDMHostSession::SizeOfMappings: begin");
	TInt bytes(2);  // 2 = count 16bits
	for ( TInt i(0); i<iMapUris->Count(); i++ )
		{
		bytes += 2 + iMapUris->At(i)->Size() + 2 + iMapLuids->At(i)->Size();
		}
	_DBG_FILE("CNSmlDMHostSession::SizeOfMappings: end");
	return bytes;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SizeOfStatuses
//	Auxiliary function for calculating how much memory do the arrived statuse need.
//	This function is called from GetStatusesL in order to adjust the chunk 
//	to adequate size before the statuses are written to the chunk. 
//	@return TInt. Needed memory area in bytes for all the unread statuses.
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDMHostSession::SizeOfStatuses() const
	{
	_DBG_FILE("CNSmlDMHostSession::SizeOfStatuses: begin");
	TInt bytes(2);  // 2 = count 16 bits
	for ( TInt i(0); i<iStatusCodeList.Count(); i++ )
		{
		bytes += 2 + 1;		
		}
	_DBG_FILE("CNSmlDMHostSession::SizeOfStatuses: end");
	return bytes;
	}



// ------------------------------------------------------------------------------------------------
// Callbacks from the dm adapters
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SetResultsL
//	MSmlDmCallback - callback interface.
// 	For returning fetch results from dm plug-in adapters.
//	@param aResultsRef. Reference to correct command.
//	@param aObject. The data which should be returned.
//	@param aType. MIME type of the object.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::SetResultsL ( TInt aResultsRef, CBufBase& aObject, const TDesC8& aType )
	{
	_DBG_FILE("CNSmlDMHostSession::SetResultsL ( CBufBase ): begin");
	
	if ( !iFetchLinkResult )
		{
		TNSmlDmCallbackRef newCallback;
		newCallback.callBack = ENSmlDmCallbackResults;
		newCallback.reference = aResultsRef;

		CNSmlDmResultElement* newResult = new (ELeave) CNSmlDmResultElement;
		CleanupStack::PushL ( newResult );
		newResult->iResultRef = aResultsRef;
		newResult->iResult = CBufFlat::NewL ( aObject.Size() );
		newResult->iResult->InsertL ( 0, aObject.Ptr(0), aObject.Size() );
		newResult->iMimeType = aType.AllocL();
		if ( iResultList.Append ( newResult ) == KErrNone )
			{
			CleanupStack::Pop();  // newResult
			if ( iCallbackList.Append ( newCallback ) != KErrNone )
				{
				delete iResultList[iResultList.Count()-1];
				iResultList.Remove(iResultList.Count()-1);
				}
			}
		else
			{
			CleanupStack::PopAndDestroy();  // newResult			
			}
		}
	else
		{
		delete iResultOtherAdapter;
		iResultOtherAdapter = NULL;
		iResultOtherAdapter = aObject.Ptr(0).AllocL();
		}
	_DBG_FILE("CNSmlDMHostSession::SetResultsL ( CBufBase ): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SetResultsL
//	MSmlDmCallback - callback interface.
// 	For returning fetch results from dm plug-in adapters (using streaming).
//	@param aResultsRef. Reference to correct command.
//	@param aStream. Large data which should be returned.
//	@param aType. MIME type of the object.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::SetResultsL ( TInt aResultsRef, RReadStream*& aStream, const TDesC8& aType )
	{
	_DBG_FILE("CNSmlDMHostSession::SetResultsL ( RReadStream ): begin");
	if ( !iFetchLinkResult )
		{	
		iResultReadStream = NULL;
		iResultOffset = 0;
	
		TNSmlDmCallbackRef newCallback;
		newCallback.callBack = ENSmlDmCallbackResults;
		newCallback.reference = aResultsRef;

		CNSmlDmResultElement* newResult = new (ELeave) CNSmlDmResultElement;
		CleanupStack::PushL ( newResult );
		newResult->iResultRef = aResultsRef;
		newResult->iMimeType = aType.AllocL();
	
		TInt available = RoomForLargeData ( aType );
	
		RBufWriteStream writeStream;
		CleanupClosePushL ( writeStream );
		if ( aStream->Source()->SizeL() >  available )
			{
			newResult->iStreamed = ETrue;
			iResultReadStream = aStream;
			newResult->iResult = CBufFlat::NewL ( available );
			writeStream.Open ( *newResult->iResult );
			aStream->ReadL ( writeStream, available );
			iResultOffset = available;
			}
		else
			{
			newResult->iResult = CBufFlat::NewL ( aStream->Source()->SizeL() );
			writeStream.Open ( *newResult->iResult );
			aStream->ReadL ( writeStream, aStream->Source()->SizeL() );
			aStream->Close();
			}
		CleanupStack::PopAndDestroy();  // writeStream
		if ( iResultList.Append ( newResult ) == KErrNone )
			{
			CleanupStack::Pop();   // newResult
			if ( iCallbackList.Append ( newCallback ) != KErrNone )
				{
				delete iResultList[iResultList.Count()-1];
				iResultList.Remove(iResultList.Count()-1);				
				}
			}
		else
			{
			CleanupStack::PopAndDestroy();   // newResult			
			}
		}
	else
		{
		delete iResultOtherAdapter;
		iResultOtherAdapter = NULL;
		iResultOtherAdapter = HBufC8::NewL ( aStream->Source()->SizeL() );
		TPtr8 tmpPtr = iResultOtherAdapter->Des();
		aStream->ReadL ( tmpPtr );
		aStream->Close();
		}
		
	_DBG_FILE("CNSmlDMHostSession::SetResultsL ( RReadStream ): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SetStatusL
//	MSmlDmCallback - callback interface.
// 	For returning statuses for dm adapter commands.
//	@param aStatusRef. Reference to correct command.
//	@param aErrorCode. Information about the command success.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::SetStatusL ( TInt aStatusRef, MSmlDmAdapter::TError aErrorCode )
	{
	_DBG_FILE("CNSmlDMHostSession::SetStatusL(): begin");
		
	if(aErrorCode  )
		{
		RProperty::Set(KPSUidNSmlDMSyncAgent, 
				KNSmlDMCmdAddNodeSuccess, EFailed );
		}
		else 
		{
		RProperty::Set(KPSUidNSmlDMSyncAgent, 
				KNSmlDMCmdAddNodeSuccess, EAdded );
		}
	if ( !iFetchLinkResult )
		{		
		TNSmlDmCallbackRef newCallback;
		newCallback.callBack = ENSmlDmCallbackStatuses;
		newCallback.reference = aStatusRef;

		TNSmlDmStatusElement newStatus;
		newStatus.iStatusRef = aStatusRef;
		newStatus.iStatus = aErrorCode;	
    	if(iCommitStatus.iStreamingOngoing&&iCommitStatus.iStreamCommitted)
		    {
		    if(aStatusRef==iCommitStatus.iOldStatusRef)
		        {
		        newStatus.iStatusRef = iCommitStatus.iNewStatusRef;
		        }
		    }

		if ( iStatusCodeList.Append ( newStatus ) == KErrNone )
			{
			if ( iCallbackList.Append ( newCallback ) != KErrNone )
				{
				iStatusCodeList.Remove ( iStatusCodeList.Count()-1 );
				}
			}
		}
	else 
		{
		iStatusOtherAdapter = aErrorCode;
		}
	_DBG_FILE("CNSmlDMHostSession::SetStatusL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::SetMappingL
//	MSmlDmCallback - callback interface.
// 	This function is called for a new management object, 
//	both for node objects and for leaf objects by dm adapter.
//	@param aURI. URI of the object. 
//	@param aLUID. Typically this is ID for the database table.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::SetMappingL ( const TDesC8& aURI, const TDesC8& aLUID )
	{
	_DBG_FILE("CNSmlDMHostSession::SetMappingL(): begin");
	if ( !iFetchLinkResult )
		{
		if ( !iMapUris )
			{
			iMapUris = new (ELeave) CArrayPtrSeg<HBufC8>(KNSmlDmGranularity);			
			}
		if ( !iMapLuids )
			{
			iMapLuids = new (ELeave) CArrayPtrSeg<HBufC8>(KNSmlDmGranularity);			
			}

		HBufC8* newUri = aURI.AllocLC();
		iMapUris->AppendL ( newUri );
		CleanupStack::Pop();  // newUri
		 
		HBufC8* newLuid = aLUID.AllocLC();
		iMapLuids->AppendL ( newLuid );
		CleanupStack::Pop();  // newLuid 

		TNSmlDmCallbackRef newCallback;
		newCallback.callBack = ENSmlDmCallbackMappings;
		newCallback.reference = 0;
		iCallbackList.Append ( newCallback );	
		}
	else
		{
		if ( iCbSessConnected && ( aURI.Find ( KNSmlDmApAdapterURI1 ) == 0 || aURI.Find ( KNSmlDmApAdapterURI2 ) == 0 ) )
			{
			iCbSession.AddMappingInfoL ( KNSmlInternetAdapterImplUid, aURI, aLUID );			
			}
		}
	_DBG_FILE("CNSmlDMHostSession::SetMappingL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::FetchLinkL
//	MSmlDmCallback - callback interface.
// 	The function is used to make a fetch to other adapters. 
//	@param aURI. URI of the object. 
//	@param aData. Reference to data, i.e. data is returned here
//	@param aStatus. The status of fetch command is returned here
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::FetchLinkL ( const TDesC8& aURI, CBufBase& aData, MSmlDmAdapter::TError& aStatus )
	{
	_DBG_FILE("CNSmlDMHostSession::FetchLinkL(): begin");
	// Remove possible ./ from the beginning
	TPtrC8 uri = NSmlDmURI::RemoveDotSlash ( aURI );
	
	// Check the ACL rights
	if ( !iCbSessConnected )
		{
		if ( iCbSession.Connect() != KErrNone )
			{
			aData.InsertL ( 0, KNullDesC8 );
			aStatus = MSmlDmAdapter::EError;
			return;			
			}
		else 
			{
			iCbSessConnected = ETrue;			
			}
		}
	
	if ( iCbSession.CheckDynamicAclL( uri, EAclGet, iMgmtTree ) ) //tarm
		{
		// Check if this server has access to the required adapter
		TUint32 adId(0);
		TNSmlDmDDFFormat nodeType;
		if ( AdapterIdFromUriL ( uri, adId, nodeType ) )
			{	
			FetchLinkFromAdapterL ( uri, adId, aData, aStatus, nodeType );
			}
		else
			{
			// Help from other servers needed
			FetchLinkViaIPCL ( uri, aData, aStatus );		
			}
		}
	else 
		{
		aData.InsertL ( 0, KNullDesC8 );
		aStatus = MSmlDmAdapter::EError;
		}
	
	_DBG_FILE("CNSmlDMHostSession::FetchLinkL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::FetchLinkViaIPCL
//	Serves the FetchLink callback when the AP adapter is not found from the server.
//	Issues an IPC -FetchLink command to the other DM Host Server 
//	using RNSmlDMFetchLink -client API.
//	@param aURI. A path to the requested interior node object or leaf object. 
//	@param aData. A reference parameter, i.e. a place for fetch result. 
//	@param aStatus. A reference parameter telling the command success (e.g. EOk or ENotFound).
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::FetchLinkViaIPCL ( const TDesC8& aURI, CBufBase& aData, MSmlDmAdapter::TError& aStatus ) const
	{
	_DBG_FILE("CNSmlDMHostSession::FetchLinkViaIPCL(): begin");
	if ( ( aURI.Find ( KNSmlDmApAdapterURI1 ) == 0 || aURI.Find ( KNSmlDmApAdapterURI2 ) == 0 )
	      && iOwnId != KNSmlDmApAdapterServerId )
		{
		RNSmlDMFetchLink fetchLink;
		TInt openErr ( KErrNone );
		switch ( KNSmlDmApAdapterServerId )
			{
			case 1:
				TRAP ( openErr, fetchLink.OpenL ( KNSmlDmHostServer1Name ) );				
				break;
			case 2:
				TRAP ( openErr, fetchLink.OpenL ( KNSmlDmHostServer2Name ) );				
				break;
			case 3:
				TRAP ( openErr, fetchLink.OpenL ( KNSmlDmHostServer3Name ) );				
				break;
			case 4:
				TRAP ( openErr, fetchLink.OpenL ( KNSmlDmHostServer4Name ) );				
				break;
			default:
				break;
			}
		TRAPD ( chunkErr, fetchLink.SendChunkHandleL ( iChunk ) );
		if ( openErr == KErrNone && chunkErr == KErrNone )
			{
			HBufC8* answer = NULL;
			RMemReadStream readStream ( iChunk.Base(), iChunk.Size() );
			CleanupClosePushL ( readStream );
			TRAPD ( connErr, ( answer = fetchLink.FetchL ( aURI, readStream ) ) );
			CleanupStack::PopAndDestroy();  // readStream
			fetchLink.Close();
			if ( answer && connErr == KErrNone )
				{
				aStatus = MSmlDmAdapter::EOk;
				aData.InsertL ( 0, *answer );
				delete answer;
				answer = 0;
				return;				
				}
			}
		}
	aData.InsertL ( 0, KNullDesC8 );
	aStatus = MSmlDmAdapter::EError;		
	_DBG_FILE("CNSmlDMHostSession::FetchLinkViaIPCL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::FetchLinkFromAdapterL
//	Serves the FetchLink callback when the AP adapter is owned this server.
//	Asks first a luid from the callback server.
//	Calls FetchLeafObjectL -function of the DM adapter if the requested object is a leaf. 
//	Calls ChildURIListL -function of the DM adapter if the requested object is an interior node. 
//	@param aURI. A path to the requested interior node object or leaf object. 
//	@param aAdapterId. Identifies the requested adapter. 
//	@param aData. A reference parameter, i.e. a place for fetch result. 
//	@param aStatus. A reference parameter telling the command success (e.g. EOk or ENotFound).
//	@param aNodeType. Defines whether the object is a leaf or a node.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::FetchLinkFromAdapterL ( const TDesC8& aURI, TUint32 aAdapterId, CBufBase& aData, MSmlDmAdapter::TError& aStatus, TNSmlDmDDFFormat aNodeType )
	{
	_DBG_FILE("CNSmlDMHostSession::FetchLinkFromAdapterL(): begin");
	delete iResultOtherAdapter;
	iResultOtherAdapter = NULL;
	CSmlDmAdapter* adapter = NULL;
	for ( TInt i(0); i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i].adapterId == aAdapterId )
			{
			if ( !iAdapters[i].adapterPtr )
				{
				adapter = LoadNewAdapterL ( i, aAdapterId );
				if ( !adapter )
					{
					break;					
					}
				}
			HBufC8* luid = GetLuidAllocL ( aURI );
			CleanupStack::PushL ( luid );
			if ( aNodeType == ENSmlDmDDFLeaf )
				{
				iFetchLinkResult = ETrue;
				iAdapters[i].adapterPtr->FetchLeafObjectL ( aURI, *luid, KNullDesC8, KNSmlDmNoRef, KNSmlDmNoRef );
				iFetchLinkResult = EFalse;
				}
			else if ( aNodeType == ENSmlDmDDFNode )
				{
				CArrayFixFlat<TSmlDmMappingInfo>* prevURISegList = new ( ELeave ) CArrayFixFlat<TSmlDmMappingInfo> ( KNSmlDmGranularity );
				CleanupStack::PushL ( prevURISegList ); 
				PrevURISegListL ( aAdapterId, aURI, *prevURISegList );
				iFetchLinkResult = ETrue;
				iAdapters[i].adapterPtr->ChildURIListL ( aURI, *luid, *prevURISegList, KNSmlDmNoRef, KNSmlDmNoRef );			
				iFetchLinkResult = EFalse;
				CleanupStack::PopAndDestroy();  // prevURISegList
				}
			CleanupStack::PopAndDestroy();  // luid
			if ( !iResultOtherAdapter )
				{
				break;				
				}
			aData.InsertL ( 0, *iResultOtherAdapter );
			aStatus = iStatusOtherAdapter;
			return;
			}
		}
	aData.InsertL ( 0, KNullDesC8 );
	aStatus = MSmlDmAdapter::EError;
	_DBG_FILE("CNSmlDMHostSession::FetchLinkFromAdapterL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::GetLuidAllocL
//	MSmlDmCallback - callback interface.
// 	The function returns the LUID which is mapped to aURI. 
//	@param aURI. URI of the object. 
//	@return HBufC8*. If LUID is not found, the function allocates a null length string.
// ------------------------------------------------------------------------------------------------
//
HBufC8* CNSmlDMHostSession::GetLuidAllocL ( const TDesC8& aURI )
	{
	_DBG_FILE("CNSmlDMHostSession::GetLuidAllocL()");
	DBG_ARGS8( _S8("GetLuidAllocL(): aURI = %S "), &aURI );	
	HBufC8* luid = NULL;
	
	// Search matching URI from the internal buffer,
	// in case a mapping information is not yet written to db.
	TBool foundInternally(EFalse);
	if ( iMapUris && iMapLuids )
	    {
	    for ( TInt i(0); i<iMapUris->Count(); i++ )
	        {
	        if ( *iMapUris->At(i) == aURI )
	            {
            	_DBG_FILE("CNSmlDMHostSession::GetLuidAllocL() LUID found from internal buffer");	            
	            foundInternally = ETrue;
	            luid = iMapLuids->At(i)->AllocL();
	            }
	        }
	    }
	if ( !foundInternally )
	    {
    	if ( !iCbSessConnected )
	    	{
		    if ( iCbSession.Connect() != KErrNone )
			    {
            	_DBG_FILE("ERROR!!! Connect to Callback server FAILED!!!!");			    
			    luid = HBufC8::NewL(0);
			    return luid;
			    }
		    else 
			    {
			    iCbSessConnected = ETrue;
			    }
		    }
        TUint32 adId(0);
        TNSmlDmDDFFormat nodeType;
	    if ( AdapterIdFromUriL ( aURI, adId, nodeType ) )
	        {
		    luid = iCbSession.GetLuidAllocL ( adId, aURI );
	        }
	    else
		    {
            _DBG_FILE("ERROR!!! AdapterId could not be found using URI");		    
		    luid = HBufC8::NewL(0);	
		    }							
	    }
	DBG_ARGS8( _S8("GetLuidAllocL(): returned luid = %S "), luid );	    
    return luid;			
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::PrevURISegListL
//	Auxiliary function which is called from FetchLinkFromAdapterL.
//	Gets a previous URI segment list from the callback server. 
//	This function is called only during the fetchlink callback and when
//	the requested object is an interior node object.
//	@param aAdapterId. Identifies the requested adapter. 
//	@param aURI. A path to the requested interior node object. 
//	@param aURISegList. A reference parameter where callback server writes a list of the child nodes.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostSession::PrevURISegListL ( TUint32 aAdapterId, const TDesC8& aURI, CArrayFixFlat<TSmlDmMappingInfo>& aURISegList )
	{
	_DBG_FILE("CNSmlDMHostSession::PrevURISegListL(): begin");
	if ( !iCbSessConnected )
		{
		if ( iCbSession.Connect() != KErrNone )
			{
			return;			
			}
		else 
			{
			iCbSessConnected = ETrue;			
			}
		}
	iCbSession.GetUriSegListL ( aAdapterId, aURI, aURISegList );
	_DBG_FILE("CNSmlDMHostSession::PrevURISegListL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::AdapterIdFromUriL
//	Checks if the given URI can be found from the combined DDF structure.
//	If not, most probably the URI belongs to some other DM adapter which this server
//	is not capable of loading.  
//	@param aURI. A path to the requested interior node object or leaf object. 
//	@param aAdapterId. A reference parameter, to which DM Utils service writes the implementation
//	                   UID of the adapter - if the URI was found. 
//	@param aNodeType. A reference parameter defining whether the object is a leaf or a node.
//	@return TBool. ETrue if both the URI and the adapter id were found, EFalse otherwise.
// ------------------------------------------------------------------------------------------------
//
TBool CNSmlDMHostSession::AdapterIdFromUriL ( const TDesC8& aURI, TUint32& aAdapterId, TNSmlDmDDFFormat& aNodeType )
	{
	_DBG_FILE("CNSmlDMHostSession::GetAdapterIdL(): begin");
	CNSmlDmNodeList* finalPtr = NULL;
	aNodeType = iMgmtTree->FindNameFromNodeListL ( aURI, finalPtr );
	if ( finalPtr )
		{
		TUint8 sessId(0);
		finalPtr->GetImplUid ( aAdapterId, sessId );
		if ( aAdapterId || sessId )
			{
			return ETrue;			
			}
		}
	_DBG_FILE("CNSmlDMHostSession::GetAdapterIdL(): end");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostSession::RoomForLargeData
//	Auxiliary function for defining how much there is room in the chunk for actual
//	data of the large object.
//	@param aType. MIME type of the result item.
//	@return TInt. The available size in bytes.
// ------------------------------------------------------------------------------------------------
//
inline TInt CNSmlDMHostSession::RoomForLargeData ( const TDesC8& aType ) const
	{
	return ( ( ( ( KNSmlDmHostChunkMaxSize - KNSmlDmChunkMarginal ) - KNSmlDmConstItemSize ) - aType.Size() ) - 2 );
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::GetAccessPointImplUid()
//	This function is called for getting the accesspoint implementation uid.  
//	@return TUint. The Implementationuid.
// ------------------------------------------------------------------------------------------------
//
	TUint CNSmlDMHostSession::GetAccessPointImplUid()
	{
	if(FeatureManager::FeatureSupported(KFeatureIdFfDmConnmoAdapter))
		{
			return KNSmlDMConnMOAdapterImplUid;
		}
	else
		{
			return KNSmlInternetAdapterImplUid;
		}	
	}

// ------------------------------------------------------------------------------------------------
// void CNSmlDMHostSession::RemoveMappingL()
//	This function is called to remove an URI from tree database.  
//	@return TInt. The Error code.
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDMHostSession::RemoveMappingL( TUint32 aAdapterId, const TDesC8& aURI, TBool aChildAlso )
    {
    TInt ret = KErrNotFound;
    if ( !iCbSessConnected )
        {
        ret = iCbSession.Connect();
        if ( ret != KErrNone )
            {
            _DBG_FILE("ERROR!!! RemoveMappingL Connect to Callback server FAILED!!!!");                            
            }
        else 
            {
            iCbSessConnected = ETrue;           
            }
        }
    ret = iCbSession.RemoveMappingInfoL( aAdapterId, aURI, aChildAlso );
    return ret;
    }
	
// ------------------------------------------------------------------------------------------------
// Client code to access other dm host servers
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// RNSmlDMFetchLink::RNSmlDMFetchLink()
//	C++ constructor.
// ------------------------------------------------------------------------------------------------
//
RNSmlDMFetchLink::RNSmlDMFetchLink()
	: RSessionBase()
	{
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMFetchLink::OpenL()
//	Creates a session (i.e. a connection) to the other DM Host Server.
//	@param aServer. The server name (e.g. "nsmldmhostserver2").	
// ------------------------------------------------------------------------------------------------
//
void RNSmlDMFetchLink::OpenL ( const TDesC& aServer )
	{
	_DBG_FILE("RNSmlDMFetchLink::OpenL(): begin");
	TVersion version ( KNSmlDmHostServerVerMajor, KNSmlDmHostServerVerMinor, KNSmlDmHostServerVerBuild );
	TInt res = CreateSession ( aServer, version );
	User::LeaveIfError( res );	
	_DBG_FILE("RNSmlDMFetchLink::OpenL(): end");
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMFetchLink::SendChunkHandleL
//	Sets a handle to the global memory chunk. 
//	The client DM Host Server will send a handle of the already existing chunk.
//	In other words, no new chunks are created for the FetchLink IPC command. 
//	Additionally, a server id is sent in the message.
//	This id tells to the common session class that it is acting as a 'FetchLink server'.
//	This information is needed in order to avoid destroying the chunk still in use.
//	@param aHandle. A handle to the existing global memory chunk. 
// ------------------------------------------------------------------------------------------------
//
void RNSmlDMFetchLink::SendChunkHandleL ( const RChunk& aChunk ) const
	{
	_DBG_FILE("RNSmlDMFetchLink::SendChunkHandleL(): begin");
	TIpcArgs args;
 	args.Set ( 0, aChunk );
 	args.Set ( 1, KNSmlDmFetchLinkServerId );
 	TInt error = SendReceive ( ENSmlDMChunkHandle, args );
	User::LeaveIfError ( error );		
	_DBG_FILE("RNSmlDMFetchLink::SendChunkHandleL(): end");
	}


// ------------------------------------------------------------------------------------------------
// void RNSmlDMFetchLink::FetchL()
//	Issues a fetch IPC command to the other DM Host Server.  
//	@param aURI. A path to the requested interior node object or the leaf object. 
//	@param aReadStream. A reference to the stream in the chunk from where the result
//	  				    can be read. 
//	@return HBufC8*. The result of the fetch link command.
// ------------------------------------------------------------------------------------------------
//
HBufC8* RNSmlDMFetchLink::FetchL ( const TDesC8& aURI, RReadStream& aReadStream ) const
	{
	_DBG_FILE("RNSmlDMFetchLink::FetchL(): begin");	
    User::LeaveIfError ( SendReceive ( ENSmlDMFetchLink, TIpcArgs( &aURI ) ) );

	TInt ansLength = aReadStream.ReadUint16L();
    HBufC8* answer = HBufC8::NewLC ( ansLength );
    TPtr8 answerPtr = answer->Des();
    if ( ansLength )
    	{
    	aReadStream.ReadL ( answerPtr );    	
    	}
    CleanupStack::Pop();  // answer
    _DBG_FILE("RNSmlDMFetchLink::FetchL(): end");
    return answer;
	}
	


// ------------------------------------------------------------------------------------------------
// void RNSmlDMFetchLink::Close()
//	Closes a session to the other DM Host Server.  
// ------------------------------------------------------------------------------------------------
//
void RNSmlDMFetchLink::Close()
	{
	_DBG_FILE("RNSmlDMFetchLink::Close(): begin");
	// close session	
	RSessionBase::Close();
	_DBG_FILE("RNSmlDMFetchLink::Close(): end");
	}
