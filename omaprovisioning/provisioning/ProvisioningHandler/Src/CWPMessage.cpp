/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main class for handling provisioning message
*
*/



// INCLUDE FILES
#include <biodb.h>      // BIO Message Database and message query methods
#include <msvuids.h>    // KUidMsvMessageEntry const
#include <biouids.h>    // KUidBioMessageTypeMtm const
#include <msvapi.h>
#include <msvids.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <smut.h>
#include <smuthdr.h>
#include <provisioninghandlerresource.rsg>
#include <pushmessage.h>
#include <CWPEngine.h>
#include <WPAdapterUtil.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <csmsgetdetdescinterface.h>
#include "MWPPhone.h"
#include "WPPhoneFactory.h"
#include "CWPBootstrap.h"
#include "CWPMessage.h"
#include "CWPPushMessage.h"
#include "WPHandlerDebug.h"
#include "WPHandlerUtil.h"
#include "ProvisioningDebug.h"
#include "ProvisioningInternalCRKeys.h"


// CONSTANTS
_LIT(KResourceName, "ProvisioningHandler");

_LIT(KDirAndFile,"z:ProvisioningHandler.rsc");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPMessage::CWPMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPMessage::CWPMessage()
    {
    }

// -----------------------------------------------------------------------------
// CWPMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPMessage::ConstructL( CPushMessage& aMessage )
    {
    FLOG( _L( "CWPMessage::ConstructL: Opening msg session" ) );

    iSession = CMsvSession::OpenSyncL(*this);

    FLOG( _L( "CWPMessage::ConstructL: Constructing push message" ) );
    iMessage = CWPPushMessage::NewL();

    TPtrC8 bodyValue;
    TPtrC8 headerValue;
    if (aMessage.GetMessageBody(bodyValue)
        && aMessage.GetHeader(headerValue) )
        {
        iMessage->SetL( headerValue, bodyValue );
        }

    TPtrC8 serverValue;
    if( aMessage.GetServerAddress( serverValue ) )
        {
        FLOG( _L( "CWPMessage::Begin originator" ) );
        DHEX( serverValue );
        FLOG( _L( "CWPMessage::End originator" ) );
        iMessage->SetOriginatorL( serverValue );
        iSender = serverValue.AllocL();
        }

    FLOG( _L( "CWPMessage::Begin header" ));
    DHEX( iMessage->Header() );
    FLOG( _L( "CWPMessage::Begin message body" ) );
    DHEX( iMessage->Body() );
    FLOG( _L( "CWPMessage::End message" ) );

    FLOG( _L( "CWPMessage::ConstructL: Retrieving BIO message type" ) );
    BioMessageTypeL( aMessage );

    FLOG( _L( "CWPMessage::ConstructL: Finished" ) );
    }

