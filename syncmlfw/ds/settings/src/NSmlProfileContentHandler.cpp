/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "NSmlProfileContentHandler.h"
#include "nsmldssettings.h"

#include <commdb.h>    //access point resolution
#include <commsdattypesv1_1.h>



// ============================ MEMBER FUNCTIONS ===============================


CNSmlProfileContentHandler::CNSmlProfileContentHandler()
	{
	}

// -----------------------------------------------------------------------------
// CXMLAttributes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlProfileContentHandler::ConstructL(CNSmlDSSettings* aSettings ,CNSmlProfileArray* aArray)
    {
	iCounter = 0;
	//iDSAccountInProgress = FALSE;
	iDSSettingInProgress = FALSE;
	iDSContentInProgress = FALSE;
	iSettings = aSettings;
	iCustomProfileArray = aArray;
	//iCustomProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDSProfile> (5);
	iContentArray = new (ELeave) CArrayPtrFlat<CNSmlDSContentSettingType> (7);
	iSyncTypeArray = new (ELeave) CArrayFixFlat<TInt> (1);
	iContentCounter = 0;
	iInvalidContent = EFalse;
    }

// -----------------------------------------------------------------------------
// CXMLAttributes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlProfileContentHandler* CNSmlProfileContentHandler::NewL(CNSmlDSSettings* aSettings,CNSmlProfileArray* aArray)
    {
		CNSmlProfileContentHandler* self = new( ELeave ) CNSmlProfileContentHandler;
    
		CleanupStack::PushL( self );
		self->ConstructL(aSettings ,aArray);
		CleanupStack::Pop();

		return self;
    }

    
// Destructor
CNSmlProfileContentHandler::~CNSmlProfileContentHandler()
    {
	if (iContentArray)
		{
		iContentArray->ResetAndDestroy();
		}
	delete iContentArray;
	delete iSyncTypeArray;
    }


TInt CNSmlProfileContentHandler::StartDocument()
   	{
   		return 1;
   	}

TInt CNSmlProfileContentHandler::EndDocument()
	{		
	
	return 1;
	}

//===============================================
//		CNSmlProfileContentHandler::StartElement()
//		
//		
//===============================================	

TInt CNSmlProfileContentHandler::StartElement(TDesC& /*aURI*/, TDesC& /*aLocalName*/, TDesC& aName, MXMLAttributes* /*aAttributeList*/)
	{
		if (aName == _L("DSSettings"))
		{
			iDSSettingInProgress = TRUE;
			CNSmlDSProfile* profile = iSettings->CreateProfileL();
			iCustomProfileArray->AppendL(profile);  
			
						
		}
		if (aName == _L("DSContentSettings"))
		{
			iDSContentInProgress = TRUE;
			CNSmlDSContentSettingType* content = CNSmlDSContentSettingType::NewL();
			iContentArray->AppendL(content);
			
						
		}
		if (iDSSettingInProgress)
		{
		    if (aName.Compare(KNSmlDSProfileDisplayName) == 0)
			{
				iState = EProfileDisplayName;
			}
			
			if (aName.Compare(KNSmlDSProfileServerURL) == 0)
			{
				iState = EProfileServerURL;
			
			}
			if (aName.Compare(KNSmlDSProfileIAPId) == 0)
			{
				iState = EProfileIAPId;
			
			}
			if (aName.Compare(KNSmlDSProfileTransportId) == 0)
			{
				iState = EProfileTransportId;
			
			}
			if (aName.Compare(KNSmlDSProfileSyncServerUsername) == 0)
			{
				iState = EProfileSyncServerUsername;
			}
			if (aName.Compare(KNSmlDSProfileSyncServerPassword) == 0)
			{
				iState = EProfileSyncServerPassword;
			}
			if (aName.Compare(KNSmlDSProfileServerAlertedAction) == 0)
			{
				iState = EProfileServerAlertedAction;
			}
			if (aName.Compare(KNSmlDSProfileHidden) == 0)
			{
				iState = EProfileHidden;
			}
			if (aName.Compare(KNSmlDSProfileHttpAuthUsed) == 0)
			{
				iState = EProfileHttpAuthUsed;
			}
			if (aName.Compare(KNSmlDSProfileHttpAuthUsername) == 0)
			{
				iState = EProfileHttpAuthUsername;
			}
			if (aName.Compare(KNSmlDSProfileHttpAuthPassword) == 0)
			{
				iState = EProfileHttpAuthPassword;
			}
			if (aName.Compare(KNSmlDSProfileServerId) == 0)
			{
				iState = EProfileServerId;
			
			}
			if (aName.Compare(KNSmlDSProfileProtocolVersion) == 0)
			{
				iState = EProfileProtocolVersion;
			}
			if (aName.Compare(KNSmlAdapterSyncType)  == 0)
			{
				iState = ESyncType;
			}
			if (aName.Compare(KNSmlDSProfileDefaultProfile)  == 0)
            {
                iState = EDefaultProfile;
            }
		}
		else if(iDSContentInProgress)
		{
			if (aName.Compare(KNSmlDSProfileDisplayName) == 0)
			{
				iState = EProfileDisplayName;
			}
			if (aName.Compare(KNSmlDSProfileServerId) == 0)
			{
				iState = EProfileServerId;
			}
			if (aName.Compare(KNSmlAdapterImlementationUID) == 0)
			{
				iState = EAdapterImplementationId;
			}
			if (aName.Compare(KNSmlAdapterServerDataSource) == 0)
			{
				iState = EAdapterServerDataSource;
			}
				
		}
	
		return 1;
	}

