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
#include <utf.h>
#include <e32svr.h>
#include <e32math.h>
#include <gsmerror.h>

#include "SCPServer.h"
#include "SCPSession.h"

#ifdef SCP_USE_POLICY_ENGINE
#include <PolicyEngineClient.h>
#include <PolicyEngineXACML.h>
#include <RequestContext.h>
#include "DMUtilObserver.h"
#include "SCPParamDBController.h"
#endif // SCP_USE_POLICY_ENGINE

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
#include "SCPPluginManager.h"
#include "SCPPluginEventHandler.h"
#include <SCPParamObject.h>
#include "SCP_IDs.h"
// For Central Repository
#include <centralrepository.h>
#include "SCPCodePrivateCRKeys.h"
#include "SCPLockCode.h"
#include <TerminalControl3rdPartyAPI.h>
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS

#include <mmtsy_names.h>

#include "SCPDebug.h"
#include <featmgr.h>
// For Device encryption
#include <DevEncEngineConstants.h>
#include <DevEncSessionBase.h>
#include <startupdomainpskeys.h>

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// PanicServer Panics the server thread
// 
// Status : Approved
// ---------------------------------------------------------
//
GLDEF_C void PanicServer(TSCPServPanic aPanic)
	{
	_LIT( KTxtServerPanic,"SCP Server panic" );
	User::Panic( KTxtServerPanic, aPanic );
	}

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSession2* CSCPServer::CSCPServer()
// C++ default constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPServer::CSCPServer(TInt aPriority)
	: CPolicyServer( aPriority, CSCPServerPolicy ),
	  iConfiguration( &iRfs ),
	  iALPeriodRep( NULL ),
	  iShutdownTimer( NULL ),
	  iOverrideForCleanup(EFalse) {
	Dprint( (_L("--> CSCPServer::CSCPServer()") ));
	
	Dprint( (_L("<-- CSCPServer::CSCPServer()") ));
	}



// ---------------------------------------------------------
// CSession2* CSCPServer::ConstructL()
// Symbian 2nd phase constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPServer::ConstructL()
    {
    Dprint(_L("[CSCPServer]-> ConstructL() >>>"));

    TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
	{
		User::Leave(errf);
	}
	
    StartL( KSCPServerName );
    
    User::LeaveIfError( iRfs.Connect() );
       
    iConfiguration.Initialize();
    
    // Assign default config flag
    iConfiguration.iConfigFlag = KSCPConfigUnknown;
    
    
    // Assign the default codes
    iConfiguration.iSecCode.Zero();
    iConfiguration.iSecCode.Append( KSCPDefaultSecCode );

    // Assign the default codes
    iConfiguration.iCryptoCode.Zero();
    iConfiguration.iCryptoCode.Append( KSCPDefaultSecCode );

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS 
if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
{
    // Reset code block parameters
    iConfiguration.iFailedAttempts = 0;
    iConfiguration.iBlockedAtTime = KSCPNotBlocked;
    
    iPluginEventHandler = CSCPPluginEventHandler::NewL( &iRfs );
    
    iPluginManager = CSCPPluginManager::NewL( this );           
        
    // Hash the default code
    TBuf<KSCPMaxHashLength> hashBuf;
    hashBuf.Zero();
    
    HBufC* codeHBuf = HBufC::NewLC( KSCPPasscodeMaxLength + 1 );
    TPtr codeBuf = codeHBuf->Des();
    codeBuf.Zero();
    
    codeBuf.Copy( KSCPDefaultEnchSecCode );
    iPluginEventHandler->HashInput( codeBuf, hashBuf );
    
    iConfiguration.iEnhSecCode.Zero();
    iConfiguration.iEnhSecCode.Append( hashBuf );
    
    CleanupStack::PopAndDestroy( codeHBuf );
}
//#endif
   
    // Assign the default max timeout
    iConfiguration.iMaxTimeout = KSCPDefaultMaxTO;
   	iConfiguration.iBlockedInOOS = 0;
   
    // Read the configuration, overwriting the default values
    TInt ret = KErrNone;
    TRAPD( err, ret = iConfiguration.ReadSetupL() );
    if ( ( err != KErrNone ) || ( ret != KErrNone ) )
        {
        Dprint( (_L("CSCPServer::ConstructL(): ERROR reading the \
            configuration file: %d"), err ));        
        }         
    else
        {
        Dprint( (_L("CSCPServer::ConstructL(): Configration read OK") ));
        }
    
    
   

    Dprint( (_L("CSCPServer::ConstructL(): Connecting to CenRep") ));
    iALPeriodRep = CRepository::NewL( KCRUidSecuritySettings );        
    
    // Check for factory settings
    CheckIfRfsPerformedL();
    Dprint(_L("[CSCPServer]-> ConstructL() <<< "));
    }




// ---------------------------------------------------------
// CSession2* CSCPServer::NewL()
// Static constructor.
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPServer* CSCPServer::NewL()
    {
    Dprint( (_L("--> CSCPServer::NewL()") ));

    CSCPServer *pS = new (ELeave) CSCPServer( EPriority );    

    CleanupStack::PushL( pS );
    pS->ConstructL();
    CleanupStack::Pop( pS );
	
    Dprint( (_L("<-- CSCPServer::NewL()") ));
    return pS;
    }
	
   
   
// ---------------------------------------------------------
// CSession2* CSCPServer::~CSCPServer ()
// Destructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPServer::~CSCPServer()
	{
	Dprint( (_L("--> CSCPServer::~CSCPServer()") ));

    TRAPD( err, iConfiguration.WriteSetupL() );
    if ( err != KErrNone )
        {
        Dprint( (_L("CSCPServer::~CSCPServer(): ERROR writing the \
            configuration file: %d"), err ));
        }
                           
    delete iALPeriodRep;
    iALPeriodRep = NULL;
    
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
{
	
    delete iPluginManager;
    iPluginManager = NULL;
    
    delete iPluginEventHandler;
    iPluginEventHandler = NULL;
}
FeatureManager::UnInitializeLib();
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS 

	if ( iPhone.SubSessionHandle() )
	    {    
	    iPhone.Close();
	    }
	    	    
    if ( iTelServ.Handle() )
        {        
        iTelServ.Close();                        
        }
        
    if ( iRfs.Handle() )
        {
        iRfs.Close();
        }    
                          
	Dprint( (_L("<-- CSCPServer::~CSCPServer()") ));
	}



// ---------------------------------------------------------
// CSession2* CSCPServer::NewSessionL (const TVersion &aVersion, const RMessage2& aMsg )
// Checks the version and creates a new session if it matches the server version.
// 
// Status : Approved
// ---------------------------------------------------------
//
CSession2* CSCPServer::NewSessionL (const TVersion &aVersion, 
                                    const RMessage2& aMsg ) const
	{
	Dprint( (_L("--> CSCPServer::NewSessionL()") ));
	(void)aMsg;
			
	// Check we're the right version
	TVersion v( KSCPServMajorVersionNumber, KSCPServMinorVersionNumber, 
	            KSCPServBuildVersionNumber );

	if ( !User::QueryVersionSupported(v, aVersion) ) 
	    User::Leave( KErrNotSupported );	

	Dprint( (_L("<-- CSCPServer::NewSessionL()") ));
	
	return CSCPSession::NewL( (CSCPServer&)( *this ) );
	}



// ---------------------------------------------------------
// TInt CSCPServer::ThreadFunctionStage2()
// Constructs the active scheduler and the server object, and
// runs the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPServer::ThreadFunctionStage2L()
    {
    Dprint( (_L("--> CSCPServer::ThreadFunctionStage2()") ));
    
    // Naming the server thread after the server helps to debug panics  
    User::LeaveIfError( User::RenameThread( KSCPServerName ) );
	
    // Construct the active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler);

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server    
    CSCPServer* server = CSCPServer::NewL();
    CleanupStack::PushL( server );

    RProcess::Rendezvous(KErrNone);
	
    // Start handling requests
    Dprint( (_L("CSCPServer: Server started") ));
    CActiveScheduler::Start();
        
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( activeScheduler );
	
    Dprint( (_L("<-- CSCPServer::ThreadFunctionStage2()") ));
    return KErrNone;
    }
	
	
	
// ---------------------------------------------------------
// TInt CSCPServer::ThreadFunction( TAny* )
// First-stage server entry point, creates the cleanup stack,
// and calls the actual server worker function.
// 
// Status : Approved
// ---------------------------------------------------------
//	
TInt CSCPServer::ThreadFunction(TAny* /*aNone*/)
    {	
    Dprint( (_L("--> CSCPServer::ThreadFunction()") ));
	
    __UHEAP_MARK;
    
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !cleanupStack )
	      {
        PanicServer( ECreateTrapCleanup );
        }	

    TRAPD( err, ThreadFunctionStage2L() );
    if ( err != KErrNone )
        {
        PanicServer( ESvrStartServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;
    
    __UHEAP_MARKEND;

    Dprint( (_L("<-- CSCPServer::ThreadFunction()") ));
    return KErrNone;
    }


// ---------------------------------------------------------
// void CSCPServer::GetEtelHandlesL()
// Opens the handles to the Etel API, if not open, and copies
// their addresses to the given pointers, if available.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPServer::GetEtelHandlesL( RTelServer** aTelServ, RMobilePhone** aPhone )
    {
    if ( !iTelServ.Handle() )
        {
        // Connect to Etel
        User::LeaveIfError( iTelServ.Connect() );
                
        Dprint( (_L("CSCPServer::GetEtelHandlesL(): TelServ connected OK") ));
        }
	
	if ( !iPhone.SubSessionHandle() )
	    {
        // Load the phone module
        TInt err = iTelServ.LoadPhoneModule( KMmTsyModuleName );
        if ( err != KErrAlreadyExists )
            {
            User::LeaveIfError( err );
            }	        
        Dprint( (_L("CSCPServer::GetEtelHandlesL(): LoadPhoneModule OK") ));
           
        // Open the multimode TSY
        User::LeaveIfError( iPhone.Open( iTelServ, KMmTsyPhoneName ) );
                 
        Dprint( (_L("CSCPServer::GetEtelHandlesL(): MM-Phone connected OK") ));
	    }	    

	if ( aTelServ )
	    {
	    *aTelServ = &iTelServ;
	    }
	
	if ( aPhone )
	    {
	    *aPhone = &iPhone;
	    }            
    }
    

// ---------------------------------------------------------
// TInt CSCPServer::GetCode( TDes& aCode )
// Writes the stored ISA code to aCode
// 
// Status : Approved
// ---------------------------------------------------------
//	
TInt CSCPServer::GetCode( TDes& aCode )
    {
    Dprint( (_L("--> CSCPServer::GetCode()") ));
   
    aCode.Zero();
    aCode.Copy( iConfiguration.iSecCode );
   
    Dprint( (_L("<-- CSCPServer::GetCode()") ));   
    return KErrNone;
    }
   
   
   
// ---------------------------------------------------------
// TInt CSCPServer::StoreCode( TDes& aCode )
// Sets the stored ISA code to aCode and saves it to flash
// 
// Status : Approved
// ---------------------------------------------------------
//	
TInt CSCPServer::StoreCode( TDes& aCode )
    {
    Dprint( (_L("--> CSCPServer::StoreCode()") ));
    
    TInt ret = KErrNone;
    
    iConfiguration.iSecCode.Zero();
    iConfiguration.iSecCode.Copy( aCode );

    // Unset the invalid configuration flag
    iConfiguration.iConfigFlag = KSCPConfigOK;
    
    // The code has changed, write the new value instantly to flash
    TRAPD( err, iConfiguration.WriteSetupL() );
    if ( err != KErrNone )
        {
        Dprint( (_L("CSCPServer::~CSCPServer(): ERROR writing the configuration\
            file: %d"), err ));
        ret = err;
        }           

    Dprint( (_L("<-- CSCPServer::StoreCode(): %d"), ret ));
    
    return ret;
    }   
   


// ---------------------------------------------------------
// CSCPServer::ChangeISACodeL()
// Initializes the TSY and calls the function to change 
// the ISA-side code. Automatically uses the old code stored
// on the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPServer::ChangeISACodeL( RMobilePhone::TMobilePassword& aNewPassword )
    {
    Dprint( (_L("--> CSCPServer::ChangeISACodeL()") ));    
           
    RMobilePhone::TMobilePassword oldPassword;
    TInt ret;
    
    // Check the given code
    if ( !IsValidISACode( aNewPassword ) )
        {
        Dprint( (_L("CSCPServer::ChangeISACodeL(): ERROR:\
            Invalid code format") ));
        User::Leave( KErrArgument );
	    }           
   
    // No access to the ISA side in emulator      
    GetEtelHandlesL();
			
    // Fetch the stored ISA password from the server    

    ret = GetCode( oldPassword );
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPServer::ChangeISACodeL(): ERROR:\
            Failed to get stored code") ));
        User::Leave( ret );
        }           
    
    TRequestStatus status = KErrNone;    
    
#ifndef __WINS__    
    // No access to the ISA side in emulator, for WINS the call always succeeds
    
    RMobilePhone::TMobilePhonePasswordChangeV1 changeInfo;
    changeInfo.iOldPassword = oldPassword;
    changeInfo.iNewPassword = aNewPassword;
   
    RMobilePhone::TMobilePhoneSecurityCode securityCode = 
        RMobilePhone::ESecurityCodePhonePassword;
	
	Dprint( (_L("CSCPServer::ChangeISACodeL(): Trying to change\
	    the code, values %s, %s"), oldPassword.PtrZ(), aNewPassword.PtrZ() ));
	    
	iPhone.ChangeSecurityCode(status, securityCode, changeInfo );
	
	// This is a quick call, just wait here
	User::WaitForRequest( status );
	
#endif // WINS
    ret = status.Int();

	if ( ret == KErrNone )
		{
		Dprint( (_L("CSCPServer::ChangeISACodeL(): Code change OK") ));
		
		// Update the stored code in the server
		if ( StoreCode( aNewPassword ) != KErrNone )
            {
            Dprint( (_L("CSCPServer::ChangeISACodeL():\
                Failed to store the new code!") ));
            ret = KErrGeneral;
            }
		}
	else
		{
        Dprint( (_L("CSCPServer::ChangeISACodeL(): FAILED:\
            ChangeSecurityCode returned %d"), ret ));					
		}	
	
	User::LeaveIfError( ret );
	Dprint( (_L("<-- CSCPServer::ChangeISACodeL()") ));	
	}	



