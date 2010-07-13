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

#include <dmdevdialogclient.h>
#include "dmdevdialogclientserver.h"
#include "dmdevdialogclientdebug.h"

// ---------------------------------------------------------------------------
// StartServerL()  
// Starts DM device Dialog server
// ---------------------------------------------------------------------------
TInt StartServerL()
	{
	FLOG(_L("StartServerLt Started"));
	TInt res = KErrNone;
	RProcess server;
	res=server.Create(KDMDEVDIALOGSERVER,KNullDesC);
	FLOG(_L("StartServerL-- create server error as %d"),res);
	if (res!=KErrNone)
		{		
		return res;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	res=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	FLOG(_L("StartServerL-- server.ExitType() returns %d"),res);
	server.Close();
	return res;
	}

// ---------------------------------------------------------------------------
// RDmDevDialog::OpenL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RDmDevDialog::OpenL()
	{	
	FLOG(_L("RDmDevDialog::OpenL()- Begin"));
	TInt res = KErrNone;
		FLOG(_L("RDmDevDialog::OpenL()- Secured client"));
		res = CreateSession( KDMDEVDIALOGSERVER,TVersion(KDmDevDialogServerVersionMajor,
		        KDmDevDialogServerVersionMinor,0),KDefaultMessageSlots );
		if ( res != KErrNone )
			{
			FLOG(_L("RDmDevDialog::OpenL()- session not created"));
			res = StartServerL();
			User::LeaveIfError( res );
			res = CreateSession( KDMDEVDIALOGSERVER,TVersion(KDmDevDialogServerVersionMajor,
	                KDmDevDialogServerVersionMinor,0),KDefaultMessageSlots );
			}							
	FLOG(_L("RDmDevDialog::OpenL()- End %d"),res);	
	return res;
	}

// ---------------------------------------------------------------------------
// RDmDevDialog::Close()
// ---------------------------------------------------------------------------
EXPORT_C void RDmDevDialog::Close()
	{
    FLOG(_L(" RDmDevDialog::Close"));
	RSessionBase::Close();
	}
	
// ----------------------------------------------------------------------------
// RDmDevDialog::LaunchPkgZeroL()
// ----------------------------------------------------------------------------	
EXPORT_C void RDmDevDialog::LaunchPkgZero(const TInt& aProfileId, 
        const TInt& ajobId, const TInt& aUimode, 
        TDes8& aResponse, TRequestStatus& aStatus )
	{		
    FLOG(_L(" RDmDevDialog::LaunchPkgZero--Begin"));
    SendReceive( EServerAlert, TIpcArgs(aProfileId,ajobId,aUimode,&aResponse),
            aStatus);
    FLOG(_L(" RDmDevDialog::LaunchPkgZero end "));
	}

// ---------------------------------------------------------------------------
// RDmDevDialog::IsPkgZeroConnectNoteShown() 
// ---------------------------------------------------------------------------  
EXPORT_C TInt RDmDevDialog::IsPkgZeroConnectNoteShown(TInt& aConectNoteShown)
    {
    FLOG(_L(" RDmDevDialog::IsPkgZeroConnectNoteShown--Begin"));
    TPckgBuf<TInt> result(aConectNoteShown);
    TInt err = SendReceive( EPkgConnectNoteIsActive,TIpcArgs(&result));    
    aConectNoteShown = result();
    FLOG(_L(" RDmDevDialog::IsPkgZeroConnectNoteShown--End as %d,%d"),err,
            aConectNoteShown);
    return err;    
    }

// ---------------------------------------------------------------------------
// RDmDevDialog::DismissConnectDialog() 
// --------------------------------------------------------------------------- 
EXPORT_C TInt RDmDevDialog::DismissConnectDialog()
    {
    FLOG(_L(" RDmDevDialog::DismissConnectDialog--Begin"));    
    TInt err = SendReceive( EPkgZeroConnectNoteDismiss,TIpcArgs());        
    FLOG(_L(" RDmDevDialog::DismissConnectDialog--End as %d,%d"),err);    
		return err;
    }

// ---------------------------------------------------------------------------
// RDmDevDialog::ShowConnectDialog() 
// --------------------------------------------------------------------------- 
EXPORT_C TInt RDmDevDialog::ShowConnectDialog()
    {
    FLOG(_L(" RDmDevDialog::ShowConnectDialog--Begin"));    
    TInt err = SendReceive( EPkgZeroConnectNoteShowAgain,TIpcArgs());        
    FLOG(_L(" RDmDevDialog::ShowConnectDialog--error is %d"),err);
    return err;    
    }

// ---------------------------------------------------------------------------
// RDmDevDialog::ShowDisplayAlert() 
// --------------------------------------------------------------------------- 
EXPORT_C void RDmDevDialog::ShowDisplayAlert(const TDesC& aNoteDisplayText,
            TRequestStatus& aStatus)
    {
    FLOG(_L(" RDmDevDialog::ShowDisplayAlert--Begin"));      
    SendReceive( EShowDisplayAlert,TIpcArgs(&aNoteDisplayText),aStatus);        
    FLOG(_L(" RDmDevDialog::ShowDisplayAlert end" ));
    }


// ---------------------------------------------------------------------------
// RDmDevDialog::ShowConfirmationAlert() 
// --------------------------------------------------------------------------- 
EXPORT_C void RDmDevDialog::ShowConfirmationAlert(const TInt& aTimeout,
        const TDesC& aNoteHeader, const TDesC& aNoteDisplayText, 
        TRequestStatus& aStatus)
    {
    FLOG(_L(" RDmDevDialog::ShowConfirmationAlert--Begin"));
    SendReceive( EShowConfirmationalert,TIpcArgs(aTimeout,&aNoteHeader,
            &aNoteDisplayText),aStatus);
    FLOG(_L(" RDmDevDialog::ShowConfirmationAlert--End "));    
    }

// ---------------------------------------------------------------------------
// RDmDevDialog::CancelPkgZeroNote() 
// --------------------------------------------------------------------------- 
EXPORT_C TInt RDmDevDialog::CancelPkgZeroNote()
    {
    FLOG(_L(" RDmDevDialog::CancelPkgZeroNote--Begin"));
    TInt err = SendReceive(ECancelServerAlert, TIpcArgs());    
    FLOG(_L(" RDmDevDialog::CancelPkgZeroNote--End "));    
    return err;
    }

//End of file
