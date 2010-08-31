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
* Description:  Parser for SyncML 1.2 formatted alert message
*
*/


#include <ecom.h>
#include <barsread.h>
#include <barsc.h>
#include <s32mem.h>
#include <nsmldstypesres.rsg>
#include <data_caging_path_literals.hrh> 

#include <nsmlconstants.h>
#include <nsmldsconstants.h>
#include "NSmlAlertQueue.h"
#include "nsmldssettings.h"
#include "nsmldshostclient.h"
#include <nsmldebug.h>

//Fix to Remove the Bad Compiler Warnings
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ---------------------------------------------------------
// CNSmlDSAlertParser12(CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
// Returns pointer to the buffer
// ---------------------------------------------------------
//
CNSmlDSAlertParser12::CNSmlDSAlertParser12( CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
: CNSmlMessageParserBase( aAlertInfo, aHistoryInfo )
	{
	_DBG_FILE("CNSmlDSAlertParser12::CNSmlDSAlertParser12");
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser12::~CNSmlDSAlertParser12()
// Destructor
// ---------------------------------------------------------
//
CNSmlDSAlertParser12::~CNSmlDSAlertParser12()
	{
	_DBG_FILE("CNSmlDSAlertParser12::~CNSmlDSAlertParser12");
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser12::ParseMessageL()
// Parses the aler message
// ---------------------------------------------------------
//
void CNSmlDSAlertParser12::ParseMessageL()
	{
	_DBG_FILE("CNSmlDSAlertParser12::ParseMessageL : Begin");
	CheckLengthL( KNSmlAlertVersionPos + 1 );
		
	iHistoryInfo.SetMsgDigest( Message().Left( KNSmlAlertVersionPos ) );
	
	// version
	TInt version;
	version = ((TUint8) Message()[ KNSmlAlertVersionPos ]) << 8;
	version |= (TUint8) Message()[ KNSmlAlertVersionPos + 1 ];
	version = version >> 6;
	
	// ui interaction mode
	TInt uiMode = (TUint8) Message()[ KNSmlAlertVersionPos + 1 ] & KUiModeMask; 
	uiMode = uiMode >> 4;
	
	if (uiMode == 0)
		{
		uiMode = CSmlAlertInfo::ECreateJob;
		}
	iAlertInfo.SetJobControl( (CSmlAlertInfo::TJobControl) uiMode );
	
	// initiator
	TInt initiator = (TUint8) Message()[ KNSmlAlertVersionPos + 1 ] & KInitiatorMask; 
	initiator = initiator >> 3;
	
	// reserved
	TInt futNum = (TUint8) Message()[KNSmlAlertVersionPos + 1] & KMaskUpperFuture;
	
	// session id
	CheckLengthL( KNSmlAlertSession + 1 );
	TInt sessionId;
	sessionId = ((TUint8) Message()[ KNSmlAlertSession ]) << 8;
	sessionId |= (TUint8) Message()[ KNSmlAlertSession + 1 ];
	iAlertInfo.SetSessionId( sessionId );
	
	CheckLengthL( KNSmlAlertServerIdLength );
	// server id length
	TInt serverIdLength;
	serverIdLength = (TUint8) Message()[ KNSmlAlertServerIdLength ];
	
	CheckLengthL( KNSmlAlertServerIdLenPos + serverIdLength );

	// server id
	HBufC8* hostAddress = HBufC8::NewLC( serverIdLength );
	hostAddress->Des().Copy( Message().Mid(KNSmlAlertServerIdLenPos, serverIdLength));

	// Try to find profile before accessing content types
	SearchProfileL( *hostAddress );
#ifdef __NSML_DEBUG__
	TPtrC8 pn( hostAddress->Des() );
	DBG_ARGS8(_S8("CNSmlDSAlertParser12::ParseMessageL: hostAddress %S"), &pn);
#endif // __NSML_DEBUG__

	if ( iFoundProfiles.Count() == 0 )
		{
		iAlertInfo.SetProfileId( KNSmlNullId );
		User::Leave( KErrNotFound );
		}
		
	CleanupStack::PopAndDestroy(); // hostAddress

	// number of syncs
	TInt numSyncs = (TUint8) Message()[ KNSmlAlertServerIdLenPos + serverIdLength] >> 4;
	
	// reserved for future
	(TUint8) Message()[ KNSmlAlertServerIdLenPos + serverIdLength] & KFutureMask;
	
	TInt firstBytePlace = KNSmlAlertServerIdLenPos + serverIdLength + 1;
	
	if ( numSyncs )
		{
		TInt syncType;
		TInt contentType;
	//	TBuf8<KNSmlBufLength> syncTypeString;
	//	TBuf8<KNSmlBufLength> contentTypeString;
		
		for (TInt i=0; i<numSyncs; i++)
			{
			// synctype
			syncType = (TUint8) Message()[firstBytePlace] >> 4;
			ValidateSyncType( syncType );
			
			// future use
			(TUint8) Message()[firstBytePlace] & KFutureMask;
			
			// content type
			contentType = (TUint8) Message()[firstBytePlace + 2] << 8;
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S("CNSmlDSAlertParser12::ParseMessageL: Before bitwise or:  '%d'"), contentType );
#endif // __NSML_DEBUG__
			contentType |= (TUint8) Message()[firstBytePlace + 3];

			// server uri length
			TInt dbPathLength = (TUint8) Message()[firstBytePlace + 4];
			
			// server uri
			HBufC8* databaseURI = HBufC8::NewLC( dbPathLength );
			
			databaseURI->Des().Copy( Message().Mid(firstBytePlace + KNSmlDatabasePathStartPos, dbPathLength ));
#ifdef __NSML_DEBUG__ 
	// writting little late
	DBG_ARGS(_S("CNSmlDSAlertParser12::ParseMessageL: After bitwise or : '%d'"), contentType );
#endif // __NSML_DEBUG__			
			if ( syncType != -1 )
				{
				for (TInt profileCount = 0; profileCount < iFoundProfiles.Count(); profileCount++)
					{
					MatchContentTypeL( *databaseURI, contentType, (TSmlSyncType) syncType, profileCount);
					}
				}
			
			// find the start of the next content type in message
			// start + position of database path + length of database path
			firstBytePlace = firstBytePlace + KNSmlDatabasePathStartPos + dbPathLength;
			
			CleanupStack::PopAndDestroy(); // databaseURI
			}
		}
	
	// vendor-info
	HBufC8* vendorInfo = HBufC8::NewLC( Message().Length() - firstBytePlace );
	vendorInfo->Des().Copy( Message().Mid( firstBytePlace, Message().Length() - firstBytePlace ));
	iAlertInfo.SetVendorSpecificInfoL( vendorInfo->Des() );
	CleanupStack::PopAndDestroy(); // vendorInfo
	
	// Resolve the profile to use
	ResolveProfileL( numSyncs );
	_DBG_FILE("CNSmlDSAlertParser12::ParseMessageL : Ends");
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser12::ValidateSyncType( TInt aSyncType )
// Changes the server sync type to synchronization initiation sync type
// ---------------------------------------------------------
//
void CNSmlDSAlertParser12::ValidateSyncType( TInt& aSyncType )
	{
	switch ( KNSmlServerAlertCode + aSyncType )
		{
		
		case KNSmlServerAlertCodeTwoWay: 
			aSyncType = ESmlTwoWay;
			break;
		
		case KNSmlServerAlertCodeOneWayFromClient: 
			aSyncType = ESmlOneWayFromClient;
			break;
		
		case KNSmlServerAlertCodeRefreshFromClient: 
			aSyncType = ESmlRefreshFromClient;
			break;
		
		case KNSmlServerAlertCodeOneWayFromServer: 
			aSyncType = ESmlOneWayFromServer;
			break;
		
		case KNSmlServerAlertCodeRefreshFromServer: 
			aSyncType = ESmlRefreshFromServer;
			break;
		
		default:
			aSyncType = -1; 
			break;
		
		}
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser12::ResolveProfileL( TInt aContentCount )
// Resolves best matching profile
// ---------------------------------------------------------
//
void CNSmlDSAlertParser12::ResolveProfileL( TInt aContentCount )
	{
	CNSmlAlertInfo* info = NULL;
	TInt matchCheck(0);
	TInt matchIndex(0);
	
	TBool match(EFalse);
	
	#ifdef __NSML_DEBUG__
		DBG_ARGS(_S("CNSmlDSAlertParser12::ResolveProfileL: aContentCount:  '%d'"), aContentCount );
	#endif // __NSML_DEBUG__
	for ( TInt index = 0; index < iFoundProfiles.Count(); index++ )
		{
		info = iFoundProfiles[index];
		
		if ( info->iMatchCount == aContentCount )
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
		iAlertInfo.TaskSyncTypes().AppendL( info->iTaskInfo[i]->iSyncType );
		}
	
	
	}
	
// ---------------------------------------------------------
// CNSmlDSAlertParser12::SearchProfileL( TDesC8& aServerUri )
// Finds profile comparing the server id
// ---------------------------------------------------------
//
void CNSmlDSAlertParser12::SearchProfileL( TDesC8& aServerUri )
	{
	CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
	
	CNSmlDSProfileList* profileList = new ( ELeave ) CArrayPtrFlat<CNSmlDSProfileListItem>(1);
	CleanupStack::PushL( PtrArrCleanupItem( CNSmlDSProfileListItem, profileList ) );

	settings->GetAllProfileListL( profileList );
	
	HBufC *uri = HBufC::NewLC(aServerUri.Size());
	TPtr typePtr = uri->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8( typePtr, aServerUri);

	//read profile values
	for (TInt index = 0; index < profileList->Count(); index++ )
		{
		TInt profileId = profileList->At(index)->IntValue(EDSProfileId);
		
		CNSmlDSProfile* profile = settings->ProfileL(profileId);
		CleanupStack::PushL( profile );
		
		if (uri->Des().Compare( profile->StrValue( EDSProfileServerId) ) == 0)
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
		CleanupStack::PopAndDestroy(); //profile
		}
		
	CleanupStack::PopAndDestroy(3); //uri, profileList, settings	
	
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
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser12::MatchContentTypeL( TDesC8& aPath, TInt aContentType, TSmlSyncType aSyncType, TInt aProfileIndex )
// Matches alerted content type by local database
// ---------------------------------------------------------
//
void CNSmlDSAlertParser12::MatchContentTypeL( TDesC8& aPath, TInt aContentType, TSmlSyncType aSyncType, TInt aProfileIndex )
	{
#ifdef __NSML_DEBUG__
	TPtrC8 pn( aPath ); 
	DBG_ARGS8(_S8("CNSmlDSAlertParser12::MatchContentTypeL: aPath %S"), &pn);
#endif // __NSML_DEBUG__

	RImplInfoPtrArray implArray;
	CleanupStack::PushL(PtrArrCleanupItemRArr(CImplementationInformation, &implArray));
	
	TUid dsUid = { KNSmlDSInterfaceUid };
	REComSession::ListImplementationsL(dsUid, implArray);
	
	// Look through adapters to get right uid and ParameterList
	TInt countImpls = implArray.Count();
	CImplementationInformation* implInfo = NULL;
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S("CNSmlDSAlertParser12::MatchContentTypeL: countImpls : '%d'"), countImpls );
#endif // __NSML_DEBUG__
	
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
			
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S16("CNSmlDSAlertParser12::MatchContentTypeL: typePtr : %S"), &typePtr);
	DBG_ARGS(_S16("CNSmlDSAlertParser12::MatchContentTypeL: type Sting Val : %S"), &type->StrValue( EDSAdapterServerDataSource ));
#endif // __NSML_DEBUG__

			if (typePtr.Compare( type->StrValue( EDSAdapterServerDataSource ))  == 0)
				{
				/* CR: 403-1188 */
				if (iAlertInfo.Transport() != KUidNSmlMediumTypeInternet.iUid)
					{
					_DBG_FILE("CNSmlDSAlertParser12::MatchContentTypeL : Inside Not Internet");
					if ( ConvertContentTypeL( aContentType, implInfo->ImplementationUid().iUid ))
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
					_DBG_FILE("CNSmlDSAlertParser12::MatchContentTypeL : Inside Internet");
					if ( ConvertContentTypeL( aContentType, implInfo->ImplementationUid().iUid ) && ( type->IntValue(EDSAdapterEnabled ) != EFalse) )
						{
						iFoundProfiles[aProfileIndex]->iMatchCount++;
						
						TNSmlContentTypeInfo* info = new (ELeave) TNSmlContentTypeInfo;
						info->iTaskId = type->IntValue( EDSAdapterTableId );
						info->iSyncType = aSyncType;
						
						iFoundProfiles[aProfileIndex]->iTaskInfo.AppendL( info );
						}	
					}				
				}

			CleanupStack::PopAndDestroy(); //uri
			
			}
		CleanupStack::PopAndDestroy(2); // settings, profile
		}
					
	REComSession::FinalClose();
	
	CleanupStack::PopAndDestroy(); //implArray
	
	}

// ---------------------------------------------------------
// CNSmlDSAlertParser12::ConvertContentTypeL( const TInt aContentNum, const TInt aDataProviderId )
// Matches the mime types of the content type
// ---------------------------------------------------------
//
TBool CNSmlDSAlertParser12::ConvertContentTypeL( const TInt aContentNum, const TInt aDataProviderId )
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
	
#ifdef __NSML_DEBUG__
		TPtrC16 pn( fileName.Ptr(), fileName.Size());
		DBG_ARGS(_S16("CNSmlDSAlertParser12::ConvertContentTypeL: typePtr %S"), &pn);
#endif // __NSML_DEBUG__

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
	TUint32 numCode;
	TBool contentFound(EFalse);
	
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S("CNSmlDSAlertParser12::ConvertContentTypeL: nOfTypes : '%d'"), nOfTypes );
#endif // __NSML_DEBUG__
		
	for ( TUint32 i=0; i < nOfTypes; i++)
		{
		numCode = reader.ReadInt16();
		textPtr = reader.ReadHBufCL();
		if ( numCode == aContentNum )
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
#ifdef __NSML_DEBUG__
	TPtrC16 pn( tmpPtr);
	DBG_ARGS(_S16("CNSmlDSAlertParser12::ConvertContentTypeL: tmpPtr %S"), &pn);
#endif // __NSML_DEBUG__
		tmpPtr.LowerCase();
		contentType = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *textPtr );
		CleanupStack::PushL( contentType );  // Leave aContentText to the cleanupstack
		pc++;
		
		CNSmlDSHostClient* hostClient = CNSmlDSHostClient::NewLC();
		pc++;
		
		RArray<TSmlDataProviderId> dataProviders;
		dataProviders.AppendL( aDataProviderId );
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S("CNSmlDSAlertParser12::ConvertContentTypeL: aDataProviderId: '%d'"), aDataProviderId );
#endif // __NSML_DEBUG__
		CleanupClosePushL( dataProviders );
		pc++;
		
		RArray<TInt> resultArray;
		CleanupClosePushL( resultArray );
		pc++;
		TInt status(KErrNone);
		
		hostClient->CreateDataProvidersL( dataProviders, resultArray );
		
		for (TInt index = 0; index < dataProviders.Count(); index++)
			{
			if (resultArray[index] != KErrNone)
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
#ifdef __NSML_DEBUG__
	DBG_ARGS(_S("CNSmlDSAlertParser12::ConvertContentTypeL: mimeCount: '%d'"), mimeCount );
#endif // __NSML_DEBUG__
				TPtrC8 pType = contentType->Des();
				
				if ( aContentNum != 0 )
					{
					for (TInt i = 0; i < mimeCount; i++)
						{
						TPtrC8 pInfo = (* adapterInfo->iMimeTypes )[i];						
#ifdef __NSML_DEBUG__
	DBG_ARGS8(_S8("CNSmlDSAlertParser12::ConvertContentTypeL: pType %S"), &pType);
	DBG_ARGS8(_S8("CNSmlDSAlertParser12::ConvertContentTypeL: pInfo %S"), &pInfo);
#endif // __NSML_DEBUG__
						if ( pType.Compare( pInfo ) == 0 )
							{
							mimeSupported = ETrue;
							break;
							}	
						}
						
					}
				else
					{
					mimeSupported = ETrue;	
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

