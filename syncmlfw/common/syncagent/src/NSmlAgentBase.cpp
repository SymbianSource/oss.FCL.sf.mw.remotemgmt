/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class in Commmon Agent
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif
#include <tconvbase64.h>
#include <hash.h>
#include <SyncMLHistory.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldefines.h>
#include <nsmlphoneinfo.h>
#include <nsmlunicodeconverter.h>
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>
#include <e32property.h>
#include <http/thttpevent.h>
#include "nsmlprivatepskeys.h"
#include "nsmlcliagconstants.h"
#include "NSmlAgentBase.h"
#include "NSmlCmdsBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResponseController.h"
#include "nsmlerror.h"
#include "NSmlURI.h"
#include "nsmltransport.h"
#include "nsmlagenttestdefines.h"
#if defined (__NOTRANSPORT) || defined (_DEBUG)
#include "wbxml2xmlconverter.h"
#include <bautils.h>
#endif
//Rnd_AutoRestart
#include <es_sock.h> // RConnection RSocket
#include <es_enum.h> // TConnectionInfo
#include <commdb.h>
#include <cmconnectionmethoddef.h>
#include <cmconnectionmethod.h>
#include <cmmanager.h>
#include <cmpluginwlandef.h>
#include "nsmlhttp.h"

//Fix to Remove the Bad Compiler Warnings
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TUid KCRUidDataSyncInternalKeys  = { 0x2000CF7E };
const TInt KNsmlDsAutoRestart = 7; //(0x00000007)
//Rnd_AutoRestart

// CONSTANTS
_LIT( KPanicInvalidState, "Invalid state" );

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CNSmlAgentBase::CNSmlAgentBase()
// C++ constructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlAgentBase::CNSmlAgentBase() : CActive(EPriorityLow), iBufferArea(0, 0)
    {
	}

// ---------------------------------------------------------
// CNSmlAgentBase::~CNSmlAgentBase()
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlAgentBase::~CNSmlAgentBase()
    {
	Cancel();
	delete iImeiCode;
	delete iSyncMLUserName;
	delete iSyncMLPassword;
	delete iSyncServer;
	delete iSyncMLCmds;
	delete iSyncMLMIMEType;
	delete iResultAlertCode;
	delete iEndOfDataAlertCode;
	delete iSessionAbortAlertCode;
	delete iNextMessageCode;
	delete iTransport;		
	delete iError;
    delete iIAPIdArray;
	delete iSyncHTTPAuthUserName;
	delete iSyncHTTPAuthPassword;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::AlreadyAuthenticated()
// 
// ---------------------------------------------------------
TBool CNSmlAgentBase::AlreadyAuthenticated() const
	{
	return iAlreadyAuthenticated;
	}
	
// ---------------------------------------------------------
// CNSmlAgentBase::BasicCredentialL
// Creates B64 string form SyncML userID and password 
// ---------------------------------------------------------
HBufC8* CNSmlAgentBase::BasicCredentialL() const
	{
	TBase64 B64Coder;
	HBufC8* userName;
	NSmlUnicodeConverter::HBufC8InUTF8LC( *iSyncMLUserName, userName );
	HBufC8* password;
	NSmlUnicodeConverter::HBufC8InUTF8LC( *iSyncMLPassword, password );
	_LIT8( KColon, ":" );
	HBufC8* clearText = HBufC8::NewLC( userName->Length() + KColon.iTypeLength + password->Length());
	TPtr8 clearTextPtr = clearText->Des();
	clearTextPtr = *userName;
	clearTextPtr += KColon;
	clearTextPtr += *password;
	HBufC8* B64String = HBufC8::NewLC( clearText->Length() * 2 );
	TPtr8 B64StringPtr( B64String->Des() );
	User::LeaveIfError( B64Coder.PortableEncode( *clearText, B64StringPtr ) );
	CleanupStack::Pop(); // B64String
	CleanupStack::PopAndDestroy( 3 ); //clearText, password, userName
	return B64String;
	}
	
// ---------------------------------------------------------
// CNSmlAgentBase::Md5CredentialL
// Creates MD5 string form SyncML userID, password and nonce
// ---------------------------------------------------------
HBufC8* CNSmlAgentBase::Md5CredentialL( TBool aServerAuth ) const
	{

	CMD5* md5 = CMD5::NewL();
	CleanupStack::PushL( md5 );
	TBase64 B64Coder;
	HBufC8* userName;
	if ( aServerAuth )
		{
		NSmlUnicodeConverter::HBufC8InUTF8LC( ServerUserName(), userName );
		}
	else
		{
		NSmlUnicodeConverter::HBufC8InUTF8LC( *iSyncMLUserName, userName );
		}
	HBufC8* password;
	if ( aServerAuth )
		{
		NSmlUnicodeConverter::HBufC8InUTF8LC( ServerPassword(), password );
		}
	else
		{
		NSmlUnicodeConverter::HBufC8InUTF8LC( *iSyncMLPassword, password );
		}
	HBufC* nonceInUnicode;
	if ( aServerAuth )
		{
		nonceInUnicode = ServerNonceAllocL();
		}
	else
		{
		nonceInUnicode = NonceL();
		}
	CleanupStack::PushL( nonceInUnicode );
	HBufC8* nonce = HBufC8::NewLC( nonceInUnicode->Length() );
	nonce->Des().Copy( *nonceInUnicode );
	_LIT8( KColon, ":" );
		
	HBufC8* userNamePassword = HBufC8::NewLC( userName->Length() + KColon.iTypeLength + password->Length() );
	TPtr8 userNamePasswordPtr = userNamePassword->Des();
	userNamePasswordPtr = *userName;
	userNamePasswordPtr += KColon;
	userNamePasswordPtr += *password;
	TPtrC8 userNamePasswordHash;
	userNamePasswordHash.Set( md5->Hash( *userNamePassword ) );
	HBufC8* B64UserNamePasswordString = HBufC8::NewLC( userNamePasswordHash.Length() * 2 );
	TPtr8 B64UserNamePasswordStringPtr( B64UserNamePasswordString->Des() );
	User::LeaveIfError( B64Coder.PortableEncode( userNamePasswordHash, B64UserNamePasswordStringPtr ) );
	HBufC8* userNamePasswordNonce = HBufC8::NewLC( B64UserNamePasswordStringPtr.Length() + KColon.iTypeLength + nonce->Length() );
	TPtr8 userNamePasswordNoncePtr = userNamePasswordNonce->Des();
	userNamePasswordNoncePtr = B64UserNamePasswordStringPtr;
	userNamePasswordNoncePtr += KColon;
	userNamePasswordNoncePtr += *nonce;
		
	TPtrC8 finalHash;
	md5->Reset();
	finalHash.Set( md5->Hash( *userNamePasswordNonce ) );
	HBufC8* B64String = HBufC8::NewLC( finalHash.Length() * 2 );
	TPtr8 B64StringPtr( B64String->Des() );
	User::LeaveIfError( B64Coder.PortableEncode( finalHash, B64StringPtr ) );
	CleanupStack::Pop();   // B64String
	CleanupStack::PopAndDestroy( 8 ); //userNamePasswordNonce, B64userNamePasswordNonce, userNamePassword, nonce, nonceInUnicode, password, userName, md5
	return  B64String;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ServerNonceAllocL()
// 
// ---------------------------------------------------------
EXPORT_C HBufC* CNSmlAgentBase::ServerNonceAllocL() const
	{
	return HBufC::NewL( 0 );
	}
// ---------------------------------------------------------
// CNSmlAgentBase::SetServerNonceL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::SetServerNonceL( const TDesC& /*aNonce*/ ) const
	{
	}
// ---------------------------------------------------------
// CNSmlAgentBase::SetEndOfDataAlertRequest()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::SetEndOfDataAlertRequest()
	{
	iEndOfDataAlertRequest = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlAgentBase::SyncLog()
// Returns pointer to the syncml history log.
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryJob* CNSmlAgentBase::SyncLog()
	{
	return iSyncLog;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::CallbackCalled()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlAgentBase::CallbackCalled() const
	{
	return iCallbackCalled;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::SetOffCallbackCalled()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::SetOffCallbackCalled()
	{
	iCallbackCalled = EFalse;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::RunL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::RunL()
    {
	TRAPD( leaveCode, NavigateMainStateL() );
	if ( leaveCode != KErrNone )
		{
		ErrorHandling( leaveCode );
		}
		
	if ( iCancelForced )
		{
		ErrorHandling( KErrCancel );
		}
		
	if ( !iEnd )
		{
		SetActive();
		#ifndef __NOTRANSPORT
		if ( !iCommandIssued ) //When asychronous transport service is not called
							   //thread is signalled immediately 	
		#endif
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete( status, KErrNone );
			}
		}
	
	if ( iEnd )
		{
		FreeResources();	
		}
	
	}


// ---------------------------------------------------------
// CNSmlAgentBase::DoOutsideCancel()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DoOutsideCancel() 
	{ 
	iCancelForced = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlAgentBase::DoCancel()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DoCancel()
    {
	#ifndef __NOTRANSPORT
	iTransport->Disconnect(); 
	#endif
	ErrorHandling( KErrCancel );
	return;
    }
// ---------------------------------------------------------
// CNSmlAgentBase::smlAddCmdFunc
// Add command from server
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlAddCmdFuncL( SmlAddPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModUpdatesStateL( KNSmlAgentAdd(), aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAdd, aContent->cmdID, aContent->flags );
		}
	return 0;
	}
// ---------------------------------------------------------
// CNSmlAgentBase::smlCopyCmdFunc
// Copy command from server, Copy is not supported
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlCopyCmdFuncL( SmlCopyPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentCopy, aContent->cmdID, aContent->flags );
	return 0;
	}
// ---------------------------------------------------------
// CNSmlAgentBaset::smlDeleteCmdFunc
// Delete command from server
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlDeleteCmdFuncL( SmlDeletePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModUpdatesStateL( KNSmlAgentDelete(), aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentDelete, aContent->cmdID, aContent->flags );
		}
	return 0;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::smlExecCmdFunc
// Exec command from server
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlExecCmdFuncL( SmlExecPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentExec, aContent->cmdID, aContent->flags );
	return 0;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::smlMapCmdFunc
// Map command from server, Map is not supported
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlMapCmdFuncL( SmlMapPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentMap, aContent->cmdID );
	return 0;
	}
// ---------------------------------------------------------
// CNSmlAgentBase::smlReplaceCmdFunc
// Replace command from server
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlReplaceCmdFuncL( SmlReplacePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModUpdatesStateL( KNSmlAgentReplace(), aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentReplace, aContent->cmdID, aContent->flags );
		}
	return 0;
	}
// ---------------------------------------------------------
// CNSmlAgentBase::smlSearchCmdFunc
// Search command from server
// ---------------------------------------------------------
EXPORT_C Ret_t CNSmlAgentBase::smlSearchCmdFuncL( SmlSearchPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentSearch, aContent->cmdID, aContent->flags );
	return 0;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::smlMoveCmdFunc
// Copy command from server, Copy is not supported
// ---------------------------------------------------------
//1.2 CHANGES
EXPORT_C Ret_t CNSmlAgentBase::smlMoveCmdFuncL( SmlMovePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentMove, aContent->cmdID, aContent->flags );
	return 0;
	}
//end chnages


//
//  Protected member functions
//
// ---------------------------------------------------------
// CNSmlAgentBase::ConstructL()
// 
// ---------------------------------------------------------
//
EXPORT_C void CNSmlAgentBase::ConstructL()
    {
    iSendRetries = 3;
	iCurrMainState = EBeginning;
	#ifndef __NOTRANSPORT
	iTransport = CNSmlTransport::NewL();	
	#endif
	CNSmlPhoneInfo* phoneInfo = CNSmlPhoneInfo::NewLC();
	iImeiCode = HBufC::NewL( 50 );
	TPtr imeiCodePtr = iImeiCode->Des();  
	phoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, imeiCodePtr );
	CleanupStack::PopAndDestroy();  //phoneInfo
	InitialiseSubStates();
	//Auto_Restart
    iPacketDataUnAvailable = EFalse;    
    iNetmonAPId = 0;
    iAllowAutoRestart = EFalse;
	}


