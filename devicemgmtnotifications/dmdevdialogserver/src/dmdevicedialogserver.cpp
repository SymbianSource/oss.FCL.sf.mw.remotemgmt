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
 * Description: Implementation of DM Device Dialogs
 *
 */

#include <hbtextresolversymbian.h>
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>
#include "dmdevicedialogserver.h"
#include "dmdevdialogclientserver.h"
#include "dmdevicedialogsession.h"
//#include <devicedialogconsts.h>
// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
    {
    // naming the server thread after the server helps to debug panics
    User::LeaveIfError(User::RenameThread(KDMDEVDIALOGSERVER));

    // create and install the active scheduler
    CActiveScheduler* s = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    // create the server (leave it on the cleanup stack)
    CDmDeviceDialogServer::NewLC();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);

    // Ready to run
    CActiveScheduler::Start();

    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy(2);
    }

// ----------------------------------------------------------------------------------------
// static method ShutDownL() called to submit async n/w request
// ----------------------------------------------------------------------------------------
static TInt ShutDownL(TAny* aPtr)
    {
    CDmDeviceDialogServer* ptr = (CDmDeviceDialogServer*) aPtr;
    FLOG(_L("static ShutDownL:-Begin"));
    TRAPD(err, ptr->CloseServer());
    FLOG(_L("closing the server"));
    ptr->CancelServerCloseRequest();
    FLOG(_L("closing the server:-end"));
    return err;
    }

