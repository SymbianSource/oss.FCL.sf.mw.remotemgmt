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
* Description:  Used by ProvisioningSC to read data from SIM cards
*
*/



#ifndef CWPWIMHANDLER_H
#define CWPWIMHANDLER_H

//  INCLUDES
#include	<e32base.h>
#include	<msvapi.h>
// #include	<MWPCard.h>
#include <ProvSC.h>
// FORWARD DECLARATIONS
class	MWPWimObserver;
class	CWimOMAProv;

/**
*  
*  
*  @since 2.5
*/
class CWPWimHandler : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPWimHandler* NewL( MWPWimObserver& aObserver );

		/**
        * Two-phased constructor.
        */
        static CWPWimHandler* NewLC( MWPWimObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CWPWimHandler();

	public: 
		/**
        * Reads provisioning file asynchronously
        * @since 2.5
        */
		void ReadProvFile( const TOMAType& aFileType );
		        /**
        * Gets the length of the provisioning
		* document
        * @since 2.5
        * @return length
        */
		// TInt DocLength();
		/**
        * Fetches the read provisioning document
        * @since 2.5
        * @return pointer to a copy of the prov doc
        */
		HBufC8* DocL();

	protected:
		// from CActive
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();


    private:

        /**
        * C++ default constructor.
        */
        CWPWimHandler( MWPWimObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void Retrieve();

	private:
		
        enum TOMAPhase
            {
            EInitialize,
            EConnectClientSession,
            EInitializeWim,
            EInitializeDone,
            EGetSize,
            EGetSizeDone,
            ERetrieve,
            ERetrieveDone
            };

    private:    // Data

	CWimOMAProv* iFileLoader; // owns
	HBufC8* iProvisioningDoc; // owns
	MWPWimObserver& iObserver;
	TRequestStatus* iGetStatus; // owns
	TPtr8 iProvData;
	TOMAPhase iPhase;
	TInt	iFileSize;
	TBool	iClientInitialized;
	TOMAType iFileType;
    };

#endif      // CWPWimHandler_H   
            
// End of File
