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
* Description:  Handles the communication with DM Tree Module (client). 
* 				 Transfers adapter commands to DM Plug In Adapters.
*
*/


#ifndef __NSMLDMHOSTSERVERSESSION_H__
#define __NSMLDMHOSTSERVERSESSION_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <ecom.h>
#include <smldmadapter.h>
#include "nsmldmclientandserver.h"
#include "nsmldmtreedtd.h"
#include "nsmldmcmdarghandler.h"
#include <nsmldmtreedbclient.h>

// ------------------------------------------------------------------------------------------------
// Data fórmats
// ------------------------------------------------------------------------------------------------
	
struct TNSmlDmAdapter
	{
	CSmlDmAdapter* adapterPtr;
	TUint32 adapterId;
	TBool startAtomicCalled;
	TBool commandsCalled;
	TInt streamingLimit;
	};	

struct TNSmlDmCallbackRef
	{
	TNSmlDmHostCallbackType callBack;
	TInt reference;
	};

struct TNSmlDmWriteStream
	{
    RWriteStream* streamPtr;
	TUint32 adapterId;
	};
	

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class CNSmlDMHostServer;


/**
* Class CNSmlDMHostSession. Handles communication with the DM TreeModule (client).
* Transfers adapter commands to DM Plug In Adapters.
*
* @lib nsmldmhostserver.lib
*/
class CNSmlDMHostSession : public CSession2, public MSmlDmCallback
	{
public:
	enum TNSmlDmResultAnalysis
		{
		ENSmlDmAllWillFit = 0,
		ENSmlDmSomeWillFit,
		ENSmlDmStreamed,
		ENSmlDmLargeDoesntFit
		};
		
	/**
	* Creates a new session object.
	* @param aServer. Reference to server is needed in order to increase and decrease session counts.
	* @return CNSmlDMHostSession*. New instance of this class.
	*/
	static CNSmlDMHostSession* NewL ( CNSmlDMHostServer& aServer );
	
	/**
	* Handles the servicing of client requests.
	*  Entry point for arriving messages.
	* @param aMessage. An object which encapsulates a client request.
	*/
	void ServiceL ( const RMessage2 &aMessage );

	/**
	* Extracts the operation code from the message. 
	*  Based on that calls the right private function.
	* @param aMessage. An object which encapsulates a client request.
	*/
	void DispatchMessageL ( const RMessage2 &aMessage );


	/**
	* MSmlDmCallback - callback interface.
 	*  For returning fetch results from dm plug-in adapters.
	* @param aResultsRef. Reference to correct command.
	* @param aObject. The data which should be returned.
	* @param aType. MIME type of the object.
	*/
	void SetResultsL ( TInt aResultsRef, CBufBase& aObject, const TDesC8& aType );

	/**
	* MSmlDmCallback - callback interface.
 	*  For returning fetch results from dm plug-in adapters (using streaming).
	* @param aResultsRef. Reference to correct command.
	* @param aStream. Large data which should be returned.
	* @param aType. MIME type of the object.
	*/
	void SetResultsL ( TInt aResultsRef, RReadStream*& aStream, const TDesC8& aType );

	/**
	* MSmlDmCallback - callback interface.
 	*  For returning statuses for dm adapter commands.
	* @param aStatusRef. Reference to correct command.
	* @param aErrorCode. Information about the command success.
	*/
	void SetStatusL ( TInt aStatusRef, MSmlDmAdapter::TError aErrorCode );

	/**
	* MSmlDmCallback - callback interface.
 	*  This function is called for a new management object, 
 	*  both for node objects and for leaf objects by dm adapter.
	* @param aURI	URI of the object. 
	* @param aLUID	Typically this is ID for the database table.
	*/
	void SetMappingL ( const TDesC8& aURI, const TDesC8& aLUID );

	/**
	* MSmlDmCallback - callback interface.
 	*  The function is used to make a fetch to other adapters. 
	* @param aURI. URI of the object. 
	* @param aData. Reference to data, i.e. data is returned here
	* @param aStatus. The status of fetch command is returned here
	*/
	void FetchLinkL ( const TDesC8& aURI, CBufBase& aData, MSmlDmAdapter::TError& aStatus );
	
	/**
	* MSmlDmCallback - callback interface.
 	*  The function returns the LUID which is mapped to aURI. 
	* @param aURI. URI of the object. 
	* @return HBufC8*. If LUID is not found, the function allocates a null length string.
	*/	
	HBufC8* GetLuidAllocL ( const TDesC8& aURI );
	
	/**
	* MSmlDmCallback - callback interface.
 	*  The function removes aURI of the corresponding adapter.  
	*@param aAdapterId Adapter Id.
	*@param aURI. URI of the object. 
	*@param aChildAlso Indicates whether childs to be removed or not.
	* @return TInt Error code.
	*/	
	TInt RemoveMappingL( TUint32 aAdapterId, const TDesC8& aURI, TBool aChildAlso );

	/**
	* C++ destructor.
	*/
	~CNSmlDMHostSession();	

private:

	/**        
	* Symbian 2nd phase constructor
	*/
	void ConstructL();
	
	/**
	* C++ constructor.
	* @param aServer. The server instance, owner of session.
	*/
	CNSmlDMHostSession ( CNSmlDMHostServer& aServer);
    
	/**
	* Panics the client
	* @param aMessage. Message from client to panic.
	* @param aReason. Reason code.
	*/
    void PanicClient ( const RMessage2& aMessage, TInt aReason ) const;

	/**
	* Empty function.
	*  Kept here for possible future needs. 
	*  Freeing of resources is done in the destructor.
	*/
    void CloseSessionL() const;
 

	/**
	* Gets DDF structure from the dm adapters.
	*  aMessage parameter contains a list of adapters which are already loaded.
	*  Writes the combined DDF structure to the chunk in the WBXML format.
	*  Returns also a checksum of the DDF structure to the client. 
	* @param aMessage. A list of already loaded adapters, and place for checksum.
	*/
    void GetDDFStructureL ( const RMessage2& aMessage );

	/**
	* Auxiliary function called by GetDDFStructureL.
	*  Reads the UIDs of the loaded adapters from the message, 
	*  and stores those to internal list. 
	* @param aMessage. An object which encapsulates a client request.
	* @param aUids. A reference parameter, a place for extracted list of UIDs.
	*/
    void ReadUidsL ( const RMessage2& aMessage, RArray<TUint32>& aUids ) const;

	/**
	* Auxiliary function called by GetDDFStructureL.
	*  Gets a list of all the potential dm adapters.
	*  Removes from it those which are alreay loaded.  
	* @param aAllAds. A reference parameter, after this function call 
	*                 contains the acceptable adapters.
	* @param aUids. A list of adapters which are already loaded.
	*/
	void RemoveAlreadyLoadedL ( RImplInfoPtrArray& aAllAds, RArray<TUint32>& aUids ) const;    

	/**
	* Auxiliary function called by GetDDFStructureL.
	*  Asks initial information from the loaded dm adapter.
	*  E.g. whether the adapter supports streaming or not.
	* @param aAdapter. A pointer to the just loaded dm plug-in adapter. 
	* @param aVersions. A list of DDF versions collected from the adapters. 
	* @param aUids. A list of adapter UIDs. Note that aVersions and aUids
	*               are always ordered to ascending order in order to prevent
	*               a checksum to change value if ECom framework gives a list
	*				of potential plug-in adapters in different order.
	* @param aAdapterId. Identifier of the dm plug-in adapter.
	*/
	void AskInfoFromAdapterL ( CSmlDmAdapter* aAdapter, RPointerArray<HBufC8>& aVersions, RArray<TInt32>& aUids, TInt32 aAdapterId );
    
	/**
	* Auxiliary function called by GetDDFStructureL.
	*  Writes a combined DDF stucture to the chunk using DM Utils services.
	*/
    void WriteTreeToChunkL();

	/**
	* Auxiliary function called by GetDDFStructureL.
	*  Calculates a CCITT CRC checksum from the collected DDF versions and adapter UIDs 
	* @param aVersions. A collected list from adapters containing versions of the DDF structure. 
	* @param aUids. A list of adapter UIDs. 
	* @return TUint16. Calculated checksum value.
	*/
    TUint16 CalcCheckSumL ( RPointerArray<HBufC8>& aVersions, RArray<TInt32>& aUids ) const;
 

	/**
	* Common function for handling UpdateLeafObject and Execute DM commands.
	*  Reads parameters from the chunk.
	*  Loads the requested adapter if this is a first command to it in this session.
	*  Checks the need to inform adapter at atomic command situation.
	*  Decides whether or not use streaming for passing data to dm plug-in adapter.
	*  Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aCommand. Needed to choose between Update and Execute commands. 
	*/
    void UpdateLeafOrExecuteL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand );

	/**
	* Common function for handling UpdateLeafObject and Execute DM commands for large objects.
	*  Reads parameters from the chunk.
	*  Loads the requested adapter if this is a first command to it in this session.
	*  Checks the need to inform adapter at atomic command situation.
	*  Decides whether or not use streaming for passing data to dm plug-in adapter.
	*  In case this is last package of the large object, closes and commits stream,
	*  provided streaming was used for passing a data.
	*  Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aCommand. Needed to choose between Update and Execute commands. 
	*/
    void UpdateLeafOrExecuteLargeL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand );

	/**
	* Auxiliary function of UpdateLeafOrExecuteLargeL.
	*  Provided the dm plug-in adapter doesn't support streaming, this function
	*  takes care of appending large object piece by piece.
	*  When the last package arrives, calls the adapter and gives the built-up 
	*  large object as a parameter.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aAdIndex. Identifies the assigned adapter. 
	* @param aCommand. Needed to choose between Update and Execute commands. 
	*/
    void UpdateOrExecuteLargeNoStreamL ( const RMessage2& aMessage, TInt aAdIndex, TNSmlDmHostOperationCodes aCommand );

	/**
	* Common function for handling Add Interior Node, Copy Node, or Delete Object DM commands.
	*  Reads parameters from the chunk.
	*  Loads the requested adapter if this is a first command to it in this session.
	*  Checks the need to inform adapter at atomic command situation.
	*  Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aCommand. Needed to choose between Add, Copy, and Delete commands. 
	*/
    void AddCopyOrDeleteL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand );

	/**
	* Common function for handling Fetch Leaf Object, Get Child URI List, or Fetch Leaf Object Size 
	* DM commands.
	*  Reads parameters from the chunk.
	*  Loads the requested adapter if this is a first command to it in this session.
	*  Checks the need to inform adapter at atomic command situation.
	*  Informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aCommand. Needed to choose between Fetch, Get Child URI List, and Fetch Size commands. 
	*/
	void FetchLeafChildUrisOrSizeL ( const RMessage2& aMessage, TNSmlDmHostOperationCodes aCommand );

	/**
	* Called only from other DM Host Servers when they need to cross-reference AP -adapter. 
	*  Loads the AP dm plug in adapter (i.e. Internet adapter).
	*  Asks the DDF structure from the AP adapter.
	*  Checks that the given URI is found from the DDF structure.
	*  If the above actions are successful, calls the Fetch Leaf Object or Get Child URI List 
	*  commands of AP adapter.
	*  Writes the returned result to the chunk.
	* @param aMessage. An object which encapsulates a client (other DM Host Server in this case) request. 
	*/
	void FetchLinkL ( const RMessage2& aMessage );


	/**
	* Sets a general atomic command state. 
	*  When this state is set, StartAtomicL function of the adapters
	*  is called when the first 'normal' command arrives.
	*  In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void StartAtomicL ( const RMessage2& aMessage );

	/**
	* Resets the general atomic command state (When atomic commands were successful). 
	*  This command is forwarded only to those adapters which have received StartAtomicL call.
	*  In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void CommitAtomicL ( const RMessage2& aMessage );

	/**
	* Resets the general atomic command state (When one or more atomic commands failed). 
	*  This command is forwarded only to those adapters which have received StartAtomicL call.
	*  In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void RollbackAtomicL ( const RMessage2& aMessage );

	/**
	* Calls CompleteOutstandingCmdsL -function of the DM plug-in adapters.
	*  This command is issued only to those adapters which have received some adapter 
	*  commands during the session.
	*  In addition informs the client (DM Tree Module) if there are unread statuses, results or mappings.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void CompleteCommandsL ( const RMessage2& aMessage );

	/**
	* Sets a handle to the global memory chunk. 
	*  The chunk is a common data area between the client (DM Tree Module) and the server
	*  (DM Host Server).
	*  Additionally, a server id is sent in the message.
	*  This id tells to the (common) session class which server session is serving.  
	* @param aMessage. An object which encapsulates a client request. 
	*/
    void SetChunkHandleL ( const RMessage2& aMessage );


	/**
	* Gets the URI / Luid mappings which have been received from the dm adapters 
	*  via callback function calls. 
	*  These mappings are buffered by the session class, and - during this call,
	*  all written to the chunk at once.
	*  Additionally informs the client (DM Tree Module) if there are also unread statuses or results.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void GetMappingsL ( const RMessage2& aMessage ); 

	/**
	* Gets the DM command statuses received from the dm adapters via callback function calls. 
	*  These statuse are buffered by the session class, and - during this call,
	*  all written to the chunk at once.
	*  Additionally informs the client (DM Tree Module) if there are also unread mappings or results.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void GetStatusesL ( const RMessage2& aMessage ); 

	/**
	* Gets the DM command results received from the dm adapters via callback function calls.
	*  In practice, these are the results from the 'FetchLeafObject', 'FecthLeafObjectSize', 
	*  and 'GetChildURIList' commands.
	*  The results are all written to the chunk at once if they just fit there.
	*  In case of large objects the result item is cut into pieces by writing one 'chunkfull' at time.
	*  Informs the client (DM Tree Module) if the result is cut up and needs thus additional 
	*  GetResultsL -function calls. Additionally, informs the client if there are unread mappings 
	*  or statuses.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void GetResultsL ( const RMessage2& aMessage ); 
   	

	/**
	* Auxiliary function called by the command handling functions (e.g. AddCopyOrDeleteL).
	*  Loads the ECom plug-in Dm Adapter.
	*  The adapter is identified with the given implementation UID.
	* @param aIndex. A place in the internal list where the adpater pointer is stored. 
	* @param aAdapterId. The implementation UID of the requested DM adapter. 
	* @return CSmlDmAdapter*. New instance of the ECom plug-in DM adapter.
	*/
   	CSmlDmAdapter* LoadNewAdapterL ( TInt aIndex, TUint32 aAdapterId );

	/**
	* Auxiliary function called by the command handling functions (e.g. UpdateLeafOrExecuteL).
	*  Reads the DM command parameters from the message and from the chunk.
	*  Utilizes the DM Utils services for parsing data in the chunk.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void ReadCmdParamsL ( const RMessage2& aMessage );

	/**
	* Auxiliary function called by the FetchLeafChildUrisOrSizeL.
	*  Reads the DM command parameters from the message and from the chunk.
	*  Utilizes the DM Utils services for parsing data in the chunk.
	* @param aMessage. An object which encapsulates a client request. 
	* @param aPreviousLuids. A reference parameter, needed for freeing the reserved 
	*                        heap memory when the luids can be destroyed. 
	*/
   	void ReadCmdFetchParamsL ( const RMessage2& aMessage, RPointerArray<HBufC8>& aPreviousLuids );

	/**
	* Auxiliary function called by the AddCopyOrDeleteL.
	*  Reads the DM command parameters from the message and from the chunk.
	*  Utilizes the DM Utils services for parsing data in the chunk.
	* @param aMessage. An object which encapsulates a client request. 
	*/
   	void ReadCopyCmdParamsL ( const RMessage2& aMessage );
    
	/**
	* Auxiliary function for deleting previous set of DM command parameters.
	*/
    void DeletePrevParams();

	/**
	* Auxiliary function called by the UpdateLeafOrExecuteLargeL.
	*  Checks that the arrived command is not to different adapter,
	*  during the large object update to the other. 
	*/
    void CheckIfCorrectAdapterL() const;

	/**
	* Auxiliary function called by the the command handling functions, 
	* when the streaming is not used.
	*  Calls the chosen DM adapter function of the given adapter.
	*  The function parameters are set from the internal member variables.
	* @param aCommand. Identifies the chosen adapter command. 
	* @param aAdIndex. Identifies the chosen DM plug-in adapter. 
	* @param aLargeObject. If the data is a large object, a different 
	*                      internal variable is chosen. 
	*/
    void CallAdapterCommandL ( TNSmlDmHostOperationCodes aCommand, TInt aAdIndex, TBool aLargeObject = EFalse );

	/**
	* Auxiliary function called by the the command handling functions, 
	* when the streaming is used.
	*  Calls the chosen DM adapter function of the given adapter.
	*  The function parameters are set from the internal member variables.
	* @param aCommand. Identifies the chosen adapter command. 
	* @param aAdIndex. Identifies the chosen DM plug-in adapter. 
	* @param aWriteStream. A reference parameter. The adapter sets this parameter to point
	*                      to the correct place. 
	*/
    void CallAdapterCommandL ( TNSmlDmHostOperationCodes aCommand, TInt aAdIndex, RWriteStream*& aWriteStream );

	/**
	* Serves the FetchLink callback when the AP adapter is not found from the server.
	*  Issues an IPC -FetchLink command to the other DM Host Server 
	*  using RNSmlDMFetchLink -client API.
	* @param aURI. A path to the requested interior node object or leaf object. 
	* @param aData. A reference parameter, i.e. a place for fetch result. 
	* @param aStatus. A reference parameter telling the command success (e.g. EOk or ENotFound).
	*/
    void FetchLinkViaIPCL ( const TDesC8& aURI, CBufBase& aData, MSmlDmAdapter::TError& aStatus ) const;

	/**
	* Serves the FetchLink callback when the AP adapter is owned this server.
	*  Asks first a luid from the callback server.
	*  Calls FetchLeafObjectL -function of the DM adapter if the requested object is a leaf. 
	*  Calls ChildURIListL -function of the DM adapter if the requested object is an interior node. 
	* @param aURI. A path to the requested interior node object or leaf object. 
	* @param aAdapterId. Identifies the requested adapter. 
	* @param aData. A reference parameter, i.e. a place for fetch result. 
	* @param aStatus. A reference parameter telling the command success (e.g. EOk or ENotFound).
	* @param aNodeType. Defines whether the object is a leaf or a node.
	*/
    void FetchLinkFromAdapterL ( const TDesC8& aURI, TUint32 aAdapterId, CBufBase& aData, MSmlDmAdapter::TError& aStatus, TNSmlDmDDFFormat aNodeType );

	/**
	* Checks if the given URI can be found from the combined DDF structure.
	*  If not, most probably the URI belongs to some other DM adapter which this server
	*  is not capable of loading.  
	* @param aURI. A path to the requested interior node object or leaf object. 
	* @param aAdapterId. A reference parameter, to which DM Utils service writes the implementation
	*                    UID of the adapter - if the URI was found. 
	* @param aNodeType. A reference parameter defining whether the object is a leaf or a node.
	* @return TBool. ETrue if both the URI and the adapter id were found, EFalse otherwise.
	*/
	TBool AdapterIdFromUriL ( const TDesC8& aURI, TUint32& aAdapterId, TNSmlDmDDFFormat& aNodeType );

	/**
	* Auxiliary function which is called from FetchLinkFromAdapterL.
	*  Gets a previous URI segment list from the callback server. 
	*  This function is called only during the fetchlink callback and when
	*  the requested object is an interior node object.
	* @param aAdapterId. Identifies the requested adapter. 
	* @param aURI. A path to the requested interior node object. 
	* @param aURISegList. A reference parameter where callback server writes a list of the child nodes.
	*/
	void PrevURISegListL ( TUint32 aAdapterId, const TDesC8& aURI, CArrayFixFlat<TSmlDmMappingInfo>& aURISegList );

	/**
	* Auxiliary function for calculating how much memory does one particular result item reserve.
	*  This function is called from AnalyzeResultList. 
	* @param aIndex. The place of the item in the internal result list.
	* @return TInt. Needed memory area in bytes for the given result item.
	*/
	inline TInt ResultItemSize ( TInt aIndex );

	/**
	* Auxiliary function for calculating how much memory do the mappings need.
	*  This function is called from GetMappingsL in order to adjust the chunk 
	*  to adequate size before the mappings are written to the chunk. 
	* @return TInt. Needed memory area in bytes for all the unread mappings.
	*/
	TInt SizeOfMappings();

	/**
	* Auxiliary function for calculating how much memory do the arrived statuse need.
	*  This function is called from GetStatusesL in order to adjust the chunk 
	*  to adequate size before the statuses are written to the chunk. 
	* @return TInt. Needed memory area in bytes for all the unread statuses.
	*/
	TInt SizeOfStatuses() const;

	/**
	* Auxiliary function called by GetResultsL.
	*  This function analyzes whether all the results will fit into the chunk
	*  or not. 
	* @param aOkToWrite. A reference parameter which is used only in case of
	*                    'some will fit' for telling how many results can be
	*                    written at once.
	* @param aAdjustChunkSize. A reference parameter for adjusting the chunk to 
	*					       adequate size before writing takes place.
	* @return TNSmlDmResultAnalysis. Possible return values are: 1) 'all will fit',
	*                                2) 'some will fit', 3) 'large object (streamed) -
	*								 won't fit', and 4) 'large object (not streamed)
	*								 - won't fit'.
	*/
	TNSmlDmResultAnalysis AnalyzeResultList ( TInt& aOkToWrite, TInt& aAdjustChunkSize );

	/**
	* Auxiliary function for removing the 'read' results from the internal list.
	*  This function is called from GetResultsL after some (or all) results 
	*  are written to the chunk. 
	* @param aNumber. The number of the results which are needed to be 
	*				  removed from the internal list.
	*/
	void RemoveResultCallbacksL ( TInt aNumber );

	/**
	* Auxiliary function for defining how much there is room in the chunk for actual
	* data of the large object.
	* @param aType. MIME type of the result item.
	* @return TInt. The available size in bytes.
	*/
	inline TInt RoomForLargeData ( const TDesC8& aType ) const;
	
	// FOTA
	/**
	* This function is called only for the FOTA adapter.
	*  Makes a request for the FOTA adapter to mark generic alerts sent.
	*  In other words, the written generic alerts are issued without
	*  errors to the remote server.
	*/
	void MarkGenAlertsSentL();
	// FOTA end
	
	/**
	* This function is called any adapter for marking GA sent
	*  Makes a request for the required adapter to mark generic alerts sent.
	*  In other words, the written generic alerts are issued without
	*  errors to the remote server.
	*/
	void MarkGenAlertsSentL(const RMessage2& aMessage);
	
	/**
	* This function is called for getting the accesspoint implementation uid
	*/
	TUint GetAccessPointImplUid();

