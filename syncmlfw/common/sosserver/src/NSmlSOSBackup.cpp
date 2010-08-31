/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Symbian OS Server source.
*
*/


#include <sbdefs.h>
#include <d32dbms.h>
#include <nsmldebug.h>
#include "nsmlsosserver.h"
#include "nsmlagentlog.h"
// --------------------------------------------------------------------------
// CNSmlSOSBackup* CNSmlSOSBackup::NewL( CNSmlSOSHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlSOSBackup* CNSmlSOSBackup::NewL( CNSmlSOSHandler* aHandler )
	{
	CNSmlSOSBackup* self = new (ELeave) CNSmlSOSBackup( aHandler );
	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
	}

// --------------------------------------------------------------------------
// CNSmlSOSBackup::CNSmlSOSBackup( CNSmlSOSHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlSOSBackup::CNSmlSOSBackup( CNSmlSOSHandler* aHandler ) :
    CActive( EPriorityNormal ),
    iHandler( aHandler ),
    iRestoring( EFalse )
	{
    CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// void CNSmlSOSBackup::Subscribe()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::Subscribe()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );    
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSBackup::ConstructL()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::ConstructL()
    {
    iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey );
    Subscribe();    
    TInt currValue(0);
    iProperty.Get( currValue );
    
    if ( currValue && currValue != ( conn::EBURNormal | conn::ENoBackup ) )
        {
        iHandler->ServerSuspendedL( ETrue );
        }

   	BackUpModeL();
    }

// --------------------------------------------------------------------------
// CNSmlSOSBackup::~CNSmlSOSBackup()
// --------------------------------------------------------------------------
//
CNSmlSOSBackup::~CNSmlSOSBackup()
	{
    Cancel();
    iProperty.Close();
    }

// --------------------------------------------------------------------------
// void CNSmlSOSBackup::RunL()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::RunL()
    {
    _DBG_FILE("CNSmlSOSBackup::RunL() : Begin");
    Subscribe();

    TInt value(0);
    iProperty.Get( value );
    
    const conn::TBURPartType partType =
            static_cast<conn::TBURPartType>( value & conn::KBURPartTypeMask );
    if ( partType && partType != ( conn::EBURUnset | conn::EBURNormal ) )
        {
        _DBG_FILE("CNSmlSOSBackup::RunL() :: Before BackUpModeL");
        BackUpModeL();
        iHandler->ServerSuspendedL( ETrue );
        _DBG_FILE("CNSmlSOSBackup::RunL() :: After BackUpModeL");
        }
    else
        {
        if ( iRestoring )
            {
            _DBG_FILE("Before : ->DeleteDataInRestoreL()");
    		TRAP_IGNORE( DeleteDataInRestoreL() );
    		_DBG_FILE("After  : ->DeleteDataInRestoreL()");
    		
    		_DBG_FILE("Ending restoring");
    		iRestoring = EFalse;
            }
        iHandler->ServerSuspendedL( EFalse );
        }
        
    _DBG_FILE("CNSmlSOSBackup::RunL() : End");
    }

TInt CNSmlSOSBackup::RunError ( TInt aError )
    {
    DBG_FILE_CODE(aError, _S8("CNSmlSOSBackup::RunError() The Error occurred is "));
    return KErrNone;
    }	
// --------------------------------------------------------------------------
// void CNSmlSOSBackup::DoCancel()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::DoCancel()
    {
    iProperty.Cancel();
    }

// --------------------------------------------------------------------------
// void CNSmlSOSBackup::BackUpModeL()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::BackUpModeL()
	{
	_DBG_FILE("CNSmlSOSBackup::BackUpModeL() : Begin");
	
	TInt value(0);
    iProperty.Get( value );
    
    //Get the type of current backup process
    const conn::TBURPartType partType =
            static_cast<conn::TBURPartType>( value & conn::KBURPartTypeMask );
	    
	conn::CActiveBackupClient* client = conn::CActiveBackupClient::NewL();
	CleanupStack::PushL( client );
	
	_DBG_FILE("Before : ->ConfirmReadyForBURL()");
	client->ConfirmReadyForBURL( KErrNone );
	_DBG_FILE("After : ->ConfirmReadyForBURL()");
	
	if ( ( partType == conn::EBURRestoreFull ) ||
	     ( partType == conn::EBURRestorePartial ) )
		{
		_DBG_FILE("Started restoring");
		iRestoring = ETrue;
		}
		
	CleanupStack::PopAndDestroy( client ); //client
			
	_DBG_FILE("CNSmlSOSBackup::BackUpModeL() : End");
	}

// --------------------------------------------------------------------------
// void CNSmlSOSBackup::DeleteDataInRestoreL()
// --------------------------------------------------------------------------
//
void CNSmlSOSBackup::DeleteDataInRestoreL()
	{
	_DBG_FILE("CNSmlSOSBackup::DeleteDataInRestore() : Begin");	

    RDbs database;
    User::LeaveIfError( database.Connect() );
    CleanupClosePushL( database );
    TParse name;

#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlAgentLogDbName(), NULL, NULL );
#else
    name.Set( KNSmlAgentLogDbName(), KNSmlDatabasesNonSecurePath, NULL );
#endif
    
    TInt error = database.DeleteDatabase( name.FullName(), KNSmlSOSServerPolicyUID );
    CleanupStack::PopAndDestroy( &database ); // database
    
    if ( error != KErrNone && error != KErrNotFound )
        {
        _DBG_FILE("CNSmlSOSBackup::DeleteDataInRestore() : End with error");
        User::Leave( error );
        }
     // update the settings db
     CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
     settings->UpdateHiddenProfilesL();
     CleanupStack::PopAndDestroy(); //settings
     CNSmlDMSettings* dmsettings = CNSmlDMSettings::NewLC();
     dmsettings->ScanAndRepairRestoredProfilesL();
     CleanupStack::PopAndDestroy(); //dmsettings
	
	_DBG_FILE("CNSmlSOSBackup::DeleteDataInRestore() : End");			
	}
	

//  End of File
