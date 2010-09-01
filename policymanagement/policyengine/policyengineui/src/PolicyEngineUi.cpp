/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of PolicyEngineUI
*
*/


// INCLUDE FILES
#include "PolicyEngineUi.h"
#include "debug.h"
#include "policyengineui.hrh"
#include <policyengineuidlgs.rsg>

#include <bautils.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <eikprogi.h>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h>
#include <data_caging_path_literals.hrh>
#include "SyncService.h"

#include <implementationproxy.h>


_LIT( KCUIResourceFileName, "PolicyEngineUIDlgs.rsc" );
const TUid KUidPolicyEngineUi = { 0x10207817 };
const TUid KScreenOutputChannel = { 0x0000781F };

LOCAL_C void CreateNotifiersL( CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers );


// ---------------------------------------------------------
// CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
// ---------------------------------------------------------
//

LOCAL_C void CreateNotifiersL( CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
{
    MEikSrvNotifierBase2 *policyEngineNotifier = CPolicyEngineNotifier::NewL();
    
    CleanupStack::PushL( policyEngineNotifier );
    aNotifiers->AppendL( policyEngineNotifier );
    CleanupStack::Pop( policyEngineNotifier ); // serNotify
	RDEBUG("PolicyEngineUI notifier created!");
}



CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    { 
    CArrayPtrFlat<MEikSrvNotifierBase2>* array =
    new CArrayPtrFlat<MEikSrvNotifierBase2>( 1 );

    if (array)
        {
        TRAPD( err, CreateNotifiersL( array ) );
        if (err != KErrNone)
            {
            delete array;
            array = NULL;
            }
        }
    return array;
    }

const TImplementationProxy ImplementationTable[] = { {{0x10207817},(TProxyNewLPtr)NotifierArray} };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	RDEBUG("PolicyEngineUI notifier detected!");
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}



CPolicyEngineNotifier::CPolicyEngineNotifier()
	{
	}


CPolicyEngineNotifier::~CPolicyEngineNotifier()
	{
	//delete and close policy engine ui
	delete iPolicyEngineUi;	
	}


CPolicyEngineNotifier* CPolicyEngineNotifier::NewL()
	{
	CPolicyEngineNotifier* self = new (ELeave) CPolicyEngineNotifier();
	
	CleanupStack::PushL( self);
	self->ConstructL();	
	CleanupStack::Pop();
	
	return self;
	}

void CPolicyEngineNotifier::ConstructL()
	{
	}
		

void CPolicyEngineNotifier::Release()
	{
	delete this;	
	}


CPolicyEngineNotifier::TNotifierInfo CPolicyEngineNotifier::RegisterL()
	{
	//Registration info
    iInfo.iUid = KUidPolicyEngineUi;
    iInfo.iPriority = ENotifierPriorityHigh;
	iInfo.iChannel = KScreenOutputChannel;	
	
	return iInfo;
	}


CPolicyEngineNotifier::TNotifierInfo CPolicyEngineNotifier::Info() const
	{
	//Registration info
	return iInfo;
	}


TPtrC8 CPolicyEngineNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8().Ptr();
	}


void CPolicyEngineNotifier::Complete( TUserResponse aResponse)
	{
	RDEBUG("CPolicyEngineNotifier::Complete");

	//Write return value to message and compeltes it
	if ( aResponse == EUserAccept )
		{
		iMessage.Write( iReplySlot, KUserAcceptMark, 0);	
		}
	else
		{
		iMessage.Write( iReplySlot, KUserDenyMark, 0);	
		}
	
    iManager->CancelNotifier( iInfo.iUid );   // Notify framework we've done
	iMessage.Complete( KErrNone);
	}


void CPolicyEngineNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	RDEBUG("CPolicyEngineNotifier::StartL!");

	TInt index = aBuffer.Locate( KDelimeterChar);
	
	if ( index != KErrNotFound && !iPolicyEngineUi)
		{	
		//decode name and fingerprint from message
		TPtrC8 name = aBuffer.Left( index);
		TPtrC8 fingerPrint = aBuffer.Mid( index + 1);

		iReplySlot = aReplySlot;
		iMessage = aMessage;

		//Show policy engine ui dialogs
		iPolicyEngineUi = CPolicyEngineUi::NewL();
		iPolicyEngineUi->ActivateL( name, fingerPrint, this);
		}
	else
		{
		Complete( EUserDeny);
		}
	}


void CPolicyEngineNotifier::Cancel()
	{
	RDEBUG("CPolicyEngineNotifier::Cancel!");

	//Delete policy engine ui
	delete iPolicyEngineUi;
	iPolicyEngineUi = 0;
	}