// ---------------------------------------------------------
// TInt CSCPServer::CheckIfRfsPerformedL()
// Checks if RFS has been run from the DMUtil server.
// 
// Status : Approved
// ---------------------------------------------------------
//	
void CSCPServer::CheckIfRfsPerformedL()
    {
    Dprint( (_L("--> CSCPServer::CheckIfRfsPerformedL()") ));
    
    TBool resetServer = EFalse;
    
#ifdef SCP_USE_POLICY_ENGINE
	Dprint( (_L("--> CSCPServer::CheckIfRfsPerformedL() : checking if Rfs performed") ));
	TRAPD(err,TDMUtilPassiveObserver::IsRFSPerformedL());
    if ( err == 1 )
        {
        resetServer = ETrue;
        }
    else if ( err == 0)
    	{
    	Dprint( (_L("--> CSCPServer::CheckIfRfsPerformedL() : Rfs is not performed") ));
    	}
    else
   		{
    	Dprint( (_L("--> CSCPServer::CheckIfRfsPerformedL() : Leave occured. Ignore it.") ));
    	}
#endif // SCP_USE_POLICY_ENGINE       
    
    if ( resetServer )
        {
        Dprint( (_L("CSCPServer::CheckIfRfsPerformedL(): Rfs performed, resetting parameters") ));
                
        // Reset max. period
        iConfiguration.iMaxTimeout = 0;
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS    
if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))   
{
        // Reset the common plugin parameter storage
        TRAPD( err, iPluginEventHandler->ResetConfigurationL() );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPServer::CheckIfRfsPerformedL(): ERROR resetting the plugin\
                configuration: %d"), err ));
            }
        
        // Reset the plugin's private storage
        CSCPParamObject* inParams = CSCPParamObject::NewL();
        CleanupStack::PushL( inParams );
        
        inParams->Set( KSCPParamContext, KSCPContextRfs );        
                
        CSCPParamObject* reply = 
            iPluginManager->PostEvent( KSCPEventReset, *inParams );
        CleanupStack::PopAndDestroy( inParams ); 
        
        // Ignore the reply, no plugin should request any actions
        if ( reply != NULL )
            {
            delete reply;
            }
}
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS               

        } 
    
    Dprint( (_L("<-- CSCPServer::CheckIfRfsPerformedL()") ));        
    }
    
    
    
   
// ---------------------------------------------------------
// void CSCPServer::ValidateConfigurationL()
// Checks if the correct ISA code is stored on the server.
// This method is used to counter the 3-button format function
// available on some devices.
// 
// Status : Approved
// ---------------------------------------------------------
//	
void CSCPServer::ValidateConfigurationL( TInt aMode )
    {
    if (aMode == KSCPInitial) {
        // Return here, must be checked by complete mode
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@@aMode == KSCPInitial "));
        User::Leave(KErrAccessDenied);
    }
    
    RMobilePhone::TMobilePassword storedCode;
    storedCode.Zero();
    User::LeaveIfError(GetCode(storedCode));
    TBool isDefaultLockcode = ETrue;
    TInt err = KErrNone;
    
    RMobilePhone::TMobilePassword defaultLockcode;
    defaultLockcode.Zero();
    defaultLockcode.Copy(KSCPDefaultSecCode);
    if (storedCode.Compare(KSCPDefaultSecCode) == 0) {
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@config has KSCPDefaultSecCode "));
        TRAP( err, CheckISACodeL( defaultLockcode ) );
        if (err == KErrNone) {
            RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@ISA also has KSCPDefaultSecCode "));
            iConfiguration.iConfigFlag = KSCPConfigOK;
            isDefaultLockcode = ETrue;
        }
        else {
            RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@ISA doesn't has KSCPDefaultSecCode "));
            iConfiguration.iConfigFlag = KSCPConfigInvalid;
            iConfiguration.iFailedAttempts++;
            isDefaultLockcode = EFalse;
        }
    }
    else {
        RDebug::Print(_L("CSCPServer::CheckISACodeL(): config lock code %s"), storedCode.PtrZ());
        TRAP( err, CheckISACodeL( storedCode ) );
        if (err == KErrNone) {
            RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@ISA and config are in SYNC !! "));
            iConfiguration.iConfigFlag = KSCPConfigOK;
            isDefaultLockcode = EFalse;
        }
        else {
            iConfiguration.iSecCode.Zero();
            iConfiguration.iSecCode.Append(KSCPDefaultSecCode);
            RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@May be ISA has KSCPDefaultSecCode "));
            TRAP(err,ChangeISACodeL(storedCode));
            if (err == KErrNone) {
                RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()chnaged ISA code with config value "));
                iConfiguration.iConfigFlag = KSCPConfigOK;
                isDefaultLockcode = EFalse;
            }
            else
            {
                RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()it shouldn't reach this :( "));
            }
        }
    }

    CRepository* repository = CRepository::NewL(KCRUidSCPLockCode);
    CleanupStack::PushL(repository);
    if (isDefaultLockcode ) {
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()setting def. lockcode to 12345 "));
        repository->Set(KSCPLockCodeDefaultLockCode, 12345);
    }
    else {
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()setting def. lockcode to 0 "));
        repository->Set(KSCPLockCodeDefaultLockCode, 0);
    }
    CleanupStack::PopAndDestroy(repository);
    
    TRAP( err, iConfiguration.WriteSetupL() );
    if (err != KErrNone) {
        Dprint( (_L("CSCPServer::ValidateConfigurationL(): WARNING: failed to write configuration\
                : %d"), err ));
    }

    if (iConfiguration.iConfigFlag == KSCPConfigOK) {
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@iConfigFlag == KSCPConfigOK "));
        err = KErrNone;
    }
    else {
        RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()@iConfigFlag == KErrAccessDenied "));
        err = KErrAccessDenied;
    }

    User::LeaveIfError(err);
}

        
    
    
// ---------------------------------------------------------
// void CSCPServer::CheckISACodeL( )
// Verifies the given ISA code.
// 
// Status : Approved
// ---------------------------------------------------------
//	
void CSCPServer::CheckISACodeL( RMobilePhone::TMobilePassword aCode )
    {        
    TInt ret = KErrNone;    

#ifdef __WINS__    
    
    (void)aCode;    

#endif // __WINS__


Dprint( (_L("CSCPServer::CheckISACodeL(): current lock code %s"), aCode.PtrZ() ));
RDebug::Print(_L("CSCPServer::CheckISACodeL(): current lock code %s"), aCode.PtrZ());
    RMobilePhone::TMobilePhoneSecurityCode secCodeType;
    secCodeType = RMobilePhone::ESecurityCodePhonePassword;
    
    RMobilePhone::TMobilePassword required_fourth;
                
    TRequestStatus status = KRequestPending;
            
    GetEtelHandlesL();
    
#ifndef __WINS__   // We cannot check the code in WINS, always return KErrNone. 
       
    iPhone.VerifySecurityCode( status, secCodeType, aCode, required_fourth );
    
    // This should be quick, wait here
    User::WaitForRequest( status );
    
    ret = status.Int();
    
#endif // !__WINS__
        
    if ( ret == KErrNone )
        {
        // Code OK
        Dprint( (_L("CSCPServer::CheckISACodeL(): ISA code OK") ));
        iConfiguration.iBlockedInOOS = 0;
        }
    else 
        {
        if ( ( ret == KErrAccessDenied ) || ( ret == KErrGsm0707IncorrectPassword ) )
            {
            Dprint( (_L("CSCPServer::CheckISACodeL(): ISA code NOT OK") ));
            if (iConfiguration.iBlockedInOOS == 1)
	         {
	            	iConfiguration.iBlockedInOOS = 0;
   	            	Dprint( (_L("CSCPServer::CheckISACodeL():iBlockedInOOS = 0, KErrAccessDenied") ));
   	            	RDebug::Print(_L("--> CSCPServer::CheckISACodeL()@@iBlockedInOOS = 0, KErrAccessDenie"));
	         }            
            ret = KErrAccessDenied;             
            }
        else if ( ( ret == KErrGsmSSPasswordAttemptsViolation ) || ( ret == KErrLocked ) )
            {
            Dprint( (_L("CSCPServer::CheckISACodeL(): ISA code BLOCKED") ));            
            if (ret==KErrGsmSSPasswordAttemptsViolation)
            {
            	Dprint( (_L("CSCPServer::CheckISACodeL(): KErrGsmSSPasswordAttemptsViolation") ));  
            	RDebug::Print(_L("--> CSCPServer::CheckISACodeL()@@KErrGsmSSPasswordAttemptsViolation"));
            }
            else
            {
            	Dprint( (_L("CSCPServer::CheckISACodeL(): KErrLocked") ));
            	RDebug::Print(_L("--> CSCPServer::CheckISACodeL()@@KErrLocked"));
            }	            
            ret = KErrLocked;
            if (iConfiguration.iBlockedInOOS == 0)
        	 {
            	iConfiguration.iBlockedInOOS = 1;
            	Dprint( (_L("CSCPServer::CheckISACodeL():iBlockedInOOS = 1, KSCPErrCodeBlockStarted") ));
            	RDebug::Print(_L("--> CSCPServer::CheckISACodeL()@@@@@"));
            	ret = KSCPErrCodeBlockStarted;	
        	 }
            }            
        else
            {
            Dprint( (_L("CSCPServer::CheckISACodeL(): ERROR reply checking ISA code: %d"),
                status.Int() ));
                RDebug::Print(_L("--> CSCPServer::ValidateConfigurationL()"));
            }            
        }
        TRAPD( err, iConfiguration.WriteSetupL() );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPServer::StoreEnhCode(): WARNING:\
                failed to write configuration: %d"), err ));         
        }                
    
    User::LeaveIfError( ret );
    }    


