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
* Description:  SyncML DM command handling 
*
*/



// INCLUDE FILES
//
#include <f32file.h>
#include <utf.h>
// FOTA
#include <DevManInternalCRKeys.h>
#include <featmgr.h>
// FOTA end
#include <SyncMLNotifierParams.h>

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmlphoneinfo.h>
#include <nsmlunicodeconverter.h>
// DM specific includes
#include <nsmldmconst.h>
#include <nsmldmmodule.h>
//For P&S keys
#include <e32property.h>
#include <e32math.h> 
#include "nsmldminternalpskeys.h"
// common includes with DS
#include "nsmlcliagconstants.h"
#include "NSmlCmdsBase.h"
#include "NSmlAgentBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResponseController.h"
#include "NSmlResultsContainer.h"
#include "NSmlAuth.h"
#include "NSmlURI.h"
#include "WBXMLSyncMLGenerator.h"
#include "WBXMLSyncMLParser.h"
#include "WBXMLGeneratorError.h"
#include "WBXMLParser.h"
#include "smldtd.h"
#include "smldef.h"
#include "nsmlagenttestdefines.h"
// DM specific includes
#include "nsmldmagconstants.h"
#include "NSmlDMCmds.h"
#include "nsmldmerror.h"
#ifdef __TEST_TREEMODULE
#include "nsmldmtestmodule.h"
#else
#include <nsmldmmodule.h>
#endif


#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


const TUid KNSmlSyncDialogUid = { 0x101F876A };
// FOTA
const TInt KNSmlDmNoRequest = -1;
// FOTA end
const TInt KNSmlDmChoiceChunkMinSize = 32;
const TInt KNSmlDmChoiceChunkMaxSize = 10000;

_LIT8 ( KNSmlDMMetaTypeUserRequest, "org.openmobilealliance.dm.firmwareupdate.userrequest" );
_LIT8 ( KNSmlDMMetaFormatUserRequest, "chr" );
_LIT(KChunkName,"AlertItems");


// ---------------------------------------------------------
// CNSmlDMCmds::NewL()
// Creates new instance of CNSmlDMCmds 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlDMCmds* CNSmlDMCmds::NewL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, const TDesC& aDMServerId, MSyncMLProgressObserver* aDMObserver )
    {
	CNSmlDMCmds* self = new (ELeave) CNSmlDMCmds();
	CleanupStack::PushL( self );
    self->ConstructL( aAgent, aSessionID, aVerProto, aSyncMLUserName, aSyncServer, aDMServerId, aDMObserver );
    CleanupStack::Pop(); //self
	return self;
    }

// ---------------------------------------------------------
// CNSmlDMCmds::~CNSmlDMCmds()
// Destructor
// ---------------------------------------------------------
//
CNSmlDMCmds::~CNSmlDMCmds()
    {
	delete iDMServerId;
	delete iDMModule;
	if ( iDMDevInfo )
		{
		delete iDMDevInfo->iFormat;
		delete iDMDevInfo->iType; 
		delete iDMDevInfo->iObject;
		delete iDMDevInfo;
		}
	delete iLargeObjectUri;

	if(iChunk.Handle())
	    {
	    iChunk.Close();
	    }
	// FOTA
	// uninitialize feature manager
	FeatureManager::UnInitializeLib();
	delete iRepository;
	
	//P&S key deletion,even fine if the key is not there
	RProperty::Delete( KPSUidNSmlDMSyncAgent,KNSmlDMCmdAddOnExistingNodeorLeafKey);	
	// FOTA end	
	}


// ---------------------------------------------------------
// CNSmlDMCmds::CNSmlDMCmds()
// Constructor
// ---------------------------------------------------------
//
CNSmlDMCmds::CNSmlDMCmds()
    {
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ConstructL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ConstructL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, const TDesC& aDMServerId, MSyncMLProgressObserver* aDMObserver )
    {
    if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
    {
	CNSmlCmdsBase::ConstructL( aAgent, aSessionID, aVerProto,KNSmlSyncMLPublicId12 , aSyncMLUserName, aSyncServer, EFalse );
    }
    else
    {
    CNSmlCmdsBase::ConstructL( aAgent, aSessionID, aVerProto,KNSmlSyncMLPublicId , aSyncMLUserName, aSyncServer, EFalse );	
    }
	iDMServerId = aDMServerId.AllocL();
	iDMObserver = aDMObserver;
#ifdef __TEST_TREEMODULE
	iDMModule = CNSmlDmTestModule::NewL( this );
#else
	iDMModule = CNSmlDmModule::NewL( this );
#endif
	HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aDMServerId);
	CleanupStack::PushL(serverId);
	iDMModule->SetServerL( *serverId );
	CleanupStack::PopAndDestroy(); //serverId
	
	// FOTA
	// init feature manager
	FeatureManager::InitializeLibL();
	if ( FeatureManager::FeatureSupported ( KFeatureIdSyncMlDmFota ) )
		{
		TRAPD ( error, iRepository = CRepository::NewL ( KCRUidDeviceManagementInternalKeys ) );
		if ( error != KErrNone )
			{
			iRepository = NULL;
			}
		}
	// FOTA end	
    }

// ---------------------------------------------------------
// CNSmlDMCmds::DoDevInfoItemsL()
// Function which is called recursive when writing DevInfo items
// ---------------------------------------------------------
void CNSmlDMCmds::DoDevInfoItemsL( const TDesC8& aURI ) 
	{
	iDMModule->FetchObjectL( aURI, KNullDesC8, 0, 0, ETrue );
	if ( !iDMDevInfo->iResults )
		{
		return;
		}
	iDMDevInfo->iResults = EFalse;
	if ( *iDMDevInfo->iFormat == _L8("node") )
		{
		HBufC8* childList = iDMDevInfo->iObject->AllocLC();
		TPtrC8 childListPtr( *childList );
		TInt pos = 0;
		TBool moreData = ETrue;
		do 
			{
			pos = childListPtr.Locate('/');
			if ( pos == KErrNotFound )
				{
				moreData = EFalse;
				pos = childListPtr.Length();
				}
			User::LeaveIfError( pos );
			HBufC8* childURI = HBufC8::NewLC( aURI.Length() + 1 + pos );
			*childURI = aURI;
			TPtr8 childURIPtr = childURI->Des();
			childURIPtr += _L8("/");
			HBufC8* segment = HBufC8::NewLC( pos );
			segment->Des().Copy( childListPtr.Left( pos ) );
			childURIPtr += *segment;
			DoDevInfoItemsL( *childURI );
			if ( moreData )
				{
				childListPtr.Set( childListPtr.Right( childListPtr.Length() - ( segment->Length() +1 ) ));
				}
			CleanupStack::PopAndDestroy( 2 ); // segment, childURI
			} while ( moreData );
		CleanupStack::PopAndDestroy(); // childList
		}
	else
		{
		if ( iDMDevInfo->iFirst )
			{
			iDMDevInfo->iFirst = EFalse;
			}
		else
			{
			(*iDMDevInfo->iItemListPtr)->next = new( ELeave ) SmlItemList_t;
			iDMDevInfo->iItemListPtr = &(*iDMDevInfo->iItemListPtr)->next;
			(*iDMDevInfo->iItemListPtr)->item = new( ELeave ) SmlItem_t;
			}
		DoSourceL( (*iDMDevInfo->iItemListPtr)->item->source, aURI );
		SmlMetInfMetInf_t* metInf;
		DoMetInfLC( metInf ); 
		PcdataNewL( metInf->format, *iDMDevInfo->iFormat );
		PcdataNewL( metInf->type, _L8("text/plain") );
		DoMetaL( (*iDMDevInfo->iItemListPtr)->item->meta, metInf );
		CleanupStack::Pop(); //metInf , only Pop because ownership has been changed
		PcdataNewL( (*iDMDevInfo->iItemListPtr)->item->data, *iDMDevInfo->iObject );
		}
	}
// ---------------------------------------------------------
// CNSmlDMCmds::FreeDMDevinfo()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::FreeDMDevinfo()
	{
	if ( iDMDevInfo )
		{
		delete iDMDevInfo->iFormat;
		delete iDMDevInfo->iType; 
		delete iDMDevInfo->iObject;
		delete iDMDevInfo;
		iDMDevInfo = NULL;
		}
	}
// ---------------------------------------------------------
// CNSmlDMCmds::AlertParameter()
// 
// ---------------------------------------------------------
TInt CNSmlDMCmds::AlertParameter( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
	{
	TInt valueNum = 0;
	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				if ( parameters.Length() > startPos + aParamID.Length() )
					{
					TPtrC8 strPart = parameters.Right( parameters.Length() - ( startPos + aParamID.Length() ) );
					TInt length = 0;
					while ( length < strPart.Length() )
						{
						TChar character = strPart[length];
						if ( character.IsDigit() )
							{
							++length;
							}
						else
							{
							break;
							}
						}
					TPtrC8 valueStr = strPart.Left(	length );
					TLex8 lexicalValue( valueStr );
					if ( lexicalValue.Val( valueNum ) != KErrNone )
						{
						valueNum = 0;
						}
					}
				}
			}
		}
	return valueNum; 
	}

//#ifdef RD_DM_TEXT_INPUT_ALERT
// ---------------------------------------------------------
// CNSmlDMCmds::AlertInputTypeParameter()
// For 1102-User Input server alert
// For Input Type optional parameter
// ---------------------------------------------------------
TInt CNSmlDMCmds::AlertInputTypeParameter( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
	{
	_DBG_FILE("CNSmlDMCmds::AlertInputTypeParameter: begin");
	TInt valueNum = 0;//alphanumeric by default
	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				if ( parameters.Length() > startPos + aParamID.Length() )
					{
					TPtrC8 strPart = parameters.Right( parameters.Length() - ( startPos + aParamID.Length() ) );
					TInt length = 0;
					TChar character = strPart[length];
						
						switch( character )
						{
							case 'A': //Alphanumeric
							 valueNum = ESyncMLInputTypeAlphaNumeric;
							 break;
							case 'N': //Numeric
							 valueNum = ESyncMLInputTypeNumeric;
							 break;
							case 'D': //Date
							 valueNum = ESyncMLInputTypeDate;
							 break;
							case 'T': //Time
							 valueNum = ESyncMLInputTypeTime;
							 break;
							case 'P': //Phone number
							 valueNum = ESyncMLInputTypePhoneNumber;
							 break;
							case 'I': //Ip address
							 valueNum = ESyncMLInputTypeIPAddress;
							 break;
						}
					}
				}
			}
		}
		_DBG_FILE("CNSmlDMCmds::AlertInputTypeParameter: end");
	return valueNum; 
	}	
	
