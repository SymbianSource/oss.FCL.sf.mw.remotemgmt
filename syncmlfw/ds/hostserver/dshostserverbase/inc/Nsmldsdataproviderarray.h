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
* Description:  array for storing dataproviders and datastores.
*
*/


#ifndef __NSMLDSDATAPROVIDERARRAY_H__
#define __NSMLDSDATAPROVIDERARRAY_H__

// INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include <nsmldsconstants.h>
#include <nsmlconstants.h>

#include "nsmldshostconstants.h"
#include "nsmldshostitem.h"
#include "nsmldsitemmodificationset.h"

// FORWARD DECLARATIONS
class CSmlDataProvider;
class CSmlDataStore;
class CNSmlDSAsyncRequestHandler;
class CNSmlAdapterLog;
class CSmlDataStoreFormat;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// Container for CSmlDataStore.
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
typedef RPointerArray<TSmlDbItemUid> RBatchItemUidArray;

struct TNSmlDSDataStoreElement
    {
	public:
		/**
		* deletes iBatchItemUids and its objects.
		*/
		void FreeBatchItemArray();
    
		/**
		* is iDataStore opened.
		* @return TBool. ETrue if iDataStore is opened, EFalse otherwise.
		*/
		TBool IsOpen() const;
    
		/**
		* sets iDataStore as opened.
		*/
		void SetOpened();
		
		/**
		* The name of iDataStore.
		* @return const TDesC&. The name of iDataStore
		*/	
		const TDesC& StoreName() const;
		
		void setStoreFormat( CSmlDataStoreFormat* aDSFormat );
	private:

		/**
		* C++ default constructor.
		*/
		TNSmlDSDataStoreElement( const HBufC* aStoreName );
		
	public:		//Data

		//This class doesn't free memory reserved for member variable pointers.
		
		CNSmlDSAsyncRequestHandler* iDSAO;	//async handler for the data store. 
    										//Null if no asynch call outstanding
		CSmlDataStore* iDataStore;			//contained data store
		RBatchItemUidArray* iBatchItemUids;	//array of item uid when in batch mode.
		CNSmlAdapterLog* iAdpLog;			//sync relationship
		TSmlDbItemUid iCreatedUid;			//uid for CSmlDataStore::CreateItemL
		TNSmlHostMode iHostMode;			//data store mode
	private:	//Data

		const HBufC* iStoreName; //Only temporary. Contains store name only when created but not yet opened. 
		CSmlDataStoreFormat* iDSFormat;		//data store format.
	private:

		friend class RNSmlDSDataProviderArray;	
    };
    
// ------------------------------------------------------------------------------------------------
// Container for CSmlDataProvider.
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
typedef RPointerArray<TNSmlDSDataStoreElement> RDataStoreElementArray;

struct TNSmlDSDataProviderElement
    {
    //This class doesn't free memory reserved for member variable pointers.
    CSmlDataProvider* iDataprovider;
    RDataStoreElementArray* iDataStores;	//created data stores for data provider.
    };

// ------------------------------------------------------------------------------------------------
// Array for created data providers and created and opened data stores. 
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class RNSmlDSDataProviderArray : protected RPointerArray<TNSmlDSDataProviderElement>
    {
	public: //Constructor and destructor

		/**
		* A C++ constructor of this class. 
		*/
		RNSmlDSDataProviderArray();

	public:
    
		/**
		* Destroys all the the elements and resets the array.
		*/
		void ResetAndDestroy();
		
    	/**
		* Finds and returns the dataprovider using data provider id. 
		* @param aId. Search key.
		* @return CSmlDataProvider*. The object found or NULL if no match.
		*/
		CSmlDataProvider* DataProvider( TSmlDataProviderId aId );
    
		/**
		* Finds and returns the dataprovider element using data provider id. 
		* @param aId. Search key.
		* @return TNSmlDSDataProviderElement*. The object found or NULL if no match.
		*/
		TNSmlDSDataProviderElement* DataProviderElement( TSmlDataProviderId aId );
    
		/**
		* Finds and returns the datastore using data provider id and store name. 
		* @param aId. Search key. data provider id
		* @param aId. Search key. Store name.
		* @return CSmlDataStore*. The object found or NULL if no match.
		*/
		CSmlDataStore* DataStore( TSmlDataProviderId aId, const TDesC& aStoreName );
    
		/**
		* Finds and returns the datastore element from data provider element 
		* using data provider id and store name. 
		* @param aId. Search key. data provider id
		* @param aId. Search key. Store name.
		* @return TNSmlDSDataStoreElement*. The object found or NULL if no match.
		*/
		static TNSmlDSDataStoreElement* DataStoreElement( const TNSmlDSDataProviderElement* aItem, 
    		const TDesC& aStoreName );
    
		/**
		* Inserts data provider to the array. Leaves if fails.
		* @param aDataprovider. Data provider to add.
		* @return TInt. KErrNone if object is inserted.
		*               KErrAlreadyExists if object is already inserted.
		*/
		TInt InsertL( CSmlDataProvider* aDataprovider );
    
		/**
		* Inserts created data store to data provider element.
		* @param aDataproviderItem. object where to add.
		* @param aDataStore. datastore to add.
		* @param aDataStoreName. store name for aDataStore.
		* @return TNSmlDSDataStoreElement*. Newly created data store element, that contains aDataStore.
		*/    
		TNSmlDSDataStoreElement* InsertL( TNSmlDSDataProviderElement* aDataproviderItem, 
    							CSmlDataStore* aDataStore, HBufC* aDataStoreName );

		/**
		* Removes and frees the memory reserved to data store element. 
		* Doesn't free the returned object. Caller must take the ownership of that.
		* @param aDataproviderItem. data provider element where to remove.
		* @param aDataStoreItem. data store to remove.
		* @return CNSmlDSAsyncRequestHandler*. Request handler for aDataStoreItem. 
		* NULL if no asynchronous request is outstanding.
		*/    						
		CNSmlDSAsyncRequestHandler* Remove( TNSmlDSDataProviderElement* aDataproviderItem, 
    		TNSmlDSDataStoreElement* aDataStoreItem );
    		
	protected:

		/**
		* Destroys data store element.
		* @param aDataStoreItem. data store element to destroy. After destruction points to NULL.
		* @return CNSmlDSAsyncRequestHandler*. Request handler for aDataStoreItem. 
		* NULL if no asynchronous request is outstanding.
		*/
		static CNSmlDSAsyncRequestHandler* Destroy( TNSmlDSDataStoreElement*& aDataStoreItem );
    };    

#endif //__NSMLDSDATAPROVIDERARRAY_H__

// End of File
