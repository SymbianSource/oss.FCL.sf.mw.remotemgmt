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
* Description:  SyncML HTTP client
*
*/


#ifndef __NSMLDIALIAP_H__
#define __NSMLDIALIAP_H__

#include <e32base.h>
#include <commdb.h>
#include <cdbstore.h>
#include <in_iface.h>
#include <extendedconnpref.h>
#include <nifman.h>
#include <connectprog.h>
#include <commdbconnpref.h>
#include <es_enum.h>
#include <nsmldebug.h>



//============================================================
// CNSmlDialUpAgent declaration
//============================================================
class CNSmlDialUpAgent : public CActive 
    {

	public:
		CNSmlDialUpAgent();
		void ConstructL(TBool aDmJob);
		void ConnectL( CArrayFixFlat<TUint32>* aIAPIdArray, TRequestStatus &aStatus );
		~CNSmlDialUpAgent();
	
	private:
		void DoCancel();
		void RunL();

	private:
		TBool IsConnectedL() const;
		void DoSettingsL();
		void StartDatacallL();
		void AttachToConnectionL();
		TBool IsInOfflineModeL();
		void IsRoamingL(TBool& aRoaming);
	private:
		TRequestStatus* iEngineStatus;
		RConnection iConnection;
		RSocketServ iSocketServer;
		TUint iIAPid;		
		TBool iCancelCalled;
		TBool iSocketConnection;
        CArrayFixFlat<TUint32>* iIAPArray;
        TInt iIAPCnt;
        TInt iOffline;
        TBool iTimerReturn;
        TInt iRetryCounter;
        RTimer iTimer;
        TConnPrefList iPrefList;
        TExtendedConnPref iExtPrefs;
		TBool iDmjob;

	private:
		friend class CNSmlHTTP;

};



#endif 