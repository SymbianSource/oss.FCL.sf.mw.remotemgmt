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


#ifndef __NSMLDMSETTINGS_H
#define __NSMLDMSETTINGS_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <s32strm.h>
#include <f32file.h>

// CONSTANTS
_LIT(KNSmlDMProfilesRsc,"\\Resource\\NSMLDMPROFILERES.RSC");

const TInt  KNSmlDmSettingsFatMinSize=200;
const TInt  KNSmlDmCreateDB=50;
const TInt  KNSmlDmMaxIntegerLength=16;
const TInt  KNSmlDMDriveLetter = EDriveC;
const TInt  KNSmlDMProfileTableColCount = 23;  //correct this after somemtime
const TInt  KMaxXmlPathLength = 1024;

//Database name & location
_LIT( KNSmlDMSettingsDbName, "c:nsmldmsettings.db" ); 

//Database tables
_LIT( KNSmlDMTableProfiles, "Profiles" );
_LIT( KNSmlTableCommonSettings, "CommonSettings" );

//table Profiles columns
_LIT( KNSmlDMProfileId,						 "Id" );
_LIT( KNSmlDMProfileDisplayName,			 "DisplayName" );
_LIT( KNSmlDMProfileIAPId,				 	 "IAPId" );
_LIT( KNSmlDMProfileTransportId,		 	 "TransportId" );
_LIT( KNSmlDMProfileServerUsername,			 "ServerUsername" );
_LIT( KNSmlDMProfileServerPassword,			 "ServerPassword" );
_LIT( KNSmlDMProfileServerNonce,			 "ServerNonce" );
_LIT( KNSmlDMProfileClientPassword,			 "ClientPassword" );
_LIT( KNSmlDMProfileClientNonce,			 "ClientNonce" );
_LIT( KNSmlDMProfileServerURL,				 "ServerURL" );
_LIT( KNSmlDMProfileServerId,				 "ServerId" );
_LIT( KNSmlDMProfileSessionId,				 "DMSessionId" );
_LIT( KNSmlDMProfileServerAlertedAction,	 "ServerAlertedAction" );
_LIT( KNSmlDMProfileAuthenticationRequired,	 "AuthenticationRequired" );
_LIT( KNSmlDMProfileCreatorId,				 "CreatorId" );
_LIT( KNSmlDMProfileDeleteAllowed,			 "DeleteAllowed" );
_LIT( KNSmlDMProfileHidden,					 "HiddenProfile" );
_LIT( KNSmlDMProfileLog,					 "Log" );
_LIT( KNSmlDMProfileHttpAuthUsed,		 "HttpAuthUsed" );
_LIT( KNSmlDMProfileHttpAuthUsername,	 "HttpAuthUsername" );
_LIT( KNSmlDMProfileHttpAuthPassword,	 "HttpAuthPassword" );
_LIT(  KNSmlDMLastSync ,                 "LastSync");
_LIT(  KNSmlDMProfileLock ,                 "ProfileLock");

// Table commonsettings columns
_LIT( KLogColumn, "DMAllowed");

// SQL clauses
_LIT( KDMCreateDMProfilesTable, "CREATE TABLE Profiles ( Id COUNTER, DisplayName CHAR(%d), IAPId SMALLINT NOT NULL, TransportId INTEGER, ServerUsername CHAR(%d), ServerPassword CHAR(%d), ServerNonce LONG VARCHAR, ClientPassword CHAR(%d), ClientNonce LONG VARCHAR, ServerURL CHAR(%d), ServerId CHAR(%d), DMSessionId UNSIGNED SMALLINT, ServerAlertedAction SMALLINT, AuthenticationRequired UNSIGNED TINYINT, CreatorId INTEGER, DeleteAllowed BIT,HiddenProfile BIT, Log LONG VARBINARY, LastSync LONG VARBINARY, HttpAuthUsed BIT, HttpAuthUsername CHAR(%d), HttpAuthPassword CHAR(%d),ProfileLock BIT )" );
_LIT( KDMCreateCommSetTable, "CREATE TABLE CommonSettings ( DMAllowed BIT NOT NULL)" ); 
_LIT( KDMSQLGetProfile, "SELECT * FROM Profiles WHERE Id = %d" );
_LIT( KDMSQLGetAllProfiles, "SELECT * FROM Profiles ORDER BY Id" );
_LIT( KDMSQLDeleteProfile, "DELETE FROM Profiles WHERE Id = %d" );
_LIT( KDMSQLGetAllCommonSettings, "SELECT * FROM CommonSettings");
_LIT( KDMSQLSelectMatchingServerId, "SELECT * FROM Profiles WHERE ServerId = '%S' AND Id <> %d");
_LIT( KDMSQLSelectMatchingServerIdOnly, "SELECT * FROM Profiles WHERE ServerId = '%S'");

