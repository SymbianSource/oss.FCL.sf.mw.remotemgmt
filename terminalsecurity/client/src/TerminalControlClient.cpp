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
/* TerminalControlClient.cpp
*/

#include <e32svr.h>

#include <TerminalControl3rdPartyAPI.h>
#include "TerminalControlClientServer.h"
#include "TerminalControlClient.h"
#include "debug.h"




// Standard server startup code
// 
static TInt StartServer()
	{
	RDEBUG("TerminalControlServer: Starting server...");
	
	const TUidType serverUid(KNullUid,KNullUid,KTerminalControlServerUid);

	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	//TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
	TInt r=server.Create(KTerminalControlServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("TerminalControlClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("TerminalControlClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}


// This is the standard retry pattern for server connection
EXPORT_C TInt RTerminalControl::Connect()
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTerminalControlServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}
	
EXPORT_C void RTerminalControl::Close()
	{
	RSessionBase::Close();  //basecall
	}



// ----------------------------------------------------------------------------------------
//
//  RTerminalControlSession subsessions
//
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::Open
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::Open(RTerminalControl &aServer, TCertInfo &aCertification)
    {
    RDEBUG("RTerminalControlSession::Open");
    TIpcArgs args1;

    TPckg<TCertInfo> certp( aCertification );
    args1.Set(0, &certp);

	return CreateSubSession(aServer, ECreateSubSession, args1);
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::Close
// ----------------------------------------------------------------------------------------
EXPORT_C void RTerminalControlSession::Close()
	{
	RDEBUG("RTerminalControlSession::Close");
	RSubSessionBase::CloseSubSession(ECloseSubSession);
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::FileScanL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::FileScan( const TDesC8& aStartPath /*= KTCDefaultStart*/,
                                                  TBool aRecursive /*= EFalse*/ ) 
	{
	RDEBUG("RTerminalControlSession::FileScan");
	
	TInt        status = KErrNone;
    TIpcArgs    args1;
    TInt        recursiveFlag = aRecursive?1:0;    
    
    // Set parameters
    args1.Set(0, recursiveFlag);
    args1.Set(1, &aStartPath);
	
    // Run file scan
	status = SendReceive( EFileScan, args1 );

    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::FileScanResultsL
// @param aBuffer Constructed CBufFlat to be filled with file scan results (will be resized)
// @discussion aBuffer should be pushed into cleanup stack if this method is not trapped
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::FileScanResultsL( CBufFlat* aBuffer )
    {
    RDEBUG("RTerminalControlSession::FileScanResultsL");
    
    User::LeaveIfError((aBuffer == NULL?KErrGeneral:KErrNone));

	TInt        status = KErrNone;
	TInt        size = 0;

	status = FileScanResultsSize(size);

	// Resize client buffer (or handle out of memory)
	aBuffer->ResizeL( size );

    //
    // If we get size, we can create object to transfer results
    //
	if ( status == KErrNone )
		{
		// We're using out own data buffer for data transfer from server
		HBufC8 *buf = HBufC8::NewLC( size );
        TPtr8 ptr(buf->Des());    	

        TIpcArgs args1;
        args1.Set(0, &ptr);
		status = SendReceive( EFileScanResults, args1 );

		if ( status == KErrNone )
			{			
            aBuffer->Write(0, ptr);
			}
		else
			{
			RDEBUG_2("TerminalControlServer: EFileScanResults failed... %d", status);
			}

		CleanupStack::PopAndDestroy( buf );
		}
	else
		{
		RDEBUG_2("TerminalControlServer: EFileScanResultsSize failed... %d", status);
		}

	return status;        
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::FileScanResultsSizeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::FileScanResultsSize( TInt &aSize )
    {
    RDEBUG("RTerminalControlSession::FileScanResultsSize");
    
	TInt        status = KErrNone;
	TInt        size = 0;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;
    TLex8       lex;
    
    //
    // Get size of file scan from the server
    //
    status = FileScanResultsSize(sizeBuf);
		if( status != KErrNone )
			{
			return status;	
			}
			
	lex.Assign(sizeBuf);
	status = lex.Val(size);

	aSize = size;
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::FileScanResultsSize
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::FileScanResultsSize( TDes8 &aSizeBuf )
    {
    RDEBUG("RTerminalControlSession::FileScanResultsSize");
    
	TInt        status = KErrNone;
    TIpcArgs    args1;    
    
    //
    // Get size of file scan from the server
    //    
    args1.Set(0, &aSizeBuf);
    status = SendReceive( EFileScanResultsSize, args1 );
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::DeleteFileL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::DeleteFile( const TDesC8& aFileName )
    {
    RDEBUG("RTerminalControlSession::DeleteFile");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    
    args1.Set(0, &aFileName);
    
    status = SendReceive( EFileDelete, args1 );
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetAutolockPeriodL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetAutolockPeriod( TInt32 aPeriod )
    {
    RDEBUG("RTerminalControlSession::SetAutolockPeriodL");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TInt       value( aPeriod );
    
    args1.Set(0, value);
    
    status = SendReceive( EDeviceLock_AutoLockPeriod_Set, args1 );
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetAutolockPeriodL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetAutolockPeriod( TInt32 &aPeriod )
    {    
    RDEBUG("RTerminalControlSession::GetAutolockPeriod");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> period;
    TLex8      lex;
    
    args1.Set(0, &period);
    
    status = SendReceive( EDeviceLock_AutoLockPeriod_Get, args1 );
    
    if(status == KErrNone)
        {
        lex.Assign( period );
        status = lex.Val( aPeriod );
        }
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetMaxAutolockPeriodL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetMaxAutolockPeriod( TInt32 aPeriod )
    {
    RDEBUG("RTerminalControlSession::SetMaxAutolockPeriod");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TInt       value( aPeriod );
    
    args1.Set(0, value);
    
    status = SendReceive( EDeviceLock_MaxAutoLockPeriod_Set, args1 );
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetMaxAutolockPeriodL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetMaxAutolockPeriod( TInt32 &aPeriod )
    {    
    RDEBUG("RTerminalControlSession::GetMaxAutolockPeriod");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> period; 
    TLex8 lex;

    args1.Set(0, &period);    
    status = SendReceive( EDeviceLock_MaxAutoLockPeriod_Get, args1 );
    
    if(status == KErrNone)
        {
        lex.Assign( period );
        status = lex.Val( aPeriod );
        }
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetDeviceLockLevelL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetDeviceLockLevel( TInt32 aLevel )
    {    
    RDEBUG("RTerminalControlSession::SetDeviceLockLevel");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TInt       value( aLevel );
    
    args1.Set(0, value);
    
    status = SendReceive( EDeviceLock_LockLevel_Set, args1 );
       
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetDeviceLockLevelL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetDeviceLockLevel( TInt32 &aLevel )
    {
    RDEBUG("RTerminalControlSession::GetDeviceLockLevel");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> level;
    TLex8 lex;

    args1.Set(0, &level);
    status = SendReceive( EDeviceLock_LockLevel_Get, args1 );
    
    if(status == KErrNone)
        {
        lex.Assign( level );
        status = lex.Val( aLevel );
        }
        
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetDeviceLockCodeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetDeviceLockCode( const TDesC8 &aCode )
    {
    RDEBUG("RTerminalControlSession::SetDeviceLockCodeL");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;

    args1.Set(0, &aCode);

    status = SendReceive( EDeviceLock_LockCode_Set, args1 );

    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetDeviceLockCodeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetDeviceLockCode( TDesC8 &/*aCode*/ )
    {
    RDEBUG("RTerminalControlSession::GetDeviceLockCode");
    
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::DeviceWipeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::DeviceWipe( )
    {
    RDEBUG("RTerminalControlSession::DeviceWipe");
    
    return SendReceive( EDeviceWipe );
    }




// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMinLength
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMinLength( TInt32 aMinLength )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMinLength");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMinLength );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMinLength_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMinLength
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMinLength( TInt32& aMinLength )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMinLength");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMinLength_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMinLength );
        }
        
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMaxLength
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMaxLength( TInt32 aMinLength )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMaxLength");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMinLength );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMaxLength_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMaxLength
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMaxLength( TInt32& aMinLength )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMaxLength");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMaxLength_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMinLength );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeRequireUpperAndLower
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeRequireUpperAndLower( TBool aRequire )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeRequireUpperAndLower");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aRequire );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeRequireUpperAndLower_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeRequireUpperAndLower
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeRequireUpperAndLower( TBool& aRequire )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeRequireUpperAndLower");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeRequireUpperAndLower_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aRequire );
        }
        
    return status;
	}



// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeRequireCharsAndNumbers
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeRequireCharsAndNumbers( TBool aRequire )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeRequireCharsAndNumbers");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aRequire );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeRequireCharsAndNumbers_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeRequireCharsAndNumbers
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeRequireCharsAndNumbers( TBool& aRequire )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeRequireCharsAndNumbers");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> req;
    TLex8 lex;

    args1.Set( 0, &req );
    status = SendReceive( EDeviceLock_PasscodeRequireCharsAndNumbers_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( req );
        status = lex.Val( aRequire );
        }
        
    return status;
	}



// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMaxRepeatedCharacters
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMaxRepeatedCharacters( TInt32 aMaxChars )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMaxRepeatedCharacters");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMaxChars );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMaxRepeatedCharacters_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMaxRepeatedCharacters
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMaxRepeatedCharacters( TInt32& aMaxChars )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMaxRepeatedCharacters");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMaxRepeatedCharacters_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMaxChars );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeHistoryBuffer
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeHistoryBuffer( TInt32 aBufferLength )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeHistoryBuffer");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aBufferLength );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeHistoryBuffer_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeHistoryBuffer
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeHistoryBuffer( TInt32& aBufferLength )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeHistoryBuffer");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeHistoryBuffer_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aBufferLength );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeExpiration
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeExpiration( TInt32 aExpiration )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeExpiration");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aExpiration );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeExpiration_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeExpiration
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeExpiration( TInt32& aExpiration )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeExpiration");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeExpiration_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aExpiration );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMinChangeTolerance
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMinChangeTolerance( TInt32 aMinTolerance )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMinChangeTolerance");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMinTolerance );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMinChangeTolerance_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMinChangeTolerance
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMinChangeTolerance( TInt32& aMinTolerance )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMinChangeTolerance");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMinChangeTolerance_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMinTolerance );
        }
        
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMinChangeInterval
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMinChangeInterval( TInt32 aMinInterval)
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMinChangeInterval");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMinInterval );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMinChangeInterval_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMinChangeInterval
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMinChangeInterval( TInt32& aMinInterval )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMinChangeInterval");
	
    TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMinChangeInterval_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMinInterval );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeCheckSpecificStrings
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeCheckSpecificStrings( TBool aCheckSpecificStrings )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeCheckSpecificStrings");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aCheckSpecificStrings );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeCheckSpecificStrings_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeCheckSpecificStrings
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeCheckSpecificStrings( TBool& aCheckSpecificStrings )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeCheckSpecificStrings");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeCheckSpecificStrings_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aCheckSpecificStrings );
        }
        
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::DisallowSpecificPasscodeString
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::DisallowSpecificPasscodeString( const TDesC8 &aString )
	{
	RDEBUG("RTerminalControlSession::DisallowSpecificPasscodeString");
	
	TInt status = KErrNone;
    TIpcArgs args1;

    args1.Set( 0, &aString );
    status = SendReceive( EDeviceLock_PasscodeDisallowSpecific, args1 );

    return status;	
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::AllowSpecificPasscodeString
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::AllowSpecificPasscodeString( const TDesC8 &aString )
	{
	RDEBUG("RTerminalControlSession::AllowSpecificPasscodeString");
	
	TInt status = KErrNone;
    TIpcArgs args1;

    args1.Set( 0,&aString );
    status = SendReceive( EDeviceLock_PasscodeAllowSpecific, args1 );

    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::ClearSpecificPasscodeStrings
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::ClearSpecificPasscodeStrings()
	{
	RDEBUG("RTerminalControlSession::ClearSpecificPasscodeStrings");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    
    status = SendReceive( EDeviceLock_PasscodeClearSpecificStrings, args1 );
       
    return status;
	}


// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMaxAttempts
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMaxAttempts( TInt32 aMaxAttempts )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeMaxAttempts");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMaxAttempts );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMaxAttempts_Set, args1 );
       
    return status;
	}

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMaxAttempts
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMaxAttempts( TInt32& aMaxAttempts )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeMaxAttempts");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMaxAttempts_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMaxAttempts );
        }
        
    return status;
	}




EXPORT_C TInt RTerminalControlSession::SetPasscodeConsecutiveNumbers( TBool aConsecutiveNumbers )
	{
	RDEBUG("RTerminalControlSession::SetPasscodeConsecutiveNumbers");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aConsecutiveNumbers );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeConsecutiveNumbers_Set, args1 );
       
    return status;
	}


EXPORT_C TInt RTerminalControlSession::GetPasscodeConsecutiveNumbers( TBool& aConsecutiveNumbers )
	{
	RDEBUG("RTerminalControlSession::GetPasscodeConsecutiveNumbers");
	
	TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeConsecutiveNumbers_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aConsecutiveNumbers );
        }
        
    return status;
	}
		
// ----------------------------------------------------------------------------------------
// RTerminalControlSession::SetPasscodeMinSpecialCharacters
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::SetPasscodeMinSpecialCharacters( TInt32 aMinChars )
    {
    RDEBUG("RTerminalControlSession::SetPasscodeMinSpecialCharacters");
    
    TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aMinChars );
    
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeMinSpecialCharacters_Set, args1 );
       
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetPasscodeMinSpecialCharacters
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetPasscodeMinSpecialCharacters( TInt32& aMinChars )
    {
    RDEBUG("RTerminalControlSession::GetPasscodeMinSpecialCharacters");
    
    TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;

    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeMinSpecialCharacters_Get, args1 );
    
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aMinChars );
        }
        
    return status;
    }
		
