/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM-settings 
*
*/


#include <nsmldmprofileres.rsg>
#include <SyncMLDef.h>
#include <utf.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldmconstants.h>
#include <nsmldmtreedbclient.h>
#include <xml/RXMLReader.h>
#include <centralrepository.h>
#include "nsmldmsettings.h"
#include "barsc.h"
#include "bautils.h"
#include "NSmlDMResourceProfile.h"
#include "DMprofileContentHandler.h"
#include "DevManInternalCRKeys.h"

//------------------------------------------------------------------------------
//
//		CNSmlDMSettings
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//		CNSmlDMSettings::NewL()
//		Creates a new instance of CNSmlDMSettings object.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMSettings* CNSmlDMSettings::NewL()
	{
	CNSmlDMSettings* self = CNSmlDMSettings::NewLC();	
	CleanupStack::Pop();
	return self;
	}	

//------------------------------------------------------------------------------
//		CNSmlDMSettings::NewLC()
//		Creates a new instance of CNSmlDMSettings object. 
//		Pushes and leaves new instance into CleanupStack.
//		Opens/creates settings database.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMSettings* CNSmlDMSettings::NewLC()
	{

	CNSmlDMSettings* self = new( ELeave ) CNSmlDMSettings;
	CleanupStack::PushL( self );

	User::LeaveIfError( self->iFsSession.Connect() );
	User::LeaveIfError( self->iRdbSession.Connect() );	
	
    TParse name;
    
#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlDMSettingsDbName(), NULL, NULL );
#else
    name.Set( KNSmlDMSettingsDbName(), KNSmlDatabasesNonSecurePath, NULL );
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
		}
	else
		{
		if ( (err == KErrEof) || (err == KErrCorrupt) ||
		     (err == KErrArgument) || (err == KErrNotSupported))
			{
			self->iRdbSession.DeleteDatabase( name.FullName(),
                                              KNSmlSOSServerPolicyUID );

			}
		User::LeaveIfError( err );
		}
	User::LeaveIfError( self->iTableProfiles.Open( self->iDatabase, KNSmlDMTableProfiles ) );
	self->iColSet = self->iTableProfiles.ColSetL();
	//Upgrade from older releases 
	if( self->iColSet->Count() != KNSmlDMProfileTableColCount )
	{		
		self->ScanAndRepairRestoredProfilesL();
	}

	return self;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::~CNSmlDMSettings()
