/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CSCPUserInf implementation
*
*/

#include "SCPUserInf.h"
#include <SCPTimestampPluginLang.rsg>

#include <AknGlobalNote.h>
#include <aknnotewrappers.h>
#include "SCPDebug.h"

#include <hal.h>
// For wipe
#include <starterclient.h>
#include <syslangutil.h>
#include <rfsClient.h>
#include "DMUtilClient.h"
CSCPUserInf::CSCPUserInf() :
    CActive(EPriorityStandard) // Standard priority
    {
    }

CSCPUserInf* CSCPUserInf::NewLC()
    {
    CSCPUserInf* self = new (ELeave) CSCPUserInf();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSCPUserInf* CSCPUserInf::NewL()
    {
    CSCPUserInf* self = CSCPUserInf::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

void CSCPUserInf::ConstructL()
    {
    //User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
    Dprint( (_L("CSCPUserInf::ConstructL()") ));
    CActiveScheduler::Add(this); // Add to scheduler
    }

CSCPUserInf::~CSCPUserInf()
    {
    Cancel(); // Cancel any request, if outstanding
    //iTimer.Close(); // Destroy the RTimer object
    // Delete instance variables if any
	if (idispText)
    delete idispText;
    }

void CSCPUserInf::DoCancel()
    {
    //iTimer.Cancel();
    }

void CSCPUserInf::StartL(TDesC& aString)
    {
	idispText = HBufC16::NewL( aString.Length() );
    idispText->Des().Append(aString);
    Cancel(); // Cancel any request, just to be sure
    iState = EGlobalConf;
    Dprint( (_L("CSCPUserInf::StartL()") ));
    TRequestStatus* aStatus = &iStatus;
    User::RequestComplete(aStatus, KErrNone);
    SetActive(); // Tell scheduler a request is active
    }
	
void CSCPUserInf::DoRfsL()
	{
	Cancel(); // Cancel any request, just to be sure
    iState = ERfs;
    Dprint( (_L("CSCPUserInf::DoRfsL()") ));
    TRequestStatus* aStatus = &iStatus;
    User::RequestComplete(aStatus, KErrNone);
	// This will give the control back to SecUi
    SetActive(); // Tell scheduler a request is active
    }

void CSCPUserInf::RunL()
    {
	
    Dprint( (_L("CSCPUserInf::RunL()") ));
	if (iState == EGlobalConf)
	{
	Dprint( (_L("CSCPUserInf::EGlobalConf") ));
    TRAP_IGNORE(                                            
		TRequestStatus status;
		    TPtr16 bufDes = idispText->Des();                    
		CAknGlobalNote* lInfoNote = CAknGlobalNote :: NewLC();
		lInfoNote->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY__OK);
		lInfoNote->SetAnimation(R_QGN_NOTE_WARNING_ANIM);
		lInfoNote->ShowNoteL(status, EAknGlobalInformationNote, bufDes);
		lInfoNote->SetPriority(ECoeWinPriorityAlwaysAtFront + 1);
		    User::WaitForRequest( status );
		CleanupStack::PopAndDestroy(1); // note
			);
	}
	else if (iState == ERfs)
	{
	Dprint( (_L("CSCPUserInf::ERfs") ));
    // Mark MMC card to be formatted also in bootup
	RDMUtil util;
	if ( util.Connect() == KErrNone )
	    {        
        TInt err = util.MarkMMCWipe();
		Dprint( (_L("CSCPUserInf::rfsClient.markmmc for wipe") ));
        if( err != KErrNone )
        	{
        	// even if not successfull we try to reset as much as possible -> continue
        	Dprint( ( _L( "CSCPUserInf::WipeDeviceL(): FAILED to mark MMC wipe: %d"), err ) );        	
        	}    
        util.Close();
	    }	
	
	// In case of deep level RFS, set the default language code
	// here, before RFS reboot.
	
	TInt language( 0 );
	Dprint( (_L("CSCPUserInf::Language") ));
	// If default language is not found, we reset anyway
	if ( SysLangUtil::GetDefaultLanguage( language ) == KErrNone )
	{
	HAL::Set( HALData::ELanguageIndex, language );
	Dprint( (_L("CSCPUserInf::HAL,set") ));
	}

    // Reboot with RFS reason  
    Dprint( (_L("CSCPUserInf::rfsClient.Reboot with RFS reason") ));
	RStarterSession startersession;
	if( startersession.Connect() == KErrNone )
		{
		startersession.Reset( RStarterSession::EDeepRFSReset );
		Dprint( (_L("CSCPUserInf::rfsClient.starter reset") ));
		startersession.Close();
		}
    
    TInt ret;	
    if ( ret != KErrNone )
		{
		Dprint( ( _L( "CSCPUserInf::WipeDeviceL(): Rfs FAILED: %d"), ret ) );
		}  
	Dprint( (_L("CSCPUserInf::out of ERfs") ));
	}	
    }
	
TInt CSCPUserInf::RunError(TInt aError)
    {
    return aError;
    }