//enumerations

enum TNSmlDMProfileData
	{
	EDMProfileId,
	EDMProfileDisplayName,
	EDMProfileIAPId,
	EDMProfileTransportId,
	EDMProfileServerUsername,
	EDMProfileServerPassword,
	EDMProfileServerNonce,
	EDMProfileClientPassword,
	EDMProfileClientNonce,
	EDMProfileServerURL,
	EDMProfileServerId,
	EDMProfileSessionId,
	EDMProfileServerAlertAction,
	EDMProfileAuthenticationRequired,
	EDMProfileCreatorId,
	EDMProfileDeleteAllowed,
	EDMProfileHttpAuthUsed,
	EDMProfileHttpAuthUsername,
	EDMProfileHttpAuthPassword,
	EDMProfileHidden,
	EDMProfileLock
	};


enum TNSmlDMTable
	{
	EDMProfiles
	};

enum TNSmlDMProfileAllowed
	{
	EDMNotAllowed,
	EDMAllowed
	};

enum TNSmlDMEnabled
	{
	EDMDisabled,
	EDMEnabled
	};


// FORWARD DECLARATIONS
class CNSmlDMSettings;
class CNSmlDMProfile;
class CNSmlDMProfileListItem;
class CNSmlDMCrypt;

// CLASS DECLARATION

/**
*  CNSmlDMProfileListItem contains profile id. It is used to list all id's of profiles.
*
*  @lib nsmldmsettings.lib
*/
class CNSmlDMProfileListItem : public CBase
{	
	public:
		/**
		* two-phase constructor - Creates new CNSmlDMProfileListItem*
		* @param TInt aId - id of profile
		* @return CNSmlDMProfileListItem* - item that was created
		*/
		IMPORT_C static CNSmlDMProfileListItem* NewL( const TInt aId );
		
		/**
		* two-phase constructor - Creates new CNSmlDMProfileListItem*, 
		* which is left into cleanup stack.
		* @param TInt aId - id of profile
		* @return CNSmlDMProfileListItem* - item that was created
		*/
		IMPORT_C static CNSmlDMProfileListItem* NewLC( const TInt aId );
				
		/**
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDMProfileListItem();

		/**
		* Returns value of given integer field - only profile id
		* @param TNSmlDMProfileData aProfileItem - field enumeration
		* @return TInt - id of profile
		*/
		IMPORT_C TInt IntValue( TNSmlDMProfileData aProfileItem ) const;
		
		/**
		* Copies values from sourceitem
		* @param CNSmlDMProfileListItem* aSourceItem - Item form which values are copied from
		* @return -
		*/
		IMPORT_C void AssignL( const CNSmlDMProfileListItem* aSourceItem );

	private:
		/**
		* two-phase construction
		* @param -
		* @return -
		*/
		void ConstructL();

	private:
		TInt   iId;

};

typedef CArrayPtr<CNSmlDMProfileListItem> CNSmlDMProfileList;

/**
*  CNSmlDMSettings provides an interface to dm settings database
*
*  @lib nsmldmsettings.lib
*/
class CNSmlDMSettings : public CBase
{
	public:
		/**
		* two-phase constructor - Creates new CNSmlDMSettings*
		* @param -
		* @return CNSmlDMSettings* - new CNSmlDMSettings
		*/
		IMPORT_C static CNSmlDMSettings* NewL();	
		
		/**
		* two-phase constructor - Creates new CNSmlDMSettings*, which
		* is left in cleanup stack
		* @param -
		* @return CNSmlDMSettings* - new CNSmlDMSettings
		*/
		IMPORT_C static CNSmlDMSettings* NewLC();
		
