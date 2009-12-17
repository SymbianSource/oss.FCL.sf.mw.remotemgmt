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
* Description:  DM Tree module
*
*/


#ifndef __NSMLDMMODULE_H
#define __NSMLDMMODULE_H

#include <e32std.h>
#include <e32base.h>

class MNSmlDmModuleCallBack;
class CNSmlDmDDF;
class CNSmlDmCommandBuffer;



enum TNSmlDmProperty 
	{
	EPropACL = 0,
	EPropFormat,	
	EPropName,
	EPropSize,
	EPropTitle,
	EPropTStamp,
	EPropType,
	EPropVerNo,
	EPropCaseSense
	};


struct CNSmlGetPropertyElement : public CBase
	{
	~CNSmlGetPropertyElement();
	TNSmlDmProperty iProp;
	TInt iResultsRef;
	TInt iStatusRef;
	TInt iStatusCode;
	HBufC8* iUri;
	};


// ----------------------------------------------------------------------------
// CSmlDmModule provides basic interface for DM tree operations
//
// @lib nsmldmmodule.dll
// ----------------------------------------------------------------------------



// ===========================================================================
// CSmlDmModule
// ===========================================================================

class CNSmlDmModule : public CBase 
	{
	public:

	/**
	* Two-phased constructor.
	* @param	aCallBack			Callback pointer
	* @return						Pointer to newly created module instance
	*/
	IMPORT_C static CNSmlDmModule* NewL( MNSmlDmModuleCallBack* aCallBack); 

	/**
	* Destructor
	*/
	IMPORT_C ~CNSmlDmModule();
	
	/**
	* Sets the server identifier
	* @param	aServer				The server id of current session
	*/
	IMPORT_C void SetServerL( const TDesC8& aServer );

	/**
	* Add operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	*/
	IMPORT_C void AddObjectL( const TDesC8& aURI,
		const TDesC8& aObject, const TDesC8& aType,
		TInt aStatusRef,
		TBool aLargeItem=EFalse);
		
	/**
	* Replace operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	* @param	aTotSizeOfLarge	The overall size of the large object.
	*							Sent with the first chunk.
	*/
	IMPORT_C void UpdateObjectL( const TDesC8& aURI,
		const TDesC8& aObject,
		const TDesC8& aType,
		TInt aStatusRef,
		TBool aLargeItem=EFalse,
		// FOTA 
		TInt aTotSizeOfLarge=0
		// FOTA end
		);

	/**
	* Get operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aType			Mime type
	* @param	aResultsRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	* @param	aAclPass		Passes acl checking if true
	*/
	IMPORT_C void FetchObjectL(  const TDesC8& aURI,
		const TDesC8& aType,
		TInt aResultsRef,
		TInt aStatusRef,
		TBool aAclPass = EFalse );
		
	/**
	* Delete operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aStatusRef		Reference to status element
	*/
	IMPORT_C void DeleteObjectL( const TDesC8& aURI, TInt aStatusRef );
	
	/**
	* Execute operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aCorrelator		Correlator received as an argument in 
	*							the exec command.
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	*/
	IMPORT_C void ExecuteObjectL( const TDesC8& aURI,
		const TDesC8& aObject,
		const TDesC8& aType,
		TInt aStatusRef,
		// FOTA
		const TDesC8& aCorrelator,
		// FOTA end
		TBool aLargeItem=EFalse);
		
	/**
	* Copy operation in DM protocoll.
	* @param	aTargetURI		Target URI in command
	* @param	aSourceURI		Source URI in command
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	*							or not
	*/
	IMPORT_C void CopyObjectL( const TDesC8& aTargetURI,
		const TDesC8& aSourceURI,
		const TDesC8& aType,
		TInt aStatusRef);
		
	/**
	* Start Atomic operation
	*/
	IMPORT_C void StartTransactionL( );

	/**
	* Commit Atomic operation
	*/
	IMPORT_C void CommitTransactionL();

	/**
	* Rollback Atomic operation
	*/
	IMPORT_C void RollBackL( );

	/**
	* Indicates message ending
	*/
	IMPORT_C void EndMessageL();

	/**
	* Returns a checksum of ddf versions. Checksum changes if the ddf changes
	* @return					Checksum of plugin adapter ddf versions
	*/
	IMPORT_C TInt IsDDFChangedL();

	/**
	* Gets more data in case of largeobject
	* @param	aData			Reference to buffer where to add more data
	*/
	IMPORT_C void MoreDataL(CBufBase*& aData);


	/**
	* Replace property operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aOffset			Property start offset in aURI
	* @param	aStatusRef		Reference to status element
	*/
	void UpdatePropertyL(const TDesC8& aURI,
		const TDesC8& aObject,
		const TDesC8& aType,
		TInt aOffset,
		TInt aStatusRef);
		
	/**
	* Get property operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aType			Mime type
	* @param	aOffset			Property start offset in aURI
	* @param	aResultsRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	*/
	void GetPropertyL(const TDesC8& aURI,
		const TDesC8& aType,
		TInt aOffset,
		TInt aResultsRef,
		TInt aStatusRef);
		
	/**
	* Deletes te items added inside the atomic, in case that atomic fails
	* @param	aURI			Target URI to be deleted
	* @param	aCallBack		Temporary callback interface for internal
	*							commands -> status not given to agent
	*/
	void DeleteInTransactionL(const TDesC8& aURI,
		MNSmlDmModuleCallBack *aCallBack);

	/**
	* The status is provided throug this function to caller
	* @param	aStatusRef			Reference to status element
	* @param	aStatusCode			Status code to be given
	* @param	aSkipCmdBuf			Indicates that status is given to agent
	*								despite existance of command buffer
	*/
	void DoSetStatusL(TInt aStatusRef,
		TInt aStatusCode,
		TBool aSkipCmdBuf = EFalse);
		
	/**
	* Sets the result to caller by using callback interface
	* @param	aResultsRef			Reference to result element
	* @param	aObject				Result data to be given
	* @param	aType				Mime type of object
	* @param	aFormat				Format of object, got from ddf
	* @param	aTotalSize			Total size of object, in case of large
	* 								object, this is different from aObject
	*								size
	* @param	aSkipCmdBuf			Indicates that status is given to agent
	*								despite existance of command buffer
	*/
	void DoSetResultsL(TInt aResultsRef,
		CBufBase& aObject,
		const TDesC8& aType,
		const TDesC8& aFormat,
		TInt aTotalSize,
		TBool aSkipCmdBuf = EFalse);
		
	/**
	* Closes the connections to callback server and host servers.
	* Separate function is needed, since the disconnecting cannot 
	* be made after the active scheduler of the thread is stopped.
	*/			
	IMPORT_C void DisconnectFromOtherServers();
	
	// FOTA
	/**
	* When the generic alerts are successfully sent to the remote 
	* server, the FOTA adapter needs to be informed about this.
	* This command is chained through the DM engine.
	*/			
	IMPORT_C void MarkGenAlertsSentL();
	// FOTA end

    
      IMPORT_C void MarkGenAlertsSentL(const TDesC8& aURI);
	private:
	/**
	*  Sets the real property result in case that property fetched
	* @param	aIndex				Index in property result buffer
	* @param	aObject				Result data to be given
	* @return						Status code
	*/
	TInt HandlePropertyResultsL(TInt aIndex, CBufBase& aObject);

	/**
	*  Second phase constructor.
	*/
	void ConstructL();

	private:
	/**
	*  Atomic operation is going on or not
	*/
	TBool iInTransaction;

	/**
	*  Pointer to DDF
	*/
	CNSmlDmDDF *iDDF;

	/**
	*  Pointer to module callback interface
	*/
	MNSmlDmModuleCallBack* iCallBack;

	/**
	* This is set true when command is internal, i.e. status not given
	* to agent
	*/
	TBool iInternalCommand;

	/**
	* Property result buffer, new element is created for each get to
	* prperty element
	*/
	RPointerArray<CNSmlGetPropertyElement> iPropResults;
	
	/**
	* DDF checsum
	*/
	TInt iDDFCrc;
	
	/**
	* Internal status ref
	*/
	TInt iInternalStatusRef;
	
	/**
	* Pointer to command buffer. This is created when atomic starts
	*/
	CNSmlDmCommandBuffer* iCommandBuffer;
	};



// ===========================================================================
// MNSmlDmModuleCallBack
// ===========================================================================
/**
* DmModule callback interface. Status and Result information is given through
* this interface to caller of the module (agent)
*
*  @since 
*/
class MNSmlDmModuleCallBack
	{
	public:
	/**
	* Function for setting result in callback interface
	* @param	aResultsRef			Reference to result element
	* @param	aObject				Data
	* @param	aType				Mime type
	* @param	aFormat				Format of the object
	* @param	aTotalSize			Total size
	*/
	virtual void SetResultsL( TInt aResultsRef,
		const CBufBase& aObject,
		const TDesC8& aType,
		const TDesC8& aFormat,
		TInt aTotalSize ) = 0;
		
	/**
	* Function for setting status in callback interface
	* @param	aStatusRef			Reference to result element
	* @param	aStatusCode			Status of command
	*/
	virtual void SetStatusL( TInt aStatusRef, TInt aStatusCode ) = 0;
	};


#endif // __NSMLDMMODULE_H