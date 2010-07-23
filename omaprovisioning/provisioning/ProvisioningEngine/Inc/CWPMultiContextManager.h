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
* Description: 
*     Multiple context manager.
*
*/


#ifndef CWPMULTICONTEXTMANAGER_H
#define CWPMULTICONTEXTMANAGER_H

// INCLUDES
#include <e32base.h>
#include <d32dbms.h>
#include "MWPContextManager.h"

// FORWARD DECLARATIONS

// CONSTANTS
_LIT( KDbTableData, "C" );
_LIT( KDbIndexDataContextId, "CA" );
_LIT( KDbColumnDataContextId, "CB" );
_LIT( KDbColumnDataAdapterId, "CC" );
_LIT( KDbColumnDataSaveData, "CE" );

// CLASS DECLARATION

/**
*  CWPMultiContextManager handles data in contexts.
*
*  @lib ProvisioningEngine
*  @since 2.1
 */ 
class CWPMultiContextManager : public CActive, public MWPContextManager
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return Context manager
        */
		static CWPMultiContextManager* NewL();

        /**
        * Two-phased constructor.
        * @return Context manager
        */
		static CWPMultiContextManager* NewLC();

        /**
        * Destructor.
        */
		~CWPMultiContextManager();

    protected: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

	public: // From MWPContextManager

        TUint32 CreateContextL( const TDesC& aName, const TDesC& aTPS, 
            const MDesC16Array& aProxies );
        void DeleteContextL( MWPContextExtensionArray& aArray, TUint32 aUid );
        CArrayFix<TUint32>* ContextUidsL();
        HBufC* ContextNameL( TUint32 aUid );
        HBufC* ContextTPSL( TUint32 aUid );
        CDesC16Array* ContextProxiesL( TUint32 aUid );
        TUint32 ContextL( const TDesC& aTPS );
        TBool ContextExistsL( TUint32 aUid );
        TBool ContextExistsL( const TDesC& aTPS );
        void SaveL( MWPContextExtension& aExtension, TInt aItem );
        TBool DeleteContextDataL( 
            MWPContextExtensionArray& aArray, TUint32 aUid );
        TInt ContextDataCountL( TUint32 aUid );
        TUint32 CurrentContextL();
        void SetCurrentContextL( TUint32 aUid );
        void RegisterContextObserverL( 
            MWPContextObserver* aObserver );
        TInt UnregisterContextObserver( 
            MWPContextObserver* aObserver );

	protected: // New methods
        /**
        * C++ default constructor.
        */
		CWPMultiContextManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

        /**
        * Opens the context database.
        */
        void OpenDatabaseL();

        /**
        * Creates the context database.
        */
        void CreateDatabaseL();

        /**
        * Checks the database and performs recovery if needed.
        */
        void CheckDatabaseL();

        /**
        * Creates the names table.
        * @param aDb The database
        */
        void CreateNamesTableL( RDbDatabase& aDb );

        /**
        * Creates the proxies table.
        * @param aDb The database
        */
        void CreateProxiesTableL( RDbDatabase& aDb );

        /**
        * Creates the data table.
        * @param aDb The database
        */
        void CreateDataTableL( RDbDatabase& aDb );

        /**
        * Pushes a database rollback cleanup item and starts a transaction.
        */
        void DatabaseBeginLC();

        /**
        * Pops a database rollback cleanup item and commits a transaction.
        */
        void DatabaseCommitLP();

        /**
        * Cleanup item. Rolls back database transaction.
        * @param aAny The RDbDatabase pointer
        */
        static void CleanupRollback( TAny* aAny );

        /**
        * Pushes a cancel cleanup item on stack.
        * @param aTable The table to cancel on failure
        */
        void InsertBeginLC( RDbTable& aTable ) const;

        /**
        * Puts changes performed on a row.
        * @param aTable The table to change
        */
        void InsertCommitLP( RDbTable& aTable ) const;

        /**
        * Cleanup item. Cancels table update.
        * @param aAny The RDbTable pointer
        */
        static void CleanupCancel( TAny* aAny );

        /**
        * Cleanup item. Deletes the database.
        * @param aAny The RFs pointer
        */
        static void CleanupDeleteDb( TAny* aAny );

        /**
        * Opens a table and puts it on cleanup stack.
        * @param aName Name of the table
        * @param aTable The table object to fill
        * @param aAccess The access type
        */
        void OpenLC( const TDesC& aName, RDbTable& aTable, 
            RDbTable::TAccess aAccess=RDbTable::EUpdatable );

        /**
        * Issues a change notify event to the context database.
        */
        void IssueRequest();

    protected:
		
        // Database manager
        RDbs                                iDbSession;
        // Database
        RDbNamedDatabase                    iDataBase;
        // The current context
        TUint32                             iCurrentContext;
        // The database change notifier
        RDbNotifier                         iNotifier;
        // The context observers. Observers referred.
        RPointerArray<MWPContextObserver>   iObservers;
        TFileName                           iDataBasePath;
	};

#endif /* CWPMULTICONTEXTMANAGER_H */
