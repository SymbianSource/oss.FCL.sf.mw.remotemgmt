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
* Description:  DS-settings 
*
*/


#include <nsmldsprofileres.rsg>

#include <nsmlconstants.h>
#include <nsmldsconstants.h>
#include "nsmldssettings.h"
#include "NSmlDSResourceProfile.h"
#include "barsc.h"
#include "bautils.h"
#include "nsmlroam.h"
//XML profiles
#include "xml/RXMLReader.h"
#include "NSmlProfileContentHandler.h"
#include <centralrepository.h> //CRepository

//CONSTANTS
_LIT(Kinfile,"z:\\Private\\101F99FB\\VariantData.xml");


//=============================================
//
//		CNSmlDSSettings
//
//=============================================

//=============================================
//		CNSmlDSSettings::NewL()
//		Creates a new instance of CNSmlDSSettings object.
//=============================================

EXPORT_C CNSmlDSSettings* CNSmlDSSettings::NewL()
	{
	CNSmlDSSettings* self = CNSmlDSSettings::NewLC();
	CleanupStack::Pop();
	return self;
	}	

//=============================================
//		CNSmlDSSettings::NewLC()
//		Creates a new instance of CNSmlDSSettings object. 
//		Pushes and leaves new instance into CleanupStack.
//		Opens/creates settings database.
//=============================================

EXPORT_C CNSmlDSSettings* CNSmlDSSettings::NewLC()
	{

	CNSmlDSSettings* self = new( ELeave ) CNSmlDSSettings;
	CleanupStack::PushL( self );
	self->ConstructL();

	User::LeaveIfError( self->iFsSession.Connect() );
	User::LeaveIfError( self->iRdbSession.Connect() );	
	
   TParse name;
    
#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlSettingsDbName(), NULL, NULL );
#else
    name.Set( KNSmlSettingsDbName(), KNSmlDatabasesNonSecurePath, NULL );
#endif

	TInt err = self->iDatabase.Open( self->iRdbSession,
	                                 name.FullName(),
	                                 KNSmlDBMSSecureSOSServerID );
	
	if ( err == KErrNotFound )
		{
		self->CreateDatabaseL( name.FullName() );
		self->iDatabase.Open( self->iRdbSession,
		                      name.FullName(),
		                      KNSmlDBMSSecureSOSServerID );
		User::LeaveIfError( self->iTableProfiles.Open( self->iDatabase, KNSmlTableProfiles ) );
		self->iColSet = self->iTableProfiles.ColSetL();
		}
	else
		{
		if ( (err == KErrEof) || (err == KErrCorrupt) ||
		     (err == KErrArgument) || (err == KErrNotSupported) )
			{
			self->iRdbSession.DeleteDatabase( name.FullName(),
			                                  KNSmlSOSServerPolicyUID );
			}
		User::LeaveIfError( err );
	User::LeaveIfError( self->iTableProfiles.Open( self->iDatabase, KNSmlTableProfiles ) );
	self->iColSet = self->iTableProfiles.ColSetL();
		}
	return self;
	}

//=============================================
//		CNSmlDSSettings::ConstructL()
//	    2nd phase constructor
//=============================================
void CNSmlDSSettings::ConstructL()
	{
	iResourceProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDSProfile> (5);
	}


//=============================================
//		CNSmlDSSettings::~CNSmlDSSettings()
//		Destructor.
//=============================================

EXPORT_C CNSmlDSSettings::~CNSmlDSSettings()
	{
	if(iResourceProfileArray)
		{
		iResourceProfileArray->ResetAndDestroy();
		}
	delete iResourceProfileArray;
	
	iView.Close();
	delete iColSet;
	iTableProfiles.Close();
	iDatabase.Close();
	iFsSession.Close();
	iRdbSession.Close();
	}

//=============================================
//		CNSmlDSSettings::CreateDatabaseL()
//		Creates settings database.
//=============================================