// ---------------------------------------------------------
// CNSmlDMCmds::AlertEchoTypeParameter()
// For 1102-User Input server alert
// For Echo Type optional parameter
// ---------------------------------------------------------	
TInt CNSmlDMCmds::AlertEchoTypeParameter( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
	{
	_DBG_FILE("CNSmlDMCmds::AlertEchoTypeParameter: begin");
	TInt valueNum = 0;//Text by default
	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				if ( parameters.Length() > startPos + aParamID.Length() )
					{
					TPtrC8 strPart = parameters.Right( parameters.Length() - ( startPos + aParamID.Length() ) );
					TInt length = 0;
					TChar character = strPart[length];
						switch( character )
						{
							 
							case 'T': valueNum = ESyncMLEchoTypeText;
							 break;
							case 'P': valueNum = ESyncMLEchoTypePassword;
							 break;
							 default: 
							 break;
						}
					}
				}
			}
		}
		_DBG_FILE("CNSmlDMCmds::AlertEchoTypeParameter: end");
	return valueNum; 
	}
// ---------------------------------------------------------
// CNSmlDMCmds::CheckDRPresence()
// For 1102-User Input server alert
// For Default Response item existence verification
// ---------------------------------------------------------		
TBool CNSmlDMCmds::CheckDRPresence( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
   {
   	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				 return ETrue;
				}
			}
		}
		return EFalse;
   }
	
// ---------------------------------------------------------
// CNSmlDMCmds::AlertDefaultResponseParam()
// For 1102-User Input server alert
// For Default Response optional parameter
// ---------------------------------------------------------
TPtrC8 CNSmlDMCmds::AlertDefaultResponseParam( const SmlPcdata_t* aData, const TDesC8& aParamID ) const
	{
	_DBG_FILE("CNSmlDMCmds::AlertDefaultResponseParam: begin");
	TLex8 value;
	if ( aData )
		{
		if ( aData->content )
			{
			TPtr8 parameters( (TUint8*) aData->content, aData->length, aData->length );
			TrimRightSpaceAndNull( parameters );
			TInt startPos = parameters.Find( aParamID );
			if ( startPos >= 0 )
				{
				if ( parameters.Length() > startPos + aParamID.Length() )
					{
					 TPtrC8 strPart = parameters.Right( parameters.Length() - ( startPos + aParamID.Length() ) );
					TInt length = 0;
					while ( length < strPart.Length() )
						{
						TChar character = strPart[length];
						if ( character != '&' )
							{
							++length;
							}
						else
							{
							break;
							}
						}
					TPtrC8 valueStr = strPart.Left(	length );
					value.Assign( valueStr );
										
					}
				}
			}
		}
		_DBG_FILE("CNSmlDMCmds::AlertDefaultResponseParam: end");
	 return value.Remainder();
	}	
//#endif

// ---------------------------------------------------------
// CNSmlDMCmds::AlertDataLC()
// 
// ---------------------------------------------------------
HBufC8* CNSmlDMCmds::AlertDataLC( const SmlItemList_t* aItemList ) const
	{
	HBufC8* unicodeData = NULL;
	_DBG_FILE("CNSmlDMCmds::AlertDataLC: Begin");
	if (  aItemList )
		{
		if (  aItemList->next )
			{
			if (  aItemList->next->item )
				{
			  if (  aItemList->next->item->data )
				  {
				  if (  aItemList->next->item->data->content )
					{
					TPtr8 data( (TUint8*)  aItemList->next->item->data->content,  aItemList->next->item->data->length, aItemList->next->item->data->length );
					TrimRightSpaceAndNull( data );
					unicodeData = data.AllocLC();
					data.Zero();
					}
				  }
				}
			}
		}
		
	if ( unicodeData == NULL )
		{
		unicodeData = HBufC8::NewLC( 0 );
		}
	_DBG_FILE("CNSmlDMCmds::AlertDataLC: end");	
	return unicodeData;
	}
// ---------------------------------------------------------
// CNSmlDMCmds::InitStatusToAtomicOrSequenceL()
// 
// ---------------------------------------------------------
TInt CNSmlDMCmds::InitStatusToAtomicOrSequenceL( const TDesC8& aCmd, const SmlAtomic_t* aAtomic ) const
	{
	TInt statusID( 0 );
	statusID = iStatusToServer->CreateNewStatusElementL(); 
	iStatusToServer->SetCmdRefL( statusID, aAtomic->cmdID );
	iStatusToServer->SetCmdL( statusID, aCmd ); 
	if ( iStatusToServerNoResponse || IsFlagSet( aAtomic->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	return statusID;
	}

// ---------------------------------------------------------
// CNSmlDMCmds::DoAlertL()
// Makes Alert element and calls parser to generate xml
// ---------------------------------------------------------
void CNSmlDMCmds::DoAlertL( const TDesC8& aAlertCode, TTime* /*aLastSyncAnchor*/, TTime* /*aNextSyncAnchor*/ )
	{
	//initialise Alert
	SmlAlert_t* alert = new( ELeave ) SmlAlert_t; 
	CleanupStack::PushL( alert );
	//element type
	alert->elementType = SML_PE_ALERT;
	//CmdID element
	DoCmdIdL( alert->cmdID );
	//Alert code to Data element 
	PcdataNewL ( alert->data, aAlertCode );
	TInt ret = iGenerator->smlAlertCmd( alert);
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	CleanupStack::PopAndDestroy(); //alert
	//save Status response information for response status controlling
	CreateAndInitResponseItemL( KNSmlAgentAlert );
	}

// FOTA
// ---------------------------------------------------------
// CNSmlDMCmds::DoGenericAlertL()
//	Makes a generic alert element (or possible many of them) 
//	and calls WBXML generator.
//	@param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
//	about to be reported.
//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command.
// ---------------------------------------------------------
//
void CNSmlDMCmds::DoGenericAlertL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, 
									TInt aFinalResult, const TDesC8& aFwCorrelator ) 
	{	
	SmlAlert_t* alert = new (ELeave) SmlAlert_t; 
	CleanupStack::PushL ( alert );
	alert->elementType = SML_PE_ALERT;
	DoCmdIdL ( alert->cmdID );
	PcdataNewL ( alert->data, KNSmlDMAgentGenericAlert );

	if ( aFwCorrelator != KNullDesC8 )
		{
		PcdataNewL ( alert->correlator, aFwCorrelator );		
		}

	alert->itemList = new ( ELeave ) SmlItemList_t;
	alert->itemList->item = new ( ELeave ) SmlItem_t;
	// URI comes without ./ from the adapter - add it now
	TPtr8 uri = HBufC8::NewLC ( aFwMgmtUri.Length() + KNSmlAgentRelativeURIPrefix().Length() )->Des();
	uri.Append ( KNSmlAgentRelativeURIPrefix );
	uri.Append ( aFwMgmtUri );
	
	DoSourceL ( alert->itemList->item->source, uri );
	CleanupStack::PopAndDestroy();  // uri
	
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC ( metInf ); 
		
	PcdataNewL ( metInf->type, aMetaType );
	PcdataNewL ( metInf->format, aMetaFormat );
	DoMetaL ( alert->itemList->item->meta, metInf );
	CleanupStack::Pop(); // metInf
	TBuf8<KNSmlMaxInt32Length> result;
	result.Num ( aFinalResult );
	PcdataNewL ( alert->itemList->item->data, result );

	TInt ret = iGenerator->smlAlertCmd ( alert );
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	CleanupStack::PopAndDestroy(); //alert
	//response item is not created, because client is not
	//interested what server responds to generic alert.
	// ...
	}

//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command.
// ---------------------------------------------------------
//
void CNSmlDMCmds::DoGenericAlertL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList ) 
	{	
	SmlAlert_t* alert = new (ELeave) SmlAlert_t; 
	CleanupStack::PushL ( alert );
	alert->elementType = SML_PE_ALERT;
	DoCmdIdL ( alert->cmdID );
	PcdataNewL ( alert->data, KNSmlDMAgentGenericAlert );

	if ( aCorrelator != KNullDesC8 )
		{
		PcdataNewL ( alert->correlator, aCorrelator );		
		}

	SmlItemList_t* itemPtr = NULL;
	TInt count = 1;
	if(&aItemList)
	    count = aItemList.Count();
	for(TInt i = 0; i<count; i++)
	{
	if( itemPtr == NULL)
	{
		alert->itemList = new ( ELeave ) SmlItemList_t;
		itemPtr = alert->itemList;		
	}
	else
	{
		itemPtr->next = new ( ELeave ) SmlItemList_t;
		itemPtr = itemPtr->next;
	}
	itemPtr->item = new ( ELeave ) SmlItem_t;
	// URI comes without ./ from the adapter - add it now
	TPtr8 uri = HBufC8::NewLC ((*aItemList[i].iSource).Length() + KNSmlAgentRelativeURIPrefix().Length())->Des();
	uri.Append ( KNSmlAgentRelativeURIPrefix );
	uri.Append ( *aItemList[i].iSource );
	
	DoSourceL ( itemPtr->item->source, uri );
	CleanupStack::PopAndDestroy();  // uri

	TPtr8 targeturi = HBufC8::NewLC ( (*aItemList[i].iTarget).Length())->Des();
	targeturi.Append ( *aItemList[i].iTarget);	
	if(targeturi.Compare(KNullDesC8)!=0)
	DoTargetL ( itemPtr->item->target, targeturi  );
	CleanupStack::PopAndDestroy();  // targeturi   
	
	SmlMetInfMetInf_t* metInf;
	DoMetInfLC ( metInf ); 
		
	PcdataNewL ( metInf->type, *aItemList[i].iMetaType );
	PcdataNewL ( metInf->format, *aItemList[i].iMetaFormat );
	PcdataNewL ( metInf->mark, *aItemList[i].iMetaMark );

	DoMetaL ( itemPtr->item->meta, metInf );
	CleanupStack::Pop(); // metInf
	
	PcdataNewL ( itemPtr->item->data, *aItemList[i].iData );

	}
	TInt ret = iGenerator->smlAlertCmd ( alert );
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	CleanupStack::PopAndDestroy(); //alert
	//response item is not created, because client is not
	//interested what server responds to generic alert.
	// ...
	}
	
