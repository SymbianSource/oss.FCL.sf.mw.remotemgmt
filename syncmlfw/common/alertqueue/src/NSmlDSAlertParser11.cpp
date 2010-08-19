/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parser for SyncML 1.1 formatted alert message
*
*/


#include <s32mem.h>
#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>
#include <ecom.h>
#include <barsread.h>
#include <barsc.h>
#include <s32mem.h>
#include <data_caging_path_literals.hrh>
#include <nsmldstypesres.rsg>

#include <nsmlconstants.h>
#include <nsmlunicodeconverter.h>
#include <nsmldsconstants.h>
#include "nsmldshostclient.h"
#include "nsmldsagconstants.h"
#include "NSmlAlertQueue.h"
#include <nsmldebug.h>

// ---------------------------------------------------------
// CNSmlDSAlertParser11::CNSmlDSAlertParser11(CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
// Constructor
// ---------------------------------------------------------
CNSmlDSAlertParser11::CNSmlDSAlertParser11( CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
: CNSmlMessageParserBase( aAlertInfo, aHistoryInfo )
	{
	_DBG_FILE("CNSmlDSAlertParser11::CNSmlDSAlertParser11");
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::~CNSmlDSAlertParser11()
// Destructor
// ---------------------------------------------------------
CNSmlDSAlertParser11::~CNSmlDSAlertParser11()
	{
	_DBG_FILE("CNSmlDSAlertParser11::~CNSmlDSAlertParser11");
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::ParseMessageL()
// Parses the message
// ---------------------------------------------------------
void CNSmlDSAlertParser11::ParseMessageL()
	{
	_DBG_FILE("CNSmlDSAlertParser11::ParseMessageL : Begin");
	CWBXMLSyncMLDocHandler* docHandler = CWBXMLSyncMLDocHandler::NewL( this );
	CleanupStack::PushL( docHandler );
	CWBXMLParser* parser = CWBXMLParser::NewL();
	CleanupStack::PushL( parser );
	parser->SetDocumentHandler( docHandler );
	parser->SetExtensionHandler( docHandler );
	RDesReadStream readStream;
	readStream.Open( Message() );
	CleanupClosePushL ( readStream );
	parser->SetDocumentL( readStream );
	TWBXMLParserError ret = KWBXMLParserErrorOk;

	while ( ret == KWBXMLParserErrorOk )
		{
	    ret = parser->ParseL();
		}
	
	if ( iFoundProfiles.Count() == 0 )
		{
		iAlertInfo.SetProfileId( KNSmlNullId );
		User::Leave(KErrNotFound);
		}
	
	ResolveProfileL( iContentCount );
	
	iAlertInfo.SetJobControl( CSmlAlertInfo::ECreateJob );
	
	CleanupStack::PopAndDestroy( 3 ); //readStream, parser, docHandler
	_DBG_FILE("CNSmlDSAlertParser11::ParseMessageL : End");
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::ResolveProfileL()
// Resolves profile if more than one match is found
// ---------------------------------------------------------
void CNSmlDSAlertParser11::ResolveProfileL( TInt /*aContentCount*/ )
	{
	CNSmlAlertInfo* info = NULL;
	TInt matchCheck(0);
	TInt matchIndex(0);
	
	TBool match(EFalse);
	
	for (TInt index = 0; index < iFoundProfiles.Count(); index++)
		{
		info = iFoundProfiles[index];
		
		if (info->iMatchCount == iContentCount)
			{
			match = ETrue;
			break; //all contents found and full match
			}
		
		if ( info->iMatchCount > matchCheck )
			{
			matchIndex = index;
			}
		
		}
			
	if ( !match )	
		{
		info = iFoundProfiles[matchIndex];
		}
	
	iAlertInfo.SetProfileId( info->iProfileId );
	
	for (TInt i=0; i< info->iTaskInfo.Count(); i++ )
		{
		iAlertInfo.TaskIds().AppendL( info->iTaskInfo[i]->iTaskId );
		iAlertInfo.TaskSyncTypes().AppendL( info->iTaskInfo[i]->iSyncType);
		}
	iAlertInfo.SetVendorSpecificInfoL( KNullDesC8() );
	}
	
// ---------------------------------------------------------
// CNSmlDSAlert::PCDATAToUnicodeLC()
// Converts data in Pcdata structure (used by xml parser)
// to Unicode 
// ---------------------------------------------------------
void CNSmlDSAlertParser11::PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const
	{
	if ( !aPcdata.content )
		{
		aUnicodeData = NULL;
		aUnicodeData = HBufC::NewLC( 0 );
		}
	TPtr8 pcdata( (TUint8*) aPcdata.content, aPcdata.length, aPcdata.length );
	TrimRightSpaceAndNull( pcdata );
	NSmlUnicodeConverter::HBufC16InUnicodeLC( pcdata, aUnicodeData );    
	}

/// ---------------------------------------------------------
// CNSmlDSAlert::TrimRightSpaceAndNull
// Trims right spaces and zero terminator (NULL) 
// ---------------------------------------------------------
void CNSmlDSAlertParser11::TrimRightSpaceAndNull( TDes8& aDes ) const
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

//
// Callback functions implementation
//
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlAlertCmdFuncL
// Alert command from server.  
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlAlertCmdFuncL( SmlAlertPtr_t aAlert )
	{
	TSmlSyncType syncType(ESmlTwoWay);
	
	if ( aAlert->data )	
		{
		TPtr8 pcdata( (TUint8*) aAlert->data->content, aAlert->data->length, aAlert->data->length );
		TrimRightSpaceAndNull( pcdata );
				
		if (pcdata == KNSmlDSTwoWayByServer)
			{
			syncType = ESmlTwoWay;
			}
		else if (pcdata == KNSmlDSOneWayFromClientByServer)
			{
			syncType = ESmlOneWayFromClient;
			}
		else if (pcdata == KNSmlDSRefreshFromClientByServer)
			{
			syncType = ESmlRefreshFromClient;
			}
		else if (pcdata == KNSmlDSOneWayFromServerByServer)
			{
			syncType = ESmlOneWayFromServer;
			}
		else if (pcdata == KNSmlDSRefreshFromServerByServer)
			{
			syncType = ESmlRefreshFromServer;
			}
		else if (pcdata == KNSmlDSSlowSync)
			{
			syncType = ESmlSlowSync;
			}
		else 
			{
			syncType = ESmlTwoWay;
			} 
			
		}

	SmlMetInfMetInf_t* metInf = NULL;
	TPtr8 mediaType( NULL, 0, 0 );

	if ( aAlert->itemList )
		{
		if (aAlert->itemList->item)
			{	
			if ( aAlert->itemList->item->meta )
				{
				if ( aAlert->itemList->item->meta->content && 
				aAlert->itemList->item->meta->contentType == SML_PCDATA_EXTENSION && 
				aAlert->itemList->item->meta->extension == SML_EXT_METINF )
				{
				metInf = (SmlMetInfMetInf_t*) aAlert->itemList->item->meta->content;
			
				if ( metInf->type )
					{
					mediaType.Set( static_cast<TUint8*>( metInf->type->content ), metInf->type->length, metInf->type->length );
					TrimRightSpaceAndNull( mediaType );
					}
				}
				}
			if ( aAlert->itemList->item->source )
				{
				HBufC8* remotePath = NULL;
				
				TPtr8 pcdata( (TUint8*) aAlert->itemList->item->source->locURI->content, aAlert->itemList->item->source->locURI->length, aAlert->itemList->item->source->locURI->length );
				TrimRightSpaceAndNull( pcdata );
				remotePath = pcdata.AllocLC();
				TPtr8 p = remotePath->Des();				
				iContentCount++;
				for (TInt i = 0; i < iFoundProfiles.Count(); i++)
					{
					MatchContentTypeL( p, mediaType, syncType, i );	
					}
							
				CleanupStack::PopAndDestroy(); // remotePath	
    		}
			}
		}
	
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlGetCmdFuncL
// Get command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlGetCmdFuncL( SmlGetPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlPutCmdFuncL
// Put command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlPutCmdFuncL( SmlPutPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlResultsCmdFuncL
// Results command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlResultsCmdFuncL( SmlResultsPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlStatusCmdFuncL
// Status command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlStatusCmdFuncL( SmlStatusPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlStartMessageFuncL
// SyncHdr from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlStartMessageFuncL( SmlSyncHdrPtr_t aSyncHdr )
	{
	if ( !aSyncHdr || !aSyncHdr->source || !aSyncHdr->source->locURI  ) 
		{
		User::Leave( KErrGeneral );
		}

    HBufC* syncHdrSource = NULL; 
    PCDATAToUnicodeLC( *aSyncHdr->source->locURI, syncHdrSource );
    syncHdrSource->Des().TrimRight();
    CNSmlURI* syncHdrSourceURI = CNSmlURI::NewLC( *syncHdrSource );
    SearchProfileURIL( *syncHdrSourceURI ); 
    CleanupStack::PopAndDestroy( 2 ); //syncHdrSourceURI, syncHdrSource
    
    HBufC* sessionId = NULL;
    PCDATAToUnicodeLC( *aSyncHdr->sessionID, sessionId );
    sessionId->Des().TrimRight();
    
    TInt id = 0;
	TLex lex( sessionId->Des() );
	lex.Val(id);
	
	iAlertInfo.SetSessionId( id );
	CleanupStack::PopAndDestroy( ); // sessionId
	
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::SearchProfileURIL( CNSmlURI& aUri )
// Matches profile
// ---------------------------------------------------------
TBool CNSmlDSAlertParser11::SearchProfileURIL( CNSmlURI& aUri )
	{
	TBool found( EFalse );
	
	CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
	
	CNSmlDSProfileList* profileList = new ( ELeave ) CArrayPtrFlat<CNSmlDSProfileListItem>(1);
	CleanupStack::PushL( PtrArrCleanupItem( CNSmlDSProfileListItem, profileList) );

	settings->GetAllProfileListL( profileList);
	
	//read profile values
	for (TInt index = 0; index < profileList->Count(); index++ )
		{
		TInt profileId = profileList->At(index)->IntValue(EDSProfileId);
		CNSmlDSProfile* profile = settings->ProfileL(profileId);
		CleanupStack::PushL( profile );
		
	    CNSmlURI* syncServer= CNSmlURI::NewL( profile->StrValue( EDSProfileServerURL), (profile->IntValue(EDSProfileTransportId) == KUidNSmlMediumTypeInternet.iUid));
    	
		if ( syncServer->HostNameWithPortL( EFalse ) == aUri.HostName() || syncServer->HostNameWithPortL( ETrue ) == aUri.HostName() )
			{
			if ( iAlertInfo.Transport() == KUidNSmlMediumTypeInternet.iUid )
			    {
			    // accept only internet bearers
			    if ( ( profile->IntValue(EDSProfileTransportId) == KUidNSmlMediumTypeInternet.iUid ) 
			       && ( profile->IntValue(EDSProfileServerAlertedAction) != ESmlDisableSync ) )
    				{
    				CNSmlAlertInfo* info = new (ELeave) CNSmlAlertInfo;
    				info->iProfileId = profile->IntValue( EDSProfileId );
    				info->iIAPId = profile->IntValue( EDSProfileIAPId );
    				iFoundProfiles.AppendL( info );
    				}        
			    }
			else
			    {
			    // accept only local bearers
			    if ( ( profile->IntValue(EDSProfileTransportId) != KUidNSmlMediumTypeInternet.iUid ) 
			        && ( profile->IntValue(EDSProfileServerAlertedAction) != ESmlDisableSync ) )
    				{
    				CNSmlAlertInfo* info = new (ELeave) CNSmlAlertInfo;
    				info->iProfileId = profile->IntValue( EDSProfileId );
    				info->iIAPId = profile->IntValue( EDSProfileIAPId );
    				iFoundProfiles.AppendL( info );
    				}        
			    }
			}
		delete syncServer;
		syncServer = NULL;
		
		CleanupStack::PopAndDestroy(); //profile
		}
		
	CleanupStack::PopAndDestroy(2); //profileList, settings
	
	if ( iAlertInfo.Transport() == KUidNSmlMediumTypeInternet.iUid )
	    {
    	if ( iFoundProfiles.Count() > 1 )
    	    {
    	    RArray<TInt> indexes;
            CleanupClosePushL( indexes );
            
            for (TInt j = 0; j < iFoundProfiles.Count(); j++ )
                {
                CNSmlAlertInfo* info = iFoundProfiles[j];
                if ( info->iIAPId == -1 )    
                    {
                    indexes.Append( j );    
                    }
                }
            
            if ( indexes.Count() < iFoundProfiles.Count() )
                {
                for ( TInt count(0); count < indexes.Count(); count++ )
                    {
                    CNSmlAlertInfo* temp = iFoundProfiles[indexes[count]];
                    iFoundProfiles.Remove( indexes[count] );
                    delete temp;
                    temp = NULL;       
                    }	        
                }
                
        	CleanupStack::PopAndDestroy( &indexes );    
    	    }
	    }
	    
	return found;
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::SearchProfileL( TDesC8& /*aServerUri*/ )
// Derived from base class
// ---------------------------------------------------------
void CNSmlDSAlertParser11::SearchProfileL( TDesC8& /*aServerUri*/ )
	{
	
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::MatchContentTypeL(TDesC8& aPath, TDesC8& aMimeType, TSmlSyncType aSyncType, TInt aProfileIndex)
// Matches content type
// ---------------------------------------------------------
void CNSmlDSAlertParser11::MatchContentTypeL( TDesC8& aPath, TDesC8& aMimeType, TSmlSyncType aSyncType, TInt aProfileIndex )
	{
	RImplInfoPtrArray implArray;
	CleanupStack::PushL(PtrArrCleanupItemRArr(CImplementationInformation, &implArray));
	
	TUid dsUid = { KNSmlDSInterfaceUid };
	REComSession::ListImplementationsL(dsUid, implArray);

	// Look through adapters to get right uid and ParameterList
	TInt countImpls = implArray.Count();
	CImplementationInformation* implInfo = NULL;
	
	for (TInt i = 0 ; i < countImpls; i++)
		{
		implInfo = implArray[ i ];
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		CNSmlDSProfile* profile = settings->ProfileL( iFoundProfiles[aProfileIndex]->iProfileId );
		CleanupStack::PushL( profile );
		
		CNSmlDSContentType* type = profile->ContentType( implInfo->ImplementationUid().iUid );
		
		if ( type )
			{
			
			HBufC *uri = HBufC::NewLC(aPath.Size());
			TPtr typePtr = uri->Des();
			CnvUtfConverter::ConvertToUnicodeFromUtf8( typePtr, aPath);
			
			HBufC *mimeType = HBufC::NewLC(aMimeType.Size());
			TPtr mimePtr = mimeType->Des();
			CnvUtfConverter::ConvertToUnicodeFromUtf8( mimePtr, aMimeType);

			if (typePtr.Compare( type->StrValue( EDSAdapterServerDataSource ))  == 0)
				{
				/* CR: 403-1188 */
				if (iAlertInfo.Transport() != KUidNSmlMediumTypeInternet.iUid)
					{
					_DBG_FILE("CNSmlDSAlertParser11::MatchContentTypeL : Inside Not Internet");
					if ( ConvertContentTypeL( mimePtr, implInfo->ImplementationUid().iUid ))
						{
						iFoundProfiles[aProfileIndex]->iMatchCount++;
					
						TNSmlContentTypeInfo* info = new (ELeave) TNSmlContentTypeInfo;
						info->iTaskId = type->IntValue( EDSAdapterTableId );
						info->iSyncType = aSyncType;
					
						iFoundProfiles[aProfileIndex]->iTaskInfo.AppendL( info );
						
						type->SetIntValue (EDSAdapterEnabled, ETrue) ; 
										
						profile->SaveL() ;
						}
					} // CR: 403-1188
				else
					{
					_DBG_FILE("CNSmlDSAlertParser11::MatchContentTypeL : Inside Internet");
					if ( ConvertContentTypeL( mimePtr, implInfo->ImplementationUid().iUid ) && (type->IntValue(EDSAdapterEnabled) != EFalse) )
						{
					
						iFoundProfiles[aProfileIndex]->iMatchCount++;
						
						TNSmlContentTypeInfo* info = new (ELeave) TNSmlContentTypeInfo;
						info->iTaskId = type->IntValue(EDSAdapterTableId);
						info->iSyncType = (TSmlSyncType) aSyncType;
						
						iFoundProfiles[aProfileIndex]->iTaskInfo.AppendL(info);
						}
					}				
				}
			CleanupStack::PopAndDestroy(2); // uri, mimeType
			}
			
		CleanupStack::PopAndDestroy(2); // settings, profile
		}
		
	REComSession::FinalClose();
	CleanupStack::PopAndDestroy(); //implArray

	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::ConvertContentTypeL( TDesC& aMimeType, const TInt aDataProviderId )
// Targets content type
// ---------------------------------------------------------
TBool CNSmlDSAlertParser11::ConvertContentTypeL( TDesC& aMimeType, const TInt aDataProviderId )
	{
	
	
	HBufC8* contentType;
	
	TInt pc(0);
	RFs	fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);
	pc++;
	
	TFileName fileName;
	Dll::FileName( fileName );
	
	TParse parse;
	parse.Set( KNSmlAlertDirAndResource, &KDC_RESOURCE_FILES_DIR, NULL );
	fileName = parse.FullName();
	
    RResourceFile resourceFile; 
	resourceFile.OpenL( fs,fileName );
	CleanupClosePushL( resourceFile );
	pc++;
	
	HBufC8* typesRes = resourceFile.AllocReadLC( NSML_DS_CONTENT_TYPES );
	pc++;
	TResourceReader reader;
	reader.SetBuffer( typesRes );

	TUint32 nOfTypes = reader.ReadInt16();
	HBufC* textPtr = NULL;
	TBool contentFound(EFalse);
	
	for ( TUint32 i=0; i < nOfTypes; i++)
		{
		reader.ReadInt16();	//num code
		textPtr = reader.ReadHBufCL();
		TPtr tmp = textPtr->Des();
		tmp.LowerCase();
		if ( textPtr->Des() == aMimeType )
			{
			contentFound = ETrue;
			break;
			}
		delete textPtr;
		textPtr = NULL;
		}
	
	if ( contentFound )
		{
		CleanupStack::PushL( textPtr );
		pc++;
		
		TPtr tmpPtr = textPtr->Des();
		tmpPtr.LowerCase();
		contentType = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *textPtr );
		CleanupStack::PushL( contentType );  // Leave aContentText to the cleanupstack
		pc++;
		
		CNSmlDSHostClient* hostClient = CNSmlDSHostClient::NewLC();
		pc++;
		
		RArray<TSmlDataProviderId> dataProviders;
		dataProviders.AppendL( aDataProviderId );
		CleanupClosePushL( dataProviders );
		pc++;
		
		RArray<TInt> result;
		CleanupClosePushL( result );
		pc++;
		
		TInt status(KErrNone);
		hostClient->CreateDataProvidersL( dataProviders, result );
		
		for (TInt index = 0; index < dataProviders.Count(); index++)
			{
			if (result[index] != KErrNone)
				{
				contentFound = EFalse;
				}
			}
		
		if ( contentFound )
			{
			TNSmlDPInformation* adapterInfo = hostClient->DataProviderInformationL( aDataProviderId, status );
			
			if ( status == KErrNone)
				{
				
				TInt mimeSupported(EFalse);
				
				TInt mimeCount = adapterInfo->iMimeTypes->Count();
				TPtrC8 pType = contentType->Des();
				
				for (TInt i = 0; i < mimeCount; i++)
					{
					TPtrC8 pInfo = (* adapterInfo->iMimeTypes )[i];
				
					if ( pType.Compare( pInfo ) == 0 )
						{
						mimeSupported = ETrue;
						break;
						}	
					}
				
				contentFound = mimeSupported;
				
				}
			else
				{
				contentFound = EFalse;
				}
			
			if ( adapterInfo )
			    {
			    delete adapterInfo->iDisplayName;
		    	delete adapterInfo->iMimeTypes;
			    delete adapterInfo->iMimeVersions;
			    delete adapterInfo;		        
			    }
			}
		
		}
	
	CleanupStack::PopAndDestroy(pc);
		
	return contentFound;
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlEndMessageFuncL
// End of SyncML message was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlEndMessageFuncL( Boolean_t /*aFinal*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlStartSyncFuncL
// Sync command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlStartSyncFuncL( SmlSyncPtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlEndSyncFuncL
// End of Sync command was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlEndSyncFuncL()
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlStartAtomicFuncL
// Atomic command from server, Atomic is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlStartAtomicFuncL( SmlAtomicPtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlEndAtomicFuncL
// End of Atomic command was reached, Atomic is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlEndAtomicFuncL()
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlStartSequenceFuncL
// Sequence command from server, Sequence is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlStartSequenceFuncL( SmlSequencePtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlEndSequenceFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlEndSequenceFuncL()
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlAddCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlAddCmdFuncL( SmlAddPtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlCopyCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlCopyCmdFuncL( SmlCopyPtr_t /*aContent*/)
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlDeleteCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlDeleteCmdFuncL( SmlDeletePtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlExecCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlExecCmdFuncL( SmlExecPtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlMapCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlMapCmdFuncL( SmlMapPtr_t /*aContent*/ )   
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlReplaceCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlReplaceCmdFuncL( SmlReplacePtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlSearchCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlSearchCmdFuncL( SmlSearchPtr_t /*aContent*/ )
	{
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser11::smlMoveCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlertParser11::smlMoveCmdFuncL( SmlMovePtr_t /*aContent*/ )
	{
	return 0;
	}

//End of File
