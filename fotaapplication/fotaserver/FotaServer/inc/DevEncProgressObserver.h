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
* Description:   GPRS and Wlan status getter for sending Generic Alerts
*
*/



#ifndef __NFEOBSERVER_H__
#define __NFEOBSERVER_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include "DevEncProgressDlg.h"

//Forward declarations
class CDevEncController;
class CDevEncSession;
class CDevEncProgressDlg;
//Constants used in this class

const TTimeIntervalMicroSeconds32 KNfeTimeInterval = 500000; //0.5 second, duration between each retry.
const KProgressComplete = 100; //100 percent

/**
* Actice object class that runs a timer for probing network status (GPRS & WLAN)
*  @lib    fotaserver
*  @since  S60 v3.2
*/
NONSHARABLE_CLASS (CDevEncProgressObserver) : public MDEProgressDlgObserver
	{
	public:
		//Symbian 2-Phase construction
		
		IMPORT_C static CDevEncProgressObserver* NewL(CDevEncController* aObserver, TInt aResource);
		IMPORT_C static CDevEncProgressObserver* NewLC(CDevEncController* aObserver, TInt aResource);

       /**
        * Destructor.
        */      
		virtual ~CDevEncProgressObserver();

	public: 
		/**
	     * Starts monitoring for Network status before sending Generic Alert
		 *
     	 * @since   S60   v5.2
     	 * @param   None
     	 * @return  None
     	 */

		void StartMonitoringL(CDevEncSession* aSession);
		
    public: // Functions from base classes

        void CheckProgressL();
        
		TBool HandleDEProgressDialogExitL(TInt aButtonId);
		
	private: //functions
		
		//Constructors
		
		CDevEncProgressObserver();
		CDevEncProgressObserver(CDevEncController* aObserver);

		//Symbian 2-Phase construction 
		void ConstructL(TInt aResource);
		
		TInt GetStatusL();
		
	
	private: // data
	
		
		CDevEncController* iObserver;
		
        CDevEncProgressDlg* iProgressDlg;
		
        CPeriodic* iPeriodicTimer;
		
		//Not owned
		CDevEncSession*     iEncMemorySession;
	
	};

#endif // __NFEOBSERVER_H__

// End of File
