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
* Description:  API definition for dedicated clients.
*
*/

#ifndef MAHLEDEDICATEDAPI_H
#define MAHLEDEDICATEDAPI_H

#include <badesca.h>
#include <ahlededicated.h>

class MAHLEObserver;

/**
*  AHLE API for dedicated clients.
*  Provides API interface for dedicated clients
*
*  @lib AHLE2CLIENT.lib
*  @since Series 60 3.1
*/
class MAHLEDedicatedAPI
    {
    public:

        /**
        * Destructor.
        */
        inline virtual ~MAHLEDedicatedAPI();

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
        * Used by Browser, Emails, Logs, Messaging, Phonebook
        *
        * @param aItem Access descriptor, key value in adaptive list
        *              Browser - url. URLs should follow URI Generic Syntax
        *                          described in RFC 2396
        *              Emails - email address
        *              Logs - telephone number
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aDescription specific information about aItem
        *              Browser - name (e.g. Content of WML/HTML title tag)
        *                  (defaults to empty string)
        *              Emails - defaults to empty string
        *              Logs - defaults to empty string
        *              Messaging - telephone number or e-mail address
        *                          (second part of key value)
        *              Phonebook - telephone number (second part of key value)
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is in use (e.g. reading or writing file) and
        *          KErrArgument if size of one of parameters is out of range.
        */
        virtual TInt NewAccess(
                const TDesC& aItem,
                const TDesC& aDescription = KAHLEItemDefaultDescription) = 0;

        /**
        * Logging new access asynchronously.
        * Used by Browser, Emails, Logs, Messaging, Phonebook
        *
        * @param aStatus Asychronous request status.
        * @param aItem Access descriptor, key value in adaptive list
        *              Browser - url. URLs should follow URI Generic Syntax
        *                          described in RFC 2396
        *              Emails - email address
        *              Logs - telephone number
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aDescription Name or telephone number
        *              Browser - name (e.g. Content of WML/HTML title tag)
        *                        (defaults to empty string)
        *              Emails - defaults to empty string
        *              Logs - defaults to empty string
        *              Messaging - telephone number or e-mail address
        *                          (second part of key value)
        *              Phonebook - telephone number (second key value)
        */
        virtual void NewAccessL(
                TRequestStatus& aStatus,
                const TDesC& aItem,
                const TDesC& aDescription = KAHLEItemDefaultDescription) = 0;

        /**
        * Get adaptive list synchronously. Used by Emails and Logs.
        *
        * @param aItems Descriptor array containing items in ranked order.
        *              Emails - email addresses
        *              Logs - telephone numbers
        * @param aSize Maximum length of returned list
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file).
        */
        virtual TInt AdaptiveListL(
                CDesCArray&  aItems,
                const TUint  aSize) = 0;

        /**
        * Get adaptive list asynchronously. Used by Emails and Logs.
        *
        * @param aStatus Asynchronous request status.
        * @param aItems Descriptor array containing items in ranked order.
        *              Emails - email addresses
        *              Logs - telephone numbers
        * @param aSize Maximum length of returned list
        */
        virtual void AdaptiveListL(
                TRequestStatus& aStatus,
                CDesCArray& aItems,
                const TUint aSize) = 0;

        /**
        * Get adaptive list synchronously.
        * Used by Browser, Messaging and Phonebook.
        * For Browser there is an option of selecting sites/groups or details
        * (e.g. All the urls within a site or members of a group) for a
        * site/group. In case of auto-complete, parameter
        * EAHLEAdaptiveAutoComplete as aState should be used.
        *
        * @param aItems Descriptor array containing key items in ranked order
        *              Browser - array of URLs
        *              Messaging - array of linkIDs
        *              Phonebook - array of linkIDs
        * @param aDescription Descriptor array containing names or telephone
        *        numbers in ranked order
        *              Browser - array of URL names
        *              Messaging - array of telephone numbers or
        *                          e-mail addresses (indirectly thrue phonebook)
        *              Phonebook - array of telephone numbers
        * @param aSize Maximum length of returned list
        * @param aMatch
        *              Browser - Substring to match beginnings of adaptive list
        *                        items. Matching is case insensitive.
        *              Messaging - default value
        *              Phonebook - default value
        * @param aState
        *              Browser - Details of adaptive list items - possible
        *                         values:
        *                  EAHLEAdaptiveSiteList,
        *                  EAHLEAdaptiveSiteDetails and
        *                  EAHLEAdaptiveAutoComplete.
        *              Messaging - default value
        *              Phonebook - default value
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if size of one of parameters is incorrect.
        */
        virtual TInt AdaptiveListL(
                CDesCArray&  aItems,
                CDesCArray& aDescription,
                const TUint aSize,
                const TDesC& aMatch = KAHLENoMatch,
                const TAHLEState aState = EAHLEAdaptiveSiteList) = 0;

        /**
        * Get adaptive list asynchronously.
        * Used by Browser, Messaging and Phonebook.
        * For Browser there is an option of selecting sites/groups or details
        * (e.g. All the urls within a site or members of a group) for a
        * site/group. In case of auto-complete, parameter
        * EAHLEAdaptiveAutoComplete as aState should be used.
        *
        * @param aStatus Asynchronous request status.
        * @param aItems Descriptor array containing key items in ranked order
        *              Browser - array of URLs
        *              Messaging - array of linkIDs
        *              Phonebook - array of linkIDs
        * @param aDescription Descriptor array containing names or telephone
        *        numbers in ranked order
        *              Browser - array of URL names
        *              Messaging - array of telephone numbers or
        *                          e-mail addresses (indirectly thrue phonebook)
        *              Phonebook - array of telephone numbers
        * @param aSize Maximum length of returned list
        * @param aMatch
        *              Browser - Substring to match beginnings of adaptive list
        *                        items. Matching is case insensitive.
        *              Messaging - default value
        *              Phonebook - default value
        * @param aState
        *              Browser - Details of adaptive list items - possible
        *                         values:
        *                  EAHLEAdaptiveSiteList,
        *                  EAHLEAdaptiveSiteDetails and
        *                  EAHLEAdaptiveAutoComplete.
        *              Messaging - default value
        *              Phonebook - default value
        */
        virtual void AdaptiveListL(
                TRequestStatus& aStatus,
                CDesCArray&  aItems,
                CDesCArray&  aDescription,
                const TUint  aSize,
                const TDesC& aMatch = KAHLENoMatch,
                const TAHLEState aState = EAHLEAdaptiveSiteList) = 0;

        /**
        * Remove item synchronously.
        * Used by Browser, Emails, Logs, Messaging, Phonebook
        *
        * @param aItem Item to be removed
        *              Browser - a url
        *              Emails - an email
        *              Logs - a telephone number
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aPhoneNumber telephone number to be removed from a contact
        *              Browser - defaults to empty string
        *              Emails - defaults to empty string
        *              Logs - defaults to empty string
        *              Messaging - telephone number or e-mail address
        *              Phonebook - telephone number
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if size of one of parameters is to long.
        */
        virtual TInt Remove(
                const TDesC& aItem,
                const TDesC& aPhoneNumber = KAHLEItemDefaultDescription) = 0;

        /**
        * Remove item asynchronously.
        * Used by Browser, Emails, Logs, Messaging, Phonebook
        *
        * @param aStatus Request status (in asynchronous versions)
        * @param aItem Item to be removed
        *              Browser - a url
        *              Emails - an email
        *              Logs - a telephone number
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aPhoneNumber telephone number to be removed from a contact
        *              Browser - defaults to empty string
        *              Emails - defaults to empty string
        *              Logs - defaults to empty string
        *              Messaging - telephone number or e-mail address
        *              Phonebook - telephone number
        */
        virtual void RemoveL(
                TRequestStatus& aStatus,
                const TDesC& aItem,
                const TDesC& aPhoneNumber = KAHLEItemDefaultDescription) = 0;

        /**
        * Remove items synchronously.
        * Used by Browser, Emails, Logs
        *
        * @param aItems Items to be removed
        *              Browser - array of urls
        *              Emails - array of emails
        *              Logs - array of telephone numbers
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if table is empty or one of items in table is
        *          empty.
        */
        virtual TInt RemoveL(const CDesCArray& aItems) = 0;

        /**
        * Remove items asynchronously.
        * Used by Browser, Emails, Logs
        *
        * @param aStatus Request status
        * @param aItems Items to be removed
        *              Browser - array of urls
        *              Emails - array of emails
        *              Logs - array of telephone numbers
        */
        virtual void RemoveL(
                TRequestStatus& aStatus,
                const CDesCArray& aItems) = 0;


        /**
        * Remove items synchronously.
        * Used by Messaging, Phonebook
        *
        * @param aItems Array of linkIDs to be removed
        * @param aPhoneNumbers Array of telephone numbers or/and
        *                      e-mail addresses to be removed
        *        from a contact.
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file)  and
        *          KErrArgument if table is empty or one of items in table is
        *          empty.
        */
        virtual TInt RemoveL(
                const CDesCArray& aItems,
                const CDesCArray& aPhoneNumbers) = 0;

        /**
        * Remove items asynchronously.
        * Used by Messaging, Phonebook
        *
        * @param aStatus Request status
        * @param aItems Array of linkIDs to be removed
        * @param aPhoneNumbers Array of telephone numbers or/and
        *                      e-mail addresses to be removed
        */
        virtual void RemoveL(
                TRequestStatus& aStatus,
                const CDesCArray& aItems,
                const CDesCArray& aPhoneNumbers) = 0;

        /**
        * Remove matching items synchronously.
        * Used by Browser, Messaging, Phonebook
        *
        * @param aMatch Match for items to be removed.
        *              Browser - matching string. Removes all items that match
        *                  the given string. Matching is case sensitive.
        *              Messaging - linkID. Removes all items corresponding to
        *                  the linkID
        *              Phonebook - linkID. Removes all items corresponding to
        *                  the linkID
        * @returns The number of removed items, or otherwise an error code.
        *          Returns KErrInUse if AHLE server is busy (e.g. reading or
        *          writing file) and KErrArgument if match is to long.
        */
        virtual TInt RemoveMatching(const TDesC& aMatch) = 0;

        /**
        * Remove matching items asynchronously.
        * Used by Browser, Messaging, Phonebook
        *
        * @param aStatus Request status
        * @param aMatch Match for items to be removed.
        *              Browser - matching string. Removes all items that match
        *                  the given string. Matching is case sensitive.
        *              Messaging - linkID. Removes all items corresponding to
        *                  the linkID
        *              Phonebook - linkID. Removes all items corresponding to
        *                  the linkID
        */
        virtual void RemoveMatchingL(
                TRequestStatus& aStatus,
                const TDesC& aMatch) = 0;

        /**
        * Rename item synchronously
        * Used by Browser, Messaging, Phonebook
        *
        * @param aItem Item to be renamed
        *              Browser - a url
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aNewDescription New description.
        *              Browser - new name of url
        *              Messaging - new telephone number or e-mail address
        *              Phonebook - new telephone number
        * @param aDescription  Description to be renamed.
        *              Browser - defaults to empty string
        *              Messaging - old telephone number or e-mail address
        *              Phonebook - old telephone number
        * @returns KErrNone if ok, otherwise an error code. Returns KErrInUse
        *          if AHLE server is busy (e.g. reading or writing file) and
        *          KErrArgument if size of one of parameters is to long.
        */
        virtual TInt RenameL(
                const TDesC& aItem,
                const TDesC& aNewDescription,
                const TDesC& aDescription = KAHLEItemDefaultDescription) = 0;

        /**
        * Rename item asynchronously
        * Used by Browser, Messaging, Phonebook
        *
        * @param aStatus Request status
        * @param aItem Item to be renamed
        *              Browser - a url
        *              Messaging - linkID
        *              Phonebook - linkID
        * @param aNewDescription New description.
        *              Browser - new name of url
        *              Messaging - new telephone number or e-mail address
        *              Phonebook - new telephone number
        * @param aDescription  Description to be renamed.
        *              Browser - defaults to empty string
        *              Messaging - old telephone number or e-mail address
        *              Phonebook - old telephone number
        */
        virtual void RenameL(
                TRequestStatus& aStatus,
                const TDesC& aItem,
                const TDesC& aNewDescription,
                const TDesC& aDescription = KAHLEItemDefaultDescription) = 0;

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
    };

#include "ahlededicatedapi.inl"

/**
* AHLE Client API constructor
* @param aClientType AHLE client type
* @return MAHLEDedicatedAPI pointer to AHLE Client API
*/
IMPORT_C MAHLEDedicatedAPI* NewAHLEClientL(TAHLEClientType aClientType);

#endif      // MAHLEDEDICATEDAPI_H

// End of File
