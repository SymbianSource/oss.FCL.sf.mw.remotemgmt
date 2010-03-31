/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
// TerminalControlSession.cpp
//
//#define __INCLUDE_CAPABILITY_NAMES__

#include <e32svr.h>
#include <f32file.h>
#include <apgtask.h>

#include <rfsClient.h>

#include <TerminalControl3rdPartyAPI.h>
#include "TerminalControlSession.h"
#include "PlatformSecurityPolicies.h"
#include "TcTrustedSession.h"
//Device encryption utility
#include <DevEncEngineConstants.h>
#include <DevEncSessionBase.h>
//Feature manager
#include <featmgr.h>
//For debugging purpose
#include "debug.h"


// ----------------------------------------------------------------------------------------
// CTerminalControlSession::CTerminalControlSession
// ----------------------------------------------------------------------------------------
CTerminalControlSession::CTerminalControlSession()
: iContainer(0), iTrustedSessions(0)
	{
	RDEBUG("CTerminalControlSession::CTerminalControlSession");

	}

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::Server
// ----------------------------------------------------------------------------------------
CTerminalControlServer& CTerminalControlSession::Server()
	{
	RDEBUG("CTerminalControlSession::Server");
	
	return *static_cast<CTerminalControlServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::CreateL()
	{
	RDEBUG("CTerminalControlSession::CreateL");

	iTrustedSessions = CObjectIx::NewL();
	iContainer = Server().NewContainerL();
	Server().AddSession();
	}
	
// ----------------------------------------------------------------------------------------
// CTerminalControlSession::~CTerminalControlSession
// ----------------------------------------------------------------------------------------
CTerminalControlSession::~CTerminalControlSession()
	{
	RDEBUG("CTerminalControlSession::~CTerminalControlSession");

	delete iTrustedSessions;	
	Server().DropSession();
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::ServiceL
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::ServiceL(const RMessage2& aMessage)
    {
	RDEBUG("CTerminalControlSession::ServiceL");

    TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::DispatchMessageL
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG_2("CTerminalControlServer::DispatchMessageL; %d", aMessage.Function() );

    HBufC8*    results = 0;
    HBufC8*    paramCopy  = 0;
    HBufC8*    paramCopy2 = 0;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;
    TInt       value;

    //
    // Create new sub session
    //
	switch(aMessage.Function())
	    {
	        case ECreateSubSession:
	        NewTrustedSessionL( aMessage );
	        return;
	    };

    //
    // Get access level from the sub session information
    // Note: this is where connections without a valid subsession are terminated
    //
    CTcTrustedSession* session = TrustedSessionFromHandle( aMessage );
    if(session == NULL)
        {
        // NOTE: This leave happens after client is paniced
        RDEBUG("CTerminalControlSession::DispatchMessageL() - SESSION NOT TRUSTED - ABORT!");
        User::Leave( KErrAccessDenied );
        }

    TInt accessLevel = session->AccessLevel();
    TBool hasAllFilesCap = aMessage.HasCapability(ECapabilityAllFiles);
    TBool hasDiskAdminCap = aMessage.HasCapability(ECapabilityDiskAdmin);

    //
    // Disconnecting
    //
	switch(aMessage.Function())
	    {
	        case ECloseSubSession:
	        if(session->iWipeRequested)
	            {
    	        DeleteTrustedSession( aMessage );
        	    Server().WipeDeviceL();
	            }
	        else if(session->iRebootRequested)
	            {
    	        DeleteTrustedSession( aMessage );
        	    Server().RebootDeviceL();
	            }
	        else
	            {
	            DeleteTrustedSession( aMessage );
	            }
	        return;
	    };

	if((accessLevel == CTcTrustedSession::EAccessLevelNone) && (!hasAllFilesCap))
        {
		RDEBUG("CTerminalControlSession::DispatchMessageL() - SESSION HAS NO ACCESS LEVEL - ABORT!");
		User::Leave( KErrAccessDenied );
        }    
    //
    // Message handlers for third party messages
    //
    if((((accessLevel & CTcTrustedSession::EAccessLevelApplication) != 0 )&& (hasDiskAdminCap))||(hasAllFilesCap))
        {
        switch(aMessage.Function())
            {
            case E3rdPartySetterGet:            
            GetDeviceLockParameterL( aMessage );
            break;

            case E3rdPartySetterGetSize:
            GetDeviceLockParameterSizeL( aMessage );
            break;

            case E3rdPartySetterSet:
            SetDeviceLockParameterL( aMessage );
            break;
            }
        }

    //
    // Message handlers for DM adapter
    //
    if((accessLevel & CTcTrustedSession::EAccessLevelDMAdapter) != 0)
        {
    	switch(aMessage.Function())
    	    {
            //-------------------------------------------------------------------------
	        case EFileScan:
                // Read startup folder
        	    paramCopy = CopyParameterL( aMessage, 1 );
        	    CleanupStack::PushL( paramCopy );
                session->FileScanL( *paramCopy, (aMessage.Int0() == 1) );
                CleanupStack::PopAndDestroy( paramCopy );
	        break;

            //-------------------------------------------------------------------------
	        case EFileScanResults:
	            results = session->FileScanResultsL();

                if( results != NULL)
                    {
    	            CleanupStack::PushL( results );
    	            
                    TInt size1 = aMessage.GetDesMaxLength(0);
                    TInt size2 = results->Size();
                    
                    if(size1 >= size2)
                        {
        	            aMessage.WriteL(0, *results);
                        }
                    else
                        {
                        User::Leave( KErrArgument );
                        }

                    CleanupStack::PopAndDestroy( results );
                    }
                else
                    {
                    User::Leave(KErrGeneral);
                    }
	        break;

            //-------------------------------------------------------------------------
        	case EFileScanResultsSize:
	            results = session->FileScanResultsL();
        	    
                if( results != NULL )
                    {                    
                    sizeBuf.Num( results->Size() );
                    delete results;
                    }
                else
                    {
                    sizeBuf.Num(0);
                    }
                    
                if(aMessage.GetDesMaxLength(0) >= sizeBuf.Length())
                    {
    	            aMessage.WriteL(0, sizeBuf);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
	        break;

            //-------------------------------------------------------------------------
        	case EFileDelete:
        	    paramCopy = CopyParameterL( aMessage, 0 );
        	    CleanupStack::PushL( paramCopy );
        	    Server().DeleteFileL( *paramCopy );
                CleanupStack::PopAndDestroy( paramCopy );
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_AutoLockPeriod_Set:
        	    value = aMessage.Int0();
        	    Server().SetLockTimeoutL( value );
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_AutoLockPeriod_Get:
                sizeBuf.Num( Server().GetLockTimeoutL() );
                if(aMessage.GetDesMaxLength(0) >= sizeBuf.Length())
                    {
    	            aMessage.WriteL(0, sizeBuf);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_MaxAutoLockPeriod_Set:
        	    value = aMessage.Int0();
        	    Server().SetLockMaxTimeoutL( value );
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_MaxAutoLockPeriod_Get:
                sizeBuf.Num( Server().GetLockMaxTimeoutL() );                    
                if(aMessage.GetDesMaxLength(0) >= sizeBuf.Length())
                    {
    	            aMessage.WriteL(0, sizeBuf);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_LockLevel_Set:
        	    value = aMessage.Int0();
        	    Server().SetLockLevelL( value );
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_LockLevel_Get:
        	    {
                sizeBuf.Num( Server().GetLockLevelL() );                    
                if(aMessage.GetDesMaxLength(0) >= sizeBuf.Length())
                    {
    	            aMessage.WriteL(0, sizeBuf);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_LockCode_Set:
        	    paramCopy = CopyParameterL( aMessage, 0 );
        	    CleanupStack::PushL( paramCopy );
                Server().SetLockCodeL( *paramCopy );
                CleanupStack::PopAndDestroy( paramCopy );
	        break;

            //-------------------------------------------------------------------------
        	case EDeviceLock_LockCode_Get:
        		{
        		RDEBUG("CTerminalControlSession::DispatchMessageL() - GetLockCode not supported - ABORT!");
        	    User::Leave( KErrNotSupported );
        		}
   	        break;

            //-------------------------------------------------------------------------
        	case EDeviceWipe:
        	    // Schedule wipe to when client has been disconnected
    	        session->iWipeRequested = ETrue;
	        break;
	        
	        
	        
// ----- enhanced features BEGIN ----------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
	        
	        case EDeviceLock_PasscodeMinLength_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMinLengthL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMinLength_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMinLengthL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxLength_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMaxLengthL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxLength_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMaxLengthL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeRequireUpperAndLower_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeRequireUpperAndLowerL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeRequireUpperAndLower_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeRequireUpperAndLowerL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeRequireCharsAndNumbers_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeRequireCharsAndNumbersL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeRequireCharsAndNumbers_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeRequireCharsAndNumbersL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxRepeatedCharacters_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMaxRepeatedCharactersL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxRepeatedCharacters_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMaxRepeatedCharactersL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeHistoryBuffer_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeHistoryBufferL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeHistoryBuffer_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeHistoryBufferL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeExpiration_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeExpirationL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeExpiration_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeExpirationL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMinChangeTolerance_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMinChangeToleranceL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMinChangeTolerance_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMinChangeToleranceL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMinChangeInterval_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMinChangeIntervalL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMinChangeInterval_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMinChangeIntervalL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeCheckSpecificStrings_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeCheckSpecificStringsL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeCheckSpecificStrings_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeCheckSpecificStringsL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeDisallowSpecific:
        	    {
				paramCopy = CopyParameterL( aMessage, 0 );
        	    CleanupStack::PushL( paramCopy );
        	    Server().DisallowSpecificPasscodesL( *paramCopy );
                CleanupStack::PopAndDestroy( paramCopy );    
                paramCopy = NULL;            
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeAllowSpecific:
        	    {
                paramCopy = CopyParameterL( aMessage, 0 );
        	    CleanupStack::PushL( paramCopy );
        	    Server().AllowSpecificPasscodesL( *paramCopy );
                CleanupStack::PopAndDestroy( paramCopy );    
                paramCopy = NULL; 
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeClearSpecificStrings:
        	    {
                Server().ClearSpecificPasscodeStringsL();
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxAttempts_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetPasscodeMaxAttemptsL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeMaxAttempts_Get:
        	    {
                sizeBuf.Num( Server().GetPasscodeMaxAttemptsL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;	
	        
	        case EDeviceLock_PasscodeConsecutiveNumbers_Set:
        	    {
                value = aMessage.Int0();
        	    Server().SetConsecutiveNumbersL( value );
        	    }
	        break;
	        
	        case EDeviceLock_PasscodeConsecutiveNumbers_Get:
        	    {
                sizeBuf.Num( Server().GetConsecutiveNumbersL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
    	            aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
        	    }
	        break;        

            case EDeviceLock_PasscodeMinSpecialCharacters_Set:
                {
                value = aMessage.Int0();
                Server().SetPasscodeMinSpecialCharactersL( value );
                }
            break;
            
            case EDeviceLock_PasscodeMinSpecialCharacters_Get:
                {
                sizeBuf.Num( Server().GetPasscodeMinSpecialCharactersL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
                    aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
                }
            break;
            
            case EDeviceLock_PasscodeDisallowSimple_Set:
                {
                value = aMessage.Int0();
                Server().SetDisallowSimpleL( value );
                }
            break;
            case EDeviceLock_PasscodeDisallowSimple_Get:
                {
                sizeBuf.Num( Server().GetDisallowSimpleL() );                    
                if( aMessage.GetDesMaxLength( 0 ) >= sizeBuf.Length() )
                    {
                    aMessage.WriteL( 0, sizeBuf );
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
                }
            break;        
	        
// ----- enhanced features END ----------------
//#endif	        
	        
	        
	        

            //-------------------------------------------------------------------------
        	case EProcesses:
        	    {
                /*if( session->iProcessList == NULL )
                    {
                    // Note: This should not happen since process list size may vary
                    // Process list is updated when size is queried
                    delete session->iProcessList; session->iProcessList = 0;
        	        session->iProcessList = Server().GetRunningProcessesL();
                    }*/

                if( session->iProcessList != NULL )
                    {
                    TInt size1 = aMessage.GetDesMaxLength(0);
                    TInt size2 = session->iProcessList->Size();
                    if(size1 >= size2)
                        {
        	            aMessage.WriteL(0, session->iProcessList->Ptr(0));
                        }
                    else
                        {
                        
                        User::Leave(KErrGeneral);
                        }
                    }
                else
                    {
                    RDEBUG("Process list is NULL");
                    User::Leave(KErrGeneral);
                    }
        	    }        	
	        break;

            //-------------------------------------------------------------------------
	        case EProcessesSize:
	            {
    	        // NOTE: Processes are listed only in this function
    	        // Process list size may change in each listing
    	        delete session->iProcessList; session->iProcessList = 0;
    	        session->iProcessList = Server().GetRunningProcessesL();

    	        // Get size in buffer    	        
                if( session->iProcessList != NULL )
                    {                    
                    sizeBuf.Num( session->iProcessList->Size() );
                    }
                else
                    {
                    sizeBuf.Num(0);
                    }

                // Return size
    	        if(aMessage.GetDesMaxLength(0) >= sizeBuf.Length())
                    {
    	            aMessage.WriteL(0, sizeBuf);
                    }
                else
                    {
                    RDEBUG("Invalid descriptor buffer length!");
                    User::Leave( KErrArgument );
                    }
	            }
	        break;

            //-------------------------------------------------------------------------
        	case EProcessData:
        	    paramCopy  = CopyParameterL( aMessage, 0 );
        	    CleanupStack::PushL( paramCopy );
        	    paramCopy2 = CopyParameterL( aMessage, 1 );
        	    CleanupStack::PushL( paramCopy2 );

                if( session->iProcessList == NULL )
                    {
                    // Note: Process list may have changed since last listing...
                    // If there is a previous list, we don't want to update it.
                    delete session->iProcessList; session->iProcessList = 0;
        	        session->iProcessList = Server().GetRunningProcessesL();
                    }

                delete session->iProcessData; session->iProcessData = 0;
        	    session->iProcessData = Server().GetProcessDataL(*paramCopy, *paramCopy2);
        	    
                if( session->iProcessData != NULL )
                    {
                    TInt size1 = aMessage.GetDesMaxLength(2);
                    TInt size2 = session->iProcessData->Size();
                    if(size1 >= size2)
                        {
        	            aMessage.WriteL(2, session->iProcessData->Ptr(0));
                        }
                    else
                        {
                        RDEBUG("Invalid descriptor buffer length!");
                        User::Leave( KErrArgument );
                        }
                    }
                else
                    {
                    RDEBUG("process data is NULL");
                    User::Leave(KErrGeneral);
                    }        	    

                CleanupStack::PopAndDestroy( paramCopy2 );
                CleanupStack::PopAndDestroy( paramCopy );
	        break;
        	
            //-------------------------------------------------------------------------
        	case EReboot:
        	    // Schedule reboot to when client has been disconnected
        	    value = aMessage.Int0();
        	    if( value == 0 )
        	        {
                    session->iRebootRequested = EFalse;
        	        }
        	    else
        	        {
        	        session->iRebootRequested = ETrue;
        	        }
	        break;
        	
            //-------------------------------------------------------------------------
        	case ELocalOperation_Start:
        	    paramCopy  = CopyParameterL( aMessage, 0 );        	    
        	    value = aMessage.Int1();
        	    CleanupStack::PushL( paramCopy );
        	    if(value == 1)
        	        {
                    Server().StartProcessByUidL( *paramCopy );
        	        }
        	    else
        	        {
        	        Server().StartProcessByFullNameL( *paramCopy );
        	        }
                CleanupStack::PopAndDestroy( paramCopy );
	        break;
        
            //-------------------------------------------------------------------------
        	case ELocalOperation_Stop:
        	    paramCopy  = CopyParameterL( aMessage, 0 );
        	    value = aMessage.Int1();
        	    CleanupStack::PushL( paramCopy );
        	    if(value == 1)
        	        {
                    Server().StopProcessByUidL( *paramCopy );
        	        }
        	    else
        	        {
                    Server().StopProcessByFullNameL( *paramCopy );
        	        }
                CleanupStack::PopAndDestroy( paramCopy );
	        break;
	        
	        default:
	        	{
	        	RDEBUG("CTerminalControlSession::DispatchMessageL() - DM Adapter - ERROR - KErrNotSupported");
	        	User::Leave( KErrNotSupported );	
	        	}
	        break;
	        
    	    }
        }
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::NewTrustedSessionL
// Create a new counter; pass back its handle via the message
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::NewTrustedSessionL( const RMessage2& aMessage)
	{
	RDEBUG("CTerminalControlSession::NewTrustedSessionL");

	TInt param0Size = aMessage.GetDesLength(0);
	TInt certSize = sizeof(TCertInfo);
	TBool hasCert = ( param0Size == certSize );

	TCertInfo cert;
	TPckg<TCertInfo> certp( cert );
	certp.FillZ();

	if( hasCert )
	    {
    	RDEBUG("CTerminalControlSession::NewTrustedSessionL Session has a certificate");
	    aMessage.Read(0, certp);
	    }
	else
	    {
    	RDEBUG("CTerminalControlSession::NewTrustedSessionL Warning: Session does not have a certificate");
	    }

    //
	// Add new CTcTrustedSession object into container and object index
	//
	TSecureId secureID = aMessage.SecureId();

	CTcTrustedSession* trustedSession = 0;

    if( hasCert )
        {
    	trustedSession = CTcTrustedSession::NewL( secureID, cert );
        }
    else
        {
        trustedSession = CTcTrustedSession::NewL( secureID );
        }

	if(trustedSession == 0)
	    {
	    PanicClient( aMessage, EBadDescriptor);
	    }
      else
      {
    	iContainer->AddL( trustedSession );
    	TInt handle = iTrustedSessions->AddL( trustedSession );

    	// Send handle to client
    	TPckg<TInt> handlePckg( handle );
    	TInt checkValue = handlePckg();
    	TRAPD( r, aMessage.WriteL(3, handlePckg))

    	RDEBUG_2("CTerminalControlSession::NewTrustedSessionL: %d", handle );

      if ( r != KErrNone)
        {
        iTrustedSessions->Remove(handle);
        PanicClient( aMessage, EBadDescriptor);
        return;
        }

    	iSubsessionCount++;
      }
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::TrustedSessionFromHandle
// ----------------------------------------------------------------------------------------
CTcTrustedSession* CTerminalControlSession::TrustedSessionFromHandle( const RMessage2& aMessage)
    {
	RDEBUG_2("CTerminalControlSession::TrustedSessionFromHandle: %d", aMessage.Int3() );
    
	CTcTrustedSession* trustedSession = (CTcTrustedSession*)iTrustedSessions->At(aMessage.Int3());
	if (trustedSession == NULL)
		PanicClient( aMessage, EBadSubsessionHandle); 
	return trustedSession;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::DeleteTrustedSession
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::DeleteTrustedSession( const RMessage2& aMessage)
    {
	RDEBUG_2("CTerminalControlSession::DeleteTrustedSession: %d", aMessage.Int3() );


	CTcTrustedSession* trustedSession = (CTcTrustedSession*)iTrustedSessions->At(aMessage.Int3());
	
	//if (trustedSession == NULL)
    //    PanicClient( aMessage, EBadSubsessionHandle); 

	iTrustedSessions->Remove(aMessage.Int3());		

	// delete trustedSession is not needed

	iSubsessionCount--;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::ServiceError
// Handle an error from CHelloWorldSession::ServiceL()
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("TerminalControlServer: CTerminalControlSession::ServiceError %d", aError);

	CSession2::ServiceError(aMessage,aError);
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::SetDeviceLockParameterL
// ----------------------------------------------------------------------------------------
TInt CTerminalControlSession::SetDeviceLockParameterL(const RMessage2 &aMessage)
    {
	RDEBUG_2("CTerminalControlSession::SetDeviceLockParameterL: %d", aMessage.Int0());

    TInt        status      = KErrNone;
    TInt        type        = aMessage.Int0();
    HBufC8*     data        = CopyParameterL(aMessage, 1);
    TPtr8       ptr         (data->Des());
    TInt        value       = 0;
    TLex8       lex;

    CleanupStack::PushL( data );

    //Check if the device memory is encrypted or not.
    TBool encryptionEnabled = IsDeviceMemoryEncrypted();
    
    switch(type)
        {
        case RTerminalControl3rdPartySession::ETimeout:
            lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
            if ( 0 == value )
                {
                if ( encryptionEnabled )
                    {
                    //If drive memory is encrypted, do not set the value
                    User::Leave( KErrPermissionDenied );
                    }
                }
    	    Server().SetLockTimeoutL( value );
        break;

        case RTerminalControl3rdPartySession::EMaxTimeout:
            RDEBUG_2("Value of max timeout is %d", value);
            lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );

            if ( encryptionEnabled )
                {
                RDEBUG("Memory is encrypted");
                if ((0 == value) || value > KMaxAutolockPeriod)
                    {
                    RDEBUG_2("Denying setting of max auto lock as value is %d", value);
                    User::Leave( KErrPermissionDenied );
                    }
                }
            else
                {
                RDEBUG("Memory is decrypted");
                }
    	    Server().SetLockMaxTimeoutL( value );
        break;

        case RTerminalControl3rdPartySession::EPasscode:
            Server().SetLockCodeL( *data );
        break;

        case RTerminalControl3rdPartySession::EPasscodePolicy:
            lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
            Server().SetPasscodePolicyL( value );
        break;

        case RTerminalControl3rdPartySession::ELock:
            lex.Assign( *data );
            User::LeaveIfError( lex.Val( value ) );
            RDEBUG_2("RTerminalControl3rdPartySession::ELock: %d", value );
            Server().SetLockLevelL( value );
        break;

        case RTerminalControl3rdPartySession::EDeviceWipe:
            if(data->Des() != _L8("0"))
                {
                Server().WipeDeviceL();
                }
        break;

// ----- enhanced features BEGIN ----------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

       	case RTerminalControl3rdPartySession::EPasscodeMinLength:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMinLengthL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxLength:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMaxLengthL( value );
       		}
       	break;

		case RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeRequireUpperAndLowerL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeRequireCharsAndNumbersL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMaxRepeatedCharactersL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeHistoryBuffer:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeHistoryBufferL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeExpiration:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeExpirationL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMinChangeToleranceL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMinChangeInterval:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMinChangeIntervalL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeCheckSpecificStringsL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeDisallowSpecific:
       		{
       		Server().DisallowSpecificPasscodesL( *data );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeAllowSpecific:
       		{
       		Server().AllowSpecificPasscodesL( *data );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings:
       		{
    	    Server().ClearSpecificPasscodeStringsL();
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxAttempts:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetPasscodeMaxAttemptsL( value );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers:
       		{
       		lex.Assign( *data );
    	    User::LeaveIfError( lex.Val( value ) );
    	    Server().SetConsecutiveNumbersL( value );
       		}
       	break;

        case RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters:
            {
            lex.Assign( *data );
            User::LeaveIfError( lex.Val( value ) );
            Server().SetPasscodeMinSpecialCharactersL( value );
            }
        break;
        case RTerminalControl3rdPartySession::EPasscodeDisallowSimple:
            {
            lex.Assign( *data );
            User::LeaveIfError( lex.Val( value ) );
            Server().SetDisallowSimpleL( value );
            }
        break;
        
//#endif
// ----- enhanced features END ----------------
       	
        default:
        	{
        	RDEBUG("CTerminalControlSession::SetDeviceLockParameterL() - 3rd party - ERROR - KErrNotSupported");
            User::Leave( KErrNotSupported );
        	}
        break;        

        }

    CleanupStack::PopAndDestroy( data );

    return status;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::GetDeviceLockParameterSizeL
// ----------------------------------------------------------------------------------------
TInt CTerminalControlSession::GetDeviceLockParameterSizeL(const RMessage2 &aMessage)
    {
	RDEBUG_2("CTerminalControlSession::GetDeviceLockParameterSizeL: %d", aMessage.Int0() );
    
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf2;
    TInt type = aMessage.Int0();

    GetDeviceLockParameterL( sizeBuf, type );
    sizeBuf2.Num(sizeBuf.Length());

    if(aMessage.GetDesMaxLength(1) >= sizeBuf2.Length())
        {
        aMessage.WriteL(1, sizeBuf2);
        }
    else
        {
        User::Leave(KErrGeneral);
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::GetDeviceLockParameterL
// ----------------------------------------------------------------------------------------
TInt CTerminalControlSession::GetDeviceLockParameterL(const RMessage2 &aMessage)
    {
	RDEBUG_2("CTerminalControlSession::GetDeviceLockParameterL: %d", aMessage.Int0() );

    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;
    TInt        status      = KErrNone;
    TInt        type        = aMessage.Int0();

    GetDeviceLockParameterL( sizeBuf, type );

    if(aMessage.GetDesMaxLength(1) >= sizeBuf.Length())
        {
        aMessage.WriteL(1, sizeBuf);
        }
    else
        {
        User::Leave(KErrGeneral);
        }

    return status;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlSession::GetDeviceLockParameterL
// ----------------------------------------------------------------------------------------
void CTerminalControlSession::GetDeviceLockParameterL(TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> &sizeBuf, TInt aType)
    {
	RDEBUG_2("CTerminalControlSession::GetDeviceLockParameterL2: %d", aType );

    switch(aType)
        {
        case RTerminalControl3rdPartySession::ETimeout:
            sizeBuf.Num( Server().GetLockTimeoutL() );
        break;

        case RTerminalControl3rdPartySession::EMaxTimeout:
            sizeBuf.Num( Server().GetLockMaxTimeoutL() );
        break;

        case RTerminalControl3rdPartySession::EPasscode:
            User::Leave( KErrNotSupported );        
        break;

        case RTerminalControl3rdPartySession::EPasscodePolicy:
            sizeBuf.Num( Server().GetPasscodePolicyL() );
        break;

        case RTerminalControl3rdPartySession::ELock:
            User::Leave( KErrNotSupported );        
        break;
        
// ----- enhanced features BEGIN ----------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS        

        case RTerminalControl3rdPartySession::EPasscodeMinLength:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMinLengthL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxLength:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMaxLengthL() );
       		}
       	break;

		case RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower:
       		{
    	    sizeBuf.Num( Server().GetPasscodeRequireUpperAndLowerL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers:
       		{
    	    sizeBuf.Num( Server().GetPasscodeRequireCharsAndNumbersL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMaxRepeatedCharactersL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeHistoryBuffer:
       		{
    	    sizeBuf.Num( Server().GetPasscodeHistoryBufferL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeExpiration:
       		{
    	    sizeBuf.Num( Server().GetPasscodeExpirationL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMinChangeToleranceL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMinChangeInterval:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMinChangeIntervalL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings:
       		{
    	    sizeBuf.Num( Server().GetPasscodeCheckSpecificStringsL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeDisallowSpecific:
       		{
       		// this is set only value, can not get
       		User::Leave( KErrNotSupported );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeAllowSpecific:
       		{
       		// this is set only value, can not get
       		User::Leave( KErrNotSupported );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings:
       		{
    	    // this is set only value, can not get
       		User::Leave( KErrNotSupported );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeMaxAttempts:
       		{
    	    sizeBuf.Num( Server().GetPasscodeMaxAttemptsL() );
       		}
       	break;
       	
       	case RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers:
       		{
       		sizeBuf.Num( Server().GetConsecutiveNumbersL() );
       		}
       	break;
       	
        case RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters:
            {
            sizeBuf.Num( Server().GetPasscodeMinSpecialCharactersL() );
            }
        break;
        
        case RTerminalControl3rdPartySession::EPasscodeDisallowSimple:
            {
            sizeBuf.Num( Server().GetDisallowSimpleL() );
            }
        break;
       	
//#endif
// ----- enhanced features END ----------------
        
        case RTerminalControl3rdPartySession::EDeviceWipe:
        default:
        	{
        	RDEBUG("CTerminalControlSession::GetDeviceLockParameterL() - 3rd party - ERROR - KErrNotSupported");
            User::Leave( KErrNotSupported );
        	}
        break;
        
        }    
    }
    
// ----------------------------------------------------------------------------------------
// CTerminalControlSession::CopyParameterL
// ----------------------------------------------------------------------------------------
HBufC8* CTerminalControlSession::CopyParameterL( const RMessage2 &aMsg, TInt aIndex )
    {
	RDEBUG("CTerminalControlSession::CopyParameterL");

    TInt length = aMsg.GetDesLengthL( aIndex );
    
    HBufC8* buffer = HBufC8::NewL( length );
    TPtr8 ptr( buffer->Des() );
    aMsg.ReadL( aIndex, ptr );

    return buffer;
    }

// ---------------------------------------------------------
// CTerminalControlSession::IsDeviceMemoryEncrypted
// Checks if phone memory is encrypted or not. 
// ---------------------------------------------------------
TBool CTerminalControlSession::IsDeviceMemoryEncrypted()
    {
    RDEBUG("CTerminalControlSession::IsDeviceMemoryEncrypted >>");
    TBool ret(EFalse);
    //First check if the feature is supported on device
    TRAPD(ferr, FeatureManager::InitializeLibL());
    if (ferr != KErrNone)
        {
        RDEBUG_2("feature mgr initialization error, %d", ferr);
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
            RDEBUG_2("Error in finding the library... %d", err);
            ret = EFalse;
            }
        else
        	{
		       TLibraryFunction entry = library.Lookup(1);
					 
	        if (!entry)
	            {
	            RDEBUG("Error in loading the library...");
	            ret = EFalse;
	            }
	        else
	        	{
		        devEncSession = (CDevEncSessionBase*) entry();
		        RDEBUG("Library is found and loaded successfully...");
		      	}
	        }

        if (!devEncSession)
            {
            RDEBUG("Can't instantiate device encryption session..");
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
		            RDEBUG_2("encstatus = %d", encStatus);
		            if (  err == KErrNone && encStatus != EDecrypted )
		                {
		                RDEBUG("Memory is encrypted");
		                ret = ETrue;
		                }
		            else
		                {
		                RDEBUG("Memory is not encrypted");
		                ret = EFalse;
		                }
		            }
		        else
		            {
		            RDEBUG_2("Error %d while establishing connection with device encryption engine", err);
		            ret = EFalse;
		            }
						}
				 delete devEncSession; devEncSession = NULL;

	       if (library.Handle())
    	      library.Close();    

		    }
    RDEBUG_2("CTerminalControlSession::IsDeviceMemoryEncrypted, ret = %d <<", ret);
    return ret;
    }

// ----------------------------------------------------------------------------------------
// End of file