//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

// ---------------------------------------------------------
// CSCPServer::HashISACode()
// Creates a 5-digit ISA code from the given MD5 digest.
// 
// Status : Approved
// ---------------------------------------------------------
//	
TInt CSCPServer::HashISACode( TDes& aHashBuf )
    {                
    // aHashBuf must be a MD5 hash digest
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
			return KErrNotSupported;
	}
    if ( aHashBuf.Length() < KSCPMD5HashLen )
        {
        PanicServer( EBadHashDigest );
        }
        
    // Compute the hash sum as four 32-bit integers.
    TInt64 hashSum = *(reinterpret_cast<TInt32*>(&aHashBuf[0])) + 
                     *(reinterpret_cast<TInt32*>(&aHashBuf[4])) + 
                     *(reinterpret_cast<TInt32*>(&aHashBuf[8])) +
                     *(reinterpret_cast<TInt32*>(&aHashBuf[12]));
    
    // Create a five-digit security code from this number
    TInt ISAcode = ( hashSum % 90000 ) + 10000;
    Dprint( (_L("CSCPServer::HashISACode(): Hashed ISA code is %d"), ISAcode ));
    
    return ISAcode;
    }
    
   
// ---------------------------------------------------------
// TInt CSCPServer::StoreEnhCode( TDes& aCode )
// Sets the stored enhanced code to aCode and saves it to flash
// 
// Status : Approved
// ---------------------------------------------------------
//	
TInt CSCPServer::StoreEnhCode( TDes& aCode, TSCPSecCode* aNewDOSCode /*=NULL*/)
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
			return KErrNotSupported;
	}
    Dprint( (_L("CSCPServer::StoreEnhCode()") ));
    
    // Hash the code and save it
    TBuf<KSCPMaxHashLength> hashBuf;
    hashBuf.Zero();
    iPluginEventHandler->HashInput( aCode, hashBuf );        
        
    TInt ISAcode = HashISACode( hashBuf );
    
    RMobilePhone::TMobilePassword newCode;
    newCode.Zero();
    newCode.AppendNum( ISAcode );
    
    // Change the ISA code
    TRAPD( err, ChangeISACodeL( newCode ) );
    
    if ( err == KErrNone )
        {
        iConfiguration.iEnhSecCode.Zero();
        iConfiguration.iEnhSecCode.Copy( hashBuf );        

        if ( aNewDOSCode != NULL )
            {
            // Copy the updated DOS code
            (*aNewDOSCode).Copy( newCode );
            }
        
        TRAPD( err, iConfiguration.WriteSetupL() );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPServer::StoreEnhCode(): WARNING:\
                failed to write configuration: %d"), err ));         
                                            
            }    
        else
        	{
        	/* Get the very first character of the new lock code and set the default input mode of the
         	lock code query on the basis of the first character. */
         	ch = aCode[0];
		
				CRepository* repository = NULL;
			def_mode = ch.IsDigit() ? 0 : 1;
    		TRAPD(err, repository = CRepository::NewL( KCRUidSCPParameters ));
    		if (err == KErrNone)
    		{    	
    			err = repository->Set( KSCPLockCodeDefaultInputMode , def_mode );
    	   	delete repository;
    		}
    		
    		/* Set the value in the cenrep that the default lock code has been changed if it is not 
    		 * already set
    		 * */
    		TRAP(err, repository = CRepository :: NewL(KCRUidSCPLockCode));
    		
    		if(err == KErrNone) {
    			RDebug::Print(_L("<-- CSCPServer::StoreEnhCode()  setting KSCPLockCodeDefaultLockCode to 0"));
                err = repository->Set(KSCPLockCodeDefaultLockCode, 0);
                delete repository;
    		}
        	}    
        }
    else
        {
        Dprint( (_L("CSCPServer::StoreEnhCode(): ERROR:\
            failed to change ISA code, aborting storage operation: %d"), err ));
        
        }    
    
    return err;   
    }

//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS




// ---------------------------------------------------------
// TBool CSCPServer::IsOperationInProgress( TSCPAdminCommand aCommand )
// Loops through the sessions on the server and and calls the
// query command to check if the given operation is in progress.
// 
// Status : Approved
// ---------------------------------------------------------
//	
TBool CSCPServer::IsOperationInProgress( TSCPAdminCommand aCommand )
    {
    Dprint( (_L("--> CSCPServer::IsOperationInProgress()") ));
   
    // Iterate through the sessions and find out if one has started the given operation
    TBool ret = EFalse;
   
    iSessionIter.SetToFirst();
    CSCPSession* curSession = NULL;
    while ( ( curSession = static_cast<CSCPSession*>( iSessionIter++ ) ) != NULL )
        {                
        if ( curSession->IsOperationInProgress( aCommand ) )
            {
            ret = ETrue;
            break;        
            }   
        }    
    
    Dprint( (_L("<-- CSCPServer::IsOperationInProgress(): %d"), ret ));    
    return ret;
    }   




// ---------------------------------------------------------
// TBool CSCPServer::AcknowledgementReceived( TSCPAdminCommand aCommand )
// Loops through the sessions on the server, sends the acknowledgements,
// and checks if one was waiting for it (ETrue).
// 
// Status : Approved
// ---------------------------------------------------------
//	
TBool CSCPServer::AcknowledgementReceived( TSCPAdminCommand aCommand )
    {
    Dprint( (_L("--> CSCPServer::AcknowledgementReceived()") ));
   
    // Iterate through the sessions and propagate the acknowledgement.
    // Set the return parameter, if one was waiting for the ack
    TBool ret = EFalse;
   
    iSessionIter.SetToFirst();
    CSCPSession* curSession = NULL;
    while ( ( curSession = static_cast<CSCPSession*>( iSessionIter++ ) ) != NULL )
        {
        if ( curSession->AcknowledgeOperation( aCommand ) ) ret = ETrue;
        }    
    
    Dprint( (_L("<-- CSCPServer::AcknowledgementReceived(): %d"), ret ));    
    return ret;
    }   
   
   
   
   
// ---------------------------------------------------------
// TInt CSCPServer::SetAutolockPeriod()
// Sets the autolock period in either SharedData or CenRep.
// 
// Status : Approved
// ---------------------------------------------------------
// 
TInt CSCPServer :: SetAutolockPeriodL(TInt aValue) {
    Dprint( (_L("CSCPServer::SetAutolockPeriodL(): Setting the value") ));

    TInt ret = KErrNone;
    
    
    // Change the value in CenRep     
    ret = iALPeriodRep->Set( KSettingsAutoLockTime, aValue );            
     
    Dprint( (_L("[CSCPServer]-> SetAutolockPeriodL() <<<") ));
    return ret;
    }   
    
// ---------------------------------------------------------
// TInt CSCPServer::GetAutolockPeriodL( TInt& aValue )
// Gets the autolock period from either SharedData or CenRep.
// 
// Status : Approved
// ---------------------------------------------------------
// 
TInt CSCPServer::GetAutolockPeriodL( TInt& aValue )
    {
    TInt ret = KErrNone;    
    
    Dprint( (_L("CSCPServer::GetAutolockPeriodL(): Getting the value") )); 
            
    // Fetch the value from CenRep   
    ret = iALPeriodRep->Get( KSettingsAutoLockTime, aValue );
    
    return ret;
    } 
    
    
    
    
// ---------------------------------------------------------
// CSCPServer::ChangeCodePolicy()
// Sets the new policy either internally or through the policy
// engine.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPServer::ChangeCodePolicy( const TDesC& aValue )
    {    
    TInt ret = KErrNone;
    
    TLex lex( aValue );
    TInt value;
                
    ret = lex.Val( value );
                
    if ( ret == KErrNone )
        {
        if ( ( value < 0 ) || ( value > 1 ) ) 
            {
            ret = KErrArgument;
            }
        else
            {
            Dprint( (_L("CSCPServer::ChangeCodePolicy(): %d"), value ));
            
#ifdef SCP_USE_POLICY_ENGINE
    
            RPolicyEngine policyEngine;
            ret = policyEngine.Connect();

            RPolicyManagement policyManagement;
            if ( ret == KErrNone )
                {               
                ret = policyManagement.Open( policyEngine );
                }
    
            TParserResponse response;        
    
            if ( ret == KErrNone )
                {
                Dprint( (_L("CSCPServer::ChangeCodePolicy(): Executing operation") ));
                if ( value == 1 )
                    {
                    // Allow change
                    ret = policyManagement.ExecuteOperation( 
                        KSCPCodePolicyAllowOperation,
                        response
                        );
                    }
                else
                    {
                    // Deny change                    
                    ret = policyManagement.ExecuteOperation( 
                        KSCPCodePolicyDisallowOperation, 
                        response
                        );                        
                    }
                
                if ( ret == KErrNone )
                    {
                    const TDesC8& retMsg = response.GetReturnMessage();
                    
                    if ( retMsg.Compare( KSCPOKResponse ) != 0 )
                        {
                        #ifdef _DEBUG
                        TBuf<256> dummyBuf;
                        dummyBuf.Copy( retMsg );
                        Dprint( (_L("CSCPServer::ChangeCodePolicy(): Response not OK:\
                            %S"), &dummyBuf ));
                        #endif
                        
                        // We must check this for an error as well
                        ret = KErrGeneral;
                        }
                    }
                }
                
            if ( policyManagement.SubSessionHandle() )
                {
                policyManagement.Close();
                }

            if ( policyEngine.Handle() )
                {
                policyEngine.Close();
                }                        
                                
#else // !SCP_USE_POLICY_ENGINE
               
            iCodePolicy = value;                

#endif // SCP_USE_POLICY_ENGINE                
            }                
        }

    Dprint( (_L("<-- CSCPServer::ChangeCodePolicy(): %d"), ret ));
    return ret;
    }
      
      
      
        
// ---------------------------------------------------------
// CSCPServer:GetCodePolicy()
// Gets the policy from internal storage of from the policy
// engine.
// 
// Status : Approved
// ---------------------------------------------------------
//        
TInt CSCPServer::GetCodePolicy( TDes& aValue )
    {
    Dprint( (_L("--> CSCPServer::GetCodePolicy()") ));
    TInt ret = KErrNone;

    aValue.Zero();
    
#ifdef SCP_USE_POLICY_ENGINE            
    TResponse response;
    RPolicyEngine policyEngine;
    ret = policyEngine.Connect();

    RPolicyRequest policyRequest;
    if ( ret == KErrNone )
        {        
        ret = policyRequest.Open( policyEngine );
        }

    if(ret == KErrNone) {
        TRequestContext context;
    
        using namespace PolicyEngineXACML;
        TRAPD( err ,
            context.AddResourceAttributeL(  KResourceId, 
                                            KDeviceLockPasscodeVisibility,
                                            KStringDataType)
              );
        if ( err == KErrNone )
            {
            ret = policyRequest.MakeRequest( context, response );
            }
        else
            {
            ret = err;
            }        
        }

    if ( ret == KErrNone )
        {
        if ( response.GetResponseValue() == EResponsePermit )
            {
    	    // Allow security code change
	        aValue.AppendNum( 1 );
            }
        else
            {
	        // Do not allow security code change
	        aValue.AppendNum( 0 );
            }                    
        }
    
    if ( policyRequest.SubSessionHandle() )
        {
        policyRequest.Close();
        }

    if ( policyEngine.Handle() )
        {
        policyEngine.Close();
        }    
            
#else // !SCP_USE_POLICY_ENGINE
                            
     aValue.AppendNum( iCodePolicy );
            
#endif // SCP_USE_POLICY_ENGINE    

    Dprint((_L("[CSCPServer] <<< GetCodePolicy(): %d"), ret));
    return ret;
    }