EXPORT_C TInt RTerminalControlSession::SetPasscodeDisallowSimple( TBool aDisallowSimple )
    {
    RDEBUG("RTerminalControlSession::SetPasscodeDisallowSimple");
    TInt status = KErrNone;
    TIpcArgs args1;
    TInt value( aDisallowSimple );
    args1.Set( 0, value );
    status = SendReceive( EDeviceLock_PasscodeDisallowSimple_Set, args1 );
    return status;
    }

EXPORT_C TInt RTerminalControlSession::GetPasscodeDisallowSimple( TBool& aDisallowSimple )
    {
    RDEBUG("RTerminalControlSession::GetPasscodeDisallowSimple");
    TInt status = KErrNone;
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> length;
    TLex8 lex;
    args1.Set( 0, &length );
    status = SendReceive( EDeviceLock_PasscodeDisallowSimple_Get, args1 );
    if( status == KErrNone )
        {
        lex.Assign( length );
        status = lex.Val( aDisallowSimple );
        }
    return status;
    }

//#endif
// --------- Enhanced features END --------------------



// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetProcessListL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetProcessListL( CBufFlat* aBuffer )
    {
    RDEBUG("RTerminalControlSession::GetProcessListL");
    
    User::LeaveIfError((aBuffer == NULL?KErrGeneral:KErrNone));

	TInt        status = KErrNone;
	TInt        size = 0;

	status = GetProcessListSize(size);

	// Resize client buffer (or handle out of memory)
	aBuffer->ResizeL( size );

    //
    // If we get size, we can create object to transfer results
    //
	if ( status == KErrNone )
		{
		// We're using out own data buffer for data transfer from server
		if( size > 0 ) // size == 0 is not possible if everything works ok
		    {
    		HBufC8 *buf = HBufC8::NewLC( size );
            TPtr8 ptr(buf->Des());    	

            TIpcArgs args1;
            args1.Set(0, &ptr);
    		status = SendReceive( EProcesses, args1 );

    		if ( status == KErrNone )
    			{			
                aBuffer->Write(0, ptr);
    			}
    		else
    			{
    			RDEBUG_2("TerminalControlServer: GetProcessListL failed... %d", status);
    			}

    		CleanupStack::PopAndDestroy( buf );
		    }
		else
		    {
		    // Do nothing
		    }
		}
	else
		{
		RDEBUG_2("TerminalControlServer: GetProcessListL failed... %d", status);
		}

	return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetProcessListSizeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetProcessListSize( TInt &aSize )
    {
    RDEBUG("RTerminalControlSession::GetProcessListSize");
    
	TInt        status = KErrNone;
    TIpcArgs    args1;
	TInt        size = 0;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;
    TLex8       lex;
    
    //
    // Get size of file scan from the server
    //
    GetProcessListSize(sizeBuf);

	lex.Assign(sizeBuf);
	TInt err = lex.Val(size);
	if( err != KErrNone )
		{
		return err;
		}

	aSize = size;
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetProcessListSizeL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetProcessListSize( TDes8& aSizeBuf )
    {
    RDEBUG("RTerminalControlSession::GetProcessListSize");
    
	TInt        status = KErrNone;
    TIpcArgs    args1;    
    
    //
    // Get size of file scan from the server
    //
    args1.Set(0, &aSizeBuf);
    status = SendReceive( EProcessesSize, args1 );
    
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::GetProcessDataL
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::GetProcessDataL(
                    CBufFlat* aBuffer, const TDesC8& aProcessName, const TDesC8 &aData )
    {
    RDEBUG("RTerminalControlSession::GetProcessDataL");
    
    User::LeaveIfError((aBuffer == NULL?KErrGeneral:KErrNone));

	TInt       status = KErrNone;    
    TIpcArgs   args1;
    TFileName8 results;

    args1.Set(0, &aProcessName);
    args1.Set(1, &aData);
    args1.Set(2, &results);

    status = SendReceive( EProcessData, args1 );
    
    aBuffer->InsertL(0, results);

    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::RebootDevice
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::RebootDevice( TBool aRestartDevice/*=ETrue*/ )
    {
    RDEBUG("RTerminalControlSession::RebootDevice");
    
    TIpcArgs   args1;    
    args1.Set(0, aRestartDevice);
    
    return SendReceive( EReboot, args1 );
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::StartNewProcess
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::StartNewProcess( TBool aIsUid, const TDesC8& aProcessName )
    {
    RDEBUG("RTerminalControlSession::StartNewProcess");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    
    args1.Set(0, &aProcessName);
    args1.Set(1, aIsUid?1:0);
    
    status = SendReceive( ELocalOperation_Start, args1 );
               
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControlSession::StopProcess
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControlSession::StopProcess( TBool aIsUid, const TDesC8& aProcessName )
    {
    RDEBUG("RTerminalControlSession::StopProcess");
    
	TInt       status = KErrNone;
    TIpcArgs   args1;
    
    args1.Set(0, &aProcessName);
    args1.Set(1, aIsUid?1:0);
    
    status = SendReceive( ELocalOperation_Stop, args1 );
               
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControl3rdPartySession::Open()
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControl3rdPartySession::Open(RTerminalControl &aServer)
    {
    RDEBUG("RTerminalControl3rdPartySession::Open");
    
    TIpcArgs args1;
    TPtr8 argument0(0, 0);
    args1.Set(0, &argument0);

	return CreateSubSession(aServer, ECreateSubSession, args1);
    }

// ----------------------------------------------------------------------------------------
// RTerminalControl3rdPartySession::Close()
// ----------------------------------------------------------------------------------------
EXPORT_C void RTerminalControl3rdPartySession::Close()
    {
    RDEBUG("RTerminalControl3rdPartySession::Close");
    
	RSubSessionBase::CloseSubSession(ECloseSubSession);    
    }

// ----------------------------------------------------------------------------------------
// RTerminalControl3rdPartySession::SetDeviceLockParameter()
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControl3rdPartySession::SetDeviceLockParameter(TInt aType, const TDesC8& aData)
    {
    RDEBUG("RTerminalControl3rdPartySession::SetDeviceLockParameter");
    
    TIpcArgs args1;

    args1.Set(0, aType);
    args1.Set(1, &aData);

    return SendReceive( E3rdPartySetterSet, args1 );
    }

// ----------------------------------------------------------------------------------------
// RTerminalControl3rdPartySession::GetDeviceLockParameterSize()
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControl3rdPartySession::GetDeviceLockParameterSize(TInt aType)
    {
    RDEBUG("RTerminalControl3rdPartySession::GetDeviceLockParameterSize");
    
    TIpcArgs args1;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> sizeBuf;

    args1.Set(0, aType);
    args1.Set(1, &sizeBuf);

    TInt status = SendReceive( E3rdPartySetterGetSize, args1 );

    if( status == KErrNone )
        {
        TLex8 lex;
        TInt size;
        
        lex.Assign( sizeBuf );
        if ( KErrNone == (lex.Val( size ) ) && (size > 0) )
            {
            return size;
            }
            
        status = KErrGeneral;
        // Flow to error
        }

    // Error:
    return status;
    }

// ----------------------------------------------------------------------------------------
// RTerminalControl3rdPartySession::GetDeviceLockParameter()
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RTerminalControl3rdPartySession::GetDeviceLockParameter(TInt aType, TDes8& aData)
    {
    RDEBUG("RTerminalControl3rdPartySession::GetDeviceLockParameter");
    
    TIpcArgs args1;

    args1.Set(0, aType);
    args1.Set(1, &aData);

    return SendReceive( E3rdPartySetterGet, args1 );
    }