//===============================================
//		CNSmlProfileContentHandler::StartElement()
//		
//		
//===============================================	
TInt CNSmlProfileContentHandler::EndElement(TDesC& /*aURI*/, TDesC& /*aLocalName*/, TDesC& aName)
	{
				
		if (aName == _L("DSSettings") && iDSSettingInProgress )
		{
	    	iDSSettingInProgress = FALSE;
	    	iCounter++;
			
		}
		else if (aName == _L("DSContentSettings") && iDSContentInProgress )
		{
			iDSContentInProgress = FALSE;
			CreateContentTypeL();
			iContentCounter++;
			iInvalidContent = EFalse;
		}
		
		if (iState != EStateHandled)
		{
			iState = EStateHandled;   //Assume that empty feild. No need to set.
		}
		
		
				
		return 1;
}

//===============================================
//		CNSmlProfileContentHandler::StartCDATA()
//		
//		
//===============================================	
TInt CNSmlProfileContentHandler::StartCDATA()
{

	return 1;
}

//===============================================
//		CNSmlProfileContentHandler::EndCDATA()
//		
//		
//===============================================	
TInt CNSmlProfileContentHandler::EndCDATA()
{

	return 1;
}

//===============================================
//		CNSmlProfileContentHandler::Charecters()
//		
//		
//===============================================	
TInt CNSmlProfileContentHandler::Charecters(TDesC& aBuf, TInt /*aStart*/, TInt /*aLength*/)
{	

	TInt aValue;
	if (iDSSettingInProgress)
	{
		
		if(iState == EProfileDisplayName)
		{
			if(aBuf.Length() > KNSmlMaxProfileNameLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, aBuf) ;
			}
			iState = EStateHandled;		
		}
		if(iState == EProfileServerURL)
		{
			if(aBuf.Length() > KDSMaxURLLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			SetServerURL(aBuf);
			}
			iState = EStateHandled;		
		}
		if(iState == EProfileIAPId)
		{
		
			if(aBuf.Length() > KAccessPointNameMaxLen)
			{
			_LIT(KDefaultValue ,"-1");
			aBuf = KDefaultValue;	
			}
			
			TInt iapId = AccessPointIdL(aBuf);
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileIAPId, iapId) ;
			iState = EStateHandled;		
		}
		if(iState == EProfileTransportId)
		{
			_LIT16(KHexCode,"0x");
			TBuf<64> tempBuf(aBuf);
			TInt temp = tempBuf.Find(KHexCode); 
			tempBuf.Delete(0, temp + 2);

			iLex = tempBuf;
			TInt64 aVal;
			User::LeaveIfError(iLex.Val(aVal ,EHex));
			if (aVal != KUidNSmlMediumTypeInternet.iUid && aVal != KUidNSmlMediumTypeBluetooth.iUid)
				{
				aVal = KUidNSmlMediumTypeInternet.iUid;	
				}
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileTransportId, aVal) ;
			iState = EStateHandled;		
		}
		
		if(iState == EProfileSyncServerUsername)
		{
			if(aBuf.Length() > KNSmlMaxUsernameLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileSyncServerUsername, aBuf) ;
			}
			iState = EStateHandled;	
		}
		if(iState == EProfileSyncServerPassword)
		{
			if(aBuf.Length() > KNSmlMaxPasswordLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileSyncServerPassword, aBuf) ;
			}
			iState = EStateHandled;	
		}
		if(iState == EProfileServerAlertedAction)
		{
			iLex = aBuf;
			User::LeaveIfError(iLex.Val(aValue));
			TInt typeId = ServerAlertType(aValue);
			
			if(aValue != ESmlEnableSync && aValue != ESmlConfirmSync && aValue != ESmlDisableSync)
				{
				aValue = ESmlEnableSync;
				}
			
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileServerAlertedAction, typeId) ;
			iState = EStateHandled;		
		}
	
		if(iState == EProfileHidden)
		{
			iLex = aBuf;
			User::LeaveIfError(iLex.Val(aValue));
			SetVisibility(aValue);
			iState = EStateHandled;		
		}
		
		if(iState == EProfileHttpAuthUsed)
		{
			iLex = aBuf;
			User::LeaveIfError(iLex.Val(aValue));
			if (aValue != 0 && aValue != 1)
			{
				aValue = 1;
			}
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileHttpAuthUsed, aValue) ;
			iState = EStateHandled;		
		}
		
		if(iState == EProfileHttpAuthUsername)
		{	
			if(iCustomProfileArray->At(iCounter)->IntValue(EDSProfileTransportId) 
											== KUidNSmlMediumTypeBluetooth.iUid 
				|| !iCustomProfileArray->At(iCounter)->IntValue(EDSProfileHttpAuthUsed))
				{
				iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileHttpAuthUsername, KEmpty) ;
				}
			else if (aBuf.Length() > KNSmlMaxHttpAuthUsernameLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileHttpAuthUsername, aBuf) ;
			}
			iState = EStateHandled;
		}
		
		if(iState == EProfileHttpAuthPassword)
		{	
			if(iCustomProfileArray->At(iCounter)->IntValue(EDSProfileTransportId) 
											== KUidNSmlMediumTypeBluetooth.iUid 
				|| !iCustomProfileArray->At(iCounter)->IntValue(EDSProfileHttpAuthUsed))
				{
				iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileHttpAuthPassword, KEmpty) ;
				}
			else if(aBuf.Length() > KNSmlMaxHttpAuthPasswordLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileHttpAuthPassword, aBuf) ;
			}
			iState = EStateHandled;	
		}
		
		if(iState == EProfileServerId)
		{
			if(aBuf.Length() > KNSmlMaxServerIdLength)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileDisplayName, KEmpty) ;
			}
			else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerId, aBuf) ;						
			}
			iState = EStateHandled;	
		}
		if(iState == EProfileProtocolVersion)
		{
			iLex = aBuf;
			User::LeaveIfError(iLex.Val(aValue));
			
			if(aValue != ESmlVersion1_1_2 && aValue != ESmlVersion1_2)
				{
				aValue = ESmlVersion1_2;
				}
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileProtocolVersion, aValue) ;
			iState = EStateHandled;		
		}
		
		if(iState == ESyncType)
		{
			TInt syncType = SyncType (aBuf);
			iSyncTypeArray->AppendL(syncType);
			iState = EStateHandled;		
		}
		
		if(iState == EDefaultProfile)
        {
            if(aBuf.Compare(_L("true")) == 0)
                {
                aValue = 1;
                }
            else
                {
                aValue = 0;
                }
            iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileDefaultProfile, aValue );
            iState = EStateHandled;     
        }
	}
	else if (iDSContentInProgress)
	{
		if(iState == EProfileDisplayName)
		{
			if(aBuf.Length() > KNSmlMaxProfileNameLength)
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSProfileName, KEmpty) ;
			iInvalidContent = ETrue;
			}
			else
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSProfileName ,aBuf);
			}
			iState = EStateHandled;
		}
		if(iState == EProfileServerId)
		{	
			if(aBuf.Length() > KNSmlMaxServerIdLength || iInvalidContent)
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSProfileName, KEmpty) ;
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSServerId, KEmpty) ;
			}
			else
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSServerId ,aBuf);
			}
			iState = EStateHandled;
		}
		if(iState == EAdapterServerDataSource )
		{	
			if(aBuf.Length() > KDSMaxRemoteNameLength)
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSServerDataSource, KEmpty) ;
			}
			else
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSServerDataSource, aBuf);
			}
			iState = EStateHandled;
		}
		if(iState == EAdapterImplementationId )
		{	
			if(aBuf.Length() > KNSmlMaxAdapterIdLength)
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSAdapaterId, KEmpty) ;
			}
			else
			{
			iContentArray->At(iContentCounter)->SetStrValue( EXMLDSAdapaterId, aBuf);
			}
			iState = EStateHandled;
		}
		

	}

	return 1;
}