// -----------------------------------------------------------------------------
// CWPMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPMessage* CWPMessage::NewL( CPushMessage& aMessage )
    {
    CWPMessage* self = new(ELeave) CWPMessage; 
    CleanupStack::PushL(self);
    self->ConstructL( aMessage );
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPMessage::~CWPMessage()
    {
    delete iPhone;
    delete iSession;
    delete iSender;
    delete iMessage;
    }

// -----------------------------------------------------------------------------
// CWPMessage::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CWPMessage::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    }

// -----------------------------------------------------------------------------
// CWPMessage::ProcessL
// -----------------------------------------------------------------------------
//
void CWPMessage::ProcessL()
    {
    FLOG( _L( "CWPMessage::ProcessL: Started" ) );

    // Create a phone if one does not already exist
    if( !iPhone )
        {
        iPhone = WPPhoneFactory::CreateL();
        }

    FLOG( _L( "CWPMessage::ProcessL: Creating engine" ) );

    // Read the message into the engine
    CWPEngine* engine = CWPEngine::NewLC(); // on CS

    FLOG( _L( "CWPMessage::ProcessL: Importing document" ) );
    engine->ImportDocumentL( iMessage->Body() );

    FLOG( _L( "CWPMessage::ProcessL: Populating adapters" ) );
    engine->PopulateL();

    FTRACE(RDebug::Print(_L(" WPMessage::ProcessL: Number of settings: (%d)"), engine->ItemCount()));
    // Check if the sender can be trusted
    TPtrC8 orig8( iMessage->Originator() );
    HBufC* originator = HBufC::NewLC( orig8.Length() );
    TPtr orig16( originator->Des() );
    orig16.Copy( orig8 );
    if( engine->ContextExistsL( orig16 ) )
        {
        iMessage->SetAuthenticated( ETrue );
        }
    CleanupStack::PopAndDestroy(); // originator
    
    // Try bootstrapping
    FLOG( _L( "CWPMessage::ProcessL: Bootstrapping" ) );
    CWPBootstrap* bootstrap = CWPBootstrap::NewL( iPhone->SubscriberId() );
    CleanupStack::PushL( bootstrap );

    CWPBootstrap::TBootstrapResult result( 
        bootstrap->BootstrapL( *iMessage, *engine, KNullDesC ) );
    CleanupStack::PopAndDestroy(); // bootstrap

    FTRACE(RDebug::Print(_L(" WPMessage::ProcessL: Bootstrap result: (%d)"), result));
        // See UI specs figure 1 
    TBool haveSettings( engine->ItemCount() > 0 );
    switch( result )
        {
        case CWPBootstrap::ENoBootstrap:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping ENoBootstrap" ) );
            // If there's no bootstrap, just save the message
            if( haveSettings )
                {
                StoreMsgL();
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }

        case CWPBootstrap::ENotAuthenticated:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping ENotAuthenticated" ) );
            if( haveSettings )
                {
                StoreMsgL();
                }
            else
            {
                
            // Message is ignored and an information SMs is put to Inbox.
            //Information SMs can be Class0,  based on operator requirement.
            TInt auth_value;
            CRepository * rep = 0;
            TRAPD( err, rep = CRepository::NewL( KCRUidOMAProvisioningLV ));
            if(err == KErrNone)
            {
            	rep->Get( KOMAProvAuthFailMsgHandling, auth_value );
            	delete rep;
            	
            	/*if(auth_value == 1)
            	StoreMsgclass0L(R_TEXT_AUTHENTICATION_FAILED);
            	else
            	StoreMsgL(R_TEXT_AUTHENTICATION_FAILED);*/
            	
            	User::Leave( KErrAccessDenied ); 
            }
            else
            {
            //StoreMsgL(R_TEXT_AUTHENTICATION_FAILED);
            User::Leave( KErrAccessDenied );                         
            }
                }
            break;
            }

        case CWPBootstrap::EPinRequired:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping EPinRequired" ) );
            
            // If PIN is required, defer authentication to ProvisioningBC
            if( haveSettings )
                {
                StoreMsgL();
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }

        case CWPBootstrap::EAuthenticationFailed:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping EAuthenticationFailed" ) );
            
            // Bootstrap is ignored and an information SMS is put to Inbox.
            //Information SMs can be Class0,  based on operator requirement.
            TInt auth_value;
            CRepository * rep = 0;
            TRAPD( err, rep = CRepository::NewL( KCRUidOMAProvisioningLV ));
            if(err == KErrNone)
            {
            	rep->Get( KOMAProvAuthFailMsgHandling, auth_value );
            	delete rep;
            	
            	/*if(auth_value == 1)
            	StoreMsgclass0L(R_TEXT_AUTHENTICATION_FAILED);
            	else
            	StoreMsgL(R_TEXT_AUTHENTICATION_FAILED);*/
            	
            	User::Leave( KErrAccessDenied );
            	
            }
            else
            {
            //StoreMsgL(R_TEXT_AUTHENTICATION_FAILED);
            User::Leave( KErrAccessDenied );
            }
            break;
            }

        case CWPBootstrap::EBootstrapExists:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping EBootstrapExists" ) );
            
            if( haveSettings )
                {
                StoreMsgL();
                }
            else
                {
                // Bootstrap is ignored and an information SMS is put to Inbox
                StoreMsgL( R_TEXT_BOOTSTRAP_EXISTS );
                User::Leave( KErrAlreadyExists );
                }
            break;
            }

        case CWPBootstrap::ESucceeded:
            {
            FLOG( _L( "CWPMessage::ProcessL: Bootstrapping ESucceeded" ) );
            
            if( haveSettings )
                {
                StoreMsgL();
                }
            else
                {
                StoreMsgL( R_TEXT_BOOTSTRAPPED );
                }
            break;
            }

        default:
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // engine

    FLOG( _L( "CWPMessage::ProcessL: Finished" ) );
    }

