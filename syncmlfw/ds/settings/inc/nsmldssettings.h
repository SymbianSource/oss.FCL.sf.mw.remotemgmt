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


#ifndef __NSMLDSSETTINGS_H
#define __NSMLDSSETTINGS_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <s32strm.h>
#include <f32file.h>
#include <SyncMLDef.h>
#include <SyncMLDataFilter.h>
// CONSTANTS

const TInt KNsmlDsSuspendResume = 1;//RD_SUSPEND_RESUME
const TInt KNsmlDsAutoRestart = 7; //(0x00000007) RD_AUTO_RESTART

const TInt  KNSmlDsSettingsFatMinSize=1000;
const TInt  KNSmlDsSettingsMaxIntegerLength=16;

_LIT(KNSmlDSProfilesRsc,"\\Resource\\NSMLDSPROFILERES.RSC");

// Constants for visibility
// The visibility array size, equals to the amount of 
// fields in TNSmlDSProfileData-enum!
const TInt KNSmlDSVisibilityArraySize=18;
_LIT( KNSmlDefaultVisibility, "nnnnnnnnnnnnnnnnnn"); // Default value for the visibility
_LIT( KNSmlDSVisibilityHidden, "h");
_LIT( KNSmlDSVisibilityReadOnly, "r");
_LIT( KNSmlDSVisibilityNormal, "n");
const TText KNSmlDSHiddenChar = 'h';
const TText KNSmlDSReadOnlyChar = 'r';
const TText KNSmlDSNormalChar = 'n';

_LIT(KReadOnlyVisiblity ,"nrhrnnrrhnrnnrnrnn");
const TInt KNSmlMaxAdapterIdLength = 10;
const TInt KAccessPointNameMaxLen = 50;

//Database name & location
_LIT( KNSmlSettingsDbName, "c:nsmldssettings.db" ); 

//Database tables
_LIT( KNSmlTableProfiles, "Profiles" );
_LIT( KNSmlTableAdapters, "Adapters" );

// central repository uid for variating XML profile definition
const TUid KCRUidDataSyncInternalKeys  = { 0x2000CF7E };
const TUid KCRUidDSDefaultProfileInternalKeys  = { 0x20021338 };
const TInt KNsmlDsCustomProfiles = 0;
const TInt KNsmlDsDefaultProfile = 0;

_LIT(KEmpty, "");

const TInt KDSMaxURLLength = 144;
const TInt KDSMaxRemoteNameLength = 125;

// sync adapder uids
const TUid KUidNSmlAdapterCalendar  = { 0x101F6DDE };
const TUid KUidNSmlAdapterContact   = { 0x101F6DDD };
const TUid KUidNSmlAdapterEMail     = { 0x101F6DDF };
const TUid KUidNSmlAdapterNote      = { 0x101F8612 };
const TUid KUidNSmlAdapterSms       = { 0x10206B5C };
const TUid KUidNSmlAdapterMMS       = { 0x101FB0E9 };
const TUid KUidNSmlAdapterBookmarks = { 0x102751BA };

//table Profiles columns
_LIT( KNSmlDSProfileId,				 	 "Id" );
_LIT( KNSmlDSProfileDisplayName,		 "DisplayName" );
_LIT( KNSmlDSProfileServerURL,			 "ServerURL" );
_LIT( KNSmlDSProfileIAPId,				 "IAPid" );
_LIT( KNSmlDSProfileTransportId,		 "TransportId" );
_LIT( KNSmlDSProfileSyncServerUsername,	 "SyncServerUsername" );
_LIT( KNSmlDSProfileSyncServerPassword,	 "SyncServerPassword" );
_LIT( KNSmlDSProfileServerAlertedAction, "ServerAlertedAction" );
_LIT( KNSmlDSProfileDeleteAllowed,	 	 "DeleteAllowed" );
_LIT( KNSmlDSProfileHidden,			 	 "HiddenProfile" );
_LIT( KNSmlDSProfileLog,				 "Log" );
_LIT( KNSmlDSProfileHttpAuthUsed,		 "HttpAuthUsed" );
_LIT( KNSmlDSProfileHttpAuthUsername,	 "HttpAuthUsername" );
_LIT( KNSmlDSProfileHttpAuthPassword,	 "HttpAuthPassword" );
_LIT( KNSmlDSProfileAutoChangeIAP,	 	 "AutoChangeIAP" );
_LIT( KNSmlDSProfileVisibilityStr,	 	 "Visibility" );
_LIT( KNSmlDSProfileCreatorID,		 	 "CreatorID" );
_LIT( KNSmlDSProfileServerId,			 "ServerId" );
_LIT( KNSmlDSProfileProtocolVersion,	 "ProtocolVersion" );
_LIT( KNSmlDSProfileDefaultProfile,      "DefaultProfile" );    