// ----------------------------------------------------------------------------------------
// Server process entry-point
// ----------------------------------------------------------------------------------------
TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
        {
        TRAP(r,RunServerL());
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return r;
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::NewLC
// ----------------------------------------------------------------------------------------
CServer2* CDmDeviceDialogServer::NewLC()
    {
    CDmDeviceDialogServer* self = new (ELeave) CDmDeviceDialogServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::ConstructL
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogServer::ConstructL()
    {
    FLOG(_L("CDmDeviceDialogServer::ConstructL- begin"));
    StartL( KDMDEVDIALOGSERVER);    
    iSyncMLSession.OpenL();
    FLOG(_L("CDmDeviceDialogServer::ConstructL- end"));
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::CDmDeviceDialogServer() 
// ----------------------------------------------------------------------------------------
CDmDeviceDialogServer::CDmDeviceDialogServer() :
    CServer2(EPriorityStandard, EUnsharableSessions)
    {
    iSessionCount = 0;
    iServerCloseRequest = NULL;
    iProfileId = EFalse;
    iJobId = EFalse;
    iStopServer = EFalse;
    iResponse = -1;
    iPkgZeroConnectionNoteShown = EFalse;
    iDmIndicator = NULL;
    iDevDialog = NULL;
    iAnyDialogDisplaying = EFalse;
    iIndicatorLaunched = EFalse;
    iPkgZeroQueryNoteDisplaying = EFalse;
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::CloseServer()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::CloseServer()
    {
    iSessionCount = 0;
    //Kill the server
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::DataReceived()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::DataReceived(CHbSymbianVariantMap& aData)
    {
    FLOG(_L("CDmDeviceDialogServer::DataReceived>>"));
    TRAP_IGNORE(DevDialogResponseL(aData));
    FLOG(_L("CDmDeviceDialogServer::DataReceived<<"));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::DevDialogResponseL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::DevDialogResponseL(CHbSymbianVariantMap& aData)
    {
    FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL>>"));
    _LIT(KNotifierReturnType, "notifiertype");
    _LIT(KKeyPress, "keypress");
    const CHbSymbianVariant* notifiertypevariant = aData.Get(KNotifierReturnType);    
    TInt *notifiertype = notifiertypevariant->Value<TInt> ();
    const CHbSymbianVariant* returnkeyvariant = aData.Get(KKeyPress);    
    TInt *returnvalue = returnkeyvariant->Value<TInt> ();   
    iResponse = *returnvalue;
    iDevDialog->Cancel();
    iAnyDialogDisplaying = EFalse;
    if (EPkgZeroNote == *notifiertype )//Server alert note
        {
        FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL EPkgZeroNote \
                response from user is %d"),iResponse);
        if (iResponse == 0)
            {
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL OK pressed"));
            CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
            CleanupStack::PushL(varMap);            
            TInt id = KConnectNotifierId;
            HBufC* notifierid = HBufC::NewL(10);
            CleanupStack::PushL(notifierid);            
            *notifierid = KNotifierType;
            CHbSymbianVariant* notifieridvar = CHbSymbianVariant::NewL(&id,
                    CHbSymbianVariant::EInt);            
            varMap->Add(*notifierid, notifieridvar);
            TInt err = iDevDialog->Show(KDeviceDialogType, *varMap, this);
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseLconnect note \
                    launch error is %d"), err);
            iAnyDialogDisplaying = ETrue;
            iPkgZeroConnectionNoteShown = ETrue;
            GetDMJobStatusL();
            TPckgBuf<TInt> response(KDmDevDialogUid);
            iMessage.WriteL(iReplySlot,response);
            iMessage.Complete(KErrNone);
            iSessionCount--;
            CleanupStack::PopAndDestroy(2);
            //GetDMJobStatusL();
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL DM Job \
                     started"));
            }
        else
            {
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL user pressed \
                     cancel"));
            iMessage.Complete(KErrCancel);
            iSessionCount--;            
            StopServer();
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL stop \
                     server called"));
            }
        iPkgZeroQueryNoteDisplaying = EFalse;
        }
    else if (EInformativeAlertNote == *notifiertype )// Iformative alert
        {
        FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL informative alert"));
        iMessage.Complete(KErrNone);
        iSessionCount--;
        if (!iJobongoing)
            StopServer();
        else
            ShowConnectDialogAgainL();
        }
    else if (EConfirmationAlertNote == *notifiertype)// conformative alert
        {
        FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL conformative alert"));
        iMessage.Complete(iResponse);
        iSessionCount--;
        if (!iJobongoing)
            StopServer();
        else
            ShowConnectDialogAgainL();
        }
    else //connecting note
        {
        iPkgZeroConnectionNoteShown = EFalse;
        FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL connecting note"));
        if (iResponse == KErrCancel)
            {
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL user cancelled \
                     connection"));
            if (iJobongoing)
                {
                FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL job stop \
                         requested"));
                iSyncJob.StopL();
                FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL job stop\
                         done"));
                }
            DismissDialog();                               
            StopServer();
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL connecting note \
                     stopserver called"));
            }
        else // Launch indicator
            {
            FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL Launch \
                    Indicator"));            
            //Send the text id
			_LIT(KFileName, "deviceupdates_");
			_LIT(KPath, "z:/resource/qt/translations/"); 
			TBool result = HbTextResolverSymbian::Init(KFileName, KPath);
    		if(result)
    			{
    		    if(iDmIndicator == NULL)
				iDmIndicator = CHbIndicatorSymbian::NewL();
				_LIT(KIndicatorTest, 
				    "txt_deviceupdate_dblist_device_updates_val_connect_fota");       
				// returns the string "Text test"
				HBufC* text = HbTextResolverSymbian::LoadL(KIndicatorTest);
    			CleanupStack::PushL(text);
    			TPtr textptr = text->Des();
				CHbSymbianVariant* IndicatorText = CHbSymbianVariant::NewL(
									&textptr, CHbSymbianVariant::EDes);
				CleanupStack::PushL(IndicatorText);	       
				FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL Indicator activated")); 
				iDmIndicator->SetObserver(this);
				FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL indicator Observer set"));
				iDmIndicator->Activate(KDMProgressIndicatorType,IndicatorText);
				
				iIndicatorLaunched = ETrue;
				CleanupStack::PopAndDestroy(2);  
				}
            }
        }
    FLOG(_L("CDmDeviceDialogServer::DevDialogResponseL End"));    
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::DeviceDialogClosed()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::DeviceDialogClosed(TInt /*aError*/)
    {
    FLOG(_L("CDmDeviceDialogServer::DeviceDialogClosed "));
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::~CDmDeviceDialogServer() 
// ----------------------------------------------------------------------------------------
CDmDeviceDialogServer::~CDmDeviceDialogServer()
    {
    FLOG(_L("CDmDeviceDialogServer::~CDmDeviceDialogServer()"));
    if (iServerCloseRequest)
        {
        FLOG(_L("CDmDeviceDialogServer::~CDmDeviceDialogServer() \
                iServerCloseRequest cancel)"));
        iServerCloseRequest->Cancel();
        delete iServerCloseRequest;
        iServerCloseRequest = NULL;
        }
    if (iDevDialog)
        {
        FLOG(_L("CDmDeviceDialogServer::~CDmDeviceDialogServer():if loop)"));
        iDevDialog->Cancel();
        delete iDevDialog;
        }
    if (iDmIndicator)
        {
        iDmIndicator->Deactivate(KDMProgressIndicatorType);
        delete iDmIndicator;
        }
    FLOG(_L("CDmDeviceDialogServer::~CDmDeviceDialogServer()job to close"));
    if(iJobongoing)
        iSyncJob.Close();
    iSyncMLSession.Close();
    FLOG(_L("CDmDeviceDialogServer::~CDmDeviceDialogServer() end"));
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::NewSessionL() 
// ----------------------------------------------------------------------------------------
CSession2* CDmDeviceDialogServer::NewSessionL(const TVersion&,
        const RMessage2&) const
    {
    return new (ELeave) CDmDeviceDialogSession();
    }

// ----------------------------------------------------------------------------
// CDmDeviceDialogServer::PeriodicServerCloseL() 
// ----------------------------------------------------------------------------
void CDmDeviceDialogServer::PeriodicServerCloseL()
    {
    if (iServerCloseRequest)
        {
        FLOG(_L("CDmDeviceDialogServer::PeriodicServerCloseL() \
                :-iServerCloseRequest cancel "));
        iServerCloseRequest->Cancel();
        delete iServerCloseRequest;
        iServerCloseRequest = NULL;
        }
    iServerCloseRequest = CPeriodic::NewL(EPriorityNormal);    
    iServerCloseRequest->Start(TTimeIntervalMicroSeconds32(
            KRequestTriggerWaitTime * 5), TTimeIntervalMicroSeconds32(
            KRequestTriggerWaitTime * 5), TCallBack(ShutDownL, this));
    FLOG(_L("CDmDeviceDialogServer::PeriodicServerCloseL():-Request logged)"));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::LaunchPkgZeroNoteL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::LaunchPkgZeroNoteL(TInt &aProfileId,
        TInt &aJobId, const TInt &aReplySlot, TInt &aUimode,
        const RMessage2& aMessage)
    {
    FLOG(_L("CDmDeviceDialogSession::LaunchPkgZeroNoteL \
             called:"));
    iProfileId = aProfileId;
    iReplySlot = aReplySlot;
    iJobId = aJobId;
    iUimode = aUimode;
    iMessage = aMessage;
    CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL(varMap);

    HBufC* notifierid = HBufC::NewL(10);
    CleanupStack::PushL(notifierid);
    *notifierid = KNotifierType;

    HBufC* profileid = HBufC::NewL(10);
    CleanupStack::PushL(profileid);
    *profileid = KProfileId;

    HBufC* uimode = HBufC::NewL(10);
    CleanupStack::PushL(uimode);
    *uimode = KUImode;

    HBufC* serverdisplay = HBufC::NewL(20);
    CleanupStack::PushL(serverdisplay);
    *serverdisplay = KServerdisplayname;
    
    HBufC* defaultprofile = HBufC::NewL(20);
    CleanupStack::PushL(defaultprofile);
    *defaultprofile = KFotaDefaultProfile;
    
    HBufC* timeout = HBufC::NewL(20);
    CleanupStack::PushL(timeout);
    *timeout = KPkgZeroTimeout;
    
    TInt profileId( KErrNotFound );
    TBool Isdefault = EFalse;
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    if (err == KErrNone)
        {
        centrep->Get(KNSmlDMDefaultFotaProfileKey, profileId);
        delete centrep;
        centrep = NULL;
        if (aProfileId == profileId)
            Isdefault = ETrue;
        }
    TRAP( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );
    TInt Timeout(KErrNone);
    if (err == KErrNone)
        {
        centrep->Get(KDevManServerAlertTimeout, Timeout);
        //Timeout has minutes, convert to milli second
        Timeout = Timeout * 60 * 1000;
        delete centrep;
        centrep = NULL;        
        }
    
    TInt id = KDefaultNotifierId;
    CHbSymbianVariant* notifieridvar = CHbSymbianVariant::NewL(&id,
            CHbSymbianVariant::EInt);
    CleanupStack::PushL(notifieridvar);
    CHbSymbianVariant* infoprofileid = CHbSymbianVariant::NewL(&iProfileId,
            CHbSymbianVariant::EInt);
    CleanupStack::PushL(infoprofileid);
    CHbSymbianVariant* infouimode = CHbSymbianVariant::NewL(&iUimode,
            CHbSymbianVariant::EInt);
    CleanupStack::PushL(infouimode);
    RSyncMLDevManProfile DmProfile;
    DmProfile.OpenL( iSyncMLSession, iProfileId, ESmlOpenRead );
    TBuf<256> servername= DmProfile.DisplayName();                  
    DmProfile.Close();
    CHbSymbianVariant* serverdisplayname = CHbSymbianVariant::NewL(
            &servername, CHbSymbianVariant::EDes);
    CleanupStack::PushL(serverdisplayname);
    CHbSymbianVariant* defaultprofilevalue = CHbSymbianVariant::NewL(
            &Isdefault, CHbSymbianVariant::EBool);
    CleanupStack::PushL(defaultprofilevalue);
    
    CHbSymbianVariant* pkgzerotimeout = CHbSymbianVariant::NewL(
                &Timeout, CHbSymbianVariant::EInt);
        CleanupStack::PushL(pkgzerotimeout);
    
    varMap->Add(*notifierid, notifieridvar);
    varMap->Add(*profileid, infoprofileid);
    varMap->Add(*uimode, infouimode);
    varMap->Add(*serverdisplay, serverdisplayname);
    varMap->Add(*defaultprofile, defaultprofilevalue);
    varMap->Add(*timeout, pkgzerotimeout);
    CleanupStack::Pop(6);
    if (iDevDialog == NULL)
        iDevDialog = CHbDeviceDialogSymbian::NewL();
    else if (iAnyDialogDisplaying)
        {
        iDevDialog->Cancel();
        iAnyDialogDisplaying = EFalse;
        }
    else
        {
        }
    err = 
        iDevDialog->Show(KDeviceDialogType, *varMap, this);
    iPkgZeroQueryNoteDisplaying = ETrue;
    FLOG(_L("CDmDeviceDialogSession::LaunchPkgZeroNoteL \
         err in devdialog is %d"), err);
    iAnyDialogDisplaying = ETrue;
    CleanupStack::PopAndDestroy(7);
    FLOG(_L("CDmDeviceDialogSession::LaunchPkgZeroNoteL \
                 end:"));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::StopServer()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::StopServer()
    {
    FLOG(_L("CDmDeviceDialogServer::StopServer Begin"));
    if(!iStopServer && !iJobongoing)
        {
        iPkgZeroConnectionNoteShown = EFalse;
        iStopServer = ETrue;
        FLOG(_L("CDmDeviceDialogServer::StopServer Requested to shut down"));
        TRAP_IGNORE(PeriodicServerCloseL());
        }
    FLOG(_L("CDmDeviceDialogServer::StopServer Done"));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::OnSyncMLSessionEvent()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::OnSyncMLSessionEvent(TEvent aEvent,
        TInt aIdentifier, TInt aError, TInt /*aAdditionalData*/)
    {
    FLOG(_L("[OMADM]\tDmDeviceDialogServer::OnSyncMLSessionEvent(),\
                     aEvent = %d, aIdentifier = %d, aError = %d"),
            aEvent, aIdentifier, aError);
    if (iSyncJobId == aIdentifier && (aEvent == EJobStop || aEvent
            == EJobStartFailed || aEvent == EJobRejected))
        {
        FLOG(_L("[OMADM]\tDmDeviceDialogServer::OnSyncMLSessionEvent(), \
                        ajob stoping" ));
        iSyncJob.Close();
        iJobongoing = false;        
        //Stop the dialog
        DismissDialog();
        StopServer();
        }
    FLOG(_L( "[OMADM]\tDmDeviceDialogServer::OnSyncMLSessionEvent()end" ));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::DismissDialog()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::DismissDialog()
    {
    FLOG(_L("CDmDeviceDialogServer::dismissDialog "));

    if (iDevDialog && iPkgZeroConnectionNoteShown && !iIndicatorLaunched)
        {
        FLOG(_L("CDmDeviceDialogServer::dismissDialog 3"));
        iDevDialog->Cancel();
        iAnyDialogDisplaying = EFalse;
        iPkgZeroConnectionNoteShown = EFalse;
        }
    if (iIndicatorLaunched)
        {
        FLOG(
            _L("CDmDeviceDialogServer::dismissDialog deactivating indicator"));
        iDmIndicator->Deactivate(KDMProgressIndicatorType);       
        FLOG(
           _L("CDmDeviceDialogServer::dismissDialog deactivating indicator done"));
        iIndicatorLaunched = false;
        }
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::GetDMJobStatusL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::GetDMJobStatusL()
    {
    FLOG(_L("CDmDeviceDialogServer::GetDMJobStatusL "));
    iSyncJob.OpenL(iSyncMLSession, iJobId);
    FLOG(_L("CDmDeviceDialogServer::GetDMJobStatusL Job started "));
    iJobongoing = true;
    iSyncJobId = iSyncJob.Identifier();
    TInt err = KErrNone;
    // for MSyncMLEventObserver events
    TRAP( err, iSyncMLSession.RequestEventL(*this) );
    FLOG(_L("CDmDeviceDialogServer::GetDMJobStatusL error %d"), err);
    if (err != KErrNone)
        {
        iSyncJob.StopL();
        iSyncJob.Close();
        FLOG(_L("CDmDeviceDialogServer::GetDMJobStatusL leaving"));
        User::Leave(err);
        }    
    FLOG(_L("CDmDeviceDialogServer::GetDMJobStatusL end "));
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::IsConenctDialogDisplaying()
// -----------------------------------------------------------------------------
TBool CDmDeviceDialogServer::IsConenctDialogDisplaying()
    {
    return iPkgZeroConnectionNoteShown;
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::ShowConnectDialogAgainL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::ShowConnectDialogAgainL()
    {
    if (iJobongoing && !iIndicatorLaunched && !iPkgZeroConnectionNoteShown)
        {
        FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 1"));
        CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
        CleanupStack::PushL(varMap);
        TInt id = KConnectNotifierId;
        HBufC* notifierid = HBufC::NewL(10);
        CleanupStack::PushL(notifierid);
        FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 2"));
        *notifierid = KNotifierType;
        CHbSymbianVariant* notifieridvar = CHbSymbianVariant::NewL(&id,
                CHbSymbianVariant::EInt);
        FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 3"));
        varMap->Add(*notifierid, notifieridvar);
        FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 4"));
        if (iDevDialog == NULL)
            {
            iDevDialog = CHbDeviceDialogSymbian::NewL();
            FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 5"));
            }
        else if (iAnyDialogDisplaying)
            iDevDialog->Cancel();
        else
            {
            }
        iDevDialog->Show(KDeviceDialogType, *varMap, this);
        iAnyDialogDisplaying = ETrue;
        FLOG(_L("CDmDeviceDialogServer::ShowConnectDialogAgain 6"));
        iPkgZeroConnectionNoteShown = ETrue;
        CleanupStack::PopAndDestroy(2);
        }
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::ShowDisplayalertL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::ShowDisplayalertL(const TDesC& aAlertText,
        const RMessage2& aMessage)
    {
    FLOG(_L("CDmDeviceDialogServer::ShowDisplayalert "));
    iMessage = aMessage;
    CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL(varMap);
    HBufC* keyBuf = HBufC::NewL(25);
    CleanupStack::PushL(keyBuf);
    *keyBuf = KNotifierType;
    HBufC* servalertType = HBufC::NewL(25);
    CleanupStack::PushL(servalertType);
    *servalertType = KServerpushalertInfo;
    TInt id = 0;
    CHbSymbianVariant* notifierid = CHbSymbianVariant::NewL(&id,
            CHbSymbianVariant::EInt);
    varMap->Add(*keyBuf, notifierid);
    CHbSymbianVariant* serveralertmsg = CHbSymbianVariant::NewL(&aAlertText,
            CHbSymbianVariant::EDes);
    varMap->Add(*servalertType, serveralertmsg); // takes ownership
    FLOG(_L("CDmDeviceDialogServer::ShowDisplayalert before newl"));
    if (iDevDialog == NULL)
        iDevDialog = CHbDeviceDialogSymbian::NewL();
    else if (iAnyDialogDisplaying)
        {
        iDevDialog->Cancel();
        }
    else
        {
        }
    FLOG(_L("CDmDeviceDialogServer::ShowDisplayalert before show"));
    iDevDialog->Show(KDeviceDialogType, *varMap, this);
    iAnyDialogDisplaying = ETrue;
    CleanupStack::PopAndDestroy(3);
    }

// -----------------------------------------------------------------------------
// CDmDeviceDialogServer::ShowConfirmationalertL()
// -----------------------------------------------------------------------------
void CDmDeviceDialogServer::ShowConfirmationalertL(const TInt& /*aTimeOut*/,
        const TDesC& /*aHeader*/, const TDesC& aText, const RMessage2& aMessage)
    {
    iMessage = aMessage;
    CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL(varMap);
    HBufC* keyBuf = HBufC::NewL(25);
    CleanupStack::PushL(keyBuf);
    *keyBuf = KNotifierType;
    HBufC* servalertType = HBufC::NewL(25);
    CleanupStack::PushL(servalertType);
    *servalertType = KServerpushalertConfirm;
    TInt id = 0;
    CHbSymbianVariant* notifierid = CHbSymbianVariant::NewL(&id,
            CHbSymbianVariant::EInt);
    varMap->Add(*keyBuf, notifierid);
    CHbSymbianVariant* serveralertmsg = CHbSymbianVariant::NewL(&aText,
            CHbSymbianVariant::EDes);    
    varMap->Add(*servalertType, serveralertmsg); // takes ownership   
    if (iDevDialog == NULL)
        iDevDialog = CHbDeviceDialogSymbian::NewL();
    else if (iAnyDialogDisplaying)
        iDevDialog->Cancel();
    else
        {
        }
    iDevDialog->Show(KDeviceDialogType, *varMap, this);
    iAnyDialogDisplaying = ETrue;
    CleanupStack::PopAndDestroy(3);
    }


// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::NewAsyncRequest() 
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogServer::NewAsyncRequest()
    {
    iSessionCount++;
    }
// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::RequestPending() 
// Any request pending
// ----------------------------------------------------------------------------------------
TBool CDmDeviceDialogServer::RequestPending()
    {
    if( iSessionCount > 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::CancelServerCloseRequest() 
// Cancels Server shutdown
// ----------------------------------------------------------------------------------------	
void CDmDeviceDialogServer::CancelServerCloseRequest()    
	{
	FLOG(_L("CDmDeviceDialogServer::CancelServerCloseRequest():-begin)"));
	if(iServerCloseRequest)
		{
		FLOG(_L("CDmDeviceDialogServer::CancelServerCloseRequest():-iServerCloseRequest cancel)"));
		iServerCloseRequest->Cancel();
		delete iServerCloseRequest;
		iServerCloseRequest = NULL;
		}	
	FLOG(_L("CDmDeviceDialogServer::CancelServerCloseRequest():-end)"));
	}

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::IsPkgZeroNoteDisplaying() 
// ---------------------------------------------------------------------------------------- 
TBool CDmDeviceDialogServer::IsPkgZeroNoteDisplaying()    
    {
    FLOG(_L("CDmDeviceDialogServer::IsPkgZeroNoteDisplaying())"));
    return iPkgZeroQueryNoteDisplaying;    
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::CancelPendingServerAlert() 
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogServer::CancelPendingServerAlert()
    {
    FLOG(_L("CDmDeviceDialogServer::CancelPendingServerAlert())"));
    if(IsPkgZeroNoteDisplaying())
        {
        iSessionCount--;
        iDevDialog->Cancel();
        iPkgZeroQueryNoteDisplaying = EFalse;
        iMessage.Complete(KErrCancel);
        FLOG(_L("CDmDeviceDialogServer::CancelPendingServerAlert() cancelled"));    
        }
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogServer::IndicatorUserActivated() 
// ----------------------------------------------------------------------------------------
void  CDmDeviceDialogServer::IndicatorUserActivated (const TDesC& /*aType*/,
                        CHbSymbianVariantMap& /*aData*/)
    {
    //Deactivate the indicator done by plugin
    FLOG(_L("CDmDeviceDialogServer::IndicatorUserActivated()"));
    if (iDmIndicator)
        {
        //Indicator deactivated on plugin side
        FLOG(_L("CDmDeviceDialogServer::IndicatorUserActivated() Indicator \
                deactivated"));
        iIndicatorLaunched = EFalse;
        FLOG(_L("CDmDeviceDialogServer::IndicatorUserActivated() connect \
                 dialog to be activated"));
        TRAP_IGNORE(ShowConnectDialogAgainL());
        FLOG(_L("CDmDeviceDialogServer::IndicatorUserActivated() connect \
                         dialog activated"));
        }
    FLOG(_L("CDmDeviceDialogServer::IndicatorUserActivated() end"));
    }
//End of file
