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
#include <bldvariant.hrh>
#include <startupdomainpskeys.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <f32file.h>
#include <sysutil.h> 
#include <centralrepository.h>
#include <ctsydomainpskeys.h>
#include "fotaserverPrivateCRKeys.h"
#include "fmsserver.h"
#include "fmsinterruptaob.h"
#include "fmsclientserver.h"
#include "fmsserversession.h"


// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KFMSServerName)); 

	// create and install the active scheduler
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	// create the server (leave it on the cleanup stack)
	CFMSServer::NewLC();

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}


// ----------------------------------------------------------------------------------------
// static method LogNwRequestL() called to submit async n/w request
// ----------------------------------------------------------------------------------------
static TInt LogNwRequestL(TAny* aPtr)
	{
	CFMSServer* ptr = (CFMSServer*) aPtr;
	FLOG(_L("static LogNwRequestL:-Begin"));
	TRAPD(err, ptr->StartMonitoringL(ENetworkInterrupt));
	FLOG(_L("LogNwRequestL started monitoring"));
	ptr->StopAsyncRequest();
	FLOG(_L("static LogNwRequestL:-end"));
	return err;
	}


// ----------------------------------------------------------------------------------------
// static method LogCallEndMonitorRequestL() called to submit async call end monitor request
// ----------------------------------------------------------------------------------------
static TInt LogCallEndMonitorRequestL(TAny* aPtr)
    {
    CFMSServer* ptr = (CFMSServer*) aPtr;
    FLOG(_L("static LogCallEndMonitorRequestL:-Begin"));
    TRAPD(err, ptr->StartUpdateInterruptMonitoringL(EMonitorPhoneCallEnd));
    FLOG(_L("LogCallEndMonitorRequestL started monitoring"));
    ptr->StopAsyncRequest();
    FLOG(_L("static LogCallEndMonitorRequestL:-end"));
	return err;
	}