// -----------------------------------------------------------------------------
// CWPMessage::BioMessageTypeL
// -----------------------------------------------------------------------------
//
void CWPMessage::BioMessageTypeL( CPushMessage& aMessage )
    {
    FLOG( _L( "CWPMessage::BioMessageTypeL" ) );
    
    // need to create local RFs for BIO otherwise raises exception
    RFs localFS;
    User::LeaveIfError(localFS.Connect());
    CleanupClosePushL(localFS);
    CBIODatabase* bioDB = CBIODatabase::NewLC(localFS);

    TPtrC contentTypePtr;
    aMessage.GetContentType(contentTypePtr);
    
    iBioUID = KNullUid;
    // IsBioMessageL returns KErrNone if found or KErrNotFound if not found 
    TInt retVal = bioDB->IsBioMessageL(EBioMsgIdIana, contentTypePtr, NULL, iBioUID);
    FTRACE(RDebug::Print(_L(" CWPMessage::BioMessageTypeL (%d)"), retVal));
    CleanupStack::PopAndDestroy(2); // bioDB, localFS
    }

// -----------------------------------------------------------------------------
// CWPMessage::StoreMsgL
// -----------------------------------------------------------------------------
//
void CWPMessage::StoreMsgL()
    {
    FLOG( _L( "CWPMessage::StoreMsgL" ) );
    
    // create an invisible blank entry 
    TMsvEntry entry;
    PrepareEntryLC( entry ); // details on cleanup stack
    entry.iBioType = iBioUID.iUid;
    entry.iMtm = KUidBIOMessageTypeMtm;
    
    // Look up and set the description
    FLOG( _L( "CWPMessage::StoreMsgL 1" ) );
    
    TInt index;
    CBIODatabase* bioDB = CBIODatabase::NewLC( iSession->FileSession() );
    FLOG( _L( "CWPMessage::StoreMsgL 2" ) );
    TRAPD( err, bioDB->GetBioIndexWithMsgIDL( iBioUID, index ) );
    if (err ==KErrNone)
        {
        FLOG( _L( "CWPMessage::StoreMsgL 3" ) );
        HBufC* description = bioDB->BifReader(index).Description().AllocL();
        FLOG( _L( "CWPMessage::StoreMsgL 4" ) );
        entry.iDescription.Set(*description);
        FLOG( _L( "CWPMessage::StoreMsgL 5" ) );
        CleanupStack::PopAndDestroy();  // bioDB
        CleanupStack::PushL( description );
        }
    else
        {
        FTRACE(RDebug::Print(_L(" CWPMessage::StoreMsgL err (%d)"), err));
        CleanupStack::PopAndDestroy();  // bioDB
        }
        
    FLOG( _L( "CWPMessage::StoreMsgL 6" ) );
    // Store entry in inbox
    CMsvEntry* msvEntry = iSession->GetEntryL( KMsvGlobalInBoxIndexEntryId );
    FLOG( _L( "CWPMessage::StoreMsgL 7" ) );
    CleanupStack::PushL(msvEntry);
    msvEntry->CreateL(entry);
    msvEntry->Session().CleanupEntryPushL(entry.Id());
    msvEntry->SetEntryL(entry.Id());
    FLOG( _L( "CWPMessage::StoreMsgL 8" ) );
    // Save the message
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store);
    FLOG( _L( "CWPMessage::StoreMsgL 9" ) );
    iMessage->StoreL( *store );
    store->CommitL();
    
    // Complete processing the message
    PostprocessEntryL( *msvEntry, entry );

    CleanupStack::PopAndDestroy(); //store
    msvEntry->Session().CleanupEntryPop(); //entry
    CleanupStack::PopAndDestroy(3); //description, details, msvEntry
    FLOG( _L( "CWPMessage::StoreMsgL Done" ) );
    }

