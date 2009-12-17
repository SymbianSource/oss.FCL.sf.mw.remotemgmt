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
* Description:  DM tree etc.
*
*/


#include <s32mem.h>
#include <smldmadapter.h>
#include <data_caging_path_literals.hrh>

#include <nsmlconstants.h>
#include "nsmldmhostclient.h"
#include "nsmldmcmdarghandler.h"
#include "nsmldmtreedtd.h"
#include "nsmldmddf.h"
// FOTA
#include "nsmldmimpluids.h"
// FOTA end

const TInt KNSmlMinChunk = 4096;

//********************************
//RNSmlDMDataSession
//********************************

// ------------------------------------------------------------------------------------------------
//void RNSmlDMDataSession::RNSmlDMDataSession()
// ------------------------------------------------------------------------------------------------
RNSmlDMDataSession::RNSmlDMDataSession() :
    iCallback( NULL ),
	iMapUris( NULL ),
	iMapLuids( NULL ),
	iServerId( 0 ),
	iLargeObjectUri( NULL )
	{
	}

// ------------------------------------------------------------------------------------------------
//void RNSmlDMDataSession::~RNSmlDMDataSession()
// ------------------------------------------------------------------------------------------------
RNSmlDMDataSession::~RNSmlDMDataSession()
	{
	if(iMapUris!=0)
		{
		iMapUris->ResetAndDestroy();
		delete iMapUris;
		}
	if(iMapLuids!=0)
		{
		iMapLuids->ResetAndDestroy();
		delete iMapLuids;
		}
	delete iLargeObjectUri;
	}

// ------------------------------------------------------------------------------------------------
//void RNSmlDMDataSession::Connect()
// Connect to the  server - default number of message slots is four
// ------------------------------------------------------------------------------------------------
TInt RNSmlDMDataSession::ConnectL(const TDesC& aServerName,
	CNSmlDmDDF* aCallback)
	{
	iCallback = aCallback;
	
	TInt result( KErrNone );
	TVersion ver ( KNSmlDmHostServerVerMajor, KNSmlDmHostServerVerMinor,
		KNSmlDmHostServerVerBuild );

	result = CreateSession( aServerName, ver );
	if(result!=KErrNone)
		{
		if(aServerName.Compare(KNSmlDmHostServer1Name)==0)
			{
			result = LaunchServerL( KNSmlDmHostServer1Exe );
			iServerId = 1;
			}
		else if(aServerName.Compare(KNSmlDmHostServer2Name)==0)
			{
			result = LaunchServerL( KNSmlDmHostServer2Exe );
			iServerId = 2;
			}
		else if(aServerName.Compare(KNSmlDmHostServer3Name)==0)
			{
			result = LaunchServerL( KNSmlDmHostServer3Exe );
			iServerId = 3;
			}
		else if(aServerName.Compare(KNSmlDmHostServer4Name)==0)
			{
			result = LaunchServerL( KNSmlDmHostServer4Exe );
			iServerId = 4;
			}
		if ( result == KErrNone )
			{
			result = CreateSession( aServerName, ver );
			}
		}
	
	return result; 
	}
	

// ------------------------------------------------------------------------------------------------
// void RNSmlDMDataSession::Version()
// Return the version number
// ------------------------------------------------------------------------------------------------
TVersion RNSmlDMDataSession::Version(void) const
	{
	TVersion ver ( KNSmlDmHostServerVerMajor, KNSmlDmHostServerVerMinor,
		KNSmlDmHostServerVerBuild );

	return ver;
	}


// ------------------------------------------------------------------------------------------------
// void RNSmlDMDataSession::Close()
// Close the session; makes call to server which deletes the object
// container and object index for this session
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::Close()
	{
	iChunk.Close();
	SendReceive(ENSmlDMCloseSession,TIpcArgs());
	RHandleBase::Close();
	if(iMapUris)
		{
		iMapUris->ResetAndDestroy();
		delete iMapUris;
		iMapUris = NULL;
		}
	if(iMapLuids)
		{
		iMapLuids->ResetAndDestroy();
		delete iMapLuids;
		iMapLuids = NULL;
		}
	delete iLargeObjectUri;
	iLargeObjectUri = NULL;
	}


