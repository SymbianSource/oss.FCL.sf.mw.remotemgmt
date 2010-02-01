/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <e32math.h>
#include <e32uid.h>
#include <AknGlobalNote.h>
#include <aknnotewrappers.h> 
#include <AknQueryDialog.h>
#include <AknGlobalConfirmationQuery.h>

#include "SCPClient.h"
#include "SCPQueryDialog.h"
#include "SCPParamObject.h"

#include <SCPNotifier.rsg>
#include "SCP_IDs.h"

#include <centralrepository.h>
#include "SCPLockCode.h"
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
#include <TerminalControl3rdPartyAPI.h>
#include <SCPServerInterface.h>
#include <SecUi.rsg>
#include <SecUi.hrh>
#include <StringLoader.h>
#include <bautils.h>
//#endif // DEVICE_LOCK_ENHANCEMENTS

#include <featmgr.h>
#include "SCPDebug.h"
#include <e32property.h>
/*#ifdef _DEBUG
#define __SCP_DEBUG
#endif // _DEBUG

// Define this so the precompiler in CW 3.1 won't complain about token pasting,
// the warnings are not valid
#pragma warn_illtokenpasting off

#ifdef __SCP_DEBUG
#define Dprint(a) RDebug::Print##a
#else
#define Dprint(a)
#endif // _DEBUG*/

static const TUint KDefaultMessageSlots = 3;
static const TInt KSCPConnectRetries( 2 );


//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
_LIT( KDriveZ, "Z:" );
_LIT( KSCPResourceFilename, "\\Resource\\SCPNotifier.RSC" );
_LIT( KSCPSecUIResourceFilename, "\\Resource\\SecUi.RSC" );
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS

// Uid for the application; this should match the mmp file
const TUid KServerUid3 = {0x10207836};

#ifdef __WINS__
static const TUint KServerMinHeapSize =  0x1000;  //  4K
static const TUint KServerMaxHeapSize = 0x10000;  // 64K
#endif

static TInt StartServer();
static TInt CreateServerProcess();


// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ---------------------------------------------------------
// StartServer() Server starter check
// Determines if the server is running, if not, calls the starter function
// Returns: TInt: Operation status, a standard error code
//
// Status : Approved
// ---------------------------------------------------------
//
static TInt StartServer()
    {
    Dprint( (_L("--> SCPClient::StartServer()") ));        
        
    TInt result;
    
    TFindServer findSCPServer( KSCPServerName );
    TFullName name;

    result = findSCPServer.Next( name );
    if ( result != KErrNone )
        {
        // The server is not running, try to create the server process
        result = CreateServerProcess();
        } 

    Dprint( (_L("<-- SCPClient::StartServer(): Exit code: %d"), result ));
    return result;
    }


// ---------------------------------------------------------
// CreateServerProcess() Server starter function
// Starts the SCP server
// Returns: TInt: Operation status, a standard error code
//
// Status : Approved
// ---------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    Dprint( (_L("--> SCPClient::CreateServerProcess()") ));
    TInt result;

    const TUidType serverUid( KNullUid, KNullUid, KServerUid3 );

    RProcess server;

    _LIT( KEmpty, "");   
    result = server.Create( KSCPServerFileName, KEmpty );

    if ( result != KErrNone )
        {
        Dprint( (_L("<-- SCPClient::CreateServerProcess(), process creation error!") ));
        return result;
        }

    TRequestStatus stat;
   
    server.Rendezvous(stat);
  
    if ( stat != KRequestPending )
        {
        server.Kill(0);    // abort startup
        }    
    else
        {
        server.Resume(); // logon OK - start the server
        }
    
    User::WaitForRequest(stat); // wait for start or death
  
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    result = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
  
    server.Close();

    Dprint( (_L("<-- SCPClient::CreateServerProcess(): %d"), result ));
    return result;
    }

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

// ---------------------------------------------------------
// RunDialog() Dialog execution wrapper
// Initialize and run the query dialog
// Returns: TInt: The return code from the dialog.
//          Can leave with a generic error code.
//
// Status : Approved
// ---------------------------------------------------------
//
TInt RunDialogL( TDes& aReplyBuf,
                RSCPClient::TSCPButtonConfig aButtonsShown,
                TInt aMinLen,
                TInt aMaxLen,
                TUint aResId = 0,
                TDesC* aPrompt = NULL,
                TBool aECSSupport = EFalse         
              )
    {
    Dprint(_L("[RSCPClient]-> RunDialogL() >>> "));
    FeatureManager::InitializeLibL();
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
			FeatureManager::UnInitializeLib();
		return KErrNotSupported;
	}
		FeatureManager::UnInitializeLib();
    if ( ( aPrompt == NULL ) && ( aResId == 0 ) )
        {
        return KErrArgument;
        }
    Dprint((_L("--> SCPClient::RunDialogL() start the dialog")));    
    CSCPQueryDialog* dialog = new (ELeave) CSCPQueryDialog( 
        aReplyBuf,
        aButtonsShown,
        aMinLen,
        aMaxLen,
        aECSSupport
        ); 
        
    CleanupStack::PushL( dialog );            
    
    if ( aResId != 0 )
        {
        // Load and set the prompt from a resource ID
        HBufC* prompt;
        
        prompt = StringLoader::LoadLC( aResId );   
        dialog->SetPromptL( *prompt ); 
        
        CleanupStack::PopAndDestroy( prompt );       
        }
    else
        {
        // Set the given prompt
        dialog->SetPromptL( *aPrompt );
        }        
    Dprint((_L("-- SCPClient::RunDialogL() dialog->ExecuteLD")));
    TInt ret = dialog->ExecuteLD( R_SCP_CODE_QUERY );
    
    CleanupStack::Pop( dialog );
    Dprint( (_L("-- SCPClient::RunDialogL(): ret val %d"), ret));
    return ret;
    }
    

