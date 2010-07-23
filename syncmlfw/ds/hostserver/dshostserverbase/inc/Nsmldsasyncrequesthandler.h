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
* Description:  Asynchronous request handlers for DS host server.
*
*/


#ifndef __NSMLDSASYNCREQUESTHANDLER_H__
#define __NSMLDSASYNCREQUESTHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <nsmldsconstants.h>
#include <nsmlconstants.h>

#include "nsmldshostconstants.h"
#include "nsmldshostitem.h"
#include "nsmldsitemmodificationset.h"

// FORWARD DECLARATIONS
class CNSmlDSHostSession;
class CSmlDataStore;
struct TNSmlDSDataStoreElement;
struct TNSmlDSDataProviderElement;
class CNSmlAdapterLog;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// Base class for asynchronous request handlers.
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSAsyncRequestHandler : public CActive
    {
	public: // destructor

		/**
		* C++ default destructor.
		*/	
		virtual ~CNSmlDSAsyncRequestHandler();
		
	public:	// New functions

		/**
		* Initializes the object for asynchronous calls.
		* @return CSmlDataStore&. The asynchronous service provider for the request.
		*/
		CSmlDataStore& CallDSAsyncLC();
		
		/**
		* Pushes item to cleanupstack, item, which sets the request status
		* on leave CleanupStack::PopAndDestroy()
		*/
		void SetRequestStatusOnLeaveLC();  
    
		/**
		* Called when the item pushed in CNSmlDSAsyncCallBackForOpen::SetRequestStatusOnLeaveLC() is 
		* destroyed from the cleanupstack. Sets the request status.
		* @param aP. instance of CNSmlDSAsyncCallBackForOpen.
		*/
		static void SetRequestStatusOnLeaveCleanup( TAny* aP );
		
	public: // Functions from base classes

		/**
		* calls CActive::SetActive().
		*/
		void SetActive();
		
	protected:

		/**
		* A C++ constructor of this class. 
		* @param aPriority. The priority of the active object,
		* @param aSession. The session object of the request.
		* @param aDSItem. contains the data store, which provides the asynchronous service.
		* @param aMessage. The message associated with the request.
		*/
		CNSmlDSAsyncRequestHandler(TInt aPriority,
    		CNSmlDSHostSession* aSession, 
			TNSmlDSDataStoreElement* aDSItem, 
			const RMessagePtr2& aMessage );
        
		/**
		* Adds object to Active scheduler.
		*/
		void AddToSchedulerL();
		
	private:	//Functions from base class

		TInt RunError(TInt aError);
		
	public:		//Data

		CNSmlDSHostSession* iSession;		//session for request.
		TNSmlDSDataStoreElement* iDSItem;	//data store for request
		RMessagePtr2 iMessage;			//message for request.
    };

// ------------------------------------------------------------------------------------------------
// Asynchronous request handler, that calls callback method, when request finishes.
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSAsyncCallBack : public CNSmlDSAsyncRequestHandler
    {
	public:	//DATA TYPES
		enum TCallBackOperation
			{
			EFinished = 0,
			ECanceled,
			EFree
			};
		typedef TInt (CNSmlDSHostSession::*RequestFinishedFunction) 
			(CNSmlDSAsyncCallBack* aDSAO, CNSmlDSAsyncCallBack::TCallBackOperation aOperation);
	public: 	//Constructors and destructors

		/**
		* A C++ constructor of this class.
		* @param aSession. The session object of the request.
		* @param aDSItem. Contains the data store, which provides the asynchronous service.
		* @param aMessage. The message associated with the request.
		* @param aReqFinishedFunc.the call back function this object calls, when request completes.
		* @param aPtr. Pointer to request specific data.
		*/
		CNSmlDSAsyncCallBack( CNSmlDSHostSession* aSession, 
			TNSmlDSDataStoreElement* aDSItem, 
			const RMessage2& aMessage,
			RequestFinishedFunction aReqFinishedFunc = NULL,
			TAny* aPtr = NULL );
		/**
		* C++ destructor
		*/
		virtual ~CNSmlDSAsyncCallBack();

	private: // Functions from base classes

		void DoCancel();
		void RunL();
    
	public:

		TAny* iPtr; // request specific data.

	protected:

		RequestFinishedFunction iReqFinishedFunc;
    };

