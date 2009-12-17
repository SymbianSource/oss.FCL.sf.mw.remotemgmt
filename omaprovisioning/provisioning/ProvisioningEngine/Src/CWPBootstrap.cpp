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
* Description:  Handles the persistance of Bootstrap settings
*
*/


//  INCLUDE FILES
#include "CWPBootstrap.h"
#include <badesca.h>
#include <uri16.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPEngine.h>
#include "CWPPushMessage.h"
#include "MWPContextManager.h"

// CONSTANTS
const TInt KProxiesGranularity = 3;
const TInt KInitialHBufCSize = 1;
const TInt KProvURLMaxLength = 50;
_LIT( KHttp, "http://" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPBootstrap::CWPBootstrap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPBootstrap::CWPBootstrap( const TDesC& aIMSI )
                        : iIMSI( aIMSI )
    {
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBootstrap::ConstructL()
    {
    iProxies = new(ELeave) CDesC16ArrayFlat( KProxiesGranularity );
    iTPS = HBufC::NewL( KInitialHBufCSize );
    iName = HBufC::NewL( KInitialHBufCSize );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPBootstrap* CWPBootstrap::NewL( const TDesC& aIMSI )
    {
    CWPBootstrap* self = NewLC( aIMSI );
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPBootstrap* CWPBootstrap::NewLC( const TDesC& aIMSI )
    {
    CWPBootstrap* self = new( ELeave ) CWPBootstrap(aIMSI);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPBootstrap::~CWPBootstrap()
    {
    delete iProxies;
    delete iTPS;
    delete iName;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::SaveL
// -----------------------------------------------------------------------------
//
void CWPBootstrap::SaveL( CWPEngine& aEngine )
    {
    aEngine.CreateContextL( *iName, *iTPS, *iProxies );
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::LoadL
// -----------------------------------------------------------------------------
//
void CWPBootstrap::LoadL( CWPEngine& aEngine )
    {
    // Check if the saved settings apply to the current home network
    if( !aEngine.ContextExistsL( *iTPS ) )
        {
        iLoaded = EFalse;
        }
    else
        {
        // Find TPS
        TUint32 tps( aEngine.ContextL( *iTPS ) );
        CDesCArray* proxies = aEngine.ContextProxiesL( tps );
        CleanupStack::PushL( proxies );
        HBufC* name = aEngine.ContextNameL( tps );
        CleanupStack::Pop(); // proxies

        delete iProxies;
        iProxies = proxies;

        delete iName;
        iName = name;

        iLoaded = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::BootstrapL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPBootstrap::TBootstrapResult CWPBootstrap::BootstrapL( 
                                                    CWPPushMessage& aMessage,
                                                    CWPEngine& aEngine,
                                                    const TDesC& aPIN )
    {
    aEngine.AcceptL( *this );

    TBootstrapResult result( ENoBootstrap );

    if( !aMessage.Authenticated() )
        {
        result = DoAuthenticateL( aMessage, aPIN, aEngine );
        }
    else  if( iTPS->Length() > 0 )
        {
        result = DoBootstrapL( aEngine );
        }

    // Set TPS as the sender to later locate the correct configuration context
    if( result == ESucceeded )
        {
        HBufC8* orig = HBufC8::NewLC( iTPS->Length() );
        orig->Des().Copy( *iTPS );
        aMessage.SetOriginatorL( *orig );
        CleanupStack::PopAndDestroy(); // orig
        }
    
    return result;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::TPS
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWPBootstrap::TPS() const
    {
    return *iTPS;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::DoBootstrapL
// -----------------------------------------------------------------------------
//
CWPBootstrap::TBootstrapResult CWPBootstrap::DoAuthenticateL( 
                                                    CWPPushMessage& aMessage,
                                                    const TDesC& aPIN,
                                                    CWPEngine& aEngine )
    {
    TBootstrapResult result( ENoBootstrap );
    TInt authResult( aMessage.AuthenticateL( iIMSI, aPIN ) );
    
    switch( authResult )
        {
        case KWPAuthResultAuthenticated:
            {
            if( iTPS->Length() > 0 )
                {
                result = DoBootstrapL( aEngine );
                }

            aMessage.SetAuthenticated( ETrue );
            break;
            }
            
        case KWPAuthResultPinRequired:
            {
            result = EPinRequired;
            break;
            }
            
        case KWPAuthResultAuthenticationFailed:
            {
            result = EAuthenticationFailed;
            break;
            }
            
        case KWPAuthResultNoAuthentication:
            {
            if( iTPS->Length() > 0 )
                {
                result = ENotAuthenticated;
                }
            else
                {
                result = ENoBootstrap;
                }
            break;
            }
            
        default:
            {
            break;
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::DoBootstrapL
// -----------------------------------------------------------------------------
//
CWPBootstrap::TBootstrapResult CWPBootstrap::DoBootstrapL( CWPEngine& aEngine )
    {
    TBootstrapResult result( ESucceeded );

    LoadL( aEngine );
    if( !iLoaded )
        {
        TRAPD( err, SaveL( aEngine ) );

        if( err == KErrNone )
            {
            result = ESucceeded;
            }
        else if( err == KErrAlreadyExists )
            {
            result = EBootstrapExists;
            }
        else
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
        result = EBootstrapExists;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::VisitL
// -----------------------------------------------------------------------------
//
void CWPBootstrap::VisitL( CWPCharacteristic& aCharacteristic )
    {
    TInt prevChar = iCurrentChar;
    iCurrentChar = aCharacteristic.Type();

    switch( aCharacteristic.Type() )
        {
        case KWPBootstrap:
            /* FALLTHROUGH */
        case KWPPxLogical:
            /* FALLTHROUGH */
        case KWPPxPhysical:
            {
            aCharacteristic.AcceptL( *this );
            break;
            }
        default:
            {
            break;
            }
        }
    iCurrentChar = prevChar;
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::VisitL
// -----------------------------------------------------------------------------
//
void CWPBootstrap::VisitL( CWPParameter& aParameter )
    {
    TPtrC value( aParameter.Value() );
    TPtrC http( KHttp );

    switch( aParameter.ID() )
        {
        case EWPParameterProvURL:
            {
            TInt ProvURLLength = value.Length();
            if( ProvURLLength <= KProvURLMaxLength && iCurrentChar == KWPBootstrap && iTPS->Length() == 0 )
                {
                HBufC* buf = HBufC::NewLC( ProvURLLength + http.Length() );
                TPtr ptr( buf->Des() );
                ptr.Copy( value );

                if( ptr.Left( http.Length() ).CompareF( http ) != 0 )
                    {
                    ptr.Insert( 0, http );
                    }

                TUriParser16 uri;
                if( uri.Parse( ptr ) == KErrNone && uri.IsPresent(EUriHost) )
                    {
                    HBufC* provUrl = uri.Extract(EUriHost).AllocL();
                    delete iTPS;
                    iTPS = provUrl;
                    }

                CleanupStack::PopAndDestroy(); // buf
                }
            break;
            }

        case EWPParameterName:
            {
            if( iCurrentChar == KWPBootstrap && iName->Length() == 0 )
                {
                delete iName;
                iName = NULL;
                TInt length = aParameter.Value().Length();
                if( length >= 50 ) 
                {
                TPtrC ptr = aParameter.Value().Left( 50 );
                iName = ptr.AllocL();
                }
				else
				{
					iName = aParameter.Value().AllocL();
				}
                }
            break;
            }

        case EWPParameterPxAddr:
            {
            TInt pos;
            if( iCurrentChar == KWPPxPhysical 
                && iProxies->Find( aParameter.Value(), pos ) != 0)
                {
                iProxies->AppendL( aParameter.Value() );
                }
            break;
            }

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CWPBootstrap::VisitL
// -----------------------------------------------------------------------------
//
void CWPBootstrap::VisitLinkL( CWPCharacteristic& aLink )
    {
    if( aLink.Type() == KWPPxLogical )
        {
        TInt prevChar = iCurrentChar;
        iCurrentChar = KWPPxLogical;
        aLink.AcceptL( *this );
        iCurrentChar = prevChar;
        }
    }

//  End of File  