//table Adapters columns
_LIT( KNSmlAdapterId,				"Id" );
_LIT( KNSmlAdapterProfileId,		"ProfileId" );
_LIT( KNSmlAdapterImlementationUID,	"ImplementationUID" );
_LIT( KNSmlAdapterEnabled,			"Enabled" );
_LIT( KNSmlAdapterCreatorID,		"CreatorID" );
_LIT( KNSmlAdapterFilter,			"Filter" );
_LIT( KNSmlAdapterDisplayName,		"DisplayName" );
_LIT( KNSmlAdapterServerDataSource,	"ServerDataSource" );
_LIT( KNSmlAdapterClientDataSource,	"ClientDataSource" );
_LIT( KNSmlAdapterSyncType,			"SyncType" );
_LIT( KNSmlAdapterFilterMatchType,	"FilterMatchType" );

// SQL clauses
_LIT( KDSCreateProfilesTable, "CREATE TABLE Profiles ( Id COUNTER, DisplayName CHAR(%d), IAPId SMALLINT NOT NULL, ProtocolVersion UNSIGNED SMALLINT NOT NULL, TransportId INTEGER, SyncServerUsername CHAR(%d), SyncServerPassword CHAR(%d), ServerURL CHAR(%d), ServerId CHAR(%d), ServerAlertedAction UNSIGNED SMALLINT, DeleteAllowed BIT, HiddenProfile BIT, Log LONG VARBINARY, HttpAuthUsed BIT, HttpAuthUsername CHAR(%d), HttpAuthPassword CHAR(%d), AutoChangeIAP BIT, Visibility CHAR(%d), CreatorID INTEGER)" );
_LIT( KDSCreateAdaptersTable, "CREATE TABLE Adapters ( Id COUNTER, ProfileId UNSIGNED SMALLINT NOT NULL, ImplementationUID UNSIGNED INTEGER NOT NULL, Enabled BIT NOT NULL, CreatorID INTEGER, Filter LONG VARBINARY, SyncType UNSIGNED SMALLINT, DisplayName CHAR(%d), ServerDataSource CHAR(%d), ClientDataSource CHAR(%d), FilterMatchType UNSIGNED SMALLINT )");
_LIT( KDSSQLGetProfileId, "SELECT ProfileId FROM Adapters WHERE ImplementationUID = %d" );
_LIT( KDSSQLGetProfile, "SELECT * FROM Profiles WHERE Id = %d" );
_LIT( KDSSQLGetProfiles, "SELECT * FROM Profiles WHERE HiddenProfile = 0 ORDER BY Id" );
_LIT( KDSSQLGetAllProfiles, "SELECT * FROM Profiles ORDER BY Id" );
_LIT( KDSSQLDeleteProfile,"DELETE FROM Profiles WHERE Id = %d" );
_LIT( KDSSQLDeleteAdapter,"DELETE FROM Adapters WHERE ProfileId = %d" );
_LIT( KDSSQLDeleteAdapterById, "DELETE FROM Adapters WHERE Id = %d" );
_LIT( KDSSQLGetAdapterId, "SELECT Id FROM Adapters WHERE Id = %d" );
_LIT( KDSSQLCountProfiles, "SELECT * FROM Profiles WHERE HiddenProfile = 0" );
_LIT( KDSSQLFindContentType, "SELECT * FROM Adapters WHERE Id = %d");
_LIT( KDSSQLGetAllContentTypes, "SELECT * FROM Adapters WHERE ProfileId = %d");
_LIT( KDSSQLGetAllContentTypesByImplementationId, "SELECT * FROM Adapters WHERE ProfileId = %d and ImplementationUID = %d" );