// ---------------------------------------------------------
// TInt CSCPServer::SetParameterValueL( TSCPParameterID aID, TDesC& aValue )
// Handles the setting of the parameter values. 
// 
// Status : Approved
// ---------------------------------------------------------
//    
TInt CSCPServer::SetParameterValueL( TInt aID, const TDesC& aValue, TUint32 aCallerIdentity )
    {    
    Dprint( (_L("--> CSCPServer::SetParameterValueL( %d )"), aID ));
    
#ifdef __SCP_DEBUG
    // Used to "emulate 3-button boot" in debug builds
    if ( aID == 9999 )
        {
        iConfiguration.iConfigFlag = KSCPConfigUnknown;
        iConfiguration.iSecCode = KSCPDefaultSecCode;
        return KErrNone;
        }    
#endif // __SCP_DEBUG    
    
    TInt lRetStatus(KErrNone);
    Dprint(_L("[CSCPServer]-> Initiating branching on parameter..."));
    
    switch ( aID )
        {
        case ( ESCPAutolockPeriod ):
        // Flow through            
        case ( ESCPMaxAutolockPeriod ):
            {
            // Convert the value, and set it
            TInt value;
            TLex lex(aValue);
            lRetStatus = lex.Val(value);
             

            if((lRetStatus != KErrNone ) || ( value < 0) || ( value > KSCPAutolockPeriodMaximum )) {
                lRetStatus = KErrArgument;
                break;
                }
                  
            //Check if the device memory is encrypted or not.
            TBool encryptionEnabled = IsDeviceMemoryEncrypted();
            
            // Value OK
            if(aID == ESCPMaxAutolockPeriod) 
						{
						   if (  encryptionEnabled )
                  {
                  Dprint(_L("Memory is encrypted"));
                  if (( 0 == value) || value > KMaxAutolockPeriod)
                      {
                      Dprint((_L("Denying setting of max auto lock as value is %d"), value));
                      User::Leave( KErrPermissionDenied );
                      }
                      
                  }
                else
                    {
                    Dprint(_L("Memory is decrypted, hence no restrictions to max autolock"));
                    }
                    
                Dprint(_L("[CSCPServer]-> Branched to ESCPMaxAutolockPeriod..."));
                CSCPParamDBController* lParamDB = CSCPParamDBController :: NewLC();
                lRetStatus = SetBestPolicyL(RTerminalControl3rdPartySession :: EMaxTimeout, aValue, aCallerIdentity, lParamDB);

                if(lRetStatus == KErrNone) 
								{
                    // Do we have to change the Autolock period as well?
                    TInt currentALperiod;
                    lRetStatus = GetAutolockPeriodL(currentALperiod);

                    if(lRetStatus == KErrNone) 
										{
                        if((iConfiguration.iMaxTimeout > 0) && ((iConfiguration.iMaxTimeout < currentALperiod) || (currentALperiod == 0))) 
												{
                           Dprint((_L("[CSCPServer]-> Changing AL period to Max. AL period (Current ALP: %d, Max. ALP: %d)"), currentALperiod, value));
                           lRetStatus = SetAutolockPeriodL(value);
                        }
                    }
                    else 
										{
                        Dprint((_L("[CSCPServer]-> ERROR: Couldn't get the Autolock period: %d"), lRetStatus));
                    }
                }

                CleanupStack :: PopAndDestroy(); //lParamDB
            }
            
            else 
            { // Autolock Period
            	
            			//Code is commented as it is already taken care by the below condition #1343 irrespective of the drive encryption state.
            	     /*  if ( 0 == value )
                    {
                    if ( encryptionEnabled )
                        {
                        Dprint(_L("Permission denied!"));
                        User::Leave( KErrPermissionDenied );
                        }
                    }*/
                    
                Dprint(_L("[CSCPServer]-> Branched to ESCPAutolockPeriod..."));
                //  Check if this value is not allowed by the Max. Autolock period
                if ((iConfiguration.iMaxTimeout > 0) && ((iConfiguration.iMaxTimeout < value) || (value == 0))) {
                    Dprint((_L("[CSCPServer]-> ERROR: The value %d for AL period not allowed (Max. AL period: %d)"), value, iConfiguration.iMaxTimeout));

                    lRetStatus = KErrArgument;
                }
                else {
                    lRetStatus = SetAutolockPeriodL(value);

                    if(lRetStatus != KErrNone) {
                        Dprint((_L("[CSCPServer]-> ERROR: Couldn't set the Autolock period: %d"), lRetStatus));
                    }
                }
            }
        }
        break;
        case ESCPCodeChangePolicy:
            Dprint(_L("[CSCPServer]-> Branched to ESCPCodeChangePolicy..."));
            lRetStatus = ChangeCodePolicy( aValue );
        break;
        default: {
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
            Dprint(_L("[CSCPServer]-> Branched to default condition, aID=%d"), aID);
            
            if(FeatureManager :: FeatureSupported(KFeatureIdSapDeviceLockEnhancements)) {
                RPointerArray<HBufC> lSpecStrArray;
                CleanupClosePushL(lSpecStrArray);

                Dprint(_L("[CSCPServer]-> Creating an instance of the private DB..."));
                CSCPParamDBController* lParamDB = CSCPParamDBController :: NewLC();
                Dprint(_L("[CSCPServer]-> Private DB instance created successfully..."));
                
                Dprint(_L("[CSCPServer]-> Creating ParamObject..."));
                CSCPParamObject* lInParams = CSCPParamObject :: NewLC();
                Dprint(_L("[CSCPServer]-> ParamObject created successfully..."));
                
                //lInParams->Set(KSCPParamIdentity, aCallerIdentity);
                
                /*
                 * Map KSCPParamIdentity with the SID of TCServer (not aCallerIdentity)
                 */
                lInParams->Set(KSCPParamIdentity, this->Message().SecureId());

                switch(aID) {
                    case RTerminalControl3rdPartySession :: EPasscodeAllowSpecific:
                    case RTerminalControl3rdPartySession :: EPasscodeClearSpecificStrings: {
                        RPointerArray<HBufC> lSharedStrArray;
                        CleanupClosePushL(lSharedStrArray);

                        /* If the choice is EPasscodeAllowSpecific then parse the specific strings mentioned and
                         * list the specific strings in an Array (multiple specific strings can be seperated with a single space)
                         * The Memory alloted for each of the tokens representing each individual specific string will be
                         * deallocated when the array lSpecStrArray is cleared by a call to ResetAndDestroy().
                         */
                        if(aID == RTerminalControl3rdPartySession :: EPasscodeAllowSpecific) {
                            TLex16 lex (aValue);
                            lex.Mark();

                            while(!lex.Eos()) {
                                while(lex.Peek() != ' ' && !lex.Eos()) {
                                    lex.Inc();
                                }

                                const TDesC& lToken = lex.MarkedToken();
                                HBufC* lBuff(NULL);
                                TRAP(lRetStatus, lBuff = HBufC :: NewL(lToken.Length()));

                                if(lRetStatus != KErrNone) {
                                    lSpecStrArray.ResetAndDestroy();
                                    User :: Leave(lRetStatus);
                                }

                                lBuff->Des().Append(lToken);
                                lSpecStrArray.Append(lBuff);
                                lex.Inc();
                                lex.Mark();
                            }
                        }
                        else {
                            lRetStatus = lParamDB->GetValuesL(RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific,
                                    lSpecStrArray, aCallerIdentity);
                        }

                        TInt lCnt = lSpecStrArray.Count();
                        lInParams->Set(KSCPParamID, RTerminalControl3rdPartySession :: EPasscodeAllowSpecific);

                        for(TInt i=0; i < lCnt; i++) {
                            HBufC* lPtr = lSpecStrArray[i];
                            TBool lIsShared(EFalse);

                            TRAPD(lErr, lIsShared = lParamDB->IsParamValueSharedL(lSpecStrArray[i], aCallerIdentity));

                            if(lErr != KErrNone) {
                                lSpecStrArray.ResetAndDestroy();
                                lSharedStrArray.ResetAndDestroy();
                                User :: Leave(lErr);
                            }

                            if(EFalse == lIsShared) {
                                TUint16* ptr = const_cast<TUint16*>(lPtr->Des().Ptr());
                                TPtr valBuf(ptr, lPtr->Des().Length(), lPtr->Des().Length());
                                lInParams->Set(KSCPParamValue, valBuf);

                                CSCPParamObject* reply = iPluginManager->PostEvent(KSCPEventConfigurationQuery, *lInParams);

                                if((reply != NULL) && (reply->Get(KSCPParamStatus, lRetStatus) == KErrNone)) {
                                    if(lRetStatus != KErrNone) {
                                        lSpecStrArray.ResetAndDestroy();
                                        lSharedStrArray.ResetAndDestroy();
                                        User :: Leave(lRetStatus);
                                    }

                                    delete reply;
                                }
                            }
                            else {
                                lSpecStrArray.Remove(i);
                                lSharedStrArray.AppendL(lPtr);
                                i--;
                                lCnt--;
                            }
                        }

                        /*
                         * Drop all the 'disallow strings' that are not shared between any other applications from the database.
                         */
                        TRAPD(lErr, lRetStatus = lParamDB->DropValuesL(RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific, lSpecStrArray));

                        if(lErr != KErrNone) {
                            lSpecStrArray.ResetAndDestroy();
                            lSharedStrArray.ResetAndDestroy();
                            User :: Leave(lErr);
                        }

                        if(lRetStatus == KErrNone) {
                            /*
                             * Drop the entries of 'disallow strings' of only the called application
                             * (Any application that shares these 'disallow strings' will not be affected)
                             */
                            TRAP(lErr, lRetStatus = lParamDB->DropValuesL(RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific,
                                    lSharedStrArray, aCallerIdentity));
    
                            if(lErr != KErrNone) {
                                lSpecStrArray.ResetAndDestroy();
                                lSharedStrArray.ResetAndDestroy();
                                User :: Leave(lErr);
                            }
    
                            if(lRetStatus != KErrNone) {
                                Dprint(_L("[CSCPServer]-> Unable to drop 'disallow strings' from the private database..."));
                            }
                        }
                        else {
                            Dprint(_L("[CSCPServer]-> Unable to drop unshared 'disallow strings' from the private database..."));
                        }

                        lSharedStrArray.ResetAndDestroy();
                        CleanupStack :: PopAndDestroy(); // lSharedStrArray
                    }
                    break;
                    default: {
                        lInParams->Set(KSCPParamID, aID);
                        TUint16* ptr = const_cast<TUint16*>(aValue.Ptr());
                        TPtr valBuf(ptr, aValue.Length(), aValue.Length());
                        lInParams->Set(KSCPParamValue, valBuf);

                        Dprint(_L("[CSCPServer]-> INFO: Posting the event to the plugins..."));                        
                        CSCPParamObject* reply = iPluginManager->PostEvent(KSCPEventConfigurationQuery, *lInParams);

                        if((reply != NULL) && (reply->Get(KSCPParamStatus, lRetStatus) == KErrNone)) {
                            if(lRetStatus == KErrNone) {
                                Dprint(_L("[CSCPServer]-> INFO: The event was consumed by one of the plugins, initiating storage activities (if any)..."));
                                // Check if the plugin wishes to save the parameters itself
                                TInt storage = KSCPStorageCommon;
                                reply->Get(KSCPParamStorage, storage);

                                if(storage == KSCPStoragePrivate) {
                                    switch(aID) {
                                        /* The plugin will take care of storing the values for EPasscodeDisallowSpecific, this is to make sure the
                                         * value is updated accordingly even in the private database (which is used as a reference to set the best policy-done
                                         * as per MfE-Part 3 requirement
                                        */
                                        case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific: {
                                            TLex16 lex (aValue);
                                            lex.Mark();

                                            while(!lex.Eos()) {
                                                while(lex.Peek() != ' ' && !lex.Eos()) {
                                                    lex.Inc();
                                                }

                                                const TDesC& lToken = lex.MarkedToken();
                                                HBufC* lBuff(NULL);
                                                TInt lErr(KErrNone);
                                                TRAP(lErr, lBuff = HBufC :: NewL(lToken.Length()));

                                                if(lErr != KErrNone) {
                                                    lSpecStrArray.ResetAndDestroy();
                                                    delete reply;
                                                    User :: Leave(lErr);
                                                }

                                                lBuff->Des().Append(lToken);
                                                lSpecStrArray.AppendL(lBuff);
                                                lex.Inc();
                                                lex.Mark();
                                            }

                                            TInt lRet(KErrNone);

                                            TRAP(lRet, lRetStatus = lParamDB->SetValuesL(RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific,
                                                    lSpecStrArray, aCallerIdentity));

                                            if(lRet != KErrNone) {
                                                lRetStatus = lRet;
                                            }
                                        }
                                        break;
                                        default:
                                            // Don't store the parameter, the plugin will handle this
                                            Dprint((_L("[CSCPServer]-> Param %d stored in private storage..."), aID));
                                        break;
                                    }
                                }
                                else {
                                    Dprint((_L("[CSCPServer]-> Param %d stored in common storage..."), aID));
                                    lRetStatus = SetBestPolicyL(aID, aValue, aCallerIdentity, lParamDB);
                                }
                            }
                        }
                        else {                        
                            Dprint(_L("[CSCPServer]-> INFO: Event posting failed, reply=%d"), reply);
                        }
                        
                        delete reply;
                        break;
                    }
                }

                lSpecStrArray.ResetAndDestroy();
                CleanupStack :: PopAndDestroy(3); // lSpecStrArray, lParamDB, lInParams
            }
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
            break;
        }
    }

    Dprint((_L("[CSCPServer]-> <<< SetParameterValueL(): %d"), lRetStatus));
    return lRetStatus;
}
// ---------------------------------------------------------
// TInt CSCPServer::GetParameterValueL( TSCPParameterID aID, TDes& aValue )
// Handles the fetching of parameter values.
//
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPServer::GetParameterValueL( TInt aID, TDes& aValue, TUint32 aCallerIdentity )
    {
    Dprint( (_L("CSCPServer::GetParameterValueL( %d )"), aID ));
    TInt ret = KErrNone;

    switch ( aID )
        {
        case ( ESCPAutolockPeriod ):
            {
            // Get the actual autolock period
            TInt value;
            if ( GetAutolockPeriodL( value ) != KErrNone )
                {
                Dprint( (_L("CSCPServer::SetParameterValueL(): ERROR: Couldn't \
                    get the Autolock period") ));
                }
            else
                {
                aValue.Zero();
                aValue.AppendNum(value);
                }
            }                
        break;
            
        case ( ESCPMaxAutolockPeriod ):
            {
            // Just fetch the value into the buffer
            aValue.Zero();
            aValue.AppendNum( iConfiguration.iMaxTimeout );
            }                
        break;  
        
        case ( ESCPCodeChangePolicy ):
            {
            ret = GetCodePolicy( aValue );                        
            }
        break;                  
            
        default:
            ret = KErrNotSupported;
            
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS                                               
if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
                        
            // "Ask" the plugins, if this caller is allowed to access this value or
            // if the value is in private storage, retrieve the value.
            CSCPParamObject* inParams = CSCPParamObject::NewL();
            CleanupStack::PushL( inParams );
            inParams->Set( KSCPParamID, aID );
            inParams->Set( KSCPParamIdentity, aCallerIdentity );
                
            CSCPParamObject* repParams = 
                iPluginManager->PostEvent( KSCPEventRetrieveConfiguration, *inParams );
        
            CleanupStack::PopAndDestroy( inParams );
            
            TBool getFromCommonStorage = ETrue;
            TInt status;
            if ( ( repParams != NULL ) && 
                 ( repParams->Get( KSCPParamStatus, status ) == KErrNone ) )
                {                
                ret = status;
                if ( ret != KErrNone )
                    {
                    getFromCommonStorage = EFalse;
                    Dprint( (_L("CSCPServer::GetParameterValueL():\
                            Retrieve configuration for param %d not OK: %d"), aID, ret ));
                    }
                else
                    {
                    // Has the plugin sent the value?
                    TInt valueLen = repParams->GetParamLength( KSCPParamValue );
                    if ( valueLen > 0 )
                        {
                        getFromCommonStorage = EFalse;
                        // Try to retrieve the value (Get will check for overflow)
                        ret = repParams->Get( KSCPParamValue, aValue );
                        }
                    }
                } 
            
            if ( getFromCommonStorage )
                {
                Dprint(_L("[CSCPServer]-> Fetching from Common storage..."));
                // OK, no objection, so try to get the value from common storage
                ret = iPluginEventHandler->GetParameters().Get( aID, aValue );
                }            
            
            if ( repParams != NULL )
                {
                delete repParams;
                }
	}
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS                
                
        break;
        }
        
    Dprint( (_L("<-- CSCPServer::GetParameterValueL(): %d"), ret )); 
    return ret;        
    }    
      
      
        
        
