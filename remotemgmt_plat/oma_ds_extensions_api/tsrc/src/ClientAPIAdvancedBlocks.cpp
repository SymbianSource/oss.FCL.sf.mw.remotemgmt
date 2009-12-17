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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ClientAPIAdvanced.h"

//SyncML headers
#include <SyncMLDef.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLTransportProperties.h>
#include <commdb.h> 

#include "nsmlconstants.h"

// phonebook  header files
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntview.h>
#include <cpbkcontactengine.h>
#include <cpbkcontactitem.h> 
#include <cpbkfieldinfo.h> 
#include <cpbkfieldsinfo.h> 
#include <pbkfields.hrh>
//phonebook header files 


const TInt TPropertyIntenetAccessPoint = 0;
const TBool	KTaskEnabled = ETrue;
const TInt KBufSize32 = 50;
static const TInt KPhonebookFieldLength = 128;


// ============================= LOCAL FUNCTIONS ===============================


//------------------------------------------------------------------------------
//gets the Id corresponding to the Access point passed
//-------------------------------------------------------------------------------
//		
	
TInt AccessPointIdL(TDesC8& aBuf)
{
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
           	
    CleanupStack::PopAndDestroy(2);    
    return retVal;     
}
//------------------------------------------------------------------------------
//gets the connection property name
//-------------------------------------------------------------------------------
//	
void GetConnectionPropertyNameL(RSyncMLSession &aSyncsession,TDes8& aText, TInt aPropertyPos)
	{
	//
	// at the moment RSyncMLTransport is only needed for internet connection settings
	//
	RSyncMLTransport transport;
	transport.OpenL(aSyncsession, KUidNSmlMediumTypeInternet.iUid);  // no ICP call
	CleanupClosePushL(transport);
	
	const CSyncMLTransportPropertiesArray&  arr = transport.Properties();
	const TSyncMLTransportPropertyInfo& info = arr.At(aPropertyPos);
	aText = info.iName;
	
	CleanupStack::PopAndDestroy(&transport);
	}	
	
//------------------------------------------------------------------------------
//copies the string
//-------------------------------------------------------------------------------
//
void StrCopy(TDes8& aTarget, const TDesC& aSource)
    {
		TInt len = aTarget.MaxLength();
	    if(len < aSource.Length()) 
		    {
			aTarget.Copy(aSource.Left(len));
			return;
		    }
		aTarget.Copy(aSource);
    }
    
//------------------------------------------------------------------------------
//copies the string
//-------------------------------------------------------------------------------
//
	
void StrCopy(TDes& aTarget, const TDesC& aSource)
    {
		TInt len = aTarget.MaxLength();
	    if(len < aSource.Length()) 
		    {
			aTarget.Copy(aSource.Left(len));
			return;
		    }
		aTarget.Copy(aSource);
    }
    
//------------------------------------------------------------------------------
//copies the string
//-------------------------------------------------------------------------------
//    
    
 HBufC* StrConversion(const TDesC& aDefaultText)
{
	return aDefaultText.AllocLC();
}
//============================code for creating the contacts,deleting contacts and updating contacts

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhonebookParameters::CPhonebookParameters
//  for inititalizing the data members
// Called from constructor. 
// -----------------------------------------------------------------------------
//

CPhonebookParameters::CPhonebookParameters()
    {
    
	    iContactFirstName = HBufC::New(KPhonebookFieldLength);
	    iContactLastName = HBufC::New(KPhonebookFieldLength);
	    iContactCompanyName = HBufC::New(KPhonebookFieldLength);
	    iContactJobTitle = HBufC::New(KPhonebookFieldLength);
	    iContactPhoneNumberGeneral = HBufC::New(KPhonebookFieldLength);
	    iContactURL = HBufC::New(KPhonebookFieldLength);
	    iContactEmailAddress = HBufC::New(KPhonebookFieldLength);
	    iContactSyncType = HBufC::New(KPhonebookFieldLength);
	    // set defaults
	    iNumberOfPhoneNumberFields = 1;
	    iNumberOfURLFields = 1;
	    iNumberOfEmailAddressFields = 1;
    
    }
    