// ---------------------------------------------------------
// LoadResources() Resource loader
// Load the resources for the library
// Returns: TInt: A generic error code.
//
// Status : Approved
// ---------------------------------------------------------
//
TInt LoadResources( TInt& aRes1, TInt& aRes2 )
    {
    
   TRAPD ( err, FeatureManager::InitializeLibL() );   
   if ( err == KErrNone )
   {
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
			FeatureManager::UnInitializeLib();
		return KErrNotSupported;
	}
		FeatureManager::UnInitializeLib();
    // Load the resource files for this DLL
    TInt err = KErrNone;
    TInt err2 = KErrNone;
       
    // Localize the file name, and load the SCP resources
    TFileName resFile;
    resFile.Copy( KDriveZ );
    resFile.Append( KSCPResourceFilename );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );    
    TRAP( err, aRes1 = CCoeEnv::Static()->AddResourceFileL( resFile ) );
    
    if ( err == KErrNone )
        {
        // Localize the file name, and load the SecUi resources
        resFile.Copy( KDriveZ );
        resFile.Append( KSCPSecUIResourceFilename );
        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
        TRAP( err2, aRes2 = CCoeEnv::Static()->AddResourceFileL( resFile ) );
        }   
             
    if ( ( err != KErrNone ) || ( err2 != KErrNone ) )
        {        
        if ( err == KErrNone )             
            {
            // First resource was loaded OK, remove it
            CCoeEnv::Static()->DeleteResourceFile( aRes1 );
            err = err2;
            }
        } 
    }        
    return err;           
    }     

//#endif //  __SAP_DEVICE_LOCK_ENHANCEMENTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C RSCPClient::RSCPClient()
:   RSessionBase()
    {
    // No implementation required
    }

// ---------------------------------------------------------
// TInt RSCPClient::Connect()
// Creates a new session, and starts the server, if required.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::Connect()
    {
    Dprint( (_L("--> RSCPClient::Connect()") ));
    
    // Use a mutex-object so that two processes cannot start the server at the same time
    RMutex startMutex;
    
    TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
		{
		return errf;
		}
		if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
		{
			isFlagEnabled = ETrue;
		}
		else
		{
			isFlagEnabled = EFalse;
		}
		FeatureManager::UnInitializeLib();
    TInt mRet = startMutex.OpenGlobal( KSCPServerName );
    if ( mRet == KErrNotFound )
        {
        mRet = startMutex.CreateGlobal( KSCPServerName );
        }
        
    if ( mRet != KErrNone )
        {        
        return mRet;
        }
    
    // Acquire the mutex
    startMutex.Wait();
    
    TInt retry = KSCPConnectRetries;
    TInt r;
    for (;;)
        {        
        r = CreateSession( KSCPServerName, Version(), KDefaultMessageSlots );
        
        if ( ( r != KErrNotFound ) && ( r != KErrServerTerminated  ) )
            {
            break;
            }
        
        if ( --retry == 0 )
            {
            break;
            }
      
        r = StartServer();
        
        if ( ( r != KErrNone ) && ( r != KErrAlreadyExists ) )
            {
            break;   
            }
        }
    
    Dprint( (_L("<-- RSCPClient::Connect(), exiting: %d"), r ));
    
    // Release the mutex
    startMutex.Signal();
    startMutex.Close();

    return r;
    }


// ---------------------------------------------------------
// TVersion RSCPClient::Version()
// Constructs a TVersion object containing the defined version
// numbers, and returns it
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TVersion RSCPClient::Version() const
    {
    Dprint( (_L("<--> RSCPClient::Version()") ));
    return( TVersion( KSCPServMajorVersionNumber,
                      KSCPServMinorVersionNumber,
                      KSCPServBuildVersionNumber ) );
    }


// ---------------------------------------------------------
// TInt RSCPClient::GetCode( TSCPSecCode& aCode )
// Requests the stored ISA code.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::GetCode( TSCPSecCode& aCode )
    {
    Dprint( (_L("--> RSCPClient::GetCode()") ));
        
    TInt ret = SendReceive(ESCPServGetCode, TIpcArgs( &aCode ) );
  
    Dprint( (_L("<-- RSCPClient::GetCode(): %d"), ret ));
    return ret;
    }
        
   
// ---------------------------------------------------------
// TInt RSCPClient::StoreCode( TSCPSecCode& aCode )
// Propagates the store code -request to the server along
// with the buffer parameter.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::StoreCode( TSCPSecCode& aCode )
    {
    Dprint( (_L("--> RSCPClient::StoreCode()") ));
        
    TInt ret = SendReceive(ESCPServSetCode, TIpcArgs( &aCode ) );
  
    Dprint( (_L("<-- RSCPClient::StoreCode(): %d"), ret ));
    return ret;
    }   



// ---------------------------------------------------------
// TInt RSCPClient::ChangeCode( TDes& aNewCode )
// Propagates the change code -request to the server, along with
// the code parameter.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::ChangeCode( TDes& aNewCode )
    {
    Dprint( (_L("--> RSCPClient::ChangeCode()") ));
        
    TInt ret = SendReceive(ESCPServChangeCode, TIpcArgs( &aNewCode ) );
  
    Dprint( (_L("<-- RSCPClient::ChangeCode(): %d"), ret ));  
    return ret;
    }   