//enumerations

enum TNSmlDSProfileData
	{
	EDSProfileId,
	EDSProfileDisplayName,
	EDSProfileServerURL,
	EDSProfileIAPId,
	EDSProfileTransportId,
	EDSProfileSyncServerUsername,
	EDSProfileSyncServerPassword,
	EDSProfileServerAlertedAction,
	EDSProfileDeleteAllowed,
	EDSProfileHidden,
	EDSProfileHttpAuthUsed,
	EDSProfileHttpAuthUsername,
	EDSProfileHttpAuthPassword,
	EDSProfileAutoChangeIAP,
    EDSProfileCreatorId,   
	EDSProfileServerId,
	EDSProfileProtocolVersion,
	EDSProfileDefaultProfile
	};

enum TNSmlDSFieldVisibility
	{
	EVisibilityNormal,
	EVisibilityReadOnly,
	EVisibilityHidden
	};

enum TNSmlDSTable
	{
	EProfiles,
	EAdapters
	};

enum TNSmlDSContentTypeData
	{
	EDSAdapterTableId,
	EDSAdapterProfileId,
	EDSAdapterImplementationId,
	EDSAdapterEnabled,
	EDSAdapterCreatorId,
	EDSAdapterFilter,
	EDSAdapterDisplayName,
	EDSAdapterServerDataSource,
	EDSAdapterClientDataSource,
	EDSAdapterSyncType,
	EDSAdapterFilterMatchType
	};

enum TNSmlDSContentTypeGetMode
	{
	EDSActiveContentTypes,
	EDSAllContentTypes
	};

enum TNSmlDSProfileAllowed
	{
	EDSNotAllowed,
	EDSAllowed
	};
enum TNSmlDSEnabled
	{
	EDSDisabled,
	EDSEnabled
	};
	
enum TNSmlXMLContentData
	{
	EXMLDSAdapaterId ,
	EXMLDSProfileName,
	EXMLDSServerId,
	EXMLDSServerDataSource
	
	};


// FORWARD DECLARATIONS
class CNSmlDSSettings;
class CNSmlDSProfile;
class CNSmlDSContentType;
class CNSmlDSCrypt;


// CLASS DECLARATION

/**
*  CNSmlDSProfileListItem class is used when list of profiles is
*  needed (when starting application).
*
*  @lib nsmldssettings.lib
*/
class CNSmlDSProfileListItem : public CBase
{	
	public:
		/**
		* two-phase constructor - Creates new CNSmlDSProfileListItem*
		* @param TInt aId - id of profile
		* @return CNSmlDSProfileListItem* - item that was created
		*/
		IMPORT_C static CNSmlDSProfileListItem* NewL( const TInt aId );
		
		/**
		* two-phase constructor - Creates new CNSmlDSProfileListItem*, which is left
		* in cleanup stack.
		* @param TInt aId - id of profile
		* @return CNSmlDSProfileListItem* - item that was created
		*/
		IMPORT_C static CNSmlDSProfileListItem* NewLC( const TInt aId );
				
		/**
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSProfileListItem();

		/**
		* Returns value of given integer field - only profile id
		* @param TNSmlDSProfileData aProfileItem - field enumeration
		* @return TInt - id of profile
		*/
		IMPORT_C TInt IntValue( TNSmlDSProfileData aProfileItem ) const;
		
		/**
		* Copies values from sourceitem
		* @param CNSmlDSProfileListItem* aSourceItem - Item from which values are copied from
		* @return -
		*/
		IMPORT_C void AssignL( const CNSmlDSProfileListItem* aSourceItem );

	private:
		void	ConstructL();
		void	ShowHiddenProfiles();

	private:
		TInt iId;
		TBool iShowHidden;

};

