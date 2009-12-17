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


#ifndef __NSMLDMCOMMANDBUFFER_H
#define __NSMLDMCOMMANDBUFFER_H

#include <e32std.h>
#include <e32base.h>
#include <smldmadapter.h>
#include <nsmldmtreedbclient.h>
#include "nsmldmtreedtd.h"

class MNSmlDmModuleCallBack;
class CNSmlDmModule;
class CNSmlDmLinkCallback;
class CNSmlDmDDF;

_LIT8(KNSmlDmProperty, "?prop=" );

// ===========================================================================
// Commands
// ===========================================================================
/**
* Container class for a DM command buffering.
* This class is instantiated for each command
* inside the atomic. The elements are in linked list
*
*  @since 
*/
class CNSmlDmCommands : public CBase
	{
	
	public:
	/**
	* Pointer to next elelmnt
	*/
	CNSmlDmCommands *iNext;
	
	/**
	* URI of the command
	*/
	HBufC8* iURI;
	
	/**
	* Target URI of the command in case of copy
	*/
	HBufC8* iTargetURI;

	/**
	* Mime type of the item
	*/
	HBufC8* iType;

	/**
	* Data of the item in case of add, replace or execute
	*/
	HBufC8* iObject;

	/**
	* Type of command (Add,Get,...)
	*/
	TNSmlDmCmdType iCmdType;

	/**
	* Reference to status element
	*/
	TInt iStatusRef;

	/**
	* Reference to result element
	*/
	TInt iResultsRef;

	/**
	* Status code
	*/
	TInt iStatusCode;

	/**
	* Result
	*/
	CBufBase* iResult;
	
	/**
	* Format
	*/
	HBufC8* iFormat;

	/**
	* Indicates that the status is got to current command
	*/
	TBool iGotStatus;

	/**
	* Pointer to callback
	*/
	CNSmlDmLinkCallback *iCallback;

	public:
	
	/**
	* Constructor
	*/
	CNSmlDmCommands();

	/**
	* Destructor
	*/
	virtual ~CNSmlDmCommands();
	};


// ===========================================================================
// CNSmlDmCommandBuffer
// ===========================================================================
/**
* Class for handling bufered commands inside the atomic
*
*  @since 
*/
class CNSmlDmCommandBuffer : public CBase
	{
	public:

	/**
	* Two-phased constructor.
	* @return						Pointer to newly created module instance
	*/
	static CNSmlDmCommandBuffer* NewL(CNSmlDmModule& aDmModule); 

	/**
	* Destructor
	*/
	virtual ~CNSmlDmCommandBuffer();
	

	/**
	* Add operation in DM protocoll for buffering
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	*/
	void AddObjectL(const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType, TInt aStatusRef );
		
	/**
	* Replace operation in DM protocoll for buffering
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	*							or not
	*/
	void UpdateObjectL( const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType, TInt aStatusRef);

	/**
	* Delete operation in DM protocoll for buffering
	* @param	aURI			Target URI in command
	* @param	aStatusRef		Reference to status element
	*/
	void DeleteObjectL(const TDesC8& aURI, TInt aStatusRef );
	
	/**
	* Get operation in DM protocoll for buffering
	* @param	aURI			Target URI in command
	* @param	aType			Mime type
	* @param	aResultsRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	*/
	void FetchObjectL(const TDesC8& aURI, const TDesC8& aType,
		TInt aResultsRef, TInt aStatusRef );
		
	/**
	* Execute operation in DM protocoll for buffering
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	*/
	void ExecuteObjectL( const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType, TInt aStatusRef);
		
	/**
	* Copy operation in DM protocoll for buffering
	* @param	aTargetURI		Target URI in command
	* @param	aSourceURI		Source URI in command
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	*/
	void CopyObjectL( const TDesC8& aTargetURI,const TDesC8& aSourceURI,
		const TDesC8& aType, TInt aStatusRef);
		

	/**
	* Commits the atomic, i.e. executed the commands in the buffer
	* @param	aDDF		Reference to DDF module
	*/
	void CommitL( CNSmlDmDDF& aDDF);

	/**
	* Rollbacks the atomic, i.e. rollbacks the commands in the buffer
	* @param	aDDF		Reference to DDF module
	*/
	void RollBackL(CNSmlDmDDF& aDDF);
	
	/**
	* Check if this command has came inside atomic
	* @param	aStatusRef		Reference to status element
	* @return					ETrue if status ref is found from buffer
	*/
	TBool CheckStatusRef(TInt aStatusRef);

	/**
	* Check if this command has came inside atomic
	* @param	aResultRef		Reference to result element
	* @return					ETrue if resultref is found from buffer
	*/
	TBool CheckResultsRef(TInt aResultsRef);

	/**
	* Sets the status of command pointed by aStatusRef
	* @param	aStatusRef		Reference to status element
	* @param	aStatusCode		Status code to be set
	*/
	void SetStatus(TInt aStatusRef, TInt aStatusCode);
	
	/**
	* Sets the result of command pointed by aResultRef
	* @param	aResultRef		Reference to result element
	* @param	aObject			Result data to be set
	* @param	aType			Mime type of the object
	* @param	aFormat			Format of the node
	*/
	void SetResultsL(TInt aResultsRef, const CBufBase& aObject,
		const TDesC8& aType, const TDesC8& aFormat );
	
	/**
	* check    whether the atomic command is with Get
	* @param	 None
	* @return	ETrue if the atomic command is with Get or else EFalse
	*/	
	TBool IsGetWithAtomic();
	
	private:
	
	/**
	* Sends the statuses and results to caller from the command buffer.
	*/
	void SendStatusAndResultCodesL();

	/**
	* Check if parent node is deleted because of failure in atomic
	* @param	aURI		URI of the node
	* @return				ETrue if parent of node is deleted 
	*/
	TBool ParentNodeDeletedL(const TDesC8& aURI);

	/**
	* Constructor
	* @param	aDmModule		Reference to caller
	*/
	CNSmlDmCommandBuffer(CNSmlDmModule& aDmModule);

	/**
	* Atomic failure checking
	* @return	ETrue if some command failed or status missing 
	*/
	TBool AtomicFailed();

	/**
	* Change statuses to correct atomic statuses.
	*/
	void ChangeAtomicStatuses();

	private:
	CNSmlDmModule& iDmModule;
	CNSmlDmCommands* iCommandBuffer;
	TBool iAtomicFailed;
	};





#endif // __NSMLDMCOMMANDCUFFER_H