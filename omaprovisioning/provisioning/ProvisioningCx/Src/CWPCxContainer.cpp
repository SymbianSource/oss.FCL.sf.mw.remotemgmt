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
* Description:  Provisioning context list
*
*/


//  INCLUDE FILES
#include "CWPCxContainer.h"
#include <stringloader.h>
#include <aknlists.h>
#include <barsread.h>
#include <provisioningcx.rsg>
#include <csxhelp/prov.hlp.hrh>
#include "CWPEngine.h"
#include "ProvisioningUids.h"
#include "ProvisioningCx.hrh"
#include "Provisioningdebug.h"

// CONSTANTS
/// Granularity of the lines array
const TInt KLinesGranularity = 3;
/// Maximum number of characters in one line
const TInt KMaxLineLength = 128;
/// Number of tabs in front of the line
const TInt KNumTabsPrepended = 1;
/// Number of tabs in the end of the line
const TInt KNumTabsAppended = 2;

//help resources
//_LIT(KPROV_HLP_CONFCONT,"PROV_HLP_CONFCONT");
const TUint32 KGSUid = 0x100058ec;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CWPCxContainer::CWPCxContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPCxContainer::CWPCxContainer( CWPEngine& aEngine, CAknView& aAppView )
                                : iEngine( aEngine ), iAppView( aAppView )
    {
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CWPCxContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    iListBox = new(ELeave) CAknSingleStyleListBox;
    iListBox->SetContainerWindowL( *this);

	// Add vertical scroll bar for the list
    CEikScrollBarFrame* sbFrame = iListBox->CreateScrollBarFrameL( ETrue );
    sbFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,    
                                      CEikScrollBarFrame::EAuto ); // vertical
    TResourceReader rr;
    CEikonEnv::Static()->CreateResourceReaderLC( rr, R_PROVISIONINGCX_LISTBOX );
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy(); // rr

    iLines = new(ELeave) CDesCArrayFlat(KLinesGranularity);
    UpdateContextsL();

    iListBox->Model()->SetItemTextArray( iLines );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    HBufC* emptyText = StringLoader::LoadLC( R_QTN_OP_CONF_CONT_EMPTY );
    iListBox->View()->SetListEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
    
    SetRect(aRect);
    ActivateL();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPCxContainer::~CWPCxContainer()
    {
    delete iListBox;
    delete iCxUids;
    delete iLines;
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::CurrentContext
// -----------------------------------------------------------------------------
//
TUint32 CWPCxContainer::CurrentContext() const
    {
    TInt current( iListBox->CurrentItemIndex() );

    TUint32 result( KWPUidNoContext );
    if( current >= 0 )
        {
        result = iCxUids->At( current );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::CurrentContextName
// -----------------------------------------------------------------------------
//
TPtrC CWPCxContainer::CurrentContextName() const
    {
    TInt current( iListBox->CurrentItemIndex() );

    TPtrC result( iLines->MdcaPoint( current ) );
    result.Set( result.Mid( KNumTabsPrepended ) );
    result.Set( result.Left( result.Length()-KNumTabsAppended ) );
    return result;
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::ContextCount
// -----------------------------------------------------------------------------
//
TInt CWPCxContainer::ContextCount() const
    {
    return iLines->MdcaCount();
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::UpdateContextsL
// -----------------------------------------------------------------------------
//
void CWPCxContainer::UpdateContextsL()
    {
    FLOG( _L( "[Provisioning] CWPCxContainer::UpdateContextsL" ) );
    
    delete iCxUids;
    iCxUids = NULL;
    iCxUids = iEngine.ContextUidsL();

    iLines->Reset();

    TInt defaultCount( 0 );
    const TInt numUids( iCxUids->Count() );
    for( TInt i( 0 ); i < numUids; i++ )
        {
        FLOG( _L( "[Provisioning] CWPCxContainer::UpdateContextsL 1" ) );
        TBuf<KMaxLineLength> line;

        TUint32 uid( iCxUids->At( i ) );

        HBufC* tps = iEngine.ContextTPSL( uid );
        CleanupStack::PushL( tps );
        HBufC* name = iEngine.ContextNameL( uid );
        if( name->Length() == 0 )
            {
            FLOG( _L( "[Provisioning] CWPCxContainer::UpdateContextsL name length 0" ) );
            delete name;
            if( defaultCount == 0 )
                {
                name = StringLoader::LoadL( R_QTN_OP_CONTEXT_NAME );
                }
            else
                {
                name = StringLoader::LoadL( R_QTN_OP_CONTEXT_NAME2,
                                            defaultCount );
                }
            defaultCount++;
            }
        CleanupStack::PushL( name );

        line.Zero();
        line.Append( EKeyTab );
        line.Append( *name );
        line.Append( EKeyTab );
        line.Append( EKeyTab );

        CleanupStack::PopAndDestroy( name );
        CleanupStack::PopAndDestroy( tps );
        
        iLines->AppendL( line );
        }

    iListBox->HandleItemAdditionL();

    TInt index = iListBox->CurrentItemIndex(); 
    TInt count = iListBox->Model()->NumberOfItems();
    
    if ( (index < 0 || index >= count) && count > 0 )
        {
        // sets the last item as focused
        iListBox->SetCurrentItemIndexAndDraw(count-1); 
        iListBox->HandleItemRemovalL();
        }
    FLOG( _L( "[Provisioning] CWPCxContainer::UpdateContextsL done" ) );
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CWPCxContainer::SizeChanged()
    {
    iListBox->SetRect(Rect());
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CWPCxContainer::CountComponentControls() const
    {
    return 1;
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CWPCxContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CWPCxContainer::HandleControlEventL( CCoeControl* /*aControl*/,
                                          TCoeEvent /*aEventType*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CWPCxContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                             TEventCode aType)
    {
    switch ( aKeyEvent.iCode )
        {
        case EKeyBackspace:
            {
            iAppView.ProcessCommandL( EProvisioningCxCmdDeleteContext );
            break;
            }
        case EKeyOK:
            {
            iAppView.ProcessCommandL( EProvisioningCxCmdSelect );
            break;
            }
        default:
            {
            break;     
            }
        }
    
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CWPCxContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CWPCxContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iContext = KPROV_HLP_CONFCONT() ;    
	aContext.iMajor=TUid::Uid( KGSUid );
    }

// ---------------------------------------------------------------------------
// CWPCxContainer::HandleResourceChange
//  
// ---------------------------------------------------------------------------

void CWPCxContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
		    DrawNow();
        }
    CCoeControl::HandleResourceChange( aType );
    }
    

// End of File  
