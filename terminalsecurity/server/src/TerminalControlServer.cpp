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


#define __INCLUDE_CAPABILITY_NAMES__

#include "TerminalControlServer.h"

// symbian
#include <e32svr.h>
#include <f32file.h>
#include <apgtask.h>
#include <apgcli.h>
#include <e32property.h>
#include <apacmdln.h>
#include <hal.h>
#include <utf.h>
// s60
#include <PSVariables.h>
#include <rfsClient.h>
#include <starterclient.h>
#include <syslangutil.h>
// tarm
#include "TerminalControlSession.h"
#include "TPtrC8I.h"
#include "TerminalControl3rdPartyAPI.h"
#include "DMUtilClient.h"
#include "debug.h"



/*

    Implementation notes:

    - CTerminalControlServer creates a CTerminalControlSession for each connection

    - CTerminalControlSession creates subsessions that have separate privileges

    - Privileges are stored in CTcTrustedSession objects (maintained by CTerminalControlSession)

    - Privileges are given by server (according to client Secure ID and received SSL certificate)

    - Server cannot be used without a subsession, client is paniced if first message is not ECreateSubSession    

*/

/******************************************************************************************

    Security policies of Terminal Control are defined here:   

*/

/**
    Ranges of policies
*/
const TInt CTerminalControlServer::iTcRanges[TC_NUMBER_OF_POLICIES] = {0, EFileScan, E3rdPartySetterGet};

/**
    Number of elements
*/
const TUint8 CTerminalControlServer::iTcElementsIndex[TC_NUMBER_OF_POLICIES] = {0, 1, 2};

/**
    Elements for each range
*/    
const CPolicyServer::TPolicyElement CTerminalControlServer::iTcElements[TC_NUMBER_OF_POLICIES] =
	    {
	    {_INIT_SECURITY_POLICY_PASS},
	    {_INIT_SECURITY_POLICY_C2(ECapabilityPowerMgmt, ECapabilityDiskAdmin)},
	    {_INIT_SECURITY_POLICY_PASS}
	    };

/**
    The policy
*/
const CPolicyServer::TPolicy CTerminalControlServer::iTcConnectionPolicy =
    {
        CPolicyServer::EAlwaysPass,
        TC_NUMBER_OF_POLICIES,
        iTcRanges,
        iTcElementsIndex,
        iTcElements
    };

/*****************************************************************************************/

_LIT8(KFormatProcessNamePrefix, "Process");

// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------