void CNSmlDSSettings::CreateDatabaseL(const TDesC& aFullName)
	{
	// 50 is the extra length neede for integer lengths
	HBufC* createProfileTable = HBufC::NewLC( KDSCreateProfilesTable().Length() + 50);
	TPtr profileTablePtr = createProfileTable->Des();

	profileTablePtr.Format(KDSCreateProfilesTable,KNSmlMaxProfileNameLength,KNSmlMaxUsernameLength,KNSmlMaxPasswordLength,KNSmlMaxURLLength,KNSmlMaxServerIdLength,KNSmlMaxHttpAuthUsernameLength,KNSmlMaxHttpAuthPasswordLength, KNSmlDSVisibilityArraySize );

	// 25 is the extra length neede for integer lengths
	HBufC* createAdaptersTable = HBufC::NewLC( KDSCreateAdaptersTable().Length() + 25);
	TPtr adaptersTablePtr = createAdaptersTable->Des();
	adaptersTablePtr.Format(KDSCreateAdaptersTable,KNSmlMaxAdapterDisplayNameLength,KNSmlMaxRemoteNameLength, KNSmlMaxLocalNameLength );

    User::LeaveIfError( iDatabase.Create( this->iRdbSession, aFullName, KNSmlDBMSSecureSOSServerID ) );  
	
	iDatabase.Begin();

	iDatabase.Execute( *createProfileTable );
	iDatabase.Execute( *createAdaptersTable );

	CDbColSet* colSet = CDbColSet::NewLC();
	colSet->AddL(TDbCol(KNSmlVersionColumnMajor(), EDbColUint16));
	colSet->AddL(TDbCol(KNSmlVersionColumnMinor(), EDbColUint16));
	User::LeaveIfError(iDatabase.CreateTable(KNSmlTableVersion(), *colSet));
	CleanupStack::PopAndDestroy(  ); //colset

	RDbTable table;
	User::LeaveIfError(table.Open(iDatabase, KNSmlTableVersion()));
	CleanupClosePushL(table);
	colSet = table.ColSetL();
	CleanupStack::PushL(colSet);
	table.InsertL();
	table.SetColL(colSet->ColNo(KNSmlVersionColumnMajor), KNSmlSettingsCurrentVersionMajor);
	table.SetColL(colSet->ColNo(KNSmlVersionColumnMinor), KNSmlSettingsCurrentVersionMinor);
	table.PutL();
	
	iDatabase.Commit();
	
	CreateHiddenProfilesL();

	TInt keyVal;
	TRAPD (err ,ReadRepositoryL(KNsmlDsCustomProfiles, keyVal));
	if (err == KErrNone && keyVal)
	{
		TBool aRestore = EFalse;
		CreateXMLProfilesL(aRestore);
	}
	iDatabase.Close();
	CleanupStack::PopAndDestroy( 4 ); // createAdaptersTable, createProfileTable, colSet, table
	}
	
//=============================================
//		CNSmlDSSettings::CreateHiddenProfilesL()
//		Create new profiles reading data from
//		resource file
//=============================================

void CNSmlDSSettings::CreateHiddenProfilesL()
	{
	TFileName fileName;
	Dll::FileName( fileName );
	TParse parse;

	parse.Set( KNSmlDSProfilesRsc, &fileName, NULL );
	fileName = parse.FullName();

	RResourceFile resourceFile; 
	BaflUtils::NearestLanguageFile( iFsSession, fileName );

	TRAPD(leavecode,resourceFile.OpenL( iFsSession,fileName));
	if(leavecode != 0)
		{
		return;
		}
	CleanupClosePushL(resourceFile);
	
	HBufC8* profileRes = resourceFile.AllocReadLC( NSML_DS_PROFILES );
	TResourceReader reader;
	reader.SetBuffer( profileRes );

	CNSmlDSResourceProfiles* profileResReader = CNSmlDSResourceProfiles::NewLC( reader, this );
	profileResReader->SaveProfilesL(iResourceProfileArray);
	CleanupStack::PopAndDestroy(3); // profileResReader, profileRes, resourceFile
	}
	
	