// ------------------------------------------------------------------------------------------------
//void RNSmlDMDataSession::LaunchServer( const TDesC& aServerExeName )
// ------------------------------------------------------------------------------------------------
TInt RNSmlDMDataSession::LaunchServerL( const TDesC& aServerExeName )
	{
	TInt result( KErrNone );
	TParse parser;
	parser.Set ( aServerExeName, &KDC_PROGRAMS_DIR, NULL );

	RProcess server;
	result = server.Create( parser.FullName(), KNullDesC );

	// Process creation failed.
	User::LeaveIfError( result );
	
	TRequestStatus status;
	server.Rendezvous( status );

	if ( status != KRequestPending )
		{
		server.Kill( 0 );	// abort startup
		server.Close();
		return KErrGeneral;	// status can be KErrNone: don't return status.Int()
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
		
	User::WaitForRequest( status );
	
	result = status.Int();
	
	if ( status == KErrNone )
		{
		server.Close();
		}

	return result;
	}


// ------------------------------------------------------------------------------------------------
//TInt RNSmlDMDataSession::DDFStructureL(MSmlDmDDFObject& aDDF)
// ------------------------------------------------------------------------------------------------
TInt RNSmlDMDataSession::DDFStructureL(CNSmlDmMgmtTree& aRoot)
	{
	iChunk.Close();
	User::LeaveIfError(iChunk.CreateGlobal( KNullDesC,
		KNSmlDmHostChunkMinSize, KNSmlDmHostChunkMaxSize ));
	SendChunkHandle();
	
	RArray<TUint32> loadedAdapters;
	aRoot.FindLoadedAdaptersL(loadedAdapters);

	TUint8 count = loadedAdapters.Count();
	
	//1 byte for count and 4 bytes/implUid
	HBufC8* adaptersBuffer = HBufC8::NewLC(1+4*count);
	
	TPtr8 adapterBufPtr = adaptersBuffer->Des();
	RDesWriteStream writeStream(adapterBufPtr);
	CleanupClosePushL(writeStream);
	writeStream.WriteUint8L(count);
	for(TUint8 i=0;i<count;i++)
		{
		TUint32 uid = loadedAdapters[i];
		writeStream.WriteUint32L(uid);
		}
	loadedAdapters.Reset();
	loadedAdapters.Close();
	
	TPckgBuf<TInt> checkSum;
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	
	TIpcArgs args(&adapterBufPtr,0,&checkSum,&callbackCalled);
	CleanupStack::PopAndDestroy(); //writeStream
	
	TInt errCode = SendReceive( ENSmlDMGetDDF, args );
	
	CleanupStack::PopAndDestroy(); //adaptersBuf
	
	if(errCode!=KErrNone)
		{
		return 0;
		}

	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	
	aRoot.ParseWBXMLL(readStream);	
	CleanupStack::PopAndDestroy(); // readStream
	
	return checkSum();	
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::HandleCallbacks(TNSmlDmHostCallbackType aCallbackType)
// handles callbacks
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::HandleCallbacksL(
	TNSmlDmHostCallbackType aCallbackType)
	{
	TNSmlDmHostCallbackType callbackType=aCallbackType;
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	
	while(callbackType!=ENSmlDmCallbackNone)
		{
		if(callbackType==ENSmlDmCallbackMappings)
			{
           	_DBG_FILE("CNSmlDmDDF::HandleCallbacksL():mappings");
			TIpcArgs args(0,0,0,&callbackCalled);
			SendReceive( ENSmlDMGetMappings,args);

			callbackType = callbackCalled();
			RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
			CleanupClosePushL( readStream );
			
			if(iMapUris==0)
				{
				iMapUris = new (ELeave) CArrayPtrSeg <HBufC8> (4);
				}
			if(iMapLuids==0)
				{
				iMapLuids = new (ELeave) CArrayPtrSeg <HBufC8> (4);
				}

			
			NSmlDmCmdArgHandler::ParseMappingsL(readStream,*iMapUris,
				*iMapLuids);
				
			for(TInt i=0;i<iMapUris->Count();i++)
				{
				if(iMapUris->At(i)->Length()&&iMapLuids->At(i)->Length())
					{
					iCallback->SetMappingL(*iMapUris->At(i),*iMapLuids->At(i));
					}
				}
				
			iMapUris->ResetAndDestroy();
			iMapLuids->ResetAndDestroy();
							
			CleanupStack::PopAndDestroy(); // readStream
			}
		else if(callbackType==ENSmlDmCallbackStatuses)
			{
           	_DBG_FILE("CNSmlDmDDF::HandleCallbacksL():statuses");
			TIpcArgs args(0,0,0,&callbackCalled);
			SendReceive( ENSmlDMGetStatuses,args);

			callbackType = callbackCalled();
			RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
			CleanupClosePushL( readStream );
			
			RArray<TNSmlDmStatusElement> statusArray;
			CleanupClosePushL(statusArray);
			NSmlDmCmdArgHandler::ParseStatusesL(readStream,statusArray);

			for(TInt i=0;i<statusArray.Count();i++)
				{
				iCallback->SetStatusL(statusArray[i].iStatusRef,
					statusArray[i].iStatus);
				}
			CleanupStack::PopAndDestroy(2); // readStream, statusArray
			}
		else if(callbackType==ENSmlDmCallbackResults)
			{
           	_DBG_FILE("CNSmlDmDDF::HandleCallbacksL():results");
			TPckgBuf<TInt> dataLength;
			TIpcArgs args(0,0,&dataLength,&callbackCalled);
			SendReceive( ENSmlDMGetResults,args);

			callbackType = callbackCalled();
			RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
			CleanupClosePushL( readStream );
			
			RPointerArray<CNSmlDmResultElement> resultArray;
			CleanupStack::PushL ( PtrArrCleanupItemRArr ( CNSmlDmResultElement, &resultArray ) );

			NSmlDmCmdArgHandler::ParseResultsL(readStream,resultArray);
			
			if(callbackType==ENSmlDmCallbackChunkFull)
			    {
               	_DBG_FILE("CNSmlDmDDF::HandleCallbacksL():results chunk full");
			    for(TInt i=0;i<resultArray.Count();i++)
				    {
				    iCallback->SetResultsL(resultArray[i]->iResultRef,
					    *resultArray[i]->iResult,*resultArray[i]->iMimeType,
					    dataLength(),iServerId-1);
    				}
			    }
			else
			    {
    			for(TInt i=0;i<resultArray.Count();i++)
    				{
    				iCallback->SetResultsL(resultArray[i]->iResultRef,
    					*resultArray[i]->iResult,*resultArray[i]->iMimeType,
    					resultArray[i]->iResult->Size(),iServerId-1);
    				}
				}
			CleanupStack::PopAndDestroy(2); // readStream,resultArray
			}
		else if(callbackType==ENSmlDmCallbackChunkFull)
			{
			callbackType=ENSmlDmCallbackNone;
			}
		else
			{
			callbackType=ENSmlDmCallbackNone;
			}
		} // end while
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::SendChunkHandle
// Sends Chunk handle to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
TInt RNSmlDMDataSession::SendChunkHandle()
	{
	TIpcArgs args;
 	args.Set(0, iChunk);
 	args.Set(1, iServerId);
 	const TInt error = SendReceive( ENSmlDMChunkHandle, args );
 	return error;
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::UpdateLeafL
// Sends update leaf IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::UpdateLeafL(TUint32 aAdapterId,
	const TDesC8& aLuid,
	const TInt32 aStatusRef,
	const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	const TBool aLargeItem,
	// FOTA
	const TInt aTotSizeOfLarge
	// FOTA end
	)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled(ENSmlDmCallbackNone);
	TInt dataLen = aObject.Length()
		+aURI.Length()+aType.Length()+aLuid.Length()+16;
		//16=4*4bytes, TInt = 4 bytes, each datasize must be stored to stream 
	if(dataLen<KNSmlMinChunk)
		{
		iChunk.Adjust(KNSmlMinChunk);
		}
	else
		{
		iChunk.Adjust(dataLen);
		}
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketDataL(writeStream,aLuid,aObject,aURI,aType);
	CleanupStack::PopAndDestroy(); // writeStream
	TInt ret(KErrNone);
		
	if(aLargeItem)
		{
		if(iLargeObjectUri)
			{
			if(iLargeObjectUri->Compare(aURI)==0)
				{
				TIpcArgs args(aAdapterId,aStatusRef,EFalse,&callbackCalled);
				ret = SendReceive( ENSmlDMUpdateLeafLarge,args);
				}
			else
				{
				ret = KErrGeneral;
				}
			}
		else
			{
			iLargeObjectUri=aURI.AllocL();
			// FOTA
			if ( aTotSizeOfLarge && aAdapterId == KNSmlDMFotaAdapterImplUid )
				{
				// For the FOTA adapter the third parameter is overloaded.
				// It contains the overall size of the large object.
				// The size information is sent only with the first chunk.
				TIpcArgs args(aAdapterId,aStatusRef,aTotSizeOfLarge,&callbackCalled);
				ret = SendReceive( ENSmlDMUpdateLeafLarge,args);
				}
			else
				{
				TIpcArgs args(aAdapterId,aStatusRef,EFalse,&callbackCalled);			
				ret = SendReceive( ENSmlDMUpdateLeafLarge,args);
				}
			// FOTA end
			}
		}
	else
		{
		if(iLargeObjectUri)
			{
			delete iLargeObjectUri;
			iLargeObjectUri=NULL;
			TIpcArgs args(aAdapterId,aStatusRef,ETrue,&callbackCalled);
			ret = SendReceive( ENSmlDMUpdateLeafLarge,args);
			}
		else
			{
			TIpcArgs args(aAdapterId,aStatusRef,0,&callbackCalled);
			ret = SendReceive( ENSmlDMUpdateLeaf,args);
			}
		}	

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	// FOTA
	if ( aAdapterId == KNSmlDMFotaAdapterImplUid && ret == KErrTooBig )
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::ETooLargeObject);
		delete iLargeObjectUri;
		iLargeObjectUri=NULL;		
		}
	// FOTA end
	else if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		delete iLargeObjectUri;
		iLargeObjectUri=NULL;
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::AddNodeL
// Sends add node IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::AddNodeL(const TUint32 aAdapterId,
	const TDesC8& aLuid,
	const TInt32 aStatusRef,
	const TDesC8& aURI)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	iChunk.Adjust(KNSmlMinChunk);
		
	TIpcArgs args(aAdapterId,aStatusRef,0,&callbackCalled);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketDataL(writeStream,aLuid,KNullDesC8,
		aURI,KNullDesC8);
	CleanupStack::PopAndDestroy(); // writeStream
			
	SendReceive( ENSmlDMAddNode,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::ExecuteCommandL
// Sends execute IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::ExecuteCommandL(const TUint32 aAdapterId,
	const TDesC8& aLuid,
	const TInt32 aStatusRef,
	const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	const TBool aLargeItem)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled(ENSmlDmCallbackNone);

	TInt dataLen = aObject.Length()
		+aURI.Length()+aType.Length()+aLuid.Length()+16;
		//16=4*4bytes, TInt = 4 bytes, each datasize must be stored to stream 
		
	if(dataLen<KNSmlMinChunk)
		{
		iChunk.Adjust(KNSmlMinChunk);
		}
	else
		{
		iChunk.Adjust(dataLen);
		}
		
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketDataL(writeStream,aLuid,aObject,aURI,aType);
	CleanupStack::PopAndDestroy(); // writeStream
	TInt ret = KErrNone;
			
	if(aLargeItem)
		{
		if(iLargeObjectUri)
			{
			if(iLargeObjectUri->Compare(aURI)==0)
				{
				TIpcArgs args(aAdapterId,aStatusRef,EFalse,&callbackCalled);
				ret = SendReceive( ENSmlDMExecuteLarge,args);
				}
			else
				{
				ret = KErrGeneral;
				}
			}
		else
			{
			iLargeObjectUri=aURI.AllocL();
			TIpcArgs args(aAdapterId,aStatusRef,EFalse,&callbackCalled);
			ret = SendReceive( ENSmlDMExecuteLarge,args);
			}
		}
	else
		{
		if(iLargeObjectUri)
			{
			delete iLargeObjectUri;
			iLargeObjectUri=NULL;
			TIpcArgs args(aAdapterId,aStatusRef,ETrue,&callbackCalled);
			ret = SendReceive( ENSmlDMExecuteLarge,args);
			}
		else
			{
			TIpcArgs args(aAdapterId,aStatusRef,0,&callbackCalled);
			ret = SendReceive( ENSmlDMExecute,args);
			}
		}	

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::CopyCommandL
// Sends copy IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::CopyCommandL(const TUint32 aAdapterId,
	const TDesC8& aTargetLuid,
	const TDesC8& aTargetURI,
	const TDesC8& aSourceLuid,
	const TDesC8& aSourceURI,
	const TInt32 aStatusRef,
	const TDesC8& aType)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	iChunk.Adjust(KNSmlMinChunk);
		
	TIpcArgs args(aAdapterId,aStatusRef,0,&callbackCalled);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketCopyArgumentsL(writeStream,
		aTargetLuid,aTargetURI,aSourceLuid,aSourceURI,aType);
		
	CleanupStack::PopAndDestroy(); // writeStream
			
	TInt ret = SendReceive( ENSmlDMCopy,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::DeleteObjectL
// Sends delete IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::DeleteObjectL(const TUint32 aAdapterId,
	const TDesC8& aLuid,
	const TInt32 aStatusRef,
	const TDesC8& aURI)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	iChunk.Adjust(KNSmlMinChunk);
		
	TIpcArgs args(aAdapterId,aStatusRef,0,&callbackCalled);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketDataL(writeStream,
		aLuid,KNullDesC8,aURI,KNullDesC8);
		
	CleanupStack::PopAndDestroy(); // writeStream
			
	TInt ret = SendReceive( ENSmlDMDelete,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}

	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::FetchLeafObjectL