// ---------------------------------------------------------
// TInt RSCPClient::SetPhoneLock()
// Propagates the lock/unlock phone -request to the server
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::SetPhoneLock( TBool aLocked )
    {
    Dprint( (_L("--> RSCPClient::SetPhoneLock( %d)"), aLocked ));
            
    TInt ret = SendReceive(ESCPServSetPhoneLock, TIpcArgs( aLocked ) );
  
    Dprint( (_L("<-- RSCPClient::SetPhoneLock(): %d"), ret ));
    return ret;
    }   

// ---------------------------------------------------------
// TBool RSCPClient::QueryAdminCmd( TSCPAdminCommand aCommand )
// Packs the command parameter into a buffer, and propagates
// the call to the server, the response is received in the
// same buffer.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TBool RSCPClient::QueryAdminCmd( TSCPAdminCommand aCommand )
    {
    Dprint( (_L("--> RSCPClient::QueryAdminCmd()") ));
        
    TInt status = 0;
  
    TPckg<TInt> retPackage(status);
    
    TInt ret = SendReceive( ESCPServQueryAdminCmd, TIpcArgs( aCommand, &retPackage ) );
   
    Dprint( (_L("<-- RSCPClient::QueryAdminCmd(): %d"), retPackage() ));
    return static_cast<TBool>( status );
    }   
        
// ---------------------------------------------------------
// TInt RSCPClient::GetLockState( TBool& aState )
// Package the parameter, and send it to the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::GetLockState( TBool& aState )
    {
    Dprint( (_L("--> RSCPClient::GetLockState()") )); 
      
    TPckg<TBool> retPackage(aState);
    
    TInt ret = SendReceive( ESCPServGetLockState, TIpcArgs( &retPackage ) );
     
    Dprint( (_L("<-- RSCPClient::GetLockState(): %d"), retPackage() ));
    return ret;            
    }
  
  
// ---------------------------------------------------------
// TInt RSCPClient::GetParamValue( TInt aParamID, TDes& aValue )
// The server contains all the logic for the parameters, just 
// propagate the call.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::GetParamValue( TInt aParamID, TDes& aValue )
    {
    Dprint( (_L("--> RSCPClient::GetParamValue()") ));  
    
    TInt ret = SendReceive( ESCPServGetParam, TIpcArgs( aParamID, &aValue ) );
     
    Dprint( (_L("<-- RSCPClient::GetParamValue(): %d"), ret));
    return ret;            
    }
    
// ---------------------------------------------------------
// TInt RSCPClient::SetParamValue( TInt aParamID, TDes& aValue )
// The server contains all the logic for the parameters, just 
// propagate the call.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::SetParamValue( TInt aParamID, TDes& aValue )
    {
    Dprint( (_L("--> RSCPClient::SetParamValue()") ));  
    
    TInt ret = SendReceive( ESCPServSetParam, TIpcArgs( aParamID, &aValue ) );
     
    Dprint( (_L("<-- RSCPClient::SetParamValue(): %d"), ret ));
    return ret;            
    }    
    

// *********** Device lock new features ************* -->>

// ---------------------------------------------------------
// RSCPClient::SecCodeQuery()
// Request the security code from the user and authenticate
// through the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::SecCodeQuery( RMobilePhone::TMobilePassword& aPassword, 
                                        TSCPButtonConfig aButtonsShown, 
                                        TBool aECSSupport,
                                        TInt aFlags )
    {
    TInt lErr = KErrNone;
    TInt lStatus = KErrNone;
    TInt lResFileSCP = NULL;
    TInt lResFileSecUi = NULL;
    Dprint( (_L("--> RSCPClient::SecCodeQuery(%d, %d"), aButtonsShown, aECSSupport ));  
    TRAP(lErr, lStatus = SetSecurityCodeL(aPassword, aButtonsShown, aECSSupport, aFlags, lResFileSCP, lResFileSecUi));    
   
    
    if(lResFileSCP) {
        CCoeEnv :: Static()->DeleteResourceFile(lResFileSCP);
	}
    
    if(lResFileSecUi) {
    
    
        CCoeEnv :: Static()->DeleteResourceFile(lResFileSecUi);
    }
    
    Dprint((_L("<-- RSCPClient::SecCodeQuery(): lStatus= %d, lErr= %d"), lStatus, lErr));
    return (lErr != KErrNone) ? lErr : lStatus;
}

