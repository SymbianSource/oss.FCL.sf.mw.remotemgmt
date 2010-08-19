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
#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <SCPParamObject.h>
#include <SCPServerInterface.h>
#include <scptimestamppluginlang.rsg>
#include "SCP_IDs.h"
#include <bautils.h>
#include <hal.h>
#include <AknGlobalNote.h>
#include <AknGlobalConfirmationQuery.h>
// For wipe
#include <starterclient.h>
#include <sysutil.h>
#include <syslangutil.h>
#include <rfsClient.h>
#include "DMUtilClient.h"

#include "SCPTimestampPlugin.h"
#include <featmgr.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <pathinfo.h>
#include <f32file.h>
#endif //RD_MULTIPLE_DRIVEs
// CONSTANTS

// ============================= LOCAL FUNCTIONS  =============================



// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::NewL
// Two-phased contructor
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPTimestampPlugin* CSCPTimestampPlugin::NewL()
    {
	CSCPTimestampPlugin* self = new ( ELeave ) CSCPTimestampPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
    Dprint ( ( _L( "( 0x%x ) CSCPPatternPlugin::NewL()" ), self ) );

    return self;
    }

// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::CSCPTimestampPlugin
// Constructor
//
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPTimestampPlugin::CSCPTimestampPlugin()
    : iEventHandler( NULL ),
      iConfiguration( NULL ),
      iResOpen( EFalse )
    {
    Dprint ( ( _L( "CSCPTimestampPlugin::CSCPTimestampPlugin()" ) ) );
    return;
    }

// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::ConstructL
// 2nd phase constructor
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPTimestampPlugin::ConstructL()
    {   
    FeatureManager::InitializeLibL();
		if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
		{
    	FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
  	}
   	FeatureManager::UnInitializeLib();  
    Dprint ( ( _L( "CSCPTimestampPlugin::ConstructL()" ) ) );    
    
    return;
    }


// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::~CSCPTimestampPlugin
// Destructor
//
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPTimestampPlugin::~CSCPTimestampPlugin()
    {
    Dprint( ( _L( "--> CSCPTimestampPlugin::~CSCPTimestampPlugin()" ) ) );
        
    iResFile.Close();
    
    if ( iConfiguration != NULL )
        {
        delete iConfiguration;
        iConfiguration = NULL;
        }

    Dprint( ( _L( "<-- CSCPTimestampPlugin::~CSCPTimestampPlugin()" ) ) );
    return;
    }
    
    
    
// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPTimestampPlugin :: HandleEventL( TInt aID, CSCPParamObject& aParam, CSCPParamObject& aOutParam )
	{
	Dprint ( ( _L( "CSCPTimestampPlugin::HandleEvent()" ) ) );		
	
	// Get our current functional configuration
	if ( iEventHandler->GetParameters().Get( 
	        RTerminalControl3rdPartySession::EPasscodeExpiration, iExpiration ) != KErrNone )
	    {
	    iExpiration = 0;
	    }	    
	if ( iEventHandler->GetParameters().Get( 
	        RTerminalControl3rdPartySession::EPasscodeMinChangeInterval, iMinInterval) != KErrNone )
	    {
	    iMinInterval = 0;
	    }
	if ( iEventHandler->GetParameters().Get( 
	        RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance, iMinTolerance) != KErrNone )
	    {
	    iMinTolerance = 0;
	    }
	if ( iEventHandler->GetParameters().Get( 
	        RTerminalControl3rdPartySession::EPasscodeMaxAttempts, iMaxAttempts ) != KErrNone )
	    {
	    iMaxAttempts = 0;
	    }	    	    
	Dprint( ( _L( "CSCPPatternPlugin::IsChangeAllowedL(): iExpiration :%d, iExpiration :%d, iMinTolerance :%d, iMaxAttempts :%d"), iExpiration,iMinInterval,iMinTolerance,iMaxAttempts ) ); 
	Dprint( ( _L( "CSCPPatternPlugin::IsChangeAllowedL(): aID is :%d"), aID) );
	switch ( aID )
	    {	    	    
	    case ( KSCPEventPasswordChangeQuery ):
	        {
	        if ( iMinInterval > 0 )
                {   
	            // Ignore errors, the plugin will stay silent on error
	            TRAP_IGNORE( IsChangeAllowedL( aParam, aOutParam ) );
                }
	        break;
	        }
	    
	    case ( KSCPEventConfigurationQuery ):
	        {
	        
	        TInt paramID;	            	            
	            	            
	        if ( aParam.Get( KSCPParamID, paramID ) == KErrNone ) 
	            {	                
	            ConfigurationQuery( paramID, aParam, aOutParam );
	            }
	                	                	            
	        break;
	        }

	    case ( KSCPEventPasswordChanged ):
	        {	            
	        PasswordChanged( aParam, aOutParam );
	        break;
	        }

        case ( KSCPEventAuthenticationAttempted ):
            {
            TInt authStatus;
            if ( aParam.Get( KSCPParamStatus, authStatus ) == KErrNone ) // Ignore errors
                {
                TBool isSuccess = ( authStatus == KErrNone );
                // No return value required in any case
                AuthenticationAttempt( isSuccess, aParam, aOutParam );
                }	            
           	                
            break;
            }
            
        case ( KSCPEventReset ):
              {
              // Reset the configuration for this plugin.
              if ( ReadConfiguration() == KErrNone )
                {
                iConfiguration->Reset();
                WriteConfiguration();
                }
              
              break;
              }            
            
        default:
            // No implementation required, we're not interested in other events
        break;	            	            
	    }	    	    
	
    // The caller will own this pointer from now on   
	}

// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPTimestampPlugin::SetEventHandler( MSCPPluginEventHandler* aHandler )
	{
	Dprint( ( _L( "CSCPPatternPlugin::SetEventHandler()" ) ) ) ;
	iEventHandler = aHandler;
	
	iFsSession = &(iEventHandler->GetFsSession());	
	}	


// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::IsChangeAllowed()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
// 
void CSCPTimestampPlugin::IsChangeAllowedL( CSCPParamObject& aParam, CSCPParamObject& aRetParams )
    {  
    (void)aParam;
                          
    Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL()") ));                      
    TInt ret = KErrNone;
    
    if ( ReadConfiguration() != KErrNone )
        {
        Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL() ReadConfiguration() != KErrNone RETURNING") ));
        return; // Someting wrong
        }    
    
    // Check if the change is within the interval
    TInt ret2 = IsAfter( KSCPIntervalStartTime, iMinInterval, KSCPTypeHours );
            
    if ( ret2 == KSCPIsAfter )
        {
        // Interval exceeded, change OK
        Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL() KSCPIsAfter") ));
        ret = KErrNone;                    
        }
    else if ( ret2 == KSCPIsNotAfter )
        {
        // Change within the interval, check the tolerance
        Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL() KSCPIsNotAfter") )); 
        TInt tolerance = 0;
        iConfiguration->Get( KSCPUsedTolerance, tolerance ); // ignore errors
        Dprint( ( _L( "CSCPPatternPlugin::IsChangeAllowedL(): tolerance get: %d"), tolerance ) );                
        Dprint( ( _L( "CSCPPatternPlugin::IsChangeAllowedL(): iMinTolerance : %d"), iMinTolerance ) );                
        if ( tolerance >= iMinTolerance )
            {
            ret = KErrSCPCodeChangeNotAllowed;
            Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL() KErrSCPCodeChangeNotAllowed") )); 
            }
        
        // Used tolerance will increment in passwordChanged                
        }
    else 
        {
        // Error, new interval will start in passwordChanged.
        Dprint( (_L("CSCPTimestampPlugin::IsChangeAllowedL() Error, new interval will start in passwordChanged") )); 
        }                                                                     
    
    if ( ret == KErrSCPCodeChangeNotAllowed )
        {
        Dprint ( ( _L( "EDeviceLockMinChangeTolerance Failed" ) ) );
        aRetParams.AddtoFailedPolices(EDeviceLockMinChangeTolerance);
        aRetParams.Set( KSCPParamStatus, KErrSCPCodeChangeNotAllowed );
        }
    }


// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::PasswordChanged()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPTimestampPlugin::PasswordChanged( CSCPParamObject& aParam, CSCPParamObject& aRetParams )
    {
    (void)aParam;
    
    Dprint( (_L("CSCPTimestampPlugin::PasswordChanged()") ));                      
    TInt err = ReadConfiguration();
    if ( err == KErrNone )
        {
        // Check if immediate expiration is set
        Dprint( (_L("CSCPTimestampPlugin::PasswordChanged() : ReadConfiguration = KErrNone") ));                      
        TInt expireNow = 0;
        if ( iConfiguration->Get( KSCPExpireOnNextCall, expireNow ) == KErrNone )
            {
            iConfiguration->Unset( KSCPExpireOnNextCall );
            }                                
        
        TBuf<KSCPMaxInt64Length> timeBuf; 
        timeBuf.Zero();       
        
        // Fetch the current time and fill the buffer
        TTime curTime;
        curTime.UniversalTime();
        
        timeBuf.AppendNum( curTime.Int64() );
        
        if ( iExpiration > 0 )
            {
            // Set the last time the password was changed, for expiration
            iConfiguration->Set( KSCPLastChangeTime, timeBuf );
            }        
        Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): iMinInterval: %d"), iMinInterval ) );
        if ( iMinInterval > 0 )
            {
            TInt ret = IsAfter( KSCPIntervalStartTime, iMinInterval, KSCPTypeHours );
            if ( ret == KSCPIsAfter )
                {                                                        
                // Interval exceeded, start a new interval from here
                Dprint( (_L("CSCPTimestampPlugin::PasswordChanged() KSCPIsAfter") ));    
                iConfiguration->Set( KSCPIntervalStartTime, timeBuf );
                iConfiguration->Set( KSCPUsedTolerance, 1 );
                }
            else if ( ret == KSCPIsNotAfter )
                {
                // Change within the interval, increment the used tolerance
                Dprint( (_L("CSCPTimestampPlugin::PasswordChanged() KSCPIsNotAfter") ));
                TInt tolerance = 0;
                iConfiguration->Get( KSCPUsedTolerance, tolerance ); //ignore errors, default to 0
                Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): tolerance get: %d"), tolerance ) );
                tolerance++;        
                iConfiguration->Set( KSCPUsedTolerance, tolerance );                
                Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): tolerance set: %d"), tolerance ) );                
                }                                                                
            else // error
                {
                // No time set, start a new interval from here
                Dprint( (_L("CSCPTimestampPlugin::PasswordChanged() error") ));
                iConfiguration->Set( KSCPIntervalStartTime, timeBuf );
                TInt tolerance = 0;
                iConfiguration->Get( KSCPUsedTolerance, tolerance ); //ignore errors, default to 0
                Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): tolerance get: %d"), tolerance ) );
                tolerance++;        
                iConfiguration->Set( KSCPUsedTolerance, tolerance );
                Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): tolerance set: %d"), tolerance ) );  
                }                       
            }        
        
        WriteConfiguration();
        }
        else
        {
            Dprint( (_L("CSCPTimestampPlugin::PasswordChanged() ReadConfiguration() != KErrNone") ));                      
            Dprint( ( _L( "CSCPPatternPlugin::PasswordChanged(): ReadConfiguration() = %d"), err ) );
        }
    }


// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::AuthenticationAttempt()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPTimestampPlugin :: AuthenticationAttempt( TBool aIsSuccessful, 
                                                 CSCPParamObject& aParam,
                                                 CSCPParamObject& aRetParams )
    {
    Dprint( (_L("CSCPTimestampPlugin::AuthenticationAttempt") )); 
    if ( ReadConfiguration() != KErrNone )
        {
        return;
        }
        
    // Check if immediate expiration is set
    TInt expireNow = 0;
    iConfiguration->Get( KSCPExpireOnNextCall, expireNow ); // ignore errors  
    Dprint( ( _L( "CSCPPatternPlugin::AuthenticationAttempt(): expireNow = %d"), expireNow ) );
    if ( ( iExpiration == 0 ) && ( iMaxAttempts == 0) && ( expireNow == 0 ) )
        {
        return; // We have no interest in this call
        }                
    
    if ( aIsSuccessful )
        {
        TRAP_IGNORE( SuccessfulAuthenticationL( aParam, aRetParams ) );
        }
    else
        {
        // Failed authentication attempt
        if ( iMaxAttempts > 0 )
            {
            TInt failedCount = 0;
            iConfiguration->Get( KSCPFailedAttempts, failedCount ); // ignore errors
            failedCount++;                        
        
            if ( failedCount == iMaxAttempts - 1 )
                {
                Dprint ( ( _L( "EDeviceLockAllowedMaxAtempts Failed" ) ) );
                aRetParams.AddtoFailedPolices(EDeviceLockAllowedMaxAtempts);
                }
            else if ( failedCount >= iMaxAttempts )
                {
                // Try to wipe the device
                TRAPD( err, WipeDeviceL( aRetParams ) );
                
                if ( err != KErrNone )
                    {
                    Dprint( ( _L( "CSCPPatternPlugin::\
                        AuthenticationAttempt(): Wipe FAILED :%d"), err ) );
                    }
                }
            
            iConfiguration->Set( KSCPFailedAttempts, failedCount );
            }
        }
        
    WriteConfiguration();        
    }


// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::SuccessfulAuthentication()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPTimestampPlugin::SuccessfulAuthenticationL( CSCPParamObject& aParam,
                                                    CSCPParamObject& aRetParams )
    {
    Dprint( (_L("CSCPTimestampPlugin::IsAfter()..enter") ));
    // Reset the failed attempts -counter
    if ( iMaxAttempts > 0 )
        {
        iConfiguration->Set( KSCPFailedAttempts, 0 );
        }
                    
    // Check context, if the user is already changing the password, don't force it again.
    TInt context = 0;
    aParam.Get( KSCPParamContext, context ); // ignore errors

    // Check if immediate expiration is set
    TInt expireNow = 0;
    iConfiguration->Get( KSCPExpireOnNextCall, expireNow ); // ignore errors    
    Dprint( ( _L( "CSCPPatternPlugin::SuccessfulAuthenticationL(): expireNow = %d, iExpiration =%d"), expireNow,iExpiration ) );
    // Check if the code should be changed now    
    if ( ( context != KSCPContextChangePsw ) && 
         ( ( iExpiration > 0 ) ||  ( expireNow ) ) )
        {                    
        if ( ( IsAfter( KSCPLastChangeTime, iExpiration, KSCPTypeDays ) == KSCPIsAfter ) ||
             ( expireNow ) )
            {
            Dprint ( ( _L( "EDeviceLockPasscodeExpiration Failed" ) ) );
                aRetParams.AddtoFailedPolices(EDeviceLockPasscodeExpiration);
                aRetParams.Set( KSCPParamStatus, KErrSCPInvalidCode );
            }
        }
    Dprint( (_L("CSCPTimestampPlugin::IsAfter()..exit") ));
    }

// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::IsAfter()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPTimestampPlugin::IsAfter( TInt aConfID, TInt aInterval, TInt aIntType )
    {
    TInt ret;
    Dprint( (_L("CSCPTimestampPlugin::IsAfter()") ));
    TTime curTime;
    curTime.UniversalTime();        
    
    TBuf<KSCPMaxInt64Length> savedTimeBuf;
    TInt64 savedTime;
    Dprint( ( _L( "CSCPPatternPlugin::IsAfter: aConfID value is  :%d"), aConfID ) );
    ret = iConfiguration->Get( aConfID, savedTimeBuf );
    if ( ret == KErrNone )
        {    
        Dprint( (_L("CSCPTimestampPlugin::IsAfter() iConfiguration->Get == KErrNone") ));    
        TLex lex( savedTimeBuf );
        ret = lex.Val( savedTime );
        if ( ret == KErrNone )
            {
            Dprint( (_L("CSCPTimestampPlugin::IsAfter() lex.Val( savedTime ) == KErrNone") ));
            TTime configTime( savedTime );
            switch ( aIntType )
                {
                case ( KSCPTypeMinutes ):
                    {
                    Dprint( (_L("CSCPTimestampPlugin::IsAfter() KSCPTypeMinutes") ));
                    TTimeIntervalMinutes interval = aInterval;
                    configTime += interval;
                    break;
                    }
                    
                case ( KSCPTypeHours ):
                    {
                    Dprint( (_L("CSCPTimestampPlugin::IsAfter() KSCPTypeHours") ));
                    TTimeIntervalHours interval = aInterval;
                    configTime += interval;                    
                    break;
                    }  
                    
                case ( KSCPTypeDays ):
                    {
                    Dprint( (_L("CSCPTimestampPlugin::IsAfter() KSCPTypeDays") ));
                    TTimeIntervalDays interval = aInterval;
                    configTime += interval;                    
                    break;
                    }                                         
                }

            if ( curTime > configTime )
                {
                ret = KSCPIsAfter;
                Dprint( (_L("CSCPTimestampPlugin::IsAfter() ret = KSCPIsAfter") ));
                }
            else
                {
                ret = KSCPIsNotAfter;
                Dprint( (_L("CSCPTimestampPlugin::IsAfter() ret = KSCPIsNotAfter") ));
                }
            }
        }
    Dprint( ( _L( "CSCPPatternPlugin::IsAfter: Retutn value is  :%d"), ret ) );
    return ret;
    }
    
// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::ConfigurationQuery()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPTimestampPlugin::ConfigurationQuery(  TInt aParamID, 
                                               CSCPParamObject& aParam, 
                                               CSCPParamObject& aRetParams )
    {
    // First check if this is our ID    
    if ( ( aParamID == RTerminalControl3rdPartySession::EPasscodeExpiration ) ||
         ( aParamID == RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance ) ||
         ( aParamID == RTerminalControl3rdPartySession::EPasscodeMinChangeInterval ) ||
         ( aParamID == RTerminalControl3rdPartySession::EPasscodeMaxAttempts ) )
        {
        // OK, get the value, the parameters above should be TInt:s
        TInt ret = KErrNone;
        
        if ( ReadConfiguration() != KErrNone )
            {
            return;
            }        
        
        TInt paramValue;
        TBool setPrivateStorage = EFalse;        
        if ( aParam.Get( KSCPParamValue, paramValue ) == KErrNone )
            {
            switch ( aParamID )
                {
                case ( RTerminalControl3rdPartySession::EPasscodeExpiration ):
                    {
                    if ( ( paramValue < KSCPExpireImmediately ) || 
                         ( paramValue > KSCPMaxExpiration) )
                        {
                        ret = KErrArgument;
                        }
                    else if ( paramValue == 0 )
                        {
                        // Feature disabled, remove the stored data
                        iConfiguration->Unset( KSCPLastChangeTime );
                        }                        
                    else if ( paramValue == KSCPExpireImmediately )
                        {
                        // Special case, expire immediately, but retain the previous timestamp
                        iConfiguration->Set( KSCPExpireOnNextCall,  ETrue );
                        // Tell the server that this param is stored in our private storage
                        // (note that in KSCPExpireImmediately-case only..)
                        setPrivateStorage = ETrue;
                        }
                    else if ( iExpiration == 0 )
                        {
                        // Check activated, start the expiration from this time
                        Dprint ( ( _L( "CSCPTimestampPlugin::ConfigurationQuery(): Check activated" ) ) );
                        TTime curTime;
                        curTime.UniversalTime();
                        
                        TBuf<KSCPMaxInt64Length> timeBuf;
                        timeBuf.Zero();
                        timeBuf.AppendNum( curTime.Int64() );
        
                        // Set this time as the starting point for expiration
                        iConfiguration->Set( KSCPLastChangeTime, timeBuf );
                        }
                        
                    break;
                    }
                        
                case ( RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance ):
                    {
                    if ( ( paramValue < 0 ) || ( paramValue > KSCPMaxTolerance ) )
                        {
                        ret = KErrArgument;
                        }                         

                    break;
                    }
                        
                case ( RTerminalControl3rdPartySession::EPasscodeMinChangeInterval ):
                    {
                    if ( ( paramValue < 0 ) || ( paramValue > KSCPMaxInterval ) )
                        {
                        ret = KErrArgument;
                        }
                   else if ( paramValue == 0 )
                        {
                        // Feature disabled, remove the stored data
                        iConfiguration->Unset( KSCPIntervalStartTime );
                        iConfiguration->Unset( KSCPUsedTolerance );
                        }                        
                        
                    break;
                    }
                        
                case ( RTerminalControl3rdPartySession::EPasscodeMaxAttempts ):
                    {
                    if ( ( ( paramValue < KSCPMinAttempts ) || 
                           ( paramValue > KSCPMaxAttempts ) ) &&
                         ( paramValue != 0 ) )
                        {
                        ret = KErrArgument;
                        }
                    else if ( iMaxAttempts != paramValue )
                        {
                        // Remove the stored data, max attempts always restarts
                        iConfiguration->Unset( KSCPFailedAttempts );                        
                        }
                        
                    break;
                    }
                    
                default:
                    ret = KErrGeneral; // Something is seriously wrong if this is executed
                    break;                        
                } 
                
            WriteConfiguration();                       
            }
        else
            {
            // Something wrong, and this is our parameter. Signal an error
            ret = KErrArgument;
            }       

        aRetParams.Set( KSCPParamStatus, ret );
        
        if ( setPrivateStorage )
            {
            aRetParams.Set( KSCPParamStorage, KSCPStoragePrivate );
            } 
        }
    }
    
// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::WipeDeviceL()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//     
void CSCPTimestampPlugin::WipeDeviceL( CSCPParamObject& aRetParams )
    {
    // First try to format other local drives than C:
    RRfsClient rfsClient;
    
    TInt ret = rfsClient.Connect();
    
    if ( ret == KErrNone )
        {        
        CleanupClosePushL( rfsClient );    
 #ifndef RD_MULTIPLE_DRIVE
        
        TDriveList list;
        iFsSession->DriveList( list );

        TInt   i;
        TUint8 c = 'A';

        // Loop through the drives
        for( i = 0; i < KMaxDrives &&  c < 'I'; i++, c++ )
            {
            if( (list[i] != 0) && (c != 'C') )
                {
                TBuf<KSCPFormatScriptMaxLen> formatScript;
                formatScript.Format(KSCPFormatScript, (TUint)c );
                Dprint( ( _L( "CSCPPatternPlugin::WipeDeviceL(): Formatting %c:"), c ) );
                    
                ret = rfsClient.RunScriptInDescriptor(formatScript);
                
                if ( ret != KErrNone )
                    {
                    Dprint( ( _L( "CSCPPatternPlugin::WipeDeviceL():\
                     FAILED to format %c: %d"), c, ret ) );
                    }
                }
            }
#else 
		RFs fs;
    	User::LeaveIfError(fs.Connect());
   		CleanupClosePushL(fs);
		TDriveList driveList;
 		TInt driveCount;
 		
 		//Get list of drives;
  		User::LeaveIfError( DriveInfo::GetUserVisibleDrives( fs, driveList, driveCount ) );
  		
  		TInt driveListLen( driveList.Length() ); 
  		TInt phoneMemoryDrive( EDriveC );
   		RFs::CharToDrive( PathInfo::PhoneMemoryRootPath()[0], phoneMemoryDrive );
  		TDriveNumber driveNumber;
  		
  		//Loop through all drives except phonem memory drive
  		for (TInt i(0); i<driveListLen; ++i)
  		{
  			if (driveList[i])
  			{
	  			driveNumber = TDriveNumber(i);
	  			if (phoneMemoryDrive != driveNumber)
	  			{
	  				TBuf<KSCPFormatScriptMaxLen> formatScript;
	  				TChar driveLetter;
	  				RFs::DriveToChar(i,driveLetter);
	                formatScript.Format(KSCPFormatScript, (TUint)driveLetter );		                    
	                ret = rfsClient.RunScriptInDescriptor(formatScript);
	  			}
  			}
  		}
  		
  		CleanupStack::PopAndDestroy();
#endif  //RD_MULTIPLE_DRIVE   
        // In case of deep level RFS, set the default language code
        // here, before RFS reboot.
        TInt language( 0 );

        // If default language is not found, we reset anyway
        if ( SysLangUtil::GetDefaultLanguage( language ) == KErrNone )
            {
            HAL::Set( HALData::ELanguageIndex, language );
            }
        
        // Send NVD_SET_DEFAULT_REQ here, before reboot. Ignore errors.        

        CleanupStack::PopAndDestroy( &rfsClient );
        }       

    // Mark MMC card to be formatted also in bootup
	RDMUtil util;
	if ( util.Connect() == KErrNone )
	    {        
        TInt err = util.MarkMMCWipe();
        if( err != KErrNone )
        	{
        	// even if not successfull we try to reset as much as possible -> continue
        	Dprint( ( _L( "CSCPPatternPlugin::WipeDeviceL(): FAILED to mark MMC wipe: %d"), err ) );        	
        	}    
        util.Close();
	    }	

    // Reboot with RFS reason  

        RStarterSession startersession;
        if( startersession.Connect() == KErrNone )
            {
            startersession.Reset( RStarterSession::EDeepRFSReset );
            startersession.Close();
            }
                                           
    if ( ret != KErrNone )
        {
        Dprint( ( _L( "CSCPPatternPlugin::WipeDeviceL(): Rfs FAILED: %d"), ret ) );
        }                                                         
    }
    
// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::ReadConfiguration()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
// 
TInt CSCPTimestampPlugin::ReadConfiguration()
    {
    TInt ret = KErrNone;
    Dprint( (_L("CSCPTimestampPlugin::ReadConfiguration()") ));
    if ( iConfiguration == NULL )
        {
        TRAPD( err, iConfiguration = CSCPParamObject::NewL() );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPTimestampPlugin::ReadConfiguration() err != KErrNone") ));
            return err;
            }

        TFileName configFile;
        TRAP( err, ret = iEventHandler->GetStoragePathL( configFile ) ); 
        
        if ( ( err != KErrNone ) && ( ret == KErrNone ) )
            {
            Dprint( (_L("CSCPTimestampPlugin::ReadConfiguration() err != KErrNone  &&  ret == KErrNone") ));
            ret = err;
            }               
        
        if ( ( err == KErrNone ) && ( ret == KErrNone ) )
            {
            configFile.Append( KSCPTSConfigFile );
            TRAP( ret, iConfiguration->ReadFromFileL( configFile, iFsSession ) );
            Dprint( (_L("CSCPTimestampPlugin::ReadConfiguration() err == KErrNone  &&  ret == KErrNone") ));
            if ( ret == KErrNotFound )
                {
                ret = KErrNone; // Not an error, we'll create this file
                }
            }        
        }
    
    return ret;
    }

        
// ----------------------------------------------------------------------------
// CSCPTimestampPlugin::WriteConfiguration()
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
// 
TInt CSCPTimestampPlugin::WriteConfiguration()
    {
    TInt ret = KErrGeneral;

    if ( iConfiguration != NULL )
        {                
        TFileName configFile;
        TRAPD( err, ret = iEventHandler->GetStoragePathL( configFile ) );
        
        if ( ( err != KErrNone ) && ( ret == KErrNone ) )
            {
            ret = err;
            }
        
        if ( ( err == KErrNone ) && ( ret == KErrNone ) )
            {
            configFile.Append( KSCPTSConfigFile );
            TRAP( ret, iConfiguration->WriteToFileL( configFile, iFsSession ) );           
            }        
        }    
    
    return ret;    
    }
    
// End of File