private:
    struct TNSmlDmStreamCommitStatus
    	{
       	TBool iStreamingOngoing;
    	TInt iOldStatusRef;
    	TInt iNewStatusRef;
       	TBool iStreamCommitted;
    	};
	CNSmlDMHostServer& iServer;
	CNSmlDmMgmtTree* iMgmtTree;	
    RChunk iChunk;
    RArray<TNSmlDmStatusElement> iStatusCodeList;
    RPointerArray<CNSmlDmResultElement> iResultList;
    CArrayPtrSeg<HBufC8>* iMapUris;
    CArrayPtrSeg<HBufC8>* iMapLuids;
    RArray<TNSmlDmAdapter> iAdapters;
    RArray<TNSmlDmCallbackRef> iCallbackList;
    TBool iStartAtomic;
	TNSmlDmWriteStream iLargeWriteStream;
    HBufC8* iLargeObject;
    HBufC* iSemaphoreName;
    TBool iFetchLinkResult;
    HBufC8* iResultOtherAdapter;
    MSmlDmAdapter::TError iStatusOtherAdapter;
    TBool iCbSessConnected;
	RNSmlDMCallbackSession iCbSession;
	RReadStream* iResultReadStream;
	TInt iResultOffset;
	TInt iOwnId;
    
	TUint32 iAdapterId;
	TInt32 iStatusRef;
	TInt32 iResultRef;
	HBufC8* iLuid;
	HBufC8* iUri;
	HBufC8* iData;
	HBufC8* iType;
	HBufC8* iTargetLuid;
	HBufC8* iTargetURI;	
    CArrayFixFlat<TSmlDmMappingInfo>* iPrevSegURIList;
    TNSmlDmStreamCommitStatus iCommitStatus;
	};