//===============================================
//		CNSmlDSSettings::CreateXMLProfiles()
//		
//		
//===============================================	
void CNSmlDSSettings::CreateXMLProfilesL(TBool aRestore)
	{
	
	TBool status = TRUE;
	TInt error = KErrNone;
		
	//file server
	RFs wSession;
	error = wSession.Connect();
	if (error != KErrNone)
	{
	return;
	}
	
	RXMLReader DSProfileParser;
	DSProfileParser.CreateL();
	CArrayPtrFlat<CNSmlDSProfile>* customProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDSProfile> (5);
	CleanupStack::PushL(customProfileArray);
	
	CNSmlProfileContentHandler* cb = CNSmlProfileContentHandler::NewL(this ,customProfileArray);
	CleanupStack::PushL(cb);

	DSProfileParser.SetContentHandler(cb);

	DSProfileParser.SetFeature(EXMLValidation, ETrue);
	DSProfileParser.SetFeature(EXMLValidation, EFalse);
	DSProfileParser.GetFeature(EXMLBinary, status);
	DSProfileParser.GetFeature(EXMLValidation, status);

	status = TRUE;

	RFile wFile;
	TInt err =wFile.Open(wSession, Kinfile, EFileRead | EFileShareReadersOnly); 
	if (err != KErrNone)
	{
		CleanupStack::PopAndDestroy(2);	
		return;	
	}
	
	CleanupClosePushL(wFile);
	//parse file
	TRAP(error, DSProfileParser.ParseL(wFile));
	if (error != KErrNone)
	{
	CleanupStack::PopAndDestroy(3);	
	return;
	}
	TInt index;
	TBuf<150> buf;
    
    if(aRestore)
    {
    //handling back up-restore for custom profiles
    CNSmlDSProfileList* profList = new (ELeave) CArrayPtrFlat<CNSmlDSProfileListItem>(1);
	CleanupStack::PushL(profList);
    GetAllProfileListL( profList );
    for( TInt i = 0 ; i < profList->Count() ; i++ )
     {
     	TInt id = profList->At(i)->IntValue( EDSProfileId );
     	CNSmlDSProfile* prof = ProfileL(id);
     	CleanupStack::PushL( prof );
     	buf = prof->StrValue(EDSProfileServerId);
     	
     	if (buf.Compare(KEmpty) != 0)
     	{
     		for (index = 0; index < customProfileArray->Count(); index++ )
     		{
     		if (buf.Compare(customProfileArray->At(index)->StrValue(EDSProfileServerId)) == 0)
     			{
     			DeleteProfileL(id);
     			}
     		}	
     	}
     	
     	else
     	{
     		buf = prof->StrValue(EDSProfileServerURL);
     		for (index = 0; index < customProfileArray->Count(); index++ )
     		{
     		if (buf.Compare(customProfileArray->At(index)->StrValue(EDSProfileServerURL)) == 0)
     			{
     			DeleteProfileL(id);
     			}
     		}	
     	}
       	CleanupStack::PopAndDestroy(); // prof
     }
    
    profList->ResetAndDestroy();
    CleanupStack::PopAndDestroy();
    }
    
    //save profiles
    TBool defaultprofilefound = EFalse;
    //Set the ProfileID to default
    WriteRepositoryL( KCRUidDSDefaultProfileInternalKeys, KNsmlDsDefaultProfile, -1 );
	for ( index = 0; index < customProfileArray->Count(); index++ )
		{
		if (CheckXMLProfileSettings(customProfileArray ,index))
			{
			customProfileArray->At(index)->SaveL();
			if(!defaultprofilefound && customProfileArray->At(index)->IntValue(EDSProfileDefaultProfile))
			    {
			    //Set the ProfileID to be used as a Default Profile
			    WriteRepositoryL( KCRUidDSDefaultProfileInternalKeys, KNsmlDsDefaultProfile, 
			                      customProfileArray->At(index)->IntValue(EDSProfileId) );
			    defaultprofilefound = ETrue;
			    }		
			}
		}
		
	CleanupStack::PopAndDestroy(); // wFile
	CleanupStack::PopAndDestroy(); // cb
	
	customProfileArray->ResetAndDestroy();
	CleanupStack::PopAndDestroy(); //customProfileArray
	
	
	}

