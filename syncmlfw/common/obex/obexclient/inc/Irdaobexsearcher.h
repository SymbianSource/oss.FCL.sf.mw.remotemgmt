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
* Description:  IrDA device and service searcher
*
*/



#ifndef CIRDAOBEXSEARCHER_H
#define CIRDAOBEXSEARCHER_H

//  INCLUDES
#include <ObexSearcherBase.h>
#ifndef TEST_HARNESS
#include <es_sock.h>	//for RSocketServ & RHostResolver
#else
#include "es_sock_stub.h"
#endif

// FORWARD DECLARATIONS
class CObexClient;

// CLASS DECLARATION

class CIrDAObexSearcher : public CObexSearcherBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIrDAObexSearcher* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CIrDAObexSearcher();
    
	private:

        /**
        * C++ default constructor.
        */
        CIrDAObexSearcher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // Functions from base classes

        void RunL();
        TInt RunError(TInt aError);

        void DoCancel();

		/**
		 *	Search for IrDA device
		 */
        void SearchDeviceL();

		/**
		 *	Search for IrDA service
		 */
        void SearchServiceL();

		/**
		 *	Create an OBEX client
		 */
        CObexClient* CreateObexClientL();


	private: //Data

		// For IrDA connection
		RSocketServ iSocketServer;
		TProtocolDesc iProtocolInfo;	
		RHostResolver iHostResolver;
		RSocket iSocket;
		TNameEntry iLog;

		// How many times we try to reconnect if connection fails
		TInt iConnectionRetry;

		// How long are we going to wait between retries
		RTimer iRetryTimer;

		// The connection error (not timeout) that will be passed to
		// NotifyDeviceErrorL
		TInt iInitialErr;
		TBool iRetryConn;
    };

#endif      // CIRDAOBEXSEARCHER_H   
            
// End of File