// ---------------------------------------------------------
// CNSmlDMCmds::DoGenericUserAlertL()
//	Makes a generic user alert element, if the update request is set to the
//	central repository by the FOTA UI.
//	@param aProfileId. The profile id of the current dm session. 
//	@return TBool. ETrue if the alert was generated, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CNSmlDMCmds::DoGenericUserAlertL ( TInt aProfileId ) 
	{	
	// Check from the central repository if firmware update
	// is requested by the user
	TBool needsReset(EFalse);
	
	// Getting a value
	TInt readProfId ( KNSmlDmNoRequest );
	if ( iRepository )
		{
		iRepository->Get ( KDevManClientInitiatedFwUpdateId, readProfId );	
		}

	if ( readProfId != KNSmlDmNoRequest && readProfId == aProfileId )
		{
		SmlAlert_t* alert = new (ELeave) SmlAlert_t; 
		CleanupStack::PushL ( alert );
		alert->elementType = SML_PE_ALERT;
		DoCmdIdL ( alert->cmdID );
		PcdataNewL ( alert->data, KNSmlDMAgentGenericAlert );

		alert->itemList = new ( ELeave ) SmlItemList_t;
		alert->itemList->item = new ( ELeave ) SmlItem_t;
	
		SmlMetInfMetInf_t* metInf;
		DoMetInfLC ( metInf ); 
		
		PcdataNewL ( metInf->type, KNSmlDMMetaTypeUserRequest );
		PcdataNewL ( metInf->format, KNSmlDMMetaFormatUserRequest );
		DoMetaL ( alert->itemList->item->meta, metInf );
		CleanupStack::Pop(); // metInf
		
		PcdataNewL ( alert->itemList->item->data, KNullDesC8 );

		TInt ret = iGenerator->smlAlertCmd ( alert );
		if ( ret != KWBXMLGeneratorOk )
			{
			User::Leave( ret );
			}
		CleanupStack::PopAndDestroy(); //alert
		needsReset = ETrue;
		}
	return needsReset;
	}