//===============================================
//		CNSmlDSSettings::CheckXMLProfileSettings()
//		
//		
//===============================================	
TBool CNSmlDSSettings::CheckXMLProfileSettings(CNSmlProfileArray* aProfileArray,TInt aIndex)
	{
	
	const TInt KMaxXMLProfileCount = 5;
	TBuf<160> buf;
	TInt count = aProfileArray->Count();
	//max count
	if (count > KMaxXMLProfileCount)
		{
		return EFalse;
		}
	
	//mandatory settings
	if (aProfileArray->At(aIndex)->StrValue(EDSProfileDisplayName).Compare(KEmpty) == 0
		||aProfileArray->At(aIndex)->StrValue(EDSProfileServerURL).Compare(KEmpty) == 0
		)
		{
		return EFalse;
		}
	
	//Unique Server Id	
	buf = aProfileArray->At(aIndex)->StrValue(EDSProfileServerId);
	if (buf.Compare(KEmpty) != 0)
		{
	   	for(TInt i = 0 ; i < iResourceProfileArray->Count() ; i++ )
	   	    {
	   		if (iResourceProfileArray->At(i)->StrValue(EDSProfileServerId)
     						.Compare(buf) == 0)
     			{
     	 		return EFalse;	
     			}
	   	    }
     	}
    
    if(buf.Compare(KEmpty) != 0)
		{
		for (TInt i = 0; i < count ;i++)
			{
			if (buf.Compare(aProfileArray->At(i)->StrValue(EDSProfileServerId)) == 0
					&& i < aIndex)
					{
					return EFalse;
					}
			}
		}
    
	return ETrue; 
	}

// -----------------------------------------------------------------------------
// CNSmlDSSettings::ReadRepositoryL
//
// -----------------------------------------------------------------------------
//
void CNSmlDSSettings::ReadRepositoryL(TInt aKey, TInt& aValue)
	{
	const TUid KRepositoryId = KCRUidDataSyncInternalKeys;
	
    CRepository* rep = CRepository::NewLC(KRepositoryId);
    TInt err = rep->Get(aKey, aValue);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);
    
	}	
// -----------------------------------------------------------------------------
// CNSmlDSSettings::WriteRepositoryL
//
// -----------------------------------------------------------------------------
//
void CNSmlDSSettings::WriteRepositoryL(TUid aUid, TInt aKey, TInt aValue)
    {
    const TUid KRepositoryId = aUid;
    
    CRepository* rep = CRepository::NewLC(KRepositoryId);
    TInt err = rep->Set(aKey, aValue);
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(rep);
    
    }
//===============================================
//		CNSmlDSSettings::UpdateHiddenProfilesL()
//		Updates the profiles with data from 
//		resource file
//===============================================
EXPORT_C void CNSmlDSSettings::UpdateHiddenProfilesL()
{
	 
	CNSmlDSProfileList* profList = new (ELeave) CArrayPtrFlat<CNSmlDSProfileListItem>(1);
	CleanupStack::PushL(profList);
    GetAllProfileListL( profList );
    for( TInt i = 0 ; i < profList->Count() ; i++ )
    {
       	TInt id = profList->At(i)->IntValue( EDSProfileId );
     	CNSmlDSProfile* prof = ProfileL(id);
     	CleanupStack::PushL( prof );
     	if(!prof->IntValue(EDSProfileDeleteAllowed))
     	{
     		
     		DeleteProfileL(id);
     	}
     	CleanupStack::PopAndDestroy(); // prof
    }
     
    profList->ResetAndDestroy();
    CleanupStack::PopAndDestroy();
     
    CreateHiddenProfilesL();
     
    TInt keyVal;
	TRAPD (err ,ReadRepositoryL(KNsmlDsCustomProfiles, keyVal));
	if (err == KErrNone && keyVal)
	{
	    TInt aRestore = ETrue;
    	CreateXMLProfilesL(aRestore);
	}
}
//=============================================
//		CNSmlDSSettings::CreateProfileL()
//		Returns a pointer to newly created CNSmlDSProfile object.
//=============================================

