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



#ifndef __NSMLDMDDF_H
#define __NSMLDMDDF_H

#include <e32std.h>
#include <e32base.h>
#include <ecom.h>
#include <smldmadapter.h>

#include <nsmldmmodule.h>
#include <nsmldmtreedbclient.h>
#include "nsmldmtreedtd.h"
#include "nsmldmhostclient.h"

class CNSmlDmModule;

_LIT8(KNSmlDmFormatB64, "b64");
_LIT8(KNSmlDmFormatBool, "bool");
_LIT8(KNSmlDmFormatChr, "chr");
_LIT8(KNSmlDmFormatInt, "int");
_LIT8(KNSmlDmFormatNode, "node");
_LIT8(KNSmlDmFormatNull, "null");
_LIT8(KNSmlDmFormatXml, "xml");
_LIT8(KNSmlDmFormatBin, "bin");

_LIT8( KNSmlDmLitSeparator8, "/" );
_LIT ( KNSmlTxtPanicDmModule,"NSmlDmModule");

// Constant holding the value of number of slashes in a DM adapter aURI to get
// the first dynamic node.Currently DM adapter will have maximum 4 slashes 
// eg: ./SCM/Inventory/Delivered/<x*>/z ; z is a leaf or internal node,
// Before z four slashes present
const TInt KMaximumNoOfSlashes = 4;

// ===========================================================================
// CNSmlCallbackElement
// ===========================================================================
/**
* Container class for a DM callbacks
*
*  @since
*/
class CNSmlCallbackElement : public CBase
	{
	public:
	~CNSmlCallbackElement();
	HBufC8* iUri;
	TInt iRef;
	TBool iNode;
	};



// ===========================================================================
// CNSmlDmDDF
// ===========================================================================