TPtrC8 CPolicyEngineNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8().Ptr();
	}

CPolicyEngineUi::CPolicyEngineUi()
	: CActive( EPriorityStandard)
    {  
    }

void CPolicyEngineUi::ConstructL()
    {
	RDEBUG("CPolicyEngineUi::ConstructL!");

    //get pointer to CEikonEnv
	iCoeEnv = CEikonEnv::Static();    
    
    if ( !iCoeEnv )
    	{
		RDEBUG("Policy engine ui: CoeEnv not found!");
        User::Leave( KErrGeneral );        
    	}

	//Open resource file
    TFileName fileName;
    fileName.Zero();

    TFileName drivePath;
    Dll::FileName( drivePath );

    fileName.Append( TParsePtrC( drivePath ).Drive() );
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KCUIResourceFileName );   
    
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );

	//handle to resource file
    iResourceFileOffset = iCoeEnv->AddResourceFileL( fileName );

	RDEBUG("Policy engine ui resources loaded!");
 	}

CPolicyEngineUi* CPolicyEngineUi::NewL()
	{
    CPolicyEngineUi* self = new ( ELeave ) CPolicyEngineUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
	}


CPolicyEngineUi::~CPolicyEngineUi()
	{
	RDEBUG("CPolicyEngineUi::~CPolicyEngineUi()");

    // Close non-modal dialogs
    if( iResourceFileOffset )
    	{
        iCoeEnv->DeleteResourceFile( iResourceFileOffset );
    	}
	
	delete iCorporate;
	delete iRandomPart;
	}


void CPolicyEngineUi::ActivateL( const TDesC8& aCorporate, const TDesC8& aRandomPart, CPolicyEngineNotifier* aNotifier)
	{
	RDEBUG("CPolicyEngineUi::ActivateL");

	//reference to notifier (for message completion)
	iNotifier = aNotifier;
	
	//allocate new space for parameters and save them
	delete iCorporate;
	iCorporate = NULL;
	delete iRandomPart;
	iRandomPart = NULL;
	iCorporate = HBufC::NewL( aCorporate.Length());
	iRandomPart = HBufC::NewL( aRandomPart.Length());

	iCorporate->Des().Copy( aCorporate);
	iRandomPart->Des().Copy( aRandomPart);	
	
	//add active object to active scheduler and set object active
	CActiveScheduler::Add( this);
	SetActive();
	
	//complete request....
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);	
	}



void CPolicyEngineUi::RunL()
	{
	RDEBUG("CPolicyEngineUi::RunL()");
	
	//in state ERemove user has already accepted query
	if ( ERemove == iState )
		{
		//show info, remove from scheduler
		ShowDialogL( ERemove);
		Deque();
		iNotifier->Complete( EUserAccept);
		return;		
		}

	if ( EDenied == iState )
		{
		//show info, remove from scheduler
		ShowDialogL( EDenied);
		Deque();
		iNotifier->Complete( EUserDeny);
		return;		
		}

	//Control dialog
	if ( EUserAccept == ShowDialogL( (TDialog) iState))
		{
		//if user cancel dialog, show deny-dialog and return
		iState = iState + 1;
		}
	else
		{
		//user press cancel -> state = EDenied
		iState = EDenied;
		}
	
	//set active and complete message
	SetActive();
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);
	}

// ----------------------------------------------------------------------------
// CPolicyEngineUi::RunError
// ----------------------------------------------------------------------------
TInt CPolicyEngineUi::RunError ( TInt /*aError*/ )
    {
       return KErrNone;
    }	

void CPolicyEngineUi::DoCancel()
	{
	}



CPolicyEngineUi::TDlgResp CPolicyEngineUi::ShowConfirmationQueryL( 
	const TDesC& aText, const TBool& aWithCancel)
	{
	//create dialog
    CAknQueryDialog* note = 
        CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
				
	//select correct text resource 					 
	TInt resource = R_POLICYENGINEUI_CONFIRMATION_QUERY;
	
	if ( !aWithCancel )
		{
		resource = R_POLICYENGINEUI_CONFIRMATION;
		}			   
						
	//execute dialog, dialog contains self destruction			 
    TInt response = note->ExecuteLD( resource, aText );
    
	if ( response )
		{
		return EOkResp;
		}
	
	return ECancelResp;
	}


