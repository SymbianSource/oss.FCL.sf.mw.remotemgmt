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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/

#include <f32file.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>
#include <cmpluginpacketdatadef.h>
#include <etel.h> //for telephone mode
#include <etelmm.h> //for telephone mode
#include <fotaConst.h> 
#include "fmsserversession.h"
#include "fmsclientserver.h"

#if defined (__WINS__)
#include <cmpluginlanbasedef.h>
#endif
// ----------------------------------------------------------------------------------------
// CFMSSession::CFMSSession
// ----------------------------------------------------------------------------------------
CFMSSession::CFMSSession()
	{	
	iStopServer  =  EFalse;
	iWlanbearer  =  EFalse;
	iWcdmaBearer =  EFalse;
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::~CFMSSession
// ----------------------------------------------------------------------------------------
CFMSSession::~CFMSSession()
	{	
	FLOG(_L("CFMSSession::~CFMSSession-- begin"));
	if( iStopServer )
		{
		FLOG(_L("Stop fmserver CFMSSession::~CFMSSession"));	
		Server().DropSession();
		}
	FLOG(_L("CFMSSession::~CFMSSession-- end"));
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::Server
// ----------------------------------------------------------------------------------------
CFMSServer& CFMSSession::Server()
	{
	return *static_cast<CFMSServer*>(const_cast<CServer2*>(CSession2::Server()));
	}


// ----------------------------------------------------------------------------------------
// CFMSSession::CheckClientSecureIdL
// ----------------------------------------------------------------------------------------
TBool CFMSSession::CheckClientSecureIdL (
		const RMessage2& aMessage)
	{
	TUid    fota            = TUid::Uid( KFotaServerUid );
	TUid    starter         = TUid::Uid( KStarterUid );

	// Fotaserver
	if ( aMessage.SecureId() == fota.iUid ) 
		{
		FLOG(_L("[CFMSSession] CheckClientSecureIdL client is \
		fotaserver!"));
		return ETrue;
		}

	// Starter
	if ( aMessage.SecureId() == starter.iUid )
		{
		return ETrue;
		}

	// test app
	if ( aMessage.SecureId() == 0x01D278B9 )
		{
		return ETrue;
		}

	FLOG(_L("[CFMSSession] CheckClientSecureIdL client 0x%X is unknown \
	(msg %d), bailing out"),aMessage.SecureId().iId, aMessage.Function());
	iStopServer = ETrue;
	User::Leave( KErrAccessDenied );    
	return EFalse; // compiler warning otherwise
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::ServiceL
// ----------------------------------------------------------------------------------------
void CFMSSession::ServiceL(const RMessage2& aMessage)
	{	
	RThread clt; aMessage.ClientL(clt); TFullName cltnm = clt.FullName();
	FLOG(_L( "CFMSSession::ServiceL      %d   serving for %S?" )
			,aMessage.Function(), &cltnm );
	if( CheckClientSecureIdL( aMessage ) )
		{
		//already one request pending
		if( Server().RequestPending() && 
				aMessage.Function() !=  ECancelOutstandingRequest )
			{
			aMessage.Complete(KErrCancel);
			}
		else //For no pending request case
			{
			TRAPD(err,DispatchMessageL(aMessage));
			if(err != KErrNone ) 
				{
				iStopServer = ETrue;
				}
			aMessage.Complete(err);
			}
		}
	else
		{
		aMessage.Complete(KErrAccessDenied);
		}
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::ServiceError
// ----------------------------------------------------------------------------------------
void CFMSSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{		
	CSession2::ServiceError(aMessage,aError);
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::DispatchMessageL
// ----------------------------------------------------------------------------------------
void CFMSSession::DispatchMessageL(const RMessage2& aMessage)
	{	
	FLOG(_L("CFMSSession::DispatchMessageL-begin"));

	if( aMessage.Function() != EFotaStartUpPlugin && 
			aMessage.Function() != ECancelOutstandingRequest && 
			aMessage.Function() != EUpdMonitorbattery &&
			aMessage.Function() != EUpdPhoneCallActive &&
	        aMessage.Function() != EUpdMonitorPhoneCallEnd )
		{
		 
		 HandleBasedOnBearertypeL(aMessage);
		
			
						
		}
	switch(aMessage.Function())
	{
	case EDLUserInterrupt:
		{
		Server().CreateScheduledReminderL();
		
		if(Server().ChargerTobeMonitered())
			{	
			FLOG(_L("FMSSErver::EUserInterrupt"));			
			Server().WriteToFile(EDLUserInterrupt,aMessage.Int0(),
					(TDriveNumber)aMessage.Int1(),aMessage.Int2(),iWcdmaBearer);
			Server().StartMonitoringL(EDLUserInterrupt);
			}
		else
			{
			WriteDummyFile();
			iStopServer = ETrue;
			}
		}
		break;
	case EDLNetworkInterrupt:
		{
		if(Server().NetworkTobeMonitered())
			{
			FLOG(_L("FMSSErver::ENetworkInterrupt"));						
			//If wlan = on & network interrupt, then stop the server and dont monitor
			//mean other wlan n/w's present but this wlan gone off
			if(iWlanbearer && Server().CheckWlanL())
				{
				WriteDummyFile();
				Server().DeleteFile(EFalse);
				iStopServer = ETrue;
				break;
				}			
			Server().WriteToFile(EDLNetworkInterrupt,aMessage.Int0(),
					(TDriveNumber)aMessage.Int1(),aMessage.Int2(),iWcdmaBearer);			
			Server().AsyncSessionRequestL();
			}
		else
			{
			WriteDummyFile();
			iStopServer = ETrue;
			}
		}
		break;
	case EDLMemoryInterrupt:
		{			
		if(Server().MemoryTobeMonitered())
			{
			FLOG(_L("FMSSErver::EMemoryInterrupt"));				
			Server().MemoryToMonitorL(aMessage.Int2(),(TDriveNumber)aMessage.Int1()); 		
			Server().WriteToFile(EDLMemoryInterrupt,aMessage.Int0(),
					(TDriveNumber)aMessage.Int1(),aMessage.Int2(),iWcdmaBearer);
			Server().StartMonitoringL(EDLMemoryInterrupt);		
			}
		else
			{
			WriteDummyFile();
			iStopServer = ETrue;
			}
		}
		break;
	case EDLGeneralInterrupt:
		{
		if(Server().ChargerTobeMonitered())
			{
			FLOG(_L("FMSSErver::GeneralInterrupt"));	
			Server().WriteToFile(EDLUserInterrupt,aMessage.Int0(),
					(TDriveNumber)aMessage.Int1(),aMessage.Int2(),iWcdmaBearer);
			Server().StartMonitoringL(EDLUserInterrupt);
			}
		else
			{
			WriteDummyFile();
			iStopServer = ETrue;
			}
		}
		break;
	case EFotaStartUpPlugin:
		{	
		if(Server().MoniterAfterPhoneRestart())				
			{
			FLOG(_L("CFMSSession::EFotaStartUpPlugin"));	
			TInt intrreason(-1),bearer(-1),size(0);
			TInt drive(-1), wcdmabearer(-1);
			//if loop for Hard reboot--no file or size is empty
#if defined(__WINS__)
        if(ETrue)
#else
			if(Server().ReadFromFile(intrreason,bearer,drive,size,wcdmabearer) == EFalse )
#endif
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin no file or size is empty"));
				if(!Server().RequestPending())
					{	
					FLOG(_L("CFMSSession::EFotaStartUpPlugin-no request pending"));						  					  	
					TRAPD(err,PhoneModeL());
					if(err); // remove compiler warning
					FLOG(_L("PhoneModeL() fotastartup returns err as %d"),err);
					if(iWcdmaBearer)						 
						Server().NetworkTypeL( (TInt)EBearerIdWCDMA );

					else
						Server().NetworkTypeL( (TInt)EBearerIdGPRS );	  					  					  
					Server().LockSession();
					Server().TriggerFotaL(); //checks n/w & trigger FOTA DL						
					}
				break;
				}												
			TUint32 BearerId = FindBearerIdL(bearer);
			FLOG(_L("CFMSSession::EFotaStartUpPlugin wcdma bearer is %d"),wcdmabearer);
			if( BearerId == KUidWlanBearerType )
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin-wlan bearer"));	
				Server().NetworkTypeL(EBearerIdWLAN);
				if(intrreason == EDLNetworkInterrupt)
					{
					//stop the server , but dont delete file to support in second reboot
					FLOG(_L("wlan bearer & n/w interrupt--stopping the server"));
					Server().DeleteFile(EFalse);
					iStopServer = ETrue;
					break;
					}
				}
			else if( BearerId == KUidPacketDataBearerType )
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin-bearer is packet data"));	
				if( wcdmabearer == 1 )
					{
					Server().NetworkTypeL(EBearerIdWCDMA);	
					FLOG(_L("CFMSSession::EFotaStartUpPlugin-packet data and 3G"));
					}
				else
					{
					Server().NetworkTypeL(EBearerIdGPRS);
					FLOG(_L("CFMSSession::EFotaStartUpPlugin-packet data and 2G"));
					}
				}
			else
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin-unknown bearer"));
				Server().DeleteFile(EFalse);
				iStopServer = ETrue;
				break;
				}
			//when there is dummy file in the store
			if( intrreason < EDLUserInterrupt || intrreason > EDLGeneralInterrupt )
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin-Interrupt unknown"));	
				Server().DeleteFile(EFalse); //used when variation support & reboot again
				iStopServer = ETrue;
				break;
				}
			if( intrreason == EDLUserInterrupt || intrreason == EDLGeneralInterrupt )
				{
				FLOG(_L("CFMSSession::EFotaStartUpPlugin-Check n/w & trigger fotaengine"));	
				Server().CheckNetworkL(bearer,drive,size,wcdmabearer);//3 params used in monitoring if no n/w
				break;
				} 				
			//for other interrupts it will monitor
			Server().StartMonitoringL((TFmsIpcCommands)intrreason);
			}
		else
			{
			iStopServer = ETrue;
			}
		}
		break;
	case ECancelOutstandingRequest:
		{					
		FLOG(_L("CFMSSession::DispatchMessageL ECancelOutstandingRequest case:"));
		Server().DeleteScheduledRemindersL();
		iStopServer = ETrue; 
		}
		break;	

	case EUpdMonitorbattery:
	   {
	    FLOG(_L("CFMSSession::DispatchMessageL EUpdMonitorbattery case:"));
	 	Server().StartBatteryMonitoringL(EUpdMonitorbattery, aMessage.Int0());
	   }
	 	break;
	   
	case EUpdPhoneCallActive:
	    {
	    FLOG(_L("CFMSSession::DispatchMessageL EUpdPhoneCallActive case:"));
	    Server().LockSession();
	    TInt reason =EFalse;
	    Server().CheckPhoneCallActiveL(reason);
	    TPckgBuf<TInt> callactive(reason);
	    aMessage.WriteL(0,callactive);
	    Server().iSessionCount = 0;
	    FLOG(_L("CFMSSession::DispatchMessageL EUpdPhoneCallActive case:%d"),reason);
	    iStopServer = ETrue;
	    }
	    break;
	    
	case EUpdMonitorPhoneCallEnd:
	    {
	    FLOG(_L("CFMSSession::DispatchMessageL EUpdMonitorPhoneCallEnd case:%d,%d"),aMessage.Int0(),aMessage.Int1());
	    Server().LockSession();
	    if(Server().MonitorPhoneCallEndL())
	        {
	        iStopServer = ETrue;
	        Server().iSessionCount = 0;
	        }
	    else //let server monitor for active call end
	        {
	        iStopServer = EFalse;
	        }
	   }
	 	break;
	default:
		{
		FLOG(_L("CFMSSession::DispatchMessageL default case:and a leave happened"));
		iStopServer = ETrue;
		User::Leave(KErrArgument);			
		}			
	}
	FLOG(_L("CFMSSession::DispatchMessageL:end"));
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::FindBearerId
// ----------------------------------------------------------------------------------------
TUint32 CFMSSession::FindBearerIdL(TInt aIapId)
	{
	FLOG(_L("CFMSSession::FindBearerId: % d"),aIapId);
	TUint32 bearer = 0;
	TInt err( KErrNone );
	// Query CM Id
	TInt cmId ( aIapId );			        
	RCmManagerExt  CmManagerExt;
	TRAP( err, CmManagerExt.OpenL() );
	if( err == KErrNone )
		{
		RCmConnectionMethodExt cm;
		TRAP( err, cm = CmManagerExt.ConnectionMethodL( cmId ) );			        	
		if ( err == KErrNone )
			{	        
			CleanupClosePushL( cm );	  
			bearer = cm.GetIntAttributeL( CMManager::ECmBearerType );	        			        
			CleanupStack::PopAndDestroy();  // cm	        
			}	    	        
		CmManagerExt.Close();
		}
	return bearer;
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::WriteDummyFile
// This method used when the variation wouldn't support and startup called
// and also differentiates hard reeboot(with empty or no file)
// ----------------------------------------------------------------------------------------
void CFMSSession::WriteDummyFile()
	{
	FLOG(_L("CFMSSession::WriteDummyFile--begin"));
	Server().WriteToFile(KErrNotFound,KErrNotFound,
			(TDriveNumber)KErrNotFound,KErrNotFound,EFalse);
	Server().DeleteFile(EFalse);
	FLOG(_L("CFMSSession::WriteDummyFile--End"));
	}

// ----------------------------------------------------------------------------------------
// CFMSSession::PhoneMode
// This method used to know the current phone mode
// If the phone is in dual mode it will check GPRS bearer only
// ----------------------------------------------------------------------------------------
void CFMSSession::PhoneModeL()
	{
	FLOG(_L("CFMSSession::PhoneModeL--begin"));	
	// find out if we are in 2G or in 3G - it is needed for calculating bearer availability
	__UHEAP_MARK; 
	RTelServer telServer;
	User::LeaveIfError( telServer.Connect());
	FLOG(_L("CFMSSession::PhoneModeL--telServer Connected"));
	RTelServer::TPhoneInfo teleinfo;
	User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
	FLOG(_L("CFMSSession::PhoneModeL--telServer getting tel info"));
	RMobilePhone phone;
	User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
	FLOG(_L("CFMSSession::PhoneModeL--RMobilePhone opened"));
	User::LeaveIfError(phone.Initialise());	
	FLOG(_L("CFMSSession::PhoneModeL--phone.Initialise() success"));
	RMobilePhone::TMobilePhoneNetworkMode mode;        	        	
	TInt err = phone.GetCurrentMode( mode );
	phone.Close();
	telServer.Close();
	__UHEAP_MARKEND;

	if( KErrNone == err )
		{
		switch(mode)
		{
		case RMobilePhone::ENetworkModeGsm:
		//case RMobilePhone::ENetworkModeUnknown:  // emulator default
			{
			FLOG(_L("CFMSSession::PhoneModeL-begin-GSM Mode"));	                		
			iWcdmaBearer = EFalse;
			Server().NetworkTypeL(EBearerIdGPRS);
			break;		
			}
		case RMobilePhone::ENetworkModeWcdma:
			{                		     		
			FLOG(_L("CFMSSession::PhoneModeL-begin-3G Mode"));	
			iWcdmaBearer = ETrue;
			Server().SetWcdma();
			Server().NetworkTypeL(EBearerIdWCDMA);
			break;	
			}	
		default:        	
		FLOG(_L("unknown"));
		}        				
		}
	FLOG(_L("CFMSSession::PhoneModeL--end"));
	}
	
//----------------------------------------------------------------------------------------
// CFMSSession::HandleBasedOnBearertypeL
// Handles based on bearer type
//----------------------------------------------------------------------------------------	

void  CFMSSession::HandleBasedOnBearertypeL(const RMessage2& aMessage)
{
    TUint32 bearer = FindBearerIdL(aMessage.Int0());
		if( bearer == KUidPacketDataBearerType )
			{
			FLOG(_L("CFMSSession::DispatchMessageL bearer is packet data"));
			TRAPD(err,PhoneModeL());
			if(err); // remove compiler warning
			FLOG(_L("PhoneModeL() returns err as %d"),err);
			}
		else if	(	bearer == KUidWlanBearerType )
			{
			FLOG(_L("CFMSSession::DispatchMessageL bearer is WLAN"));
			iWlanbearer = ETrue;
			Server().NetworkTypeL(EBearerIdWLAN);
			}
#if defined(__WINS__)
		else if (   bearer == KUidLanBearerType )
		    {
            FLOG(_L("CFMSSession::DispatchMessageL bearer is LAN"));
            Server().NetworkTypeL(EBearerIdLAN);
		    }
#endif
		else	
			{
			FLOG(_L("CFMSSession::DispatchMessageL bearer is not known and a leave happened "));
			iStopServer = ETrue;
			User::Leave(KErrArgument);
			}
}				
//End of file