//		Destructor.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMSettings::~CNSmlDMSettings()
	{
	iView.Close();
	delete iColSet;
	iTableProfiles.Close();
	iDatabase.Close();
	iFsSession.Close();
	iRdbSession.Close();
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ScanAndRepairRestoredProfilesL()
//		Scan And Repair RestoredProfiles in database.
//------------------------------------------------------------------------------
EXPORT_C void CNSmlDMSettings::ScanAndRepairRestoredProfilesL()
{
	if( iColSet->Count() != KNSmlDMProfileTableColCount )
	{
        TBool hiddenMissing = EFalse;
        TBool lastSyncMissing = EFalse;
		TBool profilelockMissing = EFalse;		
        if(iColSet->ColNo( KNSmlDMProfileHidden ) == KDbNullColNo)
        {
        	hiddenMissing = ETrue;
            iMissingCols.AppendL(TPtrC (KNSmlDMProfileHidden)) ;
        }
        if(iColSet->ColNo( KNSmlDMLastSync ) == KDbNullColNo)
        {
        	lastSyncMissing= ETrue;
            iMissingCols.AppendL(TPtrC(KNSmlDMLastSync));
        }
        
        if(iColSet->ColNo( KNSmlDMProfileLock ) == KDbNullColNo)
                {
                    profilelockMissing= ETrue;
                    iMissingCols.AppendL(TPtrC(KNSmlDMProfileLock));
                }
            //Check for missing column 
        if ( hiddenMissing || lastSyncMissing || profilelockMissing  )
        {                   
            CNSmlDMProfileList* oldProfiles = new( ELeave ) CArrayPtrFlat<CNSmlDMProfileListItem>(1);
            CleanupStack::PushL( PtrArrCleanupItem( CNSmlDMProfileListItem, oldProfiles ) );
            GetProfileListL(oldProfiles);
            for ( TInt i = 0; i < oldProfiles->Count(); i++)
            {	
                TInt profileID = (*oldProfiles)[i]->IntValue( TNSmlDMProfileData( EDMProfileId ) );
                CNSmlDMProfile* profile = OldProfileL( profileID );
                profile->CloseDbHandles();
                iOldProfiles.AppendL(profile);
                HBufC* sqlStatement = HBufC::NewLC( KDMSQLDeleteProfile().Length() + KNSmlDmMaxIntegerLength );
                TPtr sqlStatementPtr = sqlStatement->Des();
    		    TInt err( KErrNone );
                iDatabase.Begin();
                sqlStatementPtr.Format( KDMSQLDeleteProfile, profileID );
                err = iDatabase.Execute( sqlStatementPtr );
                CleanupStack::PopAndDestroy();  // sqlStatement
                    if ( err < KErrNone )
        		    {   
        		    iDatabase.Rollback();
        	      	}
                iDatabase.Commit();
                iDatabase.Compact();
    	    }
                    
            iView.Close();
            delete iColSet;
            iColSet = NULL;
            iTableProfiles.Close();
            iDatabase.Begin();
            iDatabase.DropTable(KNSmlDMTableProfiles);
            iDatabase.DropTable(KDMCreateCommSetTable);
            iDatabase.DropTable(KNSmlTableVersion);
            iDatabase.Commit();
            iDatabase.Compact();
            CreateDMTablesL();
            User::LeaveIfError( iTableProfiles.Open( iDatabase, KNSmlDMTableProfiles ) );
            iColSet = iTableProfiles.ColSetL();
            CreateHiddenProfilesL();
            CreateDMProfilesFromXmlL();
            // Get the server ID if found in resource do not save it
            RArray<TPtrC> serverIDList;
            CNSmlDMProfileList* newProfiles = new( ELeave ) CArrayPtrFlat<CNSmlDMProfileListItem>(1);
            CleanupStack::PushL( PtrArrCleanupItem( CNSmlDMProfileListItem, newProfiles ) );
            GetProfileListL(newProfiles);
            for ( TInt i = 0; i < newProfiles->Count(); i++)
                {	
          //      (*newProfiles)[i]->StrValue( TNSmlDMProfileData( EDMProfileServerId );
                TInt profileID = (*newProfiles)[i]->IntValue( TNSmlDMProfileData( EDMProfileId ) );
                CNSmlDMProfile* profile = ProfileL( profileID );
			    CleanupStack::PushL( profile );
			
                serverIDList.AppendL(profile->StrValue( EDMProfileServerId));
                CleanupStack::PopAndDestroy(profile);
                }
        
             newProfiles->ResetAndDestroy();
            CleanupStack::PopAndDestroy(); //newProfiles                
           // now save the profiles . 
            for( TInt i = 0 ; i<iOldProfiles.Count(); i++ ) 
            {
                CNSmlDMProfile *profile = iOldProfiles[i];
                profile->ResetID();
                profile->ResetDatabaseL(&(iDatabase));
                if( serverIDList.Find(profile->StrValue( TNSmlDMProfileData( EDMProfileServerId ))) == KErrNotFound)
                    {
                    profile->SaveL();
                    }
            }
            oldProfiles->ResetAndDestroy();
            CleanupStack::PopAndDestroy(); //oldProfiles
         }
         else // Corrupt case 
         
            {
            iTableProfiles.Close();
            iDatabase.Close();
            TParse name;
    
#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlDMSettingsDbName(), NULL, NULL );
#else
    name.Set( KNSmlDMSettingsDbName(), KNSmlDatabasesNonSecurePath, NULL );
#endif
            TInt err = iRdbSession.DeleteDatabase( name.FullName(),
                                              KNSmlSOSServerPolicyUID );
            CreateDatabaseL( name.FullName() );
            err = iDatabase.Open( iRdbSession,
		                      name.FullName(),
		                      KNSmlDBMSSecureSOSServerID );
             }
           User::LeaveIfError( iTableProfiles.Open( iDatabase, KNSmlDMTableProfiles ) );
	       iColSet = iTableProfiles.ColSetL();
		        
	}
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::CreateDMTablesL()
//		Creates settings database.
//------------------------------------------------------------------------------

void CNSmlDMSettings::CreateDMTablesL()
    {
    
    HBufC* createProfileTable = HBufC::NewLC( KDMCreateDMProfilesTable().Length() + KNSmlDmCreateDB); 
	TPtr profileTablePtr = createProfileTable->Des();

	profileTablePtr.Format(KDMCreateDMProfilesTable,	KNSmlDmMaxProfileNameLength,
													KNSmlDmMaxUsernameLength,
													KNSmlDmMaxPasswordLength,
													KNSmlDmSharedSecretMaxLength,
													KNSmlDmMaxURLLength,
													KNSmlDmServerIdMaxLength,
													KNSmlDmMaxHttpAuthUsernameLength,		
													KNSmlDmMaxHttpAuthPasswordLength);
													
	iDatabase.Begin();

	iDatabase.Execute( *createProfileTable );
	iDatabase.Execute( KDMCreateCommSetTable );


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
		
//	iDatabase.Close();
	CleanupStack::PopAndDestroy( 3 ); // createProfileTable, colSet, table													
													
    }
//------------------------------------------------------------------------------
//		CNSmlDMSettings::CreateDatabaseL()
//		Creates settings database.
//------------------------------------------------------------------------------
void CNSmlDMSettings::CreateDatabaseL(const TDesC& aFullName)
	{				
	HBufC* createProfileTable = HBufC::NewLC( KDMCreateDMProfilesTable().Length() + KNSmlDmCreateDB); 
	TPtr profileTablePtr = createProfileTable->Des();

	profileTablePtr.Format(KDMCreateDMProfilesTable,	KNSmlDmMaxProfileNameLength,
													KNSmlDmMaxUsernameLength,
													KNSmlDmMaxPasswordLength,
													KNSmlDmSharedSecretMaxLength,
													KNSmlDmMaxURLLength,
													KNSmlDmServerIdMaxLength,
													KNSmlDmMaxHttpAuthUsernameLength,		
													KNSmlDmMaxHttpAuthPasswordLength);

    User::LeaveIfError( iDatabase.Create( this->iRdbSession,
                                          aFullName,
                                          KNSmlDBMSSecureSOSServerID ) );  
	
	iDatabase.Begin();

	iDatabase.Execute( *createProfileTable );
	iDatabase.Execute( KDMCreateCommSetTable );

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
	CreateDMProfilesFromXmlL();
	iDatabase.Close();
	CleanupStack::PopAndDestroy( 3 ); // createProfileTable, colSet, table
	}
	
//=============================================
//		CNSmlDMSettings::CreateHiddenProfilesL()
//		Create new profiles reading data from
//		resource file
//=============================================

void CNSmlDMSettings::CreateHiddenProfilesL()
	{
	TFileName fileName;
	Dll::FileName( fileName );
	TParse parse;

	parse.Set( KNSmlDMProfilesRsc, &fileName, NULL );
	fileName = parse.FullName();

	RResourceFile resourceFile; 
	BaflUtils::NearestLanguageFile( iFsSession, fileName );

	TRAPD(leavecode,resourceFile.OpenL( iFsSession,fileName));
	if(leavecode != KErrNone )
		{
		return;
		}
	CleanupClosePushL(resourceFile);
		
	HBufC8* profileRes = resourceFile.AllocReadLC( NSML_DM_PROFILES );
	TResourceReader reader;
	reader.SetBuffer( profileRes );
		
	CNSmlDMResourceProfiles* profileResReader = CNSmlDMResourceProfiles::NewLC( reader, this );
	profileResReader->SaveProfilesL();
	CleanupStack::PopAndDestroy(3); // profileResReader, profileRes, resourceFile
	}

//=============================================
//		CNSmlDMSettings::CreateDMProfilesFromXmlL()
//		Create new profiles reading data from
//		XML file
//=============================================

void CNSmlDMSettings::CreateDMProfilesFromXmlL()
	{
	_DBG_FILE("CNSmlDMSettings::CreateDMProfilesFromXmlL(): begin");
	
	TBuf<KMaxXmlPathLength> xmlPath;
	
    CRepository* cenrep = NULL;
    cenrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys );
    TInt result = cenrep->Get( KDevManProfilesXMLFile, xmlPath );
    delete cenrep;
    cenrep = NULL;
	
	if((xmlPath.Length()) && (result == KErrNone))
	{
		TInt error = KErrNone;
		RFile wFile;	
		if(wFile.Open(iFsSession, xmlPath, EFileRead | EFileShareReadersOnly) != KErrNone)		
		{		
		return;
		}
		CleanupClosePushL(wFile);
		RXMLReader DMProfileParser;
		DMProfileParser.CreateL();		
		CleanupClosePushL(DMProfileParser);
		
		CDMProfileContentHandler* contentHandler = CDMProfileContentHandler::NewL(this);
		CleanupStack::PushL(contentHandler);
		DMProfileParser.SetContentHandler(contentHandler);

		//parse xml file
		TRAP(error, DMProfileParser.ParseL(wFile));
		if(error == KErrNone )
		{
			contentHandler->SaveProfilesL();			
		} 
		else
		{
			DBG_FILE_CODE(error, _S8("CNSmlDMSettings::CreateDMProfilesFromXmlL() : XML parser status"));
		}   
    
		CleanupStack::PopAndDestroy(3); // contentHandler, DMProfileParser, wFile			
	}		
	_DBG_FILE("CNSmlDMSettings::CreateDMProfilesFromXmlL(): end");	
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::CreateProfileL()
//		Returns a pointer to newly created CNSmlDMProfile object.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMProfile* CNSmlDMSettings::CreateProfileL()
	{
	CNSmlDMProfile* profile = CNSmlDMProfile::NewL( this->Database() );

	// Set default values for a profile
	profile->SetIntValue( EDMProfileId,							KNSmlNewObject );
	profile->SetIntValue( EDMProfileTransportId,				KUidNSmlMediumTypeInternet.iUid );

	profile->SetIntValue( EDMProfileIAPId,						-1 );
	profile->SetIntValue( EDMProfileServerAlertAction,			ESmlConfirmSync );
	profile->SetIntValue( EDMProfileAuthenticationRequired,		EFalse );
	profile->SetIntValue( EDMProfileDeleteAllowed,				ETrue );
	profile->SetIntValue( EDMProfileHttpAuthUsed,				EFalse );
	profile->SetIntValue( EDMProfileLock,				        ETrue );
							
	return profile;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::CreateProfileL()
//		Copies all profile data with given Id to
//		a newly created CNSmlDMProfile object.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMProfile* CNSmlDMSettings::CreateProfileL( const TInt aId  )
	{
	CNSmlDMProfile* profile = ProfileL( aId);

	if ( profile )
		{
		profile->SetIntValue( EDMProfileId, KNSmlNewObject );
		}
	
	return profile;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::Database()
//		Returns a reference to settings database object.
//------------------------------------------------------------------------------
	
EXPORT_C RDbNamedDatabase* CNSmlDMSettings::Database()
	{
	return &iDatabase;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ProfileL()
//		Returns a pointer to CNSmlDMProfile object with given Id.
//		If there's no corresponding Id in the database, returns NULL.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMProfile* CNSmlDMSettings::ProfileL( TInt aId)
	{
	_DBG_FILE("CNSmlDMSettings::ProfileL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, aId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	CNSmlDMProfile* profile = CNSmlDMProfile::NewL( Database() );

	if( iView.FirstL() )
		{
		CleanupStack::PushL( profile );
		CNSmlDMCrypt* crypt = new(ELeave) CNSmlDMCrypt;
		CleanupStack::PushL(crypt);
		iView.GetL();

		profile->SetIntValue( EDMProfileId,						ViewColUint( KNSmlDMProfileId ) );
		profile->SetStrValue( EDMProfileDisplayName,			ViewColDes( KNSmlDMProfileDisplayName ) );
		profile->SetIntValue( EDMProfileIAPId,					ViewColInt( KNSmlDMProfileIAPId ) );
		profile->SetIntValue( EDMProfileTransportId,			ViewColInt( KNSmlDMProfileTransportId ) );

		profile->SetStrValue( EDMProfileServerUsername, 		ViewColDes( KNSmlDMProfileServerUsername ) );
		profile->SetStrValue( EDMProfileServerPassword, 		crypt->DecryptedL(ViewColDes( KNSmlDMProfileServerPassword ) ) );
		profile->SetStrValue( EDMProfileClientPassword,         crypt->DecryptedL(ViewColDes( KNSmlDMProfileClientPassword ) ) );
		profile->SetStrValue( EDMProfileServerURL,              ViewColDes( KNSmlDMProfileServerURL ) );
		
		profile->SetStrValue( EDMProfileServerId,				ViewColDes( KNSmlDMProfileServerId ) );
		profile->SetIntValue( EDMProfileSessionId,				ViewColInt( KNSmlDMProfileSessionId ) );

		profile->SetIntValue( EDMProfileServerAlertAction, 		ViewColInt( KNSmlDMProfileServerAlertedAction ) );
		profile->SetIntValue( EDMProfileAuthenticationRequired,	ViewColInt( KNSmlDMProfileAuthenticationRequired ) );
		profile->SetIntValue( EDMProfileCreatorId,				ViewColInt( KNSmlDMProfileCreatorId ) );
		profile->SetIntValue( EDMProfileDeleteAllowed,				ViewColInt( KNSmlDMProfileDeleteAllowed) );
		profile->SetIntValue( EDMProfileHidden,		    		ViewColInt( KNSmlDMProfileHidden) );

		profile->SetIntValue( EDMProfileHttpAuthUsed, 			ViewColInt( KNSmlDMProfileHttpAuthUsed) );
		profile->SetStrValue( EDMProfileHttpAuthUsername, 		ViewColDes( KNSmlDMProfileHttpAuthUsername ) );
		profile->SetStrValue( EDMProfileHttpAuthPassword, 		crypt->DecryptedL(ViewColDes( KNSmlDMProfileHttpAuthPassword ) ) );
			profile->SetIntValue( EDMProfileLock,				ViewColInt( KNSmlDMProfileLock) );
		
		CleanupStack::PopAndDestroy(); // crypt
		CleanupStack::Pop(); // profile
		}
	else
		{
		delete profile;
		//couldn't find
		profile = NULL;
		}

	_DBG_FILE("CNSmlDMSettings::ProfileL(): end");
	return profile;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::GetProfileListL()
//		Appends CNSmlDMProfileListItem objects to aProfileList.
//------------------------------------------------------------------------------

EXPORT_C void CNSmlDMSettings::GetProfileListL( CNSmlDMProfileList* aProfileList )
	{
	_DBG_FILE("CNSmlDMSettings::GetProfileListL(): begin");

	aProfileList->ResetAndDestroy();

	PrepareViewL( KDMSQLGetAllProfiles, iView.EUpdatable );

	if( iView.Unevaluated() )
		{
		iView.EvaluateAll(); 	
		}
	

	while ( iView.NextL() )
		{
		AddToProfileListL( aProfileList );
		}

	_DBG_FILE("CNSmlDMSettings::GetProfileListL(): end");
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::DeleteProfileL()
//		Permanently deletes profile with given Id.
//		If delete is succesfull returns KErrNone,
//		otherwise returns negative integer 
//      (defined in EPOC error codes).
//------------------------------------------------------------------------------

EXPORT_C TInt CNSmlDMSettings::DeleteProfileL( TInt aId )
	{
	_DBG_FILE("CNSmlDMSettings::DeleteProfileL(): begin");
	if ( iDatabase.InTransaction() )
		{
		return ( KErrAccessDenied );
		}
	
	CNSmlDMProfile* profile = ProfileL(aId);
	if(profile)
		{
		CleanupStack::PushL(profile);
		RNSmlDMCallbackSession cbSession;
		User::LeaveIfError( cbSession.Connect() );
		CleanupClosePushL(cbSession);
		HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
			profile->StrValue(EDMProfileServerId));
		CleanupStack::PushL(serverId);
		
		cbSession.EraseServerIdL(*serverId);
		CleanupStack::PopAndDestroy(3);  // profile,cbSession,serverId
		}

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLDeleteProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();
	
	TInt err( KErrNone );

	iDatabase.Begin();

	sqlStatementPtr.Format( KDMSQLDeleteProfile, aId );
	err = iDatabase.Execute( sqlStatementPtr );

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	if ( err < KErrNone )
		{
		iDatabase.Rollback();
		return err;
		}

	CommitAndCompact();

	_DBG_FILE("CNSmlDMSettings::DeleteProfileL(): end");

	return KErrNone;
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::PrepareViewL()
//		Closes and prepares the view
//		
//------------------------------------------------------------------------------
void CNSmlDMSettings::PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess )
	{
	iView.Close();
	User::LeaveIfError( iView.Prepare( iDatabase, TDbQuery( aSql ), aAccess ) );
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::CommitAndCompact
//		Commits update and compacts the database
//		
//------------------------------------------------------------------------------
void CNSmlDMSettings::CommitAndCompact() 
	{
	iDatabase.Commit();
	iDatabase.Compact();
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::AddToProfileListL
//		Adds database field to profile list
//		
//------------------------------------------------------------------------------
void CNSmlDMSettings::AddToProfileListL( CNSmlDMProfileList* aProfileList )
	{
	iView.GetL();

	aProfileList->AppendL( CNSmlDMProfileListItem::NewLC( iView.ColUint( iColSet->ColNo( KNSmlDMProfileId ) ) ) );
	
	CleanupStack::Pop();
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ColDes
//		
//------------------------------------------------------------------------------
TPtrC CNSmlDMSettings::ViewColDes( const TDesC& aFieldName )
	{
	return iView.ColDes( iColSet->ColNo( aFieldName ) );
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ColInt
//		
//------------------------------------------------------------------------------
TInt CNSmlDMSettings::ViewColInt( const TDesC& aFieldName )
	{
	return iView.ColInt( iColSet->ColNo( aFieldName ) );
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ColUint
//	
//		
//------------------------------------------------------------------------------
TUint CNSmlDMSettings::ViewColUint( const TDesC& aFieldName )
	{
	return iView.ColUint( iColSet->ColNo( aFieldName ) );
	}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::IsDMAllowedL
//	
//		
//------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDMSettings::IsDMAllowedL()
{
	TBuf<30> sqlStatement;
	sqlStatement.Format( KDMSQLGetAllCommonSettings );

	RDbTable table;
	CleanupClosePushL( table );

	User::LeaveIfError( table.Open( iDatabase, KNSmlTableCommonSettings ) );
	
	CDbColSet* colSet = table.ColSetL();
	CleanupStack::PushL( colSet );
	
	RDbView lView;
	CleanupClosePushL( lView );
	User::LeaveIfError( lView.Prepare( iDatabase, TDbQuery( sqlStatement ), lView.EReadOnly ) );

	TBuf<10> columnName;
	columnName.Format( KLogColumn);

	iAllowed = ETrue;

	iDatabase.Begin();	
	if ( lView.FirstL() )
		{
		lView.GetL();

		iAllowed = lView.ColInt( colSet->ColNo( columnName ));

		}	

	iDatabase.Commit();

	CleanupStack::PopAndDestroy(3);  //table, colSet, lView
	return (iAllowed);
}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::ServerIdFoundL
//	
//		
//------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDMSettings::ServerIdFoundL( const TDesC& aServerId )
{
	_DBG_FILE("CNSmlDMSettings::ServerIdFoundL(): begin");

	TBool ret = EFalse;
	if(aServerId.Length()>0)
	    {
	    HBufC* sqlStatement = HBufC::NewLC( KDMSQLSelectMatchingServerIdOnly().Length() + aServerId.Length());
        TPtr sqlStatementPtr = sqlStatement->Des();
        sqlStatementPtr.Format(KDMSQLSelectMatchingServerIdOnly, &aServerId);
    	PrepareViewL( sqlStatementPtr, iView.EReadOnly );

    	CleanupStack::PopAndDestroy();  // sqlStatement
    	
    	if ( iView.FirstL() )
    		{
    		ret = ETrue;
    		}
	    }
	_DBG_FILE("CNSmlDMSettings::ServerIdFoundL(): end");
    return ret;
}
//------------------------------------------------------------------------------
//		CNSmlDMSettings::ServerIdFoundL
//	
//		
//------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDMSettings::ServerIdFoundL( const TDesC& aServerId, TInt aProfileId )
{
	_DBG_FILE("CNSmlDMSettings::ServerIdFoundL(): begin");

	TBool ret = EFalse;
	if(aServerId.Length()>0)
	    {
	    HBufC* sqlStatement = HBufC::NewLC( KDMSQLSelectMatchingServerId().Length() + aServerId.Length() + KNSmlDmMaxIntegerLength );
        TPtr sqlStatementPtr = sqlStatement->Des();
        sqlStatementPtr.Format(KDMSQLSelectMatchingServerId, &aServerId, aProfileId );
    	PrepareViewL( sqlStatementPtr, iView.EReadOnly );

    	CleanupStack::PopAndDestroy();  // sqlStatement
    	
    	if ( iView.FirstL() )
    		{
    		ret = ETrue;
    		}
	    }
	_DBG_FILE("CNSmlDMSettings::ServerIdFoundL(): end");
    return ret;
}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::SetDMAllowedFlagL
//	
//		
//------------------------------------------------------------------------------
EXPORT_C void CNSmlDMSettings::SetDMAllowedFlagL(TBool aAllowed)
{
	_DBG_FILE("CNSmlDMSettings::SetDMAllowedFlagL(): begin");

	TBuf<30> sqlStatement;
	sqlStatement.Format( KDMSQLGetAllCommonSettings);

	RDbTable table;
	CleanupClosePushL( table );

	User::LeaveIfError( table.Open( iDatabase, KNSmlTableCommonSettings ) );
	
	CDbColSet* colSet = table.ColSetL();
	CleanupStack::PushL( colSet );
	
	RDbView lView;
	CleanupClosePushL( lView );
	User::LeaveIfError( lView.Prepare( iDatabase, TDbQuery( sqlStatement ), lView.EUpdatable ) );

	TBuf<10> columnName;
	columnName.Format( KLogColumn);

	iDatabase.Begin();	
	if ( lView.FirstL() )
		{
		lView.GetL();
		lView.UpdateL();

		lView.SetColL( colSet->ColNo( columnName ), aAllowed);

		}
	else
		{
		lView.InsertL();
		lView.SetColL( colSet->ColNo( columnName ), aAllowed );
		}
	lView.PutL();

	CleanupStack::PopAndDestroy(3);  //table, colSet, lView

	iDatabase.Commit();
	iDatabase.Compact();

	_DBG_FILE("CNSmlDMSettings::SetDMAllowedFlagL(): end");
}

//------------------------------------------------------------------------------
//		CNSmlDMSettings::OldProfileL
//	
//		
//------------------------------------------------------------------------------

CNSmlDMProfile* CNSmlDMSettings::OldProfileL( TInt aId)
	{
	_DBG_FILE("CNSmlDMSettings::OldProfileL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, aId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	CNSmlDMProfile* profile = CNSmlDMProfile::NewL( Database() );

	if( iView.FirstL() )
		{
		CleanupStack::PushL( profile );
		CNSmlDMCrypt* crypt = new(ELeave) CNSmlDMCrypt;
		CleanupStack::PushL(crypt);
		iView.GetL();

		profile->SetIntValue( EDMProfileId,						ViewColUint( KNSmlDMProfileId ) );
		profile->SetStrValue( EDMProfileDisplayName,			ViewColDes( KNSmlDMProfileDisplayName ) );
		profile->SetIntValue( EDMProfileIAPId,					ViewColInt( KNSmlDMProfileIAPId ) );
		profile->SetIntValue( EDMProfileTransportId,			ViewColInt( KNSmlDMProfileTransportId ) );

		profile->SetStrValue( EDMProfileServerUsername, 		ViewColDes( KNSmlDMProfileServerUsername ) );
		profile->SetStrValue( EDMProfileServerPassword, 		crypt->DecryptedL(ViewColDes( KNSmlDMProfileServerPassword ) ) );
		profile->SetStrValue( EDMProfileClientPassword,         crypt->DecryptedL(ViewColDes( KNSmlDMProfileClientPassword ) ) );
		profile->SetStrValue( EDMProfileServerURL,              ViewColDes( KNSmlDMProfileServerURL ) );
		
		profile->SetStrValue( EDMProfileServerId,				ViewColDes( KNSmlDMProfileServerId ) );
		profile->SetIntValue( EDMProfileSessionId,				ViewColInt( KNSmlDMProfileSessionId ) );

		profile->SetIntValue( EDMProfileServerAlertAction, 		ViewColInt( KNSmlDMProfileServerAlertedAction ) );
		profile->SetIntValue( EDMProfileAuthenticationRequired,	ViewColInt( KNSmlDMProfileAuthenticationRequired ) );
		profile->SetIntValue( EDMProfileCreatorId,				ViewColInt( KNSmlDMProfileCreatorId ) );
		profile->SetIntValue( EDMProfileDeleteAllowed,				ViewColInt( KNSmlDMProfileDeleteAllowed) );
		
		if(iMissingCols.Find(TPtrC(KNSmlDMProfileHidden))==KErrNotFound)
		    profile->SetIntValue( EDMProfileHidden,		    		ViewColInt( KNSmlDMProfileHidden) );

		profile->SetIntValue( EDMProfileHttpAuthUsed, 			ViewColInt( KNSmlDMProfileHttpAuthUsed) );
		profile->SetStrValue( EDMProfileHttpAuthUsername, 		ViewColDes( KNSmlDMProfileHttpAuthUsername ) );
		profile->SetStrValue( EDMProfileHttpAuthPassword, 		crypt->DecryptedL(ViewColDes( KNSmlDMProfileHttpAuthPassword ) ) );
		if(iMissingCols.Find(TPtrC(KNSmlDMProfileLock))==KErrNotFound)		
		  profile->SetIntValue( EDMProfileLock,				ViewColInt( KNSmlDMProfileLock) );
		
		CleanupStack::PopAndDestroy(); // crypt
		CleanupStack::Pop(); // profile
		}
	else
		{
		delete profile;
		//couldn't find
		profile = NULL;
		}

	_DBG_FILE("CNSmlDMSettings::OldProfileL(): end");
	return profile;
	}
	
//  End of File