// ---------------------------------------------------------
// CNSmlDMCmds::ResetGenericUserAlertL()
//	Resets the update request in the central repository by
//	writing '-1' value.
// ---------------------------------------------------------
//
void CNSmlDMCmds::ResetGenericUserAlertL() 
	{	
	// Reset the value in the Central Repository
	if ( iRepository )
		{
		iRepository->Set ( KDevManClientInitiatedFwUpdateId, KNSmlDmNoRequest );	
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::MarkGenAlertsSentL()
//	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
// ---------------------------------------------------------
//
void CNSmlDMCmds::MarkGenAlertsSentL() 
	{
	iDMModule->MarkGenAlertsSentL();
	}


// ---------------------------------------------------------
// CNSmlDMCmds::MarkGenAlertsSentL()
//	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
// ---------------------------------------------------------
//
void CNSmlDMCmds::MarkGenAlertsSentL(const TDesC8& aURI) 
	{
	iDMModule->MarkGenAlertsSentL(aURI);
	}
// ---------------------------------------------------------
// CNSmlDMCmds::DisconnectFromOtherServers()
//	Closes the connections to callback server and host servers.
//	Separate function is needed, since the disconnecting cannot 
//	be made after the active scheduler of the thread is stopped.
// ---------------------------------------------------------
//
void CNSmlDMCmds::DisconnectFromOtherServers() 
	{
	iDMModule->DisconnectFromOtherServers();	
	}


// FOTA end

// ---------------------------------------------------------
// CNSmlDMCmds::DoPutL()
// Empty function In DM
// ---------------------------------------------------------
void CNSmlDMCmds::DoPutL()
	{
	}

// ---------------------------------------------------------
// CNSmlDMCmds::DoResultsL()
// 
// ---------------------------------------------------------
CNSmlCmdsBase::TReturnValue CNSmlDMCmds::DoResultsL()
	{
	TBool found( ETrue );
	CNSmlCmdsBase::TReturnValue returnValue = CNSmlCmdsBase::EReturnOK;
	if ( iResultsToServer )  
		{
		iResultsToServer->Begin();
		while( found )
			{
			SmlResults_t* results;
			found = iResultsToServer->NextResultsElement( results ); 
			if ( found )
				{
				SmlPcdata_t* cmdID;
				DoCmdIdL( cmdID );
				CleanupStack::PushL( cmdID );
				iResultsToServer->SetCmdIDL( iResultsToServer->CurrentEntryID(), cmdID );
                CleanupStack::PopAndDestroy(); //cmdID;
                
                TInt dataBufferSize(0);
                
				while ( results->itemList->item->data->length < KNSmlDefaultWorkspaceSize &&  iBytesSent+dataBufferSize<iLargeObjectTotalSize)
					{
					//loop until the whole item is got from the dm module, but stop if item size is over the workspace size.
					//the while condition is false if not large object (iLargeObjectTotalSize =0)
					CBufBase *data= CBufFlat::NewL(64);
					CleanupStack::PushL(data);
					data->InsertL(0,results->itemList->item->data->content,results->itemList->item->data->length);
					TInt len = data->Size();
            		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL 1,dataLen= %d,totalSize= %d,bytesSent= %d"), len,iLargeObjectTotalSize,iBytesSent );
					iDMModule->MoreDataL(data);
					if(data->Size()==len)
						{
						CleanupStack::PopAndDestroy(); //data
						break;
						}
					delete results->itemList->item->data;
					PcdataNewL( results->itemList->item->data, data->Ptr( 0 ) );
					dataBufferSize = data->Size();
            		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL 2,dataBufferSize= %d"), dataBufferSize );
					CleanupStack::PopAndDestroy(); //data
					}
				
				if ( results->itemList->item->data->length >= KNSmlLargeObjectMinSize )
					{
					iGenerator->SetTruncate( ETrue );
					iGenerator->SetContentLength( results->itemList->item->data->length );
					}
				TInt ret = iGenerator->smlResultsCmd( results );
				switch ( ret )
					{
					case KWBXMLGeneratorOk:
						if ( iGenerator->WasTruncated() )
							{
                      		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL before subtract, length=%d"), results->itemList->item->data->length);
							SubtractConsumedFromPcData( results->itemList->item, iGenerator->DataConsumed() );
                      		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL after subtract, length=%d, dataConsumed=%d"), results->itemList->item->data->length, iGenerator->DataConsumed());
                       		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL DataConsumed, BytesSent= %d, length=%d"), iBytesSent, iGenerator->DataConsumed() );
							iBytesSent += iGenerator->DataConsumed();

							returnValue = CNSmlCmdsBase::EReturnBufferFull;
							found = EFalse;
							}
						else
							{
							iLargeObjectTotalSize = 0;
							iBytesSent = 0;
							iResultsToServer->RemoveWritten( iResultsToServer->CurrentEntryID() );
							}
						CreateAndInitResponseItemL( KNSmlAgentResults );
						break;
					case KWBXMLGeneratorBufferFull:
						returnValue = CNSmlCmdsBase::EReturnBufferFull;
						found = EFalse;
						break;
					default:	
						User::Leave( ret );
						break;
					}
           		DBG_ARGS8(_S8("CNSmlDMCmds::DoResultsL 3,BytesSent= %d"), iBytesSent );
				iGenerator->SetTruncate( EFalse );
				}
			else
				{
				iLargeObjectTotalSize = 0;
				iBytesSent = 0;
				}
			}
		}
	return returnValue;
	}
// ---------------------------------------------------------
// CNSmlDMCmds::DoGetL()
// Empty function In DM
// ---------------------------------------------------------
void CNSmlDMCmds::DoGetL()
	{
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::DoStartSyncL
// Empty function In DM 
// ---------------------------------------------------------
CNSmlCmdsBase::TReturnValue CNSmlDMCmds::DoStartSyncL() 
	{
	return CNSmlCmdsBase::EReturnOK; 
	}
// ---------------------------------------------------------
// CNSmlDMCmds::DoEndSyncL
// Empty function In DM
// ---------------------------------------------------------
void CNSmlDMCmds::DoEndSyncL()  
	{
	}
// ---------------------------------------------------------
// CNSmlDMCmds::DoAddOrReplaceOrDeleteL
// Generate Replace element, which contains DevInf 
// and calls parser to generate xml 
// ---------------------------------------------------------
CNSmlCmdsBase::TReturnValue CNSmlDMCmds::DoAddOrReplaceOrDeleteL() 
	{
	//initialise Add, Update or Delete element 
	SmlGenericCmd_t* genericCmd = new( ELeave ) SmlGenericCmd_t; 
	CleanupStack::PushL( genericCmd );
	//CmdID element
	DoCmdIdL( genericCmd->cmdID );
	
	genericCmd->itemList = new( ELeave ) SmlItemList_t;
		
	iDMDevInfo = new(ELeave) TDMDevInfo;
	iDMDevInfo->iFormat = NULL;
	iDMDevInfo->iType = NULL;
	iDMDevInfo->iObject= NULL;
	iDMDevInfo->iItemListPtr = &genericCmd->itemList;
	(*iDMDevInfo->iItemListPtr)->item = new( ELeave ) SmlItem_t;
	iDMDevInfo->iFirst = ETrue;
	iDMDevInfo->iResults = EFalse;
	iDMDevInfoResults = ETrue;
	DoDevInfoItemsL( _L8("./DevInfo") ); 
	FreeDMDevinfo();
	iDMDevInfoResults = EFalse;	
		
	genericCmd->elementType = SML_PE_REPLACE;
	TInt ret = iGenerator->smlReplaceCmd( genericCmd );
	CNSmlCmdsBase::TReturnValue returnValue = CNSmlCmdsBase::EReturnOK;
	switch ( ret )
		{
		case KWBXMLGeneratorOk:
			break;
		default:
			User::Leave( ret );
			break;
		}
	if ( ret == KWBXMLGeneratorOk )
		{
		CreateAndInitResponseItemL( KNSmlAgentReplace );
		}
	CleanupStack::PopAndDestroy();  //genericCmd 
	return returnValue;
	}


// ---------------------------------------------------------
// CNSmlDMCmds::DoMapL
// Empty function In DM 
// ---------------------------------------------------------
CNSmlDMCmds::TReturnValue CNSmlDMCmds::DoMapL()  
	{
	return EReturnOK;
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessResultsCmdL
// Empty function In DM 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessResultsCmdL( SmlResults_t* /*aResults*/ )
	{
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessPutCmd
// Empty function In DM 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessPutCmdL( SmlPut_t* /*aPut*/ )
	{
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessGetCmdL
// Handle object request from the server in Get command
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessGetCmdL( SmlGet_t* aGet )
	{
	// cmdID
	if ( aGet->cmdID->length == 0 )
		{
		StatusDataToGetCommandL( aGet, NULL, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
    TPtrC8 cmdID( (TUint8*) aGet->cmdID->content, aGet->cmdID->length );
	DBG_ARGS8(_S8("CNSmlDMCmds::ProcessGetCmdL, CmdId = %S"), &cmdID );
	
	SmlItemList_t* itemList;
	HBufC8* uri;
	itemList = aGet->itemList;
	TInt statusID;
	TInt resultsID;
	while ( itemList )
		{
		if ( iAgent->Interrupted() )
			{
			StatusDataToGetCommandL( aGet, itemList->item, TNSmlError::ESmlStatusCommandFailed );
			itemList = itemList->next;
			continue;
			}
		if ( iServerAuth->Challenged() )
			{
			StatusDataToGetCommandL( aGet, itemList->item, TNSmlError::ESmlStatusClientAuthenticationRequired );
			itemList = itemList->next;
			continue;
			}
		if ( iDMNotExecuted )
			{
			StatusDataToGetCommandL( aGet, itemList->item, TNSmlError::ESmlStatusNotExecuted  );
			itemList = itemList->next;
			continue;
			}
		// target
		if ( !TargetIsUnderItem( itemList ) )
			{
			StatusDataToGetCommandL( aGet, itemList->item, TNSmlError::ESmlStatusIncompleteCommand );
			itemList = itemList->next;
			continue;
			}
		statusID = StatusDataToGetCommandL( aGet, itemList->item, TNSmlError::ESmlStatusOK );
		resultsID = iResultsToServer->CreateNewResultsL( *iCurrServerMsgID, cmdID, itemList->item->target, itemList->item->source );
		iResultsToServer->SetStatusID( resultsID, statusID );
		
		TPtr8 pcdata( (TUint8*) itemList->item->target->locURI->content, itemList->item->target->locURI->length, itemList->item->target->locURI->length );
		TrimRightSpaceAndNull( pcdata );
		uri = pcdata.AllocLC();	
				
		iDMModule->FetchObjectL( *uri, *MetaTypeInUtf8LC( itemList->item->meta ), resultsID, statusID, EFalse );
		if ( iDMAtomic )
			{
			iStatusToServer->SetAtomicOrSequenceId( statusID, iDMAtomicID );
			}
		CleanupStack::PopAndDestroy( 2 ); // *MetaTypeLC(), uri
		itemList = itemList->next;
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::ProcessAlertCmdL
// Handles Alert command from a server.  
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessAlertCmdL( SmlAlert_t* aAlert, TBool /*aNextAlert*/, TBool /*aServerAlert*/, TBool /*aDisplayAlert*/ )
	{
	TInt statusID( 0 );
	statusID = iStatusToServer->CreateNewStatusElementL(); 
	iStatusToServer->SetCmdRefL( statusID, aAlert->cmdID );
	iStatusToServer->SetCmdL( statusID, KNSmlAgentAlert ); 
	if ( iStatusToServerNoResponse || IsFlagSet( aAlert->flags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
	
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	
	// Alert Code in Data element
	TPtr8 alertCode = AlertCode( aAlert );
	if ( alertCode.Length() == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlAlertCodeMissing, EFalse, EFalse );
		return;
		}
	if ( alertCode == KNSmlDMAgentSessionAbortAlert )
		{
		iAgent->Interrupt( TNSmlDMError::ESmlServerSessionAbort, ETrue, EFalse );
		return;
		}
	if ( iAgent->Interrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
    // cmdID
	if ( aAlert->cmdID->length == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
	if ( iServerAuth->Challenged() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusClientAuthenticationRequired );
		return;
		}
	
	if ( iDMNotExecuted )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusNotExecuted );
		return;
		}	
	

	if ( alertCode == KNSmlDMAgentDisplayAlert ||
		 alertCode == KNSmlDMAgentContinueOrAbortAlert ||
		 alertCode == KNSmlDMAgentNextMessage ||
		 alertCode == KNSmlDMAgentSessionAbortAlert ||
		 alertCode == KNSmlDMAgentServerInitAlert ||
		 alertCode == KNSmlDMAgentClientInitAlert 
		 || alertCode == KNSmlDMAgentUserInputAlert 
		 || alertCode == KNSmlDMAgentSingleChoiceAlert 
		 || alertCode == KNSmlDMAgentMultipleChoiceAlert)
		{
		HandleAlertsL(aAlert,statusID);
		}
	else
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOptFeatureNotSupported );
		return;
		}
	
	}

// ---------------------------------------------------------
// CNSmlDMCmds::HandleAlertErrorL
// Handles Error while processing Alert command from a server.  
// ---------------------------------------------------------
void CNSmlDMCmds::HandleAlertErrorL()
    {
    if ( iDMAtomic || iDMSequence )
        {
        iDMNotExecuted = ETrue;
        if (iDMAtomic )
            {
            iDMModule->RollBackL();
            iStatusToServer->SetStatusCodesInAtomicL( iDMAtomicID,    TNSmlError::ESmlStatusRollBackOK, EFalse );
            iStatusToServer->SetStatusCodeToAtomicOrSequenceCmdL( iDMAtomicID, TNSmlError::ESmlStatusNotExecuted, KNSmlAgentAtomic );
            }
        if ( iDMSequence )
            {
            iStatusToServer->SetStatusCodeToAtomicOrSequenceCmdL( iDMSequenceID, TNSmlError::ESmlStatusNotExecuted, KNSmlAgentSequence );
            }
        } 
    }

// ---------------------------------------------------------
// CNSmlDMCmds::HandleAlertsL
// Handles Alert commands from a server.  
// ---------------------------------------------------------
void CNSmlDMCmds::HandleAlertsL( SmlAlert_t* aAlert, TInt& aStatusId)
	    {
	    TPtr8 alertCode = AlertCode( aAlert );
	    if ( alertCode == KNSmlDMAgentDisplayAlert ||  alertCode == KNSmlDMAgentContinueOrAbortAlert 
	            || alertCode == KNSmlDMAgentUserInputAlert
	            || alertCode == KNSmlDMAgentSingleChoiceAlert
	            || alertCode == KNSmlDMAgentMultipleChoiceAlert )
	        {
	        if ( !aAlert->itemList )
	            {
	            iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
	            iAgent->Interrupt( TNSmlError::ESmlAlertInvalid, EFalse, EFalse );
	            return;
	            }
	        if ( !aAlert->itemList->item )
	            {
	            iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
	            iAgent->Interrupt( TNSmlError::ESmlAlertInvalid, EFalse, EFalse );
	            return;
	            }

	        if ( alertCode == KNSmlDMAgentDisplayAlert )
	            {
	            HandleDisplayAlertL(aAlert,aStatusId);
	            }
	        else if(alertCode == KNSmlDMAgentContinueOrAbortAlert)
	            {
	            HandleConfirmationAlertL(aAlert,aStatusId);
	            }
	        else if(alertCode == KNSmlDMAgentUserInputAlert)
	            {
	            HandleUserInputalertL(aAlert,aStatusId);
	            }
	        else if(alertCode == KNSmlDMAgentSingleChoiceAlert
	                || alertCode == KNSmlDMAgentMultipleChoiceAlert)
	            {	            	            
	            HandleChoiceAlertsL(aAlert,aStatusId);
	            }
	        else //
	            {

	            }
	        }
	    }

// ---------------------------------------------------------
// CNSmlDMCmds::HandleChoiceAlertsL
// Handles Choice Alert commands from a server.  
// ---------------------------------------------------------
void CNSmlDMCmds::HandleChoiceAlertsL( SmlAlert_t* aAlert, TInt& aStatusId)
    {    
    const TChar KDRSeparator('-');
    const TChar KChoiceItemSeparator(',');
    TPtr8 alertCode = AlertCode( aAlert );
    // MINDT 
    TInt mindt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMINDT );
    // MAXDT 
    TInt maxdt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXDT );
    if ( mindt > maxdt )
        {
        maxdt = mindt;
        }   
    TSyncMLDlgNotifParams notifyParams;
    TInt maxlen = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXLEN );

    //check if iDR, Default response parameter is there or not
    //DRPresent is used for destroying how many items in the pop up stack 
    TBool DRPresent = CheckDRPresence( aAlert->itemList->item->data, KNSmlDMAgentDR );
    if( DRPresent )
        {
        TPtrC8 DrPtr = AlertDefaultResponseParam( aAlert->itemList->item->data, KNSmlDMAgentDR );       
        HBufC8* hptr = HBufC8::NewLC( DrPtr.Length()+ 4 ); //cs 1dr
        TPtr8 DrBuf(hptr->Des());                           
        if(alertCode == KNSmlDMAgentMultipleChoiceAlert)
            {       
            _DBG_FILE("drbuf multichoice");                     
            DrBuf.Append(KDRSeparator);         
            DrBuf.Append(DrPtr);
            DrBuf.Append(KDRSeparator);         
            }
        else
            {           
            DrBuf.Append(DrPtr);
            }
        HBufC* DrBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( DrBuf  );
        CleanupStack::PushL( DrBuf16 ); //cs 2dr
        notifyParams.iDR = *DrBuf16;
        }
    HBufC8* alertData = AlertDataLC( aAlert->itemList ); //cs 1
    if ( alertData->Length() == 0)
        {
        if( DRPresent )
            {
            CleanupStack::PopAndDestroy(2);//alertData,hptr,DrBuf16
            }	        
        CleanupStack::PopAndDestroy();//alertData	        
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
        HandleAlertErrorL();
        return;   
        }
		
    HBufC8* alertDataWithMDT = HBufC8::NewLC(alertData->Length()+KNSmlDMAgentMINDT().Length()+KNSmlDMAgentMAXDT().Length()+6); //cs 2
    TPtr8 dataBuf = alertDataWithMDT->Des();
    dataBuf.Append(*alertData);
    HBufC* dataBuf16 = NULL;
    TRAPD(errC,  dataBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(dataBuf));
    if( errC == KErrCorrupt )
        {
        if( DRPresent )
            {
            CleanupStack::PopAndDestroy(2); //alertData hptr,DrBuf16,alertDataWithMDT
            }	        
        CleanupStack::PopAndDestroy(2); //alertData alertDataWithMDT	        
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusCommandFailed );
        return;
        }
    CleanupStack::PushL(dataBuf16);   //cs    3
    //RNotifier notifier;
    //User::LeaveIfError( notifier.Connect() );
    //CleanupClosePushL(notifier); //cs
    TPckgBuf<TBool> resBuf;    
    /* 
	//TO reduce cyclomatic complexity
	if( dataBuf16->Length() > KSyncMLMaxServerMsgLength )
        {
        notifyParams.iServerMsg = (*dataBuf16).Left(KSyncMLMaxServerMsgLength) ;    
        }
    else
        {
        notifyParams.iServerMsg = *dataBuf16;   
        }     */
	TInt datalength = FindMaxLength(dataBuf16->Length(),KSyncMLMaxServerMsgLength);	
	notifyParams.iServerMsg = (*dataBuf16).Left(datalength);  
    notifyParams.iMaxTime = maxdt;
    notifyParams.iMaxLength = maxlen;	                   
    TRequestStatus status;      
    _DBG_FILE("starting of choice alerts"); 
    //Retrieve items list 
    HBufC8* lengthbuf = HBufC8::NewLC(100); //cs 4
    TInt NumItems = 0 ;
    HBufC8* listitems = AlertChoiceItemsLC(aAlert->itemList,lengthbuf,NumItems);      //cs 5                  
	if(!NumItems)
	{
	if( DRPresent )
            {
            CleanupStack::PopAndDestroy(2); 
            }	        
        CleanupStack::PopAndDestroy(5); 	       
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
        return;
	}
    notifyParams.iNumberOfItems = NumItems;         
    HBufC* choiceitemslength = NULL;    
    TRAPD(errL,  choiceitemslength = CnvUtfConverter::ConvertToUnicodeFromUtf8L(lengthbuf->Des()));
    if( errL/* == KErrCorrupt*/ )//useful for low memory and other cases
        {
        if( DRPresent )
            {
            CleanupStack::PopAndDestroy(2); 
            }	        
        CleanupStack::PopAndDestroy(5); 	       
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusCommandFailed );
        return;
        }
    CleanupStack::PushL(choiceitemslength); //cs 6

   /* if( choiceitemslength->Length() > KSyncMLChoiceItemsLengthBuffer )
        {
        notifyParams.iItemLength = (*choiceitemslength).Left(KSyncMLChoiceItemsLengthBuffer) ;    
        }
    else
        {
        notifyParams.iItemLength = *choiceitemslength;
        }   */
   datalength = FindMaxLength(choiceitemslength->Length(),KSyncMLChoiceItemsLengthBuffer);			
   notifyParams.iItemLength = (*choiceitemslength).Left(datalength);  
    if( alertCode == KNSmlDMAgentMultipleChoiceAlert)
        {
        _DBG_FILE("multiple choice alerts");    
        notifyParams.iNoteType = ESyncMLMultiChoiceQuery;
        }
    else
        {
        _DBG_FILE("single choice alerts");  
        notifyParams.iNoteType = ESyncMLSingleChoiceQuery;
        }             
    if(iChunk.Handle())
        iChunk.Close();
    else
        {
        TTime now;
        now.HomeTime();
        TInt64 rand = now.Int64();
        // Use timestamp to get a unique seed
        TInt randnum = Math::Rand( rand );
        TBuf<KSyncMLMaxProfileNameLength> chunkname;
        chunkname.AppendNum(randnum);
        chunkname.Append(KChunkName);
        notifyParams.iChunkName = chunkname;
        TInt err1 = iChunk.CreateGlobal( chunkname,
                KNSmlDmChoiceChunkMinSize, KNSmlDmChoiceChunkMaxSize );
        DBG_FILE_CODE(err1,_S8("chunk creating error is "));
        if( err1 < 0 && err1 != KErrAlreadyExists )
            User::LeaveIfError(err1);
        if( err1 == KErrAlreadyExists )
            {    err1 = iChunk.OpenGlobal(chunkname,EFalse);
            DBG_FILE_CODE(err1,_S8("chunk opening error is "));
            User::LeaveIfError(err1);
            }
        }
    TInt size1 = iChunk.Size();
    DBG_FILE_CODE(size1,_S8("chunk size is "));

    RMemWriteStream chunkStream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL ( chunkStream ); //cs
    chunkStream.WriteL ( listitems->Des() );
    CleanupStack::PopAndDestroy(1);//chunkstream 
    TPckgBuf<TSyncMLDlgNotifParams> pkgBuf( notifyParams );                            
    TSyncMLDlgNotifReturnParams emptybuf;
    TSyncMLDlgNotifReturnParamsPckg resultBuf( emptybuf );
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );
    CleanupClosePushL(notifier); //cs  7
    _DBG_FILE("starting choice notifier");  
    notifier.StartNotifierAndGetResponse( status, KNSmlSyncDialogUid, pkgBuf, resultBuf );
    _DBG_FILE("notifier returned"); 
    User::WaitForRequest( status );
    CleanupStack::PopAndDestroy(4);//notifier,lengthbuf,listitems,choiceitemslength
    iChunk.Close();
    TBuf8<KSyncMLMaxAlertResultLength> rettext;
    rettext.Copy( resultBuf().irettext.Left( KSyncMLMaxAlertResultLength ) );
    if ( status != KErrNone )
        {
        TInt error = TNSmlError::ESmlStatusOperationCancelled ;
        iStatusToServer->SetStatusCodeL( aStatusId, error );
        HandleAlertErrorL();
        }
    else
        {
        //For sending data to server
        if( alertCode == KNSmlDMAgentMultipleChoiceAlert)
            {
            SendMultiChoiceDataToServerL(rettext,aStatusId);                   
            }
        else
            {
            SmlPcdata_t* data = NULL;
            PcdataNewL( data, rettext);
            CleanupStack::PushL( data );
            iStatusToServer->AddItemDataL( aStatusId, data );
            CleanupStack::PopAndDestroy(); //data           
            }           
        }    
    if( DRPresent ) 
        CleanupStack::PopAndDestroy(2); //hptr,DrBuf16

    CleanupStack::PopAndDestroy(3); //notifier,alertData,alertDataWithMDT,dataBuf16     
    }

// ---------------------------------------------------------
// CNSmlDMCmds::SendMultiChoiceDataToServerL
// Sends Multiple Choice Alert status to server.  
// ---------------------------------------------------------
void CNSmlDMCmds::SendMultiChoiceDataToServerL(TDes8& aData,TInt& aStatusId)
    {
    const TChar KDRSeparator('-');
    TPtrC8 temp1,temp2;
    TInt prevcommapos = 0;
    SmlPcdata_t* data = NULL;      
    for(TInt i=0;i<aData.Length();i++)
        {
        //find a slash  

        if(aData[i]== KDRSeparator)
            {
            if(prevcommapos)
                {

                temp1.Set(aData.Left(i));
                temp2.Set(temp1.Right(i-(prevcommapos+1)));
                prevcommapos=  i;
                }
            else //firsttime finding comma
                {
                prevcommapos=  i;
                temp1.Set(aData.Left(i));
                temp2.Set(temp1.Right(i));
                }
            //convert temp2 to number
            PcdataNewL( data, temp2);
            CleanupStack::PushL( data );
            iStatusToServer->AddItemDataL( aStatusId, data );
            CleanupStack::PopAndDestroy(); //data
            data = NULL;

            }
        }                           

    }

// ---------------------------------------------------------
// CNSmlDMCmds::HandleConfirmationAlertL
// Handles Confirmation Alert command from a server.  
// ---------------------------------------------------------	
void CNSmlDMCmds::HandleConfirmationAlertL( SmlAlert_t* aAlert, TInt& aStatusId)
    {    
    // MINDT 
    TInt mindt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMINDT );
    // MAXDT 
    TInt maxdt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXDT );
    if ( mindt > maxdt )
        {
        maxdt = mindt;
        }   
    TSyncMLDlgNotifParams notifyParams;
    TInt maxlen = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXLEN );                                        
    HBufC8* alertData = AlertDataLC( aAlert->itemList );
    if ( alertData->Length() == 0)
        {
        CleanupStack::PopAndDestroy();//alertData
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
        HandleAlertErrorL(); 
        return;   
        }
    HBufC8* alertDataWithMDT = HBufC8::NewLC(alertData->Length()+KNSmlDMAgentMINDT().Length()+KNSmlDMAgentMAXDT().Length()+6);
    TPtr8 dataBuf = alertDataWithMDT->Des();
    dataBuf.Append(*alertData);
    HBufC* dataBuf16 = NULL;
    TRAPD(errC,  dataBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(dataBuf));
    if( errC == KErrCorrupt )
        {
        CleanupStack::PopAndDestroy(2); //alertData alertDataWithMDT
        iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusCommandFailed );
        return;
        }
    CleanupStack::PushL(dataBuf16);    
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );
    CleanupClosePushL(notifier);
    TPckgBuf<TBool> resBuf;
    if( dataBuf16->Length() > KSyncMLMaxServerMsgLength )
        {
        notifyParams.iServerMsg = (*dataBuf16).Left(KSyncMLMaxServerMsgLength) ;    
        }
    else
        {
        notifyParams.iServerMsg = *dataBuf16;   
        }     
    notifyParams.iMaxTime = maxdt;
    notifyParams.iMaxLength = maxlen;             
    TRequestStatus status;      
    //Note type to Query note
    notifyParams.iNoteType = ESyncMLYesNoQuery;
    TPckgBuf<TSyncMLDlgNotifParams> pkgBuf( notifyParams );
    notifier.StartNotifierAndGetResponse(status,KNSmlSyncDialogUid,pkgBuf,resBuf);
    User::WaitForRequest(status);
    TBool ret = resBuf();
    if ( status == KErrCancel || status == KErrTimedOut )
        {
        TInt error = status == KErrCancel ? TNSmlError::ESmlStatusNotModified : TNSmlError::ESmlStatusRequestTimeout;
        iStatusToServer->SetStatusCodeL( aStatusId, error );
        HandleAlertErrorL();
        }
    CleanupStack::PopAndDestroy(4); //alertData alertDataWithMDT,databuf16,notifier   
    }

// ---------------------------------------------------------
// CNSmlDMCmds::HandleUserInputalertL
// Handles user Text Input Alert command from a server.  
// ---------------------------------------------------------    
 void CNSmlDMCmds::HandleUserInputalertL( SmlAlert_t* aAlert, TInt& aStatusId)
     {     
     // MINDT 
     TInt mindt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMINDT );
     // MAXDT 
     TInt maxdt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXDT );
     if ( mindt > maxdt )
         {
         maxdt = mindt;
         }   
     TSyncMLDlgNotifParams notifyParams;
     TInt maxlen = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXLEN );
     TInt InputType = AlertInputTypeParameter( aAlert->itemList->item->data, KNSmlDMAgentIT );
     TInt EchoType = AlertEchoTypeParameter( aAlert->itemList->item->data, KNSmlDMAgentET );
     //check if iDR, Default response parameter is there or not
     //DRPresent is used for destroying how many items in the pop up stack 
     TBool DRPresent = CheckDRPresence( aAlert->itemList->item->data, KNSmlDMAgentDR );
     if( DRPresent )
         {
         TPtrC8 DrPtr = AlertDefaultResponseParam( aAlert->itemList->item->data, KNSmlDMAgentDR );       
         HBufC8* hptr = HBufC8::NewLC( DrPtr.Length()+ 4 ); //cs
         TPtr8 DrBuf(hptr->Des());                           

         DrBuf.Append(DrPtr);

         HBufC* DrBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( DrBuf  );
         CleanupStack::PushL( DrBuf16 );//cs
         notifyParams.iDR = *DrBuf16;
         }
     HBufC8* alertData = AlertDataLC( aAlert->itemList );//cs
     if ( alertData->Length() == 0)
         {
         if( DRPresent )
             {
             CleanupStack::PopAndDestroy(3);//alertData,hptr,DrBuf16
             }
         else
             {
             CleanupStack::PopAndDestroy();//alertData
             }
         iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
         HandleAlertErrorL();
         return;   
         }
     HBufC8* alertDataWithMDT = HBufC8::NewLC(alertData->Length()+KNSmlDMAgentMINDT().Length()+KNSmlDMAgentMAXDT().Length()+6);//cs
     TPtr8 dataBuf = alertDataWithMDT->Des();
     dataBuf.Append(*alertData);
     HBufC* dataBuf16 = NULL;
     TRAPD(errC,  dataBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(dataBuf));
     if( errC == KErrCorrupt )
         {
         if( DRPresent )
             {
             CleanupStack::PopAndDestroy(4); //alertData hptr,DrBuf16,alertDataWithMDT
             }
         else
             {
             CleanupStack::PopAndDestroy(2); //alertData alertDataWithMDT
             }
         iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusCommandFailed );
         return;
         }
     CleanupStack::PushL(dataBuf16);    //cs
     RNotifier notifier;
     User::LeaveIfError( notifier.Connect() );
     CleanupClosePushL(notifier); //cs
     TPckgBuf<TBool> resBuf;     
     if( dataBuf16->Length() > KSyncMLMaxServerMsgLength )
         {
         notifyParams.iServerMsg = (*dataBuf16).Left(KSyncMLMaxServerMsgLength) ;    
         }
     else
         {
         notifyParams.iServerMsg = *dataBuf16;   
         }     
     notifyParams.iMaxTime = maxdt;
     notifyParams.iMaxLength = maxlen;
     notifyParams.iET = EchoType;
     notifyParams.iIT = InputType;       
     TRequestStatus status;      
     //Note type to Text input note
     notifyParams.iNoteType = ESyncMLInputQuery;
     TPckgBuf<TSyncMLDlgNotifParams> pkgBuf( notifyParams );
     TSyncMLDlgNotifReturnParams emptybuf;
     TSyncMLDlgNotifReturnParamsPckg resultBuf( emptybuf );
     notifier.StartNotifierAndGetResponse( status, KNSmlSyncDialogUid, pkgBuf, resultBuf );
     User::WaitForRequest( status );
     TBuf8<KSyncMLMaxDefaultResponseMsgLength> rettext;
     rettext.Copy( resultBuf().irettext.Left( KSyncMLMaxDefaultResponseMsgLength ) );
     if ( status == KErrCancel || status == KErrTimedOut || status ==  KErrAbort )
         {
         TInt error = TNSmlError::ESmlStatusOperationCancelled ;
         iStatusToServer->SetStatusCodeL( aStatusId, error );
         HandleAlertErrorL();
         }
     //For sending data to server    
     SmlPcdata_t* data = NULL;
     PcdataNewL( data, rettext);
     CleanupStack::PushL( data );
     iStatusToServer->AddItemDataL( aStatusId, data );
     CleanupStack::PopAndDestroy(); //data
     if( DRPresent ) 
         CleanupStack::PopAndDestroy(6); //notifier,alertData,alertDataWithMDT,dataBuf16,hptr,DrBuf16
     else
         //#endif
         CleanupStack::PopAndDestroy(4); //notifier,alertData,alertDataWithMDT,dataBuf16     
     }

 // ---------------------------------------------------------
 // CNSmlDMCmds::HandleDisplayAlertL
 // Handles Display Alert command from a server.  
 // ---------------------------------------------------------  
 void CNSmlDMCmds::HandleDisplayAlertL( SmlAlert_t* aAlert, TInt& aStatusId)
     {     
     // MINDT 
     TInt mindt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMINDT );
     // MAXDT 
     TInt maxdt = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXDT );
     if ( mindt > maxdt )
         {
         maxdt = mindt;
         }   
     TSyncMLDlgNotifParams notifyParams;
     TInt maxlen = AlertParameter( aAlert->itemList->item->data, KNSmlDMAgentMAXLEN );
     HBufC8* alertData = AlertDataLC( aAlert->itemList );
     if ( alertData->Length() == 0)
         {
         CleanupStack::PopAndDestroy();//alertData
         iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusIncompleteCommand );
         HandleAlertErrorL(); 
         return;   
         }
     HBufC8* alertDataWithMDT = HBufC8::NewLC(alertData->Length()+KNSmlDMAgentMINDT().Length()+KNSmlDMAgentMAXDT().Length()+6);
     TPtr8 dataBuf = alertDataWithMDT->Des();
     dataBuf.Append(*alertData);
     HBufC* dataBuf16 = NULL;
     TRAPD(errC,  dataBuf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(dataBuf));
     if( errC == KErrCorrupt )
         {
         CleanupStack::PopAndDestroy(2); //alertData alertDataWithMDT
         iStatusToServer->SetStatusCodeL( aStatusId, TNSmlError::ESmlStatusCommandFailed );
         return;
         }
     CleanupStack::PushL(dataBuf16);    
     RNotifier notifier;
     User::LeaveIfError( notifier.Connect() );
     CleanupClosePushL(notifier);
     TPckgBuf<TBool> resBuf;
     if( dataBuf16->Length() > KSyncMLMaxServerMsgLength )
         {
         notifyParams.iServerMsg = (*dataBuf16).Left(KSyncMLMaxServerMsgLength) ;    
         }
     else
         {
         notifyParams.iServerMsg = *dataBuf16;   
         }     
     notifyParams.iMaxTime = maxdt;
     notifyParams.iMaxLength = maxlen;     
     TRequestStatus status;              
     notifyParams.iNoteType = ESyncMLInfoNote;
     TPckgBuf<TSyncMLDlgNotifParams> pkgBuf( notifyParams );
     notifier.StartNotifierAndGetResponse(status,KNSmlSyncDialogUid,pkgBuf,resBuf);
     User::WaitForRequest(status);
     CleanupStack::PopAndDestroy(4); //alertData alertDataWithMDT,databuf16,notifier     
     }
	
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessSyncL()
// Process received Add, Replace and Delete commands
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessSyncL( SmlSync_t* /*aSync*/ )
	{
	}