// ---------------------------------------------------------
// RSCPClient::ChangeCodeRequest()
// Show the current code query dialog and continue in GetNew
// CodeAndChange.
// 
// Status : Approved
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient::ChangeCodeRequest()
    {
    Dprint((_L("[RSCPClient] ChangeCodeRequest() >>>")));
    
    if(EFalse == isFlagEnabled) {
        Dprint((_L("[RSCPClient]-> ChangeCodeRequest(): ERROR: Function not supported in this variant")));
        User :: Invariant();
        return KErrNotSupported;
	}

    TInt lRet(KErrNone);
    TInt lErr(KErrNone);

    TInt resourceFileSCP(NULL);
    TInt resourceFileSecUi(NULL);

    // Check if the code change is allowed
    {
        HBufC8* addParamsHBuf = NULL;
        TRAP(lErr, addParamsHBuf = HBufC8 :: NewL(KSCPMaxTARMNotifParamLen));
        
        if(lErr != KErrNone) {
            return lErr;
        }

        TPtr8 addParams = addParamsHBuf->Des();
        addParams.Zero();

        TInt status(KErrNone);
        TPckg<TInt> retPackage(status);
        
        TInt ret = SendReceive(ESCPServCodeChangeQuery, TIpcArgs(&retPackage, &addParams));

        if((ret == KErrNone) && (addParams.Length() > 0)) {
            // The server has sent additional parameters, ignore errors in processing
            TRAP_IGNORE(ProcessServerCommandsL(addParams));
        }

        delete addParamsHBuf;

        if((ret != KErrNone) || (status != KErrNone)) {
            // Password cannot be changed now
            return KErrAbort;
        }
    }

    // Load the required resource files into this process
    lRet = LoadResources(resourceFileSCP, resourceFileSecUi);
    
    if(lRet != KErrNone) {
        return lRet;
    }

    HBufC* codeHBuf = NULL;
    
    TRAP(lErr, codeHBuf = HBufC :: NewL(KSCPPasscodeMaxLength + 1));
    
    if(lErr != KErrNone) {
        // Remove the resource files
        CCoeEnv :: Static()->DeleteResourceFile(resourceFileSCP);
        CCoeEnv :: Static()->DeleteResourceFile(resourceFileSecUi);
        return lErr;
    }

    TPtr codeBuffer = codeHBuf->Des();
    codeBuffer.Zero();

    TInt def_code = -1;
    CRepository* lRepository = NULL;
    
    TRAP(lErr, lRepository = CRepository :: NewL(KCRUidSCPLockCode));
    
    if(KErrNone == lErr) {
        lErr = lRepository->Get(KSCPLockCodeDefaultLockCode, def_code);
       
        if(def_code == 0) {
            TRAP(lErr, lRet = RunDialogL(codeBuffer, SCP_OK_CANCEL, KSCPPasscodeMinLength,
                                     KSCPPasscodeMaxLength, R_SECUI_TEXT_ENTER_SEC_CODE));
    
            if((lRet) && (lErr == KErrNone) && (lRet != ESecUiEmergencyCall)) {
                lErr = GetNewCodeAndChange(codeBuffer, KSCPNormalChange);
            }
    
            if(lErr != KErrNone) {
                Dprint((_L("RSCPClient::ChangeCodeRequest(): Code change FAILED: %d"), lErr));
            }
    
            if(((!lRet) && (lErr == KErrNone)) || (lRet == ESecUiEmergencyCall)) {
                // Cancelled by user
                lErr = KErrAbort;
            }
        }
        else if(def_code != -1) {
            _LIT(KText, "12345");
            TBufC<10> NBuf (KText);
            TPtr codeBuf = NBuf.Des();
            
            lErr = GetNewCodeAndChange(codeBuf, KSCPNormalChange);
            
            if(lErr == KErrNone) {
                lRepository->Set(KSCPLockCodeDefaultLockCode, 0);
            }
            else {
                Dprint((_L("RSCPClient::ChangeCodeRequest(): Code change FAILED automatic: %d"), lErr));
            }
        }
        
        delete lRepository;
    }
    
    Dprint((_L("<-- RSCPClient::ChangeCodeRequest(): %d"), lErr ));
    
    // Remove the resource files
    CCoeEnv :: Static()->DeleteResourceFile(resourceFileSCP);
    CCoeEnv :: Static()->DeleteResourceFile(resourceFileSecUi);    
    delete codeHBuf;
    return lErr;
}

// ---------------------------------------------------------
// RSCPClient::CheckConfiguration()
// Ask the server if the configuration is OK
// 
// Status : Approved
// ---------------------------------------------------------
//

EXPORT_C TInt RSCPClient::CheckConfiguration( TInt aMode )
    {
    Dprint( (_L("--> RSCPClient::CheckConfiguration()") ));
    
    TInt status = 0;
  
    TPckg<TInt> retPackage(status);    
    
    TInt ret = SendReceive(ESCPServCheckConfig, TIpcArgs( aMode, &retPackage ) );
    
    if ( ret == KErrNone )
        {
        ret = status;
        
        if ( status == KErrNone )
            {
            Dprint( (_L("RSCPClient::CheckConfiguration(): Configuration OK") ));
            }
        else if ( status == KErrAccessDenied )
            {
            if ( aMode == KSCPInitial )
                {
                Dprint( (_L("RSCPClient::CheckConfiguration(): Initial check failed") ));
                }
            else
                {
                Dprint( (_L("RSCPClient::CheckConfiguration(): WARNING:\
                   Configuration Out of sync") ));
                }            
            }
        }                
  
    Dprint( (_L("<-- RSCPClient::CheckConfiguration(): %d"), status ));
    
    return ret;
    }
