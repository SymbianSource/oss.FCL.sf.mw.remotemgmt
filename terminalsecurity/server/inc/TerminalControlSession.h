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


#ifndef __TERMINALCONTROL_SESSION_H__
#define __TERMINALCONTROL_SESSION_H__

// INCLUDES
#include <e32base.h>
#include "TerminalControlServer.h"

class CTcTrustedSession;

/**
*  CTerminalControlSession 
*  Description.
*/

class CTerminalControlSession : public CSession2
	{

public:
	CTerminalControlSession();
	void CreateL();

private:
	~CTerminalControlSession();
	CTerminalControlServer& Server();

	void                ServiceL                    ( const RMessage2& aMessage );
	void                DispatchMessageL            ( const RMessage2& aMessage );
	void                ServiceError                ( const RMessage2& aMessage, TInt aError );

	void                NewTrustedSessionL          ( const RMessage2& aMessage );
	CTcTrustedSession*  TrustedSessionFromHandle    ( const RMessage2& aMessage );
	void                DeleteTrustedSession        ( const RMessage2& aMessage );

	TInt                SetDeviceLockParameterL     ( const RMessage2 &aMessage );
	TInt                GetDeviceLockParameterSizeL ( const RMessage2 &aMessage );
	TInt                GetDeviceLockParameterL     ( const RMessage2 &aMessage );
	void                GetDeviceLockParameterL     ( TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> &sizeBuf, TInt aType );
	
	HBufC8*             CopyParameterL              ( const RMessage2 &aMsg, TInt aIndex );

    /**
    * Checks if device memory is encrypted or not. The check is only done for phone memory.
    * @return TBool. ETrue if phone memory is encrypted, otherwise EFalse.
    */  
    TBool IsDeviceMemoryEncrypted();
    
private:	
	TInt            iSubsessionCount;
	CObjectCon      *iContainer; 		// object container for this session
	CObjectIx       *iTrustedSessions; 	// object index which stores objects for this session
};

#endif //__TERMINALCONTROL_SESSION_H__
