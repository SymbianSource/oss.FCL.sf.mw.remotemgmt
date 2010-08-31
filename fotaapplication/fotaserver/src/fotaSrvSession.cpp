/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   fotaengines server side session
 *
 */

// INCLUDE FILES
#include "FotasrvSession.h"
#include "FotaSrvDebug.h"
#include <eikenv.h>
#include <eikappui.h>
#include <s32mem.h>
#include <e32base.h>
#include <centralrepository.h>
#include "FotaServer.h"

// ============================= MEMBER FUNCTIONS ============================

class CFotaServer;

// ---------------------------------------------------------------------------
// CFotaSrvSession::CheckClientSecureIdL 
// Returns True if caller is syncml framework. False for fotasrv.
// Leaves if client is unknown
// ---------------------------------------------------------------------------
//
TFotaClient CFotaSrvSession::CheckClientSecureIdL(const RMessage2& aMessage)
    {
    TFotaClient client(EUnknown);
    TUid dmhostserver1 = TUid::Uid(KDMHostServer1Uid);
    TUid fota = TUid::Uid(KFotaServerUid);
    TUid omadmappui = TUid::Uid(KOmaDMAppUid);
    TUid starter = TUid::Uid(KStarterUid);
    TUid fscheduler = TUid::Uid(KFotaScheduler);
    TUid fms = TUid::Uid(KFMSServerUid);
    TUid softwarechecker = TUid::Uid(0x2000BB96);
    TUid softwarecheckerbackground = TUid::Uid(0x2000BB97);
    TUid testapp = TUid::Uid(0x102073E4);
    TUid testapp2 = TUid::Uid(0x10009CF4);

    TUid iadclient = TUid::Uid(0x2000F85A);

    // Syncml
    if (aMessage.SecureId() == dmhostserver1.iUid)
        {
        FLOG(
                _L("[CFotaSrvSession] CheckClientSecureIdL client is DmHostserver!"));
        client = EDMHostServer;
        }
    else if (aMessage.SecureId() == omadmappui.iUid)
        {
        FLOG(
                _L("[CFotaSrvSession] CheckClientSecureIdL client is omadmappui!"));
        client = EOMADMAppUi;
        }
    else if (aMessage.SecureId() == starter.iUid)
        {
        FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is starter!"));
        client = EStarter;
        }
    else if (aMessage.SecureId() == softwarechecker.iUid
            || aMessage.SecureId() == iadclient.iUid)
        {
        FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is IAD!"));
        client = ESoftwareChecker;
        }
    else if (aMessage.SecureId() == softwarecheckerbackground.iUid)
        {
        FLOG(
                _L("[CFotaSrvSession] CheckClientSecureIdL client is IAD - background!"));
        client = ESoftwareCheckerBackground;
        }
    else if (aMessage.SecureId() == fms.iUid)
        {
        FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is FMS!"));
        client = EFMSServer;
        }
    else if (aMessage.SecureId() == fscheduler.iUid)
        {
        FLOG(
                _L("[CFotaSrvSession] CheckClientSecureIdL client is fota scheduler!"));
        client = EFotaScheduler;
        }
    else if (aMessage.SecureId() == 0x0323231 || aMessage.SecureId()
            == testapp.iUid || aMessage.SecureId() == testapp2.iUid)
        {
        client = EFotaTestApp;
        }

    return client;
    }

// ---------------------------------------------------------------------------
// CFotaSrvSession::CFotaSrvSession
// ---------------------------------------------------------------------------
//
CFotaSrvSession::CFotaSrvSession()
    {
    FLOG(_L( "CFotaSrvSession::CFotaSrvSession( )" ));
    iError = KErrNone;
    //iDoc = CEikonEnv::Static()->EikAppUi()->Document();
    }

// ---------------------------------------------------------------------------
// CFotaSrvSession::~CFotaSrvSession
// ---------------------------------------------------------------------------
//
CFotaSrvSession::~CFotaSrvSession()
    {
    FotaServer()->DropSession();
    }

