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
* Description:  Common Agent SyncML command handling
*
*/


// INCLUDE FILES

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldefines.h>
#include <nsmlphoneinfo.h>
#include <nsmlunicodeconverter.h>
#include "nsmlcliagconstants.h"
#include "NSmlCmdsBase.h"
#include "NSmlAgentBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResponseController.h"
#include "NSmlResultsContainer.h"
#include "NSmlURI.h"
#include "NSmlAuth.h"
#include "WBXMLSyncMLGenerator.h"
#include "WBXMLSyncMLParser.h"
#include "WBXMLGeneratorError.h"
#include "WBXMLParser.h"
#include "smldtd.h"


// ---------------------------------------------------------
// CNSmlCmdsBase::CNSmlCmdsBase()
// Constructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlCmdsBase::CNSmlCmdsBase()
    {
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::~CNSmlCmdsBase()
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlCmdsBase::~CNSmlCmdsBase()
    {
//1.2 CHANGES: 1.1 and 1.2 version support
	delete iVerDTD;
//end changes
	delete iVerProto;
	delete iSentSyncHdrTarget;
	delete iSentSyncHdrSource;
	delete iRespURI;
	delete iStatusToServer;
	delete iResponseController;
	delete iResultsToServer;
	delete iGenerator;
	delete iParser;
	delete iDocHandler;
	delete iPhoneInfo;
	delete iServerAuth;
	delete iSessionID;
	delete iSyncMLUserName;
	delete iSyncServer;
	delete iLargeObjectItem;
	delete iRecBuf;
	delete iLargeObjectUID;
	delete iCurrServerMsgID;
	delete iBufferArea;
	iReadStream.Close();
	}


// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessCopyCmdL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessCopyCmdL(SmlCopy_t* /*aCopy*/)
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessExecCmdL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessExecCmdL(SmlExec_t* /*aExec*/)
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessMapCmdL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessMapCmdL(SmlMap_t* /*aContent*/)
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessSearchCmdL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessSearchCmdL(SmlSearch_t* /*aSearch*/)
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::FetchIDMappingTableL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::FetchIDMappingTableL(const TInt /*aType*/)
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::CloseIDMappingTableL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::CloseIDMappingTableL()
	{
	//for future use
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DeleteIDMappingTableL
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::DeleteIDMappingTableL(const TDesC& /*aId*/, const TDesC& /*aDatabase*/)
	{
	//for future use
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::EndOfServerMessageL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::EndOfServerMessageL() const
	{
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::WriteMappingInfoToDbL
// Empty default implementation.
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::WriteMappingInfoToDbL() const
	{
	// Defined only in DM agent
	return KErrNone;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoSyncHdrL
// Makes SyncHdr element and calls parser to generate xml
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoSyncHdrL() 
	{
	// create generator instance first
	if ( !iGenerator )
		{
		iGenerator = new( ELeave ) CWBXMLSyncMLGenerator( iVersionPublicId );
		}
	// create workspace
	iGenerator->CreateWorkspaceL( iWorkspaceSize );
	//initialise SyncHdr
	SmlSyncHdr_t* syncHdr = new( ELeave ) SmlSyncHdr_t; 
	CleanupStack::PushL( syncHdr );
	//element type
	syncHdr->elementType = SML_PE_HEADER;
	//VerDTD element  
	
//1.2 CHANGES: 1.2 and 1.1 version support
	PcdataNewL ( syncHdr->version, *iVerDTD );
//changes end
	
	//VerProto element  
	PcdataNewL ( syncHdr->proto, *iVerProto );
	//sessionID element
	PcdataNewL ( syncHdr->sessionID, *iSessionID );
	//msgID element
	TBuf8<16> stringMsgID; 
	stringMsgID.Num( ++iCurrMsgID );
	PcdataNewL( syncHdr->msgID, stringMsgID );
	//Target element
	delete iSentSyncHdrTarget;
	iSentSyncHdrTarget = NULL;
	if ( iRespURI )
		{
		// Target changes if a server has issued RespURI in previous message
		DoTargetL( syncHdr->target, iRespURI->HostName() );
		iSentSyncHdrTarget = CNSmlURI::NewL( iRespURI->HostName() );
		}
	else
		{
		DoTargetL( syncHdr->target, iSyncServer->HostName() ); 
		iSentSyncHdrTarget = CNSmlURI::NewL( iSyncServer->HostName() );
		}
	//Source element
	HBufC* imeiCode = HBufC::NewLC( 50 );
	TPtr imeiCodePtr = imeiCode->Des();  
	iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, imeiCodePtr );
	CNSmlAgentBase::TAuthenticationType authType;
	authType = iAgent->AuthTypeL();
	if ( ( !iAlreadyAuthenticated ) &&  ( authType == CNSmlAgentBase::EMD5 ) )
		{
		DoSourceL( syncHdr->source, imeiCodePtr, *iSyncMLUserName);
		}
	else
		{
		DoSourceL( syncHdr->source, imeiCodePtr ); 
		}
	delete iSentSyncHdrSource;
	iSentSyncHdrSource = NULL;
	iSentSyncHdrSource = CNSmlURI::NewL( *imeiCode );
	CleanupStack::PopAndDestroy(); //imeiCode
	// Cred element 
	if ( authType == CNSmlAgentBase::ENoAuth || iAgent->AlreadyAuthenticated() )
		{
		syncHdr->cred = NULL;
		}
	else
		{
		syncHdr->cred = new( ELeave ) SmlCred_t;
		SmlMetInfMetInf_t* metInf;
		DoMetInfLC( metInf ); 
		if ( authType == CNSmlAgentBase::EMD5 )
			{
			PcdataNewL( metInf->type, KNSmlAgentAuthMD5 );
			PcdataNewL( metInf->format, KNSmlAgentBase64Format );
			}
		else  // Basic
			{
			PcdataNewL( metInf->type, KNSmlAgentAuthBasic );
			}
		DoMetaL( syncHdr->cred->meta, metInf );
		CleanupStack::Pop(); //metInf

		HBufC8* credential;
		if ( authType == CNSmlAgentBase::EMD5 )
			{
			credential = iAgent->Md5CredentialL();
			}
		else // Basic
			{
			credential = iAgent->BasicCredentialL();
			}
		CleanupStack::PushL( credential ); 
		PcdataNewL ( syncHdr->cred->data, *credential );
		CleanupStack::PopAndDestroy(); //credential
		}
	// MaxMsgSize
	SmlMetInfMetInf_t* maxMsgMetInf;
	DoMetInfLC( maxMsgMetInf );
	TBuf<16> maxMsgSize;
	maxMsgSize.Num( iWorkspaceSize );
	HBufC8* maxMsgSizeInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( maxMsgSize, maxMsgSizeInUTF8 );
	PcdataNewL( maxMsgMetInf->maxmsgsize, *maxMsgSizeInUTF8 );
	CleanupStack::PopAndDestroy(); //maxMsgSizeInUTF8
	if ( iSynchHdrMaxObjSize > 0 )
		{
		DoMaxObjSizeL( *maxMsgMetInf, iSynchHdrMaxObjSize );
		}
	DoMetaL( syncHdr->meta, maxMsgMetInf );
	CleanupStack::Pop(); //maxMsgMetInf
	TInt ret;
	ret = iGenerator->smlStartMessage( syncHdr );
	if ( ret != KWBXMLGeneratorOk )
		{
		User::Leave( ret );
		}
	CleanupStack::PopAndDestroy(); //syncHdr
	iCurrCmdID = 0;
	//save Status response information for response status controlling
	CreateAndInitResponseItemL( KNSmlAgentSyncHdr );
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoStatusL
// Generate Status element and calls Ref.Toolkit parser to generate xml 
// ---------------------------------------------------------
EXPORT_C CNSmlCmdsBase::TReturnValue CNSmlCmdsBase::DoStatusL( SmlStatus_t* aStatus ) 
	{
	TReturnValue returnValue = EReturnOK;
	//CmdID element
	SmlPcdata_t* cmdID = NULL;
	DoCmdIdL( cmdID );
	CleanupStack::PushL( cmdID );
	TInt entryID = iStatusToServer->CurrentEntryID();
	iStatusToServer->SetCmdIDL( entryID, cmdID );
	CleanupStack::PopAndDestroy(); //cmdID
	// call SyncML generator
	TInt ret = iGenerator->smlStatusCmd( aStatus );
	switch ( ret )
		{
		case KWBXMLGeneratorOk:
			iStatusToServer->RemoveWritten( entryID );
			break;
		case KWBXMLGeneratorBufferFull:
			returnValue = CNSmlCmdsBase::EReturnBufferFull;
			break;
		default:
			User::Leave( ret );
		}
	return returnValue;
	}

// FOTA
// ---------------------------------------------------------
// CNSmlCmdsBase::DoGenericAlertL
// Makes generic alert. 
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoGenericAlertL (  const TDesC8& /*aFwMgmtUri*/, const TDesC8& /*aMetaType*/, 
									const TDesC8& /*aMetaFormat*/, TInt /*aFinalResult*/, const TDesC8& /*aFwCorrelator*/ ) 
	{
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoGenericAlertL
// Makes generic alert. 
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoGenericAlertL (  const TDesC8& /*aCorrelator*/, const RArray<CNSmlDMAlertItem>& /*aData*/ ) 
	{
	}
	
// ---------------------------------------------------------
// CNSmlCmdsBase::DoGenericUserAlertL
// Makes generic user alert. 
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::DoGenericUserAlertL ( TInt /*aProfileId*/ ) 
	{
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ResetGenericUserAlertL
// Resets a user request for fw update in the central repository.
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ResetGenericUserAlertL() 
	{
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::MarkGenAlertsSentL
// When the generic alerts are successfully sent to the remote 
// server, the FOTA adapter needs to be informed about this.
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::MarkGenAlertsSentL() 
	{
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::MarkGenAlertsSentL(const TDesC8)
// When the generic alerts are successfully sent to the remote 
// server, the respective adapter needs to be informed about this.
// URI will have adapter's root node.
// This is needed only in Device Management.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::MarkGenAlertsSentL(const TDesC8 &aURI) 
    {
    
    }
// ---------------------------------------------------------
// CNSmlCmdsBase::DisconnectFromOtherServers
// Closes the connections to callback server and host servers.
// This is needed only in Device Management, since the 
// disconnecting cannot be made after the active scheduler
// of the thread is stopped.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DisconnectFromOtherServers() 
	{
	}

// FOTA end

// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessReceivedDataL
// Parse next received SyncML command using Ref.Toolkit parser
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::ProcessReceivedDataL()
	{
	if ( !iDocHandler )
		{
		iDocHandler = CWBXMLSyncMLDocHandler::NewL( iAgent );
		}
	if ( !iParser )
		{
		iParser = CWBXMLParser::NewL();
		iParser->SetDocumentHandler( iDocHandler );
		iParser->SetExtensionHandler( iDocHandler );
		if ( !iBufferArea )
			{
			iBufferArea = HBufC8::NewL( iWorkspaceSize );
			}
		iReadStream.Open( *iBufferArea ); 
		iParser->SetDocumentL( iReadStream );
		}
		
	TWBXMLParserError ret = KWBXMLParserErrorOk;
	iAgent->SetOffCallbackCalled();
	while ( ret == KWBXMLParserErrorOk && !iAgent->CallbackCalled() )
		{
		ret = iParser->ParseL();
		}
	if ( ret == KWBXMLParserErrorOk )   
		{
		return ETrue;
		}
	else
		{
		delete iBufferArea;
		iBufferArea = NULL;
		delete iParser;
		iParser = NULL;
		if ( ret == KWBXMLParserErrorEof )
			{
			return EFalse;
			}
		else
			{
			User::Leave( ret );
			return EFalse;   // this sentence ignores compiler warning
			}
		}
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessSyncHdrL
// Process received SyncHdr element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessSyncHdrL( SmlSyncHdr_t* aSyncHdr )
	{
	if ( !aSyncHdr ) 
		{
		User::Leave( TNSmlError::ESmlSyncHdrMissing );
		}
	// Status response flag 
	if ( IsFlagSet( aSyncHdr->flags, SmlNoResp_f ) )
		{
		iStatusToServerNoResponse = ETrue;
		}
	else
		{
		iStatusToServerNoResponse = EFalse;
		}
	// build corresponding status element to server
	if ( !iStatusToServer )
		{
		iStatusToServer = CNSmlStatusContainer::NewL();
		}
	iStatusToServer->InitMsgRefL( aSyncHdr->msgID );
	delete iCurrServerMsgID;
	iCurrServerMsgID = NULL;
	iCurrServerMsgID = HBufC8::NewL(0);

	TInt statusID;
	statusID = iStatusToServer->CreateNewStatusElementL(); 
	HBufC8* cmdRef = HBufC8::NewLC( KNSmlAgentSyncHdrCmdID.iTypeLength );	
	*cmdRef = KNSmlAgentSyncHdrCmdID;
	SmlPcdata_t*  cmdRefElement;
	PcdataNewL ( cmdRefElement, *cmdRef );
	CleanupStack::PushL( cmdRefElement );
	iStatusToServer->SetCmdRefL( statusID, cmdRefElement );
	CleanupStack::PopAndDestroy( 2 ); //cmdRefElement, cmdRef
	iStatusToServer->SetCmdL( statusID, KNSmlAgentSyncHdr ); 
	iStatusToServer->SetNoResponse( statusID, IsFlagSet( aSyncHdr->flags, SmlNoResp_f ) );
	iStatusToServer->AddTargetRefL( statusID, aSyncHdr->target );
	iStatusToServer->AddSourceRefL( statusID, aSyncHdr->source );
	iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOK );
	//respURI
	if ( aSyncHdr->respURI )
		{
		HBufC* respURI;
		PCDATAToUnicodeLC( *aSyncHdr->respURI, respURI );
		respURI->Des().TrimRight();
		if ( respURI->Length() > 0 )
			{
			delete iRespURI;
			iRespURI = NULL;
			iRespURI = CNSmlURI::NewL( *respURI );
			}
		CleanupStack::PopAndDestroy(); //respURI
		}
	// verDTD and verProto (Check only once) 
	// sync is interrupted if version is other than 1.1 
	if ( !iVersionCheck )	
		{
		iVersionIsChecked = ETrue;
		}
	if ( !iVersionIsChecked )
		{
		if ( !aSyncHdr->version->content )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusVersionNotSupported );
			iAgent->Interrupt( TNSmlError::ESmlVerDTDMissing, EFalse, EFalse );
			return;
			}
		TPtr8 verDTD( (TUint8*) aSyncHdr->version->content, aSyncHdr->version->length, aSyncHdr->version->length );
		TrimRightSpaceAndNull( verDTD );
//1.2 CHANGES: 1.1 and 1.2 version support
		if ( ( verDTD != KNSmlAgentVerDTD11 ) && (verDTD != KNSmlAgentVerDTD12 ))
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusVersionNotSupported );
			SmlPcdata_t* data = NULL;

			PcdataNewL( data, *iVerDTD );
//changes end			
			CleanupStack::PushL( data );
			iStatusToServer->AddItemDataL( statusID, data );
			CleanupStack::PopAndDestroy(); //data
			iAgent->Interrupt( TNSmlError::ESmlVerDTDNotSupported, EFalse, EFalse );
			return;
			}
		if ( !aSyncHdr->proto->content )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusProtocolVersionNotSupported );
			iAgent->Interrupt( TNSmlError::ESmlVerProtoMissing, EFalse, EFalse );
			return;
			}
		TPtr8 verProto( (TUint8*) aSyncHdr->proto->content, aSyncHdr->proto->length, aSyncHdr->proto->length );
		TrimRightSpaceAndNull( verProto );
		if ( verProto != *iVerProto )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusProtocolVersionNotSupported );
			SmlPcdata_t* data = NULL;
			PcdataNewL( data, *iVerProto );
			CleanupStack::PushL( data );
			iStatusToServer->AddItemDataL( statusID, data );
			CleanupStack::PopAndDestroy(); //data
			iAgent->Interrupt( TNSmlError::ESmlVerProtoNotSupported, EFalse, EFalse );
			return;
			}
		iVersionIsChecked = ETrue;
		}
	// msgID
	TPtr8 msgID( (TUint8*) aSyncHdr->msgID->content, aSyncHdr->msgID->length, aSyncHdr->msgID->length );
	delete iCurrServerMsgID;
	iCurrServerMsgID = NULL;
	iCurrServerMsgID = msgID.AllocL();
	if ( msgID.Length() == 0 )
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlMsgIDMissing, EFalse, EFalse );
		return;
		}
	// target
	// here must be client's Source (IMEI), check that client has got its message  
	if ( !aSyncHdr->target ) 
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlTargetLocURIMissing, EFalse, EFalse );
		return;
		}
	else
	if ( !aSyncHdr->target->locURI ) 
		{
		iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
		iAgent->Interrupt( TNSmlError::ESmlTargetLocURIMissing, EFalse, EFalse );
		return;
		}
	else
		{
		HBufC* syncHdrTarget; 
		PCDATAToUnicodeLC( *aSyncHdr->target->locURI, syncHdrTarget );
		syncHdrTarget->Des().TrimRight();
		CNSmlURI* syncHdrTargetURI = CNSmlURI::NewLC( *syncHdrTarget );
		if ( syncHdrTargetURI->HostName().Length() != 0 && syncHdrTargetURI->HostName() != _L("/") )
			{
		    HBufC* imeiCode = HBufC::NewLC( 50 );
	        TPtr imeiCodePtr = imeiCode->Des();  
	        iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, imeiCodePtr );
			if ( syncHdrTargetURI->HostName() != *imeiCode )
				{
				iUnknownDevice = ETrue;
				iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusNotFound );
				CleanupStack::PopAndDestroy( 3 ); //imeiCode, syncHdrTargetURI,syncHdrTarget
				iAgent->Interrupt( TNSmlError::ESmlTargetLocURIInvalid, EFalse, EFalse );
				return;
				}
			CleanupStack::PopAndDestroy(); //imeiCode
			}
		CleanupStack::PopAndDestroy( 2 ); //syncHdrTargetURI, syncHdrTarget
		}
	// Cred
	if ( aSyncHdr->cred )
		{
		HBufC8* type = MetaTypeInUtf8LC( aSyncHdr->cred->meta );
		iServerAuth->SetTypeL( *type );
		CleanupStack::PopAndDestroy(); //type
		HBufC8* format = MetaFormatInUtf8LC( aSyncHdr->cred->meta );
		iServerAuth->SetFormatL( *format );
		CleanupStack::PopAndDestroy(); //format
		if ( aSyncHdr->cred->data )
			{
			if ( aSyncHdr->cred->data->content )
				{
				TPtr8 credential( (TUint8*) aSyncHdr->cred->data->content, aSyncHdr->cred->data->length, aSyncHdr->cred->data->length );
				iServerAuth->SetCredentialL( credential );
				}
			}
		}
	TNSmlError::TNSmlSyncMLStatusCode authStatus = iServerAuth->StatusCodeL();
	iStatusToServer->SetStatusCodeL( statusID, authStatus );
	if ( authStatus == TNSmlError::ESmlStatusUnauthorized )
		{
		iAgent->Interrupt( TNSmlError::ESmlServerUnauthorized, EFalse, ETrue );
		iServerAuth->CreateAndSaveNewNonceL();
		}
	
	if ( iServerAuth->ChallengeNeeded() )
		{
		iStatusToServer->SetChalL( statusID, iServerAuth->NonceL() );
		iServerAuth->SetChallenced();
		}
	

	//MaxMsgSize
	//Client must obey this value in the next message to server
	if ( aSyncHdr->meta )
		{
		if ( ( aSyncHdr->meta->content ) && ( aSyncHdr->meta->contentType == SML_PCDATA_EXTENSION ) && ( aSyncHdr->meta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aSyncHdr->meta->content;
			if ( metInf->maxmsgsize )
				{
				if ( metInf->maxmsgsize->content )
					{
					TPtr8 maxMsgSize( (TUint8*) metInf->maxmsgsize->content, metInf->maxmsgsize->length, metInf->maxmsgsize->length );
					TrimRightSpaceAndNull( maxMsgSize );
					TLex8 lexicalValue( maxMsgSize );
					TInt maxMsgSizeNum;
					if ( lexicalValue.Val( maxMsgSizeNum ) != KErrNone )
						{
						iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusIncompleteCommand );
						iAgent->Interrupt( TNSmlError::ESmlMaxMsgSizeInvalid, EFalse, EFalse );
						return;
						}
					else
						{
                                               
						if ( maxMsgSizeNum < KNSmlMininumWorkspaceSize )
							{
							iWorkspaceSize = maxMsgSizeNum;
							}
						else
							{
							if ( maxMsgSizeNum > iMaxWorkspaceSize )
								{
								iWorkspaceSize = iMaxWorkspaceSize;
								}
							else
								{
								iWorkspaceSize = maxMsgSizeNum;
								}
							}
						}
					}
				}
			}
		}
		iServerMaxObjectSize = ServerMaxObjSize( aSyncHdr->meta );
	}