CPolicyEngineUi::TDlgResp CPolicyEngineUi::DataQueryL( const TDesC& aText, TDes& aInput )
	{

	//Create dialog with reference to input descriptor
	CAknTextQueryDialog* dlg = 
		CAknTextQueryDialog::NewL( aInput, CAknQueryDialog::ENoTone);

	//set prompt
	CleanupStack::PushL( dlg);
	dlg->SetPromptL( aText);
	CleanupStack::Pop();
	
	//execute dialog, dialog contains self destruction			 
	TInt response = dlg->ExecuteLD( R_POLICYENGINEUI_DATA_QUERY);
	
	if ( response )
		{
		return EOkResp;
		}
	
	return ECancelResp;
	}


TInt CPolicyEngineUi::ShowDialogL( const TDialog& aDialog)
	{
	TInt response = EUserDeny;

	//select correct dialog
	switch ( aDialog)
		{
		case EControl : 
			{
			response = ShowPossessionMessageQueryL();
			break;
			}
		case ERemove:
			{
			//load resource
		    HBufC* displayString = StringLoader::LoadLC( R_POLICYENGINEUI_TRUST_ESTABLISHED );
			
			TInt length1 = displayString->Length();
			HBufC16* trustString = HBufC16::NewLC ( length1  );
			TPtr bufPtr = trustString -> Des();
			bufPtr.Append (*displayString);
			// Hide background connecting note
			CSyncService *syncService =
                            CSyncService::NewL(NULL, KDevManServiceStart);
                    if (syncService)
                        {
                        syncService->EnableProgressNoteL(EFalse);
                        }

                    delete syncService;
			
			response = ShowConfirmationQueryL( *trustString, EFalse);
			CleanupStack::PopAndDestroy();
			CleanupStack::PopAndDestroy();
			
			break;
			}
		case EDenied:
			{
			//load resource
			TBuf<100> array(*iCorporate);
		  HBufC* displayString = StringLoader::LoadLC( R_POLICYENGINEUI_DENIED_NOTE, array);
			
			//show dialog and get response
			response = ShowConfirmationQueryL( *displayString, EFalse);
			//delete resource
			CleanupStack::PopAndDestroy();
			
			break;
			}		
		case EUnMatch:
			{
			//load resource
		    HBufC* displayString = StringLoader::LoadLC( R_POLICYENGINEUI_UNMATCH_NOTE);
			
			//show dialog and get response
			response = ShowConfirmationQueryL( *displayString, ETrue);
			//delete resource
			CleanupStack::PopAndDestroy();
			
			break;
			}
		case EQuestion:
			{
			//question note prompt
			TBuf<100> array(*iCorporate);
    		HBufC* displayString = StringLoader::LoadLC( R_POLICYENGINEUI_QUESTION_NOTE,array);
			TBuf<RANDOM_PART_MAX_SIZE> input;
			TBool ready = EFalse;

			//until cancel pressed or valid user gives valid certificate part
			while ( !ready)
				{
				//data query
				CPolicyEngineUi::TDlgResp resp = DataQueryL( *displayString, input);
				if ( resp == EUserAccept )
					{
					if ( input.CompareF( *iRandomPart) != 0)
						{
						//if user input doesn't match
						if ( EUserDeny == ShowDialogL( EUnMatch))
							{
							//if user cancel dialog, show deny-dialog and return
							response = EUserDeny;
							break;
							}			
						}
					else
						{
						response = EUserAccept;
						ready = ETrue;				
						}
					}
				else
					{
					//if user cancel dialog, show deny-dialog and return
					response = EUserDeny;
					break;
					}
				}	
	
			//delete resource
			CleanupStack::PopAndDestroy();
			break;
			}
		default:
		break;
		}
	
	return response;
	}


CPolicyEngineUi::TDlgResp CPolicyEngineUi::ShowPossessionMessageQueryL()
	{
	HBufC* securityInfoString = StringLoader::LoadLC( R_POLICYENGINEUI_SECURITY_INFO_NOTE );
	TBuf<60> buffer;
	buffer.Copy( *securityInfoString );
	CleanupStack::PopAndDestroy( securityInfoString );
		
	//load resource
	TBuf<100> array(*iCorporate);
  HBufC* displayString = StringLoader::LoadLC( R_POLICYENGINEUI_CONTROL_NOTE, array);
			
			
	//show dialog and get response
	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *displayString );
    dlg->PrepareLC( R_POLICY_ENGINE_POSSESSION_INFO );
    dlg->QueryHeading()->SetTextL( buffer );
    TInt response = dlg->RunLD();
    
    CleanupStack::PopAndDestroy();
    
	if ( response )
		{
		return EOkResp;
		}
	
	return ECancelResp; 
	}





