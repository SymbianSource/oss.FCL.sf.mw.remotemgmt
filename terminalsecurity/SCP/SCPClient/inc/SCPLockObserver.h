/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Obsererver for Set System Locked event 
*
*/
#ifndef     __SCPLOCKOBSERVER_H
#define     __SCPLOCKOBSERVER_H

#include <e32svr.h>
#include <e32property.h>
#include "secui.hrh"

 
class CSCPQueryDialog;

NONSHARABLE_CLASS(CSCPLockObserver): public CActive
    {
    public:
		/**
        * Creates instance of the CLockObserver class.
        *
		* @param aDialog (pointer to code dialog)
		* @return Returns the instance just created.
        */
        static CSCPLockObserver* NewL(CSCPQueryDialog* aDialog, TInt aType  = ESecUiDeviceLockObserver);
		/**
        * Destructor.
        */
		~CSCPLockObserver();
		/**
        * Stops listenig PubSub events.
        */
		void StopObserver(); 
		/**
        * Starts listenig PubSub events.
        */
		void StartObserver();
		/**
        * Sets the dialog's address.
        */
		void SetAddress(CSCPQueryDialog* aDialog);    
	private:
        /**
        * Starts asynchronic listening KUidAutolockStatus event
        *
		* @return KErrNone: if no errors
        * @return KErrInUse: if already listening
		*/
		TInt Start();            
    private: // constructors
		/**
        * C++ default constructor.
		*
		* @param aDialog (pointer to code dialog)
        */
        CSCPLockObserver(CSCPQueryDialog* aDialog, TInt aType);
		 /**
        * Symbian OS constructor.
        */
        void ConstructL();
    private: // from CActive
         /** @see CActive::RunL() */
		void RunL();
		/** @see CActive::DoCancel() */
        void DoCancel();
    private: // data
        CSCPQueryDialog*      iDialog; //not owned!
        RProperty            iProperty;
        TBool				 iSubscribedToEvent;
        TInt                 iType; //Type of the observer
    };

#endif 
// End of file
