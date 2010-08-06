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
* Description:  Parser for DM 1.2 formatted alert message
*
*/


#include <centralrepository.h>
#include <DevManInternalCRKeys.h>
#include "NSmlAlertQueue.h"
#include "nsmldebug.h" 
#include <nsmldmconst.h>
#include <e32property.h>
// ---------------------------------------------------------
// CNSmlDSAlertParser11(CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
// Returns pointer to the buffer
// ---------------------------------------------------------
//
CNSmlDMAlertParser11::CNSmlDMAlertParser11( CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
: CNSmlMessageParserBase( aAlertInfo, aHistoryInfo )
	{_DBG_FILE("CNSmlDMAlertParser11::CNSmlDMAlertParser11 begin!");
	}

// ---------------------------------------------------------
// CNSmlDMAlertParser11::~CNSmlDMAlertParser11()
// Destructor
// ---------------------------------------------------------
//
CNSmlDMAlertParser11::~CNSmlDMAlertParser11()
	{_DBG_FILE("CNSmlDMAlertParser11::~CNSmlDMAlertParser11 End");
	}	

// ---------------------------------------------------------
// CNSmlDMAlertParser11::ParseMessageL()
// Parses the aler message
// ---------------------------------------------------------
//	
void CNSmlDMAlertParser11::ParseMessageL()
	{
    _DBG_FILE("CNSmlDMAlertParser11::ParseMessageL begin!");

	//digest
	CheckLengthL( KNSmlAlertVersionPos + 1 );
	iHistoryInfo.SetMsgDigest( Message().Left( KNSmlAlertVersionPos ) );

	//version
	TInt version;
	version = ((TUint8) Message()[ KNSmlAlertVersionPos ]) << 8;
	version |= (TUint8) Message()[ KNSmlAlertVersionPos + 1 ];
	version = version >> 6;
	
	//ui interaction mode
	TInt uiMode = (TUint8) Message()[ KNSmlAlertVersionPos + 1 ] & KUiModeMask; 
	uiMode = uiMode >> 4;
	DBG_FILE_CODE(uiMode, _S8("CNSmlDMAlertParser11::ParseMessageL() : ui mode "));
	TInt SanSupport( KErrNone );
	CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    if( err == KErrNone )
       {
    centrep->Get( KDevManSANUIBitVariation, SanSupport );
       }
    delete centrep;
	if( SanSupport == 1 )
   	{
	    static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	    static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
		RProperty::Define(KPSUidNSmlSOSServerKey,KNSmlDMSilentJob,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
		RProperty::Set(KPSUidNSmlSOSServerKey,KNSmlDMSilentJob,KErrNone);  
		
	    if(uiMode == ESilent) //silent
	        {
	        TInt r2=RProperty::Set(KPSUidNSmlSOSServerKey,KNSmlDMSilentJob,ESilent);
	        DBG_FILE_CODE( r2, _S8("CNSmlDMAlertParser11::ParseMessageL() KNSmlDMSilentJob set error code") );
	        }
    iAlertInfo.SetUimode(uiMode);
   	}	
	if (uiMode == 0)
		{
		uiMode = CSmlAlertInfo::ECreateJob;
		}
		
	iAlertInfo.SetJobControl((CSmlAlertInfo::TJobControl) uiMode);
		
	//initiator
	TInt initiator = (TUint8) Message()[ KNSmlAlertVersionPos + 1 ] & KInitiatorMask; 
	initiator = initiator >> 3;
	
	//reserved
	TInt futNum = (TUint8) Message()[ KNSmlAlertVersionPos + 1 ] & KMaskUpperFuture;
	
	//session id
	CheckLengthL( KNSmlAlertSession + 1 );
	TInt sessionId;
	sessionId = ((TUint8) Message()[ KNSmlAlertSession ]) << 8;
	sessionId |= (TUint8) Message()[ KNSmlAlertSession + 1 ];
	iAlertInfo.SetSessionId( sessionId );
	
	//server id length
	CheckLengthL( KNSmlAlertServerIdLength );
	TInt serverIdLength;
	serverIdLength = (TUint8) Message()[ KNSmlAlertServerIdLength ];
	
	//server id
	CheckLengthL( KNSmlAlertServerIdLenPos + serverIdLength );
	HBufC8* hostAddress = HBufC8::NewLC( serverIdLength );
	hostAddress->Des().Copy( Message().Mid(KNSmlAlertServerIdLenPos, serverIdLength));
		
	//Try to find profile before accessing content types
	SearchProfileL( *hostAddress );
	
	if ( iFoundProfiles.Count() == 0 )
		{
		iAlertInfo.SetProfileId( KNSmlNullId );
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(); //hostAddress
	
	TRAPD(error, CheckLengthL( KNSmlAlertServerIdLenPos + serverIdLength + 1 ) );
	
	if ( error == KErrNone )
		{
		TInt fut = (TUint8) Message()[ KNSmlAlertServerIdLenPos + serverIdLength] & KFutureMask;
		}
		
	TInt firstBytePlace = KNSmlAlertServerIdLenPos + serverIdLength + 1;
	
	if ( Message().Length() - firstBytePlace > 0 )
		{
		//vendor-info
		HBufC8* vendorInfo = HBufC8::NewLC( Message().Length() - firstBytePlace );
		vendorInfo->Des().Copy( Message().Mid( firstBytePlace, Message().Length() - firstBytePlace ));
		iAlertInfo.SetVendorSpecificInfoL( vendorInfo->Des() );
		CleanupStack::PopAndDestroy(); //vendorInfo
		}
	else
		{
		iAlertInfo.SetVendorSpecificInfoL( KNullDesC8() );
		}
	
	ResolveProfileL(0);
	_DBG_FILE("CNSmlDMAlertParser11::ParseMessageL End");
	}


// ---------------------------------------------------------
// CNSmlDMAlertParser11::SearchProfileL(TDesC8& aServerUri)
// Searches for the profile
// ---------------------------------------------------------
//
void CNSmlDMAlertParser11::SearchProfileL( TDesC8& aServerUri )
	{

	HBufC *serverId = HBufC::NewLC(aServerUri.Size());
	TPtr typePtr = serverId->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8( typePtr, aServerUri);
	
	CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
	if ( settings->IsDMAllowedL() )
		{
		CNSmlDMProfileList* profileList = new( ELeave ) CArrayPtrFlat<CNSmlDMProfileListItem>(1);
    	CleanupStack::PushL( PtrArrCleanupItem( CNSmlDMProfileListItem, profileList ) );
		settings->GetProfileListL( profileList ); 
		for ( TInt i = 0; i < profileList->Count(); i++)
			{	
			TInt profileID = (*profileList)[i]->IntValue( TNSmlDMProfileData( EDSProfileId ) );
			CNSmlDMProfile* profile = settings->ProfileL( profileID );
			CleanupStack::PushL( profile );
			if ( profile->IntValue( TNSmlDMProfileData( EDMProfileServerAlertAction ) ) != ESmlDisableSync )
				{				
				if ( *serverId == profile->StrValue( EDMProfileServerId ) )
					{
					if ( iAlertInfo.Transport() == KUidNSmlMediumTypeInternet.iUid )
        			    {
        			    // accept only internet bearers
        			    if ( profile->IntValue(EDMProfileTransportId) == KUidNSmlMediumTypeInternet.iUid ) 
        			       
            				{
            				CNSmlAlertInfo* info = new (ELeave) CNSmlAlertInfo;
            				info->iProfileId = profile->IntValue( EDMProfileId );
            				info->iIAPId = profile->IntValue( EDMProfileIAPId );
            				iFoundProfiles.AppendL( info );
            				}        
        			    }
        			else
        			    {
        			    // accept only local bearers
        			    if ( profile->IntValue(EDMProfileTransportId) != KUidNSmlMediumTypeInternet.iUid )         			       
            				{
            				CNSmlAlertInfo* info = new (ELeave) CNSmlAlertInfo;
            				info->iProfileId = profile->IntValue( EDMProfileId );
            				info->iIAPId = profile->IntValue( EDMProfileIAPId );
            				iFoundProfiles.AppendL( info );
            				}        
        			    }
					}
				}
			CleanupStack::PopAndDestroy(); // profile
			}
		profileList->ResetAndDestroy();
		CleanupStack::PopAndDestroy(); //profileList	
		}
	CleanupStack::PopAndDestroy( 2 ); //settings, serverId
	
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
// CNSmlDMAlertParser11::ResolveProfileL(TInt aContentCount)
// Resolves profile
// ---------------------------------------------------------
//
void CNSmlDMAlertParser11::ResolveProfileL( TInt /*aContentCount*/ )
	{
	iAlertInfo.SetProfileId(iFoundProfiles[0]->iProfileId);
	}