// ---------------------------------------------------------------------------
// CFotaSrvSession::ServiceL
// Handle client request
// ---------------------------------------------------------------------------
//
void CFotaSrvSession::ServiceL(const RMessage2& aMessage)
    {
    TInt err(KErrNone);
    TInt pkgid = 0;
    TPackageState state;
    RThread clt;
    aMessage.ClientL(clt);
    TFullName cltnm = clt.FullName();
    FLOG(_L( "CFotaSrvSession::ServiceL      %d   serving for %S?" ),
            aMessage.Function(), &cltnm);

    TFotaClient client = CheckClientSecureIdL(aMessage);

    if (client == EUnknown)
        {
        FLOG(_L("Permission denied to use fota services!"));
        User::Leave(KErrPermissionDenied);
        }

    TInt cmd = aMessage.Function();
    if ((cmd != EGetState) && (cmd != EGetResult) && (cmd != EGetCurrFwDetails) && (cmd != EGetUpdateTimestamp))
        {
    
        TInt fotaValue(1);
        CRepository* centrep(NULL);
        TUid uidValue =
            {
            0x101F9A08
            }; // KCRFotaAdapterEnabled

        centrep = CRepository::NewL(uidValue);
        if (centrep)
            {
            FLOG(_L("centralrepository found "));
            centrep->Get(1, fotaValue); // KCRFotaAdapterEnabled     
            delete centrep;
            }

        if (!fotaValue)
            {
            FLOG(_L("Fota is disabled or not supported!"));
            User::Leave(KErrNotSupported);
            }
            
        }

    switch (aMessage.Function())
        {

        case EFotaDownload:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL DOWNLOAD"));
            TDownloadIPCParams ipc;
            TPckg<TDownloadIPCParams> pkg(ipc);
            aMessage.Read(0, pkg);
            TInt deslen = aMessage.GetDesLengthL(1);
            HBufC8* urlbuf = HBufC8::NewLC(deslen);
            TPtr8 urlptr = urlbuf->Des();
            aMessage.Read(1, urlptr);
            TInt silent = aMessage.Int2();
            FotaServer()->DownloadL(ipc, urlptr, client, silent, EFalse);
            CleanupStack::PopAndDestroy(urlbuf); // urlbuf
            aMessage.Complete(KErrNone);

            break;
            }
        case EFotaUpdate:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL UPDATE" ));
            TDownloadIPCParams ipc;
            TPckg<TDownloadIPCParams> pkg(ipc);
            aMessage.Read(0, pkg);

            // If update started from omadmappui, no alert should be sent if 
            // update is cancelled
            if (client == EOMADMAppUi)
                {
                ipc.iSendAlert = EFalse;
                }
            FotaServer()->TryUpdateL(client);
            aMessage.Complete(KErrNone);
            break;
            }
        case EFotaDownloadAndUpdate:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL DOWNLOADANDUPDATE" ));
            TDownloadIPCParams ipc;
            TPckg<TDownloadIPCParams> pkg(ipc);
            aMessage.Read(0, pkg);
            TInt deslen = aMessage.GetDesLengthL(1);
            HBufC8* urlbuf = HBufC8::NewLC(deslen);
            TPtr8 urlptr = urlbuf->Des();
            aMessage.Read(1, urlptr);
            TFotaClient requester = CheckClientSecureIdL(aMessage);

            TInt silent = aMessage.Int2();
            FotaServer()->DownloadL(ipc, urlptr, requester, silent, ETrue);
            CleanupStack::PopAndDestroy(urlbuf);
            aMessage.Complete(KErrNone);
            }
            break;

        case EFotaTryResumeDownload:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL TRYRESUMEDOWNLOAD" ));
            if (client == EOMADMAppUi || client == EFMSServer || client
                    == EFotaTestApp)
                {
                TInt silent = aMessage.Int0();
                
                FotaServer()->TryResumeDownloadL(client, silent); // silent
                aMessage.Complete(KErrNone);
                }
            else
                {
                aMessage.Complete(KErrAccessDenied);
                }

            }
            break;

        case EDeletePackage:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL DELETEPACKAGE"));
            pkgid = aMessage.Int0();
            FotaServer()->DeletePackageL(pkgid);
            aMessage.Complete(KErrNone);
            }
            break;
        case EGetState:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL GETSTATE" ));
            pkgid = aMessage.Int0();

            state = FotaServer()->GetStateL(pkgid);

            FLOG(_L( "CFotaSrvSession::ServiceL GETSTATE << %d" ),  state.iState);
            TPckg<RFotaEngineSession::TState> pkg2(state.iState);
            aMessage.Write(1, pkg2);
            aMessage.Complete(KErrNone);
            }
            break;
        case EGetResult:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL GETRESULT >>" ));
            pkgid = aMessage.Int0();
            state = FotaServer()->GetStateL(pkgid);
            TPckg<TInt> pkg2(state.iResult);
            FLOG(_L( "CFotaSrvSession::ServiceL GETRESULT << %d" ),
                    state.iResult);
            aMessage.Write(1, pkg2);
            aMessage.Complete(err);
            break;
            }
        case EGetUpdatePackageIds:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL EGETUPDATEPACKAGEIDS" ));
            TPkgIdList pkgids;
            FotaServer()->GetUpdatePackageIdsL(pkgids);
            TPckg<TPkgIdList> pkgids_pkg(pkgids);
            aMessage.Write(0, pkgids_pkg);
            aMessage.Complete(KErrNone);
            }
            break;

        case EGetUpdateTimestamp:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL EGETUPDATETIMESTAMP" ));
            TBuf16<15> timestamp;
            FotaServer()->GetUpdateTimeStampL(timestamp);
            aMessage.Write(0, timestamp);
            aMessage.Complete(KErrNone);
            }
            break;

        case EGenericAlertSentForPackage:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL EGENERICALERTSENT FOR PKGID" ));
            TInt pkgid = aMessage.Int0();
            FotaServer()->ResetFotaStateL(pkgid);
            aMessage.Complete(err);
            }
            break;

        case EScheduledUpdate:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL ESCHEDULEDUPDATE" ));
            TFotaScheduledUpdate sched(-1, -1);
            TPckg<TFotaScheduledUpdate> p(sched);
            aMessage.Read(0, p);

            FLOG(_L(" pkgid: %d   scheduleid:%d"), sched.iPkgId,
                    sched.iScheduleId);
            FotaServer()->ScheduledUpdateL(sched, client);
            aMessage.Complete(KErrNone);
            
            }
            break;

        case EPauseDownload:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL EPAUSEDOWNLOAD" ));
            FotaServer()->PauseDownloadL();
            aMessage.Complete(KErrNone);
            }
            break;
        case EGetCurrFwDetails:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL EGETCURRFWDETAILS" ));

            TBuf8<KFotaMaxPkgNameLength> name;
            TBuf8<KFotaMaxPkgVersionLength> version;
            TInt size(0);

            FotaServer()->GetCurrentFwDetailsL(name, version, size);

            /*
            aMessage.Write(1, name);
            aMessage.Write(2, version);
            TPckg<TInt> psize(size);
            aMessage.Write(3, psize);

            aMessage.Complete(KErrNone);*/
            
            aMessage.Write(0, name);
            aMessage.Write(1, version);
            TPckg<TInt> psize(size);
            aMessage.Write(2, psize);

            aMessage.Complete(KErrNone);


            }
            break;
        default:
            {
            FLOG(_L( "CFotaSrvSession::ServiceL In default case" ));
            }
            break;
        }
    }

// ----------------------------------------------------------------------------------------
// CFotaSrvSession::ServiceError
// ----------------------------------------------------------------------------------------
void CFotaSrvSession::ServiceError(const RMessage2& aMessage, TInt aError)
    {
    FLOG(_L("CFotaSrvSession::ServiceError, err = %d >>"), aError);
    CSession2::ServiceError(aMessage, aError);
    FLOG(_L("CFotaSrvSession::ServiceError <<"));
    }

// ---------------------------------------------------------------------------
// CFotaSrvSession::FotaServer
// Helper function
// ---------------------------------------------------------------------------
//

CFotaServer* CFotaSrvSession::FotaServer() const
    {
    return (CFotaServer*) Server();
    }

void DispatchMessageL(const RMessage2& aMessage)
    {
    return;
    }