// ----------------------------------------------------------------------------------------
// Server process entry-point
// ----------------------------------------------------------------------------------------
TInt E32Main()
	{
	__UHEAP_MARK;
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

// ----------------------------------------------------------------------------------------
// CFMSServer::NewLC
// ----------------------------------------------------------------------------------------
CServer2* CFMSServer::NewLC()
{
CFMSServer* self=new(ELeave) CFMSServer;
CleanupStack::PushL(self);
self->ConstructL();	
return self;
}

// ----------------------------------------------------------------------------------------
// CFMSServer::ConstructL
// ----------------------------------------------------------------------------------------
void CFMSServer::ConstructL()
	{
	FLOG(_L("CFMSServer::ConstructL- begin"));
	StartL(KFMSServerName);		
	User::LeaveIfError( iFs.Connect() );
	TInt err;
	err = iFs.CreatePrivatePath(EDriveC);
	if ( err != KErrNone && err != KErrAlreadyExists )
		{ User::Leave (err); }
	User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );
	err = iFile.Create(iFs,KFotaInterruptFileName,EFileWrite);
	if(err == KErrNone)//means file created now and opened
		{
		FLOG(_L("CFMSServer::ConstructL- file closed"));
		iFile.Close();
		}
	else if( err != KErrAlreadyExists )
		{
		FLOG(_L("CFMSServer::ConstructL- leaving with err as %d"),err);
		User::Leave(err);
		}
	FindVariation();
	iFMSInterruptAob.ConstructL();
	iFMSInterruptAob.iServer = this;  
	FLOG(_L("CFMSServer::ConstructL- end"));
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::CFMSServer() 
// ----------------------------------------------------------------------------------------
CFMSServer::CFMSServer() :CServer2(EPriorityStandard, EUnsharableSessions) /*CServer2(0)*/
	{	
	iSessionCount = 0;
	//iNetworkMon = ETrue; //Incase of cenrep key has problem
	iNetworkMon = EFalse; 
	iChargerMon = EFalse;
	iPhoneRestartMon = EFalse;
	iMemoryMon = EFalse;
	iDeleteFile = ETrue;
	iLogAsyncRequest = NULL;
	iProfileId = EFalse;
	iPkgId = EFalse;
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::MemoryToMonitorL() 
// ----------------------------------------------------------------------------------------
void CFMSServer::MemoryToMonitorL(TInt aSize, TDriveNumber aDrive)
	{
	if( iSessionCount == 0 )// no pending request
		{
		iFMSInterruptAob.MemoryToMonitorL(aSize,aDrive);
		}
	}
// ----------------------------------------------------------------------------------------
// CFMSServer::NetworkTypeL() 
// ----------------------------------------------------------------------------------------
void CFMSServer::NetworkTypeL(TInt aBearer)
	{
	if( iSessionCount == 0 )// no pending request
		{
		iFMSInterruptAob.NetworkTypeL(aBearer);
		}
	}

//------------------------------------------------------------
// Call from session class in fotastartup  case 
// CFMSServer::CheckNetworkL
//------------------------------------------------------------
void CFMSServer::CheckNetworkL(TInt& aBearerId, TInt& aDrive, TInt& aSize,
		TBool& aWcdma)
	{
	if(iNetworkMon)//n/w monitoring supports
		{
		FLOG(_L("CFMSServer::CheckNetworkL- n/w monitor supported"));
		if(iFMSInterruptAob.CheckNetworkL()) //not n/w interrupt + WLAN or GPRS
			{
			FLOG(_L("CFMSServer::CheckNetworkL- n/w mon supported & launch fota"));
			iFMSInterruptAob.LaunchFotaEngineL();		
			DropSession(); // to close the session & server
			return;
			}			
		else //if network not up, monitor for that
			{
			//from session itself we set the bearer type
			FLOG(_L("CFMSServer::CheckNetworkL- n/w not up beaerer 3G %d"),aWcdma);
			WriteToFile(ENetworkInterrupt,aBearerId,
					(TDriveNumber)aDrive, aSize, aWcdma );
			StartMonitoringL(ENetworkInterrupt);
			}
		}
	else //user interrupt or general interrupt dont check n/w call fota
		{
		FLOG(_L("CFMSServer::CheckNetworkL- n/w monitor not supported & launch fota"));		
		iFMSInterruptAob.LaunchFotaEngineL();
		FLOG(_L("CFMSServer::CheckNetworkL- stop server"));		
		DropSession();
		}
	}

//------------------------------------------------------------
// Call from session class in hard reboot only 
// CFMSServer::TriggerFotaL
//------------------------------------------------------------
void CFMSServer::TriggerFotaL()
	{
	FLOG(_L("CFMSServer::TriggerFotaL- begin"));	
	if(iNetworkMon)//n/w monitoring supports
		{
		FLOG(_L("CFMSServer::TriggerFotaL- n/w monitor supported"));
		if(iFMSInterruptAob.CheckNetworkL()) 
			{
			FLOG(_L("CFMSServer::TriggerFotaL- n/w mon supported & launch fota"));
			iFMSInterruptAob.LaunchFotaEngineL();				
			}
		else if(iFMSInterruptAob.CheckGlobalRFStateL())
			{
			FLOG(_L("CFMSServer::TriggerFotaL- n/w check started"));
			iFMSInterruptAob.StartNetworkRegistryCheckL();
			return; //dont call drop session as it closes server
			}
		else
			{
			FLOG(_L("CFMSServer::TriggerFotaL- phone is offline"));			
			}
		}
	else //user interrupt or general interrupt dont check n/w call fota
		{
		FLOG(_L("CFMSServer::TriggerFotaL- n/w monitor not supported & launch fota"));		
		iFMSInterruptAob.LaunchFotaEngineL();		
		FLOG(_L("CFMSServer::TriggerFotaL- stop server"));			
		}
	DropSession(); // to close the session & server in any case
	FLOG(_L("CFMSServer::TriggerFotaL- End"));
	}

//------------------------------------------------------------
// Called only in n/w interrupt+ WLAN case from session class
// CFMSServer::CheckWlanL
//------------------------------------------------------------
TBool CFMSServer::CheckWlanL()
	{
	return iFMSInterruptAob.CheckNetworkL();
	}

//------------------------------------------------------------
// CFMSServer::SetWcdma
//------------------------------------------------------------
void CFMSServer::SetWcdma()
	{
	iFMSInterruptAob.SetWcdma();
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::LockSession() 
// ----------------------------------------------------------------------------------------
void CFMSServer::LockSession()
	{
	iSessionCount++;
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::WriteToFile() 
// ----------------------------------------------------------------------------------------
void CFMSServer::WriteToFile(TInt aReason, TInt aBearer,TDriveNumber aDrive,
		TInt aSize, TBool aWcdmaBearer )
	{
	if(iSessionCount == 0)
		{
		FLOG(_L("CFMSServer::WriteToFile- begin"));
		TInt err=iFile.Open(iFs,KFotaInterruptFileName,EFileWrite);
		if(err == KErrNone)
			{
			FLOG(_L("CFMSServer::WriteToFile--passed"));
			TBuf8<30> data;//size 30 or 32--as args is 16 bytes
			TBuf8<30> temp;
			temp.Num(aReason);
			data.Append(temp);
			data.Append(',');
			temp.Num(aBearer);// or use iFMSinterruptAob's iBearer
			data.Append(temp);
			data.Append(',');
			temp.Num((TInt)aDrive);
			data.Append(temp);
			data.Append(',');
			temp.Num(aSize);  
			data.Append(temp);
			data.Append(',');
			temp.Num(aWcdmaBearer);
			data.Append(temp);
			iFile.Write(data);
			iFile.Close();
			}
		else
			FLOG(_L("CFMSServer::WriteToFile- Failed"));
		}
	else
		FLOG(_L("CFMSServer::WriteToFile- not done as another request is there"));
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::ReadFromFile() 
// ----------------------------------------------------------------------------------------
TBool CFMSServer::ReadFromFile(TInt& aReason, TInt& aBearer, TInt& aDrive, TInt& aSize
		, TInt& aWcdmaBearer )
	{
	TInt err = iFile.Open(iFs,KFotaInterruptFileName,EFileRead);
	FLOG(_L("CFMSServer::ReadFromFile() error as %d"),err);
	if(err == KErrNone)
		{
		TInt size = KErrNone;
		err = iFile.Size(size);	
		if(size == 0) //file empty
			{
			FLOG(_L("CFMSServer::ReadFromFile() file size is empty"));
			iFile.Close();
			return EFalse;		
			}
		TBuf8<30> data;
		iFile.Read(data);
		iFile.Close();	
		TBuf8<30> data1;    
		if(data.Length()>0)
			{
			TInt len =0 ;
			if((len=data.LocateF(',')) > 0)
				{
				TLex8 value( data.Left(len));            
				value.Val(aReason);
				FLOG(_L("CFMSServer::ReadFromFile() retrieving reason as %d"),aReason);
				data1.Insert(0,data.Right(data.Length()-(len+1)));
				data.Zero();
				//    len=data1.LocateF(',');
				if( data1.Length() > 0 && (len=data1.LocateF(',')) > 0)
					{
					value=data1.Left(len);            
					value.Val(aBearer);  
					FLOG(_L("CFMSServer::ReadFromFile() retrieving aBearer as %d"),aBearer);
					data.Insert(0,data1.Right(data1.Length()-(len+1)));            
					data1.Zero();
					//  len=data.LocateF(','); 
					if(data.Length() > 0 && (  len=data.LocateF(','))> 0)
						{
						value=data.Left(len);            
						value.Val(aDrive);
						FLOG(_L("CFMSServer::ReadFromFile() retrieving aDrive as %d"),aDrive);
						data1.Insert(0,data.Right(data.Length()-(len+1)));
						data.Zero();
						if(data1.Length() > 0 && (  len=data1.LocateF(','))> 0 )
							{
							value=data1.Left(len);
							value.Val(aSize);
							FLOG(_L("CFMSServer::ReadFromFile() retrieving aSize as %d"),aSize);
							data.Insert(0,data1.Right(data1.Length()-len-1));
							data1.Zero();
							if(data.Length() > 0 )
								{
								value=data;  
								value.Val(aWcdmaBearer);
								FLOG(_L("CFMSServer::ReadFromFile() retrieving aWcdmaBearer as %d"),aWcdmaBearer);
								}
							}
						} 
					}
				}    
			}
		}
	else
		{
		return EFalse;
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CFMSServer::DropSession()
// -----------------------------------------------------------------------------

void CFMSServer::DropSession()
	{
	// A session is being destroyed	
	iSessionCount = 0;	
	//Kill the server
	CActiveScheduler::Stop();	
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::FindVariation() 
// ----------------------------------------------------------------------------------------
void CFMSServer::FindVariation()
	{
	FLOG(_L("CFMSServer::FindVariation()"));
	CRepository* centrep = NULL;
	TInt variation = 1;
	TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
	if ( centrep )
		{
		FLOG(_L("CFMSServer::FindVariation()::Inside cenrep if"));
		TInt err = centrep->Get( KFotaMonitoryServiceEnabled, variation );                 
		if( err == KErrNone)
			{ 
			FLOG(_L("CFMSServer::FindVariation()::cenrep key found with %d"),variation);
			}
		delete centrep;
		}
	if ( err == KErrNone )
		{
		TBuf<10> var; //32-bit has max of 10 chars in Decimal
		var.Num(variation,EBinary);
		TInt size = var.Length(), maxlen = 4;
		if( size < maxlen)
			{
			TBuf<4> temp;
			temp.AppendFill('0',maxlen-size);
			temp.Append(var);
			var.Zero();
			var.Append(temp);
			}
		var.AppendFill('0',6);
		if( var[0] == '1' )//memory
			{
			FLOG(_L("CFMSServer::FindVariation():: memory monitor supported"));    		
			iMemoryMon = ETrue;
			}
		if( var[1] == '1' ) //startup
			{
			FLOG(_L("CFMSServer::FindVariation()::Phone restart monitor supported"));
			iPhoneRestartMon = ETrue;
			}
		if( var[2] == '1' )//user or charger
			{			
			FLOG(_L("CFMSServer::FindVariation()::charger monitor supported"));    		
			iChargerMon = ETrue;
			}
		if( var[3] == '1' )//newtwork
			{
			FLOG(_L("CFMSServer::FindVariation()::network monitor supported"));
			iNetworkMon = ETrue;
			}    	
		}	
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::ChargerTobeMonitered() 
// ----------------------------------------------------------------------------------------
TBool CFMSServer::ChargerTobeMonitered()
	{
	FLOG(_L("CFMSServer::ChargerTobeMonitered()::charger monitor check"));
	return iChargerMon;
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::NetworkTobeMonitered() 
// ----------------------------------------------------------------------------------------
TBool CFMSServer::NetworkTobeMonitered()
	{
	FLOG(_L("CFMSServer::NetworkTobeMonitered()::network monitor check"));
	return iNetworkMon;
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::MoniterAfterPhoneRestart() 
// ----------------------------------------------------------------------------------------
TBool CFMSServer::MoniterAfterPhoneRestart()
	{
	FLOG(_L("CFMSServer::MoniterAfterPhoneRestart()::phonerestart monitor check"));
	return iPhoneRestartMon;
	}

// ----------------------------------------------------------------------------------------
// CFMSServer::MemoryTobeMonitered() 
// ----------------------------------------------------------------------------------------
TBool CFMSServer::MemoryTobeMonitered()
	{
	FLOG(_L("CFMSServer::MemoryTobeMonitered()::memory monitor check"));
	return iMemoryMon;
	}

// -----------------------------------------------------------------------------
// CFMSServer::StartMonitoring()
// -----------------------------------------------------------------------------
void CFMSServer::StartMonitoringL(TOmaDLInterruptReason aType)
	{
	// A new session is being created	
	FLOG(_L("CFMSServer::StartMonitoringL>>"));
	if( iSessionCount == 0 ) 
		{
		++iSessionCount;		
		iFMSInterruptAob.StartL(aType);
		}
		FLOG(_L("CFMSServer::StartMonitoringL<<"));
	}

// -----------------------------------------------------------------------------
// CFMSServer::DeleteFile()
// -----------------------------------------------------------------------------
void CFMSServer::DeleteFile(TBool aValue)
	{	
	iDeleteFile = aValue;
	}
// ----------------------------------------------------------------------------------------
// CFMSServer::~CFMSServer() 
// ----------------------------------------------------------------------------------------
CFMSServer::~CFMSServer()
{
FLOG(_L("CFMSServer::~CFMSServer())"));
//iFMSInterruptAob.Cancel();
if(iDeleteFile)
	{
	TInt err = iFs.Delete(KFotaInterruptFileName);
	FLOG(_L("CFMSServer::~CFMSServer() File Deleted with error as %d"),err);
	}
if(iLogAsyncRequest)
	{
	FLOG(_L("CFMSServer::~CFMSServer():-iLogAsyncRequest cancel)"));
	iLogAsyncRequest->Cancel();
	delete iLogAsyncRequest;
	iLogAsyncRequest = NULL;
	}
iFs.Close();	
}

// ----------------------------------------------------------------------------------------
// CFMSServer::NewSessionL() 
// ----------------------------------------------------------------------------------------
CSession2* CFMSServer::NewSessionL(const TVersion&,const RMessage2&) const
{
return new (ELeave) CFMSSession();
}

// ----------------------------------------------------------------------------------------
// CFMSServer::RequestPending() 
// Any request pending
// ----------------------------------------------------------------------------------------
TBool CFMSServer::RequestPending()
	{
	if( iSessionCount > 0 )
		{
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CFMSServer::AsyncSessionRequestL() 
// Asynchronous request logging
// ----------------------------------------------------------------------------
void CFMSServer::AsyncSessionRequestL()
	{
	if(iLogAsyncRequest)
		{
		FLOG(_L("CFMSServer::AsyncSessionRequestL():-iLogAsyncRequest cancel)"));
		iLogAsyncRequest->Cancel();
		delete iLogAsyncRequest;
		iLogAsyncRequest = NULL;
		}
	iLogAsyncRequest = CPeriodic::NewL (EPriorityNormal) ;
	FLOG(_L("CFMSServer::AsyncSessionRequestL():-iLogAsyncRequest created)"));
	iLogAsyncRequest->Start(
			TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*4)
			, TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*4)
			, TCallBack(LogNwRequestL,this) ) ;
	FLOG(_L("CFMSServer::AsyncSessionRequestL():-Request logged)"));
	}

// -----------------------------------------------------------------------------
// CFMSServer::AsyncSessionRequestL() 
// Asynchronous request logging
// -----------------------------------------------------------------------------
void CFMSServer::StopAsyncRequest()
	{
	FLOG(_L("CFMSServer::StopAsyncRequest():-Begin)"));
	if(iLogAsyncRequest)
		{
		FLOG(_L("CFMSServer::StopAsyncRequest():-cancelling the request)"));
		iLogAsyncRequest->Cancel();
		delete iLogAsyncRequest;
		iLogAsyncRequest = NULL;
		}
	FLOG(_L("CFMSServer::StopAsyncRequest():-End)"));
	}

// -----------------------------------------------------------------------------
// CFMSServer::StartBatteryMonitoringL() 
// Monitors for the battery
// -----------------------------------------------------------------------------
void CFMSServer::StartBatteryMonitoringL(TOmaUpdInterruptReason aType, TUint aLevel)
	{
	// A new session is being created	
	FLOG(_L("CFMSServer::StartMonitoringL, level = %d>>"), aLevel);
	if( iSessionCount == 0 ) 
		{
		++iSessionCount;		
		iFMSInterruptAob.StartBatteryMonitoringL(aType, aLevel);
		}
		FLOG(_L("CFMSServer::StartMonitoringL<<"));
	}
// -----------------------------------------------------------------------------
// CFMSServer::StartUpdateInterruptMonitoringL() 
// Monitors for the update interrupt type
// -----------------------------------------------------------------------------
void CFMSServer::StartUpdateInterruptMonitoringL(TOmaUpdInterruptReason aType)
	{
	// A new session is being created	
	FLOG(_L("CFMSServer::StartUpdateInterruptMonitoringL>>"));
	if(EMonitorPhoneCallEnd == aType)
	    {
	    iFMSInterruptAob.StartCallEndMonitoringL(aType);
	    }
	else
	    {
	    FLOG(_L("reason unknown"));
	    }
		FLOG(_L("CFMSServer::StartUpdateInterruptMonitoringL<<"));
	}

// -----------------------------------------------------------------------------
// CFMSServer::CheckPhoneCallActiveL() 
// checks any phone call is there or not at this moment
// -----------------------------------------------------------------------------
void CFMSServer::CheckPhoneCallActiveL(TInt& aStatus)
    {
    FLOG(_L("CFMSServer::CheckPhoneCallActiveL>>"));
    TInt callstatus(KErrNotFound);
    RProperty::Get(KPSUidCtsyCallInformation, KCTsyCallState, callstatus);
    //check ctsydomainpskeys.h for different combinations, below OR condition holds good
    if(EPSCTsyCallStateUninitialized == callstatus || 
            EPSCTsyCallStateNone == callstatus ) // call not active
        {
        aStatus = EFalse;
        }
    else // call active
        {
        aStatus = ETrue;
        }
    FLOG(_L("CFMSServer::CheckPhoneCallActiveL status is %d<<"),aStatus);
    }

// -----------------------------------------------------------------------------
// CFMSServer::MonitorPhoneCallEndL() 
// Monitors for active phone call end
// -----------------------------------------------------------------------------
TBool CFMSServer::MonitorPhoneCallEndL(TInt aPkgId, TInt aProfileId)
    {
    TInt CallState = KErrNotFound;
    iPkgId=aPkgId;
    iProfileId = aProfileId;
    FLOG(_L(" iPkgId:%d,iProfileId:%d<<"),iPkgId,iProfileId);
    CheckPhoneCallActiveL(CallState);
    if(CallState)
        {
        //go for call end montioring
        LogAsyncCallMonitorL();
        return EFalse;
        }
    else //trigger fota to show install query
        {
        iFMSInterruptAob.LaunchFotaUpdate(iPkgId,iProfileId);
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CFMSServer::LogAsyncCallMonitorL() 
// Async request to monitor active phone call end
// -----------------------------------------------------------------------------
void CFMSServer::LogAsyncCallMonitorL()
    {
    if(iLogAsyncRequest)
        {
        FLOG(_L("CFMSServer::LogAsyncCallMonitorL():- cancel)"));
        iLogAsyncRequest->Cancel();
        delete iLogAsyncRequest;
        iLogAsyncRequest = NULL;
        }
    iLogAsyncRequest = CPeriodic::NewL (EPriorityNormal) ;
    FLOG(_L("CFMSServer::LogAsyncCallMonitorL(): created)"));
    iLogAsyncRequest->Start(
            TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*1)
            , TTimeIntervalMicroSeconds32(KRequestTriggerWaitTime*4)
            , TCallBack(LogCallEndMonitorRequestL,this) ) ;
    FLOG(_L("CFMSServer::LogAsyncCallMonitorL():-Request logged)"));
    }

// -----------------------------------------------------------------------------
// CFMSServer::PkgId()) 
// returns the PkgId of the FOTA update
// -----------------------------------------------------------------------------
TInt& CFMSServer::PkgId()
    {
    return iPkgId;
    }

// -----------------------------------------------------------------------------
// CFMSServer::ProfileId() 
// returns the ProfileId of the FOTA
// -----------------------------------------------------------------------------
TInt& CFMSServer::ProfileId()
    {
    return iProfileId;
    }
//End of file