// ---------------------------------------------------------
// CNSmlDMCmds::ProcessUpdatesL()
// Process received Add, Replace and Delete commands
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessUpdatesL( const TDesC8& aCmd, SmlGenericCmd_t* aContent )
	{
	TInt statusID( 0 );
	// cmdID
	if ( aContent->cmdID->length == 0 )
		{
		StatusDataToGenericCommandL( aCmd, aContent, NULL, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
	SmlItemList_t* itemList=0;
	SmlItem_t* item=0;
	HBufC8* uri;
	// FOTA
	/*
	if(aCmd==KNSmlAgentExec())
		{
		item = ((SmlExecPtr_t)aContent)->item;
		}
	else
		{
	*/
		itemList = aContent->itemList;
		item = itemList->item;
	//	}
	// FOTA end	
	
	while ( item )
		{
		TNSmlError::TNSmlSyncMLStatusCode statusCode=TNSmlError::ESmlStatusOK;
		UpdateErrorStatusCode(item,statusCode);

		if(statusCode!=TNSmlError::ESmlStatusOK)
			{
			StatusDataToGenericCommandL( aCmd, aContent, item, statusCode );
			if(itemList)
				{
				itemList = itemList->next;
				if(itemList)
					{
					item=itemList->item;
					}
				else
					{
					item=0;
					}
				}
			else
				{
				item = 0;
				}
			continue;
			}
	
		TPtr8 pcdata( (TUint8*) item->target->locURI->content, item->target->locURI->length, item->target->locURI->length );
		TrimRightSpaceAndNull( pcdata );
		uri = pcdata.AllocLC();	
		CBufBase* dataBuffer = NULL;
		
		//Get the data chunk from the servers message. The data is set to dataBuffer.
		TInt totSizeOfLarge(0);
		statusCode = GetChunkL( *uri, item, aContent->meta, dataBuffer, totSizeOfLarge );
// FOTA		
//		if ( dataBuffer )
//			{
			CleanupStack::PushL( dataBuffer );
//			}
		if ( !(statusCode == TNSmlError::ESmlStatusOK||statusCode == TNSmlError::ESmlStatusItemAccepted ))
			{
			statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode );
			
			// If this was the last chunk and there was a size mismatch, send anyway
			// a null chunk package to HostServer, so that largeObject handling session ends
			// properly.
			if ( statusCode == TNSmlError::ESmlStatusSizeMismatch )
				{
				iDMModule->UpdateObjectL ( *uri, KNullDesC8, KNullDesC8, statusID, iMoreData );					
				}
			}
// FOTA end
			
		else
			{
			if ( dataBuffer )
				{
				TPtr8 objectData( dataBuffer->Ptr(0) );
		
				HBufC8* metaType = MetaTypeInUtf8LC( item->meta );
				if ( metaType->Length() == 0 )
					{
					CleanupStack::PopAndDestroy(); // metaType
					metaType = MetaTypeInUtf8LC( aContent->meta );
					}
				if ( aCmd == KNSmlAgentAdd )
					{
					statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode );
					//Set p& S key to EAddCmd 													        			    
					RProperty::Define(KPSUidNSmlDMSyncAgent,
						KNSmlDMCmdAddOnExistingNodeorLeafKey, RProperty::EInt,
							KReadPolicy, KWritePolicy );					
					RProperty::Set(KPSUidNSmlDMSyncAgent,
							KNSmlDMCmdAddOnExistingNodeorLeafKey, EAddCmd );
					iDMModule->AddObjectL( *uri, objectData, *metaType, statusID,iMoreData ); 
					}
				else if ( aCmd == KNSmlAgentReplace )
					{
					statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode);
					// FOTA
					if ( totSizeOfLarge )
						{
						// The total size of the large object is forwarded only for FOTA adapter.
						iDMModule->UpdateObjectL( *uri, objectData, *metaType, statusID, iMoreData, totSizeOfLarge );											
						}
					else
						{
						iDMModule->UpdateObjectL( *uri, objectData, *metaType, statusID, iMoreData );					
						}
					// FOTA end
					}
				else if ( aCmd == KNSmlAgentDelete )
					{
					statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode);
					iDMModule->DeleteObjectL( *uri, statusID );
					}
				// FOTA
				/*
				if ( aCmd == KNSmlAgentExec )
					{
					statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode);
					iDMModule->ExecuteObjectL( *uri, objectData, *metaType, statusID,iMoreData  );
					}
				*/
				// FOTA end
				else if ( aCmd == KNSmlAgentCopy )
					{
					statusID = StatusDataToGenericCommandL( aCmd, aContent, item, statusCode);
					TPtr8 pcdataSourceUri( (TUint8*) item->source->locURI->content, item->source->locURI->length, item->source->locURI->length );
					TrimRightSpaceAndNull( pcdataSourceUri );

					iDMModule->CopyObjectL( *uri, pcdataSourceUri, *metaType, statusID );
					}
				// FOTA : Pop and destroy for databuffer moved forward
				CleanupStack::PopAndDestroy(); // metaType
				// FOTA end 
				if ( iDMAtomic )
					{
					iStatusToServer->SetAtomicOrSequenceId( statusID, iDMAtomicID );
					}
				}
			}
		// FOTA : Pop and destroy for databuffer added
		CleanupStack::PopAndDestroy(2); // databuffer, uri
		// FOTA end
		if(itemList)
			{
			itemList = itemList->next;
			if(itemList)
				{
				item=itemList->item;
				}
			else
				{
				item=0;
				}
			}
		else
			{
			item = 0;
			}
		}
	}
	