		/**
		* Destrcutor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDMSettings();

		/**
		* Creates new profile
		* @param -
		* @return CNSmlDMProfile* - new profile
		*/
		IMPORT_C CNSmlDMProfile* CreateProfileL();
		
		/**
		* Creates new profile with values from given profile id
		* @param TInt aId - profile that values are copied from
		* @return CNSmlDMProfile* - new profile
		*/
		IMPORT_C CNSmlDMProfile* CreateProfileL( const TInt aId );

		/**
		* List all profile id's into array
		* @param CNSmlDMProfileList* aProfileList - array to fill with profile id's
		* @return -
		*/
		IMPORT_C void GetProfileListL( CNSmlDMProfileList* aProfileList );
		
		/**
		* Gets profile with given id
		* @param TInt aId - Profile id
		* @return CNSmlDMProfile* - profile
		*/
		IMPORT_C CNSmlDMProfile* ProfileL( TInt aId );

		/**
		* Deletes profile with given id
		* @param TInt aId - Profile id
		* @return TInt - KErrNone if succesfull, other system-wide errorcode otherwise
		*/
		IMPORT_C TInt DeleteProfileL( TInt aId );

		/**
		* Returns ETrue if given server id exists in database
		* @param const TDesC& aServerId - Server id
		* @return TBool - ETrue if server id was found
		*/
		IMPORT_C TBool ServerIdFoundL(const TDesC& aServerId );
		IMPORT_C TBool ServerIdFoundL(const TDesC& aServerId,TInt aProfileId);
		
		/**
		* Scan And Repair RestoredProfiles in database
		* @param -
		* @return - 
		*/
		IMPORT_C void ScanAndRepairRestoredProfilesL();
		/**
		* Returns ETrue if DMAllowed value in commonsettings table is ETrue
		* @param -
		* @return TBool - ETrue DMAllowed value is ETrue
		*/
		IMPORT_C TBool IsDMAllowedL();
		
		/**
		* Sets value of DMAllowed in commonsettings table
		* @param TBool aAllowed - new value
		* @return -
		*/
		IMPORT_C void SetDMAllowedFlagL( TBool aAllowed );

		/**
		* returns database
		* @param -
		* @return RDbNamedDatabase* - the database
		*/
		IMPORT_C RDbNamedDatabase* Database();
	
	private:
	
		/**
		* Creates new database and tables. 
		* @param const TDesC& aFullName - Name and path of database
		* @return -
		*/
		void CreateDatabaseL( const TDesC& aFullName );
		
		/**
		* Creates tables in the dm settings database
		* @param -
		* @return -
		*/
		void CreateDMTablesL();
		
		/**
		* Creates CNSmlDMProfile with older release database columns
		* @param - TInt aId
		* @return - CNSmlDMProfile*
		*/
		CNSmlDMProfile* OldProfileL( TInt aId);
		
		/**
		* Creates profiles from resources
		* @param -
		* @return -
		*/
		void CreateHiddenProfilesL();

		/**
		* Creates profiles from XML
		* @param -
		* @return -
		*/
		void CreateDMProfilesFromXmlL();
		
		/**
		* Prepares view for updating
		* @param -
		* @return -
		*/
		void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );
		
		/**
		* Commits and compacts database
		* @param -
		* @return -
		*/
		void CommitAndCompact();
		
		/**
		* Adds item to list
		* @param CNSmlDMProfileList* aProfileList - list of profiles to add the item
		* @return -
		*/
		void AddToProfileListL( CNSmlDMProfileList* aProfileList );
		
		/**
		* Returns data from given descriptor field
		* @param const TDesC& aFieldName - name of field
		* @return TPtrC - the descriptor
		*/
		TPtrC ViewColDes( const TDesC& aFieldName );
		
		/**
		* Returns data from given integer field
		* @param const TDesC& aFieldName - name of field
		* @return TInt - the integer
		*/
		TInt ViewColInt( const TDesC& aFieldName );
		
		/**
		* Returns data from given unsigned integer field
		* @param const TDesC& aFieldName - name of field
		* @return TUint- the unsigned integer
		*/		
		TUint ViewColUint( const TDesC& aFieldName );
			
	private:		
		RDbNamedDatabase iDatabase;
		RFs iFsSession;
		RDbs iRdbSession;
		RDbTable iTableProfiles;
		CDbColSet* iColSet;
		RDbView iView;
		TBool iAllowed;
		RArray<TPtrC> iMissingCols;
		RPointerArray<CNSmlDMProfile> iOldProfiles;
};


