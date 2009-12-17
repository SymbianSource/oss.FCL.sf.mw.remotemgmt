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
* Description:  Reads a provisioning document from a SIM/WIM card
*
*/


// INCLUDE FILES
#include	"CWPProvisioningSC.h"

#include	<eikdll.h>
#include	<e32std.h>
#include	<e32svr.h>
#include	<e32base.h>
#include	<CWPEngine.h>
#include	<AknGlobalNote.h> 
#include	<ProvisioningSC.rsg>
#include	<WPAdapterUtil.h>
#include	"MWPPhone.h"
#include	"CWPPushMessage.h"
#include	"CWPBootstrap.h"
#include	"CWPImsiDbHandler.h"
#include	"CWPWimHandler.h"
#include	"CWPBioMsgHandler.h"
#include	"WPPhoneFactory.h"
#include	"ProvisioningDebug.h"
#include	"WPProvScPaths.h"
#include	<ProvSC.h>

const TInt KPhoneRetryCount = 7;
const TInt KPhoneRetryTimeout = 100000; 

_LIT( KProvisioningSCThread, "ProvisioningSC" );

// LOCAL FUNCTION PROTOTYPES

// Initializes the server.
LOCAL_C void InitProvScL();




// ==================== LOCAL FUNCTIONS ====================

LOCAL_C void InitProvScL()
	{
	FLOG( _L( "[ProvisioningSC] InitProvScL" ) );

__UHEAP_MARK;

	TBool acInstalled( EFalse );
    // Scheduler exists before this point only in test code:
	if( CActiveScheduler::Current()==NULL )
		{
		CActiveScheduler* ac = new (ELeave) CActiveScheduler;
		CleanupStack::PushL( ac ) ;
		CActiveScheduler::Install( ac );
		acInstalled = ETrue;
		}
	
	CWPProvisioningSC* reader = CWPProvisioningSC::NewLC();
      RProcess::Rendezvous(KErrNone);
	if ( reader->ReadFile() )  
		{
        FLOG( _L( "[ProvisioningSC] InitProvScL starting sched" ) );
		CActiveScheduler::Start();  // start only if request issued
		FLOG( _L( "[ProvisioningSC] InitProvScL, after shed start, continuing" ) );
		}

	CleanupStack::PopAndDestroy(  ); // reader
	if( acInstalled )
		{
		FLOG( _L( "[ProvisioningSC] InitProvScL acinstalled - uninstalling." ) );
		CActiveScheduler::Install( NULL ); // uninstall
		CleanupStack::PopAndDestroy(); // ac
		}

__UHEAP_MARKEND;
	}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPProvisioningSC::CWPProvisioningSC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPProvisioningSC::CWPProvisioningSC()
    {
    }

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPProvisioningSC::ConstructL()
    {
    User::RenameThread( KProvisioningSCThread );
	iImsiHandler = CWPImsiDbHandler::NewL(); // 1
	TBool isOld = ProcessImsiL( iPhoneImsi ); 
	if( !isOld )
		{
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ConstructL new imsi" ) );
		iWimHandler = CWPWimHandler::NewL( *this );
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ConstructL StoreImsi" ) );
		iImsiHandler->StoreImsiL( iPhoneImsi );
		}
    }