// ---------------------------------------------------------
// static TBool CSCPServer::IsValidISACode( TDes& aCode )
// Checks that the given string is a valid ISA security code.
// That is, exacly 5 chars, and contains only digits.
// 
// Status : Approved
// ---------------------------------------------------------
//    

TBool CSCPServer::IsValidISACode( TDes& aCode )
    {
    TBool ret = ETrue;
    
    if ( aCode.Length() != KSCPCodeMaxLen ) 
        {
        ret = EFalse;
        }
    else
        {
        for ( TInt i = 0; i < aCode.Length(); i++ )
            {        
            if ( !( ((TChar)aCode[i]).IsDigit() ) )
                {
                ret = EFalse;
                }
            }        
        }
    
    return ret;
    }
    



// ---------------------------------------------------------
// void CSCPServer::SessionClosed()
// Checks if this was the final session. If so, exit the 
// server thread.
// 
// Status : Approved
// ---------------------------------------------------------
//    

void CSCPServer::SessionClosed()
    {
    iSessionIter.SetToFirst();
    iSessionIter++; // Skip the closing session
    
    if ( iSessionIter++ == NULL )
        {
        // No more sessions
        Dprint( (_L("CSCPServer::SessionClosed(): No more sessions, starting timeout") ));
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
        // Save the plugin configuration
        TRAP_IGNORE( iPluginEventHandler->WritePluginConfigurationL() );
	}
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS        
        
        // Start shutdown timer
        TRAPD( err, iShutdownTimer = CSCPTimer::NewL( KSCPServerShutdownTimeout,
                                          NULL,
                                          this ) );
        if ( err != KErrNone )
            {
            // Cannot create the timer!
            // No more sessions
            Dprint( (_L("CSCPServer::SessionClosed(): ERROR: Timer creation failed! Exiting..")));
            iShutdownTimer = NULL;
            Timeout( NULL );
            }    
        }
    }
    
    
 
// ---------------------------------------------------------
// void CSCPServer::SessionOpened()
// Stops the shutdown timer, if started.
// 
// Status : Approved
// ---------------------------------------------------------
//  
void CSCPServer::SessionOpened()
    {
    // Stop the shutdown timer
	if ( iShutdownTimer != NULL )
	    {
	    delete iShutdownTimer;
	    iShutdownTimer = NULL;    	
	    }      
    }
    
    
    
    
// ---------------------------------------------------------
// void CSCPServer::Timeout()
// Shuts down the server stopping the active scheduler since 
// the shutdown timer has elapsed.
// 
// Status : Approved
// ---------------------------------------------------------
//      
void CSCPServer::Timeout( TAny* aParam )
    {
    (void)aParam;
    
    delete iShutdownTimer;
    iShutdownTimer = NULL;
    
    Dprint( (_L("CSCPServer::Timeout(): Shutting down SCP server") ));
    
    CActiveScheduler::Stop();
    }    
    
    
        
// ********************** NEW FEATURES ********************>>>>

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

// ---------------------------------------------------------
// CSCPServer::IsValidEnhCode()
// Checks that the given buffer is an alphanumeric string 
// and its length is between the minimum and maximum.
// 
// Status : Approved
// ---------------------------------------------------------
// 
TBool CSCPServer::IsValidEnhCode( TDes& aCode )
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TBool ret = ETrue;
    
    // Check length
    if (    ( aCode.Length() < KSCPPasscodeMinLength ) || 
            ( aCode.Length() > KSCPPasscodeMaxLength ) )
        {
        ret = EFalse;
        }
        
    // Check the characters
    if ( ret )
        {
        for ( TInt i = 0; i < aCode.Length(); i++ )
            {
            TChar curChar = aCode[i];
            TChar::TCategory chCat = curChar.GetCategory();
            if ( curChar < 1 || curChar > 126 || curChar.IsSpace() )
            /* if ( ( !curChar.IsAlphaDigit() ) && ( chCat != TChar::ENdCategory ) ) */
                {
                ret = EFalse;
                }
            }
        }
    
    return ret;
    }


// ---------------------------------------------------------
// CSCPServer::IsCodeBlocked()
// Checks if the code is blocked, and zeroes the attempt counter
// when necessary.
//
// Status : Approved
// ---------------------------------------------------------
//  
TBool CSCPServer::IsCodeBlocked()
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TBool ret = EFalse;
    
    if ( iConfiguration.iBlockedAtTime.Compare( KSCPNotBlocked ) == 0 )
        {
        ret = EFalse;
        }
    else
        {
        TTime curTime;
        curTime.UniversalTime();        

        TTimeIntervalMinutes interval = KSCPCodeBlockedTimeMin;
        
        TInt64 savedTime;
        TLex lex( iConfiguration.iBlockedAtTime );
        ret = lex.Val( savedTime );        
        
        if ( ret == KErrNone )
            {
            TTime configTime( savedTime );
            if ( ( configTime + interval ) > curTime )
                {
                Dprint( (_L("CSCPServer::IsCodeBlocked(): Code BLOCKED") ));
                // Restart the timer                
                iConfiguration.iBlockedAtTime.Zero();
                iConfiguration.iBlockedAtTime.AppendNum( curTime.Int64() );
                
                ret = ETrue;
                }            
            else
                {
                // Not blocked anymore
                
                Dprint( (_L("CSCPServer::IsCodeBlocked(): Resetting code block") ));
                iConfiguration.iFailedAttempts = 0;
                iConfiguration.iBlockedAtTime.Zero();
                iConfiguration.iBlockedAtTime.Copy( KSCPNotBlocked );                    
                    
                ret = EFalse;
                }
                                                
            TRAPD( err, iConfiguration.WriteSetupL() );
            if ( err != KErrNone )
                {
                Dprint( (_L("CSCPServer::IsCodeBlocked(): WARNING:\
                    failed to write configuration: %d"), err ));
                                            
                }                 
            }       
        }
    
    return ret;
    }
    
    
    

// ---------------------------------------------------------
// CSCPServer::SendInvalidDOSCode()
// Resets the DOS password attempts if required and verifies the
// code.
//
// Status : Approved
// ---------------------------------------------------------
//  
void CSCPServer::SendInvalidDOSCode( RMobilePhone::TMobilePassword& aCodeToSend )
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return ;
    }
    // Reset the DOS-side PSW-attempts by changing the code to the same one
    RMobilePhone::TMobilePassword correctPsw;
    TRAPD( err, 
           GetCode( correctPsw );
           ChangeISACodeL( correctPsw );
         );
    if ( err != KErrNone )
        {
        Dprint( (_L("CSCPServer::SendInvalidDOSCode(): WARNING:\
            DOS code change failed, where it shouldn't have: %d"), err ));
        }
    
    // Send the incorrect code to DOS side
    TRAPD( ret, CheckISACodeL( aCodeToSend ) );
    #ifndef __WINS__
    if ( ( ret != KErrAccessDenied ) && ( ret != KErrGsm0707IncorrectPassword ) )
        {
        Dprint( (_L("CSCPServer::SendInvalidDOSCode(): WARNING:\
            invalid DOS code validation error: %d"), ret ));
        }
    #endif // __WINS__
    }




