/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  XML file handler for DM-settings
*
*/



// INCLUDE FILES
#include <f32file.h>
#include <commdb.h>
#include <nsmldebug.h>
#include <featmgr.h>
#include "DMprofileContentHandler.h"
#include "nsmldmsettings.h"


#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))

// ============================ MEMBER FUNCTIONS ===============================


CDMProfileContentHandler::CDMProfileContentHandler()
	{
	}

// -----------------------------------------------------------------------------
// CDMProfileContentHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDMProfileContentHandler::ConstructL(CNSmlDMSettings* aSettings)
    {
	iSettings = aSettings;
	iProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDMResourceProfile> (3);	
	// init feature manager
	FeatureManager::InitializeLibL();
    }

// -----------------------------------------------------------------------------
// CDMProfileContentHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDMProfileContentHandler* CDMProfileContentHandler::NewL(CNSmlDMSettings* aSettings )
    {
		CDMProfileContentHandler* self = new( ELeave ) CDMProfileContentHandler;
    
		CleanupStack::PushL( self );
		self->ConstructL(aSettings);
		CleanupStack::Pop();

		return self;
    }

    
// Destructor
CDMProfileContentHandler::~CDMProfileContentHandler()
    {	
	if(iProfileArray)
		{
		iProfileArray->ResetAndDestroy();
		delete iProfileArray;			
		}
	// uninitialize feature manager
	FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------------
// void CDMProfileContentHandler::RemoveDuplicateEntry()
// Remove all the duplicate entries and the entries do not have
// the values for the mandatory fields.
// ----------------------------------------------------------
void CDMProfileContentHandler::RemoveDuplicateEntry()
{
	_DBG_FILE("CDMProfileContentHandler::RemoveDuplicateEntry(): begin");
	TInt count = iProfileArray->Count();
	for(TInt i = 0; i < count; i++)
	{
		if(!(iProfileArray->At(i)->iServerId &&
		    iProfileArray->At(i)->iUserName &&
		    iProfileArray->At(i)->iServerURL ))
		{
				iProfileArray->Delete(i);
				iProfileArray->Compress();				
				i--;
				count--;
		}
	}
	
	for ( TInt i = 0; i < count; i++ )
	{
		for(TInt j = i+1; j <count; j++)
		{
			if((iProfileArray->At(i)->iServerId->Des().Compare(iProfileArray->At(j)->iServerId->Des()) == 0) &&
			(iProfileArray->At(i)->iServerId->Length() == iProfileArray->At(j)->iServerId->Length()))
			{		
				iProfileArray->Delete(j);
				iProfileArray->Compress();
				j--;
				count--;
			}
		}
	}
	_DBG_FILE("CDMProfileContentHandler::RemoveDuplicateEntry(): end");
}

// ----------------------------------------------------------
// void CDMProfileContentHandler::RemoveRSCEntryL()
// ----------------------------------------------------------
void CDMProfileContentHandler::RemoveRSCEntryL()
{
	_DBG_FILE("CDMProfileContentHandler::RemoveRSCEntryL(): begin");
	TInt count = iProfileArray->Count();
	for ( TInt i = 0; i < count; i++ )
	{
		if((iSettings->ServerIdFoundL(iProfileArray->At(i)->iServerId->Des())) ||
		((! FeatureManager::FeatureSupported ( KFeatureIdSyncMlDmObex )) &&( iProfileArray->At(i)->iTransportId == KNSmlDmBluetoothType)))
		{
			iProfileArray->Delete(i);
			iProfileArray->Compress();
			i--;
			count--;			
		}				
	}
	_DBG_FILE("CDMProfileContentHandler::RemoveRSCEntryL(): end");
}

// ----------------------------------------------------------
// void CDMProfileContentHandler::GetAccessPointIdL()
// ----------------------------------------------------------
TBool CDMProfileContentHandler::GetAccessPointIdL(TInt count,TInt &iapId)
{
	_DBG_FILE("CDMProfileContentHandler::GetAccessPointIdL(): begin");

	CCommsDatabase *database = CCommsDatabase::NewL();
    TUint32 id;
    TBool ret = EFalse; 
    CleanupStack::PushL(database);
    CCommsDbTableView*  checkView;
    checkView = database->OpenViewMatchingTextLC(TPtrC(IAP),TPtrC(COMMDB_NAME), iProfileArray->At(count)->iAccessPoint->Des());
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
    {     
     //Get the IAP ID 
     checkView->ReadUintL(TPtrC(COMMDB_ID), id);
     iapId = id; 
     ret = ETrue;     
    }
         
    CleanupStack::PopAndDestroy(2);
    _DBG_FILE("CDMProfileContentHandler::GetAccessPointIdL(): end");

    return ret;
            
}
// ----------------------------------------------------------
// void CDMProfileContentHandler::SaveProfilesL()
// save the profiles into db.
// First it will remove the duplicate entry from iProfileArray itself and also remove
// the entries present in the RSC if any.
// ----------------------------------------------------------
void CDMProfileContentHandler::SaveProfilesL()
{
	_DBG_FILE("CDMProfileContentHandler::SaveProfilesL(): begin");
	RemoveDuplicateEntry();
	RemoveRSCEntryL();
	TInt iapId = 0;
	
	for ( TInt i = 0; i < iProfileArray->Count(); i++ )
	{
		if(iProfileArray->At(i)->iAccessPoint)
		{
			if(GetAccessPointIdL(i, iapId))
			{
				iProfileArray->At(i)->iIapId = iapId;
			}
			else
			{
			iapId = KErrGeneral;
			TLex id(iProfileArray->At(i)->iAccessPoint->Des());
			id.Val(iapId);
			if( iapId == KErrGeneral || iapId == KErrNotFound )
			    {
			    iProfileArray->At(i)->iIapId = iapId;
			    }
			}
		}
		if(! iProfileArray->At(i)->iProfileDisplayName)
		{
			TBuf<KNSmlDmProfileNameMaxLength> defaultProfile;
			defaultProfile.Format( KNSmlDMDefaultProfile );
			iProfileArray->At(i)->iProfileDisplayName = defaultProfile.AllocL();
		}	
		CNSmlDMProfile* profile = iSettings->CreateProfileL();
		CleanupStack::PushL( profile );
		iProfileArray->At(i)->SaveProfileL( profile );
		profile->SaveL();
		CleanupStack::PopAndDestroy(); // profile
							
	}
	_DBG_FILE("CDMProfileContentHandler::SaveProfilesL(): end");
}
// -----------------------------------------------------------------------------
//  CDMProfileContentHandler::IsChar
//	Returns true value for other than numerical character.
// -----------------------------------------------------------------------------
TBool CDMProfileContentHandler::IsChar( const TUint8 aChar )
{
	TBool ret(ETrue);
	switch(aChar)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			ret = EFalse;
			break;
		}
		default: ret = ETrue;		
	}
	return ret;    
}

