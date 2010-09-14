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
* Description: 
*     Bio control for Provisioning documents.
*
*/


// INCLUDE FILES
#include "CWPBioControl.h"

#include <aknnotewrappers.h>
#include <msvapi.h>
#include <StringLoader.h>
#include <CRichBio.h>
#include <msgbiocontrolObserver.h>


#include <ProvisioningBC.rsg>
#include <bldvariant.hrh>
#include <msvids.h>
#include <biouids.h>
#include <featmgr.h>
#include <csxhelp/prov.hlp.hrh>
#include <sysutil.h>
#include "CWPEngine.h"
#include "CWPAdapter.h"
#include "MWPPhone.h"
#include "WPPhoneFactory.h"
#include "CWPPushMessage.h"
#include "CWPBootstrap.h"
#include "CWPSaver.h"
#include "ProvisioningUIDs.h"
#include "CWPCodeQueryDialog.h"
#include "MWPContextManager.h"
#include "ProvisioningDebug.h"
#include "CWPNameValue.h"
#include <aknmessagequerydialog.h> 
#include "ProvisioningInternalCRKeys.h"
// CONSTANTS
/// Strings are loaded from this resource
_LIT(KResourceFileName, "provisioningbc.rsc");
/// Maximum number of items with same title before they get merged.
const TInt KMaxDuplicates = 5;
/// Maximum length of PIN code
const TInt KMaxPinSize = 20;
/// Number of retries for PIN
// const TInt KPinRetries = 3; // Removed, because iMtmData1 data member of TMsvEntry in CWPMessage.cpp contains Max tries, default=3.
///NONE
_LIT(KNone,"");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CWPBioControl::CWPBioControl( MMsgBioControlObserver& aObserver,
                              CMsvSession* aSession,
                              TMsvId aId,
                              TMsgBioMode aEditorOrViewerMode,
                              const RFile* aFile )
                            : CMsgBioControl( aObserver,
                                              aSession,
                                              aId,
                                              aEditorOrViewerMode,
                                              aFile )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBioControl::ConstructL()
    {
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL:" ) );
    
    FeatureManager::InitializeLibL();

    if( IsEditor() ) //This control does not support editing mode.
        {
        User::Leave( KErrNotSupported );
        }

    LoadResourceL(KResourceFileName);
    LoadStandardBioResourceL();

    iEngine = CWPEngine::NewL();
    iRichBio = new(ELeave) CRichBio( ERichBioModeEditorBase );

    // this function leaves if not enough data exists
    RestoreMsgL();

    // Add items to CRichBio
    AddItemsL();
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL: done" ) );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPBioControl* CWPBioControl::NewL( MMsgBioControlObserver& aObserver,
                                             CMsvSession* aSession,
                                             TMsvId aId,
                                             TMsgBioMode aEditorOrViewerMode,
                                             const RFile* aFile )
    {

    CWPBioControl* self = new( ELeave ) CWPBioControl( aObserver,
                                                       aSession,
                                                       aId,
                                                       aEditorOrViewerMode,
                                                       aFile );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPBioControl::~CWPBioControl()
    {
    delete iRichBio;
    delete iEngine;
    delete iMessage;
    FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------------------------------
// CWPBioControl::SetAndGetSizeL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetAndGetSizeL( TSize& aSize )
    {
    iRichBio->SetAndGetSizeL( aSize );
    SetSizeWithoutNotification( aSize );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::SetMenuCommandSetL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetMenuCommandSetL( CEikMenuPane& aMenuPane )
    {
    AddMenuItemL( aMenuPane, R_MENU_SAVESETTING, ECmdSaveSettings );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::CurrentLineRect
// ----------------------------------------------------------------------------
//
TRect CWPBioControl::CurrentLineRect() const
    {
    return iRichBio->CurrentLineRect();
    }

// ----------------------------------------------------------------------------
// CWPBioControl::IsFocusChangePossible
// ----------------------------------------------------------------------------
//
TBool CWPBioControl::IsFocusChangePossible(
                                         TMsgFocusDirection aDirection ) const
    {
    if (aDirection == EMsgFocusUp )
        {
        return iRichBio->IsCursorLocation( EMsgTop );
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CWPBioControl::IsCursorLocation
// ----------------------------------------------------------------------------
//
TBool CWPBioControl::IsCursorLocation( TMsgCursorLocation aLocation ) const
    {
    return iRichBio->IsCursorLocation( aLocation );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::HeaderTextL
// ----------------------------------------------------------------------------
//
HBufC* CWPBioControl::HeaderTextL() const
    {
    return StringLoader::LoadL( R_TITLE_PROVISIONING );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::HandleBioCommandL()
// ----------------------------------------------------------------------------
//
TBool CWPBioControl::HandleBioCommandL(TInt aCommand)
    {
    TBool result( EFalse );

    if ( aCommand == iBioControlObserver.FirstFreeCommand() + ECmdSaveSettings )
        {
        SaveSettingsL();
        result = ETrue;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CWPBioControl::OptionMenuPermissionsL
// ----------------------------------------------------------------------------
//
TUint32 CWPBioControl::OptionMenuPermissionsL() const
    {
    TUint32 perm( EMsgBioDelete | EMsgBioMessInfo | EMsgBioMove | EMsgBioExit );

    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        perm |= EMsgBioHelp;
        }
    return perm;
    }

// ----------------------------------------------------------------------------
// CWPBioControl::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CWPBioControl::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                            TEventCode aType )
    {
    return iRichBio->OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::GetBioHelpContext
// ----------------------------------------------------------------------------
//
void CWPBioControl::GetHelpContext( TCoeHelpContext& aContext ) const
    {   
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        aContext.iContext = KPROV_HLP_PROVISIONVIEW() ;    
        aContext.iMajor = TUid::Uid( KProvisioningBCUid3 );
        }
    else
        {
        CCoeControl::GetHelpContext(aContext);
        }
    }

// ----------------------------------------------------------------------------
// CWPBioControl::CountComponentControls
// ----------------------------------------------------------------------------
//
TInt CWPBioControl::CountComponentControls() const
    {
    return 1;
    }

// ----------------------------------------------------------------------------
// CWPBioControl::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CWPBioControl::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iRichBio;
    }

// ----------------------------------------------------------------------------
// CWPBioControl::SizeChanged()
// ----------------------------------------------------------------------------
//
void CWPBioControl::SizeChanged()
    {
    iRichBio->SetExtent( Position(), iRichBio->Size() );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::FocusChanged
// ----------------------------------------------------------------------------
//
void CWPBioControl::FocusChanged( TDrawNow aDrawNow )
    {
    iRichBio->SetFocus( IsFocused(), aDrawNow );
    }

// ----------------------------------------------------------------------------
// CWPBioControl::SetContainerWindowL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRichBio->ConstructL(this);
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::VirtualHeight
// ----------------------------------------------------------------------------
//
TInt CWPBioControl::VirtualHeight()
    {
    return iRichBio->VirtualHeight();
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::VirtualVisibleTop
// ----------------------------------------------------------------------------
//
TInt CWPBioControl::VirtualVisibleTop()
    {
    return iRichBio->VirtualVisibleTop();
    }


// ----------------------------------------------------------------------------
// CWPBioControl ::RestoreMsgL
// ----------------------------------------------------------------------------
//
void CWPBioControl::RestoreMsgL()
    {
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL" ) );
    
    // Load the message from Messaging Server
    CMsvEntry* entry = MsvSession().GetEntryL(iId);
    CleanupStack::PushL( entry );
    TMsvEntry tentry( entry->Entry() );
    iMessage = CWPPushMessage::NewL();
    CMsvStore* readStore = entry->ReadStoreL();
    CleanupStack::PushL( readStore );
    iMessage->RestoreL( *readStore );
    CleanupStack::PopAndDestroy(); // readStore
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 1" ) );
    // Fill the engine
    TRAPD( result, iEngine->ImportDocumentL( iMessage->Body() ) );
    if( result == KErrCorrupt )
        {
        FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL KErrMsgBioMessageNotValid " ) );
        result = KErrMsgBioMessageNotValid;
        }
    FTRACE(RDebug::Print(_L("[ProvisioningBC] CWPBioControl::RestoreMsgL result (%d)"), result));
    User::LeaveIfError( result );
    
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 2 " ) );
    TBool preAuthenticated( iMessage->Authenticated() );
    // If the message was not authenticated, give a warning
    AuthenticateL( *iMessage );
    
    // Identify the authentication type
    TInt type = (entry->Entry()).MtmData2();
    if(type == 1)
    {    
    CRepository* repository = CRepository::NewLC( KOMAProvAuthenticationLV );	
    TInt err = repository->Set(KOMAProvMessageOpen, type);  
    if(err != KErrNone)
    	{
    		FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL Set KOMAProvMessageOpen failed" ) );
    	}    
    CleanupStack::PopAndDestroy(); // repository 
    }
    	
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 3 " ) );
    if( !tentry.ReadOnly() && preAuthenticated != iMessage->Authenticated() )
        {
        // If the entry can be written to, get its edit store and save
        // authentication flag. Failing is not dangerous, as the only
        // effect is that the user has to re-enter the PIN.
        FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 4 " ) );
        TRAPD( ignoreError, SaveMessageL() );
        if ( ignoreError ) ignoreError = 0; // prevent compiler warning	
        }
    
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 5 " ) );
    iEngine->PopulateL();
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 6 " ) );
    
    //Reset the cenrep so it can used for next message.		
	 CRepository* repository = CRepository::NewLC( KOMAProvAuthenticationLV );	
    TInt err = repository->Set(KOMAProvMessageOpen, 0); 
    if(err != KErrNone)
    	{
    		FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL Set KOMAProvMessageOpen failed" ) );
    	} 	     
    CleanupStack::PopAndDestroy();
    	
    // Empty messages are not supported
    if( iEngine->ItemCount() == 0 )
        {
        FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 7 " ) );
        User::Leave( KErrMsgBioMessageNotValid );
        }

    TPtrC8 orig8( iMessage->Originator() );
    HBufC* orig16 = HBufC::NewLC( orig8.Length() );
    orig16->Des().Copy( orig8 );
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 8 " ) );
    if( iMessage->Authenticated() 
        && iEngine->ContextExistsL( *orig16 ) )
        {
        TUint32 context( iEngine->ContextL( *orig16 ) );
        iEngine->SetCurrentContextL( context );
        }
    else
        {
        iEngine->SetCurrentContextL( KWPMgrUidNoContext );
        }
	
    CleanupStack::PopAndDestroy(2); // orig16, entry
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL done" ) );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::AuthenticateL
// ----------------------------------------------------------------------------
//
void CWPBioControl::AuthenticateL( CWPPushMessage& aMessage )
    {
    if( !aMessage.Authenticated() )
        {
        // Try authentication and bootstrap without PIN.
        
	
	      MWPPhone* phone = NULL;
        TBuf<KMaxPinSize> imsi;
        TRAPD(err, phone = WPPhoneFactory::CreateL());
	
	     if(err!=KErrNone)
        {
		      imsi = KNone;
		
        }
        else
        {
        	imsi = phone->SubscriberId();
        }
        
        
        CWPBootstrap* bootstrap = CWPBootstrap::NewL( imsi );
        
        if(phone)
        delete phone;

	      CleanupStack::PushL( bootstrap );

        CWPBootstrap::TBootstrapResult result( bootstrap->BootstrapL( 
                                                                aMessage, 
                                                                *iEngine, 
                                                                KNullDesC ) );
        if( result == CWPBootstrap::EPinRequired )
            {
            	CMsvEntry* entry = MsvSession().GetEntryL(iId);
				TInt count = (entry->Entry()).MtmData1();
            // We need to ask the PIN. Retry max. MtmData1 (=3, default) times if
            // the user supplies from PIN.
            TBuf<KMaxPinSize> pin;
	            for( TInt i(0); i < count && !aMessage.Authenticated(); i++ )
                {
                pin.Zero();
				CWPCodeQueryDialog* dlg = 
				                        new (ELeave) CWPCodeQueryDialog( pin );
				if (dlg->ExecuteLD( R_QUERY_PIN ) == 0) //R_DATA_QUERY
				    {
					    delete entry;
					    entry = NULL;
                    //enable user cancelling at any phase
                    User::Leave( KLeaveWithoutAlert );
				    }
                result = bootstrap->BootstrapL( aMessage, *iEngine, pin );

                // Wrong PIN. Give a note except on the last time. 
                // Then the user will get note 
                // R_QUERY_VERIFIC_FAILED_TOTALLY with OK key.
                if( result == CWPBootstrap::EAuthenticationFailed 
	                    && i < (const_cast <TMsvEntry&> (entry->Entry())).iMtmData1--)
                    {
	                      entry->ChangeL(entry->Entry());
						i--;	                    
						count = (entry->Entry()).MtmData1(); //(const_cast <TMsvEntry&> (entry->Entry())).iMtmData1;
                    ErrorNoteL( R_TEXT_WRONG_PIN );
                    }
                }

            // Still wrong PIN -> tell the user and delete the message.
            if( !aMessage.Authenticated() )
                {
					delete entry;
					entry = NULL;
                CAknNoteDialog* dlg = new(ELeave) CAknNoteDialog;
                dlg->ExecuteLD(R_QUERY_VERIFIC_FAILED_TOTALLY);
                MsvSession().RemoveEntry( iId );
                User::Leave( KLeaveWithoutAlert );
                }
	             if(entry)
	             {
	             	delete entry;
	             }
            }
        if(result == CWPBootstrap::EAuthenticationFailed)
        {
        		
        		CAknMessageQueryDialog* msg =  new(ELeave) CAknMessageQueryDialog;
        		msg->ExecuteLD(R_MSG_AUTHENTICATION_FAILED);

			MsvSession().RemoveEntry( iId );
        		
        		User::Leave( KLeaveWithoutAlert );
        		
        		
        }
        CleanupStack::PopAndDestroy( bootstrap );

        // If the message is still not authenticated, show a note to user.
        if( !aMessage.Authenticated() )
            {   
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        
            if ( !dlg->ExecuteLD(R_QUERY_NOTAUTHENTICATED) )
                {                
                User::Leave( KLeaveWithoutAlert );
                }
            }
        }
    if (aMessage.Authenticated() )
        {
        //update Cenrep key
        TBuf8<100> orig;
        orig.Copy(aMessage.Originator());
        TRAPD(err, SetCenrepKeyL(orig));
        User::LeaveIfError(err);

        }
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::AddItemsL
// ----------------------------------------------------------------------------
//
void CWPBioControl::AddItemsL()
    {
    // Add help text before the actual items
    HBufC* help = StringLoader::LoadLC( R_TEXT_HELPTEXT );
    iRichBio->AddItemL( *help, KNullDesC );
    CleanupStack::PopAndDestroy(); // help

    RPointerArray<CWPNameValue>* array = CollectItemsLC();
    CollateAndAddItemsL( *array );
    CleanupStack::PopAndDestroy(); // array
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::CollectItemsL
// ----------------------------------------------------------------------------
//
RPointerArray<CWPNameValue>* CWPBioControl::CollectItemsLC() const
    {
    RPointerArray<CWPNameValue>* array = 
                                    new(ELeave) RPointerArray<CWPNameValue>;
    CleanupStack::PushL( TCleanupItem( Cleanup, array ) );

    TInt count( iEngine->ItemCount() );
    for( TInt index = 0; index < count; index++ )
        {
        const TDesC& text = iEngine->SummaryText( index );
        const TDesC& title = iEngine->SummaryTitle( index );

        CWPNameValue* pair = CWPNameValue::NewLC( title, text );
        User::LeaveIfError( array->Append( pair ) );
        CleanupStack::Pop( pair );
        }

    array->Sort( TLinearOrder<CWPNameValue>( CWPBioControl::Compare ) );

    return array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Cleanup
// -----------------------------------------------------------------------------
//
void CWPBioControl::Cleanup( TAny* aAny )
    {
    RPointerArray<CWPNameValue>* array = 
                       reinterpret_cast<RPointerArray<CWPNameValue>*>( aAny );
    array->ResetAndDestroy();
    array->Close();
    delete array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Compare
// -----------------------------------------------------------------------------
//
TInt CWPBioControl::Compare( const CWPNameValue& aItem1,
                             const CWPNameValue& aItem2 )
    {
    return aItem1.Name().Compare( aItem2.Name() );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::CollateAndAddItemsL
// ----------------------------------------------------------------------------
//
void CWPBioControl::CollateAndAddItemsL( RPointerArray<CWPNameValue>& aItems )
    {
    // Add a pivot item
    CWPNameValue* pivot = CWPNameValue::NewLC( KNullDesC, KNullDesC );
    User::LeaveIfError( aItems.Append( pivot ) );
    CleanupStack::Pop( pivot );

    // Title of the items being currently collected
    TPtrC currentTitle( KNullDesC );
    // Items under the same title
    RArray<TPtrC> pointers;
    CleanupClosePushL( pointers );
    for( TInt item = 0; item < aItems.Count(); item++ )
        {
        CWPNameValue* pair = aItems[ item ];

        if( pair->Name().Compare( currentTitle ) != 0 )
            {
            // Title has changed. Inserted collected items into rich bio
            if( pointers.Count() > KMaxDuplicates )
                {
                // More than KMaxDuplicates items. Put only one line with
                // number of items.
                HBufC* string = StringLoader::LoadLC( R_TEXT_NUMITEMS,
					                                  pointers.Count() );
                iRichBio->AddItemL( currentTitle, *string );
                CleanupStack::PopAndDestroy( string );
                }
            else
                {
                // Insert all collected items into rich bio
                for( TInt index = 0; index < pointers.Count(); index++ )
                    {
                    iRichBio->AddItemL( currentTitle, pointers[index] );
                    // Make sure title is repeated only once
                    currentTitle.Set( KNullDesC );
                    }
			    } 
            // Change the current title and start collecting for it
            currentTitle.Set( pair->Name() );
            pointers.Reset();
            }
		if(  pair->Value().Compare( KNullDesC ) != 0  )
			{
			User::LeaveIfError( pointers.Append( pair->Value() ) );
			}
        }
    // Delete pivot
    aItems.Remove( aItems.Count() - 1 );
    delete pivot;
    CleanupStack::PopAndDestroy( &pointers );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::SaveSettingsL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SaveSettingsL()
    {
    if( SysUtil::FFSSpaceBelowCriticalLevelL( &iEikonEnv->FsSession() ) )
        {
        User::Leave( KErrDiskFull );
        }

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    
    if( !iMessage->Saved())
    {
    	DoSaveL();
    	delete dlg;
    }
    
    else
    if( dlg->ExecuteLD(R_QUERY_ALREADY_SAVED) )
        {
        DoSaveL();
        }
    iMessage->SetSaved( ETrue );
    SaveMessageL();
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::DoSaveL
// ----------------------------------------------------------------------------
//
void CWPBioControl::DoSaveL()
    {
    TInt numSaved( 0 );
    CWPSaver* saver = new(ELeave) CWPSaver( *iEngine, EFalse );
    HBufC* text=NULL;
    TInt err( saver->ExecuteLD( numSaved ) );
    TInt numAccessDenied = saver->GetNumAccessDenied();
    TInt itemCount( iEngine->ItemCount() );
    
	if( (numSaved != itemCount) && (iEngine->GetAccessDenied()) )
    	{
	  numSaved = numSaved + 1 - numAccessDenied;
	  iEngine->SetAccessDenied(EFalse);
    	  err= KErrAccessDenied;
    	}
    
    if( err >= 0 )
        {
        // Check if any setting can be made default
        TBool setDefault( EFalse );
        
        for( TInt i( 0 ); i < itemCount && !setDefault; i++ )
            {
            if( iEngine->CanSetAsDefault( i ) )
                {
                setDefault = ETrue;
                }
            }
        
        // Ask the user if the saved settings should be 
        // made default.
       TInt result = KErrNone;
        if( setDefault )
            {
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            
            if ( dlg->ExecuteLD(R_QUERY_SETDEFAULT) )
                {
                CWPSaver* setter = new(ELeave) CWPSaver( *iEngine, ETrue );
                TInt numSet( 0 );
                result = setter->ExecuteLD( numSet );
                }             
            }
        
        // All settings saved
        if (result)
            {
            if(numSaved == 0)
             	text = StringLoader::LoadLC( R_TEXT_NOTSAVED_SINGULAR_ZERO);
            else if(numSaved == 1)
            	text = StringLoader::LoadLC( R_TEXT_NOTSAVED_SINGULAR_ONE, numSaved );
    				else        
            	text = StringLoader::LoadLC( R_TEXT_NOTSAVED, numSaved );
            ErrorNoteL( *text );
            CleanupStack::PopAndDestroy(); // text
            }
        else
            {
            ConfirmationNoteL( R_TEXT_SAVED );
            }
        }
    else
        {
        if( err == EWPAccessPointsProtected )
            {
            // Some problem in saving or user cancelled.
            InformationNoteL( R_TEXT_AP_PROTECTED );
            }
        else
            {
            // Some problem in saving or user cancelled.
            if(numSaved == 0)
             	text = StringLoader::LoadLC( R_TEXT_NOTSAVED_SINGULAR_ZERO);
            else if(numSaved == 1)
            	text = StringLoader::LoadLC( R_TEXT_NOTSAVED_SINGULAR_ONE, numSaved );
    				else        
            	text = StringLoader::LoadLC( R_TEXT_NOTSAVED, numSaved );
            ErrorNoteL( *text );
            CleanupStack::PopAndDestroy( text );
            }
        }
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::SaveMessageL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SaveMessageL()
    {
    CMsvEntry* entry = MsvSession().GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* editStore = entry->EditStoreL();
    CleanupStack::PushL( editStore );
    iMessage->StoreL( *editStore );
    editStore->CommitL();
    CleanupStack::PopAndDestroy(2); // editStore, entry
    }

void CWPBioControl::ErrorNoteL(const TDesC& aText)
    {
    CAknErrorNote* note = new(ELeave)CAknErrorNote( ETrue );
    note->ExecuteLD(aText);
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::ErrorNoteL
// ----------------------------------------------------------------------------
//
void CWPBioControl::ErrorNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    ErrorNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::ConfirmationNoteL
// ----------------------------------------------------------------------------
//
void CWPBioControl::ConfirmationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    ConfirmationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }
    
// ----------------------------------------------------------------------------
// CWPBioControl ::InformationNoteL
// ----------------------------------------------------------------------------
//
void CWPBioControl::InformationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    InformationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }
    
// ----------------------------------------------------------------------------
// CWPBioControl ::ConfirmationNoteL
// ----------------------------------------------------------------------------
//
void CWPBioControl::ConfirmationNoteL(const TDesC& aText)
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote;
    note->ExecuteLD(aText);
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::InformationNoteL
// ----------------------------------------------------------------------------
//
void CWPBioControl::InformationNoteL(const TDesC& aText)
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote;
    note->ExecuteLD(aText);
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::SetCenrepKey
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetCenrepKeyL(const TDesC8& aValue)
    {

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL" ) );

    CRepository* rep= NULL;
    TInt errorStatus = KErrNone;

    TRAPD( errVal, rep = CRepository::NewL( KCRUidOMAProvisioningLV ))
    ;

    if (errVal == KErrNone)
        {
        errorStatus = rep->Set(KOMAProvOriginatorContent, aValue);
        }
    else
        {
        errorStatus = errVal;
        }

    if (rep)
        {
        delete rep;
        }

    if (errorStatus != KErrNone)
        {
        User::Leave(errorStatus);
        }

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL done" ) );

    }
//  End of File
