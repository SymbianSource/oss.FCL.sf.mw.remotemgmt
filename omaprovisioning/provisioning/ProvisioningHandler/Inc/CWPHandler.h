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
*     Main class for handling provisioning message
*
*/


#ifndef CWPHANDLER_H
#define CWPHANDLER_H

// INCLUDES
#include <e32base.h>
#include <ccontenthandlerbase.h>
#include "MWPVisitor.h"

// CONSTANTS

// DATA TYPES
enum TProvState { EProcessing, EDone };

// CLASS DECLARATION

/**
 * CWPHandler handles provisioning documents.
 */ 
class CWPHandler : public CContentHandlerBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWPHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CWPHandler();

    public: // From CContentHandlerBase

        void HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus);
        void  CancelHandleMessage();
        void HandleMessageL(CPushMessage* aPushMsg);
        void CPushHandlerBase_Reserved1();
        void CPushHandlerBase_Reserved2();
        
    protected: // From CActive

        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:
        /**
        * C++ default constructor.
        */
        CWPHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Process the received push message.
        */
        void ProcessingPushMsgEntryL();

    };

#endif  // CWPHANDLER_H
            
// End of File