//=====================================================
//		CNSmlProfileContentHandler::CreateContentTypeL()
//		
//		
//=====================================================	
void CNSmlProfileContentHandler::CreateContentTypeL()
{
	
	TBuf<128> buffer;
	buffer = iContentArray->At(iContentCounter)->StrValue( EXMLDSAdapaterId);
		
	TInt64 adapterId = 0;
	
	if (buffer.Compare(KEmpty) == 0)
		{
		return;
		}
	_LIT16(KHexCode,"0x");
	TInt temp = buffer.Find(KHexCode); 
	buffer.Delete(0, temp + 2);
	iLex = buffer;
	User::LeaveIfError(iLex.Val(adapterId ,EHex));
	
	if (IsContentAdded(adapterId))
		{
			return;
		}
	
  	buffer = iContentArray->At(iContentCounter)->StrValue( EXMLDSServerDataSource);
	TBuf<16> clientDataSource(KEmpty) ;
	TInt index;
	if(CheckXMLContentSetting())
	{
		if(iContentArray->At(iContentCounter)->StrValue( EXMLDSServerId).Compare(KEmpty) != 0)
		{
			for(index = 0; index < iCounter; index++)
			{
			if(iContentArray->At(iContentCounter)->StrValue( EXMLDSServerId).
										Compare(iCustomProfileArray->At(index)->StrValue(EDSProfileServerId)) == 0)
				{
				break;
				}
			}
				
		}
		else
		{
			for(index = 0; index < iCounter; index++)
			{
			if(iContentArray->At(iContentCounter)->StrValue( EXMLDSProfileName).
										Compare(iCustomProfileArray->At(index)->StrValue(EDSProfileDisplayName)) == 0)
				{
				break;
				}
			}
		}
		
		if (index < iCounter)
		{
			if (adapterId == KUidNSmlAdapterEMail.iUid && 
							iCustomProfileArray->At(index)->IntValue(EDSProfileProtocolVersion) == ESmlVersion1_1_2)
				{
				return;
				}
			iCustomProfileArray->At(index)->AddContentTypeL( adapterId, clientDataSource, buffer);
			
			CNSmlDSContentType* content = iCustomProfileArray->At(index)->ContentType(adapterId);
			content->SetIntValue( EDSAdapterSyncType, iSyncTypeArray->At(index));
		}
	
	}
	
}

