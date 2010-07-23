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
*		Declaration file for client API's event/progress notifiers
*
*/


#ifndef __CLIENTAPIACTIVECALLBACK_H__
#define __CLIENTAPIACTIVECALLBACK_H__

#include <SyncMLClient.h>
#include <SyncMLObservers.h>

class CSmlActiveEventCallback;
class CSmlActiveProgressCallback;

// 
// container class from event and progress callbacks
// 
class CSmlActiveCallback : public CBase
	{
	public:
		CSmlActiveCallback( RSyncMLSession& aSession );
		~CSmlActiveCallback();
		
		void SetEventObserverL( MSyncMLEventObserver& aObserver );
		void SetProgressObserverL( MSyncMLProgressObserver& aObserver );
		
		void CancelEvent();
		void CancelProgress();
		
		void SendReceive( TInt aCmd, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const;
		void SendReceive( TInt aCmd ) const;
		
	private:
	
		RSyncMLSession&				iSession;
		CSmlActiveEventCallback*	iEventCallback;
		CSmlActiveProgressCallback*	iProgressCallback;
	};


// 
// event callback
// 
class CSmlActiveEventCallback : public CActive
	{
	public:
		static CSmlActiveEventCallback* NewL( const CSmlActiveCallback* aCallback );
		virtual ~CSmlActiveEventCallback();
		
		void SetObserver( MSyncMLEventObserver& aObserver );
		void CancelEvent();
		
		// from CActive
		virtual void DoCancel();
		virtual void RunL();
		virtual TInt RunError(TInt aError);
		
	private:
		void Request();
		void ConstructL();
		CSmlActiveEventCallback( const CSmlActiveCallback* aCallback );
		
	private:
		TBool	iRequesting;
		TPtr8	iBufPtr;
		HBufC8*	iBuf;
		
		MSyncMLEventObserver*		iObserver; // not owned
		const CSmlActiveCallback*	iCallback; // not owned
	};


// 
// progress callback
// 
class CSmlActiveProgressCallback : public CActive
	{
	public:
		static CSmlActiveProgressCallback* NewL( const CSmlActiveCallback* aCallback );
		virtual ~CSmlActiveProgressCallback();
		
		void SetObserver( MSyncMLProgressObserver& aObserver );
		void CancelProgress();
		
		// from CActive
		virtual void DoCancel();
		virtual void RunL();
		virtual TInt RunError(TInt aError);		
		
	private:
		void Request();
		void ConstructL();
		CSmlActiveProgressCallback( const CSmlActiveCallback* aCallback );
		
		void NotifyErrorL( RReadStream& aStream ) const;
		void NotifyProgressL( RReadStream& aStream ) const;
		void NotifyModificationsL( RReadStream& aStream );
		
	private:
		TBool	iRequesting;
		TPtr8	iBufPtr;
		HBufC8*	iBuf;
		
		MSyncMLProgressObserver::TSyncMLDataSyncModifications iClientMods;
		MSyncMLProgressObserver::TSyncMLDataSyncModifications iServerMods;
		
		MSyncMLProgressObserver*	iObserver; // not owned
		const CSmlActiveCallback*	iCallback; // not owned
	};


#endif // __CLIENTAPIACTIVECALLBACK_H__
