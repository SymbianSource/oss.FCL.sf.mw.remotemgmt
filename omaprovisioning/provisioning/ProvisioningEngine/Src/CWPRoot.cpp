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
* Description:  Root of Provisioning document
*
*/


//  INCLUDE FILES
#include <s32file.h>
#include <badesca.h>
#include "CWPRoot.h"
#include "CWPCharacteristic.h"
#include "CWPParameter.h"
#include "WPElementFactory.h"
#include "ProvisioningDebug.h"

// CONSTANTS
const TInt KElementsGranularity = 5;
const TInt KIDArrayGranularity = 6;
_LIT( KInternet, "INTERNET" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPRoot::CWPRoot
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPRoot::CWPRoot()
    {
    }

// -----------------------------------------------------------------------------
// CWPRoot::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPRoot::ConstructL()
    {
    iContents = new(ELeave) 
        CArrayPtrFlat<CWPCharacteristic>( KElementsGranularity );
    }

// -----------------------------------------------------------------------------
// CWPRoot::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPRoot* CWPRoot::NewL()
    {
    CWPRoot* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPRoot::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPRoot* CWPRoot::NewLC()
    {
    CWPRoot* self = new( ELeave ) CWPRoot;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPRoot::~CWPRoot()
    {
    if( iContents )
        {
        iContents->ResetAndDestroy();
        delete iContents;
        }

    iNeeders.Close();
    iProviders.Close();
    delete iNeededIDs;
    delete iProviderIDs;
    iStack.Close();
    }

// -----------------------------------------------------------------------------
// CWPRoot::AcceptL
// -----------------------------------------------------------------------------
//
void CWPRoot::AcceptL( MWPVisitor& aVisitor )
    {
    FLOG( _L( "[Provisioning] CWPRoot::AcceptL:" ) );
    
    for( TInt i( 0 ); i < iContents->Count(); i++ )
        {
        iContents->At( i )->CallVisitorL( aVisitor );
        }
        
    FLOG( _L( "[Provisioning] CWPRoot::AcceptL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CWPRoot::InternalizeL
// -----------------------------------------------------------------------------
//
void CWPRoot::InternalizeL(RReadStream& aStream)
    {
    TInt count( aStream.ReadInt32L() );

    iContents->ResetAndDestroy();
    for( TInt i( 0 ); i < count; i++ )
        {
        TInt type( aStream.ReadInt32L() );
        CWPCharacteristic* current = 
            WPElementFactory::CreateCharacteristicLC( type );
        current->InternalizeL( aStream );
        iContents->AppendL( current );
        CleanupStack::Pop(); // current
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::ExternalizeL
// -----------------------------------------------------------------------------
//
void CWPRoot::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( iContents->Count() );

    for( TInt i( 0 ); i < iContents->Count(); i++ )
        {
        CWPCharacteristic* current = iContents->At( i );
        aStream.WriteInt32L( current->Type() );
        current->ExternalizeL( aStream );
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::InsertL
// -----------------------------------------------------------------------------
//
void CWPRoot::InsertL( CWPCharacteristic* aCharacteristic )
    {
    iContents->AppendL( aCharacteristic );
    }

// -----------------------------------------------------------------------------
// CWPRoot::StartCharacteristicL
// -----------------------------------------------------------------------------
//
void CWPRoot::StartCharacteristicL( TInt aType )
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPRoot::StartCharacteristicL: aType (%d)"), aType));

    CWPCharacteristic* newChar = 
        WPElementFactory::CreateCharacteristicLC( aType );
    if( iStack.Count() == 0 )
        {
        // Assumes ownership if doesn't leave
        InsertL( newChar );
        }
    else
        {
        CWPCharacteristic* latest = iStack[ iStack.Count()-1 ];
        // Assumes ownership if doesn't leave
        latest->InsertL( newChar );
        }
    CleanupStack::Pop(); // newChar

    User::LeaveIfError( iStack.Append( newChar ) );
    }

// -----------------------------------------------------------------------------
// CWPRoot::StartCharacteristicL
// -----------------------------------------------------------------------------
//
void CWPRoot::StartCharacteristicL( const TDesC& aName )
    {
    FLOG( _L( "[Provisioning] CWPRoot::StartCharacteristicL:" ) );

    CWPCharacteristic* newChar = 
        WPElementFactory::CreateCharacteristicLC( aName );
    if( iStack.Count() == 0 )
        {
        // Assumes ownership if doesn't leave
        InsertL( newChar );
        }
    else
        {
        CWPCharacteristic* latest = iStack[ iStack.Count()-1 ];
        // Assumes ownership if doesn't leave
        latest->InsertL( newChar );
        }
    CleanupStack::Pop(); // newChar

    User::LeaveIfError( iStack.Append( newChar ) );
    }

// -----------------------------------------------------------------------------
// CWPRoot::EndCharacteristicL
// -----------------------------------------------------------------------------
//
void CWPRoot::EndCharacteristicL()
    {
    FLOG( _L( "[Provisioning] CWPRoot::EndCharacteristicL:" ) );
    
    if( iStack.Count() > 0 )
        {
        iStack.Remove( iStack.Count()-1 );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::ParameterL
// -----------------------------------------------------------------------------
//
void CWPRoot::ParameterL( TInt aID, const TDesC& aValue )
    {
    TPtrC ptr( aValue );

    FTRACE(RDebug::Print(_L("[Provisioning] CWPRoot::ParameterL: id: %d: value: %S"), aID, &ptr));
    
    if( iStack.Count() > 0 )
        {
        CWPParameter* param = WPElementFactory::CreateParameterLC( aID, ptr );
        iStack[ iStack.Count()-1 ]->InsertL( param );
        CleanupStack::Pop(); // param
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::ParameterL
// -----------------------------------------------------------------------------
//
void CWPRoot::ParameterL( const TDesC& aName, const TDesC& aValue )
    {
    TPtrC name( aName );
    TPtrC value( aValue );

    FTRACE(RDebug::Print(_L("[Provisioning] CWPRoot::ParameterL: %S: %S"), &name, &value));
    
    if( iStack.Count() > 0 )
        {
        CWPParameter* param = 
            WPElementFactory::CreateParameterLC( name, value );
        iStack[ iStack.Count()-1 ]->InsertL( param );
        CleanupStack::Pop(); // param
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::CreateLinksL
// -----------------------------------------------------------------------------
//
void CWPRoot::CreateLinksL()
    {
    FLOG( _L( "[Provisioning] CWPRoot::ParameterL2:" ) );
    
    iNeeders.Reset();
    delete iNeededIDs;
    iNeededIDs = NULL;
    iNeededIDs = new(ELeave) CDesCArrayFlat( KIDArrayGranularity );
    delete iProviderIDs;
    iProviderIDs = NULL;
    iProviderIDs = new(ELeave) CDesCArrayFlat( KIDArrayGranularity );

    AcceptL( *this );

    // We now have arrays of links and targets of links. Put them together.
    for( TInt i( 0 ); i < iNeeders.Count(); i++ )
        {
        CWPCharacteristic* needer = iNeeders[i];
        TPtrC neededID( iNeededIDs->MdcaPoint( i ) );

        TBool foundProvider( EFalse );
        for( TInt j( 0 ); j < iProviders.Count() && !foundProvider; j++ )
            {
            CWPCharacteristic* needed = iProviders[j];
            TPtrC providerID( iProviderIDs->MdcaPoint( j ) );

            if( providerID == neededID )
                {
            	if ( needer->Type() == KWPPxPhysical && needed->Type() != KWPNapDef)
            		{
            		// incorrect link found. do nothing.
            		}
            	else
            		{
#ifndef __SYNCML_DM_OTA
					FLOG ( _L( "[Provisioning] CWPRoot::CreateLinksL*********** __SYNCML_DM_OTA ***********" ) );
            		if (KWPApplication == needer->Type())
            			{
            			_LIT( KNSmlDMProvisioningDMAppIdVal, "w7" );
            			FLOG (_L( "[Provisioning] CWPRoot::CreateLinksL:Needer is Application" ) );
            			
            			CArrayFix<TPtrC>* name = new(ELeave) CArrayFixFlat<TPtrC>(1);
				        CleanupStack::PushL(name);
				        				        
				        needer->ParameterL(EWPParameterAppID, name);
				        
				        if ((name->Count() > 0)
				        	&&
				        	0 == name->At(0).Compare(KNSmlDMProvisioningDMAppIdVal) )
				            {
				            CleanupStack::PopAndDestroy(); // name
            				continue;
				            }
				            
				        
				        CleanupStack::PopAndDestroy(); // name
            			}
            		FLOG ( _L( "[Provisioning] CWPRoot::CreateLinksL:*********** __SYNCML_DM_OTA ***********" ) );
#endif
            		
                    needer->InsertLinkL( *needed );
                    foundProvider = ETrue;
            		}
                }
            }
        }
    
    // Free the temporary memory
    iNeeders.Reset();
    iProviders.Reset();
    delete iNeededIDs;
    iNeededIDs = NULL;
    delete iProviderIDs;
    iProviderIDs = NULL;
    }

// -----------------------------------------------------------------------------
// CWPRoot::Visit
// -----------------------------------------------------------------------------
//
void CWPRoot::VisitL( CWPParameter& aParameter )
    {
    FLOG( _L( "[Provisioning] CWPRoot::VisitL Parameter:" ) );
    
    if( iCharStack )
        {
        TInt charType( iCharStack->Type() );
        TInt paramID( aParameter.ID() );

        // If the current characteristic is a logical proxy or access point,
        // add it to the list of potential targets of a link
        if( (charType == KWPPxLogical && paramID == EWPParameterProxyID )
            || (charType == KWPNapDef && paramID == EWPParameterNapID) )
            {
            iProviderIDs->AppendL( aParameter.Value() );
            }
        // Handle internet-capable NAPDEF here
        else if( charType == KWPNapDef && paramID == EWPParameterInternet )
            {
            User::LeaveIfError( iProviders.Append( iCharStack ) );
            iProviderIDs->AppendL( KInternet );
            }
        // If the parameter can link, append to the list of sources of links
        else if( paramID == EWPParameterToNapID
            || paramID == EWPParameterToProxy
            || (charType == KWPBootstrap && paramID == EWPParameterProxyID) )
            {
            iNeededIDs->AppendL( aParameter.Value() );
            User::LeaveIfError( iNeeders.Append( iCharStack ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPRoot::Visit
// -----------------------------------------------------------------------------
//
void CWPRoot::VisitL( CWPCharacteristic& aCharacteristic )
    {
    FLOG( _L( "[Provisioning] CWPRoot::VisitL Char:" ) );
    
    TInt charType( aCharacteristic.Type() );

    // Add the characteristic to the stack as current
    CWPCharacteristic* charStack = iCharStack;
    iCharStack = &aCharacteristic;

    // Logical proxy and access points as treated as potential targets of links
    if( charType == KWPPxLogical || charType == KWPNapDef )
        {
        // First get the characteristic and then use a visitor to find out the
        // id
        aCharacteristic.AcceptL( *this );

        // If no id was found, there's something wrong with the document
        if( iProviders.Count() == iProviderIDs->Count()-1 )
            {
            User::LeaveIfError( iProviders.Append( &aCharacteristic ) );
            }
        }
    else
        {
        // Just enter other characteristics
        aCharacteristic.AcceptL( *this );
        }

    // Remove the current from stack
    iCharStack = charStack;
    }

// -----------------------------------------------------------------------------
// CWPRoot::VisitLink
// -----------------------------------------------------------------------------
//
void CWPRoot::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    {
    // Links have not yet been created
    }

//  End of File  
