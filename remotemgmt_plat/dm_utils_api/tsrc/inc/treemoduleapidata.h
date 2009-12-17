/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
#include <cntdb.h>
#include <cntitem.h>
#include <s32file.h>
#include <s32mem.h>
#include <f32file.h>

//#include <nsmldmmodule.h>
//#include "test.h"
//#include "nsmldebug.h"
#include <e32def.h>
#include <utf.h>
//#include <smldmadapter.h>

#include <smldmadapter.h>

enum NSmlCmdType
	{
	ETestAdd,
	ETestGet,
	ETestDelete,
	ETestReplace,
	ETestExec,
	ETestCopy
	};

class CNSmlTestElement: public CBase
	{
	public:
	CNSmlTestElement(TDesC8& aUri,TInt aStatusRef, NSmlCmdType aCmdType);
	CNSmlTestElement(TDesC8& aUri,TDesC8& aData,TInt aStatusRef, NSmlCmdType aCmdType);
	CNSmlTestElement(TDesC8& aUri,TInt aStatusRef,TInt aResultRef, NSmlCmdType aCmdType);
	~CNSmlTestElement();
	NSmlCmdType iCmdType;
	HBufC8* iUri;
	HBufC8* iData;
	TInt iStatusRef;
	TInt iResultRef;
	};


// ===========================================================================
// CNSmlDmCallback
// ===========================================================================
/*
class CNSmlDmCallbackTest : public CBase, public MNSmlDmModuleCallBack
	{
	public:
	CNSmlDmCallbackTest();
	~CNSmlDmCallbackTest();
	static CNSmlDmCallbackTest* NewL();
	void SetResultsL(TInt aResultsRef, const CBufBase& aObject, const TDesC8& aType, const TDesC8& aFormat,TInt aTotalSize );
	void SetStatusL( TInt aStatusRef, TInt aStatusCode );
	void SetServerIdL(TDesC8& aServerId);
	void StartAtomicL();
	void EndAtomicL();
	void RollBackL();
	// FOTA 
	void MarkGenAlertsSent();
	// FOTA end
	RPointerArray<CNSmlTestElement> iArray;
	HBufC8* iServerId;
	//RFs iFs;
//	RFile iFile;
	TInt iStatuscode;
	TBuf8<50> iObject;

	};

*/

// ===========================================================================
// CNSmlDmCallback
// ===========================================================================
/*class CNSmlDmTreeTest : CActive
	{
	public:
	CNSmlDmTreeTest();
	~CNSmlDmTreeTest();
	static CNSmlDmTreeTest* NewL();
	void RunL();
	void DoCancel();
	void StartL();
	private:
	TInt iLoop;
	};

*/

class CMSmlDmCallbackTest : public CBase, public MSmlDmCallback
/**
This class is callback implements MSmlDmCallback interface which is implemented in DM Module. An
instance is passed by reference as an argument to the CSmlDmAdapter::NewL()
function. This interface is mostly used for returning results and status
codes for completed commands to the DM framework. The interface also has
functionality for mapping LUIDs and fetching from other parts of the DM Tree.
The adapter does not necessarily need to use any other functions but the
SetStatusL and SetResultsL, if it handles the LUID mapping itself.
@publishedPartner
@prototype
*/
	{
	public:
	CMSmlDmCallbackTest();
	~CMSmlDmCallbackTest();
	static CMSmlDmCallbackTest* NewL();
	/**
	The function is used to return the data in case of FetchLeafObjectL(),
	FetchLeafObjectSizeL() and ChildURIListL() functions. It should not be
	called where the DM command has failed, i.e. the error code returned in
	SetStatusL is something other than EOk.
	@param aResultsRef	Reference to correct command
	@param aObject		The data which should be returned
	@param aType			MIME type of the object
	@publishedPartner
	@prototype
	*/
	void SetResultsL( TInt aResultsRef, CBufBase& aObject,
							  const TDesC8& aType );
	
	/**
	The function is used to return the data in case of FetchLeafObjectL() and
	ChildURIListL() functions, where the size of the data being returned is
	large enough for the Adapter to stream it. This function should not be
	called when command was failed, i.e. the error code returned in SetStatusL
	is something other than EOk.
	@param aResultsRef	Reference to correct command
	@param aStream		Large data which should be returned, DM engine
							closes stream when it has read all the data
	@param aType			MIME type of the object
	@publishedPartner
	@prototype
	*/
	void SetResultsL( TInt aResultsRef, RReadStream*& aStream,
							  const TDesC8& aType );

	/**
	The function returns information about the Add,Update,Delete and Fetch
	commands success to DM engine. The reference to correct command must be
	used when calling the SetStatusL function, the reference is got from the
	argument of the command functions. The SetStatusL function must be called
	separately for every single command.
	@param aStatusRef	Reference to correct command
	@param aErrorCode	Information about the command success
	@publishedPartner
	@prototype
	*/
	void SetStatusL( TInt aStatusRef,
							 MSmlDmAdapter::TError aErrorCode );

	/**
	The function passes map information to DM Module. This function is called
	for a new management object, both for node objects and for leaf objects.
	In addition if ChildURIListL() function has returned new objects a mapping
	information of the new objects must be passed. A mapping is treated as
	inheritable. If the mapping is not set with this function, the mapping
	LUID of the parent object is passed in following commands to the object.
	@param aURI	URI of the object. 
	@param aLUID	LUID of the object. LUID must contain the all information,
					which is needed for retrieve the invidual object from the
					database. Typically it is ID for the database table. In
					more complicated structures it can be combination of IDs,
					which represent path to the object.
	@publishedPartner
	@prototype
	*/
	void SetMappingL( const TDesC8& aURI, const TDesC8& aLUID );

	/**
	The function is used to make a fetch to other adapters. The most common
	use is to make a fetch to the AP adapter, because when managing the access
	points, the data comes as URI. For example, there are ToNAPId field in
	some adapters, and data to it can be something like AP/IAPidx, and then
	the link to AP adapter is needed.
	Using FetchLinkL causes the DM Framework to make a Get request to the
	appropriate DM adapter.  The receiving adapter MUST complete the Get
	request synchronously.
	@param aURI		URI of the object. 
	@param aData		Reference to data, i.e. data is returned here
	@param aStatus	The status of fetch command is returned here
	@publishedPartner
	@prototype
	*/
	void FetchLinkL( const TDesC8& aURI, CBufBase& aData,
							 MSmlDmAdapter::TError& aStatus );

	/**
	The function returns the LUID which is mapped to aURI. If LUID is not
	found, the function allocates a null length string, i.e. the function
	allocates memory in every case.
	@param aURI	URI of the object. 
	@publishedPartner
	@prototype
	*/
	HBufC8* GetLuidAllocL( const TDesC8& aURI );
	
	RReadStream iStream;
	HBufC8* iServerId;
	//RFs iFs;
//	RFile iFile;
	TInt iStatuscode;
	TBuf8<50> iObject;

	};

