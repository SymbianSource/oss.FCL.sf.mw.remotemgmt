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
#include "SCPClientTestSession.h"
#include "SCPClientTestClientServer.h"

CSCPClientTestSession::CSCPClientTestSession()
	{
	RDEBUG("CSCPClientTestSession::CSCPClientTestSession");
	}

CSCPClientTestSession::~CSCPClientTestSession()
	{
	RDEBUG("CSCPClientTestSession::~CSCPClientTestSession");
	Server().DropSession();
	}

CSCPClientTestServer& CSCPClientTestSession::Server()
	{
	return *static_cast<CSCPClientTestServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

void CSCPClientTestSession::CreateL()
	{
	RDEBUG("CSCPClientTestSession::CreateL");
	Server().AddSession();
	}

void CSCPClientTestSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CSCPClientTestSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CSCPClientTestSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}	

void CSCPClientTestSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG("CSCPClientTestSession::DispatchMessageL");
	TInt ret = KErrNone;
	switch(aMessage.Function())
		{
		case ESCPClientTestConstructor:
		{
			RSCPClient scpClient;
			ret = KErrNone;
		}
		break;
		case ESCPClientTestConnect:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
            scpClient.Close();
            ret = KErrNone;
		}
		break;
		case ESCPClientTestVersion:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
            ret = KErrNone;
			TVersion version = scpClient.Version();
			scpClient.Close();
		}
		break;
		case ESCPClientTestGetCode:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			TSCPSecCode tmpCode;
    		tmpCode.Zero();   
       		ret = scpClient.GetCode( tmpCode );
       		scpClient.Close();    		 	
		}
		break;
		case ESCPClientTestSetCode:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			RMobilePhone::TMobilePassword password;
			_LIT(KDefaultPassowrd, "12345");
			password.Copy(KDefaultPassowrd);
			TSCPSecCode newCode;
        	newCode.Copy( password );
        	ret = scpClient.StoreCode( newCode );
        	scpClient.Close();	
		}
		break;
		case ESCPClientTestChangeCode:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			TBuf<KSCPMaxEnhCodeLen> codebuf;
    		codebuf.Zero();
    		TInt newCode1 = 12345;
    		codebuf.AppendNum( newCode1 );
    		ret = scpClient.ChangeCode( codebuf );
    		scpClient.Close();	
		}
		break;
		case ESCPClientTestSetPhoneLock:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
            ret = scpClient.SetPhoneLock( ETrue ) ;	
			if(ret == KErrNone)
			{
				ret = scpClient.SetPhoneLock( EFalse ) ;	
			}
			scpClient.Close();
		}
		break;
		case ESCPClientTestGetLockState:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			TBool state;
			ret = scpClient.GetLockState( state );
			scpClient.Close();
		}
		break;
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
		case ESCPClientTestSetParam:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			TInt newPeriod = 10;
       	
    		TBuf<16> setBuf1;
    		setBuf1.Zero();
    		setBuf1.AppendNum( newPeriod );
    		ret = scpClient.SetParamValue( ESCPAutolockPeriod, setBuf1 );
    		if(ret == KErrNone)
    		{
    			ret = scpClient.SetParamValue( ESCPMaxAutolockPeriod, setBuf1 );	
    		}
    		TInt policy = 1;
    		TBuf<1> setBuf2;
    		setBuf2.Zero();
    		setBuf2.AppendNum( policy == 1 ? 0 : 1 );
    		ret = scpClient.SetParamValue( ESCPCodeChangePolicy, setBuf2 );
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
		case ESCPClientTestAuthenticateS60:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
            RMobilePhone::TMobilePassword password;
//			_LIT(KDefaultPassowrd, "12345");
			ret = scpClient.SecCodeQuery( password, RSCPClient::SCP_OK_CANCEL, EFalse, 0 ); 
			scpClient.Close();
		}
		break;
		case ESCPClientTestCodeChangeQuery:
		{
			RSCPClient scpClient;
            User::LeaveIfError( scpClient.Connect() );
			ret = scpClient.ChangeCodeRequest();
			scpClient.Close();
		}
		break;
		case ESCPClientTestPerformCleanUp:
		    {

            RSCPClient scpClient;
            CleanupClosePushL(scpClient);
            
            User::LeaveIfError( scpClient.Connect() );
            // Fill RArray from aMessage
            
             /** // Copy the client data into a local buffer*/
            TInt32 lCount = aMessage.GetDesLength(1);
            
            // Atleast one application id has to be present in the received message (atleast 8 bytes)
            if(lCount < sizeof(TInt32)) {
                ret = KErrArgument;
                break;
            }

            TUid lUid;
            RArray<TUid> uids;
            CleanupClosePushL(uids);
            
            HBufC8* lBuffer = HBufC8 :: NewLC(lCount);
            TPtr8 bufPtr = lBuffer->Des();
            aMessage.ReadL(1, bufPtr);
            lCount /= sizeof(TInt32);
            
            RDesReadStream lBufReadStream(bufPtr);
            CleanupClosePushL(lBufReadStream);           
            
            for(TInt i=0; i < lCount; i++) {
                lUid.iUid = lBufReadStream.ReadInt32L();
                uids.AppendL(lUid);
            }
            
            CleanupStack :: PopAndDestroy(2); // lBuffer lBufReadStream
            
            TRAPD(err, ret = scpClient.PerformCleanupL(uids));
            
            if(err)
            {
                ret = err;
            }
            
            scpClient.Close();
            CleanupStack :: PopAndDestroy(2); // uids, scpClient
        }
        break;
		default:
			break;
		}
		TPckgBuf<TInt> retPackage(ret);
		aMessage.WriteL(0,retPackage);
	}
	
// end of file