//
// Beginning state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::BeginningStateL()
// This state is performed in the beginning of sync.
// Navigate according to sub state code
// ---------------------------------------------------------
//

EXPORT_C void CNSmlAgentBase::BeginningStateL() 
	{
	switch ( iCurrBeginningState )
		{
		case EBegin:
			BeginStateL();
			break;
		case EBeginConnectingToServer:
			BeginConnectingStateL();
			break;
		default:
			User::Panic( KPanicInvalidState, iCurrBeginningState );
		}
	}



//
//  Client Initialisation state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::ClientInitialisationStateL()
// Makes Sync Initialization Package from Client 
// Navigate according to sub state code.
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ClientInitialisationStateL()
	{
	switch ( iCurrClientInitState )
		{
		case EInitStartingMessage:
			InitStartingMessageStateL();
			break;
		case EInitMakingStatus:
			InitMakingStatusStateL();
			break;
		case EInitAlerting:
			InitAlertingStateL();
			break;
		case EInitDeviceInfo:
			InitDeviceInfoStateL();
			break;
		case EInitAskingDeviceInfo:
			InitAskingDeviceInfoStateL();
			break;
		case EInitEndingMessage:
			InitEndingMessageStateL();
			break;
		default:
			User::Panic( KPanicInvalidState, iCurrClientInitState );
		}
	}
// ---------------------------------------------------------
// CNSmlAgentBase::InitStartingMessageBaseStateL()
// "Initialising" event to UI,  makes SyncHdr  
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::InitStartingMessageStateBaseL()
	{
	iChallengeRequest = EFalse;
	iSyncMLCmds->DoSyncHdrL();
	iCurrClientInitState = EInitMakingStatus;
	}


// ---------------------------------------------------------
// CNSmlAgentBase::InitAskingDeviceInfoStateL()
// There is only default implementation in the base class, which 
// does not anything but set the next sub state.
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::InitAskingDeviceInfoStateL()
	{
	iCurrClientInitState = EInitEndingMessage;
	}