// ---------------------------------------------------------
// CNSmlCmdsBase::ProcessStatusCmdL
// Handles Status command from a server.
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ProcessStatusCmdL( SmlStatus_t* aStatus )
	{
	//msgRef
	if ( !aStatus->msgRef )
		{
		return;
		}
	if ( !aStatus->msgRef->content )
		{
		return;
		}
	TPtr8 msgRef( (TUint8*) aStatus->msgRef->content, aStatus->msgRef->length, aStatus->msgRef->length );
	TrimRightSpaceAndNull( msgRef );
	if ( msgRef.Length() == 0 )  
		{
		return;
		}
	//cmd
	if ( !aStatus->cmd )
		{
		return;
		}
	if ( !aStatus->cmd->content )
		{
		return;
		}
	TPtr8 cmd( (TUint8*) aStatus->cmd->content, aStatus->cmd->length, aStatus->cmd->length );
	TrimRightSpaceAndNull( cmd );
	if ( cmd.Length() == 0 )  
		{
		return;
		}
	//cmdRef
	HBufC8* cmdRef = NULL;
	if ( !aStatus->cmdRef )
		{
		if ( cmd == KNSmlAgentSyncHdr )
			{
			cmdRef = HBufC8::NewLC( KNSmlAgentSyncHdrCmdID.iTypeLength );
			*cmdRef = KNSmlAgentSyncHdrCmdID;
			}
		else
			{
			return;
			}
		}
	else
	if ( !aStatus->cmdRef->content )
		{
		if ( cmd == KNSmlAgentSyncHdr )
			{
			cmdRef = HBufC8::NewLC( KNSmlAgentSyncHdrCmdID.iTypeLength );
			*cmdRef = KNSmlAgentSyncHdrCmdID;
			}
		else
			{
			return;
			}
		}
	else
		{
		TPtr8 cmdRefPtr ( (TUint8*) aStatus->cmdRef->content, aStatus->cmdRef->length, aStatus->cmdRef->length );
		TrimRightSpaceAndNull( cmdRefPtr );
		if ( cmdRefPtr.Length() == 0 )  
			{
			if ( cmd == KNSmlAgentSyncHdr )
				{
				cmdRef = HBufC8::NewLC( KNSmlAgentSyncHdrCmdID.iTypeLength );
				*cmdRef = KNSmlAgentSyncHdrCmdID;
				}
			else
				{
				return;
				}
			}
		else
			{
			cmdRef = HBufC8::NewLC( cmdRefPtr.Length() );
			*cmdRef = cmdRefPtr;
			}
		}
	// Status code
	TPtr8 statusCode( (TUint8*) aStatus->data->content, aStatus->data->length, aStatus->data->length );
	TrimRightSpaceAndNull( statusCode );
	if ( statusCode.Length() == 0 )
		{
		CleanupStack::PopAndDestroy(); //cmdRef
		return;
		}
	TLex8 lexicalStatus( statusCode );
	TInt numericStatus;
	if ( lexicalStatus.Val (numericStatus ) != KErrNone )
		{ 
		CleanupStack::PopAndDestroy(); //cmdRef
		return;
		}
	TInt entryID = 0;
	TBool statusIsMatching;
	statusIsMatching = iResponseController->MatchStatusElement( msgRef, *cmdRef , numericStatus, entryID );
	if ( !statusIsMatching )
		{
		}
	else
		{
		// chal element is stored for later use 
		if ( aStatus->chal )
			{
			if ( aStatus->chal->meta )
				{
				if ( aStatus->chal->meta->content && aStatus->chal->meta->contentType == SML_PCDATA_EXTENSION && aStatus->chal->meta->extension == SML_EXT_METINF )
					{
					SmlMetInfMetInf_t* metInf;
					metInf = (SmlMetInfMetInf_t*) aStatus->chal->meta->content;
					if ( metInf->type )
						{
						if ( metInf->type->content ) 
							{
							TPtr8 chalType( (TUint8*) metInf->type->content, metInf->type->length, metInf->type->length ); 
							TrimRightSpaceAndNull( chalType );
							if ( chalType.Length() > 0 )
								{
								iResponseController->SetChalTypeL( entryID, chalType );
								}
							}
						}
					if ( metInf->format )
						{
						if ( metInf->format->content )
							{
							TPtr8 chalFormat( (TUint8*) metInf->format->content, metInf->format->length, metInf->format->length ); 
							TrimRightSpaceAndNull( chalFormat );
							if ( chalFormat.Length() > 0 )
								{
								iResponseController->SetChalFormatL( entryID, chalFormat );
								}
							else
								{
								iResponseController->SetChalFormatL( entryID, KNSmlAgentChrFormat );
								}
							}
						else
							{
							iResponseController->SetChalFormatL( entryID, KNSmlAgentChrFormat );
							}
						}
					else
						{
						iResponseController->SetChalFormatL( entryID, KNSmlAgentChrFormat );
						}
					if ( metInf->nextnonce )
						{
						if ( metInf->nextnonce->content )
							{
							TPtr8 chalNextNonce( (TUint8*) metInf->nextnonce->content, metInf->nextnonce->length, metInf->nextnonce->length );
							TrimRightSpaceAndNull( chalNextNonce );
							if ( chalNextNonce.Length() > 0 )
								{
								iResponseController->SetChalNextNonceL( entryID, chalNextNonce );
								}
							}
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(); //cmdRef
	if ( statusIsMatching )
		{
		iAgent->CheckServerStatusCodeL( entryID );
		}
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::AlertCode
// 
// ---------------------------------------------------------
EXPORT_C TPtr8 CNSmlCmdsBase::AlertCode( SmlAlert_t* aAlert ) const
	{
	TPtr8 alertCode( NULL,0,0 );
	if ( aAlert )
		{
		if ( aAlert->data )
			{
			if ( aAlert->data->content )
				{
				alertCode.Set( (TUint8*) aAlert->data->content, aAlert->data->length, aAlert->data->length );
				TrimRightSpaceAndNull( alertCode );
				}
			}
		}
	return alertCode;
	}


// ---------------------------------------------------------
// CNSmlCmdsBase::StatusToUnsupportedCommandL()
// Makes status data to unsupported or illegal SyncML command
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::StatusToUnsupportedCommandL( TBool aIllegal, const TDesC8& aCmd, const SmlPcdata_t* aCmdRef, Flag_t aFlags )
	{
	if ( iStatusToServer )
		{
		TInt statusID( 0 );
		statusID = iStatusToServer->CreateNewStatusElementL(); 
		iStatusToServer->SetCmdRefL( statusID, aCmdRef );
		iStatusToServer->SetCmdL( statusID, aCmd ); 
		if ( iStatusToServerNoResponse || IsFlagSet( aFlags, SmlNoResp_f ) )
			{
			iStatusToServer->SetNoResponse( statusID, ETrue );
			}
		else
			{
			iStatusToServer->SetNoResponse( statusID, EFalse );
			}
		if ( aIllegal )
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusCommandFailed );
			}
		else
			{
			iStatusToServer->SetStatusCodeL( statusID, TNSmlError::ESmlStatusOptFeatureNotSupported );
			}
		}
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::GeneratedDocument()
// 
// ---------------------------------------------------------
TPtrC8 CNSmlCmdsBase::GeneratedDocument() const
	{
	return ( iGenerator->Document() );
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::BufferAreaForParsingL
// 
// ---------------------------------------------------------
EXPORT_C TPtr8 CNSmlCmdsBase::BufferAreaForParsingL()
	{
	delete iBufferArea;
	iBufferArea = 0;
	iBufferArea = HBufC8::NewL( iWorkspaceSize );
	return iBufferArea->Des();
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::BusyStatus
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::BusyStatus() const
	{
	TBool busyStatusReceived = EFalse;
	if ( iResponseController )
		{
		if ( iResponseController->BusyStatus( iCurrMsgID ) )
			{
			busyStatusReceived = ETrue;
			}
		}
	return ( busyStatusReceived );
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::StatusContainer()
// 
// ---------------------------------------------------------
EXPORT_C CNSmlStatusContainer* CNSmlCmdsBase::StatusContainer() const
	{
	return iStatusToServer;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::ResponseController()
// 
// ---------------------------------------------------------
EXPORT_C CNSmlResponseController* CNSmlCmdsBase::ResponseController() const
	{
	return iResponseController;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::ResultsContainer()
// 
// ---------------------------------------------------------
EXPORT_C CNSmlResultsContainer* CNSmlCmdsBase::ResultsContainer() const
	{
	return iResultsToServer;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::ServerAuth()
// 
// ---------------------------------------------------------
EXPORT_C CNSmlAuth* CNSmlCmdsBase::ServerAuth() const
	{
	return iServerAuth;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::RespURI
// 
// ---------------------------------------------------------
EXPORT_C CNSmlURI* CNSmlCmdsBase::RespURI() const
	{
	return iRespURI;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::DoMetaL
// Makes Meta element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoMetaL( SmlPcdata_t*& aMeta, const SmlMetInfMetInf_t* aMetaData ) const
	{
	aMeta = new( ELeave ) SmlPcdata_t; 
	aMeta->length = 0;
	aMeta->content = ( TAny* ) aMetaData;
	aMeta->contentType = SML_PCDATA_EXTENSION;
	aMeta->extension = SML_EXT_METINF;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::DoMetInfLC
// Initialises empty MetInf structure
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoMetInfLC( SmlMetInfMetInf_t*& aMeta ) const
	{
	aMeta = new( ELeave ) SmlMetInfMetInf_t;
	CleanupStack::PushL( aMeta );
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::PcDataNewL
// Create Pcdata 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::PcdataNewL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const
	{
	aPcdata = new( ELeave ) SmlPcdata_t;
	aPcdata->SetDataL( aContent );
	aPcdata->contentType = SML_PCDATA_OPAQUE;   
	aPcdata->extension = SML_EXT_UNDEFINED; 
	}

//1.2 CHANGES

// ---------------------------------------------------------
// CNSmlCmdsBase::DoSourceL
// Makes Source or Target element without LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoSourceL( sml_source_s*& aSource, const TDesC& aLocURIData ) const
	{
	aSource = new( ELeave ) sml_source_s; 
	HBufC8* locURIDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocURIData, locURIDataInUTF8 );
	PcdataNewL ( aSource->locURI, *locURIDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locURIDataInUTF8
	aSource->locName = NULL;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoSourceL
// Makes Source or Target element without LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoSourceL( sml_source_s*& aSource, const TDesC8& aLocURIData ) const
	{
	aSource = new( ELeave ) sml_source_s; 
	PcdataNewL ( aSource->locURI, aLocURIData );
	aSource->locName = NULL;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoSourceL
// Makes Source or Target element with LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoSourceL( sml_source_s*& aSource, const TDesC& aLocURIData, const TDesC& aLocNameData ) const
	{
	aSource = new( ELeave ) sml_source_s; 
	HBufC8* locURIDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocURIData, locURIDataInUTF8 );
	PcdataNewL ( aSource->locURI, *locURIDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locURIDataInUTF8
	HBufC8* locNameDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocNameData, locNameDataInUTF8 );
	PcdataNewL ( aSource->locName, *locNameDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locNameDataInUTF8
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::DoTargetL
// Makes Source or Target element without LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoTargetL( sml_target_s*& aTarget, const TDesC& aLocURIData ) const
	{
	aTarget = new( ELeave ) sml_target_s; 
	HBufC8* locURIDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocURIData, locURIDataInUTF8 );
	PcdataNewL ( aTarget->locURI, *locURIDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locURIDataInUTF8
	aTarget->locName = NULL;
	}
	
// ---------------------------------------------------------
// CNSmlCmdsBase::DoTargetL
// Makes Source or Target element without LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoTargetL( sml_target_s*& aTarget, const TDesC8& aLocURIData ) const
	{
	aTarget = new( ELeave ) sml_target_s; 
	PcdataNewL ( aTarget->locURI, aLocURIData );
	aTarget->locName = NULL;
	}
	
// ---------------------------------------------------------
// CNSmlCmdsBase::DoTargetL
// Makes Source or Target element with LocName element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoTargetL( sml_target_s*& aTarget, const TDesC& aLocURIData, const TDesC& aLocNameData ) const
	{
	aTarget = new( ELeave ) sml_target_s; 
	HBufC8* locURIDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocURIData, locURIDataInUTF8 );
	PcdataNewL ( aTarget->locURI, *locURIDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locURIDataInUTF8
	HBufC8* locNameDataInUTF8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( aLocNameData, locNameDataInUTF8 );
	PcdataNewL ( aTarget->locName, *locNameDataInUTF8 );
	CleanupStack::PopAndDestroy(); //locNameDataInUTF8
	}
//end changes

// ---------------------------------------------------------
// CNSmlCmdsBase::PCDATAToUnicodeLC
// Converts data in Pcdata structure (used by xml parser)
// to Unicode 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const
	{
	if ( !aPcdata.content )
		{
		aUnicodeData = NULL;
		aUnicodeData = HBufC16::NewLC( 0 );
		}
	TPtr8 pcdata( (TUint8*) aPcdata.content, aPcdata.length, aPcdata.length );
	TrimRightSpaceAndNull( pcdata );
	NSmlUnicodeConverter::HBufC16InUnicodeLC( pcdata, aUnicodeData );    
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ResetLargeObjectBuffer()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ResetLargeObjectBuffer()
	{
	delete iRecBuf;
	iRecBuf = NULL;
	iServerLargeObjectSize = 0;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::WaitingLargeObjectChunk()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::WaitingLargeObjectChunk() const
	{
	if ( iRecBuf != NULL ) 
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::ConstructL
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::ConstructL( CNSmlAgentBase* aAgentBase, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, TBool aVersionCheck )
	{
	iMaxWorkspaceSize = KNSmlDefaultWorkspaceSize;
	iAgent = aAgentBase;
	iSessionID = aSessionID.AllocL();
	iWorkspaceSize = iMaxWorkspaceSize;
	iVerProto = aVerProto.AllocL();
	iVersionPublicId = aPublicId;

//1.2 CHANGES: 1.1 and 1.2 version support
	if ( aPublicId == KNSmlSyncMLPublicId )
		{
		iVerDTD = KNSmlAgentVerDTD11().AllocL();
		}
	else
		{
		iVerDTD = KNSmlAgentVerDTD12().AllocL();
		}
//end changes
	
	iVersionCheck = aVersionCheck;
	iSyncMLUserName = aSyncMLUserName.AllocL();
	iSyncServer = CNSmlURI::NewL( aSyncServer->HostNameWithPortL() );
	iPhoneInfo = CNSmlPhoneInfo::NewL();
	iStatusToServer = CNSmlStatusContainer::NewL();
	iResponseController = CNSmlResponseController::NewL();
	iResultsToServer = CNSmlResultsContainer::NewL();
	iServerAuth = CNSmlAuth::NewL();
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::DoCmdIdL
// Makes CmdID element
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoCmdIdL( SmlPcdata_t*& aCmdID ) 
	{
	HBufC8* stringCmdID = HBufC8::NewLC( 16 );	
	stringCmdID->Des().Num( ++iCurrCmdID );
	PcdataNewL ( aCmdID, *stringCmdID );
	CleanupStack::PopAndDestroy();  //stringCmdID
	}



// ---------------------------------------------------------
// CNSmlCmdsBase::DoAnchorsL
// Makes Anchor elements in MetInf DTD
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoAnchorsL( SmlMetInfMetInf_t& aMetInf, const TDesC8* aLast, const TDesC8* aNext ) const
	{
	aMetInf.anchor = new ( ELeave) SmlMetInfAnchor_t;
	if ( aLast )
		{
		PcdataNewL( aMetInf.anchor->last, *aLast );
		}
	else
		{
		aMetInf.anchor->last = NULL;
		}
	if ( aNext )
		{
		PcdataNewL( aMetInf.anchor->next, *aNext );
		}
	else
		{
		aMetInf.anchor->next = NULL;
		}
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::DoMaxObjSizeL()
// Set MaxObjSize element with default value
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::DoMaxObjSizeL( SmlMetInfMetInf_t& aMetInf, TInt aMaxObjSize ) const
	{
	HBufC8* stringSize = HBufC8::NewLC( 16 );
	stringSize->Des().Num( aMaxObjSize );
	PcdataNewL( aMetInf.maxobjsize, *stringSize );
	CleanupStack::PopAndDestroy();  //stringSize
	}





// ---------------------------------------------------------
// CNSmlCmdsBase::SubtractConsumedFromPcData
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::SubtractConsumedFromPcData( SmlItem_t*& aItem, TInt aConsumed )
	{
	TPtr8 data( (TUint8*) aItem->data->content, aItem->data->length );
	data.SetLength( aItem->data->length );
	if ( data.Length() < aConsumed )
		{
		data.Delete( 0, data.Length() );
		aItem->data->length = 0;
		}
	else
		{
		data.Delete( 0, aConsumed );
		aItem->data->length -= aConsumed;
		}
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::StatusDataToGenericCommandL()
// Save status data to Add, Replace and Delete commands
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::StatusDataToGenericCommandL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem )
	{
	return StatusDataToCommandL( aCmd, aContent->cmdID, aContent->flags, aItem, aStatusCode, aStatusItem );
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::StatusDataToGetCommandL()
// Save status data to Get command
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::StatusDataToGetCommandL( const SmlGet_t* aContent, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem )
	{
	return StatusDataToCommandL( KNSmlAgentGet, aContent->cmdID, aContent->flags, aItem, aStatusCode, aStatusItem );
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::StatusDataToCommandL()
// Save status data to command
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::StatusDataToCommandL( const TDesC8& aCmd, const SmlPcdata_t* aCmdID, TUint aFlags, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem )
	{
	TInt statusID( 0 );
	statusID = iStatusToServer->CreateNewStatusElementL(); 
	iStatusToServer->SetCmdRefL( statusID, aCmdID );
	iStatusToServer->SetCmdL( statusID, aCmd ); 
	if ( iStatusToServerNoResponse || IsFlagSet( aFlags, SmlNoResp_f ) )
		{
		iStatusToServer->SetNoResponse( statusID, ETrue );
		}
	else
		{
		iStatusToServer->SetNoResponse( statusID, EFalse );
		}
	if ( aItem )
		{
		iStatusToServer->AddTargetRefL( statusID, aItem->target );
		iStatusToServer->AddSourceRefL( statusID, aItem->source );
		}
	if ( aStatusItem )
		{
		SmlPcdata_t* data = NULL;
		PcdataNewL( data, *aStatusItem );
		CleanupStack::PushL( data );
		iStatusToServer->AddItemDataL( statusID, data );
		CleanupStack::PopAndDestroy(); //data
		}
	iStatusToServer->SetStatusCodeL( statusID, aStatusCode );
	return statusID;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::CreateAndInitResponseItemL()
// Creates and initialises response controller for Clients commands 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::CreateAndInitResponseItemL( const TDesC8& aCommand )
	{
	TInt respID;
	respID = iResponseController->CreateNewResponseItemL();
	iResponseController->SetMsgID( respID, iCurrMsgID );
	iResponseController->SetCmdID( respID, iCurrCmdID );
	iResponseController->SetCmdL( respID, aCommand );
	return respID;
	}



// ---------------------------------------------------------
// CNSmlCmdsBase::TargetIsUnderItem()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::TargetIsUnderItem( const SmlItemList_t* aItemList ) const
	{
	if ( aItemList )
		{
		if ( aItemList->item )
			{
			if ( aItemList->item->target )
				{
				if ( aItemList->item->target->locURI )
					{
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::SourceIsUnderItem()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::SourceIsUnderItem( const SmlItemList_t* aItemList ) const
	{
	if ( aItemList )
		{
		if ( aItemList->item )
			{
			if ( aItemList->item->source )
				{
				if ( aItemList->item->source->locURI )
					{
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::MetaTypeInUtf8LC()
// 
// ---------------------------------------------------------
EXPORT_C HBufC8* CNSmlCmdsBase::MetaTypeInUtf8LC( const SmlPcdata_t* aMeta ) const
	{
	HBufC8* typeInUtf8 = NULL;
	if ( aMeta )
		{
		if ( ( aMeta->content ) && ( aMeta->contentType == SML_PCDATA_EXTENSION ) && ( aMeta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aMeta->content;
			if ( metInf->type )
				{
				if ( metInf->type->content )
					{
					TPtr8 type( (TUint8*) metInf->type->content, metInf->type->length, metInf->type->length );
					TrimRightSpaceAndNull( type );
					typeInUtf8 = type.AllocLC();		
					}
				}
			}
		}
	if ( typeInUtf8 == NULL )
		{
		typeInUtf8 = HBufC8::NewLC( 0 );
		}
	return typeInUtf8;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::MetaTypeLC()
// 
// ---------------------------------------------------------
EXPORT_C HBufC* CNSmlCmdsBase::MetaTypeLC( const SmlPcdata_t* aMeta ) const
	{
	HBufC* typeInUnicode = NULL;
	if ( aMeta )
		{
		if ( ( aMeta->content ) && ( aMeta->contentType == SML_PCDATA_EXTENSION ) && ( aMeta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aMeta->content;
			if ( metInf->type )
				{
				PCDATAToUnicodeLC( *metInf->type, typeInUnicode );
				}
			}
		}
	if ( typeInUnicode == NULL )
		{
		typeInUnicode = HBufC::NewLC( 0 );
		}
	return typeInUnicode;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::MetaFormatInUtf8LC()
// 
// ---------------------------------------------------------
EXPORT_C HBufC8* CNSmlCmdsBase::MetaFormatInUtf8LC( const SmlPcdata_t* aMeta ) const
	{
	HBufC8* formatInUtf8 = NULL;
	if ( aMeta )
		{
		if ( ( aMeta->content ) && ( aMeta->contentType == SML_PCDATA_EXTENSION ) && ( aMeta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aMeta->content;
			if ( metInf->format )
				{
				if ( metInf->format->content )
					{
					TPtr8 format( (TUint8*) metInf->format->content, metInf->format->length, metInf->format->length );
					TrimRightSpaceAndNull( format );
					formatInUtf8 = format.AllocLC();		
					}
				}
			}
		}
	if ( formatInUtf8 == NULL )
		{
		formatInUtf8 = HBufC8::NewLC( 0 );
		}
	return formatInUtf8;
	}
// ---------------------------------------------------------
// CNSmlCmdsBase::IsFlagSet
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlCmdsBase::IsFlagSet( const TUint& aValue, const TUint& aBit ) const
	{
	return( (aValue & aBit) == aBit );
	}


// ---------------------------------------------------------
// CNSmlCmdsBase::TrimRightSpaceAndNull
// Trims right spaces and zero terminator (NULL) 
// ---------------------------------------------------------
EXPORT_C void CNSmlCmdsBase::TrimRightSpaceAndNull( TDes8& aDes ) const
	{
	aDes.TrimRight();
	if ( aDes.Length() > 0 )
		{
		if ( aDes[aDes.Length() - 1] == NULL )
			{
			aDes.SetLength( aDes.Length() - 1 );
			}	
		}
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::MoreDataL
// Handles the buffering of received chunked large objects 
// ---------------------------------------------------------
EXPORT_C TNSmlError::TNSmlSyncMLStatusCode CNSmlCmdsBase::MoreDataL( const HBufC8* aUID, const SmlItem_t* aCurrentItem, const SmlPcdata_t* aMetaInCommand, CBufBase*& aTotalItem )
	{
	aTotalItem = NULL;
	TNSmlError::TNSmlSyncMLStatusCode statusCode = TNSmlError::ESmlStatusOK;
	TBool moreData = IsFlagSet( aCurrentItem->flags, SmlMoreData_f );
	
	if ( iRecBuf != NULL )  // previous chunks, check that UID is same
		{
		HBufC8* UID;
		if ( aUID )
			{
			UID = aUID->AllocLC();
			}
		else
			{
			UID = HBufC8::NewLC( 0 );
			}
		if ( *UID != *iLargeObjectUID )
			{
			iAgent->SetEndOfDataAlertRequest();
			delete iRecBuf;
			iRecBuf = NULL;
			delete iLargeObjectUID;
			iLargeObjectUID = NULL;
			iServerLargeObjectSize = 0;
			}
		CleanupStack::PopAndDestroy(); //UID
		}
	if ( moreData && iRecBuf == NULL ) //save object size when the first chunk
		{
		iServerLargeObjectSize = ServerObjectSize( aCurrentItem->meta );
		if ( iServerLargeObjectSize == 0 )
			{
			iServerLargeObjectSize = ServerObjectSize( aMetaInCommand );
			}
		delete iLargeObjectUID;
		iLargeObjectUID = NULL;
		if ( aUID )
			{
			iLargeObjectUID = aUID->AllocL();
			}
		else
			{
			iLargeObjectUID = HBufC8::NewL( 0 );
			}
		}
	
	TBool previousChunks = EFalse;
	if ( iRecBuf == NULL )  
		{
		iRecBuf = CBufFlat::NewL( 1 );
		}
	else
		{
		previousChunks = ETrue;
		}
	if ( aCurrentItem->data )
		{
		if ( aCurrentItem->data->content )
			{
			TPtr8 data( (TUint8*) aCurrentItem->data->content, aCurrentItem->data->length, aCurrentItem->data->length );
			iRecBuf->InsertL( iRecBuf->Size(), data );
			}
		}
	if ( !moreData )
		{ 
		if ( (previousChunks) && (iRecBuf->Size() != iServerLargeObjectSize) )
			{ // size check with large objects
			statusCode = TNSmlError::ESmlStatusSizeMismatch;
			delete iRecBuf;
			}
		else 
			{ // it is the last chunk, ownership of the buffer is moved to a caller
			aTotalItem = iRecBuf;
			}
		iRecBuf = NULL;
		iServerLargeObjectSize = 0;
		}
	else
		{
		aTotalItem = NULL;
		statusCode = TNSmlError::ESmlStatusItemAccepted;
		}
	
	return statusCode;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ServerMaxObjSize
// MaxObjSize from Meta element structure 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::ServerMaxObjSize( const SmlPcdata_t* aMeta ) const
	{
	TInt maxObjSizeNum = 0;
	if ( aMeta )
		{
		if ( ( aMeta->content ) && ( aMeta->contentType == SML_PCDATA_EXTENSION ) && ( aMeta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aMeta->content;
			if ( metInf->maxobjsize )
				{
				if ( metInf->maxobjsize->content )
					{
					TPtr8 maxObjSize( (TUint8*) metInf->maxobjsize->content, metInf->maxobjsize->length, metInf->maxobjsize->length );
					TrimRightSpaceAndNull( maxObjSize );
					TLex8 lexicalValue( maxObjSize );
					if ( lexicalValue.Val( maxObjSizeNum ) != KErrNone )
						{
						maxObjSizeNum = 0;
						}
					}
				}
			}
		}
	return maxObjSizeNum;
	}

// ---------------------------------------------------------
// CNSmlCmdsBase::ServerObjectSize()
// Size from Meta element structure 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlCmdsBase::ServerObjectSize( const SmlPcdata_t* aMeta ) const
	{
	TInt sizeNum = 0;
	if ( aMeta )
		{
		if ( ( aMeta->content ) && ( aMeta->contentType == SML_PCDATA_EXTENSION ) && ( aMeta->extension == SML_EXT_METINF ) )
			{
			SmlMetInfMetInf_t* metInf;
			metInf = (SmlMetInfMetInf_t*) aMeta->content;
			if ( metInf->size )
				{
				if ( metInf->size->content )
					{
					TPtr8 size( (TUint8*) metInf->size->content, metInf->size->length, metInf->size->length );
					TrimRightSpaceAndNull( size );
					TLex8 lexicalValue( size );
					if ( lexicalValue.Val( sizeNum ) != KErrNone )
						{
						sizeNum = 0;
						}
					}
				}
			}
		}
	return sizeNum;
	}

// -----------------------------------------------------------------------------
// CNSmlCmdsBase::SetMaximumWorkspaceSize
// Sets the maximum workspace size
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCmdsBase::SetMaximumWorkspaceSize( const TInt aMaxSize )
	{
	iMaxWorkspaceSize = aMaxSize;
	
	if ( iWorkspaceSize == KNSmlDefaultWorkspaceSize )
		{
		iWorkspaceSize = aMaxSize;
		}
	}
	
// ---------------------------------------------------------
// CNSmlCmdsBase::BufferAreaForDebugDumpL
// 
// ---------------------------------------------------------
#if defined (__NOTRANSPORT) || defined (_DEBUG)

TPtr8 CNSmlCmdsBase::BufferAreaForDebugDumpL()
	{
	return iBufferArea->Des();
	}

#endif

