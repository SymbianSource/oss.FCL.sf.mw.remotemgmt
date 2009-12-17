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
* Description:  Client for DS and DM initiated syncs
*
*/



// INCLUDE FILES

#include <bldvariant.hrh>

#include <centralrepository.h>
#include <SyncMLNotifierDomainCRKeys.h>
#include <SyncServiceSession.h>
#include <SyncServiceParams.h>
#include <SyncService.h>
#include <SyncServiceConst.h>
#include "SyncServiceDebug.h"

#include <apgcli.h>
#include <AknServerApp.h>
#include <barsread2.h>

// ----------------------------------------------------------------------------
// RSyncService::StartSyncL
// ----------------------------------------------------------------------------
//
void RSyncService::StartSyncL( TSyncServiceParam aParam )
	{
	FLOG( _L( "[RSyncService] StartSyncL" ) );
	
	TSyncParameters parameters;
	parameters.iCommand = KSyncServCmdStartSync;
    parameters.iJobId = aParam.iJobId;
    parameters.iProfileId = aParam.iProfileId;
    parameters.iSilent = aParam.iSilent;
    TInt err = 
		SendReceive( ESyncServiceCommand, TIpcArgs( parameters.iCommand,
		                                            parameters.iJobId,
		                                            parameters.iProfileId,
		                                            parameters.iSilent ) );
	User::LeaveIfError( err );
	}

// ----------------------------------------------------------------------------
// RSyncService::EnableProgressNoteL
// ----------------------------------------------------------------------------
//
void RSyncService::EnableProgressNoteL( TBool aEnabled )
	{
	FLOG( _L("[RSyncService] EnableProgressNoteL") );

	TSyncParameters parameters;
	parameters.iCommand = KSyncServCmdShowProgress;

    if ( aEnabled )
        {
        parameters.iJobId = KSyncServEnabled;
        }
    else
        {
        parameters.iJobId = KSyncServDisabled;
        }
    
    TInt err = 
		SendReceive( ESyncServiceCommand, TIpcArgs( parameters.iCommand,
		                                            parameters.iJobId,
		                                            parameters.iProfileId,
		                                            parameters.iSilent ) );
	User::LeaveIfError( err );
	}

// ----------------------------------------------------------------------------
// RSyncService::ServiceUid
// ----------------------------------------------------------------------------
//
TUid RSyncService::ServiceUid() const
	{
	FLOG( _L( "[RSyncService] ServiceUid" ) );
	
	return TUid::Uid( KSyncServiceUid );
	}

// ----------------------------------------------------------------------------
// CSyncService::CSyncService
// ----------------------------------------------------------------------------
//
CSyncService::CSyncService( )
	{
	FLOG( _L( "[CSyncService] CSyncService" ) );
	
	}

// ----------------------------------------------------------------------------
// CSyncService::~CSyncService
// ----------------------------------------------------------------------------
//
CSyncService::~CSyncService( )
	{
	FLOG( _L( "[CSyncService] ~CSyncService" ) );
	
	delete iMonitor;
	iService.Close();
	}

// ----------------------------------------------------------------------------
// CSyncService::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSyncService* CSyncService::NewL( MAknServerAppExitObserver* aObserver,
                                           TUint aServiceId )
    {
    FLOG( _L( "[CSyncService] NewL" ) );
    
	CSyncService* self = new(ELeave) CSyncService();
	CleanupStack::PushL(self);
    self->ConstructL( aObserver, aServiceId );
	CleanupStack::Pop(self);
	return self;
    }

// ----------------------------------------------------------------------------
// CSyncService::ConstructL
// ----------------------------------------------------------------------------
//
void CSyncService::ConstructL( MAknServerAppExitObserver* /*aObserver*/,
                               TUint aServiceId )
    {
	FLOG( _L( "[CSyncService] ConstructL" ) );
    iServiceId = aServiceId;
    }

// ----------------------------------------------------------------------------
// CSyncService::StartSyncL
// ----------------------------------------------------------------------------
//
EXPORT_C TUid CSyncService::StartSyncL( TSyncServiceParam aParam )
    {
    FLOG( _L( "[CSyncService] StartSyncL" ) );

    iServiceId = aParam.iServiceId;
	iUid = DiscoverL( iServiceId );
	
    TInt error = KErrNotFound;
        
    if( iService.ServerName().Length() > 0 )
        {
        FLOG( _L( "[CSyncService] ConstructL connect to existing app" ) );
  	    TRAP( error, iService.StartSyncL( aParam ) );
  	    if ( error == KErrNone )
  	        {
  	        return iUid;
  	        }
        }
   	if ( error )
		{
		TFullName aServerName;
		if ( iServiceId == KDevManServiceStart )
		    {
    		CRepository* centrep = NULL;
    		centrep = CRepository::NewL( KCRUidNSmlNotifierDomainKeys );
    		centrep->Get( KNSmlDMAppName, aServerName );
    		delete centrep;
		    }
		else
		    {
		    aServerName = KDataSyncServiceName;
		    }
        FLOG( _L( "[CSyncService] ConstructL connect to existing app with predefined name" ) );
        iService.Close();
		TRAP( error, iService.ConnectExistingByNameL( aServerName ) );
		}
   	if ( error )
		{
		FLOG( _L( "[CSyncService] ConstructL connect to new app" ) );
    	TRAP( error, iService.ConnectNewAppL( iUid ) );
    	if( error )
    		{
    		FLOG( _L( "[CSyncService] ConnectNewAppL: Error not supported" ) );
    		User::Leave( KErrNotSupported );
    		}
 		}
    
    iService.StartSyncL( aParam );
    return iUid;
    }