// Sends fetch IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::FetchLeafObjectL(const TUint32 aAdapterId,
	const TDesC8& aURI,
	const TDesC8& aLuid,
	const TDesC8& aType,
	const TInt aResultsRef,
	const TInt aStatusRef )
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(aAdapterId,aStatusRef,aResultsRef,&callbackCalled);
	iChunk.Adjust(KNSmlMinChunk);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	CArrayFixFlat<TSmlDmMappingInfo>* mapArray =
		new (ELeave) CArrayFixFlat<TSmlDmMappingInfo>(1);
		
	CleanupStack::PushL(mapArray);
	NSmlDmCmdArgHandler::PacketFetchArgumentsL(writeStream,aLuid,
		aURI,aType,*mapArray);
		
	CleanupStack::PopAndDestroy(2); // writeStream,mapArray
			
	TInt ret = SendReceive( ENSmlDMFetchLeaf,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}

	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::FetchLeafObjectSizeL
// Sends fetch size IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::FetchLeafObjectSizeL(const TUint32 aAdapterId,
	const TDesC8& aURI,
	const TDesC8& aLuid,
	const TDesC8& aType,
	const TInt aResultsRef,
	const TInt aStatusRef )
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(aAdapterId,aStatusRef,aResultsRef,&callbackCalled);
	iChunk.Adjust(KNSmlMinChunk);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	CArrayFixFlat<TSmlDmMappingInfo>* mapArray =
		new (ELeave) CArrayFixFlat<TSmlDmMappingInfo>(1);
	CleanupStack::PushL(mapArray);
	
	NSmlDmCmdArgHandler::PacketFetchArgumentsL(writeStream,aLuid,aURI,
		aType,*mapArray);
		
	CleanupStack::PopAndDestroy(2); // writeStream,mapArray
			
	TInt ret = SendReceive( ENSmlDMFetchLeafSize,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}
	}



// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::ChildURIListL
// Sends child urilist IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::ChildURIListL(const TUint32 aAdapterId,
	const TDesC8& aURI,
	const TDesC8& aLuid,
	const CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegmentList,
	const TInt aResultsRef,const TInt aStatusRef )
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(aAdapterId,aStatusRef,aResultsRef,&callbackCalled);
	iChunk.Adjust(KNSmlMinChunk);
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	NSmlDmCmdArgHandler::PacketFetchArgumentsL(writeStream,aLuid,
		aURI,KNullDesC8,aPreviousURISegmentList);
		
	CleanupStack::PopAndDestroy(); // writeStream
			
	TInt ret = SendReceive( ENSmlDMChildUriList,args);

	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	if(ret!=KErrNone)
		{
		iCallback->SetStatusL(aStatusRef,MSmlDmAdapter::EError);
		}
	}


// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::CompleteOutstandingCmdsL
// Sends CompleteOutstandingCmds IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::CompleteOutstandingCmdsL()
	{
   	_DBG_FILE("CNSmlDmDDF::CompleteOutstandingCmdsL():begin");
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(0,0,0,&callbackCalled);
	SendReceive( ENSmlDMCompleteCommands,args);
	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
   	_DBG_FILE("CNSmlDmDDF::CompleteOutstandingCmdsL():end");
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::MoreDataL
// Sends get more data IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::MoreDataL(CBufBase*& aData)
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(0,0,0,&callbackCalled);
	SendReceive( ENSmlDMGetMore,args);
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
			
	RPointerArray<CNSmlDmResultElement> resultArray;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( CNSmlDmResultElement, &resultArray ) );

	NSmlDmCmdArgHandler::ParseResultsL(readStream,resultArray);
	

	RBufWriteStream writeStream(*aData,aData->Size());
	CleanupClosePushL(writeStream);
	
	if(resultArray.Count()>0)
		{
		writeStream.WriteL(resultArray[0]->iResult->Ptr(0),
			resultArray[0]->iResult->Size());
		}
	CleanupStack::PopAndDestroy(3); //writeStream, readStream, resultArray
	}



// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::StartAtomicL
// Sends start atomic IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::StartAtomicL()
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(0,0,0,&callbackCalled);
	SendReceive( ENSmlDMStartAtomic,args);
	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}

	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::CommitAtomicL
// Sends commit atomic IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::CommitAtomicL()
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(0,0,0,&callbackCalled);
	SendReceive( ENSmlDMCommitAtomic,args);
	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::RollbackAtomic
// Sends rollback atomic IPC call to SyncML DM Host Servers.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::RollbackAtomicL()
	{
	TPckgBuf<TNSmlDmHostCallbackType> callbackCalled;
	TIpcArgs args(0,0,0,&callbackCalled);
	SendReceive( ENSmlDMRollbackAtomic,args);
	TNSmlDmHostCallbackType callbackType = callbackCalled();
	if(callbackType!=ENSmlDmCallbackNone)
		{
		HandleCallbacksL(callbackType);
		}
	}

// FOTA
// ------------------------------------------------------------------------------------------------
// RNSmlDMDataSession::MarkGenAlertsSent()
// 	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
//  In this function a request is issued as an IPC call.
// ------------------------------------------------------------------------------------------------
void RNSmlDMDataSession::MarkGenAlertsSent()
	{
	SendReceive ( ENSmlDMGenericAlertsSent,TIpcArgs ( TIpcArgs::ENothing ) );
	}


void RNSmlDMDataSession::MarkGenAlertsSent(const TDesC8 &aURI)
	{
	
	_DBG_FILE(" RNSmlDMDataSession::MarkGenAlertsSent calling with the URI ");
	SendReceive ( ENSmlDMGenericAlertsSent,TIpcArgs ( &aURI ) );
	}
// FOTA end