// -----------------------------------------------------------------------------
// CWPProvisioningSC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPProvisioningSC* CWPProvisioningSC::NewL()
    {
	CWPProvisioningSC* self = CWPProvisioningSC::NewLC();
	CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningSC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPProvisioningSC* CWPProvisioningSC::NewLC()
    {
    CWPProvisioningSC* self = new( ELeave ) CWPProvisioningSC;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningSC::Destructor
// 
// -----------------------------------------------------------------------------
//
CWPProvisioningSC::~CWPProvisioningSC()
    {
	delete iProvisioningDoc;
	delete iImsiNumbers;
	delete iWimHandler;
	delete iImsiHandler;
    }

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ReadCompletedL
// -----------------------------------------------------------------------------
//
void CWPProvisioningSC::ReadCompletedL()
	{
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadCompletedL" ) );
	iProvisioningDoc = iWimHandler->DocL( );

	TRAPD( err, StoreDocL( *iProvisioningDoc ));
	if( err == KErrDiskFull || err == KErrNoMemory  )
		{
		ShowMemoryLowNoteL();
		}
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadCompletedL stopping sched" ) );
	CActiveScheduler::Stop();
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadCompletedL sched stopped" ) );
	}	

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ReadCancelledL
// -----------------------------------------------------------------------------
//
void CWPProvisioningSC::ReadCancelledL()
	{
	if( iWimHandler )
		{
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ReadErrorL
// -----------------------------------------------------------------------------
//
void CWPProvisioningSC::ReadErrorL()
	{
	if( iWimHandler )
		{
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ProcessImsiL
//
// -----------------------------------------------------------------------------
//
TBool CWPProvisioningSC::ProcessImsiL( TDes& aPhoneImsi )
	{
	TBool isOldImsi( ETrue );
	if( iImsiHandler )
		{
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ProcessImsiL" ) );
		TInt err( KErrNotFound );
		MWPPhone* phone = NULL; 
		for ( TInt j = 0 ; j < KPhoneRetryCount ; j++ )
			{
			TRAP( err,  phone = WPPhoneFactory::CreateL() ); 
			// note: phone not in cleanupstack
			if ( err == KErrNone )
				{
				aPhoneImsi.Copy( phone->SubscriberId() ); // store imsi
				delete phone; 
				phone = NULL;
				break;
				}
			User::After( KPhoneRetryTimeout );
			}
		User::LeaveIfError( err );
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ProcessImsiL imsiexistsL?" ) );
		isOldImsi = iImsiHandler->ImsiExistsL( aPhoneImsi );
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ProcessImsiL imsiexistsL ok" ) );
		}
	return isOldImsi;
	}

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ReadFileL
//
// -----------------------------------------------------------------------------
//
TBool CWPProvisioningSC::ReadFile()
	{
    FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadFile" ) );	
	if( iWimHandler )
		{
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadFile wimhandler" ) );
		iWimHandler->ReadProvFile( EBootStrap );
		return ETrue;
		}
	else
		{
		FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::ReadFile wimhandler null" ) );
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CWPProvisioningSC::ShowMemoryLowNoteL
//
// -----------------------------------------------------------------------------
//
void CWPProvisioningSC::ShowMemoryLowNoteL()
	{

	TFileName fileName;
	#ifndef __WINS__
	fileName = RProcess().FileName();
	#else
	Dll::FileName( fileName );
	#endif

	HBufC* memLowText = WPAdapterUtil::ReadHBufCL( fileName,
												KWPProvSCName,
												R_TEXT_MEM_SUCS );
    CleanupStack::PushL( memLowText );  // 1
	CAknGlobalNote* note = CAknGlobalNote::NewLC();  // 2
	note->ShowNoteL( EAknGlobalInformationNote, *memLowText );
	CleanupStack::PopAndDestroy( 2 ); // note, memLowText
	}

// -----------------------------------------------------------------------------
// CWPProvisioningSC::StoreDocL
//
// -----------------------------------------------------------------------------
//

void CWPProvisioningSC::StoreDocL( TDesC8& aProvDoc )
	{
    FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL" ) );
    
    HBufC8* convert = aProvDoc.Alloc();
       
    TPtr8 ptrBuffer( convert->Des() );

	while ( ( ptrBuffer[ptrBuffer.Length()-1 ] == 0xFF ) && (ptrBuffer.Length() > 1 ) )
	    {
		FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPProvisioningSC::StoreDocL ... deleting (%d)"), (ptrBuffer.Length()-1)));
		ptrBuffer.Delete( ( ptrBuffer.Length()-1 ) , 1 );
	    }
	    
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL cleanup done" ) );
        
	aProvDoc = *convert;
    delete convert;
    
    #ifdef _DEBUG
    FHex(aProvDoc);
    #endif
    
	CWPBioMsgHandler* msgHandler = CWPBioMsgHandler::NewLC(); // 1
	CWPPushMessage* msg = msgHandler->CreateMessageLC( aProvDoc ); // 2
	msg->SetAuthenticated( ETrue );

    // 0. read the message into the engine
    CWPEngine* engine = CWPEngine::NewLC();  // 3
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL2" ) );
	#ifdef _DEBUG
	FHex(msg->Body() );
	#endif
    engine->ImportDocumentL( msg->Body() );
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL3" ) );
	engine->PopulateL();

	// 1. process bootstrap
    CWPBootstrap* bootstrap = CWPBootstrap::NewL( iPhoneImsi );
    CleanupStack::PushL( bootstrap );// 4
	FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL4" ) );
    CWPBootstrap::TBootstrapResult bootstrapResult( 
        bootstrap->BootstrapL( *msg, *engine, KNullDesC ) );
    CleanupStack::PopAndDestroy(); // bootstrap, 4 
	TBool haveSettings( engine->ItemCount() > 0 );

	// 2.A Does the message contain bootstrap?
	switch( bootstrapResult )
		{
	case CWPBootstrap::ESucceeded:
			// fallthrough
		case  CWPBootstrap::EBootstrapExists:
		{
		if( haveSettings ) 
			{
			FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL5" ) );
			msgHandler->StoreMessageL( *msg );
			FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL6" ) );
			}
		else
			{
			// no other settings than the TPS, store a note into inbox
			// but first check that the TPS is not already defined
			if(!( bootstrapResult == CWPBootstrap::EBootstrapExists ) )
				{
				FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL8" ) );
				msgHandler->StoreMessageL( R_TEXT_PROV_SUCS, *msg );
				FLOG( _L( "[ProvisioningSC] CWPProvisioningSC::StoreDocL9" ) );;
				}
			}
		break;
		}
		// 2.B No bootstrap
	case CWPBootstrap::ENoBootstrap:
		{	
		if( haveSettings )
			{
			// 2.B.I store settings into inbox
			msgHandler->StoreMessageL( *msg );
			}
			// 2.B.II, no boostrap, no settings -> just ignore message.
		break;
		}	
	default:
		{
		break;
		}
	} // switch
	CleanupStack::PopAndDestroy( 3 ); // msg, handler, engine
	}

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// E32Main implements the executable entry function.
// Note that because the target type of the ProvSC module
// is EXEDLL, the entry point has different signature depending
// on the build platform.
// Creates a cleanup stack and runs the server.
// Returns: Zero.

#if defined (__WINS__)
IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()

#else  //if defined(__EPOC32__)

GLDEF_C TInt E32Main()

#endif
    {
	CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD( error, InitProvScL() );
    if ( error ) error = 0; // prevent compiler warning	
	delete cleanup;
	return KErrNone; 
    }

//  End of File  