TInt CDMProfileContentHandler::StartDocument()
   {	
		TInt error = KNSmlDmSettingsSuccess;
		iFile.Close();
		iSession.Close();
		error = iSession.Connect();
		error =iFile.Replace(iSession, iFilename, EFileWrite);
		
		return error;
   }

TInt CDMProfileContentHandler::EndDocument()
{
		iFile.Close();
		iSession.Close();
		return KNSmlDmSettingsSuccess;
}

// ----------------------------------------------------------
// void CDMProfileContentHandler::AddProfileL()
// Appends a new profile to the array.
// ----------------------------------------------------------
void CDMProfileContentHandler::AddProfileL( )
{
	_DBG_FILE("CDMProfileContentHandler::AddProfileL(): begin");
	CNSmlDMResourceProfile* profile = CNSmlDMResourceProfile::NewLC();
	iProfileArray->AppendL( profile );
	CleanupStack::Pop();
	_DBG_FILE("CDMProfileContentHandler::AddProfileL(): end");		
}

TInt CDMProfileContentHandler::StartElement(TDesC& /* aURI */, TDesC& /* aLocalName */, TDesC& aName, MXMLAttributes * /* aAttributeList*/)
{	

		if (! aName.Compare(KNSmlDMXmlFactorySettings()))
		{
			TRAPD(leavecode, AddProfileL());
			if(leavecode != KErrNone )
			{
				return KNSmlDmSettingsFail;
			}						
			iDMAccountInProgress = ETrue;
			iCounter++;
		}
		
		if (!iDMAccountInProgress)
			return KNSmlDmSettingsSuccess;
		
		if (! aName.Compare(KNSmlDMXmlProfileName()))
		{
			iState = EProfileName;			
		}	
		
		else if (! aName.Compare(KNSmlDMXmlServerId()))
		{
			iState = EServerID;			
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileServerPassword()))
		{
			iState = EServerPassword;
		}

		else if (! aName.Compare(KNSmlDMXmlProfileUsername()))
		{
			iState = EUserName;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileClientPassword()))
		{
			iState = EClientPwd;
		}

		else if (! aName.Compare(KNSmlDMXmlProfileServerUrl()))
		{
			iState = EServerURL;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileTransportId()))
		{
			iState = ETransportID;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileAP()))
		{
			iState = EAccessPoint;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileServerAlertedAction()))
		{
			iState = EServerAlertedAction;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileAuthentication()))
		{
			iState = EAuthentication;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileDeleteAllowed()))
		{
			iState = EDeleteAllowed;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileView()))
		{
			iState = EProfileView;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileNetworkAuthentication()))
		{
			iState = ENetworkAuthentication;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileHttpAuthUsername()))
		{
			iState = EHTTPUserName;
		}
		
		else if (! aName.Compare(KNSmlDMXmlProfileHttpAuthPassword()))
		{
			iState = EHTTPPwd;
		}				
		
		else if (! aName.Compare(KNSmlDMXmlProfileLock()))
		{
			iState = EProfileLock;
		}

		return KNSmlDmSettingsSuccess;
}