/**
*  CNSmlDMProfile provides methods to update and save profile data
*
*  @lib nsmldmsettings.lib
*/
class CNSmlDMProfile : public CBase
{
	public:
		/**
		* two-phase constructor - Creates new CNSmlDMProfile*
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDMProfile* - new profile
		*/
		static CNSmlDMProfile* NewL( RDbNamedDatabase* aDatabase );	
		
		/**
		* two-phase constructor - Creates new CNSmlDMProfile*, which
		* is left in cleanup stack
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDMProfile* - new profile
		*/
		static CNSmlDMProfile* NewLC( RDbNamedDatabase* aDatabase );
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDMProfile();

		/**
		* Saves profile, i.e. either creates a new one in database or updates existing
		* @param -
		* @return TInt - KErrNone if succesfull, systemwide errorcode otherwise
		*/
		IMPORT_C TInt SaveL();

		/**
		* Returns value of given descriptor field
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field
		* @return const TDesC& - the descriptor 
		*/
		IMPORT_C const TDesC& StrValue( TNSmlDMProfileData aProfileItem ) const;
		
		/**
		* Returns value of given integer field
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field
		* @return const TInt - the integer
		*/
		IMPORT_C TInt IntValue( TNSmlDMProfileData aProfileItem ) const;
		
		/**
		* Returns value of client nonce or server nonce
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field
		* @return HBufC* - value of nonce, that is left in cleanup stack
		*/
		IMPORT_C HBufC* CNSmlDMProfile::NonceLC( TNSmlDMProfileData aProfileItem);

		/**
		* Sets new value for descriptor
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field that is updated
		* @param const TDesC& aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetStrValue( TNSmlDMProfileData aProfileItem, const TDesC& aNewValue );
		
		/**
		* Sets new value for integer
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field that is updated
		* @param const TInt aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetIntValue( TNSmlDMProfileData aProfileItem, const TInt aNewValue );
		
		/**
		* Sets new value for client nonce or server nonce
		* @param TNSmlDMProfileData aProfileItem - Enumeration of the field that is updated
		* @param const TInt aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetNonceL( TNSmlDMProfileData aProfileItem, const TDesC& aNewValue );

		/**
		* Returns true if profile has not been saved yet
		* @param -
		* @return TBool - ETrue if profile has not been saved yet
		*/
		IMPORT_C TBool IsNew() const;
		
		/**
		* Returns true if profile has the same data as given profile
		* @param CNSmlDMProfile* profile - profile to compare data with
		* @return TBool - ETrue if both profiles have the same data
		*/
		IMPORT_C TBool EqualData( CNSmlDMProfile* profile ) const;
		
		/**
		* Returns true if value of given field is NULL
		* @param const TDesC& aFieldName - Name of the field
		* @return TBool - ETrue if value of given field is NULL
		*/
		IMPORT_C TBool IsNullL( const TDesC& aFieldName );
		
		/**
		* Returns true if profile has log
		* @param -
		* @return TBool - ETrue if profile has log
		*/
		IMPORT_C TBool HasLogL(); 

		/**
		* Returns true if profile has last sync time
		* @param -
		* @return TBool - ETrue if profile has last sync time
		*/
        IMPORT_C TBool HasLastSyncL(); 

		/**
		* Returns stream to read log data from
		* @param -
		* @return RReadStream& - stream to read profile log
		*/
		IMPORT_C RReadStream& LogReadStreamL();
		
		/**
		* Returns stream to read log from
		* @param -
		* @return RReadStream& - stream to read profile log
		*/
		IMPORT_C RWriteStream& LogWriteStreamL();
		
		/**
		* Commits writing to stream
		* @param -
		* @return -
		*/
		IMPORT_C void WriteStreamCommitL();

		/**
		* Returns stream to write Last Success Sync log from
		* @param -
		* @return RWriteStream& - stream to write Last Success Sync of profile 
		*/
		IMPORT_C RWriteStream& LastSyncWriteStreamL();