EXPORT_C TInt RSCPClient :: PerformCleanupL(RArray<TUid>& aAppIDs) {
    TInt lCount = aAppIDs.Count();
    
    if(lCount < 1) {
        return KErrNone; 
    }
        
    HBufC8* lBuff = HBufC8 :: NewLC(lCount * sizeof(TInt32));
    TPtr8 lBufPtr = lBuff->Des();
    RDesWriteStream lWriteStream(lBufPtr);
    CleanupClosePushL(lWriteStream);
    
    for(TInt i=0; i < lCount; i++) {
        Dprint((_L("[RSCPClient]-> Marking %d for cleanup"), aAppIDs[i].iUid ));
        lWriteStream.WriteInt32L(aAppIDs[i].iUid);
    }
    lWriteStream.CommitL();
    TInt lStatus = SendReceive(ESCPApplicationUninstalled, TIpcArgs(ESCPApplicationUninstalled, &lBuff->Des()));
    CleanupStack :: PopAndDestroy(2); // lBuff, lWriteStream
    return lStatus;
}
// ---------------------------------------------------------
// The server contains all the logic for the parameters, just
// propagate the call.
//
// ---------------------------------------------------------
//
EXPORT_C TInt RSCPClient :: SetParamValue(TInt aParamID, TDes& aValue, TUint32 aCallerSID) {
    Dprint((_L("RSCPClient::SetParamValue() >>>")));
    TPckgBuf<TUint32> lCallerID(aCallerSID);
    TInt ret = SendReceive(ESCPServSetParam, TIpcArgs(aParamID, &aValue, &lCallerID));
    Dprint((_L("RSCPClient::SetParamValue(): %d <<<"), ret));
    return ret;
}

/* ---------------------------------------------------------
 * Alternative function that can be used to set the Auto Lock period
 * Caller should have AllFiles access level
 * Primarily called from the general settings components
// ---------------------------------------------------------
*/
EXPORT_C TInt RSCPClient :: SetAutoLockPeriod( TInt aValue ) {
    Dprint((_L("[RSCPClient]-> SetAutoLockPeriod() >>>")));
    TPckgBuf<TInt> lAutoLockPeriod(aValue);
    TInt ret = SendReceive(ESCPServUISetAutoLock, TIpcArgs(&lAutoLockPeriod));
    Dprint((_L("[RSCPClient]-> SetAutoLockPeriod(): %d <<<"), ret));
    return ret;
}
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

// ---------------------------------------------------------
// RSCPClient::GetNewCodeAndChange()
// Show the new code request and verification dialogs and
// send the change request to the server.
// 
// Status : Approved
// ---------------------------------------------------------
//  
TInt RSCPClient::GetNewCodeAndChange( TDes& aOldCode, 
                                      TInt aMode,
                                      TSCPSecCode* aNewDOSCode /*=NULL*/, 
                                      HBufC** aNewCodePptr/* = NULL*/)
    {
    
    
    if(!isFlagEnabled)
	{
		return KErrNotSupported;
	}
    TInt err = KErrNone;
    TInt ret = KErrNone;            
    
            
    TInt maxLen = KSCPPasscodeMaxLength;
    TInt minLen = 1;
        
    // Fetch the minimum and maximum lengths for the code, if available.
    // This feature is commented out for correction to BU error ID: BNIN-6LC3AP.
    // Left in the code for possible inclusion in the future.
    //FetchLimits( minLen, maxLen );        
        
    // Request the new code and verify it. Repeat this until the codes match.
    
    // Create the buffers on the heap
    HBufC* newCodeHBuf = NULL;
    HBufC* verifyCodeHBuf = NULL;
    
    TRAP( err, newCodeHBuf = HBufC::NewL( KSCPPasscodeMaxLength + 1 ) );
    if ( err == KErrNone )
        {
        TRAP( err, verifyCodeHBuf = HBufC::NewL( KSCPPasscodeMaxLength + 1 ) );
        }
    
    if ( err != KErrNone )
        {
        if ( newCodeHBuf != NULL )
            {
            delete newCodeHBuf;
            }
        return err; 
        }    
        
    TPtr newCodeBuffer = newCodeHBuf->Des();
    TPtr verifyCodeBuffer = verifyCodeHBuf->Des();  
    
    // Configure the buttons according to the mode
    TSCPButtonConfig bConfig;
    TBool ecSupport;
    if ( aMode == KSCPForcedChange )
        {
        bConfig = SCP_OK_ETEL;
        ecSupport = ETrue;
        }
    else
        {
        bConfig = SCP_OK_CANCEL;
        ecSupport = EFalse;
        }
    
    TBool isMismatch = EFalse;
    do // Repeat loop BEGIN
        {
        isMismatch = EFalse;
                
        if ( err == KErrSCPInvalidCode )
            {
            err = KErrNone; // "reset"
            }
                          
        newCodeBuffer.Zero(); 
        
        TRAP( err, ret = RunDialogL( newCodeBuffer, 
                                 bConfig, 
                                 minLen,
                                 maxLen,
                                 R_SECUI_TEXT_ENTER_NEW_SEC_CODE,
                                 NULL,
                                 ecSupport ) );
    
        if ( ( ret ) && ( ret != ESecUiEmergencyCall ) && ( err == KErrNone ) )
            {        
            verifyCodeBuffer.Zero();
            
            TRAP( err, ret = RunDialogL( verifyCodeBuffer, 
                                 bConfig, 
                                 minLen,
                                 maxLen,
                                 R_SECUI_TEXT_VERIFY_NEW_SEC_CODE,
                                 NULL,
                                 ecSupport ) );                             
            }

        if ( ( !ret ) || ( err != KErrNone ) || ( ret == ESecUiEmergencyCall ) )
            {
            break;
            }            
            
        if  ( verifyCodeBuffer.Compare( newCodeBuffer ) != 0 )
            {                        
            // Ignore the errors from showing the note, it's better to continue if it fails
            
            TRAP_IGNORE(
                // Show an error note, the entered codes don't match             
                CAknNoteDialog* noteDlg = 
                    new (ELeave) CAknNoteDialog(reinterpret_cast<CEikDialog**>( &noteDlg ) );
                noteDlg->SetTimeout( CAknNoteDialog::ELongTimeout );
                noteDlg->SetTone( CAknNoteDialog::EErrorTone );
                noteDlg->ExecuteLD( R_CODES_DONT_MATCH );
            );
            
            isMismatch = ETrue; // Repeat code query
            }        
        
        if ( !isMismatch )
            {
            HBufC8* addParamsHBuf = NULL;
    
            TRAP( err, addParamsHBuf = HBufC8::NewL( KSCPMaxTARMNotifParamLen ) );
            if ( err != KErrNone )
                {
                delete verifyCodeHBuf;
                delete newCodeHBuf;             
                return err;
                }      
        
            TPtr8 addParams = addParamsHBuf->Des();
            addParams.Zero();                                    
            
            if ( !isMismatch )
                {
                // Try to change the code
                TSCPSecCode newDOSCode;
                newDOSCode.Zero();
                
                err = SendReceive(  ESCPServChangeEnhCode, 
                            TIpcArgs( &aOldCode, &verifyCodeBuffer, &addParams, &newDOSCode ) 
                         );
                
                if ( addParams.Length() > 0 )
                    {
                    // The server has sent additional parameters
                    TRAPD( err, ProcessServerCommandsL( addParams ) );
                    if ( err != KErrNone )
                        {
                        Dprint( (_L("RSCPClient::GetNewCodeAndChange():\
                            Process cmds FAILED: %d"), err ));
                        }                    
                    }
                    
                if ( aNewDOSCode != NULL )
                    {
                    (*aNewDOSCode).Copy( newDOSCode );
                    }
                }
            
            delete addParamsHBuf;
            }
                            
        } while ( ( isMismatch ) || ( err == KErrSCPInvalidCode ) ); // Loop END
        
    if ( ( ( !ret ) && ( err == KErrNone ) ) || ( ret == ESecUiEmergencyCall ) )
        {
        // Cancelled by user
        err = KErrAbort;
        }

    if((KErrNone == err) && (aNewCodePptr != 0)) {
        Dprint(_L("[RSCPClient]-> INFO: Updating new lock code to aNewCodePptr"));
        TRAP(err, *aNewCodePptr = HBufC :: NewL(verifyCodeHBuf->Des().Length()));

        if(*aNewCodePptr != NULL) {
            (*aNewCodePptr)->Des().Copy(verifyCodeHBuf->Des());
            Dprint(_L("[RSCPClient]-> INFO: Updated new lock code to aNewCodePptr"));
        }
    }
    
    delete verifyCodeHBuf;
    delete newCodeHBuf;
        
    return err;        
    }
       
            
        