//
//  Server Modifications state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::ServerModStartMessageStateL(()
// Handles SyncHdr in Server Modifications to Client Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerModStartMessageStateL( SmlSyncHdr_t* aSyncHdr )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerModStartMessageStateL begins"));
	if ( iCurrServerModState != EServerModWaitingStartMessage )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessSyncHdrL( aSyncHdr );
	iCurrServerModState = EServerModWaitingCommands;
	DBG_FILE(_S8("CNSmlAgentBase::ServerModStartMessageStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ServerModStatusCmdStateL()
// Handles Status command in Server Modifications to Client Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerModStatusCmdStateL( SmlStatus_t* aStatus )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerModStatusCmdStateL begins"));
	if ( iChallengeRequest )
		{
		return;
		}
	if ( iCurrServerModState != EServerModWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessStatusCmdL ( aStatus );
	DBG_FILE(_S8("CNSmlAgentBase::ServerModStatusCmdStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ServerModAlertCmdStateL()
// Handles Alert command in Server Modifications to Client Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerModAlertCmdStateL( SmlAlert_t* aAlert )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerModAlertCmdStateL begins"));

	//Moved from CNSmlDSAgent::smlAlertCmdFuncL to here
	if ( !iFinalMessageFromClient )
		{
		return;
		}

	if ( iCurrServerModState != EServerModWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}

	if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlAgentDisplayAlert )
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, EFalse, EFalse, ETrue );
		}
	else
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, ETrue );
		}

	DBG_FILE(_S8("CNSmlAgentBase::ServerModAlertCmdStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ServerModEndMessageStateL()
// Handles end of SyncML message, amount of received items is 
// notified to UI
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerModEndMessageStateL( TBool aFinal )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerModEndMessageStateL begins"));
	if ( iCurrServerModState != EServerModWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	if ( aFinal )
		{
		iCurrServerModState = EServerModPackageReceived;
		if ( iSyncMLCmds->WaitingLargeObjectChunk() )
			{
			iSyncMLCmds->ResetLargeObjectBuffer();
			iEndOfDataAlertRequest = ETrue;
			}
		}
	else
		{
		iCurrServerModState = EServerModMessageReceived;
		}
	DBG_FILE(_S8("CNSmlAgentBase::ServerModEndMessageStateL ends"));
	}
//
//  Data Update Status state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusStateL()
// Makes Data Update Status Package to Server
// Navigate according to state code
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DataUpdateStatusStateL()
	{
	switch ( iCurrDataUpdateStatusState )
		{
		case EStatusStartingMessage:
			DataUpdateStatusStartingMessageStateL();
			break;
		case EStatusMakingAlert:
			DataUpdateStatusMakingAlertStateL();
			break;
		case EStatusMakingStatus:
			DataUpdateStatusMakingStatusStateL();
			break;
		case EStatusMakingMap:
			DataUpdateStatusMakingMapStateL();
			break;
		case EStatusMakingResults:
			DataUpdateStatusMakingResultsStateL();
			break;
		case EStatusEndingMessage:
			DataUpdateStatusEndingMessageStateL();
			break;
		default:
			User::Panic( KPanicInvalidState, iCurrDataUpdateStatusState );
		}
	}
// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusStartingMessageStateBaseL()
// Notify "Finalising" to UI, makes SyncHdr 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DataUpdateStatusStartingMessageStateBaseL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusStartingMessageStateBAseL begins"));
	iBufferFull = EFalse;
	iSyncMLCmds->DoSyncHdrL();
	iCurrDataUpdateStatusState = EStatusMakingStatus;
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusStartingMessageStateBaseL ends"));
	}




// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusMakingMapStateL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DataUpdateStatusMakingMapStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingMapStateL begins"));
	iCurrDataUpdateStatusState = EStatusMakingResults;
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingMapStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusMakingResultsStateL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::DataUpdateStatusMakingResultsStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingResultsStateL begins"));
	iCurrDataUpdateStatusState = EStatusEndingMessage;
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingResultsStateL ends"));
	}

//
// Result Alert state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::ResultAlertStateL()
// Makes Result Alert Package to Server (response to busy 
// Status from server)
// Navigate according to state code
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ResultAlertStateL()
	{
	switch ( iCurrResultAlertState )
		{
		case EResultAlertStartingMessage:
			ResultAlertStartingMessageStateL();
			break;
		case EResultAlertMakingAlert:
			ResultAlertAlertingStateL();
			break;
		case EResultAlertEndingMessage:
			ResultAlertEndingMessageStateL();
			break;
		default:
			User::Panic( KPanicInvalidState, iCurrResultAlertState );
		}
	}

//
// Alert for next state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextStateL()
// Makes Next Alert Package to Server (response to incomplete Package)
// Navigate according to state code
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ClientAlertNextStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextStateL begins"));
	switch ( iCurrClientAlertNextState )
		{
		case EClientAlertNextStartingMessage:
			ClientAlertNextStartingMessageStateL();
			break;
		case EClientAlertNextMakingAlert:
			ClientAlertNextAlertingStateL();
			break;
		case EClientAlertNextMakingStatus:
			ClientAlertNextMakingStatusStateL();
			break;
		case EClientAlertNextMakingMap:
			ClientAlertNextMakingMapStateL();
			break;
		case EClientAlertNextEndingMessage:
			ClientAlertNextEndingMessageStateL();
			break;
		default:
			User::Panic( KPanicInvalidState, iCurrClientAlertNextState );
		}
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextStateL ends"));
	}


// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextMakingMapStateL()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ClientAlertNextMakingMapStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextMakingMapStateL begins"));
	iCurrClientAlertNextState = EClientAlertNextEndingMessage;
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextMakingMapStateL ends"));
	}

//
//  Server Alert For Next state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::ServerAlertNextStateL()
// Main state function to handle Next Alert from Server Package 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerAlertNextStateL()
	{
	if ( !iSyncMLCmds->ProcessReceivedDataL() )
		{
		if ( iCurrServerAlertNextState == EServerAlertNextMessageReceived )
			{
			CheckCommandsAreReceivedL();
			}
		else
			{
			User::Leave( TNSmlError::ESmlIncompleteMessage );
			}
		NextMainStateL();
		}
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ServerAlertNextStartMessageStateL(()
// Handles SyncHdr in Next Alert from Server Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerAlertNextStartMessageStateL( SmlSyncHdr_t* aSyncHdr )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextStartMessageStateL begins"));
	if ( iCurrServerAlertNextState != EServerAlertNextWaitingStartMessage )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessSyncHdrL( aSyncHdr );
	iCurrServerAlertNextState = EServerAlertNextWaitingCommands;
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextStartMessageStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ServerAlertNextStatusCmdStateL()
// Handles Status command in Next Alert from Server Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerAlertNextStatusCmdStateL( SmlStatus_t* aStatus )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextStatusCmdStateL begins"));
	if ( iCurrServerAlertNextState != EServerAlertNextWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessStatusCmdL ( aStatus );
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextStatusCmdStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ServerAlertNextAlertCmdStateL()
// Handles Alert command in Next Alert from Server Package
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ServerAlertNextAlertCmdStateL( SmlAlert_t* aAlert )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextAlertCmdStateL begins"));
	if ( iCurrServerAlertNextState != EServerAlertNextWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd );
		}

	if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlAgentDisplayAlert )
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, EFalse, EFalse, ETrue );
		}
	else
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, ETrue );
		}

	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextAlertCmdStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ServerAlertNextEndMessageStateL()
// Handles end of SyncML message in Next Alert from Server Package
// ---------------------------------------------------------
EXPORT_C void  CNSmlAgentBase::ServerAlertNextEndMessageStateL( TBool /*aFinal*/ )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextEndMessageStateL begins"));
	if ( iCurrServerAlertNextState != EServerAlertNextWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iCurrServerAlertNextState = EServerAlertNextMessageReceived;
	DBG_FILE(_S8("CNSmlAgentBase::ServerAlertNextEndMessageStateL ends"));
	}

