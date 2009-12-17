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
* Description:  Base class for obex searchers
*
*/



#ifndef COBEXSEARCHERBASE_H
#define COBEXSEARCHERBASE_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES
enum TObexSearcherPanics
	{
	ObexSearcherInvalidState
	};

enum TState
	{
	EIdle,
	EDeviceSearch,
	EDeviceSearchTimeout,
	EServiceSearch
	};

// FORWARD DECLARATIONS
class MObexSearcherObserver;
class MExtBTSearcherObserver;
class CObexClient;

// CLASS DECLARATION

class CObexSearcherBase : public CActive
    {
    public:  // Constructors and destructor
                
        /**
        * Destructor.
        */
        virtual ~CObexSearcherBase();

    public: // New functions
        
        void SetObserver( MObexSearcherObserver* aObserver );
        void SetExtObserver( MExtBTSearcherObserver* aExtObserver );

    public: // Abstract functions

		/**
		 *	Search for a device
		 */ 
        virtual void SearchDeviceL() = 0;

		/**
		 *	Search for a service
		 */
        virtual void SearchServiceL() = 0;

		/**
		 *	Creates an obex client
		 */
		virtual CObexClient* CreateObexClientL() = 0;
        
    protected:  // New functions
        
		/**
		 *	Handle device found
		 */
		void NotifyDeviceFoundL();

		/**
		 *	Handle device search error
		 */
		void NotifyDeviceErrorL( TInt aErr );

		/**
		 *	Handle service found
		 */
		void NotifyServiceFoundL();

		/**
		 *	Handle service search error
		 */
		void NotifyServiceErrorL( TInt aErr );

    protected:

        CObexSearcherBase();
		void BaseConstructL();

    protected:  // Data

        MExtBTSearcherObserver* iExtObserver;
		TState iState;

    private:    // Data

        MObexSearcherObserver* iObserver;

    };

#endif      // COBEXSEARCHERBASE_H   
            
// End of File