// ---------------------------------------------------------
// RSCPClient::ProcessServerCommandsL()
// Handle the commands in the server's param-buffer
// 
// Status : Approved
// ---------------------------------------------------------
//
void RSCPClient::ProcessServerCommandsL( TDes8& aInParams, 
                                         CSCPParamObject** aOutParams,
                                         TBool isNotifierEvent )
    {    

    if(!isFlagEnabled)
	{
		User::Leave(KErrNotSupported);
	}
    Dprint( (_L("--> RSCPClient::ProcessServerCommandsL()") ));
    (void)aOutParams;
    
    CSCPParamObject* theParams = CSCPParamObject::NewL();
    CleanupStack::PushL( theParams );
    
    theParams->Parse( aInParams );
    TInt actionID;
    TInt ret = theParams->Get( KSCPParamAction, actionID );
    
    Dprint( (_L("RSCPClient::ProcessServerCommandsL():Params parsed") ));
    
    if ( ret != KErrNone )
        {
        Dprint( (_L("RSCPClient::ProcessServerCommands(): Can't get action ID: %d"), ret )); 
        }
    else
        {
        switch ( actionID )
            {
            case ( KSCPActionShowUI ):
                {
                TRAP( ret, ShowUIL( *theParams ) );
                break;
                }
                
            case ( KSCPActionForceChange ):
                {
                if ( isNotifierEvent )
                    {
                    break; // Calling this through the notifier would jam the system
                    // since the server is busy.
                    }                                    
                
                HBufC* codeHBuf = NULL;
                codeHBuf = HBufC::NewL( KSCPPasscodeMaxLength + 1 );    
                CleanupStack::PushL( codeHBuf );
                   
                TPtr codeBuf = codeHBuf->Des();
                codeBuf.Zero(); 
                
                ret = theParams->Get( KSCPParamPassword, codeBuf );
                if ( ret == KErrNone )
                    {
                    TSCPSecCode newDOSCode;
                    ret = GetNewCodeAndChange( codeBuf, KSCPForcedChange, &newDOSCode );
                    
                    // If aOutParams is defined, return the new code
                    if ( aOutParams != NULL )
                        {
                        (*aOutParams) = CSCPParamObject::NewL();
                        (*aOutParams)->Set( KSCPParamPassword, newDOSCode );
                        }
                    }
                    
                CleanupStack::PopAndDestroy( codeHBuf );
                                    
                break;
                }                            
            }                
        }
                       
    CleanupStack::PopAndDestroy( theParams );
    
    Dprint( (_L("<-- RSCPClient::ProcessServerCommandsL()") ));
    User::LeaveIfError( ret );
    }
    
