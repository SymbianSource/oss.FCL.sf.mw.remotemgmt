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



#ifndef _POLICY_STORAGE_HEADER__
#define _POLICY_STORAGE_HEADER__

#define __EXCEPTION_HANDLING


// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <SettingEnforcementInfo.h>
#include "ElementBase.h"

// CONSTANTS

LOCAL_C TChar KParentListStartMark = '~';

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CElementBase;
class CRepository;
class CElementBase;
class CPolicyParser;
class CSettingEnforcementManager;

// CLASS DECLARATION

class CPolicyStorage : public CBase
{
	friend class CPolicyEngineServer;

	public:
		static CPolicyStorage * NewL();
		void ConstructL();
		CPolicyStorage();
		~CPolicyStorage();	

		//static reference		
		static CPolicyStorage * PolicyStorage();
		
		//New element handling
		TInt SaveElementL( TUint32 aElementId, HBufC8 * aElementDescription);
		void DeleteElementL( TUint32 aElementId);
		void AddNewElementL( CElementBase * aElement);
		void CommitChangesL( TBool aAcceptCommit);
		void StartCommitSessionL();
		
		//element handling
		void CheckElementL( CElementBase * aElement);
		CElementBase * GetElementL( const TDesC8& iPolicyId );
		CElementBase * GetElementL( const TUint32 aElementId);
		CElementBase * GetEditableElementL( const TDesC8& aPolicyId );
		CElementBase * GetEditableElementL( const TUint32 aElementId );
		void ReleaseElements();
//		TBool IsExist( const TDesC8& aElementId );

		//element editing
		void GetEditedElementsL( CSettingEnforcementManager* aSEManager);
		void SaveEditableMemoryL();
		void ResetEditableMemory();
		
		//id functions
		TUint32 CreateIdL( TNativeElementTypes &aType);
		TUint32 MapRealIdL( const TDesC8& aElementRealId, const TBool aOnlyCommitted);
		TBool IsRealIdValidL( const TDesC8& aElementRealId);
		TBool IsIdValidL( const TUint32& aId);
		void ResetRealIdValidTest();
		
		//setting enforcement 
		void ActivateEnforcementFlagL( KSettingEnforcements aSetting);
		void DeactivateEnforcementFlagL( KSettingEnforcements aSetting);

		//server id list
		TInt AddNewServerId( const TDesC8& aServerId);
		TInt RemoveServerId( const TDesC8& aServerId);
		TInt ResetServerIdList();
		TBool IsServerIdValid( const TDesC8& aServerId);
	private:
	
		CElementBase * FindElement( RArray<CElementBase*> &aElementList, const TUint32 aElementId);
		void AddElementL( RArray<CElementBase*> &aElementList, CElementBase * aElement);
	//	void LoadElementL( const TUint32 aElementId);
		void LoadElementL( CElementBase * aElement, TBool aAddToEditableCache);
		
		TInt SetElementL( TUint32 aId, const TDesC8& aDescription);
		TInt GetElementL( TUint32 aId, HBufC8*& aDescription);
	
		//Backup functions
		TInt CreatePaths();
		void CheckCommitStateL();
		void MakeBackupL( const TDesC& aFileName);
		void MakeBackupL( const TUint32& aSettingId);
		void RestoreBackupL();
		void RemoveBackupL();
		
		//server id lists
		void SaveServerIdListL();
		void LoadServerIdListL();
		void PrintServerIdList();
		
		//for mapping handling
		TUint32 GetEditedMappingsL( const TDesC8& aElementRealId);	
		TUint32 GetCommittedMappingsL( const TDesC8& aElementRealId);	
		TUint32 GetMappingsFromDriveL( const TDesC8& aElementRealId, TDriveNumber aDrive, TBool aUpdateList);	
		TBool ReadRealIdL( const TDesC& aFileName, const TDesC8& aElementRealId, TDriveNumber aDrive);
	private:
		RFs iRFs;
		CRepository * iCentRep;
		CRepository * iCentRepBackup;
		CPolicyParser * iParser;
		TBool iCommitted;
		static CPolicyStorage* iPolicyStorage;

		RElementArray iElementCache;
		RElementArray iDepricatedElements;
		RElementArray iEditedElements;
		RPointerArray<HBufC8> iServerIdList;
		RArray<TUint32> iSearchedUids;
		RPointerArray<HBufC8> iInvalidServerIds;
};

class TElementReserver
{
	public:
		TElementReserver();
		TElementReserver( CElementBase* aElement);
		TElementReserver( RElementContainer* aElements);
		~TElementReserver();
		void ReserveL( CElementBase* aElement);

		void Release();
	private:
		typedef RArray<CElementBase*> RElementList;
		void DoRelease();
		
#ifndef __EXCEPTION_HANDLING

		class CWrapper : public CBase
		{
			public:
				CWrapper( TElementReserver *aReserver);
				~CWrapper();
			public:
				TElementReserver *iReserver;
		};
#endif // __EXCEPTION_HANDLING
				
	private:
#ifndef __EXCEPTION_HANDLING
		CWrapper * iWrapper;
#endif // __EXCEPTION_HANDLING
		CElementBase * iElement;
		RElementContainer * iElements;
		RElementList * iElementList;
		
		friend class CWrapper;
};

#endif