typedef CArrayPtrFlat<CNSmlDSProfileListItem> CNSmlDSProfileList;
typedef CArrayPtrFlat<CNSmlDSProfile> CNSmlProfileArray;

/**
*  CNSmlDSSettings provides an interface to DS Settings database
*
*  @lib nsmldssettings.lib
*/
class CNSmlDSSettings : public CBase
{
	public:
		/**
		* two-phase constructor - Creates new CNSmlDSSettings*
		* @param -
		* @return CNSmlDSSettings* - new CNSmlDSSettings
		*/
		IMPORT_C static CNSmlDSSettings* NewL();	
		
		/**
		* two-phase constructor - Creates new CNSmlDSSettings*, which
		* is left in cleanup stack
		* @param -
		* @return CNSmlDSSettings* - new CNSmlDSSettings
		*/
		IMPORT_C static CNSmlDSSettings* NewLC();
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDSSettings();

		/**
		* Creates new profile
		* @param -
		* @return CNSmlDSProfile* - new profile
		*/
		IMPORT_C CNSmlDSProfile* CreateProfileL();
		
		/**
		* Creates new profile with values from given profile id
		* @param TInt aId - profile that values are copied from
		* @return CNSmlDSProfile* - new profile
		*/
		IMPORT_C CNSmlDSProfile* CreateProfileL( const TInt aId );

		/**
		* List all profile id's (except hidden profiles) into array
		* @param CNSmlDSProfileList* aProfileList - array to fill with profile id's
		* @return -
		*/
		IMPORT_C void GetProfileListL( CNSmlDSProfileList* aProfileList);
		
		/**
		* List all profile id's (including hidden profiles) into array
		* @param CNSmlDSProfileList* aProfileList - array to fill with profile id's
		* @return -
		*/
		IMPORT_C void GetAllProfileListL( CNSmlDSProfileList* aProfileList);
		
		/**
		* Gets profile with given id
		* @param TInt aId - Profile id
		* @param const TNSmlDSContentTypeGetMode aMode - All / active contents. 
		* @return CNSmlDSProfile* - profile
		*/
		IMPORT_C CNSmlDSProfile* ProfileL(TInt aId, const TNSmlDSContentTypeGetMode aMode = EDSAllContentTypes );

		/**
		* Returns Profile ID with given AdapterId
		* @param TInt aId - adapter id
		* @return TInt - id of profile
		*/
		IMPORT_C TInt ProfileIdL( TInt aId);
		
		/**
		* Deletes profile with given id
		* @param TInt aId - profile id
		* @return TInt - KErrNone, if succesfull
		*/
		IMPORT_C TInt DeleteProfileL( TInt aId );

		/**
		* returns database
		* @param -
		* @return RDbNamedDatabase* - the database
		*/
		IMPORT_C RDbNamedDatabase* Database();
		
		/**
		* updates the default profiles
		* @param -
		* @return -
		*/
        IMPORT_C void UpdateHiddenProfilesL();
        
        /**
		* Parse xml 
		* @param - whether data has to be restored from existing database
		* @return -
		*/
        void CreateXMLProfilesL(TBool aRestore);
        
        /**
		* check mandatory settings of the profile 
		* @param - profile, index
		* @return -
		*/
        TBool CheckXMLProfileSettings(CNSmlProfileArray* aProfileArray, TInt aIndex);
        
        /**
		* read cenrep key value
		* @param - aKey, aKey
		* @return -
		*/
        void ReadRepositoryL(TInt aKey, TInt& aValue);
	
        /**
        * read cenrep key value
        * @param - aKey, aKey
        * @return -
        */
        void WriteRepositoryL(TUid aUid, TInt aKey, TInt aValue);
        