/**
* DM DDF Handler
*
*  @since
*/
class CNSmlDmDDF : public CBase 
	{
	public:

	enum TAccess 
		{
		EOk,
		ENotAccess,
		ENotExist
		};


	/**
	* Destructor
	*/
	virtual ~CNSmlDmDDF();

	/**
	* Two-phased constructor.
	* @param	aDmModule			Reference to dmModule class
	* @return						Pointer to newly created module instance
	*/
	static CNSmlDmDDF* NewL(CNSmlDmModule& aDmModule);
	
	/**
	* Generates xml of ddf structure
	* @param	aObject				Referencet to buffer where to generata
	*								ddf xml
	*/
	void GenerateDDFL(CBufBase& aObject);

	/**
	* Sets the server identifier
	* @param	aServer				The server id of current session
	*/
	void SetServerL(const TDesC8& aServerId);
	
	/**
	* Creates sessions to Host Servers, asks ddf:s and calculates checksum of
	* all the ddf versions of plug-in adapters
	* @return						Checksum of ddf's of plug-in adapters
	*/
	TInt IsDDFChangedL();
	
	/**
	* Add operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	*/
	void AddObjectL(const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType,const TInt aStatusRef, TBool aLargeItem=EFalse);
		
	/**
	* Replace operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	*/
	void UpdateObjectL(const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType,const TInt aStatusRef, TBool aLargeItem=EFalse,
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
	void FetchObjectL( const TDesC8& aURI, const TDesC8& aType,
		const TInt aResultsRef,const TInt aStatusRef, TBool aAclPass=EFalse);
		
	/**
	* Get size property operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aType			Mime type
	* @param	aResultsRef		Reference to result element
	* @param	aStatusRef		Reference to status element
	*/
	void FetchObjectSizeL( const TDesC8& aURI, const TDesC8& aType,
		const TInt aResultsRef,const TInt aStatusRef);
		
	/**
	* Delete operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aStatusRef		Reference to status element
	*/
	void DeleteObjectL( const TDesC8& aURI, const TInt aStatusRef);
	
	/**
	* Execute operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aObject			Data
	* @param	aType			Mime type
	* @param	aStatusRef		Reference to status element
	* @param	aLargeItem		Indicates if object comes in multiple messages
	*							or not
	*/
	void ExecuteObjectL( const TDesC8& aURI, const TDesC8& aObject,
		const TDesC8& aType, TInt aStatusRef, 		
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
	*/
	void CopyObjectL( const TDesC8& aTargetURI,const TDesC8& aSourceURI,
		const TDesC8& aType, TInt aStatusRef);
		
	/**
	* Check ACL rights.
	* @param	aURI			Target URI in command
	* @param	aCmdType		Command type
	*							or not
	* @return					True if command is allowed by acls
	*/
	TBool CheckAclL(const TDesC8& aURI, TNSmlDmCmdType aCmdType);

	/**
	* Indicates message ending
	*/
	void EndMessageL();
	
	/**
	* Gets and allocates format in DDF
	* @param	aURI			Target URI in command
	* @return					pointer to format descriptor
	*/
	HBufC8* GetFormatAllocL(const TDesC8& aURI);
	
	/**
	* Gets and allocates format in DDF
	* @param	aURI			Target URI in command
	* @param	aObject			Reference to buffer where to write data
	* @return					KErrNotFound if not found, otherwise KErrNone
	*/
	TInt GetMimeTypeL(const TDesC8& aURI,CBufBase& aObject);
	
    
    /**
	* Gets and allocates case sense in DDF
	* @param	aURI			Target URI in command
	* @param	aObject			Reference to buffer where to write data
	* @return					KErrNotFound if not found, otherwise KErrNone
	*/
	
    TInt CNSmlDmDDF::GetCaseSenseL(const TDesC8& aURI,CBufBase& aObject);
   		

	/**
	* Gets and allocates name in DDF
	* @param	aURI			Target URI in command
	* @param	aObject			Reference to buffer where to write data
	* @return					KErrNotFound if not found, otherwise KErrNone
	*/
	TInt GetNameL(const TDesC8& aURI,CBufBase& aObject);
	
	/**
	* Checks if node is leaf or interior node
	* @param	aURI			Target URI in command
	* @return					ENSmlDmDDFLeaf,ENSmlDmDDFNode
	*							or ENSmlDmDDFNotFound
	*/
	TNSmlDmDDFFormat IsLeafL(const TDesC8& aURI);
	
	/**
	* Checks URI and access type and set the member variables point to correct
	* plug-in adapter and correct host server
	* @param	aURI			Target URI in command
	* @param	aCmdType		Command type
	* @return					EOk,ENotAccess or ENotExist
	*/
	TAccess CheckURIL(const TDesC8& aURI,TNSmlDmCmdType aCmd);

	/**
	* Replace ACL operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aACL			ACL data
	* @param	aStatusRef		Reference to status element
	*/
	void UpdateAclL(const TDesC8& aURI, const TDesC8& aACL,
		const TInt aStatusRef);

	/**
	* Get ACL operation in DM protocoll
	* @param	aURI			Target URI in command
	* @param	aACL			Reference to buffer where to write ACL data
	* @return					Status code
	*/
	TInt GetAclL(const TDesC8& aURI, CBufBase& aACL);
	
	/**
	* Get more data in case of getting largeobject
	* @param	aData			Reference to buffer where to write data
	*/
	void MoreDataL(CBufBase*& aData);

	/**
	* "Callback" method. This is called when plug-in adapter makes new
	* new mapping, i.e. maps aLuid to aURI 
	* @param	aURI			Uri to be mapped
	* @param	aLuid			Luid to be mapped
	*/
	void SetMappingL( const TDesC8& aURI, const TDesC8& aLUID );

	/**
	* "Callback" method. This is called when plug-in adapter has given
	* result to get command 
	* @param	aResultRef		Reference information to result element
	* @param	aObject			Result data
	* @param	aType			Mime type of data
	* @param	aTotalSize		Total size of data. In case of large object,
	*							this is different from aObject size.
	* @param	aSessionId		Id of host session, this is given in case of
	*							large object -> module keeps the information
	*							until whole item is read
	*/
	void SetResultsL( TInt aResultsRef, CBufBase& aObject,
		const TDesC8& aType,TInt aTotalSize,TInt8 aSessionId=KErrNotFound);
		
	/**
	* "Callback" method. This is called when plug-in adapter has given
	* status to command 
	* @param	aStatusRef		Reference information to status element
	* @param	aErrorCode		Status code from plug-in adapter
	*/
	void SetStatusL( TInt aStatusRef, CSmlDmAdapter::TError aErrorCode );

	/**
	* Gets and allocs luid mapped to aURI. Allocs KNullDesC8 if not exist.
	* status to command 
	* @param	aURI			Uri to be serched
	* @return					Pointer to allocated luid
	*/
	HBufC8* GetLuidAllocL(const TDesC8& aURI);

	/**
	* Start atomic indication
	*/
	void StartAtomicL();

	/**
	* Commit atomic indication
	*/
	void CommitAtomicL();

	/**
	* Rollback atomic indication
	*/
	void RollbackAtomicL();

	/**
	* Closes the connections to callback server and host servers.
	* Separate function is needed, since the disconnecting cannot 
	* be made after the active scheduler of the thread is stopped.
	*/		
	void DisconnectFromOtherServers();

	// FOTA
	/**
	* When the generic alerts are successfully sent to the remote 
	* server, the FOTA adapter needs to be informed about this.
	* This command is chained through the DM engine.
	*/		
	void MarkGenAlertsSentL();
	// FOTA end
    
    
    /* For 1.2 Any adapter can send the Generic adapter,
    Hence URI of adapter is passed 
    */
    void MarkGenAlertsSentL(const TDesC8& aURI);
    
	private:

	enum TUriCheck 
		{
		EPassAdapter,
		EBeforeAdapter,
		EUriNotFound
		};

	/**
	* Constructor
	*/
	CNSmlDmDDF(CNSmlDmModule& aDmModule);
	
	/**
	* Second phase constructor
	*/
	void ConstructL();
	
	/**
	* Set the member variables point to correct plug-in adapter and correct
	* host server and gets the access type of node
	* @param	aURI			Target URI in command
	* @param	aAccess			Access type is written to this parameter
	* @return					EPassAdapter,EBeforeAdapter or EUriNotFound
	*/
	TUriCheck CheckAdapterL(const TDesC8& aURI, TUint8 &aAccess);
	
	/**
	* Removes end and begin slashes from aObject
	* @param	aObject			Child list data from plug-in adapter
	*/
	void RemoveEndAndBeginSlashes(CBufBase& aObject) const;
	/**
	* Cleans array of delete elements and updates the add node
	* elements array
	* @param	aStatusRef		Reference information to status element
	* @param	aErrorCode		Status code from plug-in adapter
	*/
	void DeleteandAddStatusRefsL( TInt aStatusRef,
			MSmlDmAdapter::TError aErrorCode );
	
	/**
	* Buffers the internal nodes added in a session	
	* @param	aURI		Target URI in command
	* @return 	None
	*/
	void UpdateAddNodeListL(const TDesC8& aURI);	
			
	/**
	* Pointer to tree dtd
	*/
	CNSmlDmMgmtTree* iMgmtTree;
	
	/**
	* Reference to DM Module instance
	*/
	CNSmlDmModule& iDmModule;

	/**
	* Tree database client
	*/
	RNSmlDMCallbackSession iDbSession;
	
	
	/**
	* Array of result elements, used for cleaning the tree when result
	* is got from plug-in adapter
	*/
	RPointerArray<CNSmlCallbackElement> iResultRefs;

	/**
	* Array of delete elements, used for cleaning the tree when
	* succesfull delete is executed
	*/
	RPointerArray<CNSmlCallbackElement> iDeleteStatusRefs;

	/**
	* Array of add node elements, used for setting default acls to
	* node, if the current server does not have acces rigths to it
	*/
	RPointerArray<CNSmlCallbackElement> iAddNodeStatusRefs;

	/**
	* Id of current plug-in adapter
	*/
	TUint32 iAdapterId;
	
	/**
	* Id of current host session
	*/
	TUint8 iSessionId;
	
	/**
	* Id of current host session in case of getting large object
	*/
	TInt8 iResultSessionId;
	
	/**
	* Array of host sessions
	*/
	RArray<RNSmlDMDataSession> iSessionArray;
	
	/**
	* Status of host sessions
	*/
	TUint8 iOkSessions;
	
	/**
	 * Reference of add commands
	 * separated by commas
	 */
	HBufC8* iAddRootNodesList;
	};



// ===========================================================================
// CNSmlDmLinkCallback
// ===========================================================================
/**
* Class for overwriting the callback interface to caller.
* In case of internal commands, the caller (agent) is not called,
* this module is called instead
*
*  @since
*/
class CNSmlDmLinkCallback : public CBase, public MNSmlDmModuleCallBack
	{
	public:

	/**
	* Destructor
	*/
	virtual ~CNSmlDmLinkCallback();

	/**
	* Two-phased constructor.
	* @return						Pointer to newly created module instance
	*/
	static CNSmlDmLinkCallback* NewL();
	
	/**
	* The class is used in internal commands to overwrite the normal
	* callback interface. This methos overwrites the normal
	* SetResultsL function
	* @param	aResultsRef			Reference to result element
	* @param	aObject				Data
	* @param	aType				Mime type
	* @param	aFormat				Format of the object
	* @param	aTotalSize			Total size
	*/
	void SetResultsL( TInt aResultsRef, const CBufBase& aObject,
		const TDesC8& aType, const TDesC8& aFormat,TInt aTotalSize );
		
	/**
	* The class is used in internal commands to overwrite the normal
	* callback interface. This methos overwrites the normal
	* SetStatuslL function
	* @param	aStatusRef			Reference to result element
	* @param	aStatusCode			Status of command
	*/
	void SetStatusL( TInt aStatusRef, TInt aStatusCode );
	
	/**
	* Gets the result
	* @param	aData				Reference to the data buffer, where to
	*								write result data
	* @param	aStatus				Reference to status info wher to write
	*								the status of command execution
	*/
	void GetResultsL( CBufBase*& aData, CSmlDmAdapter::TError& aStatus );

	/**
	* Gets the status
	* @return						Status code
	*/
	TInt GetStatusL();
	
	private:
	/**
	* Constructor
	*/
	CNSmlDmLinkCallback();

	private:
	/**
	* Status code
	*/
	TInt iStatus;

	/**
	* Result
	*/
	CBufBase *iResults;
	};

#endif // __NSMLDMDDF_H