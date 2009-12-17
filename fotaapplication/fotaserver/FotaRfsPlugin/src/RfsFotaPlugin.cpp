/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/



// INCLUDE FILES
#include "RfsFotaPlugin.h"
#include "fotadebug.h"

_LIT(KScriptPath, "");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CRfsFotaPlugin::CRfsFotaPlugin()
    {
    FLOG(_L("CRfsFotaPlugin::CRfsFotaPlugin()"));
    }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CRfsFotaPlugin::CRfsFotaPlugin(TAny* /*aInitParams*/)
    {
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CRfsFotaPlugin::~CRfsFotaPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CRfsFotaPlugin::NewL
// ---------------------------------------------------------------------------
//
CRfsFotaPlugin* CRfsFotaPlugin::NewL(TAny* aInitParams)
    {
    FLOG(_L("CRfsFotaPlugin::NewL()"));
    CRfsFotaPlugin* self = new (ELeave) CRfsFotaPlugin(aInitParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CRfsFotaPlugin::ConstructL()
    {
    FLOG(_L("CRfsFotaPlugin::ConstructL()"));
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::RestoreFactorySettingsL
// Handle rfs. Must reserve disk space to keep user from filling it. 
// ---------------------------------------------------------------------------
//
void CRfsFotaPlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    FLOG(_L("CRfsFotaPlugin::RestoreFactorySettingsL(%d)"), aType);

    if ( aType == ENormalRfs ) 
        {
        FLOG(_L(" ERROR - NORMAL RFS (Only FirstBoot RFS is ordered) "));
        }
    if ( aType == EDeepRfs   ) 
        { 
        FLOG(_L(" ERROR - DEEP   RFS (Only FirstBoot RFS is ordered) "));
        }

    LoadStoragePluginL ();
    iStorage->AdjustDummyStorageL () ;

    UnLoadStoragePluginL();
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::GetScriptL
// ---------------------------------------------------------------------------
//
void CRfsFotaPlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    FLOG(_L("CRfsFotaPlugin::GetScriptL"));
    aPath.Copy( KScriptPath);
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::ExecuteCustomCommandL
// ---------------------------------------------------------------------------
void CRfsFotaPlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/,
                                        TDesC& /*aCommand*/ )
    {
    FLOG(_L("CRfsFotaPlugin::ExecuteCustomCommandL"));
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::LoadStoragePluginL 
// Load update storage plugin via ecom framework.
// ---------------------------------------------------------------------------
//
void CRfsFotaPlugin::LoadStoragePluginL ()
    {
    FLOG(_L("CRfsFotaPlugin::LoadStoragePluginL"));
    if(iStorage)
        {
        UnLoadStoragePluginL ();
        }
    TUid                if_uid;
    if_uid.iUid =  (0x10207384);
    RImplInfoPtrArray   pluginArray;
    REComSession::ListImplementationsL( if_uid, pluginArray );
    CleanupClosePushL (pluginArray);

    if( pluginArray.Count() )
        {
        for( TInt i = 0; i < pluginArray.Count(); i++ )
            {
            CImplementationInformation* info = pluginArray[ i ];
            TUid id = info->ImplementationUid();
            delete info;
            info = NULL;
            iStorage =(CFotaStorage*) REComSession::CreateImplementationL(
                                        id , iStorageDtorKey); 
            }
        }
    else
        {
        FLOG(_L("  storage plugin not found, ERROR"));
        User::Leave ( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(1); // array 
    }


// ---------------------------------------------------------------------------
// CRfsFotaPlugin::UnLoadStoragePluginL
// Unloads storage plugin
// ---------------------------------------------------------------------------
//
void CRfsFotaPlugin::UnLoadStoragePluginL ()
    {
    FLOG(_L("CRfsFotaPlugin::UnLoadStoragePluginL"));
    if(iStorage != NULL) 
        {
        delete iStorage;
        iStorage=0;
        }
    REComSession::DestroyedImplementation(iStorageDtorKey); // sets reference count of plugin to 0
    REComSession::FinalClose();
    }

// End of file