static void RunServerL()
	{
	RDEBUG("TerminalControlServer.cpp RunServerL");
	
	// naming the server thread after the server helps to debug panics
	//User::LeaveIfError(RThread().Rename(KTerminalControlServerName));   //Deprecated! Replace this with the line below as soon as possible
	User::LeaveIfError(User::RenameThread(KTerminalControlServerName)); //Correct PlatSec function, not available until week 2004_32

	// create and install the active scheduler
	
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	// create the server (leave it on the cleanup stack)
	CTerminalControlServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("Policy engine server is running");
	CActiveScheduler::Start();
	
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG("TerminalControlServer.cpp: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}
    
// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
void PanicClient(const RMessagePtr2& aMessage,TTerminalControlPanic aPanic)
	{
	RDEBUG("TerminalControlServer.cpp PanicClient");
	
	_LIT(KPanic,"TerminalControlServer");
	aMessage.Panic(KPanic,aPanic);
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer
// ----------------------------------------------------------------------------------------
CTerminalControlServer::CTerminalControlServer()
	: CPolicyServer(
	    EPriorityStandard, // Active object priority
        iTcConnectionPolicy,
	    ESharableSessions)
    , iProcessInfoArray(0)
	{
	RDEBUG("CTerminalControlServer::CTerminalControlServer");
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::NewLC
// ----------------------------------------------------------------------------------------
CServer2* CTerminalControlServer::NewLC()
	{    
	RDEBUG("CTerminalControlServer::NewLC");
	
	CTerminalControlServer* self=new(ELeave) CTerminalControlServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::~CTerminalControlServer
// ----------------------------------------------------------------------------------------
CTerminalControlServer::~CTerminalControlServer()
{
	RDEBUG("CTerminalControlServer::~CTerminalControlServer");

	if (iSCPClient.Handle())
	iSCPClient.Close();

	if (iContainerIndex)
	    {
	    delete iContainerIndex;
	    iContainerIndex = NULL;
	    }

	if (iProcessInfoArray)
	    {
	    iProcessInfoArray->Reset();
	    delete iProcessInfoArray;
	    iProcessInfoArray = NULL;
	    }
}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::ConstructL
// 2nd phase construction - ensure the timer and server objects are running
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::ConstructL()
	{
	RDEBUG("CTerminalControlServer::ConstructL");

	StartL(KTerminalControlServerName);	
	iContainerIndex = CObjectConIx::NewL();
	iProcessInfoArray = new (ELeave) CArrayFix<TTcProcessInfo>((TBufRep)CBufFlat::NewL, 10);
	RDEBUG_2("CTerminalControlServer::iProcessInfoArray ALLOC %x", (TAny*)iProcessInfoArray);

	User::LeaveIfError( iSCPClient.Connect() );
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::NewSessionL
// Create a new client session. This should really check the version number.
// ----------------------------------------------------------------------------------------
CSession2* CTerminalControlServer::NewSessionL(const TVersion& aVersion,const RMessage2&) const
	{
	RDEBUG("CTerminalControlServer::NewSessionL");

	// check we're the right version
	TVersion v(KTerminalControlServerMajorVersionNumber,
	           KTerminalControlServerMinorVersionNumber,
	           KTerminalControlServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	return new (ELeave) CTerminalControlSession();
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::AddSession
// A new session is being created
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::AddSession()
	{
	RDEBUG("CTerminalControlServer::AddSession");
	++iSessionCount;
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::DropSession
// A session is being destroyed
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::DropSession()
	{
	RDEBUG("CTerminalControlServer::DropSession");
	iSessionCount --;
	if( iSessionCount == 0 )
	    {
	    CActiveScheduler::Stop();
	    }
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::NewContainerL
// ----------------------------------------------------------------------------------------
CObjectCon* CTerminalControlServer::NewContainerL()
	{
	RDEBUG("CTerminalControlServer::NewContainerL");

	return iContainerIndex->CreateL();
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::CustomSecurityCheckL
// ----------------------------------------------------------------------------------------
CPolicyServer::TCustomResult CTerminalControlServer::CustomSecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	RDEBUG("CTerminalControlServer::CustomSecurityCheckL");

	return EPass;
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::DeleteFileL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::DeleteFileL( const TDesC8 &aFileName )
    {
	RDEBUG("CTerminalControlServer::DeleteFileL");

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL( fs );

    HBufC *fileName = HBufC::NewLC( aFileName.Length()+1 );
    TPtr fnptr( fileName->Des() );

    fnptr.Copy(aFileName);
    TEntry entry;
    
    User::LeaveIfError( fs.Entry( fnptr, entry ) );
    
    if( entry.IsDir() )
        {
        if(fnptr.Right(1) != _L("\\"))
            {
            fnptr.Append(_L("\\"));
            }
        User::LeaveIfError(fs.RmDir( fnptr ) );
        }
    else
        {
        User::LeaveIfError(fs.Delete( fnptr ) );
        }
    
    CleanupStack::PopAndDestroy( fileName );

    CleanupStack::PopAndDestroy( &fs );
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetRunningProcessesL
// ----------------------------------------------------------------------------------------
CBufFlat* CTerminalControlServer::GetRunningProcessesL( )
    {
	RDEBUG("CTerminalControlServer::GetRunningProcessesL");

    TFullName    processName;
    TFindProcess findProcess;
    CBufFlat *buffer = CBufFlat::NewL(128);
    iProcessInfoArray->Reset();

    while( KErrNone == findProcess.Next( processName ) )
        {
        TTcProcessInfo info;
        RProcess process;

        if( KErrNone == process.Open( findProcess ) )
            {
            //
            // Add process information to local array
            //
            info.iProcessName = processName;
            info.iFileName    = process.FileName();

            info.iHandle      = process.Handle();
            info.iId          = process.Id();
            info.iSecureId    = process.SecureId();

            info.iProtected   = EFalse; 

            process.GetMemoryInfo( info.iMemoryInfo );

            User::IsRomAddress( info.iCodeInRom, (TAny*)(info.iMemoryInfo.iCodeBase) );
            if( !info.iCodeInRom )
            	{
            	User::IsRomAddress( info.iCodeInRom, (TAny*)(info.iMemoryInfo.iCodeBase) );
            	}

            iProcessInfoArray->AppendL( info );

            //
            // Add process also to return buffer
            //
            /*
            TInt appendPosition = buffer->Size();
            if(iProcessInfoArray->Count() >= 2)
                {
                TBuf8<sizeof(info.iProcessName)> proName;
                proName.Copy(info.iProcessName);
                buffer->InsertL(appendPosition, _L8("/"));
                buffer->InsertL(appendPosition+1, proName);
                }
            else
                {
                TBuf8<sizeof(info.iProcessName)> proName;
                proName.Copy(info.iProcessName);
                buffer->InsertL(appendPosition, proName);
                }
            }
            
            */
            
            // Enumerate names from 1
            TInt appendPosition = buffer->Size();
            TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
            TBuf8<sizeof(KFormatProcessNamePrefix)+20> nameBuf;

            numBuf.Num(iProcessInfoArray->Count());
            nameBuf.Zero();
            nameBuf.Append(KFormatProcessNamePrefix);
            nameBuf.Append(numBuf);

            if(iProcessInfoArray->Count() >= 2)
                {
                buffer->InsertL(appendPosition, _L8("/"));
                buffer->InsertL(appendPosition+1, nameBuf);
                }
            else
                {
                buffer->InsertL(appendPosition, nameBuf);
                }
            }
        }
/*
	TUint32 flags = Exec::ProcessFlags(KCurrentProcessHandle);
	if (flags & KProcessFlagSystemPermanent)
		return ESystemPermanent;
	if (flags & KProcessFlagSystemCritical)
		return ESystemCritical;
	if (flags & KThreadFlagProcessPermanent)
		return EAllThreadsCritical;
	return ENotCritical;
*/
    return buffer;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::FindLocalProcessInfoL
// ----------------------------------------------------------------------------------------
CTerminalControlServer::TTcProcessInfo CTerminalControlServer::FindLocalProcessInfoL ( const TDesC8 &aProcessName )
    {
	RDEBUG("CTerminalControlServer::FindLocalProcessInfoL");

    TInt processCount = iProcessInfoArray->Count();
    if(processCount == 0)
        {
        User::Leave( KErrNotFound );
        }            
    if(aProcessName.Length() < KFormatProcessNamePrefix().Length()+1)
        {
        User::Leave(KErrNotFound);
        }
    if(aProcessName.Left( KFormatProcessNamePrefix().Length() ).Compare(KFormatProcessNamePrefix) != KErrNone)
        {
        User::Leave(KErrNotFound);
        }
    //
    // Get process index from the name
    //
    TInt  index;
    TLex8 lex;
    
    lex.Assign( aProcessName );
    lex.Inc( KFormatProcessNamePrefix().Length() );
    User::LeaveIfError( lex.Val(index) );
    index --;

    if(index < 0 || index >= processCount)
        {
        User::Leave( KErrNotFound );
        }

    return iProcessInfoArray->At(index);
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetProcessDataL
// ----------------------------------------------------------------------------------------
CBufFlat* CTerminalControlServer::GetProcessDataL( const TDesC8 &aProcessName, TDesC8 &aDataName )
    {
	RDEBUG("CTerminalControlServer::GetProcessDataL");
    
    _LIT8(KProcessDataFormat,
"\
<process-information name=\"%S\">\r\n\
\t<process-data name=\"%S\" value=\"%S\"/>\r\n\
</process-information>\r\n\
");

    TTcProcessInfo info = FindLocalProcessInfoL( aProcessName );

    CBufFlat* processData; processData = 0;
    processData = CBufFlat::NewL(128);
    CleanupStack::PushL( processData );

    // Set process data
    if(aDataName == _L8("ID"))
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
        numBuf.Num((TUint)info.iId);
        processData->InsertL(0, numBuf);
        }
        else
    if(aDataName == _L8("FromRAM"))
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
        numBuf.Num((TUint)!info.iCodeInRom);
        processData->InsertL(0, numBuf);
        }
        else
    if(aDataName == _L8("MemoryInfo"))
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> valueBuf;
        valueBuf.Num((TUint)info.iMemoryInfo.iCodeSize+
                          info.iMemoryInfo.iConstDataSize+
                          info.iMemoryInfo.iInitialisedDataSize+
                          info.iMemoryInfo.iUninitialisedDataSize);
        
        TInt responseLength = KProcessDataFormat().Length() + aProcessName.Length()
                            + aDataName.Length() + valueBuf.Length();
                            
        HBufC8* buffer = HBufC8::NewLC( responseLength );

        buffer->Des().Format( KProcessDataFormat, &aProcessName, &aDataName, &valueBuf );
        processData->InsertL(0, *buffer);

        CleanupStack::PopAndDestroy( buffer );
        }
        else
    if(aDataName == _L8("Filename"))
        {
        TBuf8<sizeof(TFileName)> fname;
        fname.Copy(info.iFileName);
        processData->InsertL(0, fname);
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::Pop( processData );

    return processData;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::WipeDeviceL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::WipeDeviceL( )
    {
	RDEBUG("CTerminalControlServer::WipeDeviceL");

	// Mark MMC card to be formatted also
	RDMUtil util;
	User::LeaveIfError( util.Connect() );
	CleanupClosePushL( util );
	TInt err = util.MarkMMCWipe();
	if( err != KErrNone )
		{
		// even if not successfull we try to reset as much as possible -> continue
		RDEBUG_2("*** ERROR: CTerminalControlServer::WipeDeviceL %d", err );
		}
	
	CleanupStack::PopAndDestroy( &util );
	DeepFactoryResetL();    
    }
    
// ----------------------------------------------------------------------------------------
// CTerminalControlServer::DeepFactoryResetL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::DeepFactoryResetL()
    {
	RDEBUG("CTerminalControlServer::DeepFactoryResetL");
    //
    // This code is originally copied from CRfsHandler::ActivateRfsL
    // Any changes there might also be required here.
    //
    // In case of deep level RFS, set the default language code
    // here, before RFS reboot.
    TInt language( 0 );

    // If default language is not found, we reset anyway
    if ( SysLangUtil::GetDefaultLanguage( language ) == KErrNone )
        {
        HAL::Set( HALData::ELanguageIndex, language );
        }

    // Send NVD_SET_DEFAULT_REQ here, before reboot


    RStarterSession starter;
    User::LeaveIfError( starter.Connect() );
    starter.Reset( RStarterSession::EDeepRFSReset ); // Initiates reset.
    // Actual RFS operation is initiated by Starter server while booting up.
    starter.Close();

    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::RebootDeviceL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::RebootDeviceL( )
    {
	RDEBUG("CTerminalControlServer::RebootDeviceL");

    RStarterSession starter;
    User::LeaveIfError( starter.Connect() );
    starter.Reset( RStarterSession::EUnknownReset ); 
    starter.Close();

    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetLockLevel
// ----------------------------------------------------------------------------------------
TInt CTerminalControlServer::GetLockLevelL( )
    {
	RDEBUG("CTerminalControlServer::GetLockLevelL");
    
    TBool state;
    User::LeaveIfError( iSCPClient.GetLockState(state) );

    return state;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetLockTimeoutL
// ----------------------------------------------------------------------------------------
TInt CTerminalControlServer::GetLockTimeoutL( )
    {
	RDEBUG("CTerminalControlServer::GetLockTimeoutL");
    
    TBuf<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> valueStr;
    TInt     value;
    TLex     lex;

    User::LeaveIfError( iSCPClient.GetParamValue(ESCPAutolockPeriod/*RTerminalControl3rdPartySession::ETimeout*/, valueStr) );

    lex.Assign(valueStr);
    User::LeaveIfError(lex.Val( value ));

    return value;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetLockMaxTimeoutL
// ----------------------------------------------------------------------------------------
TInt CTerminalControlServer::GetLockMaxTimeoutL( )
    {
	RDEBUG("CTerminalControlServer::GetLockMaxTimeoutL");
    
    TBuf<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> valueStr;
    TInt     value;
    TLex     lex;

    User::LeaveIfError( iSCPClient.GetParamValue(ESCPMaxAutolockPeriod/*RTerminalControl3rdPartySession::EMaxTimeout*/, valueStr) );

    lex.Assign(valueStr);
    User::LeaveIfError(lex.Val( value ));

    return value;
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::SetLockLevelL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::SetLockLevelL( TInt aValue )
    {
	RDEBUG("CTerminalControlServer::SetLockLevelL");
    
    _LIT(KZero, "0");
    TBuf<1> zero; zero.Copy(KZero);
    switch( aValue )
        {
            case 0: // open lock
            //User::LeaveIfError( iSCPClient.SetParamValue( ESCPAutolockPeriod, zero ) );
            User::LeaveIfError( iSCPClient.SetPhoneLock( EFalse ) );
            break;

            case 1: // user lock
            User::LeaveIfError( iSCPClient.SetPhoneLock( ETrue ) );
            break;

            case 2: // admin lock
            default:
            User::Leave( KErrNotSupported );
            break;            
        }
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::SetLockTimeoutL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::SetLockTimeoutL( TInt aValue )
    {
	RDEBUG("CTerminalControlServer::SetLockTimeoutL");
    
	SetIntValueL(aValue, ESCPAutolockPeriod);
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::SetLockMaxTimeoutL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::SetLockMaxTimeoutL( TInt aValue )
    {
	RDEBUG("CTerminalControlServer::SetLockMaxTimeoutL");
    
	SetIntValueL(aValue, ESCPMaxAutolockPeriod);
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::SetLockCodeL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::SetLockCodeL( TDesC8& aCode )
    {
	RDEBUG("CTerminalControlServer::SetLockCodeL");
    
    if ( aCode.Length() > KSCPPasscodeMaxLength )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* codeBuffer = HBufC::NewL( KSCPPasscodeMaxLength );
    CleanupStack::PushL( codeBuffer );
    TPtr codeBuf = codeBuffer->Des();
    codeBuf.Copy( aCode );

    RDEBUG("CTerminalControlServer::SetLockCodeL: Calling SCPClient::ChangeCode()");
    User::LeaveIfError( iSCPClient.ChangeCode( codeBuf ) );
    
    CleanupStack::PopAndDestroy( codeBuffer );
    }




// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS


void CTerminalControlServer::SetPasscodeMinLengthL( TInt aMinLength )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMinLengthL");
	SetIntValueL( aMinLength, RTerminalControl3rdPartySession::EPasscodeMinLength );
	}

TInt CTerminalControlServer::GetPasscodeMinLengthL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMinLengthL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMinLength );
    }

void CTerminalControlServer::SetPasscodeMaxLengthL( TInt aMaxLength )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMaxLengthL");
	SetIntValueL( aMaxLength, RTerminalControl3rdPartySession::EPasscodeMaxLength );
	}

TInt CTerminalControlServer::GetPasscodeMaxLengthL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMaxLengthL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMaxLength );
    }

void CTerminalControlServer::SetPasscodeRequireUpperAndLowerL( TInt aRequire )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeRequireUpperAndLowerL");
	SetIntValueL( aRequire, RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower );
	}

TInt CTerminalControlServer::GetPasscodeRequireUpperAndLowerL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeRequireUpperAndLowerL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower );
    }

void CTerminalControlServer::SetPasscodeRequireCharsAndNumbersL( TInt aRequire )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeRequireCharsAndNumbersL");
	SetIntValueL( aRequire, RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers );
	}

TInt CTerminalControlServer::GetPasscodeRequireCharsAndNumbersL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeRequireCharsAndNumbersL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers );
    }

void CTerminalControlServer::SetPasscodeMaxRepeatedCharactersL( TInt aMax )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMaxRepeatedCharactersL");
	SetIntValueL( aMax, RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters );
	}

TInt CTerminalControlServer::GetPasscodeMaxRepeatedCharactersL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMaxRepeatedCharactersL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters );
    }

void CTerminalControlServer::SetPasscodeHistoryBufferL( TInt aBufferLength )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeHistoryBufferL");
	SetIntValueL( aBufferLength, RTerminalControl3rdPartySession::EPasscodeHistoryBuffer );
	}

TInt CTerminalControlServer::GetPasscodeHistoryBufferL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeHistoryBufferL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeHistoryBuffer );
    }

void CTerminalControlServer::SetPasscodeExpirationL( TInt aExpiration )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeExpirationL");
	SetIntValueL( aExpiration, RTerminalControl3rdPartySession::EPasscodeExpiration );
	}

TInt CTerminalControlServer::GetPasscodeExpirationL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeExpirationL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeExpiration );
    }

