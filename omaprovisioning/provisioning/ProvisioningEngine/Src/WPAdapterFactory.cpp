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
* Description:  Factory for adapters.
*
*/


//  INCLUDE FILES
#include <e32std.h>
#include <ecom/ecom.h>
#include "WPAdapterFactory.h"
#include "ProvisioningDebug.h"
#include "CWPAdapter.h"

// CONSTANTS
const TInt KDefaultPriority = 9999;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPAdapterFactory::CreateAdaptersL
// -----------------------------------------------------------------------------
//
CArrayPtr<CWPAdapter>* WPAdapterFactory::CreateAdaptersL()
    {
    // Create an array for adapter information
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( TCleanupItem( CleanupImplArray, &implArray ) );

    // Get the list of adapters
    CWPAdapter::ListL( implArray );

    // Sort them in priority order
    implArray.Sort( 
        TLinearOrder<CImplementationInformation>( WPAdapterFactory::Compare ) );

    // Create an array for adapters
    CArrayPtr<CWPAdapter>* adapters = 
        new(ELeave) CArrayPtrFlat<CWPAdapter>( Max( 1, implArray.Count() ) );
    CleanupStack::PushL( TCleanupItem( CleanupAdapterArray, adapters ) );

    // Create the adapters
    for( TInt i( 0 ); i < implArray.Count(); i++ )
        {
        CImplementationInformation& info = *implArray[i];
        TUid implementation( info.ImplementationUid() );
        FTRACE(RDebug::Print(_L("[Provisioning] WPAdapterFactory::CreateAdaptersL(): ROM: %d UID: %x"),info.RomBased(), info.ImplementationUid()));
        CWPAdapter* adapter = NULL;
        TRAPD( error, adapter = CWPAdapter::NewL( implementation ) );
        
        if( adapter && ( error == KErrNone ) )
            {
            CleanupStack::PushL( adapter );
            adapters->AppendL( adapter );
            CleanupStack::Pop( adapter );
            }
        else
            {
            FTRACE(RDebug::Print(_L("[Provisioning] WPAdapterFactory::CreateAdaptersL(): failed UID: %x with error %d"), info.ImplementationUid(), error));
            }
        }

    CleanupStack::Pop( adapters );
    CleanupStack::PopAndDestroy(); // implArray

    FLOG( _L( "[Provisioning] WPAdapterFactory::CreateAdaptersL(): End" ) );
    return adapters;
    }

// -----------------------------------------------------------------------------
// WPAdapterFactory::CleanupImplArray
// -----------------------------------------------------------------------------
//
void WPAdapterFactory::CleanupImplArray( TAny* aAny )
    {
    RImplInfoPtrArray* implArray = 
        reinterpret_cast<RImplInfoPtrArray*>( aAny );
    implArray->ResetAndDestroy();
    implArray->Close();
    }

// -----------------------------------------------------------------------------
// WPAdapterFactory::CleanupAdapterArray
// -----------------------------------------------------------------------------
//
void WPAdapterFactory::CleanupAdapterArray( TAny* aAny )
    {
    CArrayPtr<CWPAdapter>* adapters = 
        reinterpret_cast<CArrayPtr<CWPAdapter>*>( aAny );
    adapters->ResetAndDestroy();
    delete adapters;
    }

// -----------------------------------------------------------------------------
// WPAdapterFactory::Compare
// -----------------------------------------------------------------------------
//
TInt WPAdapterFactory::Compare( const CImplementationInformation& aImpl1, 
                               const CImplementationInformation& aImpl2 )
    {
    // Compare the numerical values of opaque_data 
    TLex8 lex( aImpl1.OpaqueData() );
    TInt impl1( KDefaultPriority );
    if( lex.Val( impl1 ) != KErrNone )
        {
        impl1 = KMaxTInt;
        }

    lex.Assign( aImpl2.OpaqueData() );
    TInt impl2( KDefaultPriority );
    if( lex.Val( impl2 ) != KErrNone )
        {
        impl2 = KMaxTInt;
        }

    return impl1 - impl2;
    }

//  End of File  
