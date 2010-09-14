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
* Description:  Methods for the SyncML Appserver starter notifier
*
*/



// INCLUDE FILES
#include <eikenv.h>             // Eikon environment
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncService.h>
#include <aknnotewrappers.h>
#include <s32mem.h> 
#include <in_sock.h>
#include <StringLoader.h>    // Localisation stringloader
#include <aknlists.h>
#include <avkon.mbg>
#include <aknconsts.h>
#include <utf.h>
#include <syncmlnotifier.rsg>   // Own resources
#include "SyncMLDlgNotifier.h"  // Class definition
#include "SyncMLTimedMessageQuery.h"
#include "SyncMLAppLaunchNotifier.h"
#include "SyncMLNotifDebug.h"
#include "SyncMLTimedInputTextQuery.h"
#include "SyncMLTimedDateQuery.h"
#include "SyncMLTimedNumberQueryDialog.h"
#include "SyncMLAknPopUplist.h"

// CONSTANTS
const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLDlgNotifier* CSyncMLDlgNotifier::NewL(
    CSyncMLAppLaunchNotifier* aAppLaunchNotif )
    {
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::NewL()") );
    CSyncMLDlgNotifier* self = new (ELeave) CSyncMLDlgNotifier( aAppLaunchNotif );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::NewL() completed") );
    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSyncMLDlgNotifier::~CSyncMLDlgNotifier()
    {
    Cancel();   // Free own resources
    if(iListItems)
        {
        delete iListItems;
        iListItems = NULL;
        }
    if(iChunk.Handle())
        {
        iChunk.Close();
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::EnableSyncProgressNoteL
// -----------------------------------------------------------------------------
//
void CSyncMLDlgNotifier::EnableSyncProgressNoteL( TBool aEnable )
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLDlgNotifier::EnableSyncProgressNoteL(), aEnable = %d" ),
        aEnable ) );
        
    TSmlJobId currentJobId( KErrNotFound );
    TSmlUsageType usageType;
    TUint serviceId = 0;
    RSyncMLSession syncSession;
    CleanupClosePushL( syncSession );
    
    syncSession.OpenL();
    syncSession.CurrentJobL( currentJobId, usageType );
    syncSession.Close();
    CleanupStack::Pop( &syncSession );

    if ( currentJobId != KErrNotFound )
        {
        switch( usageType )
            {
            case ESmlDataSync:
                serviceId = KDataSyncServiceStart;
                break;
            case ESmlDevMan:
                serviceId = KDevManServiceStart;
                break;
            default:
                User::Leave( KErrArgument );
                break;
            }

        // There should be a valid pointer to the application launcher
        // notifier, but we can survive without it. Therefore we'll just
        // check for it to prevent a crash.
        if ( iAppLaunchNotif )
            {
            CSyncService* syncService = iAppLaunchNotif->SyncServiceL( serviceId );
            if ( syncService )
                {
                syncService->EnableProgressNoteL( aEnable );
                }
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::EnableSyncProgressNoteL() completed") );
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::RegisterL
// -----------------------------------------------------------------------------
//
CSyncMLDlgNotifier::TNotifierInfo CSyncMLDlgNotifier::RegisterL()
    {
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RegisterL()") );
    iInfo.iUid = KSyncMLDlgNotifierUid;
    iInfo.iChannel = KSmlDlgChannel;
    iInfo.iPriority = ENotifierPriorityHigh;
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RegisterL() completed") );
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::GetParamsL
// Initialize parameters and check if device is already
// in registry. Jump to RunL as soon as possible.
// -----------------------------------------------------------------------------
//
void CSyncMLDlgNotifier::GetParamsL( const TDesC8& aBuffer,
                                           TInt    aReplySlot,
                                     const RMessagePtr2& aMessage )
    {
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::GetParamsL()") );
    const TChar KDRSeparator('-');
    const TChar KChoiceItemSeparator(',');
    if ( iReplySlot != NULL || iNeedToCompleteMessage )
        {
        User::Leave( KErrInUse );
        }
    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;

    TSyncMLDlgNotifParams param;
    TPckgC<TSyncMLDlgNotifParams> pckg( param );
    pckg.Set( aBuffer );
    iNoteType = pckg().iNoteType;
    iServerMsg.Copy( pckg().iServerMsg.Left( KSyncMLMaxServerMsgLength ) );
    iMaxTime = pckg().iMaxTime;
    iMaxlen = pckg().iMaxLength;
    //check for iDR is there are not
    iDR.Copy( pckg().iDR.Left( KSyncMLMaxDefaultResponseMsgLength ) );
    iIT = pckg().iIT;
    iET = pckg().iET;
    //verification of other params as like maxtime
    if ( iMaxlen <= 0 || iMaxlen >KSyncMLMaxDefaultResponseMsgLength )
        {
        iMaxlen = KSyncMLMaxDefaultResponseMsgLength;
        }

    if ( iIT < ESyncMLInputTypeAlphaNumeric || iIT > ESyncMLInputTypeIPAddress )
        {
        iIT = ESyncMLInputTypeAlphaNumeric;
        }

    if ( iET < ESyncMLEchoTypeText || iET > ESyncMLEchoTypePassword )
        {
        iET = ESyncMLEchoTypeText;
        }

    if( iServerMsg.Length() == 0 )
        {
        HBufC* stringholder = NULL;
        stringholder = StringLoader::LoadLC( R_DEFAULT_ALERT_HEADING );
        iServerMsg.Copy( stringholder->Des() );
        CleanupStack::PopAndDestroy( stringholder );
        }
    if ( iMaxTime < 0 )
        {
        iMaxTime = 0;
        }
    if (iNoteType == ESyncMLSingleChoiceQuery || iNoteType == ESyncMLMultiChoiceQuery )
        {
		iNumberOfItems = pckg().iNumberOfItems;  		
        if(iNumberOfItems > 0)
            {            
            iListItems = new (ELeave) CDesCArrayFlat( iNumberOfItems );
            }
		else
			{
		    aMessage.Complete( KErrCancel );
			iNeedToCompleteMessage = EFalse;
			return;
			}
        TPtrC8 listitemsptr;
        TBuf<KSyncMLChoiceItemsLengthBuffer> listlengtharray;                
        User::LeaveIfError(iChunk.OpenGlobal(pckg().iChunkName,EFalse));
        TInt size1 = iChunk.Size();
        listitemsptr.Set(iChunk.Base(),size1);        
        HBufC* databuf = NULL;
        TRAPD(errC, databuf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(listitemsptr));
        iChunk.Close();
        if(errC)
            {
            aMessage.Complete( errC );
            iNeedToCompleteMessage = EFalse;
            return;
            }
        CleanupStack::PushL(databuf);
        TPtr listitems = databuf->Des();                       		
        listlengtharray=pckg().iItemLength;      
        TPtrC temp1,temp2,temp3,temp4;        
        TInt currlen=0,itemlength=0;
        TInt prevcommapos=0;
        TBuf<4> checkon(_L("0\t"));
        TBuf<4> checkoff(_L("1\t"));        
        for(TInt i=0;i<listlengtharray.Length();i++)
            {            
            if(listlengtharray[i]== KChoiceItemSeparator )
                {
                if(prevcommapos)
                    {
                    temp1.Set(listlengtharray.LeftTPtr(i));
                    temp2.Set(temp1.Right(i-(prevcommapos+1)));
                    prevcommapos=  i;
                    }
                else //firsttime finding comma
                    {
                    prevcommapos=  i;
                    temp1.Set(listlengtharray.LeftTPtr(i));
                    temp2.Set(temp1.Right(i));
                    }                
                TLex lexval;
                lexval.Assign(temp2);
                lexval.Val(itemlength);                
                temp3.Set(listitems.Left(currlen + itemlength));
                temp4.Set(temp3.Right(itemlength));
                currlen += itemlength;                
                HBufC* item1 = HBufC::NewLC(temp4.Length()+3);
                if(iNoteType == ESyncMLMultiChoiceQuery)
                    {
                    item1->Des().Append(checkoff);//by default dont show check on                                     
                    }                
                item1->Des().Append(temp4);
                iListItems->AppendL(item1->Des());
                CleanupStack::PopAndDestroy(1);//item,dataBuf16                
                }
            }
        CleanupStack::PopAndDestroy(1);
        }   
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );

    FTRACE( FPrint( _L(
            "[SmlNotif]\t CSyncMLDlgNotifier::GetParamsL() completed, iNoteType = %d, iTimeout = %d, iServerMsg = " ),
            iNoteType, iMaxTime ) );
    FTRACE( FPrint( _L(
            "[SmlNotif]\t \"%S\"" ),
            &iServerMsg ) );
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::Cancel
// -----------------------------------------------------------------------------
//
void CSyncMLDlgNotifier::Cancel()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::Cancel()"));

    CSyncMLNotifierBase::Cancel();

    FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::Cancel() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::RunL
// Ask user response and return it to caller.
// Store device into registry if user has accepted authorisation.
// -----------------------------------------------------------------------------
//
void CSyncMLDlgNotifier::RunL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL()"));
    const TChar KDRSeparator('-');
    const TChar KChoiceItemSeparator(',');
    TInt result( KErrNone );
 		EndKeyPress = EFalse;
    // Turn lights on and deactivate apps -key
    //
    TurnLightsOn();  

    switch( iNoteType )
        {
        case ESyncMLInfoNote:
            {
            CAknInformationNote* infoNote = new (ELeave) CAknInformationNote;
            TInt maxtime = iMaxTime * KSyncMLuSecsInSec;
            if ( maxtime < 0  )
                {
                maxtime = 0;
                }
            infoNote->SetTimeout( (CAknNoteDialog::TTimeout) maxtime );
            result = infoNote->ExecuteLD( iServerMsg );
            if ( result == KErrNone )
                {
                result = EAknSoftkeyOk;
                }
            break;
            }
        case ESyncMLErrorNote:
            {
            CAknErrorNote* errNote = new (ELeave) CAknErrorNote;
            TInt maxtime = iMaxTime * KSyncMLuSecsInSec;
            if ( maxtime < 0  )
                {
                maxtime = 0;
                }
            errNote->SetTimeout( (CAknNoteDialog::TTimeout) maxtime );
            result = errNote->ExecuteLD( iServerMsg );
            if ( result == KErrNone )
                {
                result = EAknSoftkeyOk;
                };
            break;
            }
        case ESyncMLOkQuery:
            {
            FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() Ok/Empty query shown") );
            
            CSyncMLTimedMessageQuery* dlg =
                CSyncMLTimedMessageQuery::NewL( iServerMsg, iMaxTime );
            
            // Pushed dialog is popped inside RunLD
            dlg->PrepareLC( R_SML_MESSAGE_QUERY );
            	dlg->ButtonGroupContainer().SetCommandSetL( 
                                        R_AVKON_SOFTKEYS_OK_EMPTY__OK );
            EnableSyncProgressNoteL( EFalse );
            result = dlg->RunLD();
            dlg = NULL;
            EnableSyncProgressNoteL( ETrue );
            break;
            }
            
        case ESyncMLQueryNote:  // This enumeration will be removed.
        case ESyncMLYesNoQuery:
            {
            FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() Yes/No query shown") );
            CSyncMLTimedMessageQuery* dlg = 
            CSyncMLTimedMessageQuery::NewL( iServerMsg, iMaxTime );
            // Pushed dialog is popped inside RunLD
            dlg->PrepareLC( R_SML_MESSAGE_QUERY );
            dlg->ButtonGroupContainer().SetCommandSetL( 
                    R_AVKON_SOFTKEYS_YES_NO__YES );
            EnableSyncProgressNoteL( EFalse );
            result = dlg->RunLD();
            dlg = NULL;
            EnableSyncProgressNoteL( ETrue );
            break;
            }
        case ESyncMLSingleChoiceQuery:
            {
			FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() single choice list query") );             
            if ( iNumberOfItems > 0 )
                {
                SuppressAppSwitching( ETrue );
                EnableSyncProgressNoteL( EFalse );
                CEikFormattedCellListBox* listBox = new ( ELeave )
                CAknSinglePopupMenuStyleListBox;
                CleanupStack::PushL( listBox );
                CSyncMLAknPopUpList* popupList = 
                CSyncMLAknPopUpList::NewL( (CAknSingleHeadingPopupMenuStyleListBox*)listBox, 
                            R_AVKON_SOFTKEYS_OK_CANCEL__OK,
                            AknPopupLayouts::EMenuWindow,iMaxTime );
                CleanupStack::PushL( popupList );
                listBox->ConstructL( popupList, ( EAknListBoxSelectionList | EAknListBoxLoopScrolling ) );
                listBox->Model()->SetItemTextArray( iListItems );
                listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
                listBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
                listBox->HandleItemAdditionL();
                listBox->CreateScrollBarFrameL( ETrue );
                listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
                TInt defaultindex = 0;
                TLex drval(iDR);                    
                drval.Val(defaultindex);
                if(defaultindex > 0)
                    defaultindex--;
                else
                    defaultindex = 0;
                listBox->SetCurrentItemIndex(defaultindex);                                   
                popupList->SetTitleL( iServerMsg );
                CleanupStack::Pop( popupList );
				FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() single choice list query shown") ); 
                result = popupList->ExecuteLD();
				FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() single choice list query dismissed") ); 
                popupList = NULL;
                if ( result )
                    {
                    reply.irettext.Num(listBox->CurrentItemIndex()+1);
                    }
                reply.iretval = result;                
                CleanupStack::PopAndDestroy( listBox );  // Destroys also the icon array
                EnableSyncProgressNoteL( ETrue );
                SuppressAppSwitching( EFalse );
                }
				else
				{
				reply.iretval = result;				
				}
				TSyncMLDlgNotifReturnParamsPckg pkg(reply);
                iMessage.WriteL(iReplySlot,pkg);
            break;
            }
        case ESyncMLMultiChoiceQuery:
            {           
            if ( iNumberOfItems > 0 )
                {
				SuppressAppSwitching( ETrue );
                EnableSyncProgressNoteL( EFalse );
                // create listbox
                // list item string format: "0\tLabel" where 0 is an index to icon array
                CEikFormattedCellListBox* listBox =
                new ( ELeave ) CAknSingleGraphicPopupMenuStyleListBox;
                CleanupStack::PushL( listBox );
                // create popup
                CSyncMLAknPopUpList* popup = 
                CSyncMLAknPopUpList::NewL((CAknSingleHeadingPopupMenuStyleListBox*) listBox,
                            R_AVKON_SOFTKEYS_OK_CANCEL__MARK ,
                            AknPopupLayouts::EMenuWindow,iMaxTime);
                CleanupStack::PushL( popup );
                popup->SetTitleL(iServerMsg );
#ifdef RD_SCALABLE_UI_V2
                if( AknLayoutUtils::PenEnabled() )
                    {
                    listBox->ConstructL( popup, EAknListBoxStylusMultiselectionList );
                    }
                else
                    {
                    listBox->ConstructL( popup, EAknListBoxMultiselectionList );
                    }
#else
                listBox->ConstructL( popup, EAknListBoxMultiselectionList );
#endif
                listBox->CreateScrollBarFrameL(ETrue);
                listBox->ScrollBarFrame()->
                SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );                
                SetIconsL(listBox);
                listBox->Model()->SetItemTextArray( iListItems );
                listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
                listBox->SetCurrentItemIndex(iDefindex);  
                listBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );                
                listBox->HandleItemAdditionL();
                // set all items as selected
                TInt count( listBox->Model()->NumberOfItems() );                
                TBuf<20> drcheck;
                for( TInt index( 0 ); index < count; index++ )
                    {
                    drcheck.Zero();
                    drcheck.Append(KDRSeparator);
                    drcheck.AppendNum(index+1);
                    drcheck.Append(KDRSeparator);
                    if(iDR.Find(drcheck) >= KErrNone)
                        {
                        listBox->View()->SelectItemL( index );
                        }                                                            
                    }              
                // launch popup
                result = popup->ExecuteLD();
                // No leaving functions allowed between executeLD and CleanupStack::Pop().
                CleanupStack::Pop( popup );                
                /// Own: ListBox selection array
                const CArrayFix<TInt>* iSelectionArray;                
                if( result )
                    {
                    iSelectionArray = listBox->View()->SelectionIndexes();
                    TBuf<20> temp;
                    TBuf<KSyncMLMaxAlertResultLength> temp1;
                    for(TInt i=iSelectionArray->Count()-1;i>=0;i--)
                        {
                        temp.Num(iSelectionArray->At(i)+1);
                        if((temp1.Length() + temp.Length()) < KSyncMLMaxAlertResultLength )
                            {
                            temp1.Append(temp);
                            temp1.Append(KDRSeparator);                            
                            }
                        else{
                            break;
                            }
                        } 
                    reply.irettext.Append(temp1);                    
                    // delete iSelectionArray; this deletion taken care in ~CEIKLISTBOX (eiklbx.cpp)
                    }
                reply.iretval = result;
                CleanupStack::PopAndDestroy(); // listBox                
				EnableSyncProgressNoteL( ETrue );
                SuppressAppSwitching( EFalse );					
                }
				else
				{
				reply.iretval = result;				
				}
				TSyncMLDlgNotifReturnParamsPckg pkg(reply);
                iMessage.WriteL(iReplySlot,pkg);                                          
            break;
            }
        case ESyncMLInputQuery: //For 1102 alert
            {   
            FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() Input query shown") ); 
            // For input text server alert
              
            SuppressAppSwitching( ETrue );

             switch( iIT )   
             {
             	case ESyncMLInputTypeAlphaNumeric: //Alphanumeric Input type
             	    {
             	     
          				InputTypeAlphaNumericL(result);
             	     break;
             	    }
             	    	
             	case ESyncMLInputTypeNumeric: //Numeric Input type
             		{
             		InputTypeNumericL(result);
             		break;
             		}	
             		
             	case ESyncMLInputTypeDate: //Date Input type
             	    {
								InputTypeDateL(result);
             	    break;
             	    }		
             			
             	case ESyncMLInputTypeTime: //Time Input type
             		{//create TTime from TBuf iDR
             		//hhmmss
								InputTypeTimeL(result);
             		break;	
             		}
             		
             	case ESyncMLInputTypePhoneNumber: //Phone Number Input type
             		{
								InputTypePhoneNumberL(result);
             		break;
             		}	
             		
             	case ESyncMLInputTypeIPAddress: //IP Address Input type
             	    {			
             	    	TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
                	TInetAddr destAddr;
					destAddr.Input(iDR);					
					EnableSyncProgressNoteL( EFalse );//coment out when using test app
					CAknIpAddressQueryDialog* dlg = CAknIpAddressQueryDialog::NewL (destAddr); 
					CleanupStack::PushL(dlg);
					dlg->SetPromptL(iServerMsg);
					CleanupStack::Pop(); //dlg
					result=dlg->ExecuteLD( R_NEWALERT_IP_QUERY );
					dlg=NULL;
					destAddr.Output(InputText);				
					reply.iretval = result;
					reply.irettext=InputText;					
             		EnableSyncProgressNoteL( ETrue );//coment out when using test app	
             	    break;
             	    }
             			
                default: 
                    {
                    	User::Leave( KErrArgument );
                    }			
             }
             SuppressAppSwitching( EFalse );
             TSyncMLDlgNotifReturnParamsPckg pkg(reply);
             iMessage.WriteL(iReplySlot,pkg);
             break;
            }
//#endif
            
        default: // Unhandled note type
            {
            FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() Note type not handled"));
            User::Leave( KErrArgument );
            break;
            }
        }
    // Complete message, if not cancelled
    if ( iNeedToCompleteMessage )
        {
		 FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() completing the message"));
        switch ( result )
            {
            case EAknSoftkeyYes:
            case EAknSoftkeyOk:
            case ETrue:    
                {
				FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() completing the message with KErrNone"));
                iMessage.Complete( KErrNone );
                break;
                }
            case EAknSoftkeyCancel:
            case EAknSoftkeyNo:
            case KErrNone:  // Pressing "Cancel" or "No" may return zero
                {
				FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() completing the message with KErrCancel"));
                iMessage.Complete( KErrCancel );
                break;
                }
            default:
                {
                iMessage.Complete( result );
                break;
                }
            }
        }

    iNeedToCompleteMessage = EFalse;
    iReplySlot = NULL;

    iServerMsg = KNullDesC;

    FLOG(_L("[SmlNotif]\t CSyncMLDlgNotifier::RunL() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::CSyncMLDlgNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLDlgNotifier::CSyncMLDlgNotifier( CSyncMLAppLaunchNotifier* aAppLaunchNotif )
    : iAppLaunchNotif( aAppLaunchNotif ),iListItems(NULL),iDefindex(0)
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::InputTypeAlphaNumericL(TInt & result) 
// Process if input type is alphanumeric
// -----------------------------------------------------------------------------
//    
void CSyncMLDlgNotifier::InputTypeAlphaNumericL(TInt & result)    
{
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeAlphaNumericL() startedd") );
		TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
		if( iET == ESyncMLEchoTypePassword ) //for pwd ET
    {
    		CSyncMLTimedInputTextQuery* dlg = CSyncMLTimedInputTextQuery::NewL( InputText, iServerMsg, EndKeyPress, iMaxTime);
				dlg->SetMaxLength( iMaxlen );
				EnableSyncProgressNoteL( EFalse );//coment out when using test app
				result = dlg->ExecuteLD( R_NEWALERT_DATA_PWD_QUERY );
				if(EndKeyPress)
				{
					 result = KErrAbort;	
				}
				reply.iretval = result;
				reply.irettext=InputText;
				dlg=NULL;
				EnableSyncProgressNoteL( ETrue );//coment out when using test app
    }
    else
    {
        CSyncMLTimedInputTextQuery* dlg = CSyncMLTimedInputTextQuery::NewL( InputText,iServerMsg, EndKeyPress, iMaxTime);
				dlg->SetMaxLength( iMaxlen );
				EnableSyncProgressNoteL( EFalse );//coment out when using test app
				result = dlg->ExecuteLD( R_INPUTTEXT_DATA_QUERY );
				if(EndKeyPress )
				{					 					 
					 result = KErrAbort;
        }
				reply.iretval = result;
				reply.irettext=InputText;
				dlg=NULL;
				EnableSyncProgressNoteL( ETrue );//coment out when using test app		
    }
    FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeAlphaNumericL() completed") );
}

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::InputTypeNumericL(TInt & result) 
// Process if input type is numeric
// -----------------------------------------------------------------------------
//    
void CSyncMLDlgNotifier::InputTypeNumericL(TInt & result)
{
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeNumericL() started") ); 
		TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
	  TReal value = 0;
    TLex data(iDR);
    if( iDR.Length() != 0 )
    {
    		data.Val(value);
    }
    if ( iET == ESyncMLEchoTypePassword )
    {
    		TChar ch = '0';
        for (TInt i=0;i<iDR.Length();i++)
        {
        		ch=iDR[i];
            if( ch!='+' || ch!='-' || ch!='.' )
            {
            		if( !ch.IsDigit() )
             	  {
             	  		InputText.Zero();
             	    	break;	
             	  }
            }
        }
        CSyncMLTimedInputTextQuery* dlg = CSyncMLTimedInputTextQuery::NewL( InputText, iServerMsg, EndKeyPress, iMaxTime);
				dlg->SetMaxLength( iMaxlen );
				EnableSyncProgressNoteL( EFalse );//coment out when using test app
				result = dlg->ExecuteLD( R_NEWALERT_NUMERIC_PWD_QUERY );
				dlg = NULL;
				if(EndKeyPress)
				{
					 result = KErrAbort;	
				}
		}
    else
    {
    		CSyncMLTimedNumberQueryDialog* dlg =
				CSyncMLTimedNumberQueryDialog::NewL( value, iServerMsg, EndKeyPress,  iMaxTime);
				    
				EnableSyncProgressNoteL( EFalse );//coment out when using test app
				result = dlg->ExecuteLD( R_NEWALERT_NUMBER_QUERY );
				dlg = NULL;
				if(EndKeyPress)
				{
					result = KErrAbort;	
				}
				reply.iretval = result;
				data.Val( value );
				_LIT(KSendFormat ,"%10.10f");
				InputText.Format ( KSendFormat, value );
		}
		reply.iretval = result;
		reply.irettext= InputText;
								 
		EnableSyncProgressNoteL( ETrue );//coment out when using test app  
		
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeNumericL() completed") );    		
}

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::InputTypeDateL(TInt & result) 
// Process if input type is Date
// -----------------------------------------------------------------------------
//  
void CSyncMLDlgNotifier::InputTypeDateL(TInt & result)
{
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeDateL() started") ); 
		TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
	  TTime date(_L("00010101:"));
    TInt BadData = 1;
    TChar ch = '0';
    for (TInt i=0;i<iDR.Length();i++)
    {
    		ch=iDR[i];
        if(!ch.IsDigit())
        {
        		BadData=0;
            break;
        }
    }
    if( iDR.Length() != 0 && iDR.Length()==8 && BadData )
    {
    		//iDR.Append(':');
        TBuf <9> revdate;
				revdate.Append(iDR.Mid(4,4));
				revdate.Append(iDR.Mid(2,2));
				revdate.Append(iDR.Left(2));
				revdate.Append(':');
        date.Set( revdate );
    }
		CSyncMLTimedDateQuery* dlg = CSyncMLTimedDateQuery::NewL( date, iServerMsg,EndKeyPress ,  iMaxTime);
		EnableSyncProgressNoteL( EFalse );//coment out when using test app
		result = dlg->ExecuteLD(R_NEWALERT_DATE_QUERY); 
					
		if(EndKeyPress)
		{
				result = KErrAbort;//0-keypress 1-ok,0-cancel,-1=end key	
		}
		reply.iretval = result;
		_LIT(KDateFormat,"%F%D%M%Y");
		TBuf <20> StringDate;					
		date.FormatL(StringDate, KDateFormat);
		reply.irettext=StringDate;
		dlg = NULL;
		EnableSyncProgressNoteL( ETrue );//comented out for emulator testing
		
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeDateL() completed") );            
}		

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::InputTypeTimeL(TInt & result) 
// Process if input type is Time
// -----------------------------------------------------------------------------
//             	    
void CSyncMLDlgNotifier::InputTypeTimeL(TInt & result)
{            	
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeTimeL() started") );
		TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
    //create TTime from TBuf iDR
    //hhmmss
    TTime IpTime(_L("120000."));
    TInt BadData=1;
    TChar ch= '0';
    for (TInt i=0;i<iDR.Length();i++)
    {
    		ch=iDR[i];
        if(!ch.IsDigit())
        {
        		BadData=0;
            break;
        }
    }
    if( iDR.Length() != 0 && iDR.Length() == 6 && BadData )
    {
    		iDR.Append('.');
        IpTime.Set( iDR );
    }	             	
		CSyncMLTimedDateQuery* dlg =
		CSyncMLTimedDateQuery::NewL( IpTime, iServerMsg, EndKeyPress, iMaxTime);					
		EnableSyncProgressNoteL( EFalse );//comented out for emulator testing
		result = dlg->ExecuteLD(R_NEWALERT_TIME_QUERY);					
		if(EndKeyPress)
		{
				result = KErrAbort;//0-keypress 1-ok,0-cancel,-1=end key	
		}
		reply.iretval = result;				
		_LIT(KTimeFormat,"%F%H%T%S");
		TBuf <20> StringTime;				
		IpTime.FormatL(StringTime, KTimeFormat);
		reply.irettext=StringTime;				
		dlg = NULL;  
		EnableSyncProgressNoteL( ETrue );//coment out when using test app
		
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypeTimeL() completed") );
}

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::InputTypePhoneNumberL(TInt & result) 
// Process if input type is Phone number
// -----------------------------------------------------------------------------
//  
void CSyncMLDlgNotifier::InputTypePhoneNumberL(TInt & result)
{
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypePhoneNumberL() started") );
		TBuf<KSyncMLMaxDefaultResponseMsgLength> InputText(iDR);
		EnableSyncProgressNoteL( EFalse );//comented out for emulator testing
		TInt ret=0;
		for(TInt i=0;i<iDR.Length();i++)
		{
				if(iDR[i] == 'p' || iDR[i] == 'w' || iDR[i] == '+' || iDR[i] == '*' ||iDR[i] == '#')
				{
						if( iDR[i] == '+' && i > 0)
					 	{
					 			ret = -1;
					 			break;
					 	}
					 	else
					 {
							continue;
					 }
				}
				else
				{
						TChar ch=iDR[i] ;
			 			if(!ch.IsDigit())
			 			{
			  				ret = -1;
			 					break;	
			 			}
				}
		}
				
		if(ret == -1)
		InputText.Zero();
		CSyncMLTimedInputTextQuery* dlg = CSyncMLTimedInputTextQuery::NewL(InputText,iServerMsg, EndKeyPress, iMaxTime);					
		dlg->SetMaxLength( iMaxlen );					 
		result = dlg->ExecuteLD( R_NEWALERT_PHONE_QUERY );
		if(EndKeyPress )
		{
					result = KErrAbort;
		}
		reply.iretval = result;					
		reply.irettext=InputText;
		dlg=NULL;					
		EnableSyncProgressNoteL( ETrue );//coment out when using test app 
		FLOG( _L("[SmlNotif]\t CSyncMLDlgNotifier::InputTypePhoneNumberL() completed") );
}

// -----------------------------------------------------------------------------
// CSyncMLDlgNotifier::SetIconsL() 
// Sets the icons to multiselection/markable list
// -----------------------------------------------------------------------------
void CSyncMLDlgNotifier::SetIconsL(CEikFormattedCellListBox* aListBox)
    {        
    CAknIconArray* iconArray = new( ELeave ) CAknIconArray( 1 );
        CleanupStack::PushL( iconArray );
        CFbsBitmap* checkboxOnBitmap = NULL;
        CFbsBitmap* checkboxOnBitmapMask = NULL;
        CFbsBitmap* checkboxOffBitmap = NULL;
        CFbsBitmap* checkboxOffBitmapMask = NULL;

        //CListItemDrawer is using this logical color as default for its marked icons
        TRgb defaultColor;
        defaultColor = iEikEnv->Color( EColorControlText );

        // Create 'ON' checkbox icon
        AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiCheckboxOn,
                    KAknsIIDQsnIconColors,
                    EAknsCIQsnIconColorsCG13,
                    checkboxOnBitmap,
                    checkboxOnBitmapMask,
                    KAvkonBitmapFile,
                    EMbmAvkonQgn_indi_checkbox_on,
                    EMbmAvkonQgn_indi_checkbox_on_mask,
                    defaultColor
                    );

        CGulIcon* checkboxOnIcon = CGulIcon::NewL( checkboxOnBitmap, checkboxOnBitmapMask );
        // cleanup checkboxOnBitmap, checkboxOnBitmapMask
        CleanupStack::Pop( checkboxOnBitmapMask );
        CleanupStack::Pop( checkboxOnBitmap ); 

        CleanupStack::PushL( checkboxOnIcon );
        iconArray->AppendL( checkboxOnIcon );

        // Create 'OFF' checkbox icon
        AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiCheckboxOff,
                    KAknsIIDQsnIconColors,
                    EAknsCIQsnIconColorsCG13,
                    checkboxOffBitmap,
                    checkboxOffBitmapMask,
                    KAvkonBitmapFile,
                    EMbmAvkonQgn_indi_checkbox_off,
                    EMbmAvkonQgn_indi_checkbox_off_mask,
                    defaultColor
                    );

        CGulIcon* checkboxOffIcon = CGulIcon::NewL( checkboxOffBitmap, checkboxOffBitmapMask );
        // cleanup checkboxOffBitmap, checkboxOffBitmapMask
        CleanupStack::Pop( checkboxOffBitmapMask ); //WILL BE DELETED BY CGUIICON
        CleanupStack::Pop( checkboxOffBitmap ); //WILL BE DELETED BY CGUIICON
        

        CleanupStack::PushL( checkboxOffIcon );
        iconArray->AppendL( checkboxOffIcon );
        aListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray ); // changes ownership
        
        // cleanup checkboxOnIcon, checkboxOffIcon, iconArray
        CleanupStack::Pop( checkboxOffIcon ); //WILL BE DELETED BY CGUIICON
        CleanupStack::Pop( checkboxOnIcon ); //WILL BE DELETED BY CGUIICON
        CleanupStack::Pop( iconArray );//WILL BE DELETED BY CFormattedCellListBoxData::

    }

//  End of File  