EXPORT_C CNSmlDSProfile* CNSmlDSSettings::CreateProfileL()
	{
	CNSmlDSProfile* profile = CNSmlDSProfile::NewL( this->Database() );

	profile->SetIntValue( EDSProfileId,					KNSmlNewObject );
	profile->SetIntValue( EDSProfileTransportId,		KUidNSmlMediumTypeInternet.iUid );
	profile->SetIntValue( EDSProfileHidden,				EFalse);
	profile->SetIntValue( EDSProfileDeleteAllowed,		ETrue);

	profile->SetIntValue( EDSProfileServerAlertedAction,ESmlConfirmSync);

	profile->SetIntValue( EDSProfileIAPId,				-2 );

	profile->SetIntValue( EDSProfileHttpAuthUsed,		EFalse);
	profile->SetIntValue( EDSProfileAutoChangeIAP,		EFalse);
	
	profile->SetIntValue( EDSProfileProtocolVersion, 	ESmlVersion1_2 );

	profile->InitVisibilityArray(KNSmlDefaultVisibility);

    return profile;
	}

//=============================================
//		CNSmlDSSettings::CreateProfileL()
//		Copies all profile data with given Id to
//		a newly created CNSmlDSProfile object.
//=============================================

EXPORT_C CNSmlDSProfile* CNSmlDSSettings::CreateProfileL( const TInt aId  )
	{
	CNSmlDSProfile* profile = ProfileOnlyL( aId);
	CleanupStack::PushL( profile );
	if ( profile )
		{
		profile->SetIntValue( EDSProfileId,		KNSmlNewObject );

		CNSmlDSProfile* copyProfile = ProfileL( aId);
		CleanupStack::PushL(copyProfile);
		if(!copyProfile)
			{
			profile = 0;
			return profile;
			}

		for ( TInt i = 0; i < copyProfile->iContentTypes->Count(); i++ )
			{
			TInt uid = copyProfile->iContentTypes->At( i )->IntValue(EDSAdapterImplementationId);
			TDesC clientDataSource = copyProfile->iContentTypes->At( i )->StrValue( EDSAdapterClientDataSource );
			TDesC serverDataSource = copyProfile->iContentTypes->At( i )->StrValue( EDSAdapterServerDataSource );
			profile->AddContentTypeL( uid, clientDataSource, serverDataSource );
			CNSmlDSContentType* content = profile->ContentType( uid );
			content->SetIntValue( EDSAdapterEnabled, copyProfile->iContentTypes->At( i )->IntValue(EDSAdapterEnabled) );
			content->SetStrValue( EDSAdapterDisplayName, copyProfile->iContentTypes->At( i )->StrValue(EDSAdapterDisplayName) );
			content->SetIntValue( EDSAdapterSyncType, copyProfile->iContentTypes->At( i )->IntValue(EDSAdapterSyncType) );
			content->SetIntValue( EDSAdapterFilterMatchType, copyProfile->iContentTypes->At( i )->IntValue(EDSAdapterFilterMatchType) );
			}

		CleanupStack::PopAndDestroy(); // copyProfile
		}
	CleanupStack::Pop(); //profile
	return profile;
	}

//=============================================
//		CNSmlDSSettings::Database()
//		Returns a reference to settings database object.
//=============================================
	
EXPORT_C RDbNamedDatabase* CNSmlDSSettings::Database()
	{
	return &iDatabase;
	}

