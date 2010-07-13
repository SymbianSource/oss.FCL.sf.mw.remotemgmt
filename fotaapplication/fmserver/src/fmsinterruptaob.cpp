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

#include <systemwarninglevels.hrh>
#include <e32property.h>
#include <startupdomainpskeys.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <sysutil.h> 
#include <fotaengine.h>
#include "fotaserverPrivateCRKeys.h"
#include "fmsserver.h"
#include "fmsinterruptaob.h"
#include "fmsclientserver.h"
#include "FotaIPCTypes.h"

#if defined (__WINS__)
#include <cmpluginlanbasedef.h>
#endif
// ----------------------------------------------------------------------------------------
// static method StaticCheckNwRequestL() called to check n/w availability
// ----------------------------------------------------------------------------------------
static TInt StaticCheckNwRequestL(TAny* aPtr)
	{
	CFMSInterruptAob* ptr = (CFMSInterruptAob*) aPtr;
	FLOG(_L("StaticCheckNwRequestL:-Begin"));	
	TRAPD(err, ptr->NetworkRegistryCheckL() );	
	FLOG(_L("StaticCheckNwRequestL:-end"));
	return err;
	}

// -----------------------------------------------------------------------------
// CFMSInterruptAob::CFMSInterruptAob()
// -----------------------------------------------------------------------------
CFMSInterruptAob::CFMSInterruptAob()
:CActive(EPriorityStandard)
	{
	//CActiveScheduler::Add(this);
	iConnStatus = KErrNone;
	iWlanAvailable = EFalse;
	iGPRSAvail = EFalse;
	iWcdma = EFalse;
	iUpdInterruptType = ENoInterrupt;
	iChargeToMonitor = 0;
	}

// -----------------------------------------------------------------------------
// CFMSInterruptAob::ConstructL()
// -----------------------------------------------------------------------------
void CFMSInterruptAob::ConstructL()
	{
	CActiveScheduler::Add(this);	
	User::LeaveIfError(iConnMon.ConnectL());
	}

// -----------------------------------------------------------------------------
// CFMSInterruptAob::Start()
// -----------------------------------------------------------------------------
void CFMSInterruptAob::StartL(TFmsIpcCommands aType)
	{	
	FLOG(_L("CFMSInterruptAob::Start- begin"));
	iInterruptType = aType;
	if(iInterruptType == EDLUserInterrupt && iServer->ChargerTobeMonitered())
		{
		FLOG(_L("CFMSInterruptAob::Start--EUserInterrupt"));
		if(!IsActive())
			{
			//RProperty pw;
			iProperty.Attach(KPSUidHWRMPowerState,KHWRMChargingStatus);
			iStatus=KRequestPending;
			iProperty.Subscribe(iStatus);	
			SetActive();	
			}
		}
	else if(iInterruptType == EDLNetworkInterrupt && iServer->NetworkTobeMonitered())
		{	  
		FLOG(_L("CFMSInterruptAob::Start--ENetworkInterrupt"));	
		if(!IsActive())
			{		  
			iStatus=KRequestPending;			  
			TBool phonenotoffline = CheckGlobalRFStateL();
			if ( ( iBearer == EBearerIdGPRS || iBearer == EBearerIdWCDMA )
					&& phonenotoffline )
				{
				FLOG(_L("CFMSInterruptAob::Start--ENetworkInterrupt iBearer is %d"),iBearer);	
				NotifyL();
				return;			  
				}
			else if ( iBearer == EBearerIdWLAN )
				{
				//feature supported check
				iWlanAvailable = EFalse;	  
				iConnMon.GetBoolAttribute( EBearerIdWLAN, 
						0, 
						KBearerAvailability, 
						iWlanAvailable, 
						iStatus );
				SetActive();
				}
			else
				{
				FLOG(_L("CFMSInterruptAob::Start--ENetworkInterrupt--other then GPRS/ WLan bearer "));	  
				//Stop the server	  
				//Dont delete the file if interrupt is network + offline( to differentiate from hard reboot
				iServer->DeleteFile(EFalse);
				StopServer();
				}	  
			}
		}
	else if(iInterruptType == EDLMemoryInterrupt && iServer->MemoryTobeMonitered())
		{
		FLOG(_L("CFMSInterruptAob::Start--EMemoryInterrupt on drive %d"),(TInt)iDrive );		  
		if(!IsActive())
			{
			User::LeaveIfError(iFs.Connect());
			TBool critical = EFalse;
			TInt err = KErrNone;
			err=IsPkgSizeMoreThanWL(critical);		  
			if(critical)
				{	 
				FLOG(_L("CFMSInterruptAob::Start--EMemoryInterrupt--critical pkg dont fit on drive %d"),(TInt)iDrive );		  
				iStatus=KRequestPending;					
				iFs.NotifyDiskSpace( iSize, iDrive, iStatus ); 
				FLOG(_L("CFMSInterruptAob::Start--EMemoryInterrupt-setting as active") );		  
				SetActive();	
				}
			else
				{
				if(err == KErrNone)
					{
					FLOG(_L("CFMSInterruptAob::Start--EMemoryInterrupt--critical pkg fits on drive %d"),(TInt)iDrive );
					LaunchFotaEngineL();
					}
				else
					{
					FLOG(_L("CFMSInterruptAob::Start--EMemoryInterrupt--System error %d on memory drive %d"), err,(TInt)iDrive );
					}
				//Stop the server	 
				StopServer();	   
				}
			}
		}  
	else //case where variation fails or by chance if a bad interrupt reason
		{
		StopServer();	  
		}	 
	}