// ---------------------------------------------------------
// CSCPServer::IsCorrectEnhCode()
// Checks if the given code is the correct enhanced code.
// If the configuration is not up-to-date, the code will be hashed
// and verified from the ISA-side.
//
// Status : Approved
// ---------------------------------------------------------
//  
TInt CSCPServer::IsCorrectEnhCode( TDes& aCode, TInt aFlags )
    {
    Dprint( (_L("CSCPServer::IsCorrectEnhCode") ));
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TInt ret = KErrAccessDenied;
    
    // Hash the code
    TBuf<KSCPMaxHashLength> hashBuf;
    hashBuf.Zero();
    iPluginEventHandler->HashInput( aCode, hashBuf );
        
    TInt ISACode = HashISACode( hashBuf );
        
    RMobilePhone::TMobilePassword pswCandidate;
    pswCandidate.Zero();
    pswCandidate.AppendNum( ISACode );
        
    TBool enhancedCodeMatches = EFalse;
    if ( hashBuf.Compare( iConfiguration.iEnhSecCode ) == 0 )
        {
        enhancedCodeMatches = ETrue;
        }
        
    // Check if the code is blocked (don't check if we're out-of-sync)
    if ( ( iConfiguration.iConfigFlag == KSCPConfigOK ) && ( IsCodeBlocked() ) )
        {
        if ( aFlags & KSCPEtelRequest )
            {
            // Check if the code is correct
            if ( enhancedCodeMatches )
                {
                // Mess-up the code                 
                RMobilePhone::TMobilePassword codeToSend;
                codeToSend.Zero();
                codeToSend.AppendNum( ISACode + 1 );
                
                SendInvalidDOSCode( codeToSend );
                }
            else 
                {
                // OK, the code is already invalid
                SendInvalidDOSCode( pswCandidate );
                }
            }
        return KErrLocked;
        }       
    
    TBool writeSetup = EFalse;
    
    if ( iConfiguration.iConfigFlag == KSCPConfigOK )
        {
        // Normal situation: we have the correct code stored.
        // Compare the hashes (hashing error will result in EFalse )
        if ( enhancedCodeMatches )
            {
            ret = KErrNone;
            
            if ( aFlags & KSCPEtelRequest )
                {
                // Send the correct code to DOS side
                TRAP( ret, CheckISACodeL( pswCandidate ) );
                }  
                
            if ( ret == KErrNone )
                {
                if ( iConfiguration.iFailedAttempts > 0 )
                    {
                    iConfiguration.iFailedAttempts = 0;  
                    Dprint( (_L("CSCPServer::IsCorrectEnhCode():KErrAccessDenied: iFailedAttempts (%d)."), iConfiguration.iFailedAttempts ));
                    writeSetup = ETrue;
                    }                
                }
            else
                {
                Dprint( (_L("CSCPServer::IsCorrectEnhCode(): WARNING:\
                    ISA code validation failed, where it shouldn't have: %d"), ret ));
                }
            }
        else
            {
            ret = KErrAccessDenied;
            
            iConfiguration.iFailedAttempts++;
            Dprint( (_L("CSCPServer::IsCorrectEnhCode():KErrAccessDenied: iFailedAttempts (%d)."), iConfiguration.iFailedAttempts ));
            writeSetup = ETrue;
            
            if ( iConfiguration.iFailedAttempts == KSCPCodeBlockLimit )
                {
                // Block the code
                TTime curTime;
                curTime.UniversalTime();
                
                iConfiguration.iBlockedAtTime.Zero();
                iConfiguration.iBlockedAtTime.AppendNum( curTime.Int64() );
                                                                
                // The code will be blocked for now on
                ret = KSCPErrCodeBlockStarted;
                }
            
            if ( aFlags & KSCPEtelRequest )
                {
                SendInvalidDOSCode( pswCandidate );
                }
            }
        }
    else 
        {
        // iConfiguration.iConfigFlag == KSCPConfigInvalid or KSCPConfigUnknown
        
        // We might be out-of-sync, no idea about the real code.
        // Check if the DOS code hashed from the given code is correct.
        Dprint( (_L("CSCPServer::IsCorrectEnhCode(): Attempting to correct OoS situation.") ));
        if (IsCodeBlocked()) {
            Dprint( (_L("CSCPServer::IsCorrectEnhCode(): OOS ->KErrLocked  ") ));
            return KErrLocked;
        }
        TRAP( ret, CheckISACodeL( pswCandidate ) );

        if (ret == KErrNone) {
            // OK, we must assume that this is the correct code, since
            // the hashed DOS code is correct. Save the codes, and return OK.

            Dprint( (_L("CSCPServer::IsCorrectEnhCode(): Given code has the correct hash (%d)\
                , saving codes."), ISACode ));
                
            iConfiguration.iEnhSecCode.Zero();
            iConfiguration.iEnhSecCode.Copy( hashBuf );

            iConfiguration.iSecCode.Zero();
            iConfiguration.iSecCode.AppendNum( ISACode );
            
            // Unset the invalid configuration flag
            iConfiguration.iConfigFlag = KSCPConfigOK;
            writeSetup = ETrue;
        }
        else {
            ret = KErrAccessDenied;

            iConfiguration.iFailedAttempts++;
            Dprint( (_L("CSCPServer::IsCorrectEnhCode():@@@: iFailedAttempts (%d)."), iConfiguration.iFailedAttempts ));
            writeSetup = ETrue;

            if (iConfiguration.iFailedAttempts == KSCPCodeBlockLimit) {
                Dprint( (_L("CSCPServer::IsCorrectEnhCode(): KSCPCodeBlockLimit  ") ));
                // Block the code
                TTime curTime;
                curTime.UniversalTime();

                iConfiguration.iBlockedAtTime.Zero();
                iConfiguration.iBlockedAtTime.AppendNum(curTime.Int64());

                // The code will be blocked for now on
                ret = KSCPErrCodeBlockStarted;
            }
        }

    }
    
    // Write setup if needed
    if ( writeSetup )
        {
        	Dprint( (_L("CSCPServer::IsCorrectEnhCode(): 7  ") ));
        TRAPD( err, iConfiguration.WriteSetupL() );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPServer::IsCorrectEnhCode(): WARNING:\
                failed to write configuration: %d"), err ));                                        
            }                 
        }
        
    return ret;
    }
    


// ---------------------------------------------------------
// CSCPServer::CheckCodeAndGiveISA()
// Check the given code, and notify the plugins of the result.
//
// Status : Approved
// ---------------------------------------------------------
//    
TInt CSCPServer::CheckCodeAndGiveISAL( TDes& aCodeToCheck,
                                      TDes& aISACodeToReturn,
                                      CSCPParamObject*& aRetParams,
                                      TInt aFlags )
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TInt ret = KErrNone;
    TInt status = KErrNone;
    
    // Check the code, note that IsCorrectEnhCode might change the stored code,
    // so we'll get the code after the call.    
    
    TBool isSuccessful = EFalse;
                            
    status = IsCorrectEnhCode( aCodeToCheck, aFlags );
    
    if ( status == KErrLocked )
        {
        // If the code is locked, no attempts count as failed or successful
        return status;
        }
    else if ( status == KErrNone )
        {
        // OK, successful authentication
        TSCPSecCode realCode;
		ret = GetCode( realCode );

        User::LeaveIfError( ret );
        
        aISACodeToReturn.Copy( realCode );
        isSuccessful = ETrue;
        }
    else // KSCPErrCodeBlockStarted or KErrAccessDenied
        {
        // Authentication attempt failed
        TSCPSecCode realCode;
        ret = GetCode( realCode );
        
        User::LeaveIfError( ret );
        
        TSCPSecCode fakeCode;
        do
            {
            // Create a fake code, which is between 10000-99999
            fakeCode.Zero();
            TUint32 code = Math::Random();
            code = code % 90000 + 10000;
            fakeCode.AppendNum( code );
            } while ( fakeCode.Compare( realCode ) == 0 );
        
        aISACodeToReturn.Copy( fakeCode );
        
        if ( status == KSCPErrCodeBlockStarted )
            {
        	status = KErrLocked; // Set this as the external error code
            }
        }
            
    // Send information about the authentication attempt to the plugins
    CSCPParamObject* inParams = CSCPParamObject::NewL();
    CleanupStack::PushL( inParams );
    inParams->Set( KSCPParamContext, KSCPContextQueryPsw );
    
    if ( isSuccessful )
        {
        // Authentication successful
        inParams->Set( KSCPParamStatus, KErrNone );
        }
    else
        {
        // Authentication failed
        inParams->Set( KSCPParamStatus, KErrAccessDenied );
        }
            
    CSCPParamObject* repParams = 
    iPluginManager->PostEvent( KSCPEventAuthenticationAttempted, *inParams );
    
    CleanupStack::PopAndDestroy( inParams );
      
    if ( repParams != NULL )            
        {
        // No status information here, just propagate the paramobject
        aRetParams = repParams; // pointer ownership changed
        }              
    
    return status;
    }
    
    
    
    
// ---------------------------------------------------------
// CSCPServer::CheckAndChangeEnhCodeL()
// Check the given code, and if correct, send it to the plugins
// for validation.
//
// Status : Approved
// ---------------------------------------------------------
//     
TInt CSCPServer::CheckAndChangeEnhCodeL( TDes& aOldPass, 
                                        TDes& aNewPass,
                                        CSCPParamObject*& aRetParams,
                                        TSCPSecCode& aNewDOSCode )
    {        
    Dprint(_L("CSCPServer::CheckAndChangeEnhCodeL >>"));
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TInt ret = KErrNone;
    
    ret = IsCorrectEnhCode( aOldPass, 0 ); 
    if ( ret == KErrNone )
        {
        // Authentication successful
        CSCPParamObject* inParams = CSCPParamObject::NewL();
        CleanupStack::PushL( inParams );
       
        inParams->Set( KSCPParamStatus, KErrNone ); 
        inParams->Set( KSCPParamContext, KSCPContextChangePsw );
        CSCPParamObject* repParams = 
            iPluginManager->PostEvent( KSCPEventAuthenticationAttempted, *inParams );
        
        if ( repParams != NULL ) {
            // Ignore the return params at this point
            delete repParams;
       	}  
		
        CleanupStack::PopAndDestroy(inParams);
        
        ret = ValidateLockcodeAgainstPoliciesL(aNewPass, aRetParams);
        
        if(ret != KErrNone) {
            Dprint(_L("[CSCPServer]-> ValidateLockcodeAgainstPoliciesL() failed in CheckAndChangeEnhCodeL() ret=%d"), ret);
            return ret;
        }


        if ( IsValidEnhCode( aNewPass ) )
            {
            ret = StoreEnhCode( aNewPass, &aNewDOSCode );
            }                        
        else
            {
            // Invalid code format! Should not happen at this point, but make sure.
            ret = KErrArgument; 
            }
            
        if ( ret == KErrNone )
            {
            CSCPParamObject* inParams = CSCPParamObject::NewL();
            CleanupStack::PushL( inParams );
            inParams->Set( KSCPParamPassword, aNewPass );
    
            CSCPParamObject* repParams = 
                iPluginManager->PostEvent( KSCPEventPasswordChanged, *inParams );
    
            CleanupStack::PopAndDestroy( inParams );
        
            if ( repParams != NULL )            
                {
                // Ignore the return params at this point
                delete repParams;
                }
            }
        }    
    else
        {
        if ( ( ret == KErrAccessDenied ) || ( ret == KSCPErrCodeBlockStarted ) )
            {
            // Authentication failed
            CSCPParamObject* inParams = CSCPParamObject::NewL();
            CleanupStack::PushL( inParams );
           
            inParams->Set( KSCPParamStatus, KErrAccessDenied );
            inParams->Set( KSCPParamContext, KSCPContextChangePsw );                   
            CSCPParamObject* repParams = 
                iPluginManager->PostEvent( KSCPEventAuthenticationAttempted, *inParams );
            
            CleanupStack::PopAndDestroy( inParams );
            
            if ( repParams != NULL )            
                {
                // No status information here, just propagate the paramobject                
                aRetParams = repParams; // pointer ownership changed
                } 
                
            if ( ret == KSCPErrCodeBlockStarted )
                {
        	    ret = KErrLocked; // Set this as the external error code
                }                            
            }
        }
    Dprint(_L("[CSCPServer]-> CheckAndChangeEnhCodeL ret=%d"), ret);           
    return ret;    
    }