TInt CDMProfileContentHandler::EndElement(TDesC& /* aURI */, TDesC& /* aLocalName */, TDesC& aName)
{
		if (!iDMAccountInProgress)
			return KNSmlDmSettingsSuccess;
		
		if (! aName.Compare(KNSmlDMXmlFactorySettings()))
		{
	    	iDMAccountInProgress = EFalse;
		}
		
		if (iState != EStateHandled)
		{				
			iState = EStateHandled;   //Assume that empty feild. No need to set.
		}		

		return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::StartCDATA()
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::EndCDATA()
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}

// ----------------------------------------------------------
// void CDMProfileContentHandler::AddProfileElementL()
// Assigns profile element value.
// ----------------------------------------------------------
void CDMProfileContentHandler::AddProfileElementL(TDesC& aBuf)
{
	_DBG_FILE("CDMProfileContentHandler::AddProfileElementL(): begin");
	if(iState == EProfileName)
	{
		if(aBuf.Length() > KNSmlDmProfileNameMaxLength)
		{
			TPtrC buf = aBuf.Left(KNSmlDmProfileNameMaxLength);
			iProfileArray->At(iCounter-1)->iProfileDisplayName = buf.AllocL();
		}
		else		
			iProfileArray->At(iCounter-1)->iProfileDisplayName = aBuf.AllocL();		
		iState = EStateHandled;		
	}
	
	else if(iState == EServerID)
	{
		iProfileArray->At(iCounter-1)->iServerId = aBuf.AllocL();		
		iState = EStateHandled;		
	}
	
	else if(iState == EServerPassword)
	{		
		iProfileArray->At(iCounter-1)->iPassWord = aBuf.AllocL();
		iState = EStateHandled;		
	}
	
	else if(iState == EUserName)
	{		
		iProfileArray->At(iCounter-1)->iUserName = aBuf.AllocL();
		iState = EStateHandled;		
	}
	
	else if(iState == EClientPwd)
	{		
		iProfileArray->At(iCounter-1)->iClientPassword = aBuf.AllocL();
		iState = EStateHandled;		
	}
	
	else if(iState == EServerURL)
	{		
		TInt len = aBuf.Length();
		TInt locate(0);
		if( aBuf.Find( KNSmlDMSettingsHTTP )==0 )//http:
		{
			locate = KNSmlDMSettingsHTTP().Length();
		}
		else if( aBuf.Find( KNSmlDMSettingsHTTPS )==0 )//https:
		{
			locate = KNSmlDMSettingsHTTPS().Length();
		}
		
		TPtrC buf =aBuf.Right(len-locate);
		TInt locateColon = buf.Locate(KNSmlDMColon); // ':'
		if(locateColon > 0)
		{
			TInt bufLen = buf.Length();	
			TPtrC buf1 = buf.Right(bufLen - locateColon);
			TInt digit(0);
			for( TInt i = locate + locateColon + 1; i < len; i++ )
			{			
				if (IsChar(aBuf[i]))
				{
					if(aBuf[i]!= KNSmlDMUriSeparator )
					{
						TPtrC strBuf = aBuf.Right(len - locate - locateColon);
						TInt locateSlash = strBuf.Locate(KNSmlDMUriSeparator);
						if(locateSlash > 0)
						{
							TPtrC ptrBuf1 = aBuf.Left(len-locateColon-1);
							TBuf<KNSmlDmFileNameLength> buffer;
							buffer.Append(ptrBuf1);
							TPtrC ptrBuf2 = aBuf.Right(len-locate-locateColon-locateSlash);
							buffer.Append(ptrBuf2);
							iProfileArray->At(iCounter-1)->iServerURL = buffer.AllocL();
							break;
						}
						else
						{
							TPtrC ptrBuf = aBuf.Left(locate + locateColon);
							iProfileArray->At(iCounter-1)->iServerURL = ptrBuf.AllocL();
							break;
						}
					}
					else
					{
						iProfileArray->At(iCounter-1)->iServerURL = aBuf.AllocL();
						break;
					}
				}
				if(digit>= KNSmlDmMaxPortLength) // the max length of the port number
				{
					TPtrC ptrBuf1 = aBuf.Left(locate + locateColon);
					TBuf<KNSmlDmFileNameLength> buffer;
					buffer.Append(ptrBuf1);
					TPtrC ptrBuf2 = aBuf.Right(len-i);
					TInt locateSlash = ptrBuf2.Locate(KNSmlDMUriSeparator);
					if(locateSlash > 0)
					{
						TPtrC ptrBuf3 = aBuf.Right(len-i-locateSlash);
						buffer.Append(ptrBuf3);							
					}						
					iProfileArray->At(iCounter-1)->iServerURL = buffer.AllocL();	
					break;
				}
				else if(i == (len-1))					
					iProfileArray->At(iCounter-1)->iServerURL = aBuf.AllocL();					
		
				digit++;			
			}		
		}
		else
		{
			iProfileArray->At(iCounter-1)->iServerURL = aBuf.AllocL();
		}
		iState = EStateHandled;		
	}
	else
	{
	AddRemainingElementsL(aBuf);	//To decrease cyclomatic complexity
	}	
	_DBG_FILE("CDMProfileContentHandler::AddProfileElementL(): end");
}