// -----------------------------------------------------------------------------
// CFMSInterruptAob::RunL()
// -----------------------------------------------------------------------------
void CFMSInterruptAob::RunL()
	{
	FLOG(_L("CFMSInterruptAob::RunL()- begin"));
	TInt value = 0;
	if( EUpdMonitorPhoneCallEnd == iUpdInterruptType)
      {
      FLOG(_L("RunL EPhoneCallActiveAsync--call status %d"),iCallStatus);
      if(iStatus == KErrNone && RCall::EStatusIdle == iCallStatus )
          {
          //call fota scheduler & Stop the server          
          LaunchFotaUpdate();
          StopServer();
          }
      else //call in progress, subscribe again
          {
          FLOG(_L("call in progress, subscribing again"));
          iLine.NotifyStatusChange(iStatus, iCallStatus);
          SetActive();
          }
      }
	else if( iUpdInterruptType == EUpdMonitorbattery)
	  {
	  	 FLOG(_L("CFMSInterruptAob::RunL()- handling battery monitor"));
	  	 iProperty.Attach(KPSUidHWRMPowerState,KHWRMBatteryLevel);
		   TInt err=iProperty.Get(KPSUidHWRMPowerState,KHWRMBatteryLevel,value);
		   if(err != KErrNone)
		    { 
		    	FLOG(_L("CFMSInterruptAob::RunL()- error  in getiing the battery level")); 
		    	StopServer();
		    }  
		   if(value >= iChargeToMonitor)
	    	{ iProperty.Cancel();
			    iProperty.Close();	
			  TRAPD(err, LaunchFotaScheduleUpdate());
			  if(err)
			  {
			  	FLOG(_L("CFMSInterruptAob::RunL()- error in launch fota schedule"));
			  }
            iUpdInterruptType = ENoInterrupt;
            StopServer();
			   
		    }
		
	    	else 
		   	{			
		    	FLOG(_L("CFMSInterruptAob::RunL()--status charger not connected"));
			   if(!IsActive())
			  	{
				  iStatus=KRequestPending;	
				  iProperty.Subscribe(iStatus);
				  FLOG(_L("CFMSInterruptAob::RunL()--waiting for charger Connectivity"));
				  SetActive();
				  }	
	       }
	  }     	
  else if( iInterruptType == EDLUserInterrupt)
		{
		TInt value;	
		//RProperty   pw;
		iProperty.Attach(KPSUidHWRMPowerState,KHWRMChargingStatus);
		iProperty.Get(KPSUidHWRMPowerState,KHWRMChargingStatus,value);
		// charger connected or already charging going on
		if( value == EChargingStatusCharging /*|| value == EChargingStatusChargingComplete ||
				value == EChargingStatusAlmostComplete || value == EChargingStatusChargingContinued */)
			{	
			 	FLOG(_L("CFMSInterruptAob::RunL()--status charging"));
			  HandleChargingInterruptL();			
						
			}
		else 
			{			
			FLOG(_L("CFMSInterruptAob::RunL()--status charger not connected"));
			if(!IsActive())
				{
				iStatus=KRequestPending;	
				iProperty.Subscribe(iStatus);
				FLOG(_L("CFMSInterruptAob::RunL()--waiting for charger Connectivity"));
				SetActive();
				}				
			}
		}
	else if( iInterruptType == EDLNetworkInterrupt )
		{
		FLOG(_L("CFMSInterruptAob::RunL()--ENetworkInterrupt" ));	
		/*if( iBearer == EBearerIdGPRS || iBearer == EBearerIdWCDMA )
			{
			FLOG(_L("CFMSInterruptAob::RunL()--ENetworkInterrupt:GPRS/WCDMA" ));
			}*/
		 if ( iBearer == EBearerIdWLAN )
			{
			TBuf<10> num;
			FLOG(_L("CFMSInterruptAob::RunL()--ENetworkInterrupt due to bearer id WLAN, iStatus %d"),iStatus.Int());	
			/*if(iWlanAvailable)
				FLOG(_L("iWlanAvailable :TRUE"));
			else
				FLOG(_L("iWlanAvailable :FALSE"));	*/				
			if( iWlanAvailable && iStatus == KErrNone )
				{
				FLOG(_L("CFMSInterruptAob::RunL()--status WLAN networks found"));		
				LaunchFotaEngineL();
				StopServer();				
				}
			else if( iStatus == KErrNotSupported )//feture not supported
				{
				FLOG(_L("CFMSInterruptAob::RunL()--status WLAN feature not supported"));
				StopServer();				
				}
			else 
				{
				FLOG(_L("CFMSInterruptAob::RunL()--status WLAN networks not found"));  			  
				if(!IsActive())
					{	
					iWlanAvailable = EFalse;
					iStatus=KRequestPending;	
					iConnMon.GetBoolAttribute( EBearerIdWLAN, 
							0, 
							KBearerAvailability, 
							iWlanAvailable, 
							iStatus );
					FLOG(_L("CFMSInterruptAob::RunL()--search for wlan availability"));
					SetActive();
					}		   
				}	
			}
		}	
	else if( iInterruptType == EDLMemoryInterrupt )
		{
		FLOG(_L("CFMSInterruptAob::RunL()--EMemoryInterrupt,iStatus %d"),iStatus.Int());			
		if( iStatus == KErrNone  || iStatus == KErrCancel )
			{
			TBool critical = EFalse;
			IsPkgSizeMoreThanWL(critical);
			//if download size doesn't lead to memory less than critical level
			if( !critical )	
				{
				FLOG(_L("CFMSInterruptAob::RunL()--Ok download not greater than critical level"));
				//Check network status at this moment
				
				HandleAboveCriticallevelL();
				
				
				}
			else
				{
				FLOG(_L("CFMSInterruptAob::RunL()--Ok download greater than critical level"));
				// space monitor for this		
				if(!IsActive())
					{
					iStatus=KRequestPending;					
					iFs.NotifyDiskSpace( iSize, iDrive, iStatus ); 
					SetActive(); 	
					}			
				}
			}			
		else // for size request more than disk space
			{
			FLOG(_L("CFMSInterruptAob::RunL()--bad request size greater than the disk limit"));		
			StopServer();			
			}		
		}
	else //general
		{
		//Already ahandled in session class
		}
	FLOG(_L("CFMSInterruptAob::RunL()- End"));
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::DoCancel() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::DoCancel()
	{
	FLOG(_L("CFMSInterruptAob::::DoCancel()"));
	if( iStatus == KRequestPending )
		{
		FLOG(_L("CFMSInterruptAob::::DoCancel()--request pending"));
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrCancel );
		}
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::MemoryToMonitorL() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::MemoryToMonitorL(TInt aSize, TDriveNumber aDrive)
	{
	FLOG(_L("CFMSInterruptAob::::MemoryToMonitorL()--Begin"));
	if( aSize < 0 || (aDrive < EDriveA && aDrive > EDriveZ) )
		{
		FLOG(_L("CFMSInterruptAob::::MemoryToMonitorL()--Leving with KErrArgumnet"));
		User::Leave(KErrArgument);
		}
	iDrive = aDrive;
	iSize = aSize;
	CheckMemSizeL();				
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::NetworkTypeL() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::NetworkTypeL(TInt aBearer)
	{	
	if( aBearer == EBearerIdGPRS || aBearer == EBearerIdWLAN
									|| aBearer == EBearerIdWCDMA 
#if defined (__WINS__)									
									|| aBearer == EBearerIdLAN)
#else
                                                                 )
#endif
		{
		iBearer = aBearer;		
		}
	else
		{
		FLOG(_L("CFMSInterruptAob::::NetworkTypeL()--leaving with KErrArgument"));
		User::Leave(KErrArgument);
		}
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::IsPkgSizeMoreThanWL() 
// ----------------------------------------------------------------------------------------
TInt CFMSInterruptAob::IsPkgSizeMoreThanWL(TBool& aCritical)
	{
	FLOG(_L("CFMSInterruptAob::::IsPkgSizeMoreThanWL()--Begin"));
	TInt err = KErrNone;	
	//Already checked the drive is proper or not
	//here the check happened for warning level
	TRAP(err,aCritical = SysUtil::DiskSpaceBelowCriticalLevelL ( &iFs, iSize, iDrive ));
	FLOG(_L("CFMSInterruptAob::::IsPkgSizeMoreThanWL() end %d"),err);
	return err;
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::CheckNetworkL() 
// ----------------------------------------------------------------------------------------
TBool CFMSInterruptAob::CheckNetworkL()
	{	
	FLOG(_L("CFMSInterruptAob::CheckNetworkL--begin"));
	TRequestStatus status=KRequestPending;	
	iConnStatus = KErrNone; 	  
	TBool phonenotoffline = CheckGlobalRFStateL();	  
	if ( phonenotoffline && iBearer == EBearerIdGPRS )
		{
		TRequestStatus gsmstatus = KRequestPending;	
		iConnStatus = KErrNotFound;		
		FLOG(_L("CFMSInterruptAob::CheckNetworkL--check GSM registry"));	
		iConnMon.GetIntAttribute( EBearerIdGSM, 0,KNetworkRegistration, iConnStatus, gsmstatus );
		User::WaitForRequest(gsmstatus);
		FLOG(_L("iConnStatus is %d gsmstatus is %d"),iConnStatus,gsmstatus.Int());
		if( iConnStatus == ENetworkRegistrationHomeNetwork || 
				iConnStatus == ENetworkRegistrationRoaming ) 
			{
			FLOG(_L("CFMSInterruptAob::CheckNetworkL--GSM available"));
			FLOG(_L("CFMSInterruptAob::CheckNetworkL--checking GPRS"));
			iConnMon.GetBoolAttribute( EBearerIdGPRS, 0,KPacketDataAvailability,
					iGPRSAvail, status );
			User::WaitForRequest(status);
			if( iGPRSAvail && status.Int()==KErrNone )
				{
				FLOG(_L("CFMSInterruptAob::CheckNetworkL--GPRS available"));
				return ETrue;
				}
			}
		}
	else if ( phonenotoffline && iBearer == EBearerIdWCDMA )
		{
		TRequestStatus gsmstatus = KRequestPending;	
		iConnStatus = KErrNotFound;		
		FLOG(_L("CFMSInterruptAob::CheckNetworkL--check WCDMA sim registry"));	
		iConnMon.GetIntAttribute( EBearerIdWCDMA, 0,KNetworkRegistration, iConnStatus, gsmstatus );
		User::WaitForRequest(gsmstatus);
		FLOG(_L("iConnStatus is %d WCDMA gsmstatus is %d"),iConnStatus,gsmstatus.Int());
		if( iConnStatus == ENetworkRegistrationHomeNetwork || 
				iConnStatus == ENetworkRegistrationRoaming ) 
			{
			FLOG(_L("CFMSInterruptAob::CheckNetworkL--WCDMA GSM available"));
			FLOG(_L("CFMSInterruptAob::CheckNetworkL--checking WCDMA GPRS"));
			iConnMon.GetBoolAttribute( EBearerIdWCDMA, 0,KPacketDataAvailability,
					iGPRSAvail, status );
			User::WaitForRequest(status);
			if( iGPRSAvail && status.Int()==KErrNone )
				{
				FLOG(_L("CFMSInterruptAob::CheckNetworkL--WCDMA GPRS available"));
				return ETrue;
				}
			}
		}
	else if ( iBearer == EBearerIdWLAN )
		{
		FLOG(_L("CFMSInterruptAob::CheckNetworkL--checking WLAN"));	  
		iWlanAvailable = EFalse;	  
		iConnMon.GetBoolAttribute( EBearerIdWLAN, 
				0, 
				KBearerAvailability, 
				iWlanAvailable, 
				status );	
		User::WaitForRequest(status);//Synchronous call
		if( iWlanAvailable && status == KErrNone )
			{
			FLOG(_L("CFMSInterruptAob::CheckNetworkL--wlan available"));
			return ETrue;
			}
		}
	else //phone is offline
		{
		FLOG(_L("CFMSInterruptAob::CheckNetworkL--phone is offline or invalid bearer"));
		}
	FLOG(_L("CFMSInterruptAob::CheckNetworkL--end"));
	return EFalse;
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::CheckGlobalRFStateL() 
// ----------------------------------------------------------------------------------------
TBool CFMSInterruptAob::CheckGlobalRFStateL()
	{		
	TInt val = KErrNone;
	TInt err = KErrNone;
	TInt status = EFalse;
	err = RProperty::Get(KPSUidStartup, KPSGlobalSystemState, val);
	if ( err == KErrNone && val == ESwStateNormalRfOn )
		{
		//Phone is not offline. Check for Network Registration status
		FLOG(_L("Phone is online. Check for Network Registration status"));		
		status = ETrue;
		}
	else
		{
		//Phone is offline. No Network activities allowed.
		FLOG(_L("Phone is offline. No Network activities allowed."));		
		status = EFalse;
		}
	FLOG(_L("CFotaNetworkRegStatus::CheckGlobalRFState, status = %d <<"),status);	
	return status;
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::StopServer() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::StopServer()
	{
	FLOG(_L("CFMSInterruptAob::StopServer begin"));	
	iServer->iSessionCount = 0;
	CActiveScheduler::Stop();//---for stopping the server
	FLOG(_L("CFMSInterruptAob::StopServer:server stopped"));	
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::LaunchFotaEngineL() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::LaunchFotaEngineL()
	{
	FLOG(_L("CFMSInterruptAob::LaunchFotaEngineL call to iFotaEngine.OpenL() "));
	TInt err = KErrNone;
    iServer->DeleteScheduledRemindersL();
	TRAP(err,iFotaEngine.OpenL());
	FLOG(_L("iFotaEngine.OpenL() launched withe error as %d"),err);
	if(err == KErrNone)
		{
        RFotaEngineSession::TState state = iFotaEngine.GetState(-1);
        if (state == RFotaEngineSession::EDownloadProgressing)
            {
            FLOG(_L("Trying resume download..."));
            err = iFotaEngine.TryResumeDownload();
            }
        else if (state == RFotaEngineSession::EStartingUpdate)
            {
            FLOG(_L("Trying resume update..."));
            err = iFotaEngine.Update(-1,(TSmlProfileId)1, _L8(""), _L8(""));
            }
            
        FLOG(_L("Fota engine returned : %d"), err);
		iFotaEngine.Close();
		FLOG(_L("CFMSInterruptAob::LaunchFotaEngineL END - fotaengine closed "));
		}
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::CheckMemSizeL() 
// ----------------------------------------------------------------------------------------
void CFMSInterruptAob::CheckMemSizeL(/*TInt& aMemoryNeeded*/)
	{
	FLOG(_L("CFMSInterruptAob::CheckMemSizeL-begin"));
	iSize = iSize + KDRIVECWARNINGTHRESHOLD; //irrespective of Drive
	FLOG(_L(" neededspace = sizeNeededFromFS + KDRIVECWARNINGTHRESHOLD: %d"), iSize);
	FLOG(_L("CFMSInterruptAob::CheckMemSizeL-end"));
	}

// ----------------------------------------------------------------------------------------
// CFMSInterruptAob::~CFMSInterruptAob() 
// ----------------------------------------------------------------------------------------
CFMSInterruptAob::~CFMSInterruptAob()
	{ 
	FLOG(_L("CFMSInterruptAob::~CFMSInterruptAob()"));  	
	if(iPeriodicNwCheck)
		{
		FLOG(_L("CFMSInterruptAob::~CFMSInterruptAob()--deleting iPeriodicNwCheck"));
		delete iPeriodicNwCheck;
		iPeriodicNwCheck = NULL;
		}
	StopNotify();
	Cancel();
	iConnMon.Close();
	if(iProperty.Handle())
		{    	
		iProperty.Close();
		FLOG(_L("CFMSInterruptAob::~CFMSInterruptAob()--iProperty closed")); 
		}
	
	if(iInterruptType == EDLMemoryInterrupt)
		{
		iFs.NotifyDiskSpaceCancel();
		}
	iFs.Close();	
	if(iLine.SubSessionHandle())
	    {
	    iLine.Close();
	    }
	if(iPhone.SubSessionHandle())
	    {
	    iPhone.Close();
	    }
	if(iTelServer.Handle())
	    {
	    iTelServer.Close();
	    }
	FLOG(_L("CFMSInterruptAob::~CFMSInterruptAob()--End"));  
	}


// ---------------------------------------------------------------------------
// CFMSInterruptAob::RunError
// ---------------------------------------------------------------------------
//
TInt CFMSInterruptAob::RunError( TInt aError )
	{
	FLOG( _L("CAOConnectionImpl::RunError: aError: %d"), aError );        
	return aError;
	}


//
// -----------------------------------------------------------------------------
// CFMSInterruptAob::NotifyL()
// Registers with connection monitor for notification
// -----------------------------------------------------------------------------
//
void CFMSInterruptAob::NotifyL()
	{
	FLOG(_L("CFMSInterruptAob::NotifyL Begin  "));
	iConnMon.NotifyEventL( *this );
	FLOG(_L("CFMSInterruptAob::NotifyL, after registering  "));
	// Threholds
	TInt    err = iConnMon.SetUintAttribute( iBearer, 
			0, 
			KBearerAvailabilityThreshold, 
			1 );    
	FLOG(_L("CFMSInterruptAob::NotifyL : END "));

	}


// -----------------------------------------------------------------------------
// CFMSInterruptAob::StopNotify()
// Cancels notification with connection monitor
// -----------------------------------------------------------------------------
//
void CFMSInterruptAob::StopNotify()
	{
	FLOG(_L("CFMSInterruptAob::StopNotify Begin  "));
	iConnMon.CancelNotifications();
	FLOG(_L("CFMSInterruptAob::StopNotify end "));
	}	

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::EventL()
// Method involed by connection monitor with current event. 
// This method handles the events
// -----------------------------------------------------------------------------
//
void CFMSInterruptAob::EventL( const CConnMonEventBase& aConnMonEvent )
	{
	FLOG(_L("CFMSInterruptAob::EventL, BEGIN"));
	FLOG(_L("CFMSInterruptAob::EventL event is ==  %d"), aConnMonEvent.EventType()  );
	switch ( aConnMonEvent.EventType() )
	{
	case EConnMonNetworkStatusChange:
		{
		CConnMonBearerAvailabilityChange* eventBearerAvailabilityChange;
		eventBearerAvailabilityChange = ( CConnMonBearerAvailabilityChange* ) &aConnMonEvent;

		if ( eventBearerAvailabilityChange->ConnectionId() == iBearer )
			{
			if ( eventBearerAvailabilityChange->Availability() )
				{
				// Available            				
				if(CheckNetworkL())
					{
					FLOG(_L("CFMSInterruptAob::EventL()--status GPRS up"));
					LaunchFotaEngineL();
					StopServer();	
					}
				else 
					{
					FLOG(_L("CFMSInterruptAob::EventL()--status GPRS not up-- but connmon triggererd"));
					NotifyL();
					}         				
				} 
			else
				{
				//check the phone status
				// Not available            
				FLOG(_L("CFMSInterruptAob::EventL()--search for GPRS connection not up"));	    			
				NotifyL();	    	
				}
			}
		break;
		}
	case EConnMonPacketDataAvailable:
		{
		CConnMonPacketDataAvailable* eventAvailable;
		eventAvailable = ( CConnMonPacketDataAvailable* ) &aConnMonEvent;

		if ( eventAvailable->ConnectionId() == iBearer )
			{
			// Available            				
			if(CheckNetworkL())
				{
				FLOG(_L("CFMSInterruptAob::EventL()--status GPRS up"));
				LaunchFotaEngineL();
				StopServer();	
				}
			else 
				{
				FLOG(_L("CFMSInterruptAob::EventL()--status GPRS not up-- but connmon triggererd"));
				NotifyL();
				}         				
			} 
		else
			{
			//check the phone status
			// Not available            
			FLOG(_L("CFMSInterruptAob::EventL()--search for GPRS connection not up"));	    			
			NotifyL();	    	
			}
		break;
		}
	default:
	break;
	}
	}

//------------------------------------------------------------
// CFMSInterruptAob::SetWcdma
//------------------------------------------------------------
void CFMSInterruptAob::SetWcdma()
	{
	iWcdma = ETrue;
	}

//------------------------------------------------------------
// CFMSInterruptAob::CheckNetworkRegistryL
//------------------------------------------------------------
void CFMSInterruptAob::StartNetworkRegistryCheckL()
	{
	FLOG(_L("CFMSInterruptAob::CheckNetworkRegistryL- begin"));	
	iPeriodicNwCheck = CPeriodic::NewL (EPriorityNormal) ;
	FLOG(_L("CFMSInterruptAob::CheckNetworkRegistryL:-iPeriodicNwCheck created)"));
	iPeriodicNwCheck->Start(
			TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*20)
			, TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*25)
			, TCallBack(StaticCheckNwRequestL,this) ) ;
	FLOG(_L("CFMSInterruptAob::CheckNetworkRegistryL- End"));
	}

//------------------------------------------------------------
// CFMSInterruptAob::StopNetworkRegistryCheckL
//------------------------------------------------------------
void CFMSInterruptAob::NetworkRegistryCheckL()
	{
	FLOG(_L("CFMSInterruptAob::StopNetworkRegistryCheckL- begin"));	
	if(CheckNetworkL())
		{
		FLOG(_L("CFMSInterruptAob::StopNetworkRegistryCheckL-n/w found"));
		if(iPeriodicNwCheck)
			{
			delete iPeriodicNwCheck;
			iPeriodicNwCheck = NULL;
			FLOG(_L("CFMSInterruptAob::iPeriodicNwCheck-deleted"));
			}
		LaunchFotaEngineL();
		StopServer();
		}	
	FLOG(_L("CFMSInterruptAob::StopNetworkRegistryCheckL- End"));
	}

//----------------------------------------------------------------------------
// CFMSInterruptAob::StartBatteryMonitoringL
// Starts the monitoring of Battery
//----------------------------------------------------------------------------
void CFMSInterruptAob::StartBatteryMonitoringL(TFmsIpcCommands aType, TUint aLevel)
	{	
	FLOG(_L("CFMSInterruptAob::StartBatteryMonitoringL- begin"));
	
	__ASSERT_ALWAYS(aLevel > EBatteryLevelLevel0 && aLevel <=EBatteryLevelLevel7, User::Leave(KErrArgument));
	
	iUpdInterruptType = aType;
if(iUpdInterruptType == EUpdMonitorbattery)
	{
		FLOG(_L("CFMSInterruptAob::Start- EMonitorbattery"));
		TInt value =0;
		iChargeToMonitor = aLevel;
		iProperty.Attach(KPSUidHWRMPowerState,KHWRMBatteryLevel);
		TInt err=iProperty.Get(KPSUidHWRMPowerState,KHWRMBatteryLevel,value);
		   if(err != KErrNone)
		    { 
		    	FLOG(_L("CFMSInterruptAob::StartBatteryMonitoringL- error  in getiing the battery level")); 
		    	StopServer();
		    }  
		if(value >= iChargeToMonitor)
		{  
			  iProperty.Cancel();
			  iProperty.Close();	
			TRAPD(err1, LaunchFotaScheduleUpdate());
			if (err1)
			{
				FLOG(_L("CFMSInterruptAob::error in LaunchFotaScheduleUpdateL()"));
		  }
		  
		  StopServer();
	  }
	 else
	 { 	  
		if(!IsActive())
			{
			//RProperty pw;
			FLOG(_L("CFMSInterruptAob::Start- attaching for battery"));
			
			iProperty.Attach(KPSUidHWRMPowerState,KHWRMBatteryLevel);
			iStatus=KRequestPending;
			iProperty.Subscribe(iStatus);	
			SetActive();	
			}
		}	
	}
	FLOG(_L("CFMSInterruptAob::StartBatteryMonitoringL- end"));
}	 
//----------------------------------------------------------------------------
// CFMSInterruptAob::LaunchFotaScheduleUpdate
// Launches the FOTA schedule dialog
//----------------------------------------------------------------------------

void CFMSInterruptAob::LaunchFotaScheduleUpdate()
{
    TRAP_IGNORE(iServer->DeleteScheduledRemindersL());
    
	RFotaEngineSession fotaengine;
	TRAPD(error,fotaengine.OpenL());
	if(error == KErrNone)
	  {
		  	
		  	
	    FLOG(_L("CFMSInterruptAob::LaunchFotaScheduleUpdate()- going for update again"));
	    TFotaScheduledUpdate           sched(-1,-1);   //trigger the Fota remainder dialog here..
        TPckg<TFotaScheduledUpdate>    p(sched);
        TRAPD(err, fotaengine.ScheduledUpdateL(sched));
        if(err)
         {
             	  FLOG( _L( "[LaunchFotaScheduleUpdate()] FirmwareUpdate scheduling failed   " ) );              	
         }
        FLOG(_L("CFMSInterruptAob::LaunchFotaScheduleUpdate()- closing fota engine"));
        iUpdInterruptType = ENoInterrupt;
        fotaengine.Close();	
	   }
	 else
	  {
		FLOG(_L("CFMSInterruptAob::LaunchFotaScheduleUpdate()- error in opening the fota engine"));
		  	
	  }
}

//----------------------------------------------------------------------------
// CFMSInterruptAob::CheckNetworkStatusL
// Checks the network status
//----------------------------------------------------------------------------
void CFMSInterruptAob::CheckNetworkStatusL()
{
      if(CheckNetworkL()) 
					{
					FLOG(_L("CFMSInterruptAob::RunL()--charging & network found"));				
					LaunchFotaEngineL();
					StopServer();
					}
				else 
					{				
					FLOG(_L("CFMSInterruptAob::RunL()--charging but no network found & started monitoring"));
					//StartNetWorkMonitoring
					iServer->WriteToFile(EDLNetworkInterrupt, iBearer, EDriveC, 0, iWcdma);
					StartL(EDLNetworkInterrupt);	  
					}
}		


//----------------------------------------------------------------------------
// CFMSInterruptAob::HandleChargingInterruptL
// Handle the cherging interrupt
//----------------------------------------------------------------------------
void CFMSInterruptAob::HandleChargingInterruptL()
{
	    iProperty.Cancel();
			iProperty.Close();		
			//check variation
			if(iServer->NetworkTobeMonitered())
				{			
				//Check network status at this moment	
			  	CheckNetworkStatusL();
			
				}
			else
				{
				FLOG(_L("CFMSInterruptAob::RunL()--charging & network found"));				
				LaunchFotaEngineL();
				StopServer();				
				}
}		

//----------------------------------------------------------------------------
// CFMSInterruptAob::HandleAboveCriticallevelL
// Handle the meory above critical level
//----------------------------------------------------------------------------
void CFMSInterruptAob::HandleAboveCriticallevelL()
{
        if(iServer->NetworkTobeMonitered())
					{
					if(CheckNetworkL())
						{	
						FLOG(_L("CFMSInterruptAob::RunL()--memory avail & network found"));
						//Notify fota server
						LaunchFotaEngineL();
						StopServer();			   
						}
					else
						{
						FLOG(_L("CFMSInterruptAob::RunL()--memory avail but no network found"));
						//StartNetWorkMonitoring
						iServer->WriteToFile(EDLNetworkInterrupt, iBearer, iDrive, 0, iWcdma);
						StartL(EDLNetworkInterrupt);
						}
					}
				else
					{
					FLOG(_L("CFMSInterruptAob::RunL()--memory avail & no network monitor "));
					//Notify fota server
					LaunchFotaEngineL();
					StopServer();				
					}
}										
//----------------------------------------------------------------------------
// CFMSInterruptAob::LaunchFotaUpdate
// Launches the FOTA update dialog
//----------------------------------------------------------------------------

void CFMSInterruptAob::LaunchFotaUpdate()
    {
    RFotaEngineSession fotaengine;
    TRAPD(error,fotaengine.OpenL());
    if(error == KErrNone)
        {
        FLOG(_L("CFMSInterruptAob::LaunchFotaUpdate()- going for update"));
        TInt err=fotaengine.Update( -1, -1, KNullDesC8, KNullDesC8 );
        if(err)
            {
            FLOG( _L( "LaunchFotaUpdate() failed   " ) );              	
            }
        FLOG(_L("CFMSInterruptAob::LaunchFotaUpdate()- closing fota engine"));
        iUpdInterruptType = ENoInterrupt;
        fotaengine.Close();	
        }
    else
        {
        FLOG(_L("CFMSInterruptAob::LaunchFotaUpdate()- error in opening the fota engine"));
        }
    }



//----------------------------------------------------------------------------
// CFMSInterruptAob::StartCallEndMonitoringL
// starts monitoring for call end 
//----------------------------------------------------------------------------
void CFMSInterruptAob::StartCallEndMonitoringL(TFmsIpcCommands aType)
    {    
    if(!IsActive())
        {
        FLOG(_L("Starting Call End Monitoring:Begin"));
        iUpdInterruptType = aType;        
        User::LeaveIfError(iTelServer.Connect());
        _LIT(KTsyName, "phonetsy.tsy");//TSY module names wouldn't change 
        RTelServer::TPhoneInfo info;
        RPhone::TLineInfo lineInfo;        
        User::LeaveIfError(iTelServer.LoadPhoneModule(KTsyName));
        // Get the details for the first (and only) phone.
        User::LeaveIfError(iTelServer.GetPhoneInfo(0, info));
        // Open the phone.
        User::LeaveIfError(iPhone.Open(iTelServer, info.iName));
        iPhone.Initialise();
        TInt numberLines = 0;
        User::LeaveIfError(iPhone.EnumerateLines(numberLines));
        TBool foundLine = EFalse;
        for (TInt a = 0; a < numberLines; a++)
            {
            User::LeaveIfError(iPhone.GetLineInfo(a, lineInfo));
            if (lineInfo.iLineCapsFlags & RLine::KCapsVoice)
                {
                foundLine = ETrue;
                break;
                }
            }
        if (!foundLine)
            {
            User::Leave(KErrNotFound);
            }
        User::LeaveIfError(iLine.Open(iPhone, lineInfo.iName));
        iCallStatus = RCall::EStatusIdle;
        iLine.NotifyStatusChange(iStatus, iCallStatus);
        SetActive();
        }
    FLOG(_L("Starting Call End Monitoring: end"));
    }
//End of File
