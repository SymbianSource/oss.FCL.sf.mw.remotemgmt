/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
 

#include <mmf/common/mmfcontrollerpluginresolver.h>

#include <badesca.h>
#include <Ecom/ImplementationInformation.h>
#include <e32cmn.h>
#include <f32file.h>
#include <scpclient.h>

#include "debug.h"
#include "SCPClientTestSession_AL.h"
#include "SCPClientTestClientServer_AL.h"


CSCPClientTestSession_AL::CSCPClientTestSession_AL()
	{
	RDEBUG("CSCPClientTestSession_AL::CSCPClientTestSession_AL");
	}


CSCPClientTestSession_AL::~CSCPClientTestSession_AL()
	{
	RDEBUG("CSCPClientTestSession_AL::~CSCPClientTestSession_AL");
	Server().DropSession();
	}

CSCPClientTestServer_AL& CSCPClientTestSession_AL::Server()
	{
	return *static_cast<CSCPClientTestServer_AL*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CSCPClientTestSession_AL::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CSCPClientTestSession_AL::CreateL()
	{
	RDEBUG("CSCPClientTestSession_AL::CreateL");
	Server().AddSession();
	
	}

void CSCPClientTestSession_AL::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CSCPClientTestSession_AL::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CSCPClientTestSession_AL::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
	
void CSCPClientTestSession_AL::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG("CSCPClientTestSession_AL::DispatchMessageL");
	TInt ret = KErrNone;
	switch(aMessage.Function())
		{
		case ESCPClientTestQueryAdminCmd:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			ret = scpClient.QueryAdminCmd( ESCPCommandLockPhone );
			if(ret == KErrNone)
			{
				ret = scpClient.QueryAdminCmd( ESCPCommandUnlockPhone );		
			}
			scpClient.Close();
		}
		break;
		case ESCPClientTestGetParam:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			TBuf<KSCPMaxIntLength> maxPeriodBuf;
			ret = scpClient.GetParamValue( ESCPMaxAutolockPeriod, maxPeriodBuf );
			scpClient.Close();
		}
		break;
		case ESCPClientTestCheckConfig:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			ret = scpClient.CheckConfiguration( KSCPInitial );
			if(ret == KErrNone)
			{
				ret = scpClient.CheckConfiguration( KSCPComplete );		
			}
			scpClient.Close();		
		}
		break;
		default:
			break;
		}
		TPckgBuf<TInt> retPackage(ret);
		aMessage.WriteL(0,retPackage);
	}
	
//end of file