	private:
		void ConstructL();
		void CreateDatabaseL( const TDesC& aFullName );
		void CreateHiddenProfilesL();
		void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );
		void CommitAndCompact();
		void AddToProfileListL( CNSmlDSProfileList* aProfileList ); 
		CNSmlDSProfile* ProfileOnlyL( TInt aId );
		TPtrC ViewColDes( const TDesC& aFieldName );
		TInt ViewColInt( const TDesC& aFieldName );
		TUint ViewColUint( const TDesC& aFieldName );
		TBool RemoveDeletedL();
			
	private:		
		RDbNamedDatabase iDatabase;
		RFs iFsSession;
		RDbs iRdbSession;
		RDbTable iTableProfiles;
		CDbColSet* iColSet;
		RDbView iView;
		TInt iAdapterId;
		CNSmlProfileArray* iResourceProfileArray;
};

/**
*  CNSmlDSProfile provides methods to update and save profile data
*
*  @lib nsmldssettings.lib
*/

class CNSmlDSProfile : public CBase
{
	public:

		/**
		* two-phase constructor - Creates new CNSmlDSProfile*
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDSProfile* - new profile
		*/
		static CNSmlDSProfile* NewL( RDbNamedDatabase* aDatabase );	
		
		/**
		* two-phase constructor - Creates new CNSmlDSProfile*, which is
		* left in cleanup stack
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDSProfile* - new profile
		*/
		static CNSmlDSProfile* NewLC( RDbNamedDatabase* aDatabase );
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDSProfile();

		/**
		* Adds new content type for profile
		* @param TInt aImplementationUID - Uid of adapter to be used
		* @param TDesC& aClientDataSource - Data source for client
		* @param TDesC& aServerDataSource - Data source for server
		* @return CNSmlDSContentType* - new content type
		*/
		IMPORT_C CNSmlDSContentType* AddContentTypeL ( TInt aImplementationUID, TDesC& aClientDataSource, TDesC& aServerDataSource );
		
		/**
		* Gets content type
		* @param TInt aImplementationUID - Uid of adapter to be used
		* @return CNSmlDSContentType* - the content type, or NULL if nothing was found
		*/
		IMPORT_C CNSmlDSContentType* ContentType (TInt aImplementationUID ) const;
		
		/**
		* Gets content type
		* @param TInt aId - Id of content type
		* @return CNSmlDSContentType* - the content type, or NULL if nothing was found
		*/
		IMPORT_C CNSmlDSContentType* ContentTypeId ( TInt aId ) const;
		
		/**
		* Deletes content type
		* @param TInt aTaskid - task id of content type
		* @return TBool - ETrue if delete succeeded
		*/
		IMPORT_C TBool DeleteContentTypeL( TInt aTaskId );
		
		/**
		* Saves values of profile and it's content type's into database
		* @param -
		* @return TInt - KErrNone, if succeeded
		*/		
		IMPORT_C TInt SaveL();
		
		/**
		* Returns value of given descriptor field
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field
		* @return const TDesC& - the descriptor 
		*/
		IMPORT_C const TDesC& StrValue( TNSmlDSProfileData aProfileItem ) const;
		
		/**
		* Returns value of given integer field
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field
		* @return const TInt - the integer
		*/
		IMPORT_C TInt IntValue( TNSmlDSProfileData aProfileItem ) const;
		
		/**
		* Sets new value for descriptor
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field that is updated
		* @param const TDesC& aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetStrValue( TNSmlDSProfileData aProfileItem, const TDesC& aNewValue );
		
		/**
		* Sets new value for integer
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field that is updated
		* @param const TInt aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetIntValue( TNSmlDSProfileData aProfileItem, const TInt aNewValue );
		
		/**
		* Returns true if profile has not been saved yet
		* @param -
		* @return TBool - ETrue if profile has not been saved yet
		*/
		IMPORT_C TBool IsNew() const;

		/**
		* Returns true if profile has log
		* @param -
		* @return TBool - ETrue if profile has log
		*/
		IMPORT_C TBool HasLogL(); 
		
		/**
		* Returns true if value of given field is NULL
		* @param const TDesC& aFieldName - Name of the field
		* @return TBool - ETrue if value of given field is NULL
		*/
		IMPORT_C TBool IsNullL( const TDesC& aFieldName );
		
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
		* Sets profile as hidden
		* @param TBool aHidden - new value for profile hidden field
		* @return TBool - ETrue if success
		*/
		IMPORT_C TBool SetHiddenL( TBool aHidden );

