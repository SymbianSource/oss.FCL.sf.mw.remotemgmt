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
* Description: 
*       Declaration file for client API's event/progress notifiers
*
*/


#ifndef __NSMLCLIENTCONTACTSUITEAPIACTIVECALLBACK_H_
#define __NSMLCLIENTCONTACTSUITEAPIACTIVECALLBACK_H_

#include <SyncMLClient.h>
#include <SyncMLObservers.h>

class CSmlActiveEventCallback;
class CSmlActiveProgressCallback;
class CSmlActiveContactSuiteProgressCallback;

// 
// container class from event and progress callbacks
// 
class CSmlContactSuiteActiveCallback : public CBase
    {
    public:
        CSmlContactSuiteActiveCallback( RSyncMLSession& aSession );
        ~CSmlContactSuiteActiveCallback();
                
        void SetProgressObserverL( MSyncMLProgressObserver& aObserver );
        
        void CancelProgress();
               
        void SendReceive( TInt aCmd, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const;
        void SendReceive( TInt aCmd ) const;        
        
    private:
        RSyncMLSession&             iContactSuiteSession;
        CSmlActiveCallback          iActiveCallback;
        CSmlActiveContactSuiteProgressCallback* iProgressCallback;
    };

// 
// progress callback
// 
class CSmlActiveContactSuiteProgressCallback : public CActive
    {
    public:
        static CSmlActiveContactSuiteProgressCallback* NewL( const CSmlContactSuiteActiveCallback* aCallback );
        virtual ~CSmlActiveContactSuiteProgressCallback();
        
        void SetObserver( MSyncMLProgressObserver& aObserver );        
        void CancelProgress();
        
        // from CActive
        virtual void DoCancel();
        virtual void RunL();
				virtual TInt RunError(TInt aError);	
        
    private:
        void Request();
        void ConstructL();
        CSmlActiveContactSuiteProgressCallback( const CSmlContactSuiteActiveCallback* aCallback );
        
        void NotifyErrorL( RReadStream& aStream ) const;
        void NotifyProgressL( RReadStream& aStream ) const;
        void NotifyModificationsL( RReadStream& aStream );
        
    private:
        TBool   iRequesting;
        TPtr8   iBufPtr;
        HBufC8* iBuf;
        
        MSyncMLProgressObserver::TSyncMLDataSyncModifications iClientMods;
        MSyncMLProgressObserver::TSyncMLDataSyncModifications iServerMods;
        
        MSyncMLProgressObserver*    iObserver; // not owned
        const CSmlContactSuiteActiveCallback*   iCallback; // not owned
    };

#endif /*__NSMLCLIENTCONTACTSUITEAPIACTIVECALLBACK_H_*/
