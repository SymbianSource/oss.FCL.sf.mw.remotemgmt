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
* Description:
* Implementation of CFotaCustCmdAllReasons class.
*
*/

#include "fotacustcmdallreasons.h"
#include "fotastartupDebug.h"
#ifdef __SYNCML_DM_FOTA
#include <fotaengine.h>
#include "fmsclient.h"
#include "fotaserverPrivateCRKeys.h"
#include "FotaIPCTypes.h"
#include "fmsclientserver.h"
#endif
#include <schtime.h>      
#include <csch_cli.h>    
#include <centralrepository.h>
// RProperty
#include <e32base.h>
#include <e32property.h>
// Memory status PS key headers
#include <UikonInternalPSKeys.h>
// Loading application headers
#include <apgcli.h>
#include <apacmdln.h>
//Middleware includes
#include <dmeventnotifiercrkeys.h>
#include <dmeventnotifiercommon.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::NewL
// ---------------------------------------------------------------------------
//
CFotaCustCmdAllReasons* CFotaCustCmdAllReasons::NewL()
	{
  FLOG( _L( "[CFotaCustCmdAllReasons::NewL() BEGIN " ) );
	return new ( ELeave ) CFotaCustCmdAllReasons;
	}


// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::~CFotaCustCmdAllReasons
// ---------------------------------------------------------------------------
//
CFotaCustCmdAllReasons::~CFotaCustCmdAllReasons()
    {
      FLOG( _L( "[CFotaCustCmdAllReasons::~CFotaCustCmdAllReasons()  " ) );
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::Initialize
// ---------------------------------------------------------------------------
//
TInt CFotaCustCmdAllReasons::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FLOG( _L( "[CFotaCustCmdAllReasons::~Initialize()  " ) );

    
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::Execute
// ---------------------------------------------------------------------------
//
void CFotaCustCmdAllReasons::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
     FLOG( _L( "[CFotaCustCmdAllReasons::~Execute() BEGIN " ) );
      aRequest = KRequestPending;
 #ifdef __SYNCML_DM_FOTA
    TRAP_IGNORE(ExecuteL());    
 #endif
    TRequestStatus* request = &aRequest;
    User::RequestComplete( request, KErrNone );
 FLOG( _L( "[CFotaCustCmdAllReasons::~Execute() END " ) ); 
    }

// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::ExecuteL
// ---------------------------------------------------------------------------
//
void CFotaCustCmdAllReasons::ExecuteL( )
    {
    FLOG( _L( "[CFotaCustCmdAllReasons]::ExecuteL() Boot reason AllReason BEGIN " ) );

    RFotaEngineSession fotaEngine;
    //CheckCenrep if GA feature in ON then start else nothing.
    CRepository* centrep(NULL);
    TInt err = KErrNone;
    TRAP( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    TInt sendGAAfterrebootfeature = 0;
    TInt val(EFotaDefault);

    if (err == KErrNone)
        {
        err = centrep->Get(KFotaUpdateState, val);

        if (err == KErrNone)
            {
            // check if there is GA pending

            FTRACE(RDebug::Print(_L("[CFotaCustCmdAllReasons]  startup reason is %d"), val));
            switch (val)
                {
                case EFotaDownloadInterrupted:
                    {
                    FLOG( _L( "[CFotaCustCmdAllReasons] AllReason: Start FMS server " ) );
                    RFMSClient fmsClient;
                    TRAPD(err, fmsClient.OpenL());
                    if (err == KErrNone)
                        {
                        fmsClient.Close();
                        FLOG( _L( "[CFotaCustCmdAllReasons] AllReason: Closing FMS server  " ) );
                        }
                    }
                    break;
                case EFotaPendingGenAlert:
                    {
                    FLOG( _L( "[CFotaCustCmdAllReasons] AllReason: Start fota server " ) );
                    err = centrep->Get(KGenericAlertResendAfterBoot,
                            sendGAAfterrebootfeature);
                    FTRACE(RDebug::Print(_L("[CFotaCustCmdAllReasons] feature sendGenericAlert is %d"), sendGAAfterrebootfeature));

                    if (sendGAAfterrebootfeature == 1)
                        {
                        TRAPD(oError,fotaEngine.OpenL());
                        if (oError == KErrNone)
                            {
                            fotaEngine.Close();
                            FLOG( _L( "[CFotaCustCmdAllReasons] AllReason: Closing fota server  " ) );
                            }
                        }
                    }
                    break;
                case EFotaUpdateInterrupted:
                    {
                    FLOG(_L(" update interrupted >>"));
                    RFMSClient fmsclient;
                    TRAPD(err,fmsclient.OpenL());
                    if (err)
                        {
                        FLOG(_L( " opening fms failed " ) );

                        }
                    else
                        {

                        FLOG(_L("CFotaUpdate::going into FMS client side MonitorBatteryL() >>"));
                        TRAPD(err1, fmsclient.NotifyForUpdateL(EUpdMonitorbattery, 3))// equal to EBatteryLevelLevel3
                        if (err1)
                            {
                            FLOG(_L( " MonitorBatteryL failed " ));
                            }
                        fmsclient.Close();
                        FLOG(_L(" update interrupted <<"));
                        }
                    }
                    break;
                default:
                    {
                    FLOG( _L( "[CFotaCustCmdAllReasons] pendingGAToSend is not SET " ) );
                    }
                    break;
                }

            }
        }
    delete centrep;
        { // Write the code for starting the dmEventNotifier
        TRAPD(err, checkDMEventNotifierL());
        if (err)
            {
            FLOG(_L("start DmEventNotifier.exe successfull"));
            }
        else
            {
            FLOG(_L("start DmEventNotifier.exe un-successfull"));
            }
        }
    FLOG( _L( "[CFotaCustCmdAllReasons]::ExecuteL() Boot reason AllReason END " ) );
    }

// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CFotaCustCmdAllReasons::ExecuteCancel()
    {
   
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::Close
// ---------------------------------------------------------------------------
//
void CFotaCustCmdAllReasons::Close()
    {
     FLOG( _L( "[CFotaCustCmdAllReasons]::Close() " ) );
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdAllReasons::Release
// ---------------------------------------------------------------------------
//
void CFotaCustCmdAllReasons::Release()
    {
    
     FLOG( _L( "[CFotaCustCmdAllReasons]::Close() " ) );
	   delete this;
    }
    
    
TBool  CFotaCustCmdAllReasons::FindScheduleL()
{
	FLOG( _L( "[CFotaCustCmdAllReasons]::FindScheduleL()BEGIN " ) );
	TScheduleEntryInfo2                     ret;    
	RScheduler                              sc;
	TTime                                   t; 
	TTsTime                                 time;
	TSchedulerItemRef                       scitem; 
	CArrayFixFlat<TSchedulerItemRef>*     	aSchRefArray = new CArrayFixFlat <TSchedulerItemRef>(5);
	TScheduleFilter                      	aFilter(EAllSchedules);
	User::LeaveIfError( sc.Connect() );                             // xx
	CleanupClosePushL( sc );
	CleanupStack::PushL(aSchRefArray);

	User::LeaveIfError( sc.GetScheduleRefsL( *aSchRefArray,aFilter) );  // xx
	FLOG(_L("Schedule items: "));
	for ( TInt i=0; i<aSchRefArray->Count(); ++i  )
		{
		  TSchedulerItemRef it = (*aSchRefArray)[i];
		 if ( it.iName == TUid::Uid(KFotaServerUid).Name()  )
	   {
	   	  	 
	   	  	 	CleanupStack::PopAndDestroy(aSchRefArray);  
                CleanupStack::PopAndDestroy(&sc);
	   	  	 	return ETrue;
	  	
	   }
     }
                 CleanupStack::PopAndDestroy(aSchRefArray);
                 CleanupStack::PopAndDestroy(&sc);
     FLOG( _L( "[CFotaCustCmdAllReasons]::FindScheduleL()END " ) );            
                 return EFalse; 
}


void CFotaCustCmdAllReasons::checkDMEventNotifierL()
{
    FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - started"));
 
    const TUid KAppDmEventNotifierUid = TUid::Uid(KAppUidDmEventNotifier); //UID3 from .mmp file
    CRepository* cenrep (NULL);
    cenrep = CRepository::NewLC( KAppDmEventNotifierUid );
    TInt value (KErrNone);
    TBool tocontinue (EFalse);

    FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - checking for cenrep KDmEventNotifierEnabled"));
    if ( ((cenrep->Get(KDmEventNotifierEnabled,value)) == KErrNone )
            && (EHandlerRegistered == value || EHandlerNeedRegister == value )) // v alue != EHandlerNotRegistered 
    {
//        FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - cenrep KDmEventNotifierEnabled exist, value = (%d) "), value);
				FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - cenrep KDmEventNotifierEnabled exist"));
        tocontinue = ETrue;
    }
    else
    {
        //Log errors
//        FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - checking for cenrep KDmEventNotifierEnabled does not exist: error: (%d) "), err);
				FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - cenrep KDmEventNotifierEnabled does not exist or already registered"));
        //delete cenrep; cenrep = NULL;
        CleanupStack::PopAndDestroy(); // cenrep
        return;
    }
    
    TInt pMmcstatus = KErrNotFound;
    TInt ret = RProperty::Get(KPSUidUikon, KUikMMCInserted, pMmcstatus);
//    FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - checking for present Memory status: ret = (%d) , MMCStatus = (%d) "), ret, pMmcStatus);
		FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - checking for memory status"));
    if(!ret)
    {

        TInt sMMCStatus (KErrNotFound);
        TInt error = cenrep->Get(KMMCStatus, sMMCStatus);
				CleanupStack::PopAndDestroy(); // cenrep

        //FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - checking for previous Memory status, before phone off: error = (%d) , sMMCStatus = (%d) "), error, sMMCStatus);
        if(error)
        {
        		FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - Error in memory status get"));
            //CleanupStack::PopAndDestroy(); // cenrep
            return;    
        }
        if (pMmcstatus == sMMCStatus && tocontinue) // if sMMCStatus is 0 && pMMCStatus is 0. Or sMMCStatus is 1 && pMMCStatus is 1
        {
        //start the dm event notifier
                
            RApaLsSession apaLsSession;
            User :: LeaveIfError(apaLsSession.Connect());
            TApaAppInfo appInfo;
            FLOG(_L("RApaLsSession connection successful"));
            FLOG(_L("Running dmeventnotifier.exe"));
                    
            _LIT(KExampleTaskHandlerExe, "dmeventnotifier.exe");
        
            appInfo.iFullName = KExampleTaskHandlerExe;
            CApaCommandLine* cmdLine  = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            TBuf<KMaxFileName> temp;
            temp.Copy (appInfo.iFullName);
            TInt err = apaLsSession.StartApp(*cmdLine);
            
            User :: LeaveIfError(err);
            CleanupStack :: PopAndDestroy(cmdLine);
            apaLsSession.Close();
            FLOG(_L("started dmeventnotifier.exe successfully"));
        }
    }
    //CleanupStack::PopAndDestroy(); // cenrep
    FLOG(_L("CFotaCustCmdAllReasons::checkDMEventNotifier() - ended"));
}
