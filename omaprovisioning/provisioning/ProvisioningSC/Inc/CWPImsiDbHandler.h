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
* Description:  Stores processed SIM imsis to the a database
*
*/



#ifndef CWPIMSIDBHANDLER_H
#define CWPIMSIDBHANDLER_H

//  INCLUDES
#include	<e32base.h>
#include	<f32file.h>
#include	<d32dbms.h>


/**
*  CWPImsiDbHandler manages the storage of the imsi numbers
*  of processed SIM cards.
*  @since 2.5
*/
class CWPImsiDbHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPImsiDbHandler* NewL();

		/**
        * Two-phased constructor.
        */
        static CWPImsiDbHandler* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CWPImsiDbHandler();

    public: // New functions
        
		/**
        * Initialize storage, create new if corrupt
		* or not existing
        * @since 2.5
        */
		void InitializeStorageL();

		/**
        * Is the sim imsi new to the device?
        * @since 2.5
        * @param aPhoneImsi the key
        * @return ETrue if current exists
        */
		TBool ImsiExistsL( const TDesC& aPhoneImsi );

		/**
        * Store a given IMSI to the database
        * @since 2.5
        * @param aPhoneImsi the key
        * @return ETrue if current exists
        */
		void StoreImsiL( const TDesC& aPhoneImsi );

    private:

        /**
        * C++ default constructor.
        */
        CWPImsiDbHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		/**
        * Creates a database for storing imsi numbers
        * @since 2.5
        */
		void CreateDatabaseL();
		/**
        * Creates a database for storing imsi numbers
        * @since 2.5
        */
		void CreateTableL();
		/**
        * Creates a database for storing imsi numbers
        * @since 2.5
        */
		void CreateIndexL();
		/**
        * Cancels the current database transaction
        * @since 2.5
        */
		static void CleanupRollback( TAny* aAny );
		/**
        * Cleanup item. Cancels table update.
        * @param aAny The RDbTable pointer
        */
        static void CleanupCancel( TAny* aAny );
		
    private:    // Data

		// File handle for database
		RFs	                iFs;
		// Database for imsi numbers
        RDbNamedDatabase    iDb;
		// Table to store imsi numbers
		RDbTable            iImsiTable;
		TFileName           iDataBasePath;
    };

#endif      // CWPImsiDbHandler_H   
            
// End of File