TInt CDMProfileContentHandler::Charecters(TDesC& aBuf, TInt /* aStart */, TInt /* aLength */)
{	

	if (!iDMAccountInProgress)
		return KNSmlDmSettingsSuccess;
	
	TRAPD(leavecode, AddProfileElementL(aBuf));
	if(leavecode != KErrNone )
	{
		return KNSmlDmSettingsFail;
	}		
	return KNSmlDmSettingsSuccess;
}

TInt CDMProfileContentHandler::Comment(TDesC& /* aComment */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}

TInt CDMProfileContentHandler::ProcessingInstructions(TDesC& /* aTarget */, TDesC& /* aData */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::IgnoreWhiteSpace(TDesC& /* aString */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::StartEntity(TDesC& /* aName */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::EndEntity(TDesC& /* aName */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::SkippedEntity(TDesC& /* aName */)
{
	// do nothing
	return KNSmlDmSettingsSuccess;
}
TInt CDMProfileContentHandler::Error(TInt /* aErrorCode */, TInt /* aSeverity */)
{
	return KNSmlDmSettingsSuccess;
}

// ----------------------------------------------------------
// void CDMProfileContentHandler::AddRemainingElementsL()
// Assigns remaining profile element value other than in 
// AddProfileElementL method
// ----------------------------------------------------------
void CDMProfileContentHandler::AddRemainingElementsL(TDesC& aBuf)
	{
	_DBG_FILE("CDMProfileContentHandler::AddRemainingElementsL(): end");
	if(iState == ETransportID)
		{	
		TInt val(0);
		if(! aBuf.Compare(KNSmlDMInternetTypeString()))
			val = KNSmlDmInternetType;
		else if (! aBuf.Compare(KNSmlDMBluetoothTypeString()))
			val = KNSmlDmBluetoothType;		
		iProfileArray->At(iCounter-1)->iTransportId = val;
		iState = EStateHandled;		
		}

	else if(iState == EAccessPoint)
		{		
		iProfileArray->At(iCounter-1)->iAccessPoint = aBuf.AllocL();
		iState = EStateHandled;		
		}

	else if(iState == EServerAlertedAction)
		{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iServerAlertAction = val;
		iState = EStateHandled;		
		}

	else if(iState == EAuthentication)
		{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iAuthenticationRequired = val;
		iState = EStateHandled;		
		}

	else if(iState == EDeleteAllowed)
		{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iDeleteAllowed = val;
		iState = EStateHandled;		
		}
    else if(iState == EProfileLock)
     	{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iProfileLock = val;
		iState = EStateHandled;		
	    }
	else if(iState == EProfileView)
		{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iProfileHidden = val;
		iState = EStateHandled;		
		}

	else if(iState == ENetworkAuthentication)
		{		
		TLex var(aBuf);
		TInt val;
		var.Val(val);
		iProfileArray->At(iCounter-1)->iHttpAuthUsed = val;
		iState = EStateHandled;		
		}

	else if(iState == EHTTPUserName)
		{		
		iProfileArray->At(iCounter-1)->iHttpAuthUsername = aBuf.AllocL();
		iState = EStateHandled;		
		}

	else if(iState == EHTTPPwd)
		{		
		iProfileArray->At(iCounter-1)->iHttpAuthPassword = aBuf.AllocL();
		iState = EStateHandled;		
		}
	_DBG_FILE("CDMProfileContentHandler::AddRemainingElementsL(): end");
	}
//  End of File  