//=====================================================
//		CNSmlProfileContentHandler::IsContentAdded()
//		
//		
//=====================================================	
TBool CNSmlProfileContentHandler::IsContentAdded(TInt aAdapter)
{
	TBuf<KNSmlMaxServerIdLength> serverId;
	TBuf<KNSmlMaxProfileNameLength> profileName;
	
	serverId = iContentArray->At(iContentCounter)->StrValue( EXMLDSServerId);

	if (serverId.Compare(KEmpty) != 0)
		{
		for(TInt i = 0; i < iCounter; i++)
			{
			if(serverId.Compare(iCustomProfileArray->At(i)->StrValue(EDSProfileServerId)) == 0)
				{
				if(iCustomProfileArray->At(i)->ContentType(aAdapter))
					{
					return ETrue;
					}
				}
			}
		}
	else
		{
		profileName = iContentArray->At(iContentCounter)->StrValue( EXMLDSProfileName); 
		for(TInt i = 0; i < iCounter; i++)
			{
			if(profileName.Compare(iCustomProfileArray->At(i)->StrValue(EDSProfileDisplayName)) == 0)
				{
				if(iCustomProfileArray->At(i)->ContentType(aAdapter))
					{
					return ETrue;
					}
				}
			}
		
		}
	
	return EFalse;	
}

//=====================================================
//		CNSmlProfileContentHandler::AccessPointIdL()
//		
//		
//=====================================================	
TInt CNSmlProfileContentHandler::AccessPointIdL(TDesC& aBuf)
{

	const TInt defConn = -2;
	if (aBuf == _L("-1"))
		{
			return defConn; // return default connection always
		}
		
	CCommsDatabase *database = CCommsDatabase::NewL();
    TUint32 aId ;
    TInt retVal;
    CleanupStack::PushL(database);
    CCommsDbTableView*  checkView;
    checkView = database->OpenViewMatchingTextLC(TPtrC(IAP),TPtrC(COMMDB_NAME), aBuf);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
         //Get the IAP ID 
         checkView->ReadUintL(TPtrC(COMMDB_ID), aId);
         retVal = aId;
        }
    else
    	{
        	retVal = defConn;
	   	}	  
       	
    CleanupStack::PopAndDestroy(2);    
    return retVal;          
	
	
}