// ----------------------------------------------------------------------------
// CSyncService::EnableProgressNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSyncService::EnableProgressNoteL( TBool aEnabled )
    {
    FLOG( _L( "[CSyncService] EnableProgressNoteL" ) );
    
    TFullName DmAppName;
    CRepository* centrep = NULL;
    centrep = CRepository::NewL( KCRUidNSmlNotifierDomainKeys );
    centrep->Get( KNSmlDMAppName, DmAppName );
    delete centrep;
    
    if( iService.ServerName().Length() > 0 )
        {
		TRAPD(error, iService.EnableProgressNoteL( aEnabled ) );
		if ( error != KErrNone )
		    {
    		TFullName aServerName;
    		if ( iServiceId == KDevManServiceStart )
    		    {
                aServerName = DmAppName;
    		    }
    		else
    		    {
    		    aServerName = KDataSyncServiceName;
    		    }
            FLOG( _L( "[CSyncService] EnableProgressNoteL connect to existing app with predefined name" ) );
            iService.Close();
    		iService.ConnectExistingByNameL( aServerName );        
            iService.EnableProgressNoteL( aEnabled );
		    }
        }
    else
        {
        // Sync has been started from APP UI so connect to the service and enable progressnote
		TFullName aServerName;
		if ( iServiceId == KDevManServiceStart )
		    {
		    aServerName = DmAppName;
		    }
		else
		    {
		    aServerName = KDataSyncServiceName;
		    }
        FLOG( _L( "[CSyncService] EnableProgressNoteL connect to existing app with predefined name" ) );
		iService.ConnectExistingByNameL( aServerName );        
        iService.EnableProgressNoteL( aEnabled );
        }
    FLOG( _L( "[CSyncService] EnableProgressNoteL" ) );
    }

// ----------------------------------------------------------------------------
// CSyncService::DiscoverL
// ----------------------------------------------------------------------------
//
TUid CSyncService::DiscoverL( TUint aServiceId )
	{
	FLOG( _L( "[CSyncService] DiscoverL" ) );
	
	CApaAppServiceInfoArray* services = NULL;	
	RApaLsSession ls;
	TUid appUid = TUid::Uid( 0x0 ); // Set the uid as NULL
	
	if( ls.Connect() == KErrNone )
		{
	    CleanupClosePushL( ls );
		TInt ret = ls.GetServerApps( TUid::Uid( KSyncServiceUid ) );

		if( ret != KErrNone )
			{
		    CleanupStack::PopAndDestroy( );  // closes RApaLsSession		
		    return appUid; // return NULL appUid
			}

		services = ls.GetServiceImplementationsLC( TUid::Uid( KSyncServiceUid ) );			
		
		if( services )
			{
			TArray<TApaAppServiceInfo> s = services->Array( );
			
			TInt count = s.Count( );
		
			if( count != 0)
				{	// 
				for( TInt i = 0; i < count; i++ )
					{
					if( TryMatchL( s[i], appUid, aServiceId ) )
						{ // If one of the services match, return the uid set by TryMatchL
						break;
						}
					}
				}		
			}
	    CleanupStack::PopAndDestroy( 2 );  // destroys services + closes RApaLsSession
		}
	return appUid;
	}

// ----------------------------------------------------------------------------
// CSyncService::TryMatchL
// ----------------------------------------------------------------------------
//
TBool CSyncService::TryMatchL( const TApaAppServiceInfo aInfo,
                               TUid& aAppUid,
                               TUint aServiceId )
	{
	FLOG( _L( "[CSyncService] TryMatchL" ) );
	
	RResourceReader r;
	r.OpenLC( aInfo.OpaqueData() );

	TInt serviceType = r.ReadUint32L( ); // the service type (sync start)

    CleanupStack::PopAndDestroy( );				
	
	if( serviceType == aServiceId )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		FLOG( _L( "[CSyncService] TryMatchL return ETrue" ) );
		return ETrue;
		}
    
    FLOG( _L( "[CSyncService] TryMatchL return EFalse" ) );
	return EFalse;	
	}
	
// End of file