//
// Sending state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase::SendingStateL()
// Sends SyncML Package to server
// Navigate according to state code
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::SendingStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::SendingStateL begins"));
	TInt err= KErrNone;
	if ( !iCommandIssued )
	{
		SendDataIssueL();
	}
	else
	{
		TRAP(err,SendDataDoneL());
		DBG_FILE_CODE(iStatus.Int(), _S8("CNSmlAgentBase::SendingStateL, ERROR CODE is "));
		// IF session timeout feature is ON and then if its DM session then retry 
		if(err == KErrNone)
		{
			iSendRetries = 3; 
			NextMainStateL();	
		}
		else
		{
		   //Rnd_AutoRestart
		  // Check the session type ESyncMLDMSession or ESyncMLDMSession		  
		  /*
		  	If the session is DM (ESyncMLDMSession) then check for feature sessiontimeout ON.
		  	If its ON then if teh failure is -33(timeout) or 9 (http:efailed) then 
		  	The state machine has to be reset to Sending, so that the packet is sent again.
		  	If any other error leave with the error code.
		  	Changes applicable for ***DM ONLY***. 
		  	For DS any SendDataDoneL() should leave with error 
		  	code for cases other than KErrNone 
		  */
			TInt Session=0;    
	    	TInt r=RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, Session);                       
	    	DBG_FILE_CODE(Session, _S8("CNSmlAgentBase::SendingStateL, Session type is 1 for DS and 2 for DM"));
		    if( Session == ESyncMLDMSession ) //DM Session 
		    {
		        TInt dmsessionTimeout = -1;
		        CRepository * rep = 0;
				TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ))
				DBG_FILE_CODE(err1, _S8("CNSmlAgentBase::SendingStateL, Crepository reading returned code"));
				if(err1 == KErrNone)
				{
					rep->Get( KDevManDMSessionTimeout, dmsessionTimeout );
					delete rep;
					DBG_FILE_CODE(dmsessionTimeout, _S8("CNSmlAgentBase::SendingStateL, DMSessiontimeout feature value from cenrep"));
					if( dmsessionTimeout > KNSmlDMMaxSessionTimeout || dmsessionTimeout < KNSmlDMMinSessionTimeout)
					{
						dmsessionTimeout = -1;
					}
					DBG_FILE_CODE(dmsessionTimeout, _S8("CNSmlAgentBase::SendingStateL, DMSessiontimeout feature value "));
				}			
				
				if(dmsessionTimeout != -1 )
				{
					if((err == KErrTimedOut|| err == THTTPEvent::EFailed ) && iSendRetries >0 )
					{
					DBG_FILE(_S8("CNSmlAgentBase::SendingStateL reset to Sending state again and send same packet"));
					iCurrMainState = ESending;
					iSendRetries --;
					}
					else //if (err == KErrCancel)
					{
						DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL, DMSessiontimeout feature is ON but error code is not for retrying"));
						User::Leave(err);
					}
					
				}
				else
				{
					DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL, DMSessiontimeout feature is OFF, leave with error code"));
					User::Leave(err);
				}
		       	
	       	   }
		       	else  // DS Sessions , has to leave upon any leave happening from SendData
		       	{
		       		TInt val = 0;
		       		CRepository* rep = CRepository::NewLC(KCRUidDataSyncInternalKeys);
    				TInt error = rep->Get(KNsmlDsAutoRestart, val);
    				CleanupStack::PopAndDestroy(rep);
    				
    				if(error == KErrNone && val == 1)
					{
		       		
			       		DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL The Network Error is"));
			       		
			       		if(err == TNSmlHTTPErrCode::ENSmlHTTPErr_RequestTimeout)
			       		{
			       			DBG_FILE(_S8("CNSmlAgentBase::SendingStateL Invoking the NETMON exe due to Time Out"));
			       			
			       			iPacketDataUnAvailable = ETrue;
		       				
			       			//Invoke the NetMon exe via the Agent
			       			LaunchAutoRestartL(err);
			       		}
		       		
			       		else if( iAllowAutoRestart )
			       		{
			       			DBG_FILE(_S8("CNSmlAgentBase::SendingStateL Waiting for 30 sec"));
			       			User::After(TTimeIntervalMicroSeconds32(30000000));
			       			
			       			if(	iPacketDataUnAvailable )
			       			{
			       				DBG_FILE(_S8("CNSmlAgentBase::SendingStateL Invoking the NETMON exe"));
				       			//Invoke the NetMon exe via the Agent
				       			LaunchAutoRestartL(err);	
			       			}
							else
							{
								DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL, Leaving with error code "));
								User::Leave(err);	
							}
			       			
			       			
			       		}
		       	
			       		else
			       		{
			       			DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL, Leaving with error code "));
			       			User::Leave(err);	
			       		}
		       		}
		       		
		       		else
		       		{
		       			DBG_FILE_CODE(err, _S8("CNSmlAgentBase::SendingStateL, Auto-Restart Feature is not Enabled Leaving with error code "));
		       			User::Leave(err);	
		       		}
		     	}
			}
		}
	DBG_FILE(_S8("CNSmlAgentBase::SendingStateL ends"));
	}

