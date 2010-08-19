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
* Description:  LAWMO Client implementation
*
*/

#include <e32svr.h>
#include <featmgr.h>
#include "dcmoclientserver.h"
#include "lawmoclient.h"
#include "lawmodebug.h"
// Standard server startup code
// 
static TInt StartServer()
	{
	RLDEBUG("from lawmoclient ->starting dcmo server...");
	
	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	TInt r=server.Create(KDCMOServerName,KNullDesC);

	if (r!=KErrNone)
		{
		RLDEBUG_2("lawmoclient -> server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RLDEBUG("lawmoclient -> dcmoserver Started");
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}


// This is the standard retry pattern for server connection
EXPORT_C TInt RLawmoClient::Connect()
    {
    RLDEBUG("RLawmoClient: connect...");
    TInt retry=2;
    for (;;)
        {
        TInt r=CreateSession(KDCMOServerName,TVersion(0,0,0),1);
        if (r!=KErrNotFound && r!=KErrServerTerminated)
            return r;
        if (--retry==0)
            return r;
        RLDEBUG("RLawmoClient: to start server...");
        r=StartServer();
        if (r!=KErrNone && r!=KErrAlreadyExists)
            return r;
        }
    }

// ---------------------------------------------------------------------------
// RLawmoClient::Close()
// ---------------------------------------------------------------------------
EXPORT_C void RLawmoClient::Close()
	{
	RLDEBUG(" RLawmoClient::Close");
	RSessionBase::Close();  //basecall
	   RLDEBUG(" RLawmoClient::close done");
	}

// ----------------------------------------------------------------------------------------
// RLawmoClient::DeviceWipeL
// ----------------------------------------------------------------------------------------
EXPORT_C TLawMoStatus RLawmoClient::DeviceWipe()
    {
    RLDEBUG("RLawmoClient::DeviceWipe Begin");
    TLawMoStatus status(ELawMoAccepted);
    TPckg<TLawMoStatus> statusValue(status);
    TInt err = SendReceive(EWipe,TIpcArgs(&statusValue));
    //TInt err = SendReceive(EWipe);
    if(err != KErrNone)
        status = ELawMoWipeNotPerformed;
    RLDEBUG("RLawmoClient::DeviceWipe End");
    return status;
    }

// ----------------------------------------------------------------------------------------
// RLawmoClient::DeviceWipeAll
// ----------------------------------------------------------------------------------------
EXPORT_C TLawMoStatus RLawmoClient::DeviceWipeAll( )
    {
    RLDEBUG("RLawmoClient::DeviceWipeAll Begin");
    TLawMoStatus status(ELawMoSuccess);
    TPckg<TLawMoStatus> statusValue(status);
    TInt err = SendReceive( EWipeAll,TIpcArgs(&statusValue) );
    if(err != KErrNone)//if sendreceive itself failed
        status = ELawMoWipeFailed;
    RLDEBUG("RLawmoClient::DeviceWipeAll End");
    return status;
    }

// ----------------------------------------------------------------------------------------
// RLawmoClient::SetPasscodeMaxAttempts
// ----------------------------------------------------------------------------------------
EXPORT_C TInt RLawmoClient::GetListItemName( TDes& aItemName, TDes& aNodeValue )
    {
    RLDEBUG("RLawmoClient::GetListItemName Begin");    
    
    TIpcArgs args1;
    TLawMoStatus status(ELawMoOk);
    
    args1.Set( 0, &aItemName );
    args1.Set( 1, &aNodeValue );
    args1.Set( 2, status);
    TInt err = SendReceive( EListItemName_Get, args1 );
    if(err != KErrNone)
        status = ELawMoInvalid;   
    RLDEBUG("RLawmoClient::GetListItemName End");    
    return status;
    }

EXPORT_C TInt RLawmoClient::GetToBeWipedValue( TDes& aItemName, TBool& aWipeVal )
    {
    RLDEBUG("RLawmoClient::GetToBeWipedValue Begin");    
    
    TIpcArgs args1;
    TBuf<255> wipeValue;
    TLex lex;
    
    TLawMoStatus status(ELawMoOk);
    
    args1.Set( 0, &aItemName );
    args1.Set( 1, &wipeValue );
    args1.Set( 2, status);
    
    TInt err = SendReceive( EToBeWiped_Get, args1 );
    
    if( err == KErrNone )
        {
        lex.Assign( wipeValue );
        err = lex.Val( aWipeVal );
        }
    if(err != KErrNone)
        status = ELawMoInvalid;
    
    RLDEBUG("RLawmoClient::GetToBeWipedValue End");    
    return status;
    }

EXPORT_C TInt RLawmoClient::SetToBeWipedValue( TDes& aItemName, TInt aWipeVal )
    {
    RLDEBUG("RLawmoClient::SetToBeWipedValue Begin");        
    TIpcArgs args1;    
    TLawMoStatus status(ELawMoOk);
    
    args1.Set( 0, &aItemName );
    args1.Set( 1, aWipeVal );
    args1.Set( 2, status);
    
    TInt err = SendReceive( EToBeWiped_Set, args1 );
    
    if(err != KErrNone)
        status = ELawMoInvalid;
    
    RLDEBUG("RLawmoClient::SetToBeWipedValue End");    
    return status;
    }
// ----------------------------------------------------------------------------------------
// RLawmoClient::SetPasscodeMaxAttempts
// ----------------------------------------------------------------------------------------
/*EXPORT_C TInt RLawmoClient::GetLawMoState( TDes& aState )
    {
    RLDEBUG("RLawmoClient::GetLawMoState Begin");
    
    TLawMoStatus status(ELawMoSuccess);
    TIpcArgs args1;    
    args1.Set( 0, &aState );
    args1.Set( 1, status);
    TInt err = SendReceive( EState, args1 );
      
    if(err != KErrNone)
        status = ELawMoFail;
    
    RLDEBUG("RLawmoClient::GetLawMoState End");
    return status;
    }*/