void CTerminalControlServer::SetPasscodeMinChangeToleranceL( TInt aTolerance )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMinChangeToleranceL");
	SetIntValueL( aTolerance, RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance );
	}

TInt CTerminalControlServer::GetPasscodeMinChangeToleranceL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMinChangeToleranceL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMinChangeTolerance );
    }

void CTerminalControlServer::SetPasscodeMinChangeIntervalL( TInt aInterval )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMinChangeIntervalL");
	SetIntValueL( aInterval, RTerminalControl3rdPartySession::EPasscodeMinChangeInterval );
	}

TInt CTerminalControlServer::GetPasscodeMinChangeIntervalL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMinChangeIntervalL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMinChangeInterval );
    }
    
void CTerminalControlServer::SetPasscodeCheckSpecificStringsL( TInt aChoice )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeCheckSpecificStringsL");
	SetIntValueL( aChoice, RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings );
	}

TInt CTerminalControlServer::GetPasscodeCheckSpecificStringsL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeCheckSpecificStringsL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings );
    }    
    
void CTerminalControlServer::DisallowSpecificPasscodesL( const TDesC8& aString )
	{
	RDEBUG("CTerminalControlServer::DisallowSpecificPasscodesL");
	
	HBufC* buf = HBufC::NewLC( aString.Length() * 2 );
	TPtr16 ptr = buf->Des();
	User::LeaveIfError( CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr, aString ) );
	User :: LeaveIfError(iSCPClient.SetParamValue(RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific, 
    		ptr, this->Message().SecureId().iId));
	
    
    CleanupStack::PopAndDestroy( buf );
	}   
	
