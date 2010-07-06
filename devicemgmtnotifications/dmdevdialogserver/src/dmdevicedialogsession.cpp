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
 */

#include "dmdevicedialogsession.h"
#include "dmdevdialogclientserver.h"

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::CDmDeviceDialogSession
// ----------------------------------------------------------------------------------------
CDmDeviceDialogSession::CDmDeviceDialogSession()
    {    
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::~CDmDeviceDialogSession
// ----------------------------------------------------------------------------------------
CDmDeviceDialogSession::~CDmDeviceDialogSession()
    {
    FLOG(_L("CDmDeviceDialogSession::~CDmDeviceDialogSession-- begin"));    
    if ( !Server().RequestPending() )
        {
        FLOG(_L("CDmDeviceDialogSession::~CDmDeviceDialogSession Stopserver"));
        Server().StopServer();
        }
    FLOG(_L("CDmDeviceDialogSession::~CDmDeviceDialogSession-- end"));
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::Server
// ----------------------------------------------------------------------------------------
CDmDeviceDialogServer& CDmDeviceDialogSession::Server()
    {
    return *static_cast<CDmDeviceDialogServer*> (const_cast<CServer2*> (CSession2::Server()));
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::CheckClientSecureIdL
// ----------------------------------------------------------------------------------------
TBool CDmDeviceDialogSession::CheckClientSecureId(const RMessage2& aMessage)
    {
    TUid fota = TUid::Uid(KFotaServerUid);
    TUid sosserver = TUid::Uid(KSosServerUid);
    TUid scomo = TUid::Uid(KScomoServerUid);

    // Fotaserver
    if (aMessage.SecureId() == fota.iUid)
        {
        FLOG(_L("[CDmDeviceDialogSession] CheckClientSecureId client is \
		fotaserver!"));
        return ETrue;
        }

    // Sosserver
    else if (aMessage.SecureId() == sosserver.iUid)
        {
        FLOG(_L("[CDmDeviceDialogSession] CheckClientSecureId client is \
      sosserver!"));
        return ETrue;
        }
    // Scomoserver
        else if (aMessage.SecureId() == scomo.iUid)
            {
            FLOG(_L("[CDmDeviceDialogSession] CheckClientSecureId client is \
          SCOMO server!"));
            return ETrue;
            }
    // test app
    else if (aMessage.SecureId() == 0x102073E4)
        {
        FLOG(_L("[CDmDeviceDialogSession] CheckClientSecureId client is \
        testapp!"));
        return ETrue;
        }
    else
        {
        FLOG(_L("[CDmDeviceDialogSession] CheckClientSecureId client 0x%X is\
                 unknown (msg %d), bailing out"),
                aMessage.SecureId().iId, aMessage.Function());        
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::ServiceL
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogSession::ServiceL(const RMessage2& aMessage)
    {
    RThread clt;
    aMessage.ClientL(clt);
    TFullName cltnm = clt.FullName();
    FLOG(_L( "CDmDeviceDialogSession::ServiceL      %d   serving for %S?" ),
            aMessage.Function(), &cltnm);
    if (CheckClientSecureId(aMessage))
        {
        FLOG(_L( "CDmDeviceDialogSession::ServiceL access ok"));
		Server().CancelServerCloseRequest();
		if(!Server().IsPkgZeroNoteDisplaying() || aMessage.Function() == 
		        ECancelServerAlert )
		    {
            TRAPD(err,DispatchMessageL(aMessage));
            FLOG(_L( "CDmDeviceDialogSession::ServiceL err is %d"), err);
            User::LeaveIfError(err);
		    }
		else //User not responded to earlier package zero, so no other services 
		    {
            FLOG(_L( "CDmDeviceDialogSession::ServiceL server is busy"));
            aMessage.Complete(KErrServerBusy);
		    }        
        }
    else
        {
        FLOG(_L( "CDmDeviceDialogSession::ServiceL access denied"));
        aMessage.Complete(KErrAccessDenied);
        }
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::ServiceError
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogSession::ServiceError(const RMessage2& aMessage,
        TInt aError)
    {
    CSession2::ServiceError(aMessage, aError);
    }

// ----------------------------------------------------------------------------------------
// CDmDeviceDialogSession::DispatchMessageL
// ----------------------------------------------------------------------------------------
void CDmDeviceDialogSession::DispatchMessageL(const RMessage2& aMessage)
    {
    FLOG(_L("CDmDeviceDialogSession::DispatchMessageL-begin"));
    iMessage = aMessage;
    switch (aMessage.Function())
        {
        case EServerAlert:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL \
                     Server alert case:"));
            TInt Uimode = aMessage.Int2();
            if (Uimode == CDmDeviceDialogServer::ESANSilent)
                aMessage.Complete(KErrNone);
            else
                {
                Server().NewAsyncRequest();
                TInt ProfileId = aMessage.Int0();
                TInt JobId = aMessage.Int1();
                TInt ReplySlot = 3;
                Server().LaunchPkgZeroNoteL(ProfileId, JobId, ReplySlot, Uimode,
                        iMessage);
                }
            }
            break;
        case EPkgConnectNoteIsActive:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL EPkgZeroOngoing \
                     case:"));
            TBool ret = Server().IsConenctDialogDisplaying();
            TPckgBuf<TInt> callactive(ret);
            aMessage.WriteL(0, callactive);
            aMessage.Complete(KErrNone);
            }
            break;
        case EPkgZeroConnectNoteDismiss:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL \
                    EPkgZeroConnectNoteDismiss case:"));
            if (Server().IsConenctDialogDisplaying())
                Server().DismissDialog();
            aMessage.Complete(KErrNone);
            }
            break;
        case EPkgZeroConnectNoteShowAgain:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL \
                    EPkgZeroConnectNoteShow case:"));
            if (!Server().IsConenctDialogDisplaying())
                Server().ShowConnectDialogAgainL();
            aMessage.Complete(KErrNone);
            }
            break;
        case EShowDisplayAlert:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL \
                    EDisplayAlert case:"));	
            HBufC* alerttext = HBufC::NewLC(aMessage.GetDesLengthL(0));
            TPtr bufPtr = alerttext->Des();
            aMessage.ReadL(0, bufPtr, 0);
            Server().NewAsyncRequest();
            Server().ShowDisplayalertL(bufPtr, iMessage);
            CleanupStack::PopAndDestroy();             
            }
            break;
        case EShowConfirmationalert:
            {          
            HBufC* alerttext = HBufC::NewLC(aMessage.GetDesLengthL(2));
            TPtr alerttxtptr = alerttext->Des();
            aMessage.ReadL(2, alerttxtptr);
            HBufC* alertheader = HBufC::NewLC(aMessage.GetDesLengthL(1));
            TPtr alertheaderptr = alertheader->Des();
            aMessage.ReadL(1, alertheaderptr);
            Server().NewAsyncRequest();
            Server().ShowConfirmationalertL(aMessage.Int0(), alertheaderptr,
                    alerttxtptr, iMessage);
            CleanupStack::PopAndDestroy(2);
            }
            break;
        case ECancelServerAlert:
            {          
            Server().CancelPendingServerAlert();
            aMessage.Complete(KErrNone);
            }
            break;    
        default:
            {
            FLOG(_L("CDmDeviceDialogSession::DispatchMessageL default case: \
                    and a leave happened"));            
            User::Leave(KErrArgument);
            }
        }
    FLOG(_L("CDmDeviceDialogSession::DispatchMessageL:end"));
    }
//End of file