		/**
		* A getter for each fields visibility
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field
		* @return TNSmlDSFieldVisibility - Visibility value
		*/
		IMPORT_C TNSmlDSFieldVisibility Visibility( TNSmlDSProfileData aProfileItem ) const;
		
		/**
		* A setter for each fields visibility
		* @param TNSmlDSProfileData aProfileItem - Enumeration of the field
		* @param const TNSmlDSFieldVisibility aVisibility - Visibility value to set
		* @return -
		*/
		IMPORT_C void SetVisibility( TNSmlDSProfileData aProfileItem, const TNSmlDSFieldVisibility aVisibility );
		
		void InitVisibilityArray( const TDesC& aVisibilityStr );
		TNSmlDSFieldVisibility CharToVisibility( const TText aCh ) const;
		TPtrC VisibilityToChar( const TNSmlDSFieldVisibility aVisibility ) const;

		void GetContentTypesL( const TNSmlDSContentTypeGetMode aMode );
		CNSmlDSContentType* GetContentTypeL( TInt aImplementationUID );

		CArrayPtrFlat<CNSmlDSContentType>* iContentTypes;
		
	private:
		
		HBufC* GetVisibilityStrL() const;

		void ConstructL( RDbNamedDatabase* aDatabase );		

		void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );
		void UpdateToDeleteAllowedL();
		void CommitAndCompact();
		void ResetContentTypesL();
		void TableProfilesSetColIntL( const TDesC& aFieldName, TInt aValue);
		void TableProfilesSetColDesL( const TDesC& aFieldName, TDesC& aValue);
		void ViewSetColIntL( const TDesC& aFieldName, TInt aValue);
		void ViewSetColDesL( const TDesC& aFieldName, TDesC& aValue);
		void InsertTableAdaptersL( TInt aInd );

	private:

		TFixedArray<TNSmlDSFieldVisibility, KNSmlDSVisibilityArraySize> iVisibilityArray;

		TInt iId;
		HBufC* iDisplayName;

		TInt iTransportId;
		TInt iIAPId;		
		
		HBufC* iSyncServerUsername;		
		HBufC* iSyncServerPassword;

		HBufC* iServerURL;
		HBufC* iServerId;
		
		TInt iProtocolVersion;
		TInt iServerAlertedAction;
		TBool iDeleteAllowed;
		TBool iProfileHidden;

		TBool iHttpAuthUsed;
		HBufC* iHttpAuthUsername;		
		HBufC* iHttpAuthPassword;
		TBool iAutoChangeIAP;
		TInt iCreatorId;
		
		TBool iDefaultProfile;

		RDbNamedDatabase* iDatabase;

		TInt iAdapterId;

		RDbTable iTableProfiles;
		RDbTable iTableAdapters;

		RDbColWriteStream iWs;
		RDbColReadStream  iRs;

		RDbView iRSView;
		RDbView iWSView;

		CDbColSet* iColSetProfiles;
		CDbColSet* iColSetAdapters; 

		RDbView iView;
	
};

/**
*  CNSmlDSContentType provides methods to update and save content type data
*
*  @lib nsmldssettings.lib
*/

class CNSmlDSContentType: public CBase
{
	public:

		/**
		* two-phase constructor - Creates new CNSmlDSContentType*
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDSContentType* - new profile
		*/
		static CNSmlDSContentType* NewL( RDbNamedDatabase* aDatabase );
		
		/**
		* two-phase constructor - Creates new CNSmlDSContentType*, which
		* is left in cleanup stack.
		* @param RDbNamedDatabase* aDatabase - database to be used
		* @return CNSmlDSContentType* - new profile
		*/
		static CNSmlDSContentType* NewLC( RDbNamedDatabase* aDatabase );
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSContentType();		

		/**
		* Returns value of given descriptor field
		* @param TNSmlDSContentTypeData aProfileItem - Enumeration of the field
		* @return const TDesC& - the descriptor 
		*/
		IMPORT_C const TDesC& StrValue( TNSmlDSContentTypeData aProfileItem ) const;
		
