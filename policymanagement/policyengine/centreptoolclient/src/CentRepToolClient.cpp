/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/
/* PolicyEngineClient.cpp
*/

#include <e32svr.h>
#include <featmgr.h>
#include <bldvariant.hrh>

#include "CentRepToolClientServer.h"
#include "CentRepToolClient.h"


#include "debug.h"


// Standard server startup code
// 
static TInt StartServer()
	{
	RDEBUG("CentRepToolServer: Starting server...");
	
	const TUidType serverUid(KNullUid,KNullUid,KCentRepToolServerUid);

	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	//TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
	TInt r=server.Create(KCentRepToolServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("CenrRepToolClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("CentRepToolClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}


// This is the standard retry pattern for server connection
EXPORT_C TInt RCentRepTool::Connect()
	{
	TRAPD( err, FeatureManager::InitializeLibL() );
	if( err != KErrNone )
		{
		return err;
		}
				
	TBool iSupported = FeatureManager::FeatureSupported( KFeatureIdSapPolicyManagement);
	FeatureManager::UnInitializeLib();	    
	
	if( !iSupported )
		{
		return KErrNotSupported;
		}	
	
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KCentRepToolServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}
	
EXPORT_C void RCentRepTool::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
EXPORT_C TInt RCentRepTool::CheckCommitState()
{
	return SendReceive( ECheckCommitState, TIpcArgs());	
}

EXPORT_C TInt RCentRepTool::PerformRFS()
{
	return SendReceive( EPerformCentRepToolRFS);	
}


EXPORT_C RSettingManagement::RSettingManagement()
	: iConnected( EFalse)
{
}

EXPORT_C TInt RSettingManagement::Open( TUid aRepositoryId, RCentRepTool &aServer)
{
	TPckg<TUid> uidPack( aRepositoryId);
	TInt err = CreateSubSession( aServer, ECreateRepositorySubSession, TIpcArgs( &uidPack, NULL));
	
	if ( err == KErrNone )
	{
		iConnected = ETrue;
	}
	
	return err;
}
		
EXPORT_C void RSettingManagement::Close()
{
	RSubSessionBase::CloseSubSession(ECloseRepositorySubSession);
}

EXPORT_C void RSettingManagement::InitSession( TRequestStatus& iRequestStatus)
{
	SendReceive( EInitRepositorySession, iRequestStatus);	
}

		
		
EXPORT_C TInt RSettingManagement::SetSIDWRForSetting( TUint32 aSettingId, TUid aSecurityId)
{
	TPckg<TUint32> settingPack( aSettingId);
	TPckg<TUid> securityIdPack( aSecurityId);
	return SendReceive( ESetSIDWRForSetting, TIpcArgs(&settingPack, &securityIdPack));	
}

EXPORT_C TInt RSettingManagement::SetSIDWRForMask( TUint32 aCompareValue, TUint32 aMask, TUid aSecurityId)
{
	TPckg<TUint32> comparePack( aCompareValue);
	TPckg<TUint32> maskPack( aMask);
	TPckg<TUid> securityIdPack( aSecurityId);
	return SendReceive( ESetSIDWRForMask, TIpcArgs(&comparePack, &maskPack, &securityIdPack));	
}

EXPORT_C TInt RSettingManagement::RestoreSetting( TUint32 aSettingId)
{
	TPckg<TUint32> settingPack( aSettingId);
	return SendReceive( ERestoreSetting, TIpcArgs(&settingPack));	
}

EXPORT_C TInt RSettingManagement::RestoreMask( TUint32 aCompareValue, TUint32 aMask)
{
	TPckg<TUint32> comparePack( aCompareValue);
	TPckg<TUint32> maskPack( aMask);;
	return SendReceive( ERestoreMask, TIpcArgs(&comparePack, &maskPack));	
}

EXPORT_C TInt RSettingManagement::AddSIDWRForDefaults( TUid aSecurityId)
{
	TPckg<TUid> securityIdPack( aSecurityId);
	return SendReceive( EAddSIDWRForDefaults, TIpcArgs(&securityIdPack));	
}

EXPORT_C TInt RSettingManagement::RestoreDefaults()
{
	return SendReceive( ERestoreDefaults, TIpcArgs());	
}

EXPORT_C TInt RSettingManagement::Flush()
{
	return SendReceive( EFlushRepository, TIpcArgs());	
}


EXPORT_C TInt RSettingManagement::SetSIDWRForRange( TUint32 aRangeStart, TUint32 aRangeEnd, TUid aSecurityId)
{
	TPckg<TUint32> rangeStartPack( aRangeStart);
	TPckg<TUint32> rangeEndPack( aRangeEnd);
	TPckg<TUid> securityIdPack( aSecurityId);
	return SendReceive( ESetSIDWRForRange, TIpcArgs(& rangeStartPack, &rangeEndPack, &securityIdPack));	
}


EXPORT_C TInt RSettingManagement::RestoreRange( TUint32 aRangeStart, TUint32 aRangeEnd)
{
	TPckg<TUint32> rangeStartPack( aRangeStart);
	TPckg<TUint32> rangeEndPack( aRangeEnd);
	return SendReceive( ERestoreRange, TIpcArgs(& rangeStartPack, &rangeEndPack));	
}


EXPORT_C TInt RSettingManagement::RemoveBackupFlagForMask( TUint32 aCompareValue, TUint32 aMask)
{
	TPckg<TUint32> comparePack( aCompareValue);
	TPckg<TUint32> maskPack( aMask);;
	return SendReceive( ERemoveBackupFlagForMask, TIpcArgs(&comparePack, &maskPack));	
}

EXPORT_C TInt RSettingManagement::RestoreBackupFlagForMask( TUint32 aCompareValue, TUint32 aMask)
{
	TPckg<TUint32> comparePack( aCompareValue);
	TPckg<TUint32> maskPack( aMask);;
	return SendReceive( ERestoreBackupFlagForMask, TIpcArgs(&comparePack, &maskPack));	
}

EXPORT_C TInt RSettingManagement::RemoveBackupFlagForRange( TUint32 aRangeStart, TUint32 aRangeEnd)
{
	TPckg<TUint32> rangeStartPack( aRangeStart);
	TPckg<TUint32> rangeEndPack( aRangeEnd);
	return SendReceive( ERemoveBackupFlagForRange, TIpcArgs( &rangeStartPack, &rangeEndPack));	
}

EXPORT_C TInt RSettingManagement::RestoreBackupFlagForRange( TUint32 aRangeStart, TUint32 aRangeEnd)
{
	TPckg<TUint32> rangeStartPack( aRangeStart);
	TPckg<TUint32> rangeEndPack( aRangeEnd);
	return SendReceive( ERestoreBackupFlagForRange, TIpcArgs( &rangeStartPack, &rangeEndPack));	
}


EXPORT_C TInt RSettingManagement::RemoveBackupFlagForDefaults()
{
	return SendReceive( ERemoveBackupFlagForDefaults, TIpcArgs());	
}

EXPORT_C TInt RSettingManagement::RestoreBackupFlagForDefaults()
{
	return SendReceive( ERestoreBackupFlagForDefaults, TIpcArgs());	
}

EXPORT_C TInt RAccessCheck::Open( TUid aRepositoryId, RCentRepTool &aServer)
{
	TPckg<TUid> uidPack( aRepositoryId);
	TInt err = CreateSubSession( aServer, ECreateCheckAccessSession, TIpcArgs( &uidPack, NULL));
	
	return err;
}
		
EXPORT_C void RAccessCheck::Close()
{
	RSubSessionBase::CloseSubSession(ECloseCheckAcceessSession);
}


EXPORT_C TInt RAccessCheck::CheckAccess( TUint32 aSettingId, TAccessType aAccessType, TBool& aAccessValue)
{
	TPckg<TUint32> idPack( aSettingId);
	TPckg<TAccessType> atPack( aAccessType);
	TPckg<TBool> boolPack( aAccessValue);

	return SendReceive( ECheckAccess, TIpcArgs( &idPack, &atPack, &boolPack));		
}


