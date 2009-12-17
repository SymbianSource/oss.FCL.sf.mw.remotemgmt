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
* Description:  Reads provisioning settings from sim cards
*
*/


#ifndef CWPPROVISIONINGSC_H
#define CWPPROVISIONINGSC_H

//  INCLUDES
#include <e32base.h>
#include "MWPWimObserver.h"
// CONSTANTS
const TInt KImsiNumberLength = 15;

// FORWARD DECLARATIONS
class CWPWimHandler;
class CWPImsiDbHandler;

// CLASS DECLARATION
/**
*  ProvisioningSC detects a new SIM and handles the
*  contained OMA provisioning document 
*  @since 2.6
*/
class CWPProvisioningSC : public CBase, public MWPWimObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPProvisioningSC* NewL();
        /**
        * Second two-phased constructor.
        */ 

		static CWPProvisioningSC* NewLC();
        /**
        * Destructor.
        */
        virtual ~CWPProvisioningSC();

    public: // New functions
        
		/**
        * Checks if a provisioning document is stored in the
		* smart card and reads it. Calls back the observer.
        * @since 2.6
        * @return ETrue if the file has not been already read
        */
		TBool ReadFile();

		
	// from base classes
	private: 

		// from MWPWimObserver
		void ReadCompletedL();
		void ReadCancelledL();
		void ReadErrorL();
        
    private:
        /**
        * C++ default constructor.
        */
        CWPProvisioningSC();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		/**
        * Reads the imsi number of the current sim card
		* and checks if it exists already in shared data
        * @since 2.6
        * @param aPhoneImsi modified to the current sim
        * @return ETrue if imsi was not found in shared data
        */
		TBool ProcessImsiL( TDes& aPhoneImsi);

		/**
        * Displays a memory low note
        * @since 2.6
        */
		void ShowMemoryLowNoteL();

		/**
        * Stores the provisioning document into inbox
		* if appropriate.
        * @since 2.6
		* @param provDoc to be stored
        */
		void StoreDocL( TDesC8& aProvDoc );

    private:    // Data
		HBufC8* iProvisioningDoc;// owned
		HBufC* iImsiNumbers; // owned
		CWPWimHandler* iWimHandler; // owned
		CWPImsiDbHandler* iImsiHandler;
		TBuf<KImsiNumberLength> iPhoneImsi;
    private:    // Friend classes
		friend class T_CWPProvisioningSC;
    };

#endif      // CWPPROVISIONINGSC_H   
             
// End of File
