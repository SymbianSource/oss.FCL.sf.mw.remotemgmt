/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef SCPLOCKSETTINGEVENTHANDLER_H
#define SCPLOCKSETTINGEVENTHANDLER_H


//  INCLUDES
#include <bldvariant.hrh>
#include "SCPLockEventHandler.h"
#include "SCPServer.h"

// FORWARD DECLARATIONS
class CSCPSession;

// CLASS DECLARATION

/**
*  An active object class, which handles setting the phone lock
*/
class CSCPLockSettingEventHandler : public CSCPLockEventHandler
	{		
	public:  // Methods

        // Constructors and destructor
        
        /**
        * Static constructor.
        */
   	    static CSCPLockSettingEventHandler* NewL( 
   	        const RMessage2& aLockMessage, 
            CSCPSession* aSession,
            TBool aState,
            TBool aAutolock,
            CSCPServer* aServer );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPLockSettingEventHandler* NewLC(
            const RMessage2& aLockMessage, 
            CSCPSession* aSession,
            TBool aState,
            TBool aAutolock,
            CSCPServer* aServer );

        /**
        * Destructor.
        */
        virtual ~CSCPLockSettingEventHandler();
        
       /**
        * Starts the lock/unlock operation. Must be called for the object
        * to take any action.
        */        
        void StartL();          

    protected:  // Methods

       // Methods from base classes
        
        /**
        * From CActive The active object working method.
        */
        void RunL();
        
        /**
        * From CActive The active object request cancellation method.
        */
        void DoCancel();

    private: //Methods
    
        /**
        * C++ default constructor.
        */
        CSCPLockSettingEventHandler( 
                const RMessage2& aLockMessage,
                CSCPSession* aSession,
                TBool aState,
                TBool aAutolock,
                CSCPServer* aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Used to activate/deactivate Autolock
        */
        void SetAutolockStateL( TBool aActive );

    protected:  // Data

    private:   // Data
         /** The message which initiated this call, will be completed when finished */
         const RMessage2 iLockMessage;
         /** The parent session pointer */
         CSCPSession* iSession;
         /** The state to which the lock should be set */         
         TBool iState;
         /** Should Autolock be activated/deactivated on success */
         TBool iAutolock;    
         /** Has an unlock-message to Autolock been sent*/
         TBool iUnlockSent;
         /** The return status for the message*/
         TInt iMessageStatus;
    };

#endif      // SCPLOCKSETTINGEVENTHANDLER_H   
            
// End of File