// ---------------------------------------------------------
// RSCPClient::ShowUI()
// Show the requested UI through AVKON
// 
// Status : Approved
// ---------------------------------------------------------
//
void RSCPClient::ShowUIL( CSCPParamObject& aContext )
    {
    
    if(!isFlagEnabled)
	{
		User::Leave(KErrNotSupported);
	}
    Dprint( (_L("--> RSCPClient::ShowUIL()") ));
    TInt mode;
    User::LeaveIfError( aContext.Get( KSCPParamUIMode, mode ) );
    
    switch ( mode )
        {
        case ( KSCPUINote ):
            {
            // Get prompt
            TBuf<KSCPMaxPromptTextLen> promptText;
            aContext.Get( KSCPParamPromptText, promptText );
                        
            // Try to get note icon, default is to use "error"
            TInt noteType = KSCPUINoteError;
            Dprint( (_L("RSCPClient::ShowUIL(): Creating note object") ));
            
            CAknResourceNoteDialog* note = NULL;
            
            if ( aContext.Get( KSCPParamNoteIcon, noteType ) != KErrNone )
                {
                noteType = KSCPUINoteError;
                }
                
            switch ( noteType )
                {
                case ( KSCPUINoteWarning ):
                    {
                    note = new (ELeave) CAknWarningNote( ETrue );    
                    break;
                    }
                    
                case ( KSCPUINoteError ):
                default: // default to error
                    {
                    note = new (ELeave) CAknErrorNote( ETrue ); 
                    break;
                    }                         
                }                                          

            if ( note != NULL )
                {
                Dprint( (_L("RSCPClient::ShowUIL(): Showing note") ));
                note->ExecuteLD( promptText );
                }            
            }
        }

    Dprint( (_L("<-- RSCPClient::ShowUIL()") ));
    } 
       

// ---------------------------------------------------------
// RSCPClient::FetchLimits()
// Retrieve the limit-parameter values if available
// 
// Status : Approved
// ---------------------------------------------------------
//
void RSCPClient::FetchLimits( TInt& aMin, TInt& aMax )
    {        
    if(!isFlagEnabled)
	{
		return;
	}      
    TInt maxLenID = RTerminalControl3rdPartySession::EPasscodeMaxLength;
    TInt minLenID = RTerminalControl3rdPartySession::EPasscodeMinLength;
    TBuf<KSCPMaxIntLength> intBuf;
   
    intBuf.Zero();    
    if ( GetParamValue( minLenID, intBuf ) != KErrNone )
        {
        aMin = KSCPPasscodeMinLength;
        }
    else
        {
        TLex lex( intBuf );
        if ( ( lex.Val( aMin ) != KErrNone ) || ( aMin <= 0 ) )
            {
            aMin = KSCPPasscodeMinLength;
            }
        }                
    
    intBuf.Zero();
    if ( GetParamValue( maxLenID, intBuf ) != KErrNone )
        {
        aMax = KSCPPasscodeMaxLength;
        }
    else
        {
        TLex lex( intBuf );
        if ( ( lex.Val( aMax ) != KErrNone ) || ( aMax <= 0 ) )
            {
            aMax = KSCPPasscodeMaxLength;
            }
        }
    }              