// -----------------------------------------------------------------------------
// CPhonebookParameters::CPhonebookParameters
//  
// Called from destructor. 
// -----------------------------------------------------------------------------
//    

CPhonebookParameters::~CPhonebookParameters()
{
    delete iContactEmailAddress;
    delete iContactURL;
    delete iContactPhoneNumberGeneral;
    delete iContactJobTitle;
    delete iContactCompanyName;
    delete iContactLastName;
    delete iContactFirstName;
}



// -----------------------------------------------------------------------------
// CClientAPIAdvanced::AddFieldToContactItemL
// Adds field data into the newly created contact
// called from CreateContactL
// -----------------------------------------------------------------------------

void CClientAPIAdvanced::AddFieldToContactItemL(CPbkContactItem& aItem, TPbkFieldId aFieldId, const TDesC& aFieldText)
{
    
    CPbkFieldInfo* fieldInfo = iContactDBEngine->FieldsInfo().Find( aFieldId );
    if( fieldInfo ) // field was found
        {
        // add the field and value to contact
        TPbkContactItemField* itemField = aItem.AddOrReturnUnusedFieldL( *fieldInfo );

        if( itemField && aFieldText.Length() > 0 ) // add text also
            {
            if( itemField->ItemField().StorageType() == KStorageTypeText )
                {
		        CContactTextField* textField = itemField->ItemField().TextStorage();
                
                // clip the text if it doesn't fit to the field
	            textField->SetTextL( aFieldText.Left( fieldInfo->MaxLength() ) );
	    	    }
    		}
        }
                
 }
 