//=====================================================
//		CNSmlProfileContentHandler::SetServerURL()
//		
//		
//=====================================================
void CNSmlProfileContentHandler::SetServerURL(TDesC &aBuf)
{
	
	_LIT(KColon, ":");
	_LIT(KSlash, "/");
	
	const TInt KPortMaxLen = 5;
	TBuf<150> temp;
	TBuf<KPortMaxLen> port;
	
	_LIT(KHttpHeader, "http://");
	_LIT(KHttpsHeader, "https://");
	_LIT(KDefaultPort,":80");
	
	const TInt KHttpLength = 7;
	const TInt KHttpsLength = 8;
	
	TInt insertPos = 0;
	
	if (iCustomProfileArray->At(iCounter)->IntValue(EDSProfileTransportId) == KUidNSmlMediumTypeBluetooth.iUid)
		{
		if (aBuf.Left(KHttpLength).Compare(KHttpHeader) == 0 
							|| aBuf.Left(KHttpsLength).Compare(KHttpsHeader) == 0)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, KEmpty);
			}
		else
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, aBuf);	
			}
		return;
		}
	if (aBuf.Compare(KEmpty) == 0 || aBuf.Compare(KHttpHeader) == 0 || aBuf.Compare(KHttpsHeader) == 0
		|| (aBuf.Left(KHttpLength).Compare(KHttpHeader) != 0 && aBuf.Left(KHttpsLength).Compare(KHttpsHeader) != 0))
		{
		iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, KEmpty);
		return;
		}
	
	temp = aBuf;
	if(temp.Left(KHttpsLength).Compare(KHttpsHeader) == 0)
		{
		temp.Delete(0, KHttpsLength);
		insertPos += KHttpsLength;
		}
	else if(temp.Left(KHttpLength).Compare(KHttpHeader) == 0)
		{
		temp.Delete(0, KHttpLength);
		insertPos += KHttpLength;
		}
	TInt start = temp.Find(KColon);
	
	if (start > 0)
		{
		
		temp.Delete(0, start+1);
		TInt length = temp.Find(KSlash);
		if (length == KErrNotFound)
			{
			length = temp.Length();
			}
		if (length > KPortMaxLen)
			{
			iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, KEmpty);
			return;	
			}
		if (length > 0)
			{
			port.Append(temp.Left(length));
			}
		insertPos += length;
		}
	else
		{
		TInt len = temp.Find(KSlash);
		insertPos += len;
		}
	
	if(port.Compare(KEmpty) != 0)
		{
		iLex = port;
		TInt portNum;
		TInt err = iLex.Val(portNum);
		const TInt KMinURIPortNumber = 1;
	    const TInt KMaxURIPortNumber = 65535;
	  
	    if( err != KErrNone || portNum < KMinURIPortNumber || portNum > KMaxURIPortNumber )
	   		{
	   		iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, KEmpty);
			return;
			}
		}
	else
		{
		temp = aBuf;
		TBuf<150> temp1;
		
		temp1.Append(temp.Left(insertPos));
		temp1.Append(KDefaultPort);
		temp.Delete(0, insertPos);
		temp1.Append(temp);
				
		iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, temp1);	
		return;		
		}
	
   	
	iCustomProfileArray->At(iCounter)->SetStrValue( EDSProfileServerURL, aBuf);
	
}
//=====================================================
//		CNSmlProfileContentHandler::ServerAlertType()
//		
//		
//=====================================================
TSmlServerAlertedAction CNSmlProfileContentHandler::ServerAlertType(TInt aType)
{
	TSmlServerAlertedAction retVal = ESmlEnableSync;
	switch(aType)
	{
		case 1:
		{
		retVal = ESmlEnableSync;
		break;	
		}
		
		case 2:
		{
		retVal = ESmlConfirmSync;
		break;
		}
		
		case 3:
		{
		retVal = ESmlDisableSync;
		break;
		}
		 
	}
	
	return retVal;
	
}