// ---------------------------------------------------------
// CSCPServer::GetEventHandlerL()
// Return a pointer to the event handler object. 
//
// Status : Approved
// ---------------------------------------------------------
//      
MSCPPluginEventHandler* CSCPServer::GetEventHandlerL()
    {
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return NULL;
    }
    return iPluginEventHandler;
    }
    


// ---------------------------------------------------------
// CSCPServer::IsPasswordChangeAllowedL()
// Check from the plugins if the password is allowed to be changed.
//
// Status : Approved
// ---------------------------------------------------------
          
TInt CSCPServer::IsPasswordChangeAllowedL( CSCPParamObject*& aRetParams )
    {
    Dprint(_L("CSCPServer::IsPasswordChangeAllowedL >>"));
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    	return KErrNotSupported;
    }
    TInt ret = KErrNone;
    
    // Post the event to the plugins to find out if the password can
    // be changed
    
    CSCPParamObject* inParams = CSCPParamObject::NewL();
    CleanupStack::PushL( inParams );
   
    // No additional parameters
                      
    CSCPParamObject* repParams = 
        iPluginManager->PostEvent( KSCPEventPasswordChangeQuery, *inParams );
    
    CleanupStack::PopAndDestroy( inParams );
    
    if ( repParams != NULL )            
        {
        // Check status
        TInt status;
        if ( repParams->Get( KSCPParamStatus, status ) == KErrNone )
            {
            if ( status != KErrNone )
                {
                ret = status;
                }
            }
                       
        aRetParams = repParams; // pointer ownership changed
        }
    
    return ret;
    }
    
    
    // ---------------------------------------------------------
// CSCPServer::IsDeviceMemoryEncrypted
// Checks if phone memory is encrypted or not. 
// ---------------------------------------------------------
TBool CSCPServer::IsDeviceMemoryEncrypted()
    {
    Dprint(_L("CSCPServer::IsDeviceMemoryEncrypted >>"));
    
    TBool ret(EFalse);
    
    //First check if the feature is supported on device
    TRAPD(ferr, FeatureManager::InitializeLibL());
    if (ferr != KErrNone)
        {
        Dprint(_L("feature mgr initialization error, %d"), ferr);
        return EFalse;
        }
    ret = FeatureManager::FeatureSupported( KFeatureIdFfDeviceEncryptionFeature);
    FeatureManager::UnInitializeLib();
 
    //If feature is supported, check if any drive is encrypted.
    if (ret)
        {
        RLibrary library;   
        CDevEncSessionBase* devEncSession = NULL;
        TInt err = library.Load(KDevEncCommonUtils);	 
        
        if (err != KErrNone)
            {
            Dprint(_L("Error in finding the library... %d"), err);
            ret = EFalse;
            }
        else
        	{
		       TLibraryFunction entry = library.Lookup(1);
					 
	        if (!entry)
	            {
	            Dprint(_L("Error in loading the library..."));
	            ret = EFalse;
	            }
	        else
	        	{
		        devEncSession = (CDevEncSessionBase*) entry();
		        Dprint(_L("Library is found and loaded successfully..."));
		      	}
	        }

        if (!devEncSession)
            {
            Dprint(_L("Can't instantiate device encryption session.."));
            ret = EFalse;
            }
			  else
				  	{
						devEncSession->SetDrive( EDriveC );
		        TInt err = devEncSession->Connect();
		        if (err == KErrNone)
		            {
		            //Session with device encryption is established. Check if any drive is encrypted
		            TInt encStatus (KErrNone);
		            TInt err = devEncSession->DiskStatus( encStatus );
		            devEncSession->Close();
		            Dprint(_L("err = %d, encstatus = %d"), err, encStatus);
		            if (  err == KErrNone && encStatus != EDecrypted )
		                {
		                Dprint(_L("Memory is encrypted"));
		                ret = ETrue;
		                }
		            else
		                {
		                Dprint(_L("Memory is not encrypted"));
		                ret = EFalse;
		                }
		            }
		        else
		            {
		            Dprint(_L("Error %d while establishing connection with device encryption engine"), err);
		            ret = EFalse;
		            }
						}
				delete devEncSession; devEncSession = NULL;

        if (library.Handle())
    	     library.Close();
		    }
    
    Dprint(_L("CSCPServer::IsDeviceMemoryEncrypted, ret = %d <<"), ret);
    return ret;
    }
    

/**
* Sets the best policy for the parameters that are under conflict. This is part of changes done for MFE-requirement part3.
* @params   aID: The ID of the parameter that needs to be set
*           aValue: Value of the parameter identified in aID
*           aCalledIdentity: The UID of the called application/process
*
* @return TInt: KErrNone is successful, otherwise a system error code
*/
TInt CSCPServer :: SetBestPolicyL( TInt aID, const TDesC& aValue, TUint32 aCallerIdentity, CSCPParamDBController* aParamDB ) {
    Dprint( (_L("[CSCPServer]-> SetBestPolicyL() >>>")));
    TBool lFirstTime(EFalse);
    TInt32 lNumValue (-1);
    TInt32 lNumValDB (-1);
    TInt32 lRetStatus = KErrNone;

    switch(aID) {
        case RTerminalControl3rdPartySession :: EMaxTimeout:
        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
        case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
        case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeExpiration:
        case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
        case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
        case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
        case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
            break;
        case RTerminalControl3rdPartySession :: EPasscodeClearSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
        case RTerminalControl3rdPartySession :: EPasscodeAllowSpecific:
           return KErrNotSupported;
        default:
            // No best policy check required for parameters other than the above 19 (Only 19 of the current parameters conflict!)
            TUint16* ptr = const_cast<TUint16*>( aValue.Ptr());
            TPtr valBuf( ptr, aValue.Length(), aValue.Length());
            lRetStatus = iPluginEventHandler->GetParameters().Set( aID, valBuf);
            return lRetStatus;
    }
    
    TLex lex( aValue );
    // Convert the parameter value to its numeric equivalent
    if (lex.Val( lNumValue ) != KErrNone) {
        return KErrArgument;
    }

    if(iOverrideForCleanup) {
        Dprint( _L("[CSCPServer]-> SetBestPolicy() Overriding decision logic..."));
        lRetStatus = aParamDB->DropValuesL(aID);
    }
    else {
        // Fetch the previous value of the parameter from the private database
        switch(aID) {
            case RTerminalControl3rdPartySession :: EMaxTimeout:
            case RTerminalControl3rdPartySession :: EPasscodeMinLength:
            case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
            case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
            case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
            case RTerminalControl3rdPartySession :: EPasscodeExpiration:
            case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
            case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
            case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
            case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
            case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
            case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
            case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
            case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
            case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple: {
                Dprint( (_L("[CSCPServer]-> Fetching the value of parameter '%d' from private database..."), aID));

                TInt32 lAppID;

                lRetStatus = aParamDB->GetValueL(aID, lNumValDB, lAppID);

                switch(lRetStatus) {
                case KErrNotFound: {
                    TInt lLSValue(0);
                    lFirstTime = ETrue;

                    switch(aID) {
                    case RTerminalControl3rdPartySession :: EMaxTimeout:
                    case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
                    case RTerminalControl3rdPartySession :: EPasscodeExpiration:
                    case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
                    case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
                    case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
                    case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
                    case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
                        /* For these parameters, if it is the first time that the values are being
                           set then no decision is imposed regarding the best policy...
                         */
                        break;
                    default:
                        // Get the default values for the parameter...
                        lRetStatus = iPluginEventHandler->GetParameters().Get(aID, lLSValue);
                        break;
                    }

                    lNumValDB = lLSValue;
                    Dprint((_L("[CSCPServer]-> Unable to fetch the value of parameter '%d' from private database, value from internal tree=%d"), aID, lLSValue));
                }
                break;
                case KErrNone:
                    break;
                default:
                    Dprint((_L("[CSCPServer]-> Failed to fetch parameter '%d' from the private database. ErrCode=%d"), aID, lRetStatus));
                    return lRetStatus;
                }
                break;
            }
        }

        if(lNumValue == lNumValDB) {
            Dprint(_L("[CSCPServer]-> INFO: New parameter value and value from the database are the same. Overriding SetBestPolicy()..."));
            return KErrNone;
        }

        lRetStatus = KErrNone;

        // Decision code that verifies if policy is strongest
        switch(aID) {
        case RTerminalControl3rdPartySession :: EMaxTimeout:
        case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeExpiration:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
        case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
            if(!lFirstTime && ((lNumValue > lNumValDB) || (lNumValue == 0))) {
                lRetStatus = KErrArgument;
            }
            break;
        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
            if(!lFirstTime && (lNumValue < lNumValDB)) {
                lRetStatus = KErrArgument;
            }
            break;
        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
        case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
        case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
            if(lNumValue < lNumValDB) {
                lRetStatus = KErrArgument;
            }
            break;
        case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
        case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
            if(lNumValue == false && lNumValDB == true) {
                lRetStatus = KErrArgument;
            }
            break;
        }

        if(lRetStatus != KErrNone) {
            return KErrArgument;
        }

        // Update the best policy to the DB as well as the local storage tree
        Dprint((_L("[CSCPServer]-> Updating the value of parameter '%d' to private database...UID=%d"), aID, aCallerIdentity));

        lRetStatus = aParamDB->SetValueL(aID, lNumValue, aCallerIdentity);
    }

    Dprint(_L("[CSCPServer]-> lRetStatus after DB operation = %d "), lRetStatus);
    /* Update is NOT ATOMIC!!!
     * If the earlier aParamDB->SetValueL() was successful, there is a chance that
     * the config.ini and the DB are out of sync if the operation iConfiguration.WriteSetupL()
     * is not successful (disk space not enough etc) or when the WriteToFileL() in ParamObject  
     * from iPluginEventHandler->GetParameters() fails.
     * (WriteToFileL() is called in some destructor when the SCPServer terminates) 
    */
    if (lRetStatus == KErrNone) {
        switch (aID) {
        case RTerminalControl3rdPartySession :: EMaxTimeout:
            iConfiguration.iMaxTimeout = lNumValue;
            lRetStatus = iConfiguration.WriteSetupL();
            Dprint(_L("[CSCPServer]-> After setting EMaxTimeout lRetStatus = %d "), lRetStatus);
            break;
        default:
            TUint16* ptr = const_cast<TUint16*>(aValue.Ptr());
            TPtr valBuf(ptr, aValue.Length(), aValue.Length());
            lRetStatus = iPluginEventHandler->GetParameters().Set(aID, valBuf);
            Dprint(_L("[CSCPServer]-> After setting (%d ) lRetStatus = %d "), aID, lRetStatus);
            break;
        }
    }

    Dprint(_L("[CSCPServer]-> SetBestPolicyL() lRetStatus = %d <<<"), lRetStatus);
    return lRetStatus;
}