TInt RSCPClient :: SetSecurityCodeL(RMobilePhone :: TMobilePassword& aPassword, 
            TSCPButtonConfig aButtonsShown, TBool aECSSupport, TInt aFlags, TInt& aResFileSCP, TInt& aResFileSecUi) {
    Dprint((_L("[RSCPClient]-> SetSecurityCodeL() >>>")));
    Dprint((_L("[RSCPClient]-> input params - aButtonsShown=%d, aECSSupport=%d"), aButtonsShown, aECSSupport));

    if(EFalse == isFlagEnabled) {
        (void)aPassword;
        Dprint((_L("[RSCPClient]-> ERROR: Function not supported in this variant")));
        User :: Invariant();
        return KErrNotSupported;
    }

    TInt lRet = LoadResources(aResFileSCP, aResFileSecUi);

    if(lRet != KErrNone) {
        return lRet;
    }

    TInt lDefCode = 0;
    CRepository* lRepository = CRepository :: NewLC(KCRUidSCPLockCode);
    lRet = lRepository->Get(KSCPLockCodeDefaultLockCode, lDefCode);

    if(lRet != KErrNone) {
        Dprint(_L("[RSCPClient]-> ERROR: Unable to perform get on CenRep, lErr=%d"), lRet);
        CleanupStack :: PopAndDestroy(lRepository);
        return lRet;
    }

    HBufC* codeHBuf = HBufC :: NewLC(KSCPPasscodeMaxLength + 1);
    HBufC8* addParamsHBuf = HBufC8 :: NewLC(KSCPMaxTARMNotifParamLen);
    TPtr codeBuffer = codeHBuf->Des();
    TPtr8 addParams = addParamsHBuf->Des();

    if(lDefCode == 0) {
        Dprint(_L("[RSCPClient]-> INFO: Default lock code has been set already by the user..."));

        lRet = RunDialogL(codeBuffer, aButtonsShown, KSCPPasscodeMinLength, KSCPPasscodeMaxLength,
                R_SECUI_TEXT_ENTER_SEC_CODE, NULL, aECSSupport);

        if((lRet) && (lRet != ESecUiEmergencyCall) && (lRet != EAknSoftkeyEmergencyCall)) {
            Dprint(_L("[RSCPClient]-> INFO: User has updated the lock code..."));

            lRet = SendReceive( ESCPServAuthenticateS60, TIpcArgs( &codeBuffer, &aPassword, &addParams, aFlags));

            Dprint((_L("[RSCPClient]-> INFO: addParams.Length()=%d")), addParams.Length());
            Dprint((_L("[RSCPClient]-> INFO: lRet=%d")), lRet);
        }
        else {
            switch(lRet) {
            case 0:
				lRet = KErrCancel;
				break;
            case EAknSoftkeyEmergencyCall:
                //lRet = KErrCancel;
                break;
            case ESecUiEmergencyCall:
                lRet = ESecUiEmergencyCall;
                break;
            /*default:
                break;*/
            }
        }
    }
    else {
        TRequestStatus statusSave;
        Dprint(_L("[RSCPClient]-> INFO: Default lock code not set by the user, requesting for the same"));

        HBufC* msgConfirmSave = NULL;
        CAknGlobalConfirmationQuery* query = CAknGlobalConfirmationQuery :: NewLC();

        if(aButtonsShown == SCP_OK || aButtonsShown == SCP_OK_ETEL) {
            //msgConfirmSave = CEikonEnv :: Static()->AllocReadResourceLC(R_SET_SEC_CODE);
             msgConfirmSave = CEikonEnv :: Static()->AllocReadResourceLC(R_SET_SEC_CODE_SETTING_QUERY_SERVER);
            query->ShowConfirmationQueryL(statusSave, *msgConfirmSave, R_AVKON_SOFTKEYS_OK_EMPTY__OK, R_QGN_NOTE_INFO_ANIM);
        }
        else {
            msgConfirmSave = CEikonEnv :: Static()->AllocReadResourceLC(R_SET_SEC_CODE);
        	query->ShowConfirmationQueryL(statusSave, *msgConfirmSave, R_AVKON_SOFTKEYS_YES_NO__YES, R_QGN_NOTE_QUERY_ANIM);
        }

        User :: WaitForRequest(statusSave);
        CleanupStack :: PopAndDestroy(2); // msgConfirmSave query

        if((statusSave == EAknSoftkeyYes) || (statusSave == EAknSoftkeyOk)) {
            Dprint(_L("[RSCPClient]-> INFO: calling GetNewCodeAndChange() ..."));

            TBufC<10> NBuf(KSCPDefaultEnchSecCode);
            TPtr codeBuf = NBuf.Des();

            TSCPSecCode lNewSecCode;
            TInt lButtonCfg = (aButtonsShown == SCP_OK || aButtonsShown == SCP_OK_ETEL) ? KSCPForcedChange : KSCPNormalChange;
            HBufC* lNewLkCodeBuf = NULL;
            lRet = GetNewCodeAndChange(codeBuf, lButtonCfg, &lNewSecCode, &lNewLkCodeBuf);

            Dprint(_L("[RSCPClient]-> INFO: GetNewCodeAndChange() complete, err=%d"), lRet);

            if(KErrNone == lRet) {
                /* This is being called as a workaround for a freezing issue with SecUI. This is in place
                 * as a temporary measure until the source is identified.
                */
                TInt lTmpRet = SendReceive(ESCPServAuthenticateS60, TIpcArgs(&lNewLkCodeBuf->Des(), &aPassword, &addParams, aFlags));

                Dprint(_L("[RSCPClient]-> INFO: lTmpRet from SendReceive()=%d"), lTmpRet);
                if(KErrNone == lRet) {
                    Dprint(_L("[RSCPClient]-> INFO: updating CenRep ..."));
                    lRepository->Set(KSCPLockCodeDefaultLockCode, 0);
                    Dprint(_L("[RSCPClient]-> INFO: User updated lock code for the first time...err= %d"), lRet);
                }
            }

            if(lNewLkCodeBuf) {
                delete lNewLkCodeBuf;
            }
        }
        else {
            Dprint(_L("[RSCPClient]-> INFO: Returning KErrCancel"));
            lRet = KErrCancel;
        }

        if(KErrAbort == lRet) {
            Dprint(_L("[RSCPClient]-> INFO: Returning KErrCancel"));
            lRet = KErrCancel;
        }

    }

    if(addParams.Length() > 0) {
        CSCPParamObject* tmp = CSCPParamObject :: NewLC();
        TInt lTempRet = tmp->Parse(addParams);

        if(lTempRet == KErrNone) {
            lTempRet = tmp->Set(KSCPParamPassword, codeBuffer);
        }

        if(lTempRet == KErrNone) {
            addParams.Zero();
            HBufC8* tmpBuf;
            lTempRet = tmp->GetBuffer(tmpBuf);

            if(lTempRet == KErrNone) {
                addParams.Copy(tmpBuf->Des());
                delete tmpBuf;
            }
        }

        if(lTempRet == KErrNone) {
            CSCPParamObject* outParams = NULL;
            ProcessServerCommandsL(addParams, &outParams);

            if(outParams != NULL) {
                TSCPSecCode newSecCode;
                if(outParams->Get(KSCPParamPassword, newSecCode) == KErrNone) {
                    Dprint((_L("[RSCPClient]-> INFO: Updating encoded password received from the server into aPassword...")));
                    aPassword.Copy(newSecCode);
                }

                delete outParams;
            }
        }

        CleanupStack :: PopAndDestroy(tmp);
    }

    CleanupStack :: PopAndDestroy(3); // repository, addParamsHBuf, codeHBuf
    Dprint(_L("[RSCPClient]-> SetSecurityCodeL() <<< lRet=%d"), lRet);
    return lRet;
}

//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
// <<-- *********** Device lock new features *************


// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  