// -----------------------------------------------------------------------------
// CWPMessage::StoreMsgL
// -----------------------------------------------------------------------------
//
void CWPMessage::StoreMsgL( TInt aResource )
    {
    FLOG( _L( "CWPMessage::StoreMsgL(aResource)" ) );
    
    // create an invisible blank entry 
    TMsvEntry entry;
    PrepareEntryLC( entry ); // details on cleanup stack
    entry.iMtm = KUidMsgTypeSMS;     

    // Store entry in inbox
    CMsvEntry* msvEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(msvEntry);
    msvEntry->CreateL(entry);
    msvEntry->Session().CleanupEntryPushL(entry.Id());
    msvEntry->SetEntryL(entry.Id());

    // Save the message body
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store);
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* body = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( body );
    HBufC* text = LoadStringLC( aResource );
    body->InsertL( body->DocumentLength(), *text );
    store->StoreBodyTextL( *body );

    // Store the actual message for post-mortem analysis
    iMessage->StoreL( *store );

    // Save the SMS header and create a description field
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *body );
    CleanupStack::PushL( header );
    TBuf<KSmsDescriptionLength> description;

    CSmsGetDetDescInterface* smsPlugin = CSmsGetDetDescInterface::NewL();
    CleanupStack::PushL( smsPlugin );
    smsPlugin->GetDescription( header->Message(), description );
    CleanupStack::PopAndDestroy( smsPlugin );

    entry.iDescription.Set( description );
    header->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( 5 ); // header, text, body, charformat, paraFormat

    // Complete processing the message
    PostprocessEntryL( *msvEntry, entry );

    CleanupStack::PopAndDestroy(); //store
    msvEntry->Session().CleanupEntryPop(); //entry
    CleanupStack::PopAndDestroy(2); //details, msvEntry
    }
    
// -----------------------------------------------------------------------------
// CWPMessage::StoreMsgclass0L
// -----------------------------------------------------------------------------
//
void CWPMessage::StoreMsgclass0L( TInt aResource )
    {
    FLOG( _L( "CWPMessage::StoreMsgL(aResource)" ) );
    
    // create an invisible blank entry 
    TMsvEntry entry;
    PrepareEntryLC( entry ); // details on cleanup stack
    entry.iMtm = KUidMsgTypeSMS;     

    // Store entry in inbox
    CMsvEntry* msvEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(msvEntry);
    msvEntry->CreateL(entry);
    msvEntry->Session().CleanupEntryPushL(entry.Id());
    msvEntry->SetEntryL(entry.Id());

    // Save the message body
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store);
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* body = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( body );
    HBufC* text = LoadStringLC( aResource );
    body->InsertL( body->DocumentLength(), *text );
    store->StoreBodyTextL( *body );

    // Store the actual message for post-mortem analysis
    iMessage->StoreL( *store );

    
   
    
    // Save the SMS header and create a description field
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *body );
    
    
    CleanupStack::PushL( header );
    TBuf<KSmsDescriptionLength> description;

    
    //class 0 Implementation
    CSmsMessage &msg = header->Message();
	CSmsPDU &pdu = msg.SmsPDU(); 
	pdu.SetClass(ETrue,TSmsDataCodingScheme::ESmsClass0);

 	CSmsGetDetDescInterface* smsPlugin = CSmsGetDetDescInterface::NewL();
    CleanupStack::PushL( smsPlugin );
    smsPlugin->GetDescription( msg , description );
    CleanupStack::PopAndDestroy( smsPlugin );

    entry.iDescription.Set( description );
    header->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( 5 ); // header, text, body, charformat, paraFormat

    // Complete processing the message
    PostprocessEntryL( *msvEntry, entry );

    CleanupStack::PopAndDestroy(); //store
    msvEntry->Session().CleanupEntryPop(); //entry
    CleanupStack::PopAndDestroy(2); //details, msvEntry
    }