TInt CSCPServer :: PerformCleanupL(HBufC8* aAppIDBuffer, RArray<const TParamChange>& aChangeArray, RPointerArray<HBufC8>& aParamValArray) {
    Dprint( (_L("[CSCPServer]-> PerformCleanupL() >>>")));

    if(!aAppIDBuffer) {
        return KErrArgument;
    }
    
    TInt32 lAppCount = aAppIDBuffer->Length();
    
    if(lAppCount < 1) {
        return KErrArgument;
    }
    
    lAppCount /= sizeof(TInt32);
    
    TPtr8 bufPtr = aAppIDBuffer->Des();
    
    TInt lStatus(KErrNone);
    CSCPParamDBController* lParamDB = CSCPParamDBController :: NewLC();
    
    RDesReadStream lBufReadStream(bufPtr);
    CleanupClosePushL(lBufReadStream);
    
    Dprint(_L("[CSCPServer]-> INFO: Total items received for cleanup is %d"), lAppCount);
    
    RArray <TInt> lParamIds;
    CleanupClosePushL(lParamIds);
    
    TBool lSubOpsFailed = EFalse;
    HBufC* lDefValueBuf = HBufC :: NewLC(20);
    
    // Loop through each of the byte packed application ID's specified in aAppIDBuffer and clean them up iteratively
    for(TInt i=0; i < lAppCount; i++) {
        TInt32 lAppID = lBufReadStream.ReadInt32L();        
        Dprint((_L("[CSCPServer]-> Initiating cleanup on %d  "), lAppID));


        lParamIds.Reset();
        lStatus = lParamDB->ListParamsUsedByAppL(lParamIds, lAppID);
        
        Dprint(_L("[CSCPServer]-> Status of ListParamsUsedByAppL() = %d"), lStatus);
        
       	TInt lCount = lParamIds.Count();
       	
        Dprint(_L("[CSCPServer]-> lCount=%d"), lCount);
       	
       	// Make sure that LockCodeMinChangeTolerance is always cleanedup before LockCodeHistoryBuffer
       	if(lCount > 1) {
            Dprint(_L("[CSCPServer]-> Updating the cleanup order for EPasscodeMinChangeTolerance and EPasscodeHistoryBuffer if needed..."));
            TInt lHistBuffIndex = -1;
            TInt lMinTolIndex = -1;
            
            for(TInt i=0; i < lCount; i++) {
                if(lHistBuffIndex >= 0 && lMinTolIndex >= 0) {
                    // If both the indexes have been found, stop the search
                    break;
                }
                
                switch(lParamIds[i]) {
                case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
                    lMinTolIndex = i;
                    break;
                case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
                    lHistBuffIndex = i;
                    break;
                }
            }
            
            // This step is required only if both the parameters are marked for cleanup
            if(lHistBuffIndex >= 0 && lMinTolIndex >= 0) {
                /* Switch the cleanup order of the two parameters
                 * This step is needed because ListParamsUsedByAppL() always returns the parameters
                 * in Ascending order and because LockCodeMinChangeTolerance is always required to be 
                 * cleanedup before LockCodeHistoryBuffer
                */
                lParamIds[lHistBuffIndex] = RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance;
                lParamIds[lMinTolIndex] = RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer;
                Dprint(_L("[CSCPServer]-> Switching the cleanup order of EPasscodeHistoryBuffer and EPasscodeMinChangeTolerance"));
                Dprint(_L("[CSCPServer]-> Old Index of EPasscodeHistoryBuffer=%d"), lHistBuffIndex);
                Dprint(_L("[CSCPServer]-> Old Index of EPasscodeMinChangeTolerance=%d"), lMinTolIndex);
            }
       	}

        for(TInt j=0; j < lCount; j++) {
            TInt lCurrParamID = lParamIds[j];            
            lDefValueBuf->Des().Zero();
            lDefValueBuf->Des().Format(_L("%d "), 0);
            // Initialize the default values here...
            switch(lCurrParamID) {
                case RTerminalControl3rdPartySession :: EMaxTimeout:
                    // lDefValueBuf already has the default value, 0 initialized...
                    lCurrParamID = ESCPMaxAutolockPeriod;
                    break;
                case RTerminalControl3rdPartySession :: EPasscodeMinLength:
                    lDefValueBuf->Des().Format(_L("%d"), 4);
                    break;
                case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
                    lDefValueBuf->Des().Format(_L("%d"), 255);
                    break;                    
                default:
                    // lDefValueBuf already has the default value, 0 initialized...
                    break;
            }
    
            switch(lCurrParamID) {
                case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific: {
                    RPointerArray<HBufC> lDesArr;
                    CleanupClosePushL(lDesArr);
                    
                    lStatus = lParamDB->GetValuesL(lCurrParamID, lDesArr, lAppID);
                    
                    if(KErrNone == lStatus) {
                        iOverrideForCleanup = ETrue;
                    TInt lDesCount = lDesArr.Count();
                        
                        for(TInt k=0; k < lDesCount; k++) {
                            TRAP(lStatus, lStatus = SetParameterValueL(lCurrParamID, lDesArr[k]->Des(), lAppID));
                            if(KErrNone != lStatus) {
                                Dprint(_L("[CSCPServer]-> ERROR: Unable to cleanup parameter %d error %d"), lParamIds[j], lStatus);
                                lSubOpsFailed = ETrue;
                            }
                        }
                    
                    iOverrideForCleanup = EFalse;
                    }
                    
                    lDesArr.ResetAndDestroy();
                    CleanupStack :: PopAndDestroy(1); // lDesArray
                }
                break;
                default: {
                    iOverrideForCleanup = ETrue;
                    TRAP(lStatus, lStatus = SetParameterValueL(lCurrParamID, lDefValueBuf->Des(), lAppID));
                    iOverrideForCleanup = EFalse;
    
                    if(KErrNone == lStatus) {
                        HBufC8* lTmpBuffer = HBufC8 :: NewL(lDefValueBuf->Length());
                        lTmpBuffer->Des().Copy(lDefValueBuf->Des());
                        
                        const TParamChange lChange(lParamIds[j], lTmpBuffer->Des());
                        aChangeArray.AppendL(lChange);
                        
                        // No need to destroy lTmpBuffer, it will be cleaned up by the caller (on cleanup of aParamValArray)
                        aParamValArray.AppendL(lTmpBuffer);
                    }
                }
            }
            
            if(KErrNone != lStatus) {
                Dprint(_L("[CSCPServer]-> ERROR: Unable to cleanup parameter %d error %d"), lParamIds[j], lStatus);
                lSubOpsFailed = ETrue;
            }
        }
    }

    Dprint( (_L("[CSCPServer]-> PerformCleanupL() <<<")));
    CleanupStack :: PopAndDestroy(4); // lParamIds lParamDB lBufReadStream lDefValueBuf
    return (lSubOpsFailed) ? KErrGeneral : KErrNone;
}

void CSCPServer :: GetPoliciesL(HBufC8* aAppIDBuffer, TUint32 aCallerIdentity) {
    TInt lValue;
    TInt lRet = KErrNone;
    TBuf<25> lParamValBuf;
    TPtr8 lBufPtr = aAppIDBuffer->Des();
    RDesWriteStream lWriteStream(lBufPtr);
    CleanupClosePushL(lWriteStream);
    
    lRet = GetAutolockPeriodL(lValue);
    
    if(lRet != KErrNone) {
        User :: Leave(lRet);
    }
    
    Dprint((_L("[CSCPServer]-> appending AutoLockPeriod value=%d"), lValue));
    lWriteStream.WriteInt32L(lValue);    
    
    Dprint((_L("[CSCPServer]-> appending MaxAutoLockPeriod value=%d"), iConfiguration.iMaxTimeout));
    lWriteStream.WriteInt32L(iConfiguration.iMaxTimeout);
    
    if(FeatureManager :: FeatureSupported(KFeatureIdSapDeviceLockEnhancements)) {
        Dprint(_L("[CSCPServer]-> (FeatureManager :: FeatureSupported() complete. Fetching values now..."));
        /* Fetch parameters starting from RTerminalControl3rdPartySession :: EPasscodeMinLength to 
         * RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval
         */
        TInt lPID = RTerminalControl3rdPartySession :: EPasscodeMinLength;
        
        for(; lPID <= RTerminalControl3rdPartySession :: EPasscodeDisallowSimple; lPID++) {
            switch(lPID) {
                default:
                    break;
                case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
                case RTerminalControl3rdPartySession :: EPasscodeAllowSpecific:
                case RTerminalControl3rdPartySession :: EPasscodeClearSpecificStrings:
                    // No need to fetch these three parameters
                    continue;
                case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
                    Dprint(_L("[CSCPServer]-> appending EPasscodeDisallowSpecific value..."));
                    // Get on EPasscodeDisallowSpecific returning -1 instead
                    lWriteStream.WriteInt32L(-1);
                    continue;
                case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
                    Dprint(_L("[CSCPServer]-> appending EPasscodeDisallowSimple value..."));
                    lWriteStream.WriteInt32L(1);
                    lWriteStream.WriteInt32L(1);
                    continue;
            }
            
            //lRet = GetParameterValueL(lPID, lParamValBuf, aCallerIdentity);
            lRet = iPluginEventHandler->GetParameters().Get(lPID, lValue);
            
            switch(lRet) {
                
                
                case KErrNotFound: {
                    switch(lPID) {
                        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
                            lValue = KSCPPasscodeMinLength;
                            break;
                        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
                            lValue = KSCPPasscodeMaxLength;
                            break;
                        default:
                            lValue = 0;
                            break;
                    }
                }
                break;
                case KErrNone: {
//                    TLex lLex(lParamValBuf);
//                    lRet = lLex.Val(lValue);                    
//                    User :: LeaveIfError(lRet);
                }
                break;
                default:
                    User :: Leave(lRet);
            }
            
            Dprint((_L("[CSCPServer]-> appending value for lPID=%d"), lPID));
            Dprint((_L("[CSCPServer]-> lValue=%d"), lValue));            
            
            switch(lPID) {
                default:
                    lWriteStream.WriteInt32L(lValue);
                    break;
                case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
                    lWriteStream.WriteInt32L(1);
                    lWriteStream.WriteInt32L(1);
                    break;
            }
        }
    }
    else {
        Dprint(_L("[CSCPServer]-> (FeatureManager :: FeatureSupported() failed!!..."));
    }
    
    CleanupStack :: PopAndDestroy(1); // lWriteStream
}

TInt CSCPServer :: ValidateLockcodeAgainstPoliciesL(TDes& aLockcode, CSCPParamObject*& aRetParams) {
    Dprint( (_L("[CSCPServer]-> ValidateLockcodeAgainstPoliciesL() >>>")));
    
    TInt ret = KErrNone;
    CSCPParamObject* inParams = CSCPParamObject :: NewL();
    inParams->Set(KSCPParamPassword, aLockcode);
    CSCPParamObject* repParams = iPluginManager->PostEvent(KSCPEventValidate, *inParams);
    delete inParams;
    
    if (repParams != NULL) {
        // Check if the validation failed
        TInt status;
        
        if (repParams->Get(KSCPParamStatus, status) == KErrNone) {
            if (status != KErrNone) {
                ret = status;
            }
        }
        
        aRetParams = repParams; // pointer ownership changed
    }
    
    Dprint( (_L("[CSCPServer]-> ValidateLockcodeAgainstPoliciesL() <<<")));
    return ret;
}

//#endif //  __SAP_DEVICE_LOCK_ENHANCEMENTS
// <<<< ********************** NEW FEATURES ********************

// ================= OTHER EXPORTED FUNCTIONS ==============

 // __ARMI__ /

// ---------------------------------------------------------
// E32Main EXE entry point
// Returns: TInt: Process exit value
//          
// Status : Approved
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return CSCPServer::ThreadFunction(NULL);
    }



//  End of File  