//=============================================
//		CNSmlDSSettings::ProfileIdL()
//		Returns Profile ID with given AdapterId.
//		If there's no corresponding Id returns 0.
//=============================================

EXPORT_C TInt CNSmlDSSettings::ProfileIdL( TInt aId)
	{
	TInt profileId = 0;

	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfileId().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfileId, aId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	if( iView.FirstL() )
		{
		iView.GetL();

		profileId =  ViewColUint( KNSmlDSProfileId );
		}
	else
		{
		//couldn't find
		profileId = KErrNotFound;
		}

	return profileId;
	}

//=============================================
//		CNSmlDSSettings::ProfileL()
//		Returns a pointer to CNSmlDSProfile object with given Id.
//		If there's no corresponding Id in the database, returns NULL.
//=============================================

EXPORT_C CNSmlDSProfile* CNSmlDSSettings::ProfileL( TInt aId, const TNSmlDSContentTypeGetMode aMode)
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfile, aId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	CNSmlDSProfile* profile = CNSmlDSProfile::NewLC( Database() );

	if( iView.FirstL() )
		{
		CNSmlDSCrypt* crypt = new(ELeave) CNSmlDSCrypt;
		CleanupStack::PushL(crypt);
		iView.GetL();

		profile->SetIntValue( EDSProfileId, ViewColUint( KNSmlDSProfileId ) );
		profile->SetStrValue( EDSProfileDisplayName, ViewColDes( KNSmlDSProfileDisplayName ) );
		profile->SetIntValue( EDSProfileIAPId, ViewColInt( KNSmlDSProfileIAPId ) );
		profile->SetIntValue( EDSProfileTransportId, ViewColInt( ( KNSmlDSProfileTransportId ) ) );
		profile->SetStrValue( EDSProfileServerURL, ViewColDes( KNSmlDSProfileServerURL ) );
		profile->SetStrValue( EDSProfileServerId, ViewColDes( KNSmlDSProfileServerId ) );

		profile->SetStrValue( EDSProfileSyncServerUsername, ViewColDes( KNSmlDSProfileSyncServerUsername ) );
		profile->SetStrValue( EDSProfileSyncServerPassword,  crypt->DecryptedL(ViewColDes( KNSmlDSProfileSyncServerPassword ) ) );
		
		profile->SetIntValue( EDSProfileServerAlertedAction, ViewColInt( KNSmlDSProfileServerAlertedAction ) );
		profile->SetIntValue( EDSProfileDeleteAllowed, ViewColInt( KNSmlDSProfileDeleteAllowed ) );
		profile->SetIntValue( EDSProfileHidden, ViewColInt( KNSmlDSProfileHidden ) );

		profile->SetIntValue( EDSProfileHttpAuthUsed, ViewColInt( KNSmlDSProfileHttpAuthUsed ) );
		profile->SetStrValue( EDSProfileHttpAuthUsername, ViewColDes( KNSmlDSProfileHttpAuthUsername ) );
		profile->SetStrValue( EDSProfileHttpAuthPassword,  crypt->DecryptedL(ViewColDes( KNSmlDSProfileHttpAuthPassword ) ) );
		profile->SetIntValue( EDSProfileAutoChangeIAP, ViewColInt( KNSmlDSProfileAutoChangeIAP ) );
		profile->SetIntValue( EDSProfileCreatorId, ViewColInt( KNSmlDSProfileCreatorID ) );
		
		profile->InitVisibilityArray( ViewColDes( KNSmlDSProfileVisibilityStr ) );

		profile->SetIntValue( EDSProfileProtocolVersion, ViewColInt( KNSmlDSProfileProtocolVersion ) );

		CleanupStack::PopAndDestroy(); // crypt
		}
	else
		{
		delete profile;
		//couldn't find
		profile = NULL;
		}
//
//  Get adapters in profile
//
	if ( profile )
		{
		profile->GetContentTypesL( aMode );
		}

	CleanupStack::Pop(); // profile

	return profile;
	}

