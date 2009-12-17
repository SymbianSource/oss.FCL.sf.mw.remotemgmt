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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "ClientAPI.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClientAPI::CClientAPI
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CClientAPI::CClientAPI( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CClientAPI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClientAPI::ConstructL()
    {
    iLog = CStifLogger::NewL( KClientAPILogPath, 
                          KClientAPILogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse );
    
	// print one empty line to log between test cases
    iLog->Log( _L("") );
	
    }

// -----------------------------------------------------------------------------
// CClientAPI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClientAPI* CClientAPI::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CClientAPI* self = new (ELeave) CClientAPI( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CClientAPI::~CClientAPI()
    {
    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iLog;  
    }



// -----------------------------------------------------------------------------
// Event callback, event handling.
// -----------------------------------------------------------------------------
void EventCallback::OnSyncMLSessionEvent( TEvent aEvent, TInt aIdentifier, TInt aError, TInt aAdditionalData )
	{
	CStifLogger* event_logger;
	TRAPD( err, event_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("events.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
	event_logger->Log( _L("Event received:") );
	switch( aEvent )
		{
		case EJobStart:
			event_logger->Log( _L("\t* Job started") );
			break;
		case EJobStartFailed:
			event_logger->Log( _L("\t* Job start failed") );
			break;
		case EJobStop:
			event_logger->Log( _L("\t* Job stopped") );
			break;
		case EJobRejected:
			event_logger->Log( _L("\t* Job rejected") );
			break;
		case EProfileCreated:
			event_logger->Log( _L("\t* Profile created") );
			break;
		case EProfileChanged:
			event_logger->Log( _L("\t* Profile changed") );
			break;
		case EProfileDeleted:
			event_logger->Log( _L("\t* Profile deleted") );
			break;
		case ETransportTimeout:
			event_logger->Log( _L("\t* Transport timeout") );
			break;
		case EServerSuspended:
			event_logger->Log( _L("\t* Server suspended") );
			break;
		case EServerTerminated:
			event_logger->Log( _L("\t* Server terminated") );
			break;
		default:
			break;
		}
	
	event_logger->Log( _L("\t* Identifier: %d"), aIdentifier );
	event_logger->Log( _L("\t* Error: %d"), aError );
	event_logger->Log( _L("\t* Additional data: %d"), aAdditionalData );
	delete event_logger;
	}


void EventCallback2::OnSyncMLSessionEvent( TEvent /*aEvent*/, TInt /*aIdentifier*/, TInt /*aError*/, TInt /*aAdditionalData*/ )
	{
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void ProgressCallback::OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 )
	{
	CStifLogger* progress_logger;
	TRAPD( err, progress_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("progress.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
                          
	progress_logger->Log( _L("Progress ERROR received:") );
	
	switch( aErrorLevel )
		{
		case ESmlWarning:
			progress_logger->Log( _L("\t* Error level: warning") );
			break;
		case ESmlFatalError:
			progress_logger->Log( _L("\t* Error level: fatal error") );
			break;
		default:
			progress_logger->Log( _L("\t* Error level: UNKNOWN %d"), aErrorLevel );
			break;
		}
	
	progress_logger->Log( _L("\t* Error: %d"), aError );
	progress_logger->Log( _L("\t* Task id: %d"), aTaskId );
	progress_logger->Log( _L("\t* Info1: %d"), aInfo1 );
	progress_logger->Log( _L("\t* Info2: %d"), aInfo2 );
	delete progress_logger;
	}
	
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void ProgressCallback::OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2 )
	{
	CStifLogger* progress_logger;
	TRAPD( err, progress_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("progress.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
                          
	progress_logger->Log( _L("Progress STATUS received:") );
	
	switch( aStatus )
		{
		case ESmlConnecting:
			progress_logger->Log( _L("\t* Status: connecting") );
			break;
		case ESmlConnected:
			progress_logger->Log( _L("\t* Status: connected") );
			break;
		case ESmlLoggingOn:
			progress_logger->Log( _L("\t* Status: logging on") );
			break;
		case ESmlLoggedOn:
			progress_logger->Log( _L("\t* Status: logged on") );
			break;
		case ESmlDisconnected:
			progress_logger->Log( _L("\t* Status: disconnected") );
			break;
		case ESmlCompleted:
			progress_logger->Log( _L("\t* Status: completed") );
			break;
		case ESmlSendingModificationsToServer:
			progress_logger->Log( _L("\t* Status: sending modifications to server") );
			break;
		case ESmlReceivingModificationsFromServer:
			progress_logger->Log( _L("\t* Status: receiving modifications from server") );
			break;
		case ESmlSendingMappingsToServer:
			progress_logger->Log( _L("\t* Status: sending mappings to server") );
			break;
		case ESmlProcessingServerCommands:
			progress_logger->Log( _L("\t* Status: processing server commands") );
			break;
		case ESmlReceivingServerCommands:
			progress_logger->Log( _L("\t* Status: receiving server commands") );
			break;
		default:
			progress_logger->Log( _L("\t* Status: UNKNOWN %d"), aStatus );
			break;
		}
	
	progress_logger->Log( _L("\t* Info1: %d"), aInfo1 );
	progress_logger->Log( _L("\t* Info2: %d"), aInfo2 );
	delete progress_logger;
	}
	
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void ProgressCallback::OnSyncMLDataSyncModifications( TInt aTaskId,
								const TSyncMLDataSyncModifications& aClientModifications,
								const TSyncMLDataSyncModifications& aServerModifications )
	{
	CStifLogger* progress_logger;
	TRAPD( err, progress_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("progress.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
                          
	progress_logger->Log( _L("Progress MODIFICATIONS received:") );
	progress_logger->Log( _L("\t* Task id: %d"), aTaskId );
	
	progress_logger->Log( _L("\t* Client modifications:") );
	progress_logger->Log( _L("\t\t* Added: %d"), aClientModifications.iNumAdded );
	progress_logger->Log( _L("\t\t* Replaced: %d"), aClientModifications.iNumReplaced );
	progress_logger->Log( _L("\t\t* Deleted: %d"), aClientModifications.iNumDeleted );
	progress_logger->Log( _L("\t\t* Moved: %d"), aClientModifications.iNumMoved );
	progress_logger->Log( _L("\t\t* Failed: %d"), aClientModifications.iNumFailed );
	
	progress_logger->Log( _L("\t* Server modifications:") );
	progress_logger->Log( _L("\t\t* Added: %d"), aServerModifications.iNumAdded );
	progress_logger->Log( _L("\t\t* Replaced: %d"), aServerModifications.iNumReplaced );
	progress_logger->Log( _L("\t\t* Deleted: %d"), aServerModifications.iNumDeleted );
	progress_logger->Log( _L("\t\t* Moved: %d"), aServerModifications.iNumMoved );
	progress_logger->Log( _L("\t\t* Failed: %d"), aServerModifications.iNumFailed );
	delete progress_logger;
	}
	


void ProgressCallback2::OnSyncMLSyncError( TErrorLevel /*aErrorLevel*/, TInt /*aError*/, TInt /*aTaskId*/, TInt /*aInfo1*/, TInt /*aInfo2*/ )
	{
	}

void ProgressCallback2::OnSyncMLSyncProgress( TStatus /*aStatus*/, TInt /*aInfo1*/, TInt /*aInfo2*/ )
	{
	}

void ProgressCallback2::OnSyncMLDataSyncModifications( TInt /*aTaskId*/, const TSyncMLDataSyncModifications& /*aClientModifications*/, const TSyncMLDataSyncModifications& /*aServerModifications*/ )
	{
	}
	
void ProgressCallbackErrorTest::OnSyncMLSyncError( TErrorLevel /*aErrorLevel*/, TInt aError, TInt /*aTaskId*/, TInt /*aInfo1*/, TInt /*aInfo2*/ )
	{
	if ( aError != SyncMLError::KErrClientDatabaseNotFound )
		{
		//wrong error
		iCaseFailed = ETrue;
		}
	}

void ProgressCallbackErrorTest::OnSyncMLSyncProgress( TStatus /*aStatus*/, TInt /*aInfo1*/, TInt /*aInfo2*/ )
	{
	}

void ProgressCallbackErrorTest::OnSyncMLDataSyncModifications( TInt /*aTaskId*/, const TSyncMLDataSyncModifications& /*aClientModifications*/, const TSyncMLDataSyncModifications& /*aServerModifications*/ )
	{
	}	
	
	

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    return ( CScriptBase* ) CClientAPI::NewL( aTestModuleIf );
        
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);
    
    }
#endif // EKA2
    
//  End of File
