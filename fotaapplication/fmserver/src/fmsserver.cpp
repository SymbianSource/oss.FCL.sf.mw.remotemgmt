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
#include <schinfo.h>
#include <csch_cli.h>
#include "FotaIPCTypes.h"
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
	TRAPD(err, ptr->StartMonitoringL(EDLNetworkInterrupt));
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
    TRAPD(err, ptr->StartUpdateInterruptMonitoringL(EUpdMonitorPhoneCallEnd));
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
			WriteToFile(EDLNetworkInterrupt,aBearerId,
					(TDriveNumber)aDrive, aSize, aWcdma );
			StartMonitoringL(EDLNetworkInterrupt);
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
#if defined (__WINS__)
		if(ETrue)
#else
		if(iFMSInterruptAob.CheckNetworkL())
#endif
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
void CFMSServer::StartMonitoringL(TFmsIpcCommands aType)
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
void CFMSServer::StartBatteryMonitoringL(TFmsIpcCommands aType, TUint aLevel)
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
void CFMSServer::StartUpdateInterruptMonitoringL(TFmsIpcCommands aType)
	{
	// A new session is being created	
	FLOG(_L("CFMSServer::StartUpdateInterruptMonitoringL>>"));
	if(EUpdMonitorPhoneCallEnd == aType)
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
TBool CFMSServer::MonitorPhoneCallEndL()
    {
    TInt CallState = KErrNotFound;

    CheckPhoneCallActiveL(CallState);
    if(CallState)
        {
        //go for call end montioring
        LogAsyncCallMonitorL();
        return EFalse;
        }
    else //trigger fota to show install query
        {
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


void CFMSServer::CreateScheduledReminderL()
    {
    FLOG(_L("CFMSServer::CreateScheduledReminderL ()"));
    _LIT(KFotaScheduleExe, "Z:\\sys\\bin\\fotaschedulehandler.exe");

    RScheduler scheduler;
    TTsTime startTime;
    TTime time;
    time.HomeTime();
    time = time + (TTimeIntervalHours(1));
    startTime.SetLocalTime(time);

    User::LeaveIfError(scheduler.Connect());
    CleanupClosePushL(scheduler);
    //Creating a persistent daily schedule
    
    TSchedulerItemRef persistentScheduleItem;
    CArrayFixFlat<TScheduleEntryInfo2>* entries = new CArrayFixFlat<TScheduleEntryInfo2> (1);
    CleanupStack::PushL(entries);
    persistentScheduleItem.iName = TUid::Uid(KFMSServerUid).Name();

    //TScheduleEntryInfo2 scentry1(startTime, EDaily, 1, 1);
    TScheduleEntryInfo2 scentry1;
    scentry1.SetStartTime(startTime);
    scentry1.SetInterval(1);
    scentry1.SetIntervalType(TIntervalType(EHourly));
    scentry1.SetValidityPeriod((TTimeIntervalMinutes) 1440); //1440 min = 24 hrs or 1 day
    
    entries->AppendL(scentry1);

    scheduler.Register(TFileName( KFotaScheduleExe ), 0 );
    TInt ret = scheduler.CreatePersistentSchedule(persistentScheduleItem, *entries);

    FLOG(_L("created schedule %d  %d:%d"), persistentScheduleItem.iHandle,
            time.DateTime().Hour(), time.DateTime().Minute());

    if (ret == KErrNone)
        {
        TTaskInfo taskInfo;
        taskInfo.iName = TUid::Uid(KFMSServerUid).Name();
        taskInfo.iRepeat = 1; //Repeat once
        taskInfo.iPriority = 1;

        TFotaScheduledUpdate fotareminder(-1, -1);
        TPckg<TFotaScheduledUpdate> fotareminderpkg(fotareminder);

        HBufC* data = HBufC::NewLC(fotareminderpkg.Length());
        data->Des().Copy(fotareminderpkg);

        TInt err = scheduler.ScheduleTask(taskInfo, *data,  persistentScheduleItem.iHandle);

        FLOG(_L("Schedule creation error %d"), err);

        CleanupStack::PopAndDestroy(data);
        }
    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PopAndDestroy(&scheduler); // xx
    }


void CFMSServer::DeleteScheduledRemindersL()
    {
    FLOG(_L("CFMSServer::DeleteScheduledRemindersL >>"));
    
    TScheduleEntryInfo2                     ret;
    TInt                                    err;    
    RScheduler                              sc;
    TTime                                   t; 
    TTsTime                                 time;
    TSchedulerItemRef                       scitem; 
    CArrayFixFlat<TSchedulerItemRef>*       aSchRefArray = new CArrayFixFlat <TSchedulerItemRef>(5);
    TScheduleFilter                         aFilter(EAllSchedules);
    User::LeaveIfError( sc.Connect() ); 
    CleanupClosePushL( sc );
    CleanupStack::PushL(aSchRefArray);

    User::LeaveIfError( sc.GetScheduleRefsL( *aSchRefArray,aFilter) );
    FLOG(_L("Schedule items: "));
    for ( TInt i=0; i<aSchRefArray->Count(); ++i  )
    {
    TSchedulerItemRef it = (*aSchRefArray)[i];
    if ( it.iName == TUid::Uid(KFMSServerUid).Name()  )
        {
        TScheduleState2 sc_state;
        CArrayFixFlat<TScheduleEntryInfo2>*  sc_entries = new CArrayFixFlat <TScheduleEntryInfo2>(5);
        CArrayFixFlat<TTaskInfo>*            sc_tasks  = new CArrayFixFlat <TTaskInfo>(5);
        TTsTime                              sc_duetime;
        CleanupStack::PushL( sc_entries );
        CleanupStack::PushL( sc_tasks );
        FLOG (_L("%d. schedule handle: %d name:'%S'"),i,it.iHandle, &(it.iName) );

        err = sc.GetScheduleL ( it.iHandle , sc_state, *sc_entries,*sc_tasks,sc_duetime ); // xx

        TDateTime  dtm = sc_duetime.GetLocalTime().DateTime();
        FLOG(_L("   schedule duetime:%d:%d"), dtm.Hour(), dtm.Minute());

        if ( err ) FLOG(_L("     schedule  sc get err %d"),err);
        else 
            {
            for ( TInt k=0; k<sc_entries->Count();++k)
                {
                TScheduleEntryInfo2 sc_entry = (*sc_entries)[k];
                ret = sc_entry;
                TTime sctime = sc_entry.StartTime().GetLocalTime();
                FLOG(_L("         schedule entry %d int-type:%d int:%d start: %d:%d"),k,sc_entry.IntervalType(),sc_entry.Interval(),sctime.DateTime().Hour(),sctime.DateTime().Minute());
                }

            for ( TInt j=0; j<sc_tasks->Count();++j)
                {
                TTaskInfo sc_task = (*sc_tasks)[j];
                FLOG(_L("         schedule task  %d  '%S'"),sc_task.iTaskId,&(sc_task.iName) );
                if ( sc_task.iName==TUid::Uid(KFMSServerUid).Name() )
                    {
                    FLOG(_L("          schedule DeleteTask %d"),sc_task.iTaskId);
                    User::LeaveIfError( sc.DeleteTask(sc_task.iTaskId) );
                    }
                }
            }

        FLOG(_L("     DeleteSchedule %d"),it.iHandle);
        err = sc.DeleteSchedule(it.iHandle );
		FLOG(_L("Delete status of tasks = %d"), err);

        CleanupStack::PopAndDestroy( sc_tasks );
        CleanupStack::PopAndDestroy( sc_entries );
        }
    }
    CleanupStack::PopAndDestroy( aSchRefArray );
    CleanupStack::PopAndDestroy(&sc);
    
    FLOG(_L("CFMSServer::DeleteScheduledRemindersL <<"));
    }
//End of file
