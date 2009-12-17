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



#ifndef __NSMLDMHOSTCLIENT_H
#define __NSMLDMHOSTCLIENT_H

#include <smldmadapter.h>
#include <e32base.h>
#include <nsmldebug.h>
#include "nsmldmclientandserver.h"

class CNSmlDmMgmtTree;
class CNSmlDmDDF;

// ===========================================================================
// RNSmlDMDataSession
// ===========================================================================
/**
* DM host client implementation
*
*  @since
*/
class RNSmlDMDataSession : public RSessionBase
	{
public:


	/**
	* Constructor
	*/
	RNSmlDMDataSession();

	/**
	* Destructor
	*/
	~RNSmlDMDataSession();

	/**
	* Connects to correct host server. If Host server is not launhed,
	* the function launches it
	* @param	aServerName			Server name
	* @param	aDmCallback			Pointer to DDF class
	* @return						Success of operation, systemWide error code
	*/
	TInt ConnectL(const TDesC& aServerName,CNSmlDmDDF* aDmCallback);

	/**
	* Returns server version
	* @return						Server version
	*/
	TVersion Version() const;

	/**
	* Closes the session
	*/
	void Close();
	
	/**
	* Gets the DDF structure from plugin adapters of corresponding
	* host server
	* @param	aRoot			Reference to root node of tree
	* @return					Checksum from the current hots servers plug-in
	*							adapter ddf versions
	*/
	TInt DDFStructureL(CNSmlDmMgmtTree& aRoot);
	

	/**
	* Update leaf  object in given adapter
	* @param	aAdapterId		Adapter id
	* @param	aLuid			Luid mapped to uri
	* @param	aStatusRef		Reference to status element
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type of the object
	* @param	aLargeItem		True if object comes in multiple messages
	* @param	aTotSizeOfLarge	The overall size of the large object.
	*							Sent with the first chunk for the FOTA 
	*							adapter.	
	*/
	void UpdateLeafL(const TUint32 aAdapterId,
		const TDesC8& aLuid,
		const TInt32 aStatusRef,
		const TDesC8& aURI,
		const TDesC8& aObject,
		const TDesC8& aType,
		const TBool aLargeItem=EFalse,
		// FOTA
		const TInt aTotSizeOfLarge=0
		// FOTA end
		);
		

	/**
	* Add node object in given adapter
	* @param	aAdapterId		Adapter id
	* @param	aLuid			Luid mapped to uri
	* @param	aStatusRef		Reference to status element
	* @param	aURI			Target URI in command
	*/
	void AddNodeL(const TUint32 aAdapterId,
		const TDesC8& aLuid,
		const TInt32 aStatusRef,
		const TDesC8& aURI);
		
	/**
	* Execute command in given adapter
	* @param	aAdapterId		Adapter id
	* @param	aLuid			Luid mapped to uri
	* @param	aStatusRef		Reference to status element
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type of the object
	* @param	aLargeItem		True if object comes in multiple messages
	*/
	void ExecuteCommandL(const TUint32 aAdapterId,
		const TDesC8& aLuid,
		const TInt32 aStatusRef,
		const TDesC8& aURI,
		const TDesC8& aObject,
		const TDesC8& aType,
		const TBool aLargeItem=EFalse);
		
	/**
	* Copy data in given adapter
	* @param	aAdapterId		Adapter id
	* @param	aTargetLuid		Luid mapped to uri
	* @param	aTargetURI		Target URI in command
	* @param	aSourceLuid		Source luid mapped to uri
	* @param	aSourceURI		Source URI in command
	* @param	aStatusRef		Reference to status element
	* @param	aType			Mime type of the object
	*/
	void CopyCommandL(const TUint32 aAdapterId,
		const TDesC8& aTargetLuid,
		const TDesC8& aTargetURI,
		const TDesC8& aSourceLuid,
		const TDesC8& aSourceURI,
		const TInt32 aStatusRef,
		const TDesC8& aType);
		
	/**
	* Delete object in given adapter
	* @param	aAdapterId		Adapter id
	* @param	aLuid			Luid mapped to uri
	* @param	aStatusRef		Reference to status element
	* @param	aURI			Target URI in command
	*/
	void DeleteObjectL(const TUint32 aAdapterId,
		const TDesC8& aLuid,
		const TInt32 aStatusRef,
		const TDesC8& aURI);
	
	/**
	* Get data from given adapter
	* @param	aAdapterId		Adapter id
	* @param	aURI			Target URI in command
	* @param	aLUID			Luid mapped to uri
	* @param	aType			Mime type of the object
	* @param	aResultRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	*/
	void FetchLeafObjectL(const TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLUID,
		const TDesC8& aType,
		const TInt aResultsRef,
		const TInt aStatusRef );
		
	/**
	* Get data size from given adapter
	* @param	aAdapterId		Adapter id
	* @param	aURI			Target URI in command
	* @param	aLUID			Luid mapped to uri
	* @param	aType			Mime type of the object
	* @param	aResultRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	*/
	void FetchLeafObjectSizeL(const TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLUID,
		const TDesC8& aType,
		const TInt aResultsRef,
		const TInt aStatusRef );
		
	/**
	* Get child list from given adapter
	* @param	aAdapterId				Adapter id
	* @param	aURI					Target URI in command
	* @param	aLUID					Luid mapped to uri
	* @param	aPreviousURISegmentList	Previous list from tree db
	* @param	aResultRef				Reference to result element
	* @param	aStatusRef				Reference to status element
	*/
	void ChildURIListL(const TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLUID,
		const CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegmentList,
		const TInt aResultsRef,const TInt aStatusRef );

	/**
	* Indicates host server about the atomic statrt
	*/
	void StartAtomicL();

	/**
	* Indicates host server about the atomic commit
	*/
	void CommitAtomicL();

	/**
	* Indicates host server about the atomic rollback
	*/
	void RollbackAtomicL();
	
	/**
	* Indicates host server about the message ending
	*/
	void CompleteOutstandingCmdsL();

	/**
	* Gets more data from host server in case of large object
	* @param	aData			Reference to data buffer where to
	*							write data
	*/
	void MoreDataL(CBufBase*& aData);
	
	// FOTA 
	/**
	* When the generic alerts are successfully sent to the remote 
	* server, the FOTA adapter needs to be informed about this.
	* This command is chained through the DM engine.
	* In this function a request is issued as an IPC call.
	*/				
	void MarkGenAlertsSent();
	
	
	void MarkGenAlertsSent(const TDesC8 &aURI);
	
	// FOTA end

private:
	/**
	* Launches server
	* @param	aServerExeName		Server executable name
	* @return						Success of operation
	*/
	TInt LaunchServerL( const TDesC& aServerExeName );

	/**
	* Sends chunck handle to host server
	* @return						Success of operation
	*/
	TInt SendChunkHandle();

	/**
	* Handles callbacks from host server
	* @param	aCallbackType		Callback type 
	*/
	void HandleCallbacksL(TNSmlDmHostCallbackType aCallbackType);

	
private:
	/**
	* Handle to chunk that is used to transfer data between client and server
	*/
	RChunk iChunk;				

	/**
	* Pouinter to DDF class
	*/
	CNSmlDmDDF* iCallback;

	/**
	* Array of URIs when host server sends mappings
	*/
	CArrayPtrSeg<HBufC8>* iMapUris;

	/**
	* Array of Luids when host server sends mappings
	*/
	CArrayPtrSeg<HBufC8>* iMapLuids;

	/**
	* Id of the session, 1...4
	*/
	TUint8 iServerId;
	
	/**
	* Large object uri in case of incoming large object
	*/
	HBufC8* iLargeObjectUri;
	};


	
#endif // __NSMLDMHOSTCLIENT_H