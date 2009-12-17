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
* Description:  Stores smart card provisioned settings into Inbox
*
*/



// INCLUDE FILES
#include	"CWPBioMsgHandler.h"
#include	"CWPEngine.pan"

#include	<biodb.h>		// Message DB API
#include	<msvapi.h>
#include	<msvuids.h>	// KUidMsvMessageEntry 
#include	<biouids.h>	// KUidBioMessageTypeMtm 
#include	<WPAdapterUtil.h>
#include	<mtclreg.h>	// mtm
#include	<mtclbase.h>
#include	<txtrich.h>
#include	<smsclnt.h>
#include	<smuthdr.h>
#include	<smutset.h>
#include	<ProvisioningSC.rsg> 
#include	<pushmessage.h>
#include	<e32std.h>  
#include	<sysutil.h>
#include	"CWPPushMessage.h"
#include	"WPProvScPaths.h"
#include	"ProvisioningDebug.h"

#include <csmsgetdetdescinterface.h>

// CONSTANTS
const TInt KHeaderSize = 1;

_LIT( KResourceName, "ProvisioningSC" );

GLDEF_C void Panic( TWPPanic aPanic )
    {
    User::Panic( KWPProvSCName, aPanic );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::CWPBioMsgHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPBioMsgHandler::CWPBioMsgHandler() : iSession(NULL), iMtm(NULL), 
      iMtmRegistry(NULL)
    {
    }


// -----------------------------------------------------------------------------
// CWPBioMsgHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::ConstructL()
    {
	iSession = CMsvSession::OpenSyncL( *this );
	iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPBioMsgHandler* CWPBioMsgHandler::NewL()
    {
	CWPBioMsgHandler* handler  = CWPBioMsgHandler::NewLC();
    CleanupStack::Pop();
    return handler;
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPBioMsgHandler* CWPBioMsgHandler::NewLC()
    {
    CWPBioMsgHandler* self = new( ELeave ) CWPBioMsgHandler;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CWPBioMsgHandler::~CWPBioMsgHandler
// Destructor
// -----------------------------------------------------------------------------
//
CWPBioMsgHandler::~CWPBioMsgHandler()
    {
    delete iMtm;
    delete iMtmRegistry;
    delete iSession;   
	delete iMessage;
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::HandleSessionEventL
// Handles async open msvsession
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::HandleSessionEventL(TMsvSessionEvent aEvent,
										   TAny* /* aArg1 */, TAny* /*aArg2*/,
										   TAny* /*aArg3*/)
    {
	switch ( aEvent ) 
        {
        case EMsvServerReady:
			{
            break;
			}
        case EMsvCloseSession:
			{
            iSession->CloseMessageServer();
            break;
			}
        case EMsvServerTerminated:
			{
            iSession->CloseMessageServer();
            break;
			}
        default:
			{
            break;
			}
        } 
    }
// -----------------------------------------------------------------------------
// CWPBioMsgHandler::StoreMessageL
// Stores provisioned settings into Inbox
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::StoreMessageL( CWPPushMessage& aMessage )
	{
	FLOG( _L( "[ProvisioningSC] CWPBioMsgHandler::StoreMessageL:" ) );

	TMsvEntry entry;
	// create blank entry
	PrepareEntryL( entry );
	// Look up the details
	TFileName fileName;
	#ifndef __WINS__
	fileName = RProcess().FileName();
	#else
	Dll::FileName( fileName );
	#endif

	HBufC* details = WPAdapterUtil::ReadHBufCL( fileName,
												KResourceName,
												R_FROM_SERVICEPROVIDER );
    CleanupStack::PushL( details );  // 1
	entry.iDetails.Set( *details );
	entry.iBioType = iBioUid.iUid;
	entry.iMtm = KUidBIOMessageTypeMtm;
	// Look up and set the description
	TInt index;
	CBIODatabase* bioDB = CBIODatabase::NewLC( iSession->FileSession() ); // 2
	bioDB->GetBioIndexWithMsgIDL( iBioUid, index);
	HBufC* description = bioDB->BifReader( index ).Description().AllocL();
	entry.iDescription.Set( *description );
	CleanupStack::PopAndDestroy(); // bioDB 2
	CleanupStack::PushL( description ); // 2 
	// store entry
	StoreEntryL( aMessage, entry );
	CleanupStack::PopAndDestroy( 2 ); // description, details
	}

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::StoreMessageL
// Stores note into Inbox
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::StoreMessageL( TInt aResource, CWPPushMessage& aMessage )
    {
    FLOG( _L( "[ProvisioningSC] CWPBioMsgHandler::StoreMessageL2:" ) );
	
    // create an invisible blank entry 
    TMsvEntry entry;
    PrepareEntryL( entry );
    entry.iMtm = KUidMsgTypeSMS;     

    // Store entry in inbox
    CMsvEntry* msvEntry = iSession->GetEntryL( KMsvGlobalInBoxIndexEntryId );
    CleanupStack::PushL( msvEntry );
    msvEntry->CreateL( entry );
    msvEntry->Session().CleanupEntryPushL( entry.Id() );
    msvEntry->SetEntryL( entry.Id() );

    // Save the message body
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL( store );
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* body = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( body );

	TFileName fileName;
	#ifndef __WINS__
	fileName = RProcess().FileName();
	#else
	Dll::FileName( fileName );
	#endif

	// Look up text
	HBufC* text = WPAdapterUtil::ReadHBufCL( fileName,
												KResourceName,
												aResource );
    CleanupStack::PushL( text );

	// set text
    body->InsertL( 0, *text );
    store->StoreBodyTextL( *body );

    // Store the actual message for post-mortem analysis
    aMessage.StoreL( *store );

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
    CleanupStack::PopAndDestroy( 5 ); // header,text,body,charformat,paraFormat

    // Complete processing the message
	entry.SetReadOnly( EFalse );
    entry.SetVisible( ETrue );
    entry.SetInPreparation( EFalse );
    msvEntry->ChangeL( entry );
    CleanupStack::PopAndDestroy(); //store
    msvEntry->Session().CleanupEntryPop(); //entry
    CleanupStack::PopAndDestroy( ); // msvEntry

    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::CreateMessageL
// passes the ownership of the created push message
// -----------------------------------------------------------------------------
//
CWPPushMessage* CWPBioMsgHandler::CreateMessageLC( TDesC8& aProvisioningDoc )
	{
	FLOG( _L( "[ProvisioningSC] CWPBioMsgHandler::CreateMessageLC begin" ) );
	CWPPushMessage* message = CWPPushMessage::NewLC();	
	CPushMessage* pushMsg = CreatePushMessageL( aProvisioningDoc );
	CleanupStack::PushL( pushMsg ); // 2
	TPtrC8 bodyValue;
	TPtrC8 headerValue;
	if (pushMsg->GetMessageBody( bodyValue )
		&& pushMsg->GetHeader( headerValue ) )
		{
		// set member message, takes copy of arguments
		message->SetL( headerValue, bodyValue );
		}
	// set member uid
	BioMessageTypeL( *pushMsg );
	CleanupStack::PopAndDestroy(); // pushMsg, 2
	FLOG( _L( "[ProvisioningSC] CWPBioMsgHandler::CreateMessageLC end" ) );
	return message; 
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::CreatePushMessageL
//  Stores note into Inbox
// -----------------------------------------------------------------------------
//
CPushMessage* CWPBioMsgHandler::CreatePushMessageL( TDesC8& aProvisioningDoc )
    {
	TBuf8<KHeaderSize> header;
	header.Append( 0xB6 ); // Wap push header value
	HBufC8* hdrBuf = header.AllocLC();
	HBufC8* bodyBuf = aProvisioningDoc.AllocLC();
	CPushMessage* pushMsg = CPushMessage::NewL( hdrBuf, bodyBuf );
	CleanupStack::Pop( 2 ); // hdrBuf, bodyBuf
	return pushMsg;
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::PrepareEntryL
//  
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::PrepareEntryL( TMsvEntry& aEntry )
    {
	// Current time
	TTime time;

	// Get Universal time
	time.UniversalTime();

	aEntry.iType = KUidMsvMessageEntry;
	aEntry.SetVisible(EFalse);
	aEntry.SetInPreparation(ETrue);
	aEntry.SetReadOnly(EFalse);
	aEntry.SetUnread(ETrue);
	aEntry.iDate = time;
	aEntry.iServiceId = KMsvLocalServiceIndexEntryId;
	aEntry.iError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::StoreEntryL
//  
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::StoreEntryL( CWPPushMessage& aPushMsg, TMsvEntry& aEntry)
    {
	if( SysUtil::FFSSpaceBelowCriticalLevelL( NULL, aPushMsg.Body().Length() ) )
        {
        FLOG( _L( "[ProvisioningSC] CWPBioMsgHandler::StoreEntryL KErrDiskfull:" ) );
        User::Leave( KErrDiskFull );
        }
	// Store entry in inbox
	CMsvEntry* msvEntry = iSession->GetEntryL( KMsvGlobalInBoxIndexEntryId );
	CleanupStack::PushL( msvEntry ); // 1
	msvEntry->CreateL( aEntry );
	msvEntry->Session().CleanupEntryPushL( aEntry.Id() );
	msvEntry->SetEntryL( aEntry.Id() );

	// Save the message
	CMsvStore* store = msvEntry->EditStoreL();
	CleanupStack::PushL( store ); // 2
	aPushMsg.StoreL( *store );
	store->CommitL();
	
	// Complete processing the message
	aEntry.SetReadOnly( EFalse );
	aEntry.SetVisible( ETrue );
	aEntry.SetInPreparation( EFalse );
	msvEntry->ChangeL( aEntry );

	CleanupStack::PopAndDestroy(); // store, 2
	msvEntry->Session().CleanupEntryPop(); //msvEntry's entry
	CleanupStack::PopAndDestroy(); // msvEntry, 1
    }

// -----------------------------------------------------------------------------
// CWPBioMsgHandler::BioMessageTypeL
// -----------------------------------------------------------------------------
//
void CWPBioMsgHandler::BioMessageTypeL( CPushMessage& aMessage )
	{
    // need to create local RFs for BIO otherwise raises exception
    RFs localFS;
    User::LeaveIfError( localFS.Connect() );
    CleanupClosePushL( localFS );
    CBIODatabase* bioDB = CBIODatabase::NewLC( localFS );
    TPtrC contentTypePtr;
    aMessage.GetContentType( contentTypePtr );
	iBioUid =  KNullUid;
    // Ignored return type: KErrNone if found or KErrNotFound if not found 
    bioDB->IsBioMessageL( EBioMsgIdIana, contentTypePtr, NULL, iBioUid );
    CleanupStack::PopAndDestroy(2); // bioDB, localFS
    }

//  End of File