//=============================================
//		CNSmlDSSettings::ProfileOnlyL()
//		Returns a pointer to CNSmlDSProfile object with given Id.
//		If there's no corresponding Id in the database, returns NULL.
//=============================================

CNSmlDSProfile* CNSmlDSSettings::ProfileOnlyL( TInt aId )
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfile, aId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	CNSmlDSProfile* profile = CNSmlDSProfile::NewLC( Database() );

	if( iView.FirstL() )
		{
		CNSmlDSCrypt* crypt = new(ELeave) CNSmlDSCrypt;
		CleanupStack::PushL(crypt);
		iView.GetL();

		profile->SetIntValue( EDSProfileId, ViewColUint( KNSmlDSProfileId ) );
		profile->SetStrValue( EDSProfileDisplayName, ViewColDes( KNSmlDSProfileDisplayName ) );
		profile->SetIntValue( EDSProfileIAPId, ViewColInt( KNSmlDSProfileIAPId ) );
		profile->SetIntValue( EDSProfileTransportId, ViewColInt( KNSmlDSProfileTransportId ) );
		profile->SetStrValue( EDSProfileServerURL, ViewColDes( KNSmlDSProfileServerURL ) );
		profile->SetStrValue( EDSProfileServerId, ViewColDes( KNSmlDSProfileServerId ) );

		profile->SetStrValue( EDSProfileSyncServerUsername, ViewColDes( KNSmlDSProfileSyncServerUsername ) );
		profile->SetStrValue( EDSProfileSyncServerPassword,  crypt->DecryptedL(ViewColDes( KNSmlDSProfileSyncServerPassword ) ) );
		
		profile->SetIntValue( EDSProfileServerAlertedAction, ViewColInt( KNSmlDSProfileServerAlertedAction ) );
		profile->SetIntValue( EDSProfileDeleteAllowed,  ViewColInt( KNSmlDSProfileDeleteAllowed ) );
		profile->SetIntValue( EDSProfileHidden,  ViewColInt( KNSmlDSProfileHidden ) );

		profile->SetIntValue( EDSProfileHttpAuthUsed, ViewColInt( KNSmlDSProfileHttpAuthUsed ) );
		profile->SetStrValue( EDSProfileHttpAuthUsername, ViewColDes( KNSmlDSProfileHttpAuthUsername ) );
		profile->SetStrValue( EDSProfileHttpAuthPassword,  crypt->DecryptedL(ViewColDes( KNSmlDSProfileHttpAuthPassword ) ) );
		profile->SetIntValue( EDSProfileAutoChangeIAP, ViewColInt( KNSmlDSProfileAutoChangeIAP ) );
		profile->SetIntValue( EDSProfileCreatorId, ViewColInt( KNSmlDSProfileCreatorID ) );

		profile->InitVisibilityArray( ViewColDes( KNSmlDSProfileVisibilityStr ) );

		profile->SetIntValue( EDSProfileProtocolVersion, ViewColInt( KNSmlDSProfileProtocolVersion ) );

		CleanupStack::PopAndDestroy(); // crypt
		CleanupStack::Pop(); // profile
		}
	else
		{
		CleanupStack::PopAndDestroy(profile);
		//couldn't find
		profile = NULL;
		}

	return profile;
	}
//=============================================
//		CNSmlDSSettings::GetProfileListL()
//		Appends CNSmlDSProfileListItem objects to aProfileList.
//=============================================

EXPORT_C void CNSmlDSSettings::GetProfileListL( CNSmlDSProfileList* aProfileList )
	{
	aProfileList->ResetAndDestroy();

	PrepareViewL( KDSSQLGetProfiles, iView.EUpdatable );
	
	if(iView.Unevaluated())
		{
		iView.EvaluateAll(); // Needed for order by
		}	
	
	while ( iView.NextL() )
		{
		AddToProfileListL( aProfileList );
		}

	}

//=============================================
//		CNSmlDSSettings::GetaAllProfileListL()
//		Appends CNSmlDSProfileListItem objects to aProfileList.
//=============================================

