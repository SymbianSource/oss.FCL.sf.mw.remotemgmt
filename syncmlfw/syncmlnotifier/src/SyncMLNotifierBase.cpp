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
* Description:  Implementation of the SyncML Appserver starter notifier
*
*/



// INCLUDE FILES
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <settingsinternalcrkeys.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>
#include <eikenv.h>          // Eikon environment
#include <uikon/eiksrvui.h>        // Eikon server ui (for suppressing app -key)
#include <bautils.h>         // BAFL utils (for language file)
#include <data_caging_path_literals.hrh> 
#include <AknNotiferAppServerApplication.h>

#include "SyncMLNotifierBase.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
// Literals for resource and bitmap files ( drive, directory, filename(s) )
_LIT(KFileDrive,"z:");
_LIT(KResourceFileName, "SyncMLNotifier.rsc");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::CSyncMLNotifierBase
// C++ default constructor can NOT contain any code, that
// might leave. Sets the AOs priority and puts
// itself to the active scheduler stack.
// -----------------------------------------------------------------------------
//
CSyncMLNotifierBase::CSyncMLNotifierBase() : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::ConstructL
// Symbian 2nd phase constructor can leave.
// Create registry object and open resource file.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::ConstructL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLNotifierBase::ConstructL()"));

    iEikEnv = CEikonEnv::Static();

    TFileName filename;
    filename += KFileDrive;
    filename += KDC_RESOURCE_FILES_DIR; 
    filename += KResourceFileName;
    BaflUtils::NearestLanguageFile( iEikEnv->FsSession(), filename );
    iResourceFileFlag = iEikEnv->AddResourceFileL( filename );
    FLOG(_L("[SmlNotif]\t CSyncMLNotifierBase::ConstructL() completed"));
    }
    
// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLNotifierBase::~CSyncMLNotifierBase()
    {
    Cancel();
    iEikEnv->DeleteResourceFile( iResourceFileFlag );

    // Complete the RMessage2 if needed
    //
    if ( iNeedToCompleteMessage )
        {
        iMessage.Complete( KErrDied );
        }

	// Activate apps -key again (if not previously activated yet)
	SuppressAppSwitching( EFalse );

    iReplySlot = NULL;
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::Release
// Release itself. Call to destructor.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::Release()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::Info
// Return registered information.
// -----------------------------------------------------------------------------
//
CSyncMLNotifierBase::TNotifierInfo CSyncMLNotifierBase::Info() const
    {
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::StartL
// Synchronic notifier launch. Does nothing here.
// -----------------------------------------------------------------------------
//
TPtrC8 CSyncMLNotifierBase::StartL( const TDesC8& /*aBuffer*/ )
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::StartL(sync)") );
    TPtrC8 ret( KNullDesC8 );
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::StartL(sync) completed") );
    return ( ret );
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::StartL
// Asynchronic notifier launch.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::StartL( const TDesC8& aBuffer,
                                        TInt aReplySlot,
                                  const RMessagePtr2& aMessage )
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::StartL(async)") );
    TRAPD( err, GetParamsL( aBuffer, aReplySlot, aMessage ) );
    if ( err )
        {
        aMessage.Complete( err );
        iNeedToCompleteMessage = EFalse;
        User::Leave( err );
        }
    FLOG(_L("[SmlNotif]\t CSyncMLNotifierBase::StartL(async) completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::Cancel
// Cancelling method.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::Cancel()
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::Cancel()") );
    if (iNeedToCompleteMessage)
    	{
    	iMessage.Complete(KErrCancel);
    	iNeedToCompleteMessage = EFalse;
    	}
    iNeedToCompleteMessage = EFalse;
    iReplySlot = NULL;

    // Activate apps -key again (if not previously activated yet)
	SuppressAppSwitching( EFalse );

    CActive::Cancel();

    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::Cancel() completed") );
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::UpdateL
// Notifier update. Not supported.
// -----------------------------------------------------------------------------
//
TPtrC8 CSyncMLNotifierBase::UpdateL(const TDesC8& /*aBuffer*/)
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::UpdateL()") );
    TPtrC8 ret(KNullDesC8);
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::UpdateL() completed") );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::DoCancel
// This method will be called by framework (CActive)
// if active object is still active.
// Does nothing here.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::DoCancel()
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::DoCancel()") );
	FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::DoCancel()--End") );
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::RunError
// This method is called if any leaving has been occured
// during RunL. Optional method for CActive derived objects.
// -----------------------------------------------------------------------------
//
TInt CSyncMLNotifierBase::RunError( TInt aError )
    {
    FTRACE( FPrint( _L("[SmlNotif]\t CSyncMLNotifierBase::RunError() Error = %d"), aError ) );
    // Activate apps -key again (if not previously activated yet)
	SuppressAppSwitching( EFalse );

    // Complete message if needed and return the error code.
    if( iNeedToCompleteMessage )
        {
        iMessage.Complete( aError );
        }
    iNeedToCompleteMessage = EFalse;
    iReplySlot = NULL;

    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::RunError() completed") );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::TurnLightsOn
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::TurnLightsOn()
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::TurnLightsOn()") );

    RProperty::Set( KPSUidCoreApplicationUIs, KLightsControl, ELightsOn );
    RProperty::Set( KPSUidCoreApplicationUIs, KLightsControl, ELightsOff );

    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::TurnLightsOn() completed") );
    }

// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::AutoLockOnL
// -----------------------------------------------------------------------------
//
TBool CSyncMLNotifierBase::AutoLockOnL()
    {
    FLOG( _L("[SmlNotif]\t CSyncMLNotifierBase::AutoLockOnL()") );

    TBool result = EFalse;
    TInt status = 1;
    
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL( KCRUidSecuritySettings );

    repository->Get( KSettingsAutolockStatus, status );

    // Closing connection:
    delete repository;

    if ( status == 1 )
        {
        result = ETrue;
        }

    FTRACE( FPrint(
        _L("[SmlNotif]\t CSyncMLNotifierBase::AutoLockOnL() completed with %d"),
        result ) );

    return result;
    }


// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::SuppressAppSwitchching
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierBase::SuppressAppSwitching( TBool aEnable )
    {	   
    if ( iAppsKeyBlocked != aEnable )
        {
        TInt err = ( ( CAknNotifierAppServerAppUi* )
            ( CEikonEnv::Static() )->EikAppUi() )->
            SuppressAppSwitching( aEnable );
        iAppsKeyBlocked = aEnable;
        }
    }

//  End of File  