/**
* Class RNSmlDMFetchLink. 
* Simple client for connecting to other DM Host Servers. 
* During FetchLink callback.
*
* @lib nsmldmhostserver.lib
*/

class RNSmlDMFetchLink : public RSessionBase
	{
public:

	/**
	* C++ constructor.
	*/
	RNSmlDMFetchLink();

	/**
	* Creates a session (i.e. a connection) to the other DM Host Server.
	* @param aServer. The server name (e.g. "nsmldmhostserver2").	
	*/
	void OpenL ( const TDesC& aServer );

	/**
	* Sets a handle to the global memory chunk. 
	*  The client DM Host Server will send a handle of the already existing chunk.
	*  In other words, no new chunks are created for the FetchLink IPC command. 
	*  Additionally, a server id is sent in the message.
	*  This id tells to the common session class that it is acting as a 'FetchLink server'.
	*  This information is needed in order to avoid destroying the chunk still in use.
	* @param aHandle. A handle to the existing global memory chunk. 
	*/
	void SendChunkHandleL ( const RChunk& aChunk ) const;

	/**
	* Issues a fetch IPC command to the other DM Host Server.  
	* @param aURI. A path to the requested interior node object or the leaf object. 
	* @param aReadStream. A reference to the stream in the chunk from where the result
	*					  can be read. 
	* @return HBufC8*. The result of the fetch link command.
	*/
	HBufC8* FetchL ( const TDesC8& aURI, RReadStream& aReadStream ) const;

	/**
	* Closes a session to the other DM Host Server.  
	*/
	void Close();
	};
	
#endif // __NSMLDMHOSTSERVERSESSION_H__