void CTerminalControlServer::AllowSpecificPasscodesL( const TDesC8& aString )
	{
	RDEBUG("CTerminalControlServer::AllowSpecificPasscodesL");
	
	HBufC* buf = HBufC::NewLC( aString.Length() * 2 );
	TPtr16 ptr = buf->Des();
	User::LeaveIfError( CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr, aString ) );
	User :: LeaveIfError(iSCPClient.SetParamValue(RTerminalControl3rdPartySession :: EPasscodeAllowSpecific, 
	        ptr, this->Message().SecureId().iId));
    
    CleanupStack::PopAndDestroy( buf );
	} 	 
	
void CTerminalControlServer::ClearSpecificPasscodeStringsL()
	{
	RDEBUG("CTerminalControlServer::ClearSpecificPasscodeStringsL");
	// Send the value 1 to SCP to clear the string buffer
	SetIntValueL( 1, RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings );
	}
	
void CTerminalControlServer::SetPasscodeMaxAttemptsL( TInt aAttempts )
	{
	RDEBUG("CTerminalControlServer::SetPasscodeMaxAttemptsL");
	SetIntValueL( aAttempts, RTerminalControl3rdPartySession::EPasscodeMaxAttempts );
	}

TInt CTerminalControlServer::GetPasscodeMaxAttemptsL()
    {
	RDEBUG("CTerminalControlServer::GetPasscodeMaxAttemptsL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMaxAttempts );
    }   	
    
void CTerminalControlServer::SetConsecutiveNumbersL( TInt aConsecutiveNumbers )
	{
	RDEBUG("CTerminalControlServer::SetConsecutiveNumbersL");
	SetIntValueL( aConsecutiveNumbers, RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers );
	}

TInt CTerminalControlServer::GetConsecutiveNumbersL()
    {
	RDEBUG("CTerminalControlServer::GetConsecutiveNumbersL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers );
    }   
    
void CTerminalControlServer::SetPasscodeMinSpecialCharactersL( TInt aMin )
    {
    RDEBUG("CTerminalControlServer::SetPasscodeMinSpecialCharactersL");
    SetIntValueL( aMin, RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters );
    }

TInt CTerminalControlServer::GetPasscodeMinSpecialCharactersL()
    {
    RDEBUG("CTerminalControlServer::GetPasscodeMinSpecialCharactersL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters );
    }
        		
void CTerminalControlServer::SetDisallowSimpleL( TInt aDisallowSimple )
    {
    RDEBUG("CTerminalControlServer::SetDisallowSimpleL");
    SetIntValueL( aDisallowSimple, RTerminalControl3rdPartySession::EPasscodeDisallowSimple );
    }

TInt CTerminalControlServer::GetDisallowSimpleL()
    {
    RDEBUG("CTerminalControlServer::GetDisallowSimpleL");
    return GetIntValueL( RTerminalControl3rdPartySession::EPasscodeDisallowSimple );
    }   
// ----- enhanced features END ----------------
//#endif

void CTerminalControlServer::SetIntValueL( TInt avalue, TInt aSCPCommandEnum )
	{
	TBuf<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> valueStr;
    valueStr.Format(_L("%d"), avalue );
    
    User :: LeaveIfError(iSCPClient.SetParamValue(aSCPCommandEnum, 
            valueStr, this->Message().SecureId().iId));
	}
	
	
TInt CTerminalControlServer::GetIntValueL( TInt aSCPCommandEnum )
	{
	TBuf<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> valueStr;
    TInt value;
    TLex lex;

    User::LeaveIfError( iSCPClient.GetParamValue( aSCPCommandEnum, valueStr ) );

    lex.Assign( valueStr );
    User::LeaveIfError( lex.Val( value ) );

    return value;
	}


// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StartProcessByUidL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StartProcessByUidL ( const TUid& aUID )
    {
	RDEBUG("CTerminalControlServer::StartProcessByUidL");
    
    RApaLsSession apaLs;
    User::LeaveIfError( apaLs.Connect() );
    CleanupClosePushL( apaLs );

    TThreadId threadId;
    TInt error = apaLs.StartDocument(_L(""), aUID, threadId);
    User::LeaveIfError( error );
    
    CleanupStack::PopAndDestroy( &apaLs );
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StartProcessByUidL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StartProcessByUidL ( const TDesC8& aUID )
    {
	RDEBUG("CTerminalControlServer::StartProcessByUidL2");
    
    TLex8 lex; lex.Assign( aUID );
    TInt64 uid_value;
    
    User::LeaveIfError( lex.Val( uid_value) );
    StartProcessByUidL( TUid::Uid( uid_value ) );
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StartProcessByFullNameL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StartProcessByFullNameL ( const TDesC8& aName )
    {
	RDEBUG("CTerminalControlServer::StartProcessByFullNameL");
    
    // 8bit to 16bit string
    //
    HBufC* fileNameBuf = HBufC::NewLC( aName.Length() );
    TPtr   fileName(fileNameBuf->Des());
    fileName.Copy( aName );
    
    // Connect to application architecture server
    //
    RApaLsSession apaLs;
    User::LeaveIfError( apaLs.Connect() );
    CleanupClosePushL( apaLs );
    
    // Create command line for process
    //
    CApaCommandLine* cmd = CApaCommandLine::NewLC();
    
    cmd->SetExecutableNameL( fileName );
    cmd->SetDocumentNameL( KNullDesC() );
    cmd->SetCommandL( EApaCommandRun );
    
    // Start application
    //
    User::LeaveIfError( apaLs.StartApp( *cmd ) );
    
    CleanupStack::PopAndDestroy( cmd );    
    CleanupStack::PopAndDestroy( &apaLs );
    CleanupStack::PopAndDestroy( fileNameBuf );
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StopProcessL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StopProcessByUidL ( const TUid& aUID )
    {
	RDEBUG("CTerminalControlServer::StopProcessByUidL");

    TInt rounds = 5;

    while( rounds-- )
        {
        TFullName    processName;    
        TFindProcess findProcess;
        TInt ret = KErrNone;

        while( KErrNone == ( ret = findProcess.Next( processName ) ) )
            {
            RProcess process;

            if( KErrNone == ( ret = process.Open( findProcess ) ) )
                {
                if( aUID == process.SecureId() )
                    {
                    RDEBUG("CTerminalControlServer::StopProcessByUidL Process with correct UID found => Kill(0) ");
                    process.Kill( 0 );
                    }
                }
            else
                {
                RDEBUG_2("CTerminalControlServer::StopProcessByUidL process.Open returned %d", ret );
                }
            }
        if( KErrNone != ret )
            {
            RDEBUG_2("CTerminalControlServer::StopProcessByUidL findProcess.Next returned %d", ret );
            }
        }
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StopProcessL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StopProcessByUidL ( const TDesC8& aUID )
    {
	RDEBUG("CTerminalControlServer::StopProcessByUidL2");

    TLex8 lex; lex.Assign( aUID );
    TInt64 uid_value;
    RDEBUG("CTerminalControlServer::StopProcessByUidL2 --- point 1");
    User::LeaveIfError( lex.Val( uid_value) );
    RDEBUG("CTerminalControlServer::StopProcessByUidL2 --- point 2");
    StopProcessByUidL( TUid::Uid( uid_value ) );
    RDEBUG("CTerminalControlServer::StopProcessByUidL2 --- point 3");
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::StopProcessByFullNameL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::StopProcessByFullNameL(const TDesC8& aName) {
    RDEBUG("[CTerminalControlServer]-> StopProcessByFullNameL() >>>");


    TInt rounds = 5;
    TFileName lMatchStr;
    lMatchStr.Copy(aName);
    lMatchStr.LowerCase();
    
    TBool lIsAbsolute = EFalse;
    if(lMatchStr.LocateReverse(TChar('\\')) != KErrNotFound) {
        lIsAbsolute = ETrue;
    }
    while(rounds--) {
        TInt ret = KErrNone;
        TFullName processName;
        TFindProcess findProcess;

        while(KErrNone == (ret = findProcess.Next(processName))) {
            RProcess process;

            if(KErrNone == (ret = process.Open(findProcess))) {
                RDEBUG("[CTerminalControlServer]-> Process.Open() returned");

                TFileName lStrSource(process.FileName());
                lStrSource.LowerCase();
                if(lIsAbsolute) {
                    RDEBUG("[CTerminalControlServer]-> INFO: The input was specified as an absolute path...");
                    if(lStrSource.Compare(lMatchStr) == 0) {
                        RDEBUG("[CTerminalControlServer]-> Process with correct Filename found => Kill(0) ");
                        process.Kill(0);
                    }
                }
                else {
                    RDEBUG("[CTerminalControlServer]-> INFO: The input was specified as an application name...");
                    TInt lLastDirSepIndex = lStrSource.LocateReverse('\\');
                    TInt lExeIndex = lStrSource.Find(_L(".exe"));
                    if(lLastDirSepIndex < lExeIndex) {
                        TInt lAppNameIndex = lStrSource.Find(lMatchStr);

                        /*
                         * The logic works like this, if the value of lStrSource was 'z:\sys\bin\calendar.exe', lMatchStr would contain calendar
                         * the values of the variables will be as follows
                         * lLastDirSepIndex = 10
                         * lAppNameIndex = 11
                         * lExeIndex = 19
                         * 
                         * The below logic would make sure that the right process is killed
                         */
                        if((lAppNameIndex == (lLastDirSepIndex + 1)) && ((lAppNameIndex + lMatchStr.Length()) == lExeIndex)) {
                            RDEBUG("[CTerminalControlServer]-> Process with correct Filename found => Kill(0)");
                    process.Kill(0);
                        }
                    }
                }
            }
            else {
                RDEBUG_2("[CTerminalControlServer]-> Process.Open() returned %d", ret);
                }
            }
        if(KErrNone != ret) {
            RDEBUG_2("[CTerminalControlServer]-> findProcess.Next() returned %d", ret);
            }
        }
    RDEBUG("[CTerminalControlServer]-> StopProcessByFullNameL() <<<");
    }

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::GetPasscodePolicyL
// ----------------------------------------------------------------------------------------
TBool CTerminalControlServer::GetPasscodePolicyL( )
    {
	RDEBUG("CTerminalControlServer::GetPasscodePolicyL");

    TBuf<20> policySet;
    User::LeaveIfError( iSCPClient.GetParamValue( ESCPCodeChangePolicy/*RTerminalControl3rdPartySession::EPasscodePolicy*/, policySet ) );
    
    // In SCP server, policy logic is inverted
    //
    if( policySet == _L("1") )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
    
// ----------------------------------------------------------------------------------------
// CTerminalControlServer::SetPasscodePolicyL
// ----------------------------------------------------------------------------------------
void CTerminalControlServer::SetPasscodePolicyL( TBool aIsSet )
    {
	RDEBUG("CTerminalControlServer::SetPasscodePolicyL");

    // In SCP server, policy logic is inverted
    //
    TBuf<1> policySet;
    if( aIsSet )
        {
        policySet.Copy(_L("0"));
        }
    else
        {
        policySet.Copy(_L("1"));
        }
        
    User::LeaveIfError( iSCPClient.SetParamValue( ESCPCodeChangePolicy/*RTerminalControl3rdPartySession::EPasscodePolicy*/
            ,policySet, this->Message().SecureId().iId));
    }

// ----------------------------------------------------------------------------------------
// End of file