EXPORT_C void CNSmlDSSettings::GetAllProfileListL( CNSmlDSProfileList* aProfileList )
	{
	aProfileList->ResetAndDestroy();

	PrepareViewL( KDSSQLGetAllProfiles, iView.EUpdatable );
	
	if(iView.Unevaluated())
		{
		iView.EvaluateAll(); // Needed for order by
		}
		
	while ( iView.NextL() )
		{
		AddToProfileListL( aProfileList );
		}
	}

//=============================================
//		CNSmlDSSettings::DeleteProfileL()
//		Permanently deletes profile with given Id.
//		If delete is successful returns KErrNone,
//		otherwise returns negative integer 
//      (defined in EPOC error codes).
//=============================================

EXPORT_C TInt CNSmlDSSettings::DeleteProfileL( TInt aId )
	{
	if ( iDatabase.InTransaction() )
		{
		return ( KErrAccessDenied );
		}

	TBuf<64> sqlStatement;
	
	TInt err( KErrNone );

	iDatabase.Begin();

	sqlStatement.Format( KDSSQLDeleteProfile, aId );
	err = iDatabase.Execute( sqlStatement );
	if ( err < KErrNone )
		{
		iDatabase.Rollback();
		return err;
		}

	sqlStatement.Format( KDSSQLDeleteAdapter, aId );
	err = iDatabase.Execute( sqlStatement );
	if ( err < KErrNone )
		{
		iDatabase.Rollback();
		return err;
		}

	// Delete IAP Id's from RoamTable
	CNSmlRoamTable* roamTable = CNSmlRoamTable::NewL( iDatabase );
	CleanupStack::PushL( roamTable );
	TRAPD( errRoam, roamTable->RemoveAllByIdL( aId ) );
	if ( errRoam < KErrNone ) 
		{
		CleanupStack::PopAndDestroy(); // roamTable
		iDatabase.Rollback();
		return errRoam;
		}
	CleanupStack::PopAndDestroy(); // roamTable

	CommitAndCompact();

	return KErrNone;
	}

//=============================================
//		CNSmlDSSettings::PrepareViewL()
//		Closes and prepares the view
//		
//=============================================
void CNSmlDSSettings::PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess )
	{
	iView.Close();
	User::LeaveIfError( iView.Prepare( iDatabase, TDbQuery( aSql ), aAccess ) );
	}

//=============================================
//		CNSmlDSSettings::CommitAndCompact
//		Commits update and compacts the database
//		
//=============================================
void CNSmlDSSettings::CommitAndCompact() 
	{
	iDatabase.Commit();
	iDatabase.Compact();
	}

//=============================================
//		CNSmlDSSettings::AddToProfileListL
//		Adds database field to profile list
//		
//=============================================
void CNSmlDSSettings::AddToProfileListL( CNSmlDSProfileList* aProfileList )
	{
	iView.GetL();

	aProfileList->AppendL( CNSmlDSProfileListItem::NewLC( iView.ColUint( iColSet->ColNo( KNSmlDSProfileId ) ) ) );
	CleanupStack::Pop();
	}

//=============================================
//		CNSmlDSSettings::ViewColDes
//	
//		
//=============================================
TPtrC CNSmlDSSettings::ViewColDes( const TDesC& aFieldName )
	{
	return iView.ColDes( iColSet->ColNo( aFieldName ) );
	}

//=============================================
//		CNSmlDSSettings::ViewColInt
//	
//		
//=============================================
TInt CNSmlDSSettings::ViewColInt( const TDesC& aFieldName )
	{
	return iView.ColInt( iColSet->ColNo( aFieldName ) );
	}

//=============================================
//		CNSmlDSSettings::ViewColUint
//	
//		
//=============================================
TUint CNSmlDSSettings::ViewColUint( const TDesC& aFieldName )
	{
	return iView.ColUint( iColSet->ColNo( aFieldName ) );
	}


//  End of File