// FOTA
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessExecCmdL()
//	Process the exec structure sent by the remote server.
//	@param aExec. The exec command structure, accordant with the dtd. 
// ---------------------------------------------------------
//
void CNSmlDMCmds::ProcessExecCmdL ( SmlExec_t* aExec )
	{
	if ( aExec->cmdID->length == 0 )
		{
		StatusDataToCommandL ( KNSmlAgentExec, aExec->cmdID, aExec->flags, NULL, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt ( TNSmlError::ESmlCmdIDMissing, EFalse, EFalse );
		return;
		}
	TNSmlError::TNSmlSyncMLStatusCode statusCode ( TNSmlError::ESmlStatusOK );
		
	if ( iAgent->Interrupted() )
		{
		statusCode = TNSmlError::ESmlStatusCommandFailed;
		}
	else if ( iServerAuth->Challenged() )
		{
		statusCode = TNSmlError::ESmlStatusClientAuthenticationRequired;
		}
	else if ( iDMNotExecuted )
		{
		statusCode = TNSmlError::ESmlStatusNotExecuted;
		}
	else if ( !aExec->item->target || !aExec->item->target->locURI )
		{
		statusCode = TNSmlError::ESmlStatusIncompleteCommand;
		}

	if ( statusCode != TNSmlError::ESmlStatusOK )
		{
		StatusDataToCommandL ( KNSmlAgentExec, aExec->cmdID, aExec->flags, aExec->item, statusCode );
		return;
		}
	
	TPtr8 pcdata ( (TUint8*) aExec->item->target->locURI->content, aExec->item->target->locURI->length, aExec->item->target->locURI->length );
	TrimRightSpaceAndNull ( pcdata );
	HBufC8* uri = pcdata.AllocLC();	
	CBufBase* dataBuffer = NULL;
		
	//Get the data chunk from the servers message. The data is set to dataBuffer.
	TInt dummy(0);
	statusCode = GetChunkL ( *uri, aExec->item, aExec->meta, dataBuffer, dummy );
	CleanupStack::PushL( dataBuffer );
	
	if ( !( statusCode == TNSmlError::ESmlStatusOK || statusCode == TNSmlError::ESmlStatusItemAccepted ) )
		{
		StatusDataToCommandL ( KNSmlAgentExec, aExec->cmdID, aExec->flags, aExec->item, statusCode );
		}
	else
		{
		if ( dataBuffer )
			{
			HBufC8* metaType = MetaTypeInUtf8LC( aExec->item->meta );
			if ( metaType->Length() == 0 )
				{
				CleanupStack::PopAndDestroy(); // metaType
				metaType = MetaTypeInUtf8LC ( aExec->meta );
				}
			TInt statusID ( 0 );
			statusID = StatusDataToCommandL ( KNSmlAgentExec, aExec->cmdID, aExec->flags, aExec->item, statusCode );
			if ( aExec->correlator )
				{
				TPtr8 correlator ( (TUint8*) aExec->correlator->content, aExec->correlator->length, aExec->correlator->length );
				TrimRightSpaceAndNull ( correlator );
				HBufC8* execCorrelator = correlator.AllocLC();	
				iDMModule->ExecuteObjectL ( *uri, dataBuffer->Ptr(0), *metaType, statusID, *execCorrelator, iMoreData );
				CleanupStack::PopAndDestroy();  // execCorrelator
				}
			else
				{
				iDMModule->ExecuteObjectL ( *uri, dataBuffer->Ptr(0), *metaType, statusID, KNullDesC8(), iMoreData );				
				}
			CleanupStack::PopAndDestroy(); // metaType
			if ( iDMAtomic )
				{
				iStatusToServer->SetAtomicOrSequenceId ( statusID, iDMAtomicID );
				}
			}
		}
	CleanupStack::PopAndDestroy(2); // databuffer, uri
	}

// FOTA end
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessAtomicL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessAtomicL( SmlAtomic_t* aAtomic )
	{
	TInt statusID = InitStatusToAtomicOrSequenceL( KNSmlAgentAtomic, aAtomic );
	if ( iAgent->Interrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
	if ( iServerAuth->Challenged() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusClientAuthenticationRequired );
		return;
		}
	iDMAtomicID++;
	iStatusToServer->SetAtomicOrSequenceId( statusID, iDMAtomicID );
	iDMModule->StartTransactionL();
	iDMAtomic = ETrue;
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessEndAtomicL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessEndAtomicL()
	{
	if ( (!iAgent->Interrupted()) && ( !iServerAuth->Challenged() ) )
		{
		iDMModule->CommitTransactionL();
		}
	iDMAtomic = EFalse;
	if ( !iDMSequence )
		{
		iDMNotExecuted = EFalse;
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::ProcessSequenceL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessSequenceL( SmlSequence_t* aSequence )
	{
	TInt statusID = InitStatusToAtomicOrSequenceL( KNSmlAgentSequence, aSequence );
	if ( iAgent->Interrupted() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
		return;
		}
	if ( iServerAuth->Challenged() )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusClientAuthenticationRequired );
		return;
		}
	if ( iDMNotExecuted )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusNotExecuted );
		return;
		}
	iDMSequenceID++;
	iStatusToServer->SetAtomicOrSequenceId( statusID, iDMSequenceID );
	iDMSequence = ETrue;
	}