		/**
		* Returns value of given integer field
		* @param TNSmlDSContentTypeData aProfileItem - Enumeration of the field
		* @return const TInt - the integer
		*/
		IMPORT_C TInt IntValue( TNSmlDSContentTypeData aType ) const;
		
		/**
		* Sets new value for integer
		* @param TNSmlDSContentTypeData aProfileItem - Enumeration of the field that is updated
		* @param const TInt aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetIntValue( TNSmlDSContentTypeData aType, const TInt aNewValue );		
		
		/**
		* Sets new value for descriptor
		* @param TNSmlDSContentTypeData aProfileItem - Enumeration of the field that is updated
		* @param const TDesC& aNewValue - New value for field
		* @return -
		*/
		IMPORT_C void SetStrValue( TNSmlDSContentTypeData aProfileItem, const TDesC& aNewValue );
		
		/**
		* Returns true, if content type has a filter
		* @param -
		* @return TBool - ETrue if content type has a filter
		*/
		IMPORT_C TBool HasFilterL(); 
		
		/**
		* Returns stream to read filter data from
		* @param -
		* @return RReadStream& - stream to read filter
		*/
		IMPORT_C RReadStream& FilterReadStreamL();
		
		/**
		* Returns stream to write filter into
		* @param -
		* @return RWriteStream& - stream to write filter
		*/
		IMPORT_C RWriteStream& FilterWriteStreamL();
		
		/**
		* Commits writing to stream
		* @param -
		* @return -
		*/
		IMPORT_C void WriteStreamCommitL();

	private:
		void ConstructL();		
	private:		
		TInt iId;
		TInt iProfileId;
		TInt iImplementationId;
		TNSmlDSEnabled iEnabled;
		TInt iCreatorId;
		HBufC* iDisplayName;
		HBufC* iServerDataSource;
		HBufC* iClientDataSource;
		TInt iSyncType;
		
		
		TSyncMLFilterMatchType iFilterMatchType;
		RDbNamedDatabase* iDatabase;
		CDbColSet* iColSetAdapterParams; 

		RDbView iView;

		RDbView iRSView;
		RDbView iWSView;

		TInt iParamId;
		TInt iParamTableId;
		TUid iUid;

		RDbColWriteStream iWs;
		RDbColReadStream  iRs;
};

/**
*  CNSmlDSCrypt handles encryption / decryption of data
*
*  @lib nsmldssettings.lib
*/
class CNSmlDSCrypt : public CBase
{
	public:
		/**
		* Constructor
		* @param -
		* @return -
		*/
		IMPORT_C CNSmlDSCrypt();
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		IMPORT_C ~CNSmlDSCrypt();
		
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


class CNSmlDSContentSettingType: public CBase
{
	public:
		/**
		* two-phase constructor - Creates new CNSmlDSContentSettingType*
		* @param 
		* @return CNSmlDSContentSettingType* - 
		*/
		static CNSmlDSContentSettingType* NewL( );
		
		/**
		* two-phase constructor - Creates new CNSmlDSContentSettingType*, which
		* is left in cleanup stack.
		* @param 
		* @return CNSmlDSContentSettingType* - 
		*/
		static CNSmlDSContentSettingType* NewLC( );
		
		/**
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSContentSettingType();
		
		/**
		* Returns value of given integer field
		* @param TNSmlXMLContentData aType -
		* @return const TInt - the integer
		*/
		const TDesC& StrValue(TNSmlXMLContentData aType) const ;
		
		/**
		* Sets new value for integer
		* @param TNSmlXMLContentData aType -
		* @param TInt aNewValue - New value for field
		* @return -
		*/
		void SetStrValue(TNSmlXMLContentData aType ,const TDesC& aNewValue );		
		
		
			
	private:
	
		void ConstructL();		
		
	private:
		
		HBufC* iImplementationId;
		
		HBufC* iProfileName;
		HBufC* iServerId;
		HBufC* iServerDataSource;
			

	
	
};
#endif  //__NSMLDSSETTINGS_H
