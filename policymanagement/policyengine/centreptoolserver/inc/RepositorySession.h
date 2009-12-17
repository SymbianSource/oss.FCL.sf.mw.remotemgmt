/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/



#ifndef REPOSITORY_SESSION_HEADER_
#define REPOSITORY_SESSION_HEADER_

// INCLUDES

#include <e32base.h>
#include <f32file.h> 
#include <s32file.h> 
#include <s32strm.h> 

#include "constants.h"
#include "IniFileHelper.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CRepositorySession;

// CLASS DECLARATION

class CRepositoryContent : public CBase
{
	public:
		//constructors
		~CRepositoryContent();
		
		static CRepositoryContent * NewL( const TUid& aUid);
		static CRepositoryContent * NewL( const TUid& aUid, HBufC* aContent);
	
		void ReleaseResources();
	
		//file streaming
		void ReadStreamL( RReadStream& aStream, TBool aOnlyHeader );
		void WriteStreamL( RWriteStream& aStream );


		//File reading
		void ReadHeaderL();
		void ReadOptionalDataL();
		void ReadPlatSecL();
		void ReadMainL();

		//File writing
		void WriteFileL( RFile& aFile);

		//Search setting
		CIndividualSetting* FindIndividualSetting( const TUint32 aSettingId);
		void CreateRangeSettingsL( RPointerArray<CRangeSetting>& aSettings, TUint32 aRangeStart, TUint32 aRangeEnd);
		void CreateMaskSettingsL( RPointerArray<CRangeSetting>& aSettings, TUint32 aCompareValue, TUint32 aMask);
		void CreateMaskBackupL( RPointerArray<CRangeMeta>& aSettings, TUint32 aCompareValue, TUint32 aMask);
		void CreateRangeBackupL( RPointerArray<CRangeMeta>& aSettings, TUint32 aRangeStart, TUint32 aRangeEnd);
		
		TBool CheckRangeValidity();
		
		TBool CheckAccess( const RMessage2& aMessage, TUint32 aSettingId, TAccessType aAccessType);
	private:
		//private constructors
		CRepositoryContent( const TUid& aUid);
		CRepositoryContent( const TUid& aUid, HBufC* aContent);
		void ConstructL();
		
		//repository file management
		void DeleteRepositoryFile();
		void CREFileCreationOk( TBool aState);				
		
		//setting handling methods
		CIndividualSetting* FindSettingOrCreateL( const TUint32& aSettingId);	
		void AddIndividualSettingL( CIndividualSetting* aSetting);
		TInt SingleSecuritySettingsCount();
	private:
		CDefaultSetting * iDefaultSetting;
		TUint32 iDefaultMeta;
		RPointerArray<CRangeSetting> iRangeSettings;
		RPointerArray<CRangeMeta> iRangeMetas;
		RPointerArray<CIndividualSetting> iIndividualSettings;
		
		TPtrC iContentPtr;		
		TPtrC iHeaderPtr;
		TPtrC iOwnerPtr;
		TPtrC iDefaultMetaPtr;
		
		HBufC* iContent;
		
		//repository info
		TUid iUid;
		TUid iOwner;
		TTime iTimeStamp ;

		TInt iSingleSecuritySettingCount;
		
		friend class CRepositorySession;	
};

class CRepositorySession : public CObject
{
	public:
	
		CRepositorySession( TUid aRepositoryId);
		~CRepositorySession();
	
		static CRepositorySession * NewL( TUid aRepositoryId);
		
		//repository management
		void InitRepositorySessionL();
		void CommitRepositoryL();
		
		void SetSecurityIdForSettingL( const RMessage2& aMessage);
		void SetSecurityIdForRangeL( const RMessage2& aMessage);
		void SetSecurityIdForMaskL( const RMessage2& aMessage);

		void RestoreSettingL( const RMessage2& aMessage);
		void RestoreRangeL( const RMessage2& aMessage);

		void AddSidForDefaultsL( const RMessage2& aMessage);
		void RestoreDefaultsL( const RMessage2& aMessage);
		void RestoreMaskL( const RMessage2& aMessage);
		
		void RemoveBackupForMaskL( const RMessage2& aMessage);
		void RestoreMaskBackupL( const RMessage2& aMessage);
		void RemoveBackupForRangeL( const RMessage2& aMessage);
		void RestoreRangeBackupL( const RMessage2& aMessage);
		void RemoveDefaultBackup();
		void RestoreDefaultBackupL();
		
		void CheckAccessL( const RMessage2& aMessage);
		
		//Commit functions
		static void CheckCommitStateL();
	private:
		//File reading
		void ReadFileL();
		
		//Utility
		void ReleaseResources();
		void CheckRepositoryState( TInt& aFileOpenResult);
		
	
		TInt ReadRAML( TBool aOnlyHeader);
		void CreateAndReadROML();
		CRepositoryContent* ReadROMFileL( TBool aReadSettings);	
		
		//Backup handling
		void MakeBackupL( RFile& aFile);
		static void RestoreBackupL();
		static void RemoveBackupL();
		static TInt CreatePath();
		
	private:
		//repository specific data
		TUid iRepositoryId;
		RFs iFs;
		RFile iFile;
		TBool iStreamed;	//if file repository is opened as a streamed this flag is set TRUE (*.CRE files are streamed)
		TBool iRepositoryInUse;
	
		
		//file is restored if commit is not called before close!!
		TBuf<IniConstants::KUidLengthRep> iRepName;
	
		CRepositoryContent* iRepContent;
};


#endif 