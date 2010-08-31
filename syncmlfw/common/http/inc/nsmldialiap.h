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
		void ConstructL();
		void ConnectL( CArrayFixFlat<TUint32>* aIAPIdArray, TRequestStatus &aStatus );
		~CNSmlDialUpAgent();
	
	private:
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);

	private:
		TBool IsConnectedL() const;
		void DoSettingsL();
		void StartDatacallL();
		void AttachToConnectionL();
		TBool IsInOfflineModeL();
	private:
		TRequestStatus* iEngineStatus;
		RConnection iConnection;
		RSocketServ iSocketServer;

		TUint iIAPid;
		TCommDbConnPref iPref;
		TBool iCancelCalled;
		TBool iSocketConnection;
        CArrayFixFlat<TUint32>* iIAPArray;
        TInt iIAPCnt;
        TInt iOffline;
        TBool iTimerReturn;
        TInt iRetryCounter;
        RTimer iTimer;

	private:
		friend class CNSmlHTTP;

};



#endif 