//=====================================================
//		CNSmlProfileContentHandler::SyncType()
//		
//		
//=====================================================
TSmlSyncType CNSmlProfileContentHandler::SyncType(TDesC& aBuf)
{
	TSmlSyncType retVal = ESmlTwoWay;
	if (aBuf == _L("ESmlTwoWay"))
		{
		retVal = ESmlTwoWay;
		}
	else if(aBuf ==_L("ESmlOneWayFromServer"))
		{
		retVal = ESmlOneWayFromServer;
	
		}
	else if(aBuf ==_L("ESmlOneWayFromClient"))
		{
		retVal = ESmlOneWayFromClient;
		}
		
	return retVal;;
}

//=====================================================
//		CNSmlProfileContentHandler::CheckXMLContentSetting()
//		
//		
//=====================================================
TBool CNSmlProfileContentHandler::CheckXMLContentSetting()
{

if (iContentArray->At(iContentCounter)->StrValue(EXMLDSServerDataSource).Compare(KEmpty) == 0
	|| (iContentArray->At(iContentCounter)->StrValue(EXMLDSServerId).Compare(KEmpty) == 0
		&&iContentArray->At(iContentCounter)->StrValue(EXMLDSProfileName).Compare(KEmpty) == 0)
	|| iContentArray->At(iContentCounter)->StrValue(EXMLDSAdapaterId).Compare(KEmpty) == 0)
	{
	return EFalse;
	}
	
if(iSyncTypeArray->At(iCounter - 1) != ESmlTwoWay && iSyncTypeArray->At(iCounter - 1) != ESmlOneWayFromServer
		&& iSyncTypeArray->At(iCounter - 1) != ESmlOneWayFromClient)
	{
	return EFalse;
	}
	
for (TInt index = 0; index < iCustomProfileArray->Count() ;index++ )
	{
	if (iContentArray->At(iContentCounter)->StrValue(EXMLDSProfileName).
					Compare(iCustomProfileArray->At(index)->StrValue(EDSProfileDisplayName)) == 0)
		{
		break;
		}
	else if (index == (iCustomProfileArray->Count() - 1) 
					&& iContentArray->At(iContentCounter)->StrValue(EXMLDSProfileName).Compare(KEmpty) != 0)
		{
		return EFalse;	
		}
	}
	
return ETrue;
}

//=====================================================
//		CNSmlProfileContentHandler::SetVisibility()
//		
//		
//=====================================================
void CNSmlProfileContentHandler::SetVisibility(TInt aValue)
{
	switch(aValue)
	{
		case 0:
			{
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileHidden, EFalse) ;
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileDeleteAllowed, ETrue) ;
			break;
			}
		case 1:
			{
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileHidden, EFalse) ;
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileDeleteAllowed, EFalse) ;
			iCustomProfileArray->At(iCounter)->InitVisibilityArray(KReadOnlyVisiblity);
			break;
			}
		case 2:
			{
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileHidden, ETrue) ;
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileDeleteAllowed, EFalse) ;
			break;
			}
		default:
		
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileHidden, EFalse) ;
			iCustomProfileArray->At(iCounter)->SetIntValue( EDSProfileDeleteAllowed, ETrue) ;
	}
	
}

//=====================================================
//		CNSmlProfileContentHandler::Comment()
//		
//		
//=====================================================
	
TInt CNSmlProfileContentHandler::Comment(TDesC& /*aComment*/)
{

	return 1;
}

//=====================================================
//		CNSmlProfileContentHandler::ProcessingInstructions()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::ProcessingInstructions(TDesC& /*aTarget*/, TDesC& /*aData*/)
{

	return 1;
}

//=====================================================
//		CNSmlProfileContentHandler::IgnoreWhiteSpace()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::IgnoreWhiteSpace(TDesC& /*aString*/)
{
	return 1;
}

//=====================================================
//		CNSmlProfileContentHandler::StartEntity()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::StartEntity(TDesC& /*aName*/)
{

	return 1;
}

//=====================================================
//		CNSmlProfileContentHandler::EndEntity()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::EndEntity(TDesC& /*aName*/)
{

	return 1;
}

//=====================================================
//	    CNSmlProfileContentHandler::SkippedEntity()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::SkippedEntity(TDesC& /*aName*/)
{
	return 1;
}

//=====================================================
//		CNSmlProfileContentHandler::Error()
//		
//		
//=====================================================
TInt CNSmlProfileContentHandler::Error(TInt /*aErrorCode*/, TInt /*aSeverity*/)
{
	return 1;
}



//  End of File  