// ---------------------------------------------------------
// CNSmlDMCmds::ProcessEndSequence()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessEndSequence()
	{
	iDMSequence = EFalse;
	if ( !iDMAtomic )
		{
		iDMNotExecuted = EFalse;
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::ProcessEndSync()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::ProcessEndSyncL()
	{
	}

// ---------------------------------------------------------
// CNSmlDMCmds::DoEndMessageL
// Generates end tag of SyncML element and possble final flag in SyncBody 
// and calls Ref.Toolkit parser to generate xml 
// ---------------------------------------------------------
void CNSmlDMCmds::DoEndMessageL( TBool aFinal )  
	{
	TInt ret;
	if ( aFinal )
		{
		ret = iGenerator->smlEndMessage( ETrue );
		}
	else
		{
		ret = iGenerator->smlEndMessage( EFalse );
		}
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::EndOfServerMessageL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::EndOfServerMessageL() const
	{
	iDMModule->EndMessageL();
	}



// ---------------------------------------------------------
// CNSmlDMCmds::SetResultsL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::SetResultsL( const TInt aResultsRef, const CBufBase& aObject, const TDesC8& aType, const TDesC8& aFormat,TInt aTotalSize )
	{
	if ( !iDMDevInfoResults )
		{
		if ( iServerMaxObjectSize > 0 && aObject.Size() > iServerMaxObjectSize )
			{
			iLargeObjectTotalSize = 0;
			iBytesSent=0;
			iStatusToServer->SetStatusCodeL( iResultsToServer->StatusID( aResultsRef ), TNSmlError::TNSmlSyncMLStatusCode( TNSmlError::ESmlStatusEntityTooLarge ), ETrue );
			}
		else
			{
			iBytesSent=0;
			iLargeObjectTotalSize = aTotalSize;
			iResultsToServer->AddItemL( aResultsRef, aObject, aType, aFormat );
			}
		}
	else
		{
		iDMDevInfo->iResults = ETrue;
		delete iDMDevInfo->iFormat;
		iDMDevInfo->iFormat = NULL;
		delete iDMDevInfo->iType;
		iDMDevInfo->iType = NULL;
		delete iDMDevInfo->iObject;
		iDMDevInfo->iObject = NULL;
		iDMDevInfo->iFormat = aFormat.AllocL();
		iDMDevInfo->iType = aType.AllocL();
		iDMDevInfo->iObject = CONST_CAST( CBufBase&, aObject ).Ptr(0).AllocL();
		}
	}

// ---------------------------------------------------------
// CNSmlDMCmds::SetStatusL()
// 
// ---------------------------------------------------------
void CNSmlDMCmds::SetStatusL( TInt aStatusRef, TInt aStatusCode )
	{
	if ( !iDMDevInfoResults )
		{
		iStatusToServer->SetStatusCodeL( aStatusRef, aStatusCode );
		/*if ( iDMSequence )
			{
			if ( aStatusCode >= 300 || aStatusCode < 200 ) 
				{
				iDMNotExecuted = ETrue;
				}
			}*/
		if ( iDMAtomic ) 
			{
			if ( aStatusCode >= 300 || aStatusCode < 200 ) 
				{
				iStatusToServer->SetStatusCodeToAtomicOrSequenceCmdL( iDMAtomicID, TNSmlError::ESmlStatusAtomicFailed, KNSmlAgentAtomic );
				}
			}
		}
	if ( ( aStatusCode == TNSmlError::TNSmlSyncMLStatusCode( TNSmlError::ESmlStatusDeviceFull ) ) &&
		 (!iDMDeviceFullWritten ) )
		{
		iAgent->WriteWarningL( -1, TNSmlError::ESmlStatusDeviceFull );
		iDMDeviceFullWritten = ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMCmds::GetChunkL
// Buffer the received item and handles large objects.
// -----------------------------------------------------------------------------
//
TNSmlError::TNSmlSyncMLStatusCode CNSmlDMCmds::GetChunkL(TDesC8& aLargeUri,
	const SmlItem_t* aCurrentItem, 
	const SmlPcdata_t* aMetaInCommand, 
	CBufBase*& aBuffer,
	// FOTA
	TInt& aTotSizeOfLarge 
	// FOTA end
	 )
	{
	iItemSizeInStream = 0;
	TNSmlError::TNSmlSyncMLStatusCode status( TNSmlError::ESmlStatusOK );
	iMoreData = IsFlagSet( aCurrentItem->flags, SmlMoreData_f );
	
	// check that the previously received chunk belongs to the same item
	if ( iAmountReceived != 0 )
		{
		if (iLargeObjectUri&&iLargeObjectUri->Compare(aLargeUri) )
			{
			iAgent->SetEndOfDataAlertRequest();
			delete iRecBuf;
			iRecBuf = NULL;
			delete iLargeObjectUri;
			iLargeObjectUri=NULL;
			iServerLargeObjectSize = 0;
			}
		}
		
	if ( iMoreData && ( iAmountReceived == 0 ) )
		{
		iServerLargeObjectSize = ServerObjectSize( aCurrentItem->meta );
		
		if ( iServerLargeObjectSize == 0 )
			{
			iServerLargeObjectSize = ServerObjectSize( aMetaInCommand );
			}
		// FOTA
		if ( iServerLargeObjectSize == 0 )
			{
			status = TNSmlError::ESmlStatusSizeRequired;			
			}
		// FOTA end
		iItemSizeInStream = iServerLargeObjectSize;	
		// FOTA
		aTotSizeOfLarge = iServerLargeObjectSize;
		// FOTA end
		delete iLargeObjectUri;
		iLargeObjectUri=NULL;
		iLargeObjectUri = aLargeUri.AllocL();
		}
		
	iRecBuf = CBufFlat::NewL( 1 );
	
	// buffer data
	if ( aCurrentItem->data )
		{
		if ( iItemSizeInStream == 0 )
			{
			iItemSizeInStream = aCurrentItem->data->length;
			}
			
		if ( aCurrentItem->data->content )
			{
			TPtr8 data( static_cast<TUint8*>( aCurrentItem->data->content ), aCurrentItem->data->length, aCurrentItem->data->length );
			iRecBuf->InsertL( iRecBuf->Size(), data );
			}
		}
	else
		{
		aBuffer = iRecBuf;
		iRecBuf = NULL;
		return status;
		}
		
	// last / only chunk
	if ( !iMoreData )
		{
		TBool notFirst( iAmountReceived != 0 );
		iAmountReceived += aCurrentItem->data->length;
		
		if ( notFirst && ( iAmountReceived != iServerLargeObjectSize ) )
			{
			status = TNSmlError::ESmlStatusSizeMismatch;
			delete iRecBuf;
			iRecBuf = NULL;
			}

		iAmountReceived = 0;
		}
	// middle chunk
	else
		{
		iAmountReceived += aCurrentItem->data->length;
		// FOTA 
		if ( status == TNSmlError::ESmlStatusOK )
			{
			status = TNSmlError::ESmlStatusItemAccepted;		
			}
		// FOTA end
		}
		
	aBuffer = iRecBuf;
	iRecBuf = NULL;
	
	return status;
	}

// ---------------------------------------------------------
// CNSmlDMCmds::UpdateErrorStatusCode()
//  Updates the status code with proper error
// ---------------------------------------------------------------------------------------------------------------
void CNSmlDMCmds::UpdateErrorStatusCode(SmlItem_t* aItem,
		TNSmlError::TNSmlSyncMLStatusCode& aStatusCode )
	{
	if ( iAgent->Interrupted() )
		{
		aStatusCode = TNSmlError::ESmlStatusCommandFailed;
		}
	if ( iServerAuth->Challenged() )
		{
		aStatusCode = TNSmlError::ESmlStatusClientAuthenticationRequired;
		}
	if ( iDMNotExecuted )
		{
		aStatusCode = TNSmlError::ESmlStatusNotExecuted;
		}
	// target
	if ( !aItem->target||!aItem->target->locURI )
		{
		aStatusCode = TNSmlError::ESmlStatusIncompleteCommand;
		}
	}

// ---------------------------------------------------------------------------------------------------------------
// CNSmlDMCmds::AlertChoiceItemsLC()
//  Retrieves the choice alerts information 
// ---------------------------------------------------------------------------------------------------------------
HBufC8* CNSmlDMCmds::AlertChoiceItemsLC(SmlItemList_t* aItemList,HBufC8*& aLengthBuf ,TInt& aNumItems ) const
    {
    /*cleanupstack contains
     * top : tempdata
     *      : unicodeData
     * bottom: aLengthBuf
     */
	 const TChar KDRSeparator('-');
const TChar KChoiceItemSeparator(',');
    HBufC8* unicodeData = HBufC8::NewLC(200);
    HBufC8* tempdata = NULL;
    HBufC8* lengthbuf = aLengthBuf;
    TBuf<20> lennum; 
    SmlItemList_t* temp;
    if (  aItemList )//Not null
        {
        if(aItemList->next)//removing header of dialog
            {
            temp = aItemList->next;
            while(temp->next)
                {
                if (  temp->next->item )
                    {
                    if (  temp->next->item->data )
                        {
                        if (  temp->next->item->data->content )
                            {
                            TPtr8 data( (TUint8*)  temp->next->item->data->content,  temp->next->item->data->length, temp->next->item->data->length );
                            TrimRightSpaceAndNull( data );
                            tempdata = data.AllocLC();
                            TInt length = data.Length();//item length
                            //add length to buffer and add comma 
                            if(lengthbuf->Des().MaxLength() > (lengthbuf->Des().Length() + 5/*bytes*/) )
                                {
                                if(length > KSyncMLMaxChoiceItemLength ) // choice item length restricting to 200
                                    {
                                    lennum.Num(KSyncMLMaxChoiceItemLength);
                                    lengthbuf->Des().Append(lennum);
                                    lengthbuf->Des().Append(KChoiceItemSeparator);                                    
                                    }
                                else
                                    {
                                    lennum.Num(length);                            
                                    lengthbuf->Des().Append(lennum);
                                    lengthbuf->Des().Append(KChoiceItemSeparator);
                                    }
                                }
                            else
                                {
                                //Pop the aLengthBuf from cleanupstack and realloc
                                CleanupStack::Pop(3); //tempdata,unicodeData,aLengthBuf
                                lengthbuf = lengthbuf->ReAllocL(lengthbuf->Des().Length() + 100);
                                CleanupStack::PushL(lengthbuf);
                                CleanupStack::PushL(unicodeData);
                                CleanupStack::PushL(tempdata);
                                if(length > KSyncMLMaxChoiceItemLength ) // choice item length restricting to 200
                                    {
                                    lengthbuf->Des().AppendNum(KSyncMLMaxChoiceItemLength);
                                    lengthbuf->Des().Append(KChoiceItemSeparator);  
                                    }
                                else
                                    {
                                    lengthbuf->Des().AppendNum(length); //Append(length);
                                    lengthbuf->Des().Append(KChoiceItemSeparator);
                                    }
                                }
                            if(unicodeData->Des().MaxLength()> (unicodeData->Des().Length() + length))
                                {
                                if(length > KSyncMLMaxChoiceItemLength ) // choice item length restricting to 200
                                    {
                                    unicodeData->Des().Append(tempdata->Des().Left(KSyncMLMaxChoiceItemLength)); 
                                    }
                                else
                                    {
                                    unicodeData->Des().Append(tempdata->Des());
                                    }
                                }
                            else
                                {
                                CleanupStack::Pop(2);//tempdata,unicodeData
                                unicodeData = unicodeData->ReAllocL(unicodeData->Des().Length() + length + 200);
                                CleanupStack::PushL(unicodeData);
                                CleanupStack::PushL(tempdata);
                                if(length > KSyncMLMaxChoiceItemLength ) // choice item length restricting to 200
                                    {
                                    unicodeData->Des().Append(tempdata->Des().Left(KSyncMLMaxChoiceItemLength)); 
                                    }
                                else
                                    {
                                    unicodeData->Des().Append(tempdata->Des());
                                    }
                                }
                            CleanupStack::PopAndDestroy(tempdata);
                            aNumItems++;
                            data.Zero();
                            }
                        }
                    }
                temp = temp->next;
                }
            }
        }  
        _DBG_FILE("CNSmlDMCmds::AlertDataLC: end"); 
        aLengthBuf = lengthbuf;
        return unicodeData;          
    }
	
// ---------------------------------------------------------------------------------------------------------------
// CNSmlDMCmds::FindMaxLength()
// ---------------------------------------------------------------------------------------------------------------	
TInt CNSmlDMCmds::FindMaxLength(TInt aSourceLength, TInt aDestLength)
	{
	TInt length = 0;
	if(aSourceLength > aDestLength )
	 {
	  length = aDestLength;
	 }
	else
	 {
	  length = aSourceLength;	  
	 }
	return length;
	}