// -----------------------------------------------------------------------------
// CClientAPIAdvanced::CreateContactL
// Opens the contact db and enters the new contact into the contact db.
// 
// -----------------------------------------------------------------------------

 TInt CClientAPIAdvanced::CreateContactL(CStifItemParser& aItem)
{
    CPbkContactItem* newContact = iContactDBEngine->CreateEmptyContactL();
    CleanupStack::PushL(newContact);
    TInt index;
    aItem.GetNextInt ( index );
    CPhonebookParameters *iParameters = new(ELeave) CPhonebookParameters;
    if (iParameters)
    {
       if(index==1)
       {
       	iParameters->iContactFirstName->Des().Copy(_L("FirstName1"));
       // iParameters->iContactLastName->Des().Copy(_L("LastName1"));
        iParameters->iContactCompanyName->Des().Copy(_L("NOKIA"));
        iParameters->iContactJobTitle->Des().Copy(_L("Manager"));
        iParameters->iContactPhoneNumberGeneral->Des().Copy(_L("99111"));
        iParameters->iContactURL->Des().Copy( _L("http://www.") );
        iParameters->iContactURL->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactURL->Des().Append( _L(".com/") );
        iParameters->iContactEmailAddress->Des().Copy( iParameters->iContactFirstName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L("@") );
        iParameters->iContactEmailAddress->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L(".com") );
        iParameters->iContactSyncType->Des().Append( _L("public") );
       }
       if(index==2)
       {
       	 iParameters->iContactFirstName->Des().Copy(_L("SecondName1"));
        //iParameters->iContactLastName->Des().Copy(_L("LastName2"));
        iParameters->iContactCompanyName->Des().Copy(_L("ABC"));
        iParameters->iContactJobTitle->Des().Copy(_L("Manager"));
        iParameters->iContactPhoneNumberGeneral->Des().Copy(_L("99222"));
        iParameters->iContactURL->Des().Copy( _L("http://www.") );
        iParameters->iContactURL->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactURL->Des().Append( _L(".com/") );
        iParameters->iContactEmailAddress->Des().Copy( iParameters->iContactFirstName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L("@") );
        iParameters->iContactEmailAddress->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L(".com") );
        iParameters->iContactSyncType->Des().Append( _L("private") );
       }
       if(index==3)
       {
       	iParameters->iContactFirstName->Des().Copy(_L("ThirdName1"));
       // iParameters->iContactLastName->Des().Copy(_L("LastName3"));
        iParameters->iContactCompanyName->Des().Copy(_L("XYZ"));
        iParameters->iContactJobTitle->Des().Copy(_L("Manager"));
        iParameters->iContactPhoneNumberGeneral->Des().Copy(_L("99333"));
        iParameters->iContactURL->Des().Copy( _L("http://www.") );
        iParameters->iContactURL->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactURL->Des().Append( _L(".com/") );
        iParameters->iContactEmailAddress->Des().Copy( iParameters->iContactFirstName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L("@") );
        iParameters->iContactEmailAddress->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L(".com") );
        iParameters->iContactSyncType->Des().Append( _L("public") );
       }
    	if(index==4)
       {
       	iParameters->iContactFirstName->Des().Copy(_L("FourthName"));
       // iParameters->iContactLastName->Des().Copy(_L("LastName3"));
        iParameters->iContactCompanyName->Des().Copy(_L("UVW"));
        iParameters->iContactJobTitle->Des().Copy(_L("Developer"));
        iParameters->iContactPhoneNumberGeneral->Des().Copy(_L("99801267"));
        iParameters->iContactURL->Des().Copy( _L("http://www.") );
        iParameters->iContactURL->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactURL->Des().Append( _L(".com/") );
        iParameters->iContactEmailAddress->Des().Copy( iParameters->iContactFirstName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L("@") );
        iParameters->iContactEmailAddress->Des().Append( iParameters->iContactCompanyName->Des() );
        iParameters->iContactEmailAddress->Des().Append( _L(".com") );
        iParameters->iContactSyncType->Des().Append( _L("public") );
       }
      
    }
       
    AddFieldToContactItemL (*newContact, EPbkFieldIdFirstName, iParameters->iContactFirstName->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdLastName, iParameters->iContactLastName->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdCompanyName, iParameters->iContactCompanyName->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdJobTitle, iParameters->iContactJobTitle->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdPhoneNumberGeneral, iParameters->iContactPhoneNumberGeneral->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdURL, iParameters->iContactURL->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdEmailAddress, iParameters->iContactEmailAddress->Des());
    AddFieldToContactItemL (*newContact, EPbkFieldIdSyncronization, iParameters->iContactSyncType->Des());
     
    
    // add the contact item to the database
   	iContactDBEngine->AddNewContactL(*newContact);
	
	
	delete iParameters;
	CleanupStack::PopAndDestroy(); // newContact
    return(KErrNone);
}


// -----------------------------------------------------------------------------
// CClientAPIAdvanced::DeleteContactL
// Opens the contact db and deletes the contact entry from based on the serach criteria first name
// 
// -----------------------------------------------------------------------------


