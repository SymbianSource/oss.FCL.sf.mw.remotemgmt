/*
* ==============================================================================
*  Name        : Nsmldshostsessioncancel.cpp
*  Part of     : Nokia SyncML / DS Loader Framework
*  Description : Handling Session cancel for DS host server.
*  Version     : $Revision: 41 $ $Modtime: 09.02.10 8:05 $
*
*  Copyright © 2005 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/
//#include <sbdefs.h>
#include <d32dbms.h>
#include <nsmldebug.h>
#include <DataSyncInternalPSKeys.h>
#include "Nsmldshostsessioncanceleventhandler.h"
#include "Nsmldshostsessioncancel.h"
// --------------------------------------------------------------------------
// CNSmlHostSessionCancel* CNSmlHostSessionCancel::NewL( MNSmlDsHostSessionCancelEventHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlHostSessionCancel* CNSmlHostSessionCancel::NewL( MNSmlDsHostSessionCancelEventHandler* aHandler )
	{
	CNSmlHostSessionCancel* self = new (ELeave) CNSmlHostSessionCancel( aHandler );
	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
	}

// --------------------------------------------------------------------------
// CNSmlHostSessionCancel::CNSmlHostSessionCancel( MNSmlDsHostSessionCancelEventHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlHostSessionCancel::CNSmlHostSessionCancel( MNSmlDsHostSessionCancelEventHandler* aHandler ) :
    CActive( EPriorityNormal ),
    iHandler( aHandler )
	{
    CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// void CNSmlHostSessionCancel::Subscribe()
// --------------------------------------------------------------------------
//
void CNSmlHostSessionCancel::Subscribe()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );    
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// void CNSmlHostSessionCancel::ConstructL()
// --------------------------------------------------------------------------
//
void CNSmlHostSessionCancel::ConstructL()
    {
    iProperty.Attach( KPSUidNSmlDSSyncApp, KDsJobCancel );
    Subscribe();    
    }

// --------------------------------------------------------------------------
// CNSmlHostSessionCancel::~CNSmlHostSessionCancel()
// --------------------------------------------------------------------------
//
CNSmlHostSessionCancel::~CNSmlHostSessionCancel()
	{
    Cancel();
    iProperty.Close();
    }

// --------------------------------------------------------------------------
// void CNSmlHostSessionCancel::RunL()
// --------------------------------------------------------------------------
//
void CNSmlHostSessionCancel::RunL()
    {
    _DBG_FILE("CNSmlHostSessionCancel::RunL() : Begin");
    
    TInt value(0);
    iProperty.Get( value );
    
    if( value == KErrCancel)
        {
        _DBG_FILE("CNSmlHostSessionCancel::RunL() :: user has canceled the operation");

        //user has canceled the opeartion inform session
        iHandler->HandleCancelEventL();
        }
    else
        {
        Subscribe();
        }

    _DBG_FILE("CNSmlHostSessionCancel::RunL() : End");
    }

// --------------------------------------------------------------------------
// void CNSmlHostSessionCancel::DoCancel()
// --------------------------------------------------------------------------
//
void CNSmlHostSessionCancel::DoCancel()
    {
    iProperty.Cancel();
    }

//  End of File
