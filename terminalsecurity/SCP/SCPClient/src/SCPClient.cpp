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
#include "SCPParamObject.h"

#include <scpnotifier.rsg>
#include "SCP_IDs.h"

#include <centralrepository.h>
#include "SCPLockCode.h"
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
#include <TerminalControl3rdPartyAPI.h>
#include <SCPServerInterface.h>
#include <secui.hrh>
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

EXPORT_C TInt RSCPClient::GetPolicies(RArray<TInt>& aDeviceLockPolicies) {
    Dprint(_L("[RSCPClient]-> GetPolicies >>>"));
    HBufC8* lBuff = NULL;
    TInt lStatus = KErrNone;

    TRAP(lStatus, lBuff = HBufC8 :: NewL((EDevicelockTotalPolicies - 1)  * sizeof(TInt)));

    if (lStatus == KErrNone) {

        lStatus = SendReceive(ESCPServGetParam, TIpcArgs(-1, &lBuff->Des()));

        if (lStatus == KErrNone) {
            // Copy data from lBuff to aDeviceLockPolicies
            TPtr8 bufPtr = lBuff->Des();

            if (bufPtr.Length() > 0) {
                RDesReadStream lBufReadStream(bufPtr);
                Dprint(_L("[RSCPClient]-> Get from server complete, returning service request..."));

                for (TInt i = 0; i < 17; i++) {
                    TInt32 lParamValue = 0;
                    TRAP(lStatus, lParamValue = lBufReadStream.ReadInt32L());

                    if (lStatus != KErrNone) {
                        break;
                    }

                    aDeviceLockPolicies.Append(lParamValue);
                }

                lBufReadStream.Close();
            }
            else {
                lStatus = KErrGeneral;
            }
        }
    }
    delete lBuff;
    Dprint(_L("[RSCPClient]-> GetPolicies <<<"));
    return lStatus;
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

EXPORT_C TBool RSCPClient :: IsLockcodeChangeAllowedNow(RArray<TDevicelockPolicies>& aFailedPolicies) {
    Dprint((_L("[RSCPClient]-> IsLockcodeChangeAllowedNow() >>>")));
    TInt lStatus = KErrNone;
	TInt lErr = KErrNone;
    
    // extra one for failed policies count
    //koys: if leave happens what errorcode we should return??
    HBufC8* failedPoliciesBuff = NULL;
    TRAP(lStatus, failedPoliciesBuff = HBufC8 :: NewL((EDevicelockTotalPolicies + 1)* sizeof(TInt32)));
    
	if (lStatus == KErrNone) {
		lStatus = SendReceive(ESCPServCodeChangeQuery, TIpcArgs(&failedPoliciesBuff->Des()));
		//koya: if leave happens what errorcode we should return??
		TPtr8 failedPoliciesBufPtr = failedPoliciesBuff->Des();
		TRAP(lErr, ReadFailedPoliciesL(failedPoliciesBufPtr, aFailedPolicies));
		delete failedPoliciesBuff;
	}
	
	Dprint((_L("[RSCPClient]-> IsLockcodeChangeAllowedNow() <<<")));
    return (lStatus != KErrNone) ? lStatus : ((lErr != KErrNone) ? lErr : KErrNone);
}

EXPORT_C  TInt RSCPClient :: VerifyNewLockcodeAgainstPolicies(TDesC& aLockcode, RArray<TDevicelockPolicies>& aFailedPolicies) {
    Dprint((_L("[RSCPClient]-> VerifyNewLockcodeAgainstPolicies() >>>")));
    TInt lRet = KErrNone;
    TInt lErr = KErrNone;
    // extra one for failed policies count
    HBufC8* failedPoliciesBuff = NULL;

    TRAP(lRet, failedPoliciesBuff = HBufC8 :: NewL((EDevicelockTotalPolicies + 1) * sizeof(TInt32)));

    if(lRet == KErrNone) {
        lRet = SendReceive(ESCPServValidateLockcode, TIpcArgs(&aLockcode, &failedPoliciesBuff->Des()));

        TPtr8 failedPoliciesBufPtr = failedPoliciesBuff->Des();
        TRAP(lErr, ReadFailedPoliciesL(failedPoliciesBufPtr, aFailedPolicies));

        delete failedPoliciesBuff;
    }

    Dprint((_L("[RSCPClient]-> VerifyNewLockcodeAgainstPolicies() <<<")));
    return (lRet != KErrNone) ? lRet : ((lErr != KErrNone) ? lErr : KErrNone);
}

EXPORT_C  TInt RSCPClient :: StoreLockcode (TDesC& aNewLockcode, TDesC& aOldLockcode, RArray<TDevicelockPolicies>& aFailedPolicies) {
    Dprint((_L("[RSCPClient]-> StoreLockcode() >>>")));
    TInt lErr = KErrNone;
    TInt lRet = KErrNone;

    if (!IsLockcodeChangeAllowedNow(aFailedPolicies)) {
        return KErrAccessDenied;
    }

    HBufC8* failedPoliciesBuff = NULL;

    TRAP(lRet, failedPoliciesBuff = HBufC8 :: NewL((EDevicelockTotalPolicies + 1)* sizeof(TInt32)));

    if(lRet == KErrNone) {
        lRet = SendReceive(ESCPServChangeEnhCode, TIpcArgs(&aOldLockcode, &aNewLockcode, &failedPoliciesBuff->Des()));

        TPtr8 failedPoliciesBufPtr = failedPoliciesBuff->Des();
        TRAP(lErr, ReadFailedPoliciesL(failedPoliciesBufPtr, aFailedPolicies));

        delete failedPoliciesBuff;
    }

    Dprint((_L("[RSCPClient]-> StoreLockcode() <<<")));
    return (lRet != KErrNone) ? lRet : ((lErr != KErrNone) ? lErr : KErrNone);
}

EXPORT_C  TInt RSCPClient :: VerifyCurrentLockcode (TDesC& aLockcode,RMobilePhone::TMobilePassword& aISACode,RArray< TDevicelockPolicies > &aFailedPolicies, TInt aFlags) {
    Dprint((_L("[RSCPClient]-> VerifyCurrentLockcode() >>>")));
    TInt lErr = KErrNone;
    TInt lRet = KErrNone;

    // extra one for failed policies count
    HBufC8* failedPoliciesBuff = NULL;

    TRAP(lRet, failedPoliciesBuff = HBufC8 :: NewL((EDevicelockTotalPolicies + 1)* sizeof(TInt32)));

    if(lRet == KErrNone) {
        lRet = SendReceive(ESCPServAuthenticateS60, TIpcArgs(&aLockcode, &aISACode, &failedPoliciesBuff->Des(), aFlags));

        TPtr8 failedPoliciesBufPtr = failedPoliciesBuff->Des();
        TRAP(lRet, ReadFailedPoliciesL(failedPoliciesBufPtr, aFailedPolicies));

        delete failedPoliciesBuff;
    }

    return (lRet != KErrNone) ? lRet : ((lErr != KErrNone) ? lErr : KErrNone);
}

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS


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

void RSCPClient :: ReadFailedPoliciesL(TDes8& aFailedPolicyBuf, RArray< TDevicelockPolicies>& aFailedPolicies) {
    Dprint((_L("[RSCPClient]-> ReadFailedPoliciesL() >>>")));
    
    if(aFailedPolicyBuf.Length() < 1) {
        return;
    }
    
    RDesReadStream readStream(aFailedPolicyBuf);
    CleanupClosePushL(readStream);
    
    TInt failedPoliciesCount = readStream.ReadInt32L();    
    aFailedPolicies.Reset();
    Dprint((_L("[RSCPClient]-> ReadFailedPoliciesL failedPoliciesCount =%d"), failedPoliciesCount));
    for(int i=0; i < failedPoliciesCount; i++) {
        TInt32 temp =  readStream.ReadInt32L();
        //aFailedPolicies.Append((TDevicelockPolicies) readStream.ReadInt32L());
        aFailedPolicies.AppendL((TDevicelockPolicies)temp);
        Dprint((_L("[RSCPClient]-> ReadFailedPoliciesL failed policy =%d"), temp));
    }

    CleanupStack :: PopAndDestroy(&readStream);
    Dprint((_L("[RSCPClient]-> ReadFailedPoliciesL() <<<")));
}
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
// <<-- *********** Device lock new features *************


// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  


