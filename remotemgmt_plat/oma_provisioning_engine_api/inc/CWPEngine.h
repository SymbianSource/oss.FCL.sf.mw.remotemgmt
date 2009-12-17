/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Engine class for Provisioning.
*
*/


#ifndef CWPENGINE_H
#define CWPENGINE_H

// INCLUDES
#include <e32base.h>
#include <s32std.h>

// FORWARD DECLARATIONS
class CWPRoot;
class MWPContextManager;
class MWPBuilder;
class MWPPairVisitor;
class MWPVisitor;
class CWPAdapterManager;
class MDesC16Array;
class CDesC16Array;
class MWPContextObserver;

// CONSTANTS
const TUint32 KWPUidNoContext = 0xffffffff;

// CLASS DECLARATION

/**
*  CWPEngine is an engine class used by Provisioning components.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class CWPEngine : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPEngine* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPEngine* NewLC();

        /**
        * Destructor.
        */
        ~CWPEngine();

    public: // New methods

        /**
        * Accepts a visitor for the contents of the document.
        * @param aVisitor Visitor is called for each element in the root of the document.
        */
        IMPORT_C void AcceptL(MWPVisitor& aVisitor);

        /**
        * Imports a WBXML document to the engine.
        * @param Document to import
        */
        IMPORT_C void ImportDocumentL(const TDesC8& aDocument);

        /**
        * Populates the adapters with the document contents.
        */
        IMPORT_C void PopulateL();

        /**
        * Restores the document data from a store.
        * @param aStore The store to read from.
        * @param aId The stream id to read
        */
        IMPORT_C void RestoreL(CStreamStore& aStore, TStreamId aId);
        
        /**
        * Stores the document data to a store.
        * @param aStore The store to write to.
        * @return The stream id
        */
        IMPORT_C TStreamId StoreL(CStreamStore& aStore) const;
        
        /**
        * Loads data from a stream.
        * @param aStream Stream to read from.
        */
        IMPORT_C void InternalizeL(RReadStream& aStream);
        
        /**
        * Stores data to a stream.
        * @param aStream Stream to write to.
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
        * Returns the number of settings items.
        * @return Number of saveable settings items
        */
        IMPORT_C TInt ItemCount() const;

        /**
        * Returns a summary title.
        * @param aIndex Number of the setting item to read
        * @return The summary title for the item
        */
        IMPORT_C const TDesC16& SummaryTitle(TInt aIndex) const;
        
        /**
        * Returns a summary text.
        * @param aIndex Number of the setting item to read
        * @return The summary text for the item
        */
        IMPORT_C const TDesC16& SummaryText(TInt aIndex) const;
        
        /**
        * Calls a visitor for each details line of a particular item.
        * @param aIndex The number of the item
        * @param aVisitor The visitor to call
        * @return KErrNotSupported if not supported
        */
        IMPORT_C TInt DetailsL(TInt aIndex, MWPPairVisitor& aVisitor);
        
        /**
        * Saves one setting.
        * @param aItem Setting number
        */
        IMPORT_C void SaveL( TInt aItem );

        /**
        * Checks if the setting can be set as default.
        * @param Setting number
        * @return ETrue if the setting can be set as default
        */
        IMPORT_C TBool CanSetAsDefault( TInt aItem ) const;

        /**
        * Sets the setting as default.
        * @param Setting number
        */
        IMPORT_C void SetAsDefaultL( TInt aItem );

        /**
        * Creates links from logical proxy ids to logical 
        * proxies and access point ids to access points.
        * Call this before walking through the document data.
        */
        IMPORT_C void CreateLinksL();

        /**
        * Returns a builder to be used for creating the document data.
        * @return Builder
        */
        IMPORT_C MWPBuilder& Builder() const;

        /**
        * Creates a context. Leaves with KErrOverflow if the max. number of contexts
        * has been reached.
        * @param aName Name of the context
        * @param aTPS TPS of the context
        * @param aProxies Proxies that can be used in the context
        * @return UID of the new context
        */
        IMPORT_C TUint32 CreateContextL( const TDesC& aName, const TDesC& aTPS, 
            const MDesC16Array& aProxies );

        /**
        * Deletes a context.
        * @param aUID UID of the context
        */
        IMPORT_C void DeleteContextL( TUint32 aUid );

        /**
        * Deletes a data item from a context.
        * @param aUID UID of the context
        */
        IMPORT_C TBool DeleteContextDataL( TUint32 aUid );

        /**
        * Deletes a data item from a context.
        * @param aUID UID of the context
        * @return Approximation of the number of items in context
        */
        IMPORT_C TInt ContextDataCountL( TUint32 aUid );

        /**
        * Number of contexts available.
        * @return Array of context uids. Ownership is transferred.
        */
        IMPORT_C CArrayFix<TUint32>* ContextUidsL();

        /**
        * Name of a context.
        * @param aUID UID of the context.
        * @return Context name. Ownership is transferred.
        */
        IMPORT_C HBufC16* ContextNameL( TUint32 aUid );

        /**
        * TPS of a context.
        * @param aUID UID of the context.
        * @return TPS. Ownership is transferred.
        */
        IMPORT_C HBufC16* ContextTPSL( TUint32 aUid );

        /**
        * TPS of a context.
        * @param aUID UID of the context.
        * @return TPS. Ownership is transferred.
        */
        IMPORT_C CDesC16Array* ContextProxiesL( TUint32 aUid );

        /**
        * Returns a specific context.
        * @param aTPS TPS whose context should be looked for
        * @return The UID of the context
        */
        IMPORT_C TUint32 ContextL( const TDesC& aTPS );

        /**
        * Checks if a context exists.
        * @param aUID The UID of the context
        */
        IMPORT_C TBool ContextExistsL( TUint32 aUid );

        /**
        * Checks if a context exists.
        * @param aUID The TPS of the context
        */
        IMPORT_C TBool ContextExistsL( const TDesC& aTPS );

        /**
        * Returns the current context.
        * @return Current context UID
        */
        IMPORT_C TUint32 CurrentContextL();

        /**
        * Sets the current context.
        * @param aUID The UID of the context
        */
        IMPORT_C void SetCurrentContextL( TUint32 aUid );

        /**
        * Register an observer to context information.
        * @param aObserver The new observer
        */
        IMPORT_C void RegisterContextObserverL( 
            MWPContextObserver* aObserver );

        /**
        * Unregister an observer to context information.
        * @param aObserver The observer to unregister
        * @return KErrNone, or an error code
        */
        IMPORT_C TInt UnregisterContextObserver( 
            MWPContextObserver* aObserver );
            
        
        /**
        * Retutn the status of iAccessDenied variable
        * @return value of iAccessDenied variable
        */
        IMPORT_C TInt GetAccessDenied();          
        
        /**
        * Sets the iAccessDenied variable 
        * @param aStatus The status of iAccessDenied variable
        */
        IMPORT_C void SetAccessDenied( 
            TBool aStatus );        

    private: // New methods
        /**
        * Converts an item number into a pair of adapter and adapter local item number.
        * @param aGlobalIndex The item number
        * @param aAdapter Contains adapter number after the call
        * @param aAdapterIndex Contains adapter local item number after the call
        */
        void ItemIndex( TInt aGlobalIndex, TInt& aAdapter, TInt& aAdapterIndex ) const;

        /**
        * Returns a context manager.
        * @return Context manager
        */
        MWPContextManager& ContextManager() const;

        /**
        * Returns an adapter manager.
        * @return Adapter manager
        */
        CWPAdapterManager& AdapterManagerL();

    private:
        /**
        * C++ default constructor.
        */
        CWPEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /// Root of the WAP Provisioning document. Owns.
        CWPRoot* iRoot;
        
        /// All adapters. Owns.
        CWPAdapterManager* iAdapters;

        /// Context manager
        MWPContextManager* iManager;
        
        TBool iAccessDenied;
    };

#endif /* CWPENGINE_H */
