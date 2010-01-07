/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Settings item for one destination network. Also handles
*                saving destination networks to permanent storage.
*
*/




#include <wpdestinationnwadapter.rsg>
#include <charconv.h>
#include <CWPCharacteristic.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h> // For WLAN plugin UID, i.e. bearer type.

#include "cwpdestinationnwitem.h"
#include "wpdestinationnwdebug.h"

const TInt KMaxCharsInTUint32 = 10;

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::NewL
//
// ---------------------------------------------------------------------------
//
CWPDestinationNwItem* CWPDestinationNwItem::NewL()
    {
    CWPDestinationNwItem* self = new ( ELeave ) CWPDestinationNwItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::CWPDestinationNwItem
//
// ---------------------------------------------------------------------------
//
CWPDestinationNwItem::CWPDestinationNwItem()
    {
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::~CWPDestinationNwItem
//
// ---------------------------------------------------------------------------
//
CWPDestinationNwItem::~CWPDestinationNwItem()
    {
    DBG_PRINT( "CWPDestinationNwItem::~CWPDestinationNwItem - begin" );
    delete iAppRef;
    delete iSettingsName;
    iNapDefs.Close();
    delete iSavedProfileId;
    DBG_PRINT( "CWPDestinationNwItem::~CWPDestinationNwItem - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::ConstructL
//
// ---------------------------------------------------------------------------
//
void CWPDestinationNwItem::ConstructL()
    { 
    DBG_PRINT( "CWPDestinationNwItem::ConstructL - begin" );
    iAppRef         = HBufC8::NewL( 0 );
    iSettingsName   = HBufC::NewL( 0 );
    iSavedProfileId = HBufC8::NewL( KMaxCharsInTUint32 );
    DBG_PRINT( "CWPDestinationNwItem::ConstructL - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::SetAppRefL
//
// ---------------------------------------------------------------------------
//
void CWPDestinationNwItem::SetAppRefL( const TDesC8& aAppRef )
    {
    delete iAppRef;
    iAppRef = NULL;
    iAppRef = aAppRef.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::SetSettingsNameL
//
// ---------------------------------------------------------------------------
//
void CWPDestinationNwItem::SetSettingsNameL( const TDesC& aSettingsName )
    {
    delete iSettingsName;
    iSettingsName = NULL;
    iSettingsName = aSettingsName.AllocL();
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::AddNapDefL
//
// ---------------------------------------------------------------------------
//
void CWPDestinationNwItem::AddNapDefL( CWPCharacteristic* aNapDef )
    {
    iNapDefs.AppendL( aNapDef );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::AppRef
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPDestinationNwItem::AppRef() const
    {
    return *iAppRef;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::SettingsName
//
// ---------------------------------------------------------------------------
//
const TDesC& CWPDestinationNwItem::SettingsName() const
    {
    return *iSettingsName;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::SaveL()
// Saves the Destination network using Extended connection settings API. 
// ---------------------------------------------------------------------------
//
void CWPDestinationNwItem::SaveL()
    {
    DBG_PRINT( "CWPDestinationNwItem::SaveL - begin" );
    // Number of IAPs that have been linked to this destination.
    const TInt iapCount( iNapDefs.Count() );
    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager ); // CS:1
    RCmDestinationExt destination = CreateDestinationL( cmManager );
    CleanupClosePushL( destination ); // CS:2
    RCmConnectionMethodExt tempconn;
    
    // Add linked IAPs to destination.    
    for ( TInt counter = 0; counter < iapCount; counter++ )
        {
        TPckgBuf<TUint32> apUid;
        apUid.Copy( iNapDefs[counter]->Data( 0 ) );
        // The stored id is the wap id. Convert it into the iap id
        
        TUint32 iapId = IapIdFromWapIdL( apUid() );
        
        apUid = NULL;
        RCmConnectionMethodExt connection = 
            cmManager.ConnectionMethodL( iapId );
        CleanupClosePushL( connection ); // CS:5

        RArray<TUint32> destinationIds;        
        cmManager.AllDestinationsL( destinationIds );
        CleanupClosePushL( destinationIds );
        
        TInt counter2(0);        
        for ( counter2 = 0; counter2 < destinationIds.Count(); counter2++ )
            {
            RCmDestinationExt tempdest = cmManager.DestinationL( 
                        destinationIds[counter2] );            
            
            TRAPD( err, tempconn = tempdest.ConnectionMethodByIDL( iapId ));
                        
            tempconn.Close();
            tempdest.Close();
            
            if ( err != KErrNotFound )
                {
                // Found from other destination                  
                break;                
                }           
            }                
        if ( counter2 == destinationIds.Count() )
            {
            // Not found in any other destination
            destination.AddConnectionMethodL( connection );            
            }
        else
            {            
            // Found in some other destination -> create copy
            destination.AddConnectionMethodL( tempconn = connection.CreateCopyL() );
            tempconn.Close();            
            }
        CleanupStack::PopAndDestroy( &destinationIds );

        // Change seamlessness level for linked WLAN IAPs.
        TUint32 bearerType = connection.GetIntAttributeL( 
            CMManager::ECmBearerType );
        if ( KUidWlanBearerType == bearerType )
            {
            connection.SetIntAttributeL( CMManager::ECmSeamlessnessLevel, 
                CMManager::ESeamlessnessShowprogress );
            connection.UpdateL();
            }

        destination.UpdateL();
        CleanupStack::PopAndDestroy( &connection ); // CS:4
        }    
    destination.UpdateL(); // In case there were no IAP linkages.
    // Destination ID is needed as a descriptor (used in linking settings).
    TUint32 destinationId = destination.Id();
    iSavedProfileId->Des().Num( destinationId, EDecimal );
    // Pop destination & cmManager.
    CleanupStack::PopAndDestroy( 2, &cmManager ); // CS:0
    DBG_PRINT( "CWPDestinationNwItem::SaveL - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::SaveData
//
// ---------------------------------------------------------------------------
//
const TDesC8& CWPDestinationNwItem::SaveData() const
    {
    return *iSavedProfileId;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwItem::CreateDestinationL
// Checks that settings name if unique, renames if necessary, and creates
// a destination network.
// ---------------------------------------------------------------------------
//
RCmDestinationExt CWPDestinationNwItem::CreateDestinationL( 
    RCmManagerExt& aCmManager )
    {
    DBG_PRINT( "CWPDestinationNwItem::CreateDestinationL - begin" );
    RArray<TUint32> destinationIds;
    // Get destination ID's for checking if name to be set is reserved.
    CleanupClosePushL( destinationIds ); // CS:1
    aCmManager.AllDestinationsL( destinationIds );
    const TInt destinationCount = destinationIds.Count();
    // Destination's name to actually be used.
    HBufC* newName = HBufC::NewLC( 
        iSettingsName->Des().Length() + KMaxCharsInTUint32 ); // CS:2
    newName->Des().Copy( iSettingsName->Des() );

    // Check names from all existing destinations.
    for ( TInt counter = 0; counter < destinationCount; counter++ )
        {
        RCmDestinationExt destination = aCmManager.DestinationL( 
            destinationIds[counter] );
        CleanupClosePushL( destination ); // CS:3
        HBufC* settingsName = destination.NameLC(); // CS:4
        // If destination exists, return it.
        if ( 0 == newName->Des().Compare( settingsName->Des() ) )
            {
            // Pop settingsName & destination & newName
            CleanupStack::PopAndDestroy( 3, &destination ); // CS:1
            
            RCmDestinationExt dest2 = aCmManager.DestinationL( 
                    destinationIds[counter] );
            
            CleanupStack::PopAndDestroy( &destinationIds ); // CS:0
            DBG_PRINT( "CWPDestinationNwItem::CreateDestinationL - end" );
            return dest2;
            }
        // Pop settingsName & destination.
        CleanupStack::PopAndDestroy( 2, &destination ); // CS:2
        }
   
    // Create new a destination and return it.
    RCmDestinationExt destination = aCmManager.CreateDestinationL( *newName );
    // Pop newName & destinationIds
    CleanupStack::PopAndDestroy( 2, &destinationIds ); // CS:0
    DBG_PRINT( "CWPDestinationNwItem::CreateDestinationL - end" );
    return destination;
    }
    
// ---------------------------------------------------------------------------
// CWPDestinationNwItem::IapIdFromWapId
// Returns IAP ID from WAP ID.
// ---------------------------------------------------------------------------
//
TUint32 CWPDestinationNwItem::IapIdFromWapIdL( TUint32 aWapId )
    {
    RCmManagerExt cmManager;
    cmManager.OpenLC(); // CS:1
    RArray<TUint32> iapIds;
    TUint32 iapId( 0 );

    // First get all free IAP ID's.
    cmManager.ConnectionMethodL( iapIds );
    CleanupClosePushL( iapIds ); // CS:2

    // Then get IAP ID's from all destinations.
    RArray<TUint32> destIds;
    cmManager.AllDestinationsL( destIds );
    CleanupClosePushL( destIds ); // CS:3
    TInt destCount = destIds.Count();
    for ( TInt destIndex = 0; destIndex < destCount; destIndex++ )
        {
        RCmDestinationExt dest = cmManager.DestinationL( 
            destIds[destIndex] );
        CleanupClosePushL( dest ); // CS:4
        TInt cmCount = dest.ConnectionMethodCount();
        for ( TInt cmIndex = 0; cmIndex < cmCount; cmIndex++ )
            {
            TUint32 apId = dest.ConnectionMethodL( 
                cmIndex ).GetIntAttributeL( CMManager::ECmIapId );
            iapIds.AppendL( apId );
            }
        CleanupStack::PopAndDestroy( &dest ); // CS:3
        }
    // Finally, go through all connection methods and find correct IAP ID.
    const TInt cmCount = iapIds.Count();
    for ( TInt counter = 0; counter < cmCount; counter++ )
        {
        TUint32 id = cmManager.GetConnectionMethodInfoIntL( 
            iapIds[counter], CMManager::ECmWapId );
        if ( id == aWapId )
            {
            iapId = iapIds[counter];
            // No need to go through rest of IAPs.
            break;
            }
        }
    // PopAndDestroy destIds, iapIds, cmManager.
    CleanupStack::PopAndDestroy( 3, &cmManager ); // CS:0
    return iapId;
    }


// End of file.
