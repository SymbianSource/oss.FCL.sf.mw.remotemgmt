/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  API ddefinition for generic clients and SDK.
*
*/

#ifndef MAHLEGENERICAPI_H
#define MAHLEGENERICAPI_H

#include <badesca.h>
#include <ahlegeneric.h>

class MAHLEObserver;

class MAHLEGenericAPI
    {
    public:

        /**
        * Destructor.
        */
        inline virtual ~MAHLEGenericAPI();

        /**
        * Check if the client has been connected to the server
        *
        * @return ETrue if connected, otherwise EFalse
        */
        virtual TBool IsConnected() = 0;

        /**
        * Set client observer
        *
        * @param aObserver client observer
        */
        virtual void SetObserverL(const MAHLEObserver* aObserver) = 0;

        /**
        * Cancel client observer
        */
        virtual void CancelObserver() = 0;

        /**
        * Engine reconfiguration.
        * New storage sizes have to be explicitly specified.
        *
        * @param aPrimarySize New primary storage size.
        * @param aSecondarySize New secondary storage size.
        * @param aAdaptationSpeed New adaptation speed.
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if one of parameters is out of range.
        */
        virtual TInt Reconfigure(
                TUint aPrimarySize,
                TUint aSecondarySize,
                TReal32 aAdaptationSpeed) = 0;

        /**
        * Current engine configuration.
        *
        * @param aPrimarySize Current primary storage size.
        * @param aSecondarySize Current secondary storage size.
        * @param aAdaptationSpeed Current adaptation speed.
        */
        virtual TInt GetConfiguration(
                TUint& aPrimarySize,
                TUint& aSecondarySize,
                TReal32& aAdaptationSpeed) = 0;

        /**
        * Logging new access synchronously.
        *
        * @param aItem Access descriptor, key value in adaptive list
        * @param aName Name descriptor (defaults to empty string).
        * @param aWeight Weight of an item (defaults to 1).
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is in use (e.g. reading or writing file) and
        *          KErrArgument if size of one of parameters is out of range.
        */
        virtual TInt NewAccess(
                const TDesC& aItem,
                const TDesC& aName = KAHLEItemDefaultName,
                TReal32 aWeight = KAHLEDefaultWeight) = 0;

        /**
        * Logging new access asynchronously.
        *
        * @param aStatus Asychronous request status.
        * @param aItem Access descriptor, key value in adaptive list
        * @param aName Name descriptor (defaults to empty string).
        * @param aWeight Weight of an item (defaults to 1).
        */
        virtual void NewAccessL(
                TRequestStatus& aStatus,
                const TDesC& aItem,
                const TDesC& aName = KAHLEItemDefaultName,
                TReal32 aWeight = KAHLEDefaultWeight) = 0;

        /**
        * Get adaptive list synchronously.
        *
        * @param aItems Descriptor array containing items in ranked order.
        * @param aNames Descriptor array containing aItems names
        * @param aSize Maximum length of returned list
        * @param aMatch Substring to match beginnings of adaptive list items
        *        (optional, default is to match everything). Matching is case
        *        sensitive.
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if aMatch is to long.
        */
        virtual TInt AdaptiveListL(
                CDesCArray&  aItems,
                CDesCArray&  aNames,
                const TUint  aSize,
                const TDesC& aMatch = KAHLENoMatch) = 0;

        /**
        * Get adaptive list asynchronously.
        *
        * @param aStatus Asynchronous request status.
        * @param aItems Descriptor array containing items in ranked order.
        * @param aNames Descriptor array containing aItems names
        * @param aSize Maximum length of returned list
        * @param aMatch Substring to match beginnings of adaptive list items
        *        (optional, default is to match everything). Matching is case
        *        sensitive.
        */
        virtual void AdaptiveListL(
                TRequestStatus& aStatus,
                CDesCArray& aItems,
                CDesCArray& aNames,
                const TUint aSize,
                const TDesC& aMatch = KAHLENoMatch) = 0;

        /**
        * Remove item synchronously.
        *
        * @param aItem Item to be removed
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file)
        *          and KErrArgument if size of parameter is out of range.
        */
        virtual TInt Remove(const TDesC& aItem) = 0;

        /**
        * Remove item asynchronously.
        *
        * @param aStatus Request status (in asynchronous versions)
        * @param aItem Item to be removed
        */
        virtual void RemoveL(TRequestStatus& aStatus, const TDesC& aItem) = 0;

        /**
        * Remove items synchronously.
        *
        * @param aItems Items to be removed
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if table is empty or one of items in table is
        *          empty.
        */
        virtual TInt RemoveL(const CDesCArray& aItems) = 0;

        /**
        * Remove items asynchronously.
        *
        * @param aStatus Request status
        * @param aItems Items to be removed
        */
        virtual void RemoveL(
                TRequestStatus& aStatus,
                const CDesCArray& aItems) = 0;

        /**
        * Remove matching items synchronously.
        *
        * @param aMatch Match for items to be removed. Matching is case
        *        sensitive.
        * @returns The number of removed items, or otherwise an error code.
        *          Returns KErrInUse if AHLE server is busy (e.g. reading or
        *          writing file) and KErrArgument if match is to long.
        */
        virtual TInt RemoveMatching(const TDesC& aMatch) = 0;

        /**
        * Remove matching items asynchronously.
        *
        * @param aStatus Request status
        * @param aMatch Match for items to be removed. Matching is case
        *        sensitive.
        */
        virtual void RemoveMatchingL(
                TRequestStatus& aStatus,
                const TDesC& aMatch) = 0;

        /**
        * Rename item synchronously
        *
        * @param aItem Item to be renamed
        * @param aNewName New name for the item.
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file)
        *          and KErrArgument if size of one of parameters is to long.
        */
        virtual TInt RenameL(const TDesC& aItem, const TDesC& aNewName) = 0;

        /**
        * Rename item asynchronously
        *
        * @param aItem Item to be renamed
        * @param aNewName New name for the item.
        */
        virtual void RenameL(
                TRequestStatus& aStatus,
                const TDesC& aItem,
                const TDesC& aNewName) = 0;

        /**
        * Clear all adaptive list items synchronously
        *
        * @returns KErrNone if ok, otherwise an error code.
        *          Returns KErrInUse if AHLE server is busy
        *          (e.g. reading or writing file).
        */
        virtual TInt Clear() = 0;

        /**
        * Clear all adaptive list items asynchronously
        *
        * @param aStatus Request status
        */
        virtual void Clear(TRequestStatus& aStatus) = 0;

        /**
        * Flush cached items to persistent storage synchronously
        *
        * @returns KErrNone if ok, otherwise an error code.
        *          Returns KErrInUse if AHLE server is busy
        *          (e.g. reading or writing file).
        */
        virtual TInt Flush() = 0;

        /**
        * Flush cached items to persistent storage asynchronously
        *
        * @param aStatus Request status
        * @returns KErrNone if ok, otherwise an error code.
        *          Returns KErrInUse if AHLE server is busy
        *          (e.g. reading or writing file).
        */
        virtual void Flush(TRequestStatus& aStatus) = 0;

        /**
        * Sorts items by their weights.
        * @param aItems Descriptor array containing items to be sorted.
        * @param aItemsSorted Array containing those indexes of items from
        *        aItems array that are also found from Adaptive History List
        *        Engine ordered by their weight
        *        (in Adaptive History List Engine).
        * @param aSortOrder Sorting items order (ascending or descending)
        * @return KErrNone if ok, otherwise an error code.
        */
        virtual TInt OrderByWeightL(
                const CDesCArray& aItems,
                RArray<TInt>& aItemsSorted,
                TAHLESortOrder aSortOrder = EAHLEAscending) = 0;
    };

#include <ahlegenericapi.inl>

/**
* AHLE Client API constructor
* @param aDatabaseName Name of the database.
* @return MAHLEGenericAPI pointer to AHLE Client API
*/
IMPORT_C MAHLEGenericAPI* NewAHLEClientL(const TDesC& aDatabaseName);

#endif      // MAHLEGENERICAPI_H

// End of File