		/**
		* Returns stream to read Last Success Sync log from
		* @param -
		* @return RReadStream& - stream to read Last Success Sync of profile 
		*/
		IMPORT_C RReadStream& LastSyncReadStreamL();

		/**
		* Commits Last sync time writing to stream
		* @param -
		* @return -
		*/
		IMPORT_C void LastSyncStreamCommitL();
		
		/**
		* Resets the database reference 
		* @param - RDbNamedDatabase* aDatabase
		* @return -
		*/		
		void ResetDatabaseL( RDbNamedDatabase* aDatabase );
		
		/**
		* Resets the profile ID to create new profile
		* @param - 
		* @return -
		*/
		void ResetID();
		
		/**
		* Resets table references 
		* @param - 
		* @return -
		*/
		void CloseDbHandles();

	private:
		/**
		* two-phase construction
		* @param -
		* @return -
		*/
		void ConstructL();		
		
		/**
		* two-phase construction
		* @param RDbNamedDatabase* aDatabase - the database
		* @return -
		*/
		void ConstructL( RDbNamedDatabase* aDatabase );	
			
		/**
		* Prepares view for updating
		* @param const TDesC& aSql - SQL clause to use
		* @param RDbRowSet::TAccess aAccess - access level that is needed
		* @return -
		*/
		void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );

		/**
		* Commits changes and compacts database
		* @param -
		* @return -
		*/
		void CommitAndCompact();
		
		/**
		* Sets new integer value
		* @param const TDesC& aFieldName - name of the field
		* @param TInt aValue - Value of field
		* @return -
		*/
		void TableProfilesSetColIntL( const TDesC& aFieldName, TInt aValue );
		
		/**
		* Sets new descriptor value
		* @param const TDesC& aFieldName - name of the field
		* @param TDesC& aValue - Value of field
		* @return -
		*/
		void TableProfilesSetColDesL( const TDesC& aFieldName, TDesC& aValue );
		
		/**
		* Updates integer value
		* @param const TDesC& aFieldName - name of the field
		* @param TInt aValue - Value of field
		* @return -
		*/
		void ViewSetColIntL( const TDesC& aFieldName, TInt aValue );
		
		/**
		* Updates descriptor value
		* @param const TDesC& aFieldName - name of the field
		* @param TDesC& aValue - Value of field
		* @return -
		*/
		void ViewSetColDesL( const TDesC& aFieldName, TDesC& aValue );

	private:
		TUint iId;
		HBufC* iDisplayName;

		TInt iTransportId;
		TInt iIAPId;		
		
		HBufC* iDMServerUsername;		
		HBufC* iDMServerPassword;
		HBufC* iClientPassword;
		
		TBool iHttpAuthUsed;
		HBufC* iHttpAuthUsername;		
		HBufC* iHttpAuthPassword;

		HBufC* iServerURL;
		HBufC* iServerId;

		TInt  iDMSessionId;
		TInt iServerAlertAction;
		TInt  iAuthenticationRequired;
		TInt iCreatorId;
		TBool iDeleteAllowed;
		TBool iProfileHidden;
        TBool iProfileLock;
		RDbNamedDatabase* iDatabase;

		RDbTable iTableProfiles;

		TBool iWriteStreamOpen;
		TBool iReaDMtreamOpen;

		RDbColWriteStream iWs;
		RDbColReadStream  iRs;

		RDbView iRSView;
		RDbView iWSView;

		CDbColSet* iColSetProfiles;

		RDbView iView;
};

/**
*  CNSmlDMCrypt handles encryption / decryption of data
*
*  @lib nsmldmsettings.lib
*/
class CNSmlDMCrypt : public CBase
{
	public:
		/**
		* Constructor
		* @param -
		* @return -
		*/
		IMPORT_C CNSmlDMCrypt();
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDMCrypt();
		
		/**
		* Encrypts data
		* @param const TDesC& aInput - data to be encrypted
		* @return TDesC& - Encrypted data
		*/
		IMPORT_C TDesC& EncryptedL( const TDesC& aInput );
		
		/**
		* Decrypts data
		* @param const TDesC& aInput - data to be decrypted
		* @return TDesC& - Decrypted data
		*/
	    IMPORT_C TDesC& DecryptedL( const TDesC& aInput );
	
	private:
		HBufC* iOutput;
};

#endif  //__NSMLDMSETTINGS_H