// -----------------------------------------------------------------------------
// CWPMessage::PrepareEntry
// -----------------------------------------------------------------------------
//
void CWPMessage::PrepareEntryLC( TMsvEntry& aEntry )
    {
    FLOG( _L( "CWPMessage::PrepareEntryLC" ) );
    
    // Current time
    TTime time;

	// Get Universal time
	time.UniversalTime();

	FLOG( _L( "CWPMessage::PrepareEntryLC create an invisible blank entry" ) );	
    // create an invisible blank entry 
    aEntry.iType = KUidMsvMessageEntry;
    aEntry.SetVisible(EFalse);
    aEntry.SetInPreparation(ETrue);
    aEntry.SetReadOnly(EFalse);
    aEntry.SetUnread(ETrue);
    aEntry.iDate = time;
    aEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    aEntry.iError = KErrNone;
    // iMtmData1 is been used/reserved for count, please don't use for any other purpose.
    aEntry.SetMtmData1(3);
    FLOG( _L( "CWPMessage::PrepareEntryLC create an invisible blank entry done" ) );
    // Look up the details
    HBufC* details = NULL;
    if( iMessage->Authenticated() )
        {
        FLOG( _L( "CWPMessage::PrepareEntryLC iMessage->Authenticated() true" ) );
        details = LoadStringLC( R_FROM_SERVICEPROVIDER );
        FLOG( _L( "CWPMessage::PrepareEntryLC LoadString done" ) );
        }
    else
        {
        FLOG( _L( "CWPMessage::PrepareEntryLC iMessage->Authenticated() false" ) );
        if( iSender )
            {
            FLOG( _L( "CWPMessage::PrepareEntryLC iSender true" ) );
            details = HBufC::NewLC( iSender->Length() );
            details->Des().Copy( *iSender );
            }
        else
            {
            FLOG( _L( "CWPMessage::PrepareEntryLC iSender false" ) );
            details = KNullDesC().AllocLC();
            }
        }
    FLOG( _L( "CWPMessage::PrepareEntryLC iDetails.Set" ) );
    aEntry.iDetails.Set( *details );
    FLOG( _L( "CWPMessage::PrepareEntryLC Done" ) );
    }

// -----------------------------------------------------------------------------
// CWPMessage::PostprocessEntryL
// -----------------------------------------------------------------------------
//
void CWPMessage::PostprocessEntryL( CMsvEntry& aCEntry, TMsvEntry& aTEntry )
    {
    FLOG( _L( "CWPMessage::PostprocessEntryL" ) );
    
    aTEntry.SetReadOnly(EFalse);
    aTEntry.SetVisible(ETrue);
    aTEntry.SetInPreparation(EFalse);
    aCEntry.ChangeL(aTEntry);
    }

// -----------------------------------------------------------------------------
// CWPMessage::LoadStringLC
// -----------------------------------------------------------------------------
//
HBufC* CWPMessage::LoadStringLC( TInt aResourceId )
    {
    FLOG( _L( "CWPMessage::LoadStringLC" ) );

	TParse parse;
    FLOG( _L( "CWPMessage::LoadStringLC2" ) );
    parse.Set(KDirAndFile, &KDC_RESOURCE_FILES_DIR, NULL); 

    FLOG( _L( "CWPMessage::LoadStringLC3" ) );
    HBufC* result = WPAdapterUtil::ReadHBufCL( parse.FullName(), KResourceName, aResourceId );
    CleanupStack::PushL( result );
    
    FLOG( _L( "CWPMessage::LoadStringLC done" ) );
    
    return result;
    }

//  End of File  