// ------------------------------------------------------------------------------------------------
// Asynchronous request handler for CSmlDataStore::Open
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------    
class CNSmlDSAsyncCallBackForOpen : public CNSmlDSAsyncCallBack
	{
	public: //Constructor
		/**
		* A C++ constructor of this class. 
		* @param aSession. The session object of the request.
		* @param aMessage. The message associated with the request.
		* @param aReqFinishedFunc. the call back function this object calls, when request completes.
		*/
		CNSmlDSAsyncCallBackForOpen( CNSmlDSHostSession* aSession, 
			const RMessage2& aMessage,
			RequestFinishedFunction aReqFinishedFunc = NULL );        

	public:	//Data

		TNSmlDSDataProviderElement* iDpi;	//Data provider for request
		HBufC* iServerId;
		HBufC* iRemoteDB;
	};

// ------------------------------------------------------------------------------------------------
// Asynchronous request handler for fetching all changed items.
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSChangedItemsFetcher : public CNSmlDSAsyncRequestHandler
	{
	public:	//DATA TYPES
		typedef void ( CNSmlDSHostSession::*RequestFinishedFunction ) 
			( CNSmlDSChangedItemsFetcher* aDSAO );
	public:// Constructor and destructor

		/**
		* Creates new instance of the CNSmlDSChangedItemsFetcher and pushed it to the cleanup stack.
		* @param aSession. The session object of the request.
		* @param aDSItem. Contains the data store, which provides the asynchronous service.
		* @param aMessage. The message associated with the request.
		* @param aReqFinishedFunc. The call back function this object calls, when request completes.
		* @return CNSmlDSChangedItemsFetcher*. Newly created instance.
		*/
		static CNSmlDSChangedItemsFetcher* NewLC( CNSmlDSHostSession* aSession, 
			TNSmlDSDataStoreElement* aDSItem, 
			const RMessage2& aMessage,
			RequestFinishedFunction aReqFinishedFunc );
		
		/**
		* C++ destructor
		*/
		virtual ~CNSmlDSChangedItemsFetcher();
    
	public:	//New methods
    
		/**
		* Fetches all changed items asynchronously. Calls the call back method when done.
		*/
		void FetchAllChangedItemsL();

		/**
		* Returns the changes after they are fetched. 
		* @return RNSmlDbItemModificationSet*. Contains the requested changes.
		*/    
		const RNSmlDbItemModificationSet* ChangedItems() const;
    
	protected:

		/**
		* A C++ constructor of this class. 
		* @param aSession. The session object of the request.
		* @param aDSItem. Contains the data store, which provides the asynchronous service.
		* @param aMessage. The message associated with the request.
		* @param aReqFinishedFunc. The call back function this object calls, when request completes.
		*/
		CNSmlDSChangedItemsFetcher( CNSmlDSHostSession* aSession, 
			TNSmlDSDataStoreElement* aDSItem, 
			const RMessage2& aMessage,
			RequestFinishedFunction aReqFinishedFunc );
        
		/**        
		* Symbian 2nd phase constructor
		*/
		void ConstructL();
    
	private: // Functions from base classes

		virtual void DoCancel();
		virtual void RunL();		
        TInt RunError ( TInt aError );
    
	public:
	protected:
		RequestFinishedFunction iReqFinishedFunc;
		TNSmlDbItemModification::TNSmlDbItemModificationType iNextModsToFetch;
		RNSmlDbItemModificationSet* iChangedItems;
	}; 

#endif //__NSMLDSASYNCREQUESTHANDLER_H__

// End of File