TInt CClientAPIAdvanced::DeleteContactL(CStifItemParser& aItem)
{
	TInt index;
	aItem.GetNextInt(index);
    
    CPbkFieldIdArray* searchFields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(searchFields);

    searchFields->AppendL( EPbkFieldIdFirstName );
     CContactIdArray* foundContacts;
    if(index==1)
    {
     _LIT(KFindText1, "FirstName1");
     foundContacts = iContactDBEngine->FindLC(KFindText1,searchFields);	
    }
       
    if(index==2)
    {
      _LIT(KFindText2, "SecondName1");
      foundContacts = iContactDBEngine->FindLC(KFindText2,searchFields);	
    }
    if(index==3)
    {
      _LIT(KFindText3, "ThirdName1");
       foundContacts = iContactDBEngine->FindLC(KFindText3,searchFields);    
    	
    }   
    if(index==4)
    {
      _LIT(KFindText3, "FourthName");
       foundContacts = iContactDBEngine->FindLC(KFindText3,searchFields);    
    	
    }   
    TContactItemId found;
    for(TInt i=0;i<foundContacts->Count();i++)
    {
        found=foundContacts->operator[](i);
        iContactDBEngine->DeleteContactL(found, ETrue);
    }
    
    CleanupStack::PopAndDestroy(foundContacts );
    CleanupStack::PopAndDestroy(searchFields);
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CClientAPIAdvanced::UpdateContactL
// Opens the contact db and updates the contact entry from based on the serach criteria first name
// 
// -----------------------------------------------------------------------------

TInt CClientAPIAdvanced::UpdateContactL(CStifItemParser& aItem)
{
	TInt index;
	aItem.GetNextInt(index);
    
    CPbkFieldIdArray* searchFields = new (ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(searchFields);

    searchFields->AppendL( EPbkFieldIdFirstName );
    
    CContactIdArray* foundContacts;
    if(index==1)
    {
     _LIT(KFindText1, "FirstName1");
     foundContacts = iContactDBEngine->FindLC(KFindText1,searchFields);	
    }
      
    if(index==2)
    {
      _LIT(KFindText2, "SecondName1");
      foundContacts = iContactDBEngine->FindLC(KFindText2,searchFields);	
    }
    if(index==3)
    {
      _LIT(KFindText3, "ThirdName1");
       foundContacts = iContactDBEngine->FindLC(KFindText3,searchFields);    
    
    }  
    if(index==4)
    {
      _LIT(KFindText3, "FourthName");
       foundContacts = iContactDBEngine->FindLC(KFindText3,searchFields);    
    
    }   
    TContactItemId found;
    for(TInt i=0;i<foundContacts->Count();i++)
    {
         found=foundContacts->operator[](i);
         CPbkContactItem* item=iContactDBEngine->OpenContactLCX(found);
         CPhonebookParameters *iParameters = new(ELeave) CPhonebookParameters;
         if (iParameters)
         {
         	iParameters->iContactLastName->Des().Copy(_L("LastName"));
         	AddFieldToContactItemL (*item, EPbkFieldIdLastName, iParameters->iContactLastName->Des());
         	iContactDBEngine->CommitContactL(*item);
         	        	
         }
         delete iParameters;
         CleanupStack::PopAndDestroy(2);
    }
    
    CleanupStack::PopAndDestroy(2);
   	return KErrNone;
}
 
//============================code for creating the contacts,deleting contacts and updating contacts 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClientAPIAdvanced::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CClientAPIAdvanced::Delete() 
    {
    iProfiles.Close();
    iTransports.Close();
    iTasks.Close();
    }
    
//------------------------------------------------------------------------------
//CClientAPIAdvanced::OpenSyncSessionL
//opens the syncml session
//-------------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::OpenSyncSessionL( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.OpenL();
	iSyncSession.ShareAuto(); // share automatically with other threads (notifiers)
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSProfileL
//creates the Ds profile
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::CreateDSProfileL( CStifItemParser& aItem )
	{
	TInt protocol = 0;
	aItem.GetNextInt( protocol );
	
	iDSProfile.CreateL( iSyncSession );
	iDSProfile.SetProtocolVersionL((TSmlProtocolVersion) protocol );
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSProfileL
//sets the values for the remote ds profile
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::SetRemoteDSProfileDataL( CStifItemParser& aItem )
	{
	
	 TInt creatorId;
	 aItem.GetNextInt( creatorId );
	 iDSProfile.SetCreatorId( creatorId );
	
	 TPtrC profileName;
	 aItem.GetNextString( profileName );
	 iDSProfile.SetDisplayNameL( profileName);
	 
	 TPtrC paramName;
	 TBuf8<KBufSize32> userName;
	 aItem.GetNextString( paramName );
	 StrCopy(userName,paramName);
	 iDSProfile.SetUserNameL( userName );
	 
	 
	 TPtrC paramPasswd;
	 TBuf8<KBufSize32> password;
	 aItem.GetNextString(paramPasswd);
	 StrCopy(password,paramPasswd);
	 iDSProfile.SetPasswordL( password );
	 
	 
	 TPtrC paramId;
	 TBuf8<KBufSize32> serverId;
	 aItem.GetNextString(paramId);
	 StrCopy(serverId,paramId);
     iDSProfile.SetServerIdL( serverId );
     
     
     TInt sanInteraction;
     aItem.GetNextInt( sanInteraction );
	 iDSProfile.SetSanUserInteractionL(static_cast<TSmlServerAlertedAction>(sanInteraction));
	 
	 TInt protocol;
	 aItem.GetNextInt( protocol );
	 iDSProfile.SetProtocolVersionL( static_cast<TSmlProtocolVersion>(protocol));
	 
	 return KErrNone;
	 
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::UpdateDSProfileL
//updates the ds profile with new values
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::UpdateDSProfileL( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.UpdateL();
	iProfileId = iDSProfile.Identifier();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::SetConnectionProfileL
//
//-------------------------------------------------------------------------------
//	
void CClientAPIAdvanced::SetConnectionProfileL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	if ( type == ESmlDataSync ) 
		iConnectionProfile = &iDSProfile;
	else 
		User::Leave( KErrArgument );
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::OpenConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//		
	
TInt CClientAPIAdvanced::OpenConnectionL( CStifItemParser& aItem )
	{
	SetConnectionProfileL( aItem );
	
	RArray<TSmlTransportId> conns;
	iConnectionProfile->ListConnectionsL( conns );
	CleanupClosePushL( conns );
	
	iConnectionId = conns[0];
	iConnection.OpenL( *iConnectionProfile, iConnectionId );

	
	CleanupStack::PopAndDestroy(); // conns
	
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//	

TInt CClientAPIAdvanced::CreateConnectionL( CStifItemParser& aItem )
	{
	
	TInt aIapId=0;
	
	TInt id = 0;
	aItem.GetNextInt( id );
	iConnectionId = id;
	iConnection.CreateL( *iConnectionProfile, id );
	
	TPtrC paramName;
    TBuf8<KBufSize32> serverName;
    aItem.GetNextString( paramName );
    StrCopy(serverName,paramName);
    iConnection.SetServerURIL(serverName);
   //code for access point selection
   //comment from here for emulator environment.
   
   #if defined( __WINS__ )
       	return KErrNone;
   #else
         //assuming that in cfg file acess point name is 
         //set as xxxx$yyyy -----> xxxx yyyyy
		 TBuf<KBufSize32> buf;
		 TBuf8<KBufSize32> key;
		 TBuf8<KBufSize32> value;
		 TBuf8<KBufSize32> value2;
		 TBuf8<KBufSize32> value3;
		 _LIT(KDollar,"$");
		 TBuf<10> trial;
		 trial.Copy(_L(" "));
		 
		 TPtrC string;
		 aItem.GetNextString ( string );
		 TInt pos=string.Find(KDollar);
		 StrCopy(value2,string);
		 StrCopy(value3,trial);
		 value2.Replace(pos,1,value3);
		 aIapId=AccessPointIdL(value2);
    	 buf.Num(aIapId);
	     StrCopy(value, buf);
	   	 GetConnectionPropertyNameL(iSyncSession,key, TPropertyIntenetAccessPoint);
		 iConnection.SetPropertyL(key, value);
		 return KErrNone;
   #endif
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::UpdateConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::UpdateConnectionL( CStifItemParser& /*aItem*/ )
	{
	iConnection.UpdateL();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateTaskL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::CreateTaskL( CStifItemParser& aItem )
	{
	TInt dataProvider = 0;
	aItem.GetNextInt( dataProvider );

	 if ( dataProvider == 0 ) // not given as parameter, get from session
		{
		RArray<TSmlDataProviderId> adapters;
		CleanupClosePushL( adapters );
		iSyncSession.ListDataProvidersL( adapters );
		dataProvider = adapters[0];
		CleanupStack::PopAndDestroy(); // adapters
		}
	
	 iDataProviderId = dataProvider;
	
	 TPtrC serverDatastore;
	 TPtrC clientDatastore;
	 aItem.GetNextString( serverDatastore );
	 aItem.GetNextString( clientDatastore );
	 iTask.CreateL( iDSProfile, dataProvider, serverDatastore, clientDatastore );
	 
	 return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::SetTaskDataL
//opens the connection
//-------------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::SetTaskDataL( CStifItemParser& aItem )
	{
	TInt creatorId;
	aItem.GetNextInt ( creatorId );
    iTask.SetCreatorId( static_cast<TSmlCreatorId>( creatorId ));
        
    TInt syncType;
    aItem.GetNextInt(syncType);
    iTask.SetDefaultSyncTypeL(static_cast<TSmlSyncType>( syncType ));
    
    iTask.SetEnabledL( KTaskEnabled );
 
 	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::UpdateTaskL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::UpdateTaskL( CStifItemParser& /*aItem*/ )
	{
	iTask.UpdateL();
	iTaskId = iTask.Identifier();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CloseTask
//opens the connection
//-------------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::CloseTask( CStifItemParser& /*aItem*/ )
	{
	iTask.Close();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CloseDSProfile
//opens the connection
//-------------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::CloseDSProfile( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.Close();
	return KErrNone;
	}

//------------------------------------------------------------------------------
//CClientAPIAdvanced::CloseSyncSession
//opens the connection
//-------------------------------------------------------------------------------
//		
TInt CClientAPIAdvanced::CloseSyncSession( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.Close();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CloseDSJob
//opens the connection
//-------------------------------------------------------------------------------
//		
TInt CClientAPIAdvanced::CloseDSJob( CStifItemParser& /*aItem*/ )
	{
	iDSJob.Close();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CloseDSJob
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::CloseConnection( CStifItemParser& /*aItem*/ )
	{
	iConnection.Close();
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::OpenDSProfileByNameL
//opens the profile based on the id 
//-------------------------------------------------------------------------------
//	
TSmlProfileId CClientAPIAdvanced::OpenDSProfileByNameL( TSmlProfileId aProfileId,TDesC& aProfilename )
	{
	RSyncMLDataSyncProfile prof;
	prof.OpenL( iSyncSession, aProfileId, ESmlOpenReadWrite );
    HBufC* hBuf=StrConversion(prof.DisplayName());
    TPtrC ptr=hBuf->Des();
    if(ptr.Compare(aProfilename) == 0	)
	 {     
		prof.Close();
		CleanupStack::PopAndDestroy(hBuf);
		return(aProfileId);
	 }
	prof.Close();
	CleanupStack::PopAndDestroy(hBuf);
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::ListProfilesL
//list all the profiles and stores the profil-id in profiles list
//-------------------------------------------------------------------------------
//	
	
void CClientAPIAdvanced::ListProfilesL( TSmlUsageType type )
	{
	TRAPD( err, iSyncSession.ListProfilesL( iProfiles, type ) );
	if ( err == KErrNotSupported ) // ds sync not supported, use dummy values
		{
		iProfiles.Append( 1 );
		}
	}
	

//------------------------------------------------------------------------------
//CClientAPIAdvanced::OpenDSProfileByIdL
//opens the connection
//-------------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::OpenDSProfileByIdL( CStifItemParser& aItem )
	{
	TInt id = -2;
	aItem.GetNextInt( id );
	ListProfilesL( ESmlDataSync );
	
	TPtrC profileName;
	aItem.GetNextString( profileName );

	if ( id == -2 )
		{
		iDSProfile.OpenL( iSyncSession, iProfileId, ESmlOpenReadWrite );
		}
	else if (id==-1)
	    {
	     TSmlProfileId profileId;
	     TInt i;
	     for (i=0;i<iProfiles.Count() ;++i)
	     {
	      	
	       profileId=OpenDSProfileByNameL(iProfiles[i],profileName);
	      if(profileId!=KErrNone)
	       {
	      	iDSProfile.OpenL( iSyncSession, profileId, ESmlOpenReadWrite );
	      	break;
	       }
	
	      }
	     
		}
	else
		{
		iDSProfile.OpenL( iSyncSession, id, ESmlOpenReadWrite );
		}
	
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSJobForProfileL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::CreateDSJobForProfileL( CStifItemParser& aItem )
{
	TInt id = -2;
	aItem.GetNextInt( id );
	ListProfilesL( ESmlDataSync );
	
	TPtrC profileName;
	aItem.GetNextString( profileName );

	if ( id == -2 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0] );
		iJobId = iDSJob.Identifier();
		}
	else if(id==-1)
		{
				 
		  	TInt i=0;		
			for (i=0;i<iProfiles.Count() ;++i)
			{
			  TSmlProfileId profileId;
		   	  profileId=OpenDSProfileByNameL(iProfiles[i],profileName);
		   	  if(profileId!=KErrNone)
		   	  {
 				iDSJob.CreateL( iSyncSession, iProfiles[i]);
		  	    iJobId = iDSJob.Identifier();
		  	    break;
		   	  }
		   	     
		  	}
		}
	else
		{
			iDSJob.CreateL( iSyncSession, iProfileId);
	  		iJobId = iDSJob.Identifier();
		}
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::OpenTaskL
//opens the task
//-------------------------------------------------------------------------------
//	
TInt CClientAPIAdvanced::OpenTaskL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlTaskId> tasks;
	CleanupClosePushL( tasks );
	
	iDSProfile.ListTasksL( tasks );
	iTask.OpenL( iDSProfile, tasks[0] ); // open the first in list
	
	CleanupStack::PopAndDestroy(); // tasks
	
	return KErrNone;
	}	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::UsageType
//opens the connection
//-------------------------------------------------------------------------------
//
TSmlUsageType CClientAPIAdvanced::UsageType( CStifItemParser& aItem )
	{
	TInt type = -1;
	aItem.GetNextInt( type );
	
	switch ( type )
		{
		case 0:
			return ESmlDataSync;
			break;
		case 1:
			return ESmlDevMan;
			break;
		default:
			User::Leave( KErrArgument );
			break;
		}
		
	return ESmlDataSync;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSJobForTasksL
//creates the job for the defined tasks
//-------------------------------------------------------------------------------
//
	
void CClientAPIAdvanced::ListTasksL(TInt aId,TDesC& aProfilename)
	{
		RSyncMLDataSyncProfile profile;
		CleanupClosePushL( profile );
	    if(aId==-1)
	    {
	     	 TSmlProfileId profileId;
		     TInt i;
		     for (i=0;i<iProfiles.Count() ;++i)
		     {
		       profileId=OpenDSProfileByNameL(iProfiles[i],aProfilename);
		     }
		    profile.OpenL( iSyncSession, profileId,ESmlOpenReadWrite );
			profile.ListTasksL( iTasks );	
	    }
	    else
	    {
	      	profile.OpenL( iSyncSession, iProfiles[0], ESmlOpenReadWrite );
		  	profile.ListTasksL( iTasks );	
	    }
	    
	    RSyncMLTask task;
		CleanupClosePushL( task );
		task.OpenL( profile, iTasks[0] );
		task.SetDefaultSyncTypeL( ESmlTwoWay );
		task.UpdateL();
		profile.UpdateL();
		CleanupStack::PopAndDestroy(); // task
		CleanupStack::PopAndDestroy(); // profile
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSJobForTasksL
//creates the job for the defined tasks
//-------------------------------------------------------------------------------
//
	
TInt CClientAPIAdvanced::CreateDSJobForTasksL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	TPtrC profileName;
    aItem.GetNextString( profileName );
		
	ListProfilesL( ESmlDataSync );
	ListTasksL(id,profileName);
	
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTasks );
		iJobId = iDSJob.Identifier();
		}
	else if ( id == -1 )
		{
		 TSmlProfileId profileId;
	     TInt i;
	     for (i=0;i<iProfiles.Count() ;++i)
	     {
	       	
	       profileId=OpenDSProfileByNameL(iProfiles[i],profileName);
	     } 
		iDSJob.CreateL( iSyncSession, profileId, iTasks );
		iJobId = iDSJob.Identifier();
		}	
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTasks );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	

// -----------------------------------------------------------------------------
// Ctestprofileutil::SyncL
// Starts the Synchronization.
// -----------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::SyncL( CStifItemParser& aItem )
{
    TPtrC profileName;
    aItem.GetNextString( profileName );
   
	ListProfilesL( ESmlDataSync );
    TSmlProfileId profileId;
	TInt i;
	for (i=0;i<iProfiles.Count() ;++i)
	{
	       	
	      profileId=OpenDSProfileByNameL(iProfiles[i],profileName);
	}

    CSync *sync = CSync::NewL(profileId);

    CActiveScheduler::Start();
   
   	return KErrNone;

}

	
TInt CClientAPIAdvanced::Wait( CStifItemParser& /*aItem*/ )
	{
	User::After(200);
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CClientAPIAdvanced::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "OpenSyncSession", 	CClientAPIAdvanced::OpenSyncSessionL ),
        ENTRY( "CreateDSProfile", 	CClientAPIAdvanced	::CreateDSProfileL ),
        ENTRY( "SetDSRemoteProfileData",CClientAPIAdvanced::SetRemoteDSProfileDataL),
        ENTRY( "UpdateDSProfile",	 	CClientAPIAdvanced::UpdateDSProfileL ),
        ENTRY( "OpenConnection",		 CClientAPIAdvanced::OpenConnectionL ),
        ENTRY( "CreateConnection",		    CClientAPIAdvanced::CreateConnectionL ),
        ENTRY( "UpdateConnection",		    CClientAPIAdvanced::UpdateConnectionL ),
        ENTRY( "CreateTask",			    CClientAPIAdvanced::CreateTaskL),
        ENTRY( "SetTaskData",			    CClientAPIAdvanced::SetTaskDataL ),
        ENTRY( "UpdateTask",			    CClientAPIAdvanced::UpdateTaskL ),
        ENTRY( "CloseTask",				    CClientAPIAdvanced::CloseTask ),
        ENTRY( "CloseConnection",		    CClientAPIAdvanced::CloseConnection ),
        ENTRY( "CloseDSProfile", 			CClientAPIAdvanced::CloseDSProfile ),
        ENTRY( "CloseSyncSession", 			CClientAPIAdvanced::CloseSyncSession ),
        ENTRY( "OpenDSProfileById",			CClientAPIAdvanced::OpenDSProfileByIdL ),
        ENTRY( "CreateDSJobProf",			CClientAPIAdvanced::CreateDSJobForProfileL ),
        ENTRY( "CloseDSJob",				CClientAPIAdvanced::CloseDSJob ),
        ENTRY( "OpenTask",				CClientAPIAdvanced::OpenTaskL),
        ENTRY( "CreateDSJobTasks",			CClientAPIAdvanced::CreateDSJobForTasksL ),
        ENTRY ("Synchronize",	CClientAPIAdvanced::SyncL),
        ENTRY( "Wait", CClientAPIAdvanced::Wait ),
        //ENTRY( "OpenDSJobById",		        CClientAPIAdvanced::OpenDSJobByIdL ),
        //entries for creating the contacts,deleting the contacts and updating the contacts
        
        ENTRY( "CreateContact", CClientAPIAdvanced::CreateContactL ),
        ENTRY( "DeleteContact", CClientAPIAdvanced::DeleteContactL ),
        ENTRY( "UpdateContact", CClientAPIAdvanced::UpdateContactL ),
            
       
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    TRAP_IGNORE(RunInternalL( KFunctions, count, aItem ));
   // return RunInternalL( KFunctions, count, aItem );
   return(KErrNone);

    }

// -----------------------------------------------------------------------------
// CClientAPIAdvanced::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CClientAPIAdvanced::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KClientAPIAdvanced, "ClientAPIAdvanced" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KClientAPIAdvanced, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KClientAPIAdvanced, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CClientAPIAdvanced::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CClientAPIAdvanced::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