//
// Receiving state functions 
//
// ---------------------------------------------------------
// CNSmlAgentBase:::ReceivingStateL()
// Receive fata from a server
// Navigate according to state code 
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::ReceivingStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ReceivingStateL begins"));
	if ( !iCommandIssued )
		{
		ReceiveDataIssueL();
		}
	else
		{
		ReceiveDataDoneL();
		NextMainStateL();
		}
	DBG_FILE(_S8("CNSmlAgentBase::ReceivingStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::CheckStatusCodesAreReceivedL
// Check that server has sent Status to all Client's commands.
// ---------------------------------------------------------
EXPORT_C TBool CNSmlAgentBase::CheckStatusCodesAreReceivedL( TBool aFinal )
	{
	TBool allReceived = ETrue; 

	iSyncMLCmds->ResponseController()->Begin();	
	TBool continues = ETrue;
	while ( continues )
		{
		TInt entryID;
		if ( !iSyncMLCmds->ResponseController()->NextResponseInfo( entryID ) )
			{
			continues = EFalse;
			if ( aFinal )
				{ 
				iSyncMLCmds->ResponseController()->ResetL();
				}
			else
				{
				iSyncMLCmds->ResponseController()->RemoveAllReceivedOnes();
				}
			}
		else if ( !iSyncMLCmds->ResponseController()->StatusReceived( entryID ) )
			{
			TPtrC8 respContCmd = iSyncMLCmds->ResponseController()->Cmd(entryID);

			if ( ( iAlreadyAuthenticated && respContCmd == KNSmlAgentSyncHdr ) 
				 ||  respContCmd == KNSmlAgentResults  
				 ||  respContCmd == KNSmlAgentGet  )
				{
				}
			else
				{
				allReceived = EFalse;
				if ( aFinal )
					{
					InterruptL( iSyncMLCmds->ResponseController()->AppIndex( entryID ), TNSmlError::ESmlStatusMissing, ETrue, EFalse );
					}
				}
			}
		}
	return allReceived;
	}
	
//
// Authentications 
//

// ---------------------------------------------------------
// CNSmlAgentBase::SaveIfNonceL
// If server has transmitted NextNonce element, it is stored to Agent Log
// ---------------------------------------------------------
EXPORT_C void CNSmlAgentBase::SaveIfNonceL( const CNSmlResponseController& aResponseController, TInt aEntryID ) const
	{
	TPtrC8 chalType;
	chalType.Set( aResponseController.ChalType( aEntryID ) );
	if ( chalType.Length() == 0 )
		{
		return;
		}
	if ( chalType != KNSmlAgentAuthMD5 )
		{
		return;
		}
	TPtrC8 chalNextNonce;
	chalNextNonce.Set( aResponseController.ChalNextNonce( aEntryID ) );
	if ( chalNextNonce.Length() > 0 )
		{
		HBufC8* nextNonceDecoded = HBufC8::NewLC( chalNextNonce.Length() );
		TPtr8 nextNonceDecodedPtr( nextNonceDecoded->Des() );
		TPtrC8 chalFormat;
		chalFormat.Set( aResponseController.ChalFormat( aEntryID ) );
		if ( chalFormat == KNSmlAgentBase64Format )
			{
			//decode before saving
			TBase64 B64Coder;
			B64Coder.Decode( chalNextNonce, nextNonceDecodedPtr );
			}
		else
			{
			*nextNonceDecoded = chalNextNonce;
			}
		HBufC* nextNonceUnicode = HBufC::NewLC( nextNonceDecoded->Length() );
		nextNonceUnicode->Des().Copy( *nextNonceDecoded );
		SetNonceL( *nextNonceUnicode );
		CleanupStack::PopAndDestroy( 2 ); //nextNonceUnicode, nextNonceDecoded
		}
	}
// ---------------------------------------------------------
// CNSmlAgentBase::AuthenticationRequirementL
// Check if a server has challenged authetication
// ---------------------------------------------------------
EXPORT_C TBool CNSmlAgentBase::AuthenticationRequirementL( const CNSmlResponseController& aResponseController, TInt aEntryID )
	{
	TPtrC8 chalType;
	chalType.Set( aResponseController.ChalType( aEntryID ) );
	if ( chalType.Length() == 0 )
		{
		chalType.Set( KNSmlAgentAuthBasic );
		}
	if ( chalType != KNSmlAgentAuthBasic && chalType != KNSmlAgentAuthMD5 )
		{
		Interrupt( TNSmlError::ESmlChalInvalid, EFalse, EFalse );
		return EFalse;
		}
	if ( !iSyncMLUserName )
		{
		return EFalse;
		}
	
	TAuthenticationType prevAuthType;
	prevAuthType = AuthTypeL();
	if ( chalType == KNSmlAgentAuthBasic )
		{
		if ( prevAuthType == EBasic )
			{
			return EFalse;
			}
		else
			{
			SetAuthTypeL( EBasic );
			TPtrC empty;
			SetNonceL( empty );
			}
		}
	if ( chalType == KNSmlAgentAuthMD5 )
		{
		if ( prevAuthType == EMD5 )
			{
			// only one MD5 chal per session is accepted
			// loop must be prevented
			if (iMD5AlreadyRequested )
				{
				return EFalse;
				}
			}
		else
			{
			SetAuthTypeL( EMD5 );
			}
		iMD5AlreadyRequested = ETrue;
		}
	return ETrue;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ServerUserName()
// 
// ---------------------------------------------------------
EXPORT_C TPtrC CNSmlAgentBase::ServerUserName() const
	{
	return KNullDesC();
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ServerPassword()
// 
// ---------------------------------------------------------
EXPORT_C TPtrC CNSmlAgentBase::ServerPassword() const
	{
	return KNullDesC();
	}


// ---------------------------------------------------------
// CNSmlAgentBase::ProfileId()
// 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlAgentBase::ProfileId() const
	{
	return iProfileID;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::StatusReference()
// Returns reference to the iStatus member variable.
// ---------------------------------------------------------
EXPORT_C TRequestStatus& CNSmlAgentBase::StatusReference()
	{
	return iStatus;
	}


// ---------------------------------------------------------
// CNSmlAgentBase::FreeBaseResources()
// Release allocated resources
// ---------------------------------------------------------
//
EXPORT_C void CNSmlAgentBase::FreeBaseResources()
	{
	delete iImeiCode;
	iImeiCode = NULL;
	delete iSyncMLUserName;
	iSyncMLUserName = NULL;
	delete iSyncMLPassword;
	iSyncMLPassword = NULL;
	delete iSyncServer;
	iSyncServer = NULL;
	delete iSyncMLCmds;
	iSyncMLCmds = NULL;
	delete iSyncMLMIMEType;
	iSyncMLMIMEType = NULL;
	delete iResultAlertCode;
	iResultAlertCode = NULL;
	delete iEndOfDataAlertCode;
	iEndOfDataAlertCode = NULL;
	delete iSessionAbortAlertCode;
	iSessionAbortAlertCode = NULL;
	delete iNextMessageCode;
	iNextMessageCode = NULL;
	delete iTransport;		
	iTransport = NULL;
    delete iIAPIdArray;
    iIAPIdArray = NULL;
	delete iSyncHTTPAuthUserName;
	iSyncHTTPAuthUserName = NULL;
	delete iSyncHTTPAuthPassword;
	iSyncHTTPAuthPassword = NULL;
	}

//
// Begin state functions
//
// ---------------------------------------------------------
// CNSmlAgentBase::BeginConnectingStateL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::BeginConnectingStateL()
	{
	if ( !iCommandIssued ) 
		{
		ConnectIssueL();
		}
	else
		{
		ConnectDoneL();
		NextMainStateL();
		}
	}
//
// Client Initation state functions
//
// ---------------------------------------------------------
// CNSmlAgentBase::InitMakingStatusStateL()
// Writes all buffered status elements
// ---------------------------------------------------------
void CNSmlAgentBase::InitMakingStatusStateL()
	{
	TBool found( ETrue ); 
	if ( iSyncMLCmds->StatusContainer() )  
		{
		iSyncMLCmds->StatusContainer()->Begin();
		while( found )
			{
			SmlStatus_t* status;
			found = iSyncMLCmds->StatusContainer()->NextStatusElement( status, ETrue ); 
			if ( found )
				{
				iSyncMLCmds->DoStatusL( status );
				}
			}
		}
	iCurrClientInitState = EInitAlerting;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::InitEndingMessageStateL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::InitEndingMessageStateL()
	{
	iSyncMLCmds->DoEndMessageL();
	iFinalMessageFromClient = ETrue;
	NextMainStateL();
	}

// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusMakingAlertStateL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::DataUpdateStatusMakingAlertStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingAlertStateL begins"));
	if ( iEndOfDataAlertRequest )
		{
		iSyncMLCmds->DoAlertL( *iEndOfDataAlertCode );
		iEndOfDataAlertRequest = EFalse;
		iStatusPackage = ETrue;
		}
    if ( iSessionAbortAlertRequest )
		{
		iSyncMLCmds->DoAlertL( *iSessionAbortAlertCode );
		iSessionAbortAlertRequest = EFalse;
		iStatusPackage = ETrue;
		}
	iCurrDataUpdateStatusState = EStatusMakingMap;
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingAlertStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusMakingStatusStateL()
// Writes all buffered Status elements 
// ---------------------------------------------------------
void CNSmlAgentBase::DataUpdateStatusMakingStatusStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingStatusStateL begins"));
	CNSmlCmdsBase::TReturnValue ret = CNSmlCmdsBase::EReturnOK;
	TBool found( ETrue ); 
	iStatusPackage = EFalse;

	if ( iSyncMLCmds->StatusContainer() )
		{
		if ( iSyncMLCmds->StatusContainer()->AnyOtherThanOkSyncHdrStatus() )
			{
			// there is something to send
			iStatusPackage = ETrue;
			}
		iSyncMLCmds->StatusContainer()->Begin();
		while( found && ret == CNSmlCmdsBase::EReturnOK )
			{
			SmlStatus_t* status;
			found = iSyncMLCmds->StatusContainer()->NextStatusElement( status, ETrue ); 
			if ( found )
				{
				ret = iSyncMLCmds->DoStatusL( status );
				}
			}
		}
	if ( ret == CNSmlCmdsBase::EReturnBufferFull )
		{
		iBufferFull = ETrue;
		iCurrDataUpdateStatusState = EStatusEndingMessage;
		}
	else
		{
		iCurrDataUpdateStatusState = EStatusMakingAlert;
		}
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusMakingStatusStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::DataUpdateStatusEndingMessageStateL()
// End of SyncML Message
// ---------------------------------------------------------
void CNSmlAgentBase::DataUpdateStatusEndingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusEndingMessageStateL begins"));
	if ( iBufferFull )
		{
		iSyncMLCmds->DoEndMessageL( EFalse );
		iFinalMessageFromClient = EFalse;
		}
	else
		{
		iSyncMLCmds->DoEndMessageL();
		iFinalMessageFromClient = ETrue;
		}
	NextMainStateL();
	DBG_FILE(_S8("CNSmlAgentBase::DataUpdateStatusEndingMessageStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ResultAlertStartingMessageStateL()
// Makes SyncHdr
// ---------------------------------------------------------
void CNSmlAgentBase::ResultAlertStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertStartingMessageStateL begins"));
	if ( iResultAlertIssued )
		{
		User::Leave( TNSmlError::ESmlServerIsBusy );
		}
	else
		{
		iBusyStatusReceived = EFalse;
		iResultAlertIssued = ETrue;
		}
	iSyncMLCmds->DoSyncHdrL();
	iCurrResultAlertState = EResultAlertMakingAlert;
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertStartingMessageStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ResultAlertAlertingStateL()
// Makes Alert command
// ---------------------------------------------------------
void CNSmlAgentBase::ResultAlertAlertingStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertAlertingStateL begins"));
	if ( !Interrupted() )	
		{
		iSyncMLCmds->DoAlertL( *iResultAlertCode );
		}
	iCurrResultAlertState = EResultAlertEndingMessage;
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertAlertingStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ResultAlertEndingMessageStateL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::ResultAlertEndingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertEndingMessageStateL begins"));
	iSyncMLCmds->DoEndMessageL( EFalse );
	iFinalMessageFromClient = EFalse;
	NextMainStateL();
	DBG_FILE(_S8("CNSmlAgentBase::ResultAlertEndingMessageStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextStartingMessageStateL()
// Makes SyncHdr
// ---------------------------------------------------------
void CNSmlAgentBase::ClientAlertNextStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextStartingMessageStateL begins"));
	iSyncMLCmds->DoSyncHdrL();
	iCurrClientAlertNextState = EClientAlertNextMakingAlert;
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextStartingMessageStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextAlertingStateL()
// Makes Alert command
// ---------------------------------------------------------
void CNSmlAgentBase::ClientAlertNextAlertingStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextAlertingStateL begins"));
	if ( !Interrupted() ) 
		{
		iSyncMLCmds->DoAlertL( *iNextMessageCode );
		}
	if ( iEndOfDataAlertRequest )
		{
		iSyncMLCmds->DoAlertL( *iEndOfDataAlertCode );
		iEndOfDataAlertRequest = EFalse;
		}
	if ( iSessionAbortAlertRequest )
		{
		iSyncMLCmds->DoAlertL( *iSessionAbortAlertCode );
		iSessionAbortAlertRequest = EFalse;
		}
	iCurrClientAlertNextState = EClientAlertNextMakingStatus;
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextAlertingStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextMakingStatusStateL()
// Writes all buffered Status elements  
// ---------------------------------------------------------
void CNSmlAgentBase::ClientAlertNextMakingStatusStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextMakingStatusStateL begins"));
	CNSmlCmdsBase::TReturnValue ret = CNSmlCmdsBase::EReturnOK;
	if ( iSyncMLCmds->StatusContainer() )
		{
		iSyncMLCmds->StatusContainer()->Begin();
		TBool found( ETrue ); 
		while( found )
			{
			SmlStatus_t* status;
			found = iSyncMLCmds->StatusContainer()->NextStatusElement( status, ETrue ); 
			if ( found )
				{
				ret = iSyncMLCmds->DoStatusL( status );
				}
			}
		}
	if ( ret == CNSmlCmdsBase::EReturnBufferFull )
		{
		iCurrClientAlertNextState = EClientAlertNextEndingMessage;
		}
	else
		{
		iCurrClientAlertNextState = EClientAlertNextMakingMap;
		}
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextMakingStatusStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ClientAlertNextEndingMessageStateL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::ClientAlertNextEndingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextEndingMessageStateL begins"));
	iSyncMLCmds->DoEndMessageL( EFalse );
	iFinalMessageFromClient = EFalse;
	NextMainStateL();
	DBG_FILE(_S8("CNSmlAgentBase::ClientAlertNextEndingMessageStateL ends"));
	}
//
//  Transport functions
//
// ---------------------------------------------------------
// CNSmlAgentBase::ConnectIssueL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::ConnectIssueL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ConnectIssueL begins"));
	#ifndef __NOTRANSPORT
	HBufC8* uri8 = NULL;
	HBufC8* httpusername8 = NULL;
	HBufC8* httppassword8 = NULL;

	NSmlUnicodeConverter::HBufC8InUTF8LC( iSyncServer->HostNameWithPortL(), uri8 );
	if( iSyncHTTPAuthUserName )
		{
		TPtrC uname( iSyncHTTPAuthUserName->Des() );	
		httpusername8 = HBufC8::NewLC( uname.Length() );
		httpusername8->Des().Copy( uname );

		TPtrC pword( iSyncHTTPAuthPassword->Des() );
		httppassword8 = HBufC8::NewLC( pword.Length() );
		httppassword8->Des().Copy( pword );
		}
	else
		{	
		httpusername8 = KNullDesC8().AllocLC();
		httppassword8 = KNullDesC8().AllocLC();
		}

	iTransport->ConnectL( iMediumType, iSyncInitiation == EServerAlerted, iIAPIdArray, *uri8, *iSyncMLMIMEType, iStatus, *httpusername8, *httppassword8, iSyncHTTPAuthUsed );	
    CleanupStack::PopAndDestroy(3);  //uri8, httpusername8, httppassword8
	#endif
	iCommandIssued = ETrue;	
	DBG_FILE(_S8("CNSmlAgentBase::ConnectIssueL ends"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ConnectDoneL()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::ConnectDoneL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ConnectDoneL begins"));
	iCommandIssued = EFalse;
	if ( iStatus != KErrNone ) 
		{
		User::Leave( iStatus.Int() );
		}
	//RD_AUTO_RESTART		
	//Get the Access Point ID used for Synchronizing
	ReadAcessPointL();
	//RD_AUTO_RESTART		
	DBG_FILE(_S8("CNSmlAgentBase::ConnectDoneL ends"));
	}

//RD_AUTO_RESTART
// -----------------------------------------------------------------------------
// CNSmlAgentBase::ReadAcessPointL
// Reads the open connections' Access Point Id.
// -----------------------------------------------------------------------------
void CNSmlAgentBase::ReadAcessPointL()
{
	// Take AP from open connection
    RSocketServ socketServer;
    TInt err( KErrNone );
    err = socketServer.Connect();
    RConnection myConnection;
    err = myConnection.Open( socketServer );
    TUint connectionCount( 0 );
    err = myConnection.EnumerateConnections( connectionCount );
    DBG_FILE_CODE(connectionCount, _S8("CNSmlAgentBase::ReadAcessPointL(), The Connection count is:"));
    if ( err != KErrNone || connectionCount < 1 )
    {
        iNetmonAPId = 0;
    }

    else
    {

		TPckgBuf<TConnectionInfoV2> connectionInfo;
		err = myConnection.GetConnectionInfo( connectionCount,
				connectionInfo );	
		iNetmonAPId = connectionInfo().iIapId;
		DBG_FILE_CODE(iNetmonAPId, _S8("CNSmlAgentBase::ReadAcessPointL(), The IAPId is:"));
		RCmManager  cmmanager;
		cmmanager.OpenL();
		CleanupClosePushL(cmmanager);
		RCmConnectionMethod cm;
		cm = cmmanager.ConnectionMethodL( iNetmonAPId );
		CleanupClosePushL( cm );
		TUint32 bearer = 0;
		//TRAP_IGNORE( accesspointId = cm.GetIntAttributeL(CMManager::ECmIapId) );?
		bearer = cm.GetIntAttributeL( CMManager::ECmBearerType );
		CleanupStack::PopAndDestroy( 2 ); //cmmanager,cm
		if ( bearer == KUidWlanBearerType )
		{
			 iAllowAutoRestart = EFalse;
		}
		else
		{
			iAllowAutoRestart = ETrue;
		}
	}

    myConnection.Close();
    socketServer.Close();
}
//RD_AUTO_RESTART

// ---------------------------------------------------------
// CNSmlAgentBase::SendDataIssueL()
// Send data to a server
// This is asynchronous request. 
// ---------------------------------------------------------
void CNSmlAgentBase::SendDataIssueL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::SendDataIssueL begins"));
	TPtrC8 document( iSyncMLCmds->GeneratedDocument() );

#ifdef _DEBUG
    {
    CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
    c->ConvertL( document.Ptr(), document.Length() );
    RFs fs;
    User::LeaveIfError( fs.Connect() );  // create connect to fileserver
    CleanupClosePushL( fs );
    if( !BaflUtils::FolderExists( fs, _L("C:\\logs\\Sync\\") ) )
        {
        fs.MkDirAll( _L("C:\\logs\\Sync\\") );
        }
    _LIT( KLogFile, "C:\\logs\\Sync\\SendDataIssue.txt" );
    RFile logFile;
    CleanupClosePushL( logFile );

    TInt ret = logFile.Open( fs, KLogFile, EFileShareExclusive|EFileWrite ); // open file
    if( ret == KErrNotFound )  // if file does not exist, create it
        {
        logFile.Create( fs, KLogFile, EFileShareExclusive|EFileWrite );
        }

    TInt size = 0;
    logFile.Size( size );
    logFile.Write( size, c->Document() );
    _LIT8( KNewLine, "\n\n" );
    logFile.Write( KNewLine );
    CleanupStack::PopAndDestroy( 3, c );//privateFile, fs, c
    }
#endif
#ifdef __NOTRANSPORT
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	c->ConvertL(document.Ptr(), document.Length());
	DBG_DUMP((void*)iSyncMLCmds->GeneratedDocument().Ptr(), iSyncMLCmds->GeneratedDocument().Length(), _S8("SendDataL (WBXML)\n") );
	DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("SendDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c
#else
	if ( iSyncMLCmds->RespURI() != NULL )
		{
		iTransport->ChangeTargetURIL( *iSyncMLCmds->RespURI()->HostNameInUTF8AllocLC() );
		CleanupStack::PopAndDestroy();
		}
	iTransport->Send( document, EFalse, iStatus ); 
#endif  // __NOTRANSPORT
	iCommandIssued = ETrue;	
	DBG_FILE(_S8("CNSmlAgentBase::SendDataIssueL ends"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::SendDataDoneL()
// Response to asynchronous SendDataIssueL() call
// ---------------------------------------------------------
void CNSmlAgentBase::SendDataDoneL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::SendDataDoneL begins"));
	DBG_FILE_CODE(iStatus.Int(), _S8(" CNSmlAgentBase::SendDataDoneL ERROR CODE is "));
	iCommandIssued = EFalse;
	if ( iStatus != KErrNone ) 
		{
		DBG_FILE(_S8("CNSmlAgentBase::SendDataDoneL ERROR"));
		User::Leave( iStatus.Int() );
		}
	DBG_FILE(_S8("CNSmlAgentBase::SendDataDoneL ends"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::ReceiveDataIssueL()
// Receive data from a server
// This is asynchronous request.
// ---------------------------------------------------------
void CNSmlAgentBase::ReceiveDataIssueL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ReceiveDataIssueL begins"));
#ifndef __NOTRANSPORT
	iBufferArea.Set(iSyncMLCmds->BufferAreaForParsingL());
	iTransport->Receive( iBufferArea, iStatus );

#ifdef _DEBUG
    {
    TPtrC8 document( iSyncMLCmds->BufferAreaForDebugDumpL() );
    CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
    TRAPD( err, c->ConvertL( document.Ptr(), document.Length() ) );
    if ( err == KErrNone )
        {
        c->ConvertL( document.Ptr(), document.Length() );
        RFs fs;
        User::LeaveIfError( fs.Connect() );  // create connect to fileserver
        CleanupClosePushL( fs );
        if( !BaflUtils::FolderExists( fs, _L("C:\\logs\\Sync\\") ) )
            {
            fs.MkDirAll( _L("C:\\logs\\Sync\\") );
            }
        _LIT( KLogFile, "C:\\logs\\Sync\\ReceiveDataIssue.txt" );
        RFile logFile;
        CleanupClosePushL( logFile );

        TInt ret = logFile.Open( fs, KLogFile, EFileShareExclusive|EFileWrite ); // open file
        if( ret == KErrNotFound )  // if file does not exist, create it
            {
            logFile.Create( fs, KLogFile, EFileShareExclusive|EFileWrite );
            }
        TInt size = 0;
        logFile.Size( size );
        logFile.Write( size, c->Document() );
        _LIT8( KNewLine, "\n\n" );
        logFile.Write( KNewLine );
        CleanupStack::PopAndDestroy( 3, c );//logFile, fs, c
        }
    }
#endif

#else  
	//TESTSEQU
	_LIT(KServerInitFile,"C:\\system\\data\\servinit.wbxml");
	_LIT(KServerInitFile1,"C:\\system\\data\\servinit1.wbxml");
	_LIT(KServerInitFile2,"C:\\system\\data\\servinit2.wbxml");
	_LIT(KServerDSModFile, "C:\\system\\data\\servmod%d.wbxml");
    _LIT(KServerModFile,"C:\\system\\data\\servmod.wbxml");
	_LIT(KServerModFile1,"C:\\system\\data\\servmod1.wbxml");
	_LIT(KServerModFile2,"C:\\system\\data\\servmod2.wbxml");
	_LIT(KServerModFile3,"C:\\system\\data\\servmod3.wbxml");
	_LIT(KServerMapFile,"C:\\system\\data\\servmap.wbxml");

	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile syncmlFile;



#ifdef __DM_MODULETEST
     if ( iPrevSyncState == EClientInitialisation || iPrevSyncState == EServerInitialisation ) 
		{
		User::LeaveIfError( syncmlFile.Open(fsSession, KServerInitFile, EFileShareAny) );
		}
	 else
#ifdef __SEVERAL_SERVER_MODS
		{  
		iTestCounter ++;
		if ( iTestCounter == 3 )
			{
			User::LeaveIfError( syncmlFile.Open(fsSession, KServerModFile3, EFileShareAny) );
			}
		else
		if ( iTestCounter == 2 )
			{
			User::LeaveIfError( syncmlFile.Open(fsSession, KServerModFile2, EFileShareAny) );
			}
		else
			{
			User::LeaveIfError( syncmlFile.Open(fsSession, KServerModFile1, EFileShareAny) );
			}
		}
#else
		User::LeaveIfError( syncmlFile.Open(fsSession, KServerModFile, EFileShareAny) );
#endif // __SEVERAL_SERVER_MODS

#else
	if ( iPrevSyncState == EClientInitialisation || iPrevSyncState == EServerInitialisation ) 
		{
#ifdef __TWO_SERVER_INITS
        ++iTestCounter;
		if ( iTestCounter == 2 )
			{
			User::LeaveIfError( syncmlFile.Open(fsSession, KServerInitFile2, EFileShareAny) );
			}
		else
			{
			User::LeaveIfError( syncmlFile.Open(fsSession, KServerInitFile1, EFileShareAny) );
			} 
#else
		User::LeaveIfError( syncmlFile.Open(fsSession, KServerInitFile, EFileShareAny) );
#endif // __TWO_SERVER_INITS
		}
	else if (iPrevSyncState == EClientModifications || iPrevSyncState == EServerModifications )		
		{
		TBuf<35> modFile;
		modFile.Format( KServerDSModFile, ++iTestCounter );
		User::LeaveIfError( syncmlFile.Open( fsSession, modFile, EFileShareAny ) );
		}
	else
		{
		User::LeaveIfError( syncmlFile.Open(fsSession, KServerMapFile, EFileShareAny) );
		}
#endif // __DM_MODULE_TEST
	TPtr8 ptr = iSyncMLCmds->BufferAreaForParsingL();
	syncmlFile.Read( ptr );
	syncmlFile.Close();
	fsSession.Close();

	DBG_DUMP((void*)iSyncMLCmds->BufferAreaForDebugDumpL().Ptr(), iSyncMLCmds->BufferAreaForDebugDumpL().Length(), _S8("ReceiveDataL (XML)\n") );
	TPtrC8 document( iSyncMLCmds->BufferAreaForDebugDumpL() );
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	TRAPD(err,c->ConvertL(document.Ptr(), document.Length()));
	if ( err == KErrNone )
		DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("ReceiveDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c

#endif // __NOTRANSPORT
	
	iCommandIssued = ETrue;	
	DBG_FILE(_S8("CNSmlAgentBase::ReceiveDataIssueL begins"));
	}
// ---------------------------------------------------------
// CNSmlAgentBase::ReceiveDataDoneL()
// Response to asynchronous ReceiveDataIssueL() call
// ---------------------------------------------------------
void CNSmlAgentBase::ReceiveDataDoneL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ReceiveDataDoneL begins"));
	iCommandIssued = EFalse;
	if ( iStatus != KErrNone ) 
		{
		DBG_FILE(_S8("CNSmlAgentBase::ReceiveDataDoneL ERROR"));
		User::Leave( iStatus.Int() );
		}
	DBG_FILE(_S8("CNSmlAgentBase::ReceiveDataDoneL begins"));
	}

// ---------------------------------------------------------
// CNSmlAgentBase::Disconnect()
// 
// ---------------------------------------------------------
void CNSmlAgentBase::Disconnect()
	{
	DBG_FILE(_S8("CNSmlAgentBase::Disconnect begins"));
	#ifndef __NOTRANSPORT
	iTransport->Disconnect();
	#endif
	iCommandIssued = ETrue;	
	DBG_FILE(_S8("CNSmlAgentBase::Disconnect ends"));
	}
//
// Error Handling functions
//
// ---------------------------------------------------------
// CNSmlAgentBase::ErrorHandling()
// This function is called if sync is broken down immediately (Leave errors)
// This function MUST NOT leave.
// ---------------------------------------------------------
void CNSmlAgentBase::ErrorHandling( TInt aErrorCode )
	{
	DBG_FILE(_S8("CNSmlAgentBase::ErrorHandling begins"));
	
	DBG_FILE_CODE(aErrorCode, _S8(" CNSmlAgentBase::ErrorHandling ERROR CODE is "));
	if ( iError )
		{
		iError->SetErrorCode( aErrorCode );
		}
		
	TRAP_IGNORE( FinaliseWhenErrorL() );
	
	TRAP_IGNORE(  
	
	if ( iMediumType != KUidNSmlMediumTypeInternet &&
	     iCurrMainState < EClientInitialisation &&
	     iSyncInitiation == EServerAlerted )
		{		
		delete iTransport;
		iTransport = 0;	
		
		iTransport = CNSmlTransport::NewL();
			
		TRequestStatus status;
		TDesC8 temp = KNullDesC8();
		
	
		iTransport->ConnectL( KUidNSmlMediumTypeUSB, ETrue, NULL, temp, *iSyncMLMIMEType , status, temp, temp, 0 ); 
		User::WaitForRequest(status);
		
		if (status.Int() == KErrNone)
			{
			iTransport->Disconnect();
			}
		}
	
	)
	
	//RD_AUTO_RESTART		
	if(iPacketDataUnAvailable)
	{
		DBG_FILE(_S8("CNSmlAgentBase::FinaliseWhenErrorL Prompting for a Dialog"));
		iError->SetErrorCode( TNSmlError::ESmlCommunicationError);		
		User::RequestComplete( iCallerStatus, TNSmlError::ESmlCommunicationError );
	}
	//RD_AUTO_RESTART
	else if ( aErrorCode == KErrCancel )
		{
		User::RequestComplete( iCallerStatus, KErrCancel );
		}
	else
		{
		User::RequestComplete( iCallerStatus, aErrorCode );
		}
		
	// free resources to get space (if memory is full) to write to Sync Log.
	FreeResources();
	
	FinalizeSyncLog();
	
	iEnd = ETrue;
	DBG_FILE(_S8("CNSmlAgentBase::ErrorHandling ends"));
	}

//RD_AUTO_RESTART
// ---------------------------------------------------------
// CNSmlAgentBase::LaunchAutoRestart()
// **Auto Restart**
// ---------------------------------------------------------	
EXPORT_C void CNSmlAgentBase::LaunchAutoRestartL(TInt aError)
{
	//Do Nothing
}
//  End of File  
