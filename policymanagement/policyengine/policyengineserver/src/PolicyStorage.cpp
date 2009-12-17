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
* Description: Implementation of policymanagement components
*
*/


// INCLUDE FILES

#include "PolicyStorage.h"
#include "PolicyEngineServer.h"
#include "ElementBase.h"
#include "XACMLconstants.h"
#include "PolicyParser.h"
#include "SettingEnforcementManager.h"
#include "debug.h"
#include "ErrorCodes.h"
#include "PolicyEnginePrivateCRKeys.h"

#include "policymnginternalpskeys.h"
#include <centralrepository.h>
#include "CentRepToolClient.h"
#include <e32property.h>
#include <s32file.h>


_LIT( KCDrive, "C:\\private\\10207815\\");
_LIT( KZDrive, "Z:\\private\\10207815\\");
_LIT( KCDriveLetter, "C:");
_LIT( KZDriveLetter, "Z:");
_LIT( KFileExtensio, ".txt");
_LIT( KBackupPath, "C:\\private\\10207815\\backup\\");
_LIT8( KEmptyBackup, "|Empty|");
_LIT( KServerIdFile, "C:\\private\\10207815\\serverids.dat");
_LIT( KServerIdFileTmp, "C:\\private\\10207815\\backup\\serverids.tmp");

const TInt KUidLength = 10;
const TInt KFileNameLength = 14;
const TChar KElementStartMark		 = '<';
const TInt KFullFileNameLength = 80;
const TInt KDriveLetterLength = 2;
// not in use anymore: KSettingSpaceLength = 100;
const TInt KCacheLimit = 100;




// ==================== LOCAL FUNCTIONS ====================

TInt CompareIDs( TUint32 const& aId1, TUint32 const& aId2)
{
	if ( aId1 == aId2) return 0;
	return ( aId1 < aId2 ? -1 : 1 );
}

TInt CompareStrings( HBufC8 const& aString1, HBufC8 const& aString2)
{
	return aString1.Compare( aString2);
}


// ================= MEMBER FUNCTIONS =======================

CPolicyStorage* CPolicyStorage::iPolicyStorage = 0;

// -----------------------------------------------------------------------------
// CPolicyStorage::CPolicyStorage()
// -----------------------------------------------------------------------------
//

CPolicyStorage::CPolicyStorage()
{
}

// -----------------------------------------------------------------------------
// CPolicyStorage::~CPolicyStorage()
// -----------------------------------------------------------------------------
//

CPolicyStorage::~CPolicyStorage()
{
	RDEBUG("CPolicyStorage::~CPolicyStorage()");
	delete iCentRep;
	delete iCentRepBackup;
	delete iParser;

	//release memory used in element cache	
	for ( TInt i(0); i < iElementCache.Count(); i++)
	{
		CElementBase * element = iElementCache[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));

				
		delete element;
	}

	//release memory used in depricated element cache
	for ( TInt i(0); i < iDepricatedElements.Count(); i++)
	{
		CElementBase * element = iDepricatedElements[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));

				
		delete element;
	}

	//release memory used in depricated element cache
	for ( TInt i(0); i < iEditedElements.Count(); i++)
	{
		CElementBase * element = iEditedElements[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));

				
		delete element;
	}
	
	//release resources
	iElementCache.Close();
	iDepricatedElements.Close();
	iEditedElements.Close();
	iSearchedUids.Close();
	
	iServerIdList.ResetAndDestroy();
	iServerIdList.Close();
	
	iInvalidServerIds.ResetAndDestroy();
	iInvalidServerIds.Close();

	
	iRFs.Close();
}


// -----------------------------------------------------------------------------
// CPolicyStorage::PolicyStorage()
// -----------------------------------------------------------------------------
//
CPolicyStorage * CPolicyStorage::PolicyStorage()
{
	return iPolicyStorage;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::ConstructL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::ConstructL()
{
	RDEBUG("PolicyEngineServer: CPolicyStorage::ConstructL - start");

	iCommitted = ETrue;
	//open sessions to repotories and file server
	iCentRep = CRepository::NewL( KPolicyEngineRepositoryID);
	RDEBUG("PolicyEngineServer: Connected to repository");
	iCentRepBackup = CRepository::NewL( KPolicyEngineBackupRepositoryID);
	RDEBUG("PolicyEngineServer: Connected to backup repository");
	User::LeaveIfError( iRFs.Connect() );
	
	//Create and set up parser
	iParser = CPolicyParser::NewL( this);
	RDEBUG("PolicyEngineServer: Policy parser created");
	iParser->SetExternalIdChecked( KNoExternalIdCheck);
	
	//check commit state (restore and/or delete backup if needed)
	CheckCommitStateL();
	
	//load server id list...
	LoadServerIdListL();

	RDEBUG("PolicyEngineServer: CPolicyStorage::ConstructL - end");
}

// -----------------------------------------------------------------------------
// CPolicyStorage::NewL()
// -----------------------------------------------------------------------------
//

CPolicyStorage * CPolicyStorage::NewL()
{
	CPolicyStorage * self = new(ELeave) CPolicyStorage();
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::ReleaseElements()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::ReleaseElements()
{
	RDEBUG("PolicyEngineServer: Try to release unused elements!");


	//release memory used in element cache	
	for ( TInt i(0); i < iElementCache.Count(); i++)
	{
		CElementBase * element = iElementCache[i];
		if ( element->iReferenceCount == 0)
		{
			delete element;
			iElementCache.Remove(i--);
			RDEBUG_2("PolicyEngineServer: Element removed from cache (Cache count %d)", iElementCache.Count());
		}
	}

	//release memory used in depricated element cache
	for ( TInt i(0); i < iDepricatedElements.Count(); i++)
	{
		CElementBase * element = iDepricatedElements[i];
		if ( element->iReferenceCount == 0)
		{
			delete element;
			iDepricatedElements.Remove(i--);
			RDEBUG_2("PolicyEngineServer: Element removed from cache (Depricated element count %d)", iDepricatedElements.Count());
		}
	}
}


// -----------------------------------------------------------------------------
// CPolicyStorage::SetElementL()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::SetElementL( TUint32 aId, const TDesC8& aDescription)
{
	//resolve element file name
	TBuf<KUidLength> elementFile;
	elementFile.NumFixedWidth( aId, EDecimal, KUidLength);

	//buffer for file name (C-drive)
	TBuf<KFullFileNameLength> fileName;
	fileName.Append( KCDrive);
	fileName.Append( elementFile);
	fileName.Append( KFileExtensio);


	RDEBUG_2("PolicyEngineServer: Write element to disk (%S)", &fileName);
	MakeBackupL( fileName);

	//try open existing file
	RFile file;
	TInt err = file.Open( iRFs, fileName, EFileWrite);
	

	if ( err == KErrPathNotFound)
	{
		err = CreatePaths();
		
		//this ensures, that also element file will be created
		if ( err == KErrNone)
		{
			err = KErrNotFound;
		}
	}
	
	//and if file doesn't not exist create it
	if ( err == KErrNotFound  )
	{
		err = file.Create( iRFs, fileName, EFileWrite);
	}
	
	//write to file...
	if ( err == KErrNone)
	{
		err = file.Write( 0, aDescription);
	}
	
	//...and set size
	if ( err == KErrNone )
	{
		err = file.SetSize( aDescription.Length());
		if ( err == KErrNone )
		{
			err = file.Flush();
		}
	}	
	
	file.Close();

	RDEBUG_2("PolicyEngineServer: Write OK (%S)", &fileName);
	
	return err;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::MakeBackUpL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::MakeBackupL( const TDesC& aFileName)
{
	//buffer for file name (C-drive)
	TBuf<KFullFileNameLength> backupName;
	backupName.Append( KBackupPath);
	backupName.Append( aFileName.Right( KFileNameLength));
	
	CFileMan * fileMan = CFileMan::NewL( iRFs);
	
	//create backup
	TInt err = fileMan->Copy( aFileName, backupName, 0);
	delete fileMan;
	
	//if file doesn't exist in workarea, create empty file which indicates that in original state file is not exist
	if ( err == KErrNotFound)
	{
		RFile file;
		CleanupClosePushL( file);
		User::LeaveIfError( file.Create( iRFs, backupName, EFileWrite));

		RDEBUG_2("PolicyEngineServer: Element backup: %S indicates new element in policy storage", &backupName);
	
		CleanupStack::PopAndDestroy( &file);
	}
	else
	{
		User::LeaveIfError( err);
		RDEBUG_3("PolicyEngineServer: Element backup: %S -> %S", &aFileName, &backupName);
	}

}

// -----------------------------------------------------------------------------
// CPolicyStorage::MakeBackUpL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::MakeBackupL( const TUint32& aSettingId)
{
	//TInt settings
	TInt value = 0;
	TInt err = iCentRep->Get( aSettingId, value);
	RDEBUG_2("PolicyEngineServer: Backup for CentRep value setting: %d", aSettingId);
	
	//for "empty backup"
	if ( err == KErrNotFound )
	{
		//Set backupvalue
		TInt err = iCentRepBackup->Create( aSettingId, KEmptyBackup);
		
		//if value is already exist, it is a original value of setting is must be saved
		if ( err == KErrAlreadyExists)
		{
			err = KErrNone;
		}
				
		//Leave if backupfails
		User::LeaveIfError( err);		
	}
	else
	{
		//Set backupvalue
		TInt err = iCentRepBackup->Create( aSettingId, value);
		
		if ( err == KErrAlreadyExists)
		{
			//if value is already exist, it is a original value of setting is must be saved
			err = KErrNone;
		}
				
		//Leave if backupfails
		User::LeaveIfError( err);		
	}
}

void CPolicyStorage::RestoreBackupL()
{
	//Open directory and get file list
	RDir dir;
	CleanupClosePushL( dir);
	
	TInt err = dir.Open( iRFs, KBackupPath, KEntryAttNormal);

	//create private and backuppaths	
	if ( err == KErrPathNotFound)
	{
		CreatePaths();
		err = dir.Open( iRFs, KBackupPath, KEntryAttNormal);
	}
	
	User::LeaveIfError( err);
	err = KErrNone;

	do 
	{
		//read entries (files)
		TEntryArray array;
		err = dir.Read( array);
		if ( KErrEof != err)
		{
			User::LeaveIfError( err);
		}
	
		for ( TInt i = 0; i < array.Count(); i++)
		{
			const TEntry& entry = array[i];
		
			//original file name
			TBuf<KFullFileNameLength> fileName;
			fileName.Append( KCDrive);
			fileName.Append( entry.iName);

			//if file size is zero, remove file from work directory
			if ( entry.iSize == 0)
			{
				RDEBUG_2("PolicyEngineServer: Restoring backup: %S removed", &fileName);
				
				TInt err = iRFs.Delete( fileName);
				if ( err == KErrNotFound)
				{	
					err = KErrNone;	
				}
				
				User::LeaveIfError( err );
			}
			else
			{
				//Create backup file name
				TBuf<KFullFileNameLength> backupName;
				backupName.Append( KBackupPath);
				backupName.Append( entry.iName);
			
				//otherwise copy file to work directory
				CFileMan * fileMan = CFileMan::NewL( iRFs);
				CleanupStack::PushL( fileMan);
			
				//Copy file to work directory
				User::LeaveIfError( fileMan->Copy( backupName, fileName));
				RDEBUG_3("PolicyEngineServer: Restoring backup: %S -> %S", &backupName, &fileName);
	
	
				CleanupStack::Pop( fileMan);  //fileMan		
			}
		}
	} while ( err == KErrNone);
	
	//id list 
	RArray<TUint32> ids;
	CleanupClosePushL( ids);
	
	//find all ids
	iCentRepBackup->FindL( 0, 0x0, ids);

	for ( TInt i(0); i < ids.Count(); i++)
	{
		TUint32 id = ids[i];
		RDEBUG_2("PolicyEngineServer: Restoring centrep setting %d", id);
	
		//get value
		TBuf8<200> buffer;
		TInt err = iCentRepBackup->Get( id, buffer);
		
		if ( err != KErrNone)
		{
			TInt value = 0;
			User::LeaveIfError( iCentRepBackup->Get( id, value));
			User::LeaveIfError( iCentRep->Set( id, value));
		}
		else
		{
			if ( buffer == KEmptyBackup)
			{
				User::LeaveIfError( iCentRep->Delete( id));
			}
			else
			{
				User::LeaveIfError( iCentRep->Set( id, buffer));
			}
		}
	}
	
	CleanupStack::PopAndDestroy(2, &dir);	//RArray<TUint32>, RDir
}

// -----------------------------------------------------------------------------
// CPolicyStorage::RemoveBackupL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::RemoveBackupL()
{
	//Open directory and get file list
	RDir dir;
	CleanupClosePushL( dir);
	
	TInt err = dir.Open( iRFs, KBackupPath, KEntryAttNormal);

	//create private and backuppaths	
	if ( err == KErrPathNotFound)
	{
		CreatePaths();
		err = dir.Open( iRFs, KBackupPath, KEntryAttNormal);
	}
	
	User::LeaveIfError( err);
	err = KErrNone;

	do 
	{
		//read entries (files)
		TEntryArray array;
		err = dir.Read( array);
		if ( KErrEof != err)
		{
			User::LeaveIfError( err);
		}	
	
		for ( TInt i = 0; i < array.Count(); i++)
		{
			const TEntry& entry = array[i];
		
			TBuf<KFullFileNameLength> backupName;
			backupName.Append( KBackupPath);
			backupName.Append( entry.iName);
	
			RDEBUG_2("Policy Engine: Remove backup file %S", &backupName);
			User::LeaveIfError( iRFs.Delete( backupName));
		}
	} while ( err == KErrNone);
	
	//id list 
	RArray<TUint32> ids;
	CleanupClosePushL( ids);
	
	//find all ids
	iCentRepBackup->FindL( 0, 0x0, ids);

	for ( TInt i(0); i < ids.Count(); i++)
	{
		RDEBUG_2("Policy Engine: Remove centrep backup %d", ids[i]);
		User::LeaveIfError( iCentRepBackup->Delete( ids[i]));
	}
	
	CleanupStack::PopAndDestroy(2, &dir);	//RArray<TUint32> 	
}


// -----------------------------------------------------------------------------
// CPolicyStorage::ActivateEnforcementFlagL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::ActivateEnforcementFlagL( KSettingEnforcements aSetting)
{
	TUint32 id = KEnforcementStates + aSetting;

	MakeBackupL( id);
	TInt err = iCentRep->Set( id, ETrue);
	
	if ( KErrNotFound == err)
	{
		err = iCentRep->Create( id, ETrue);
	}

	User::LeaveIfError( err);
}

// -----------------------------------------------------------------------------
// CPolicyStorage::DeactivateEnforcementFlagL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::DeactivateEnforcementFlagL( KSettingEnforcements aSetting)
{
	TUint32 id = KEnforcementStates + aSetting;

	MakeBackupL( id);
	TInt err = iCentRep->Set( id, EFalse);
	
	if ( KErrNotFound == err)
	{
		err = iCentRep->Create( id, EFalse);
	}
	
	User::LeaveIfError( err);
}



// -----------------------------------------------------------------------------
// CPolicyStorage::GetElementL()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::GetElementL( TUint32 aId, HBufC8*& aDescription)
{
	//parse element file name
	TBuf<KUidLength> elementFile;
	elementFile.NumFixedWidth( aId, EDecimal, KUidLength);

	//buffer for file name (C-drive)
	TBuf<KFullFileNameLength> fileName;
	fileName.Append( KCDrive);
	fileName.Append( elementFile);
	fileName.Append( KFileExtensio);
	
	//try open file from C-drive
	RFile file;
	CleanupClosePushL( file);
	TInt err = file.Open( iRFs, fileName, EFileRead);
	
	//and if file does not exist in C-drive try again from z-drive
	if ( err == KErrNotFound || err == KErrPathNotFound)
	{
		fileName.Replace( 0, KDriveLetterLength, KZDriveLetter);
		err = file.Open( iRFs, fileName, EFileRead);
	}
	
	//resolve file size
	TInt size = 0;
	if ( err == KErrNone)
	{
		err = file.Size( size);
	}
	
	//empty file in the RAM-drive indicates that ROM-file is deleted
	if ( err == KErrNone && size)	
	{
		//create buffer for element
		aDescription = HBufC8::NewLC( size);
		TPtr8 ptr = aDescription->Des();
		
		//read file to buffer and remove buffer from cleanupstack
		file.Read( ptr);
		CleanupStack::Pop( aDescription);		
	}
	
	__ASSERT_ALWAYS( aDescription, User::Panic(PolicyStoragePanic, KErrGeneral));
		
	CleanupStack::PopAndDestroy( &file);		
	
	return err;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::DeleteElement()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::DeleteElementL( TUint32 aElementId)
{
	//parse element file name
	TBuf<KUidLength> elementFile;
	elementFile.NumFixedWidth( aElementId, EDecimal, KUidLength);

	//buffer for file name (C-drive)
	TBuf<KFullFileNameLength> fileName;
	fileName.Append( KCDrive);
	fileName.Append( elementFile);
	fileName.Append( KFileExtensio);
	
	
	//make backup	
	MakeBackupL( fileName);


	//if the ROM-drive contains file, we can't delete it, create empty file (c-drive) to indicate that file is deleted
	fileName.Replace( 0, KDriveLetterLength, KZDriveLetter);
	RFile file;
	TInt err = file.Open( iRFs, fileName, EFileRead);
	file.Close();
	
	fileName.Replace( 0, KDriveLetterLength, KCDriveLetter);
		
	if ( err == KErrNone)
	{
		CleanupClosePushL( file);
		User::LeaveIfError( file.Create( iRFs, fileName, EFileWrite	));
		User::LeaveIfError( file.Flush());
		CleanupStack::PopAndDestroy( &file);
	}
	else
	{
		//if file is only in the ROM-drive
		User::LeaveIfError( iRFs.Delete( fileName));
	}
	
}





// -----------------------------------------------------------------------------
// CPolicyStorage::SaveElementL()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::SaveElementL(  TUint32 aElementId, HBufC8 * aElementDescription)
{	
	//write element
	return SetElementL( aElementId, *aElementDescription);
}

// -----------------------------------------------------------------------------
// CPolicyStorage::CheckElementL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::CheckElementL( CElementBase * aElement)
{
	if ( aElement->iElementState == ENotLoaded || aElement->iElementState == ENotLoadedEditableElement)	
	{
		TBool addEditCache = (  aElement->iElementState == ENotLoaded ? EFalse : ETrue);
		LoadElementL( aElement, addEditCache);
		
		if ( aElement->iElementState == ENotLoaded)
		{	
			aElement->iElementState = ECacheElement;
		}
		else
		{
			aElement->iElementState = EEditableCacheElement;
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyStorage::LoadElementL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::LoadElementL( CElementBase * aElement, TBool aAddToEditableCache)
{
	__ASSERT_ALWAYS( aElement, User::Panic(PolicyStoragePanic, KErrGeneral));

	//load element description		
	HBufC8 * description = NULL;
	User::LeaveIfError( GetElementL( aElement->GetId(), description));
	CleanupStack::PushL( description);
	TPtr8 ptr(description->Des());
		
	TNativeElementTypes elementType = aElement->ElementType();
			
	//find parent element mapping for policies, policysets and rules
	if ( elementType == ERule || elementType == EPolicy || elementType == EPolicySet )
	{
		TInt index = ptr.Locate( KParentListStartMark); 
		
		TPtrC8 ptrParent;
		if ( index != KErrNotFound)
		{
			//separate description and parent id list
			ptrParent.Set( ptr.Mid( index));
			ptr = ptr.Left( index);
		}
			
		//and decode and add parant mapping
		while ( index != KErrNotFound)
		{
			ptrParent.Set( ptrParent.Mid(1));
			index = ptrParent.Locate(KParentListStartMark);
		
			TPtrC8 elementId( ptrParent);

			if ( index != KErrNotFound)
			{
				elementId.Set( ptrParent.Left( index));
				ptrParent.Set( ptrParent.Mid( index));
			}
		
			TUint32 policySystemid;
			TLex8 lex( elementId);
			User::LeaveIfError( lex.Val( policySystemid, EDecimal));
			
			aElement->AddParentIdL( policySystemid);
		}
	}

	//parse object
	User::LeaveIfError( iParser->ParseNativeObjects( aElement, ptr, aAddToEditableCache));

		
	CleanupStack::PopAndDestroy( description);	
}



void CPolicyStorage::AddNewElementL( CElementBase * aElement)
{
	aElement->iElementState = ENewElement;
	AddElementL( iEditedElements, aElement);
	aElement->iReferenceCount = 0;	
	
	RDEBUG_2("PolicyEngineServer: New editable element (element count %d)", iEditedElements.Count());
}


// -----------------------------------------------------------------------------
// CPolicyStorage::GetEditableElementL()
// -----------------------------------------------------------------------------
//
CElementBase * CPolicyStorage::GetEditableElementL( const TDesC8& aPolicyId )
{
	TUint32 settingId = MapRealIdL( aPolicyId, EFalse);
	
	CElementBase * element = NULL;
	
	if ( settingId )
	{
		element = GetEditableElementL( settingId);	
	}
	
	return element;
}



// -----------------------------------------------------------------------------
// CPolicyStorage::GetEditableElementL()
// -----------------------------------------------------------------------------
//
CElementBase * CPolicyStorage::GetEditableElementL( const TUint32 aElementId )
{
	//check from edit cache
	CElementBase * element = FindElement( iEditedElements, aElementId);
	
	//if doesn't not exits
	if ( element == NULL)
	{
		//create new element
		element = ElementHelper::CreateNewElementL( aElementId);

		//Aff element to edited element cache
		element->iElementState = ENotLoadedEditableElement;
		AddElementL( iEditedElements, element);
		element->iReferenceCount = 0;	

	}
	
	//if element is deleted return NULL
	if ( element->iElementState == EDeletedEditableElement)
	{
		element = NULL;
	}
	
	return element;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::GetElement()
// -----------------------------------------------------------------------------
//


CElementBase * CPolicyStorage::GetElementL( TUint32 aElementId)
{
	CElementBase * element = NULL;
	

	if ( element == NULL)
	{
		element = FindElement( iElementCache, aElementId);
	}
	
	if ( element == NULL)
	{
		element = ElementHelper::CreateNewElementL( aElementId);
		element->iElementState = ENotLoaded;
		AddElementL( iElementCache, element);
		element->iReferenceCount = 0;	
		
		RDEBUG_2("PolicyEngineServer: New cache element (element cache count %d)", iElementCache.Count());
		
		//release unused eleements from cache
		if ( iEditedElements.Count() > KCacheLimit)
		{
			RDEBUG_2("PolicyEngineServer: Cache limit exceeded (%d)", KCacheLimit);
			ReleaseElements();
		}
	}
		
	return element;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::GetElement()
// -----------------------------------------------------------------------------
//

CElementBase * CPolicyStorage::GetElementL( const TDesC8& iPolicyId )
{
	//map external id to internal id 
	TUint32 settingId = MapRealIdL( iPolicyId, ETrue);
	
	CElementBase * element = NULL;
	
	//and if valid mapping exists, get element from storage
	if ( settingId )
	{
		element = GetElementL( settingId);	
	}
	
	return element;	
}


// -----------------------------------------------------------------------------
// CPolicyStorage::FindElement()
// -----------------------------------------------------------------------------
//

CElementBase * CPolicyStorage::FindElement( RArray<CElementBase*> &aElementList, const TUint32 aElementId)
{
	__UHEAP_MARK;

	TLinearOrder<CElementBase*> linearOrder(&ElementHelper::CompareElements);
	CElementBase referenceElement( aElementId);
	
	TInt index = aElementList.FindInOrder( &referenceElement, linearOrder);
	
	CElementBase * element = NULL;
	if ( index >= 0)
	{
		element = aElementList[index];
	}
	
	__UHEAP_MARKEND;
	
	return element;

}



// -----------------------------------------------------------------------------
// CPolicyStorage::AddElementL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::AddElementL( RArray<CElementBase*> &aElementList, CElementBase * aElement)
{
	TLinearOrder<CElementBase*> linearOrder(&ElementHelper::CompareElements);
	aElementList.InsertInOrder( aElement, linearOrder);
}



// -----------------------------------------------------------------------------
// CPolicyStorage::MapRealId()
// -----------------------------------------------------------------------------
//
TUint32 CPolicyStorage::MapRealIdL( const TDesC8& aElementRealId, const TBool aOnlyCommitted)
{
	iSearchedUids.Reset();
	TUint32 retVal = 0;
	
	if ( !aOnlyCommitted )
	{
		retVal = GetEditedMappingsL( aElementRealId);	
	}
	
	if ( !retVal)
	{
		retVal = GetCommittedMappingsL( aElementRealId);	
	}
	
		
	if ( !retVal)
	{
		retVal = GetMappingsFromDriveL( aElementRealId, EDriveC, EFalse);	
	}

	if ( !retVal)
	{
		retVal = GetMappingsFromDriveL( aElementRealId, EDriveZ, EFalse);	
	}

	return retVal;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::GetEditedMappingsL()
// -----------------------------------------------------------------------------
//

TUint32 CPolicyStorage::GetEditedMappingsL( const TDesC8& aElementRealId)
{
	//seach matching external ids from edited elements
	TLinearOrder<TUint32> linearOrder( &CompareIDs);

	for ( TInt i(0); i < iEditedElements.Count(); i++)
	{
		CElementBase * element = iEditedElements[i];
			
		if ( element->iElementType == ERule || element->iElementType == EPolicy || element->iElementType == EPolicySet)
		{
			CheckElementL( element);
		
			if ( element->iElementState == EEditableCacheElement ||  element->iElementState == EEditedElement)
			{
				if ( aElementRealId.Compare( *(element->iExternalId)) == 0)
				{
					iSearchedUids.InsertInOrder( element->GetId(), linearOrder);
					return element->iElementId;
				}
			}
		}
	}
	
	return 0;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::GetCommittedMappingsL()
// -----------------------------------------------------------------------------
//
TUint32 CPolicyStorage::GetCommittedMappingsL( const TDesC8& aElementRealId)
{
	//seach matching external ids from edited elements
	TLinearOrder<TUint32> linearOrder( &CompareIDs);

	for ( TInt i(0); i < iElementCache.Count(); i++)
	{
		CElementBase * element = iElementCache[i];
			
		if ( element->iElementType == ERule || element->iElementType == EPolicy || element->iElementType == EPolicySet)
		{
			if ( iSearchedUids.Find( element->GetId()) >= 0)
			{
				CheckElementL( element);
		
				if ( element->iElementState == ECacheElement)
				{
					if ( aElementRealId.Compare( *(element->iExternalId)) == 0)
					{
						iSearchedUids.InsertInOrder( element->GetId(), linearOrder);
						return element->iElementId;
					}
				}
			}
		}
	}
	
	return 0;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::GetMappingsFromDriveL()
// -----------------------------------------------------------------------------
//
TUint32 CPolicyStorage::GetMappingsFromDriveL( const TDesC8& aElementRealId, TDriveNumber aDrive, TBool aUpdateList)
{
	//Select correct drive
	TPtrC bathPtr( KCDrive);
	
	if ( aDrive == EDriveZ)
	{
		bathPtr.Set( KZDrive);
	}
	
	//Open directory and get file list 
	RDir dir;
	CleanupClosePushL( dir);
	
	TInt err = dir.Open( iRFs, bathPtr, KEntryAttNormal);

	User::LeaveIfError( err);
	err = KErrNone;
	
	TUint32 retVal = 0;

	do 
	{
		//read entries (files)
		TEntryArray array;
		err = dir.Read( array);
		if ( KErrEof != err)
		{
			User::LeaveIfError( err);
		}	
	
		for ( TInt i = 0; i < array.Count(); i++)
		{
			const TEntry& entry = array[i];
		
			TPtrC ptr = entry.iName;
			TInt index = ptr.Locate('.');
			User::LeaveIfError( index);
			
			TLex lex( ptr.Left( index));
			TUint32 id;
			lex.Val( id, EDecimal);			
			TInt type = id & 0xff;
			
			if ( type == ERule || type == EPolicy || type == EPolicySet )
			{
				TBuf<150> file = bathPtr;
				file.Append( entry.iName);
			
				if ( ReadRealIdL( file, aElementRealId, aDrive))
				{
					retVal = id;	
				
					if ( aUpdateList)
					{
						TLinearOrder<HBufC8> linearOrder( &CompareStrings);	
						iInvalidServerIds.InsertInOrder( aElementRealId.AllocL(), linearOrder);
					}
					
					break;
				}
			}
		}
	} while ( err == KErrNone && !retVal);
	
	CleanupStack::PopAndDestroy( &dir);	//RArray<TUint32> 

	return retVal;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::ReadRealIdL()
// -----------------------------------------------------------------------------
//
TBool CPolicyStorage::ReadRealIdL( const TDesC& aFileName, const TDesC8& aElementRealId, TDriveNumber aDrive)
{
	TBool retVal = EFalse;

	//open file
	RFile file;

	TInt err = KErrNone;
	
	//check first from c-drive
	if ( aDrive == EDriveZ)
	{
		TBuf<100> buf( aFileName);
		buf.Replace( 0, KDriveLetterLength, KCDriveLetter);
		
		err = file.Open( iRFs, buf, EFileRead);
	}
	else
	{
		err = KErrNotFound;
	}

	if ( err == KErrNotFound)
	{
		err = file.Open( iRFs, aFileName, EFileRead);
	}
	
	CleanupClosePushL( file);
	if ( err == KErrNone)
	{
		TInt size;
		User::LeaveIfError( file.Size( size));
		
		if ( size )
		{
			HBufC8* content = HBufC8::NewLC( size);
			TPtr8 ptr = content->Des();
			
			User::LeaveIfError( file.Read( ptr));

			TInt index = ptr.FindF( aElementRealId);
			if ( index > 0)
			{
				if ( ptr[ index + aElementRealId.Length()] == '"' && ptr[ index - 1] == '"')
				{
					TInt identifierIndex = ptr.FindF(  PolicyLanguage::Rule::RuleId[ENative]);
					TInt length = 0;
					if ( identifierIndex < 0 )
					{
						identifierIndex = ptr.FindF(  PolicyLanguage::Policy::PolicyId[ENative]);
					    length = PolicyLanguage::Policy::PolicyId[ENative].Length();
						if ( identifierIndex < 0 )
						{
							identifierIndex = ptr.FindF(  PolicyLanguage::PolicySet::PolicySetId[ENative]);
						    length = PolicyLanguage::PolicySet::PolicySetId[ENative].Length();
						}
					}
					else
					{
						length = PolicyLanguage::Rule::RuleId[ENative].Length();
					}

					if ( (identifierIndex + length  +2 ) == index)
					{
						retVal = ETrue;
					}
				}
			}
						
			CleanupStack::PopAndDestroy( content);
		}
			
	}

	CleanupStack::PopAndDestroy( &file);
	return retVal;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::ResetRealIdValidTest()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::ResetRealIdValidTest()
{
	iInvalidServerIds.ResetAndDestroy();
}



// -----------------------------------------------------------------------------
// CPolicyStorage::IsRealIdValidL()
// -----------------------------------------------------------------------------
//

TBool CPolicyStorage::IsRealIdValidL( const TDesC8& aElementRealId)
{	
	iSearchedUids.Reset();
	TLinearOrder<HBufC8> linearOrder( &CompareStrings);	
	
	HBufC8 * buf = aElementRealId.Alloc();
	if ( iInvalidServerIds.Find( buf) >= 0 )
	{
		delete buf;
		return EFalse;
	}
	delete buf;
	
	
	TUint32 id = GetEditedMappingsL( aElementRealId);
	
	if ( !id )
	{
		id = GetCommittedMappingsL( aElementRealId);	
	}
	
	if (!id)
	{	
		id = GetMappingsFromDriveL( aElementRealId, EDriveC, ETrue);
	}

	if (!id)
	{	
		id = GetMappingsFromDriveL( aElementRealId, EDriveZ, ETrue);
	}
	
	if ( !id)
	{
		iInvalidServerIds.InsertInOrder( aElementRealId.AllocL(), linearOrder);
	}
	
	return !id;	
}

// -----------------------------------------------------------------------------
// CPolicyStorage::CreateId()
// -----------------------------------------------------------------------------
//

TUint32 CPolicyStorage::CreateIdL( TNativeElementTypes &aType)
{
	__UHEAP_MARK;

	TUint32 nextFreeId = KNextFreeId;
	TUint32 nextId(0);
	TInt id(0);
	TInt err( KErrNone);

	do
	{
		if ( id == 0)
		{
			//get next free id from CentRep
			err = iCentRep->Get( nextFreeId, id);
		}

		if ( err != KErrNone)
		{
			id = 1;
		}
		
		//reset counter
		if ( id > 0xFFFFFF )
		{
			id = 0;
		}
	
		//increase next free id value 
		nextId = id++;

		//create id (combination id next free id and element type)
		nextId = nextId << 8;
		nextId = nextId | aType;


	    RDEBUG_2("PolicyEngineServer: Next free id: %d", nextId);
	}
	while ( !IsIdValidL( nextId));

	//backup value
	MakeBackupL( KNextFreeId);
	
	//save new value
	if ( err != KErrNone)
	{
		User::LeaveIfError( iCentRep->Create( KNextFreeId, id));
	}
	else
	{
		User::LeaveIfError( iCentRep->Set( KNextFreeId, id));
	}

	 
	__UHEAP_MARKEND; 
	 
	return nextId;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::GetEditedElements()
// -----------------------------------------------------------------------------
//
TBool CPolicyStorage::IsIdValidL( const TUint32& aId)
{
    RDEBUG("PolicyEngineServer: CPolicyStorage::IsIdValidL");


	//parse element file name
	TBuf<KUidLength> elementFile;
	elementFile.NumFixedWidth( aId, EDecimal, KUidLength);

	//buffer for file name (C-drive)
	TBuf<KFullFileNameLength> fileName;
	fileName.Append( KCDrive);
	fileName.Append( elementFile);
	fileName.Append( KFileExtensio);
	
	//try open file from C-drive
	RFile file;
	CleanupClosePushL( file);
	TInt err = file.Open( iRFs, fileName, EFileRead);
	
	//and if file does not exist in C-drive try again from z-drive
	if ( err == KErrNotFound || err == KErrPathNotFound)
	{
		fileName.Replace( 0, KDriveLetterLength, KZDriveLetter);
		err = file.Open( iRFs, fileName, EFileRead);
	}
	
	CleanupStack::PopAndDestroy( &file);		

#ifdef __DEBUG
 	if ( err == KErrNotFound)
 	{
 		RDEBUG("PolicyEngineServer: Id is not valid!");
 	}
#endif
	
	return err == KErrNotFound;
}



// -----------------------------------------------------------------------------
// CPolicyStorage::GetEditedElements()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::GetEditedElementsL( CSettingEnforcementManager* aSEManager)
{
	//Check is element changed
	for ( TInt i(0); i < iEditedElements.Count(); i++)
	{
		CElementBase * element = iEditedElements[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));

		switch ( element->iElementState)
		{
			//these states indicates changes
			case EDeletedEditableElement:
			case ENewElement:
			case EEditedElement:
				//add element to modified element list
				aSEManager->AddModifiedElementL( element);
			break;
			default:
			break;
		}
	
	}
}

// -----------------------------------------------------------------------------
// CPolicyStorage::ResetEditableMemory()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::ResetEditableMemory()
{
	//release memory used in edited element cache
	for ( TInt i(0); i < iEditedElements.Count();)
	{
		CElementBase * element = iEditedElements[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));
		delete element;
		iEditedElements.Remove( i);
	}

    RDEBUG("PolicyEngineServer: Edited elements removed!");
}

// -----------------------------------------------------------------------------
// CPolicyStorage::SaveEditableMemoryL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::SaveEditableMemoryL()
{
    RDEBUG("PolicyEngineServer: Save new and modified elements");


	//save modified elements
	for ( TInt i(0); i < iEditedElements.Count(); i++)
	{
		CElementBase * element = iEditedElements[i];
		__ASSERT_ALWAYS ( element->iReferenceCount == 0, User::Panic(PolicyStoragePanic, KErrGeneral));

		TInt elementChanged = ETrue;
	
		switch ( element->iElementState)
		{
			case EDeletedEditableElement:
				//delete element and remove possible id mappings...
				RDEBUG_2("Element is deleted (id %d)", element->GetId());
				element->DeleteElementL();
			break;
			case ENewElement:
				//save element (Save new elements recursive)
				element->SaveElementL( ETrue);
				RDEBUG_2("New element (id %d)", element->GetId());
			break;
			case EEditedElement:
				//save element (not recursive)
				RDEBUG_2("Element is modified (id %d)", element->GetId());
				element->SaveElementL( EFalse);
			break;
			default:
				//no changes in elements
				elementChanged = EFalse;
			break;
		}
	
		if ( elementChanged )
		{
			//find elements from element cache (by id) 
			CElementBase referenceElement( element->GetId());
			TLinearOrder<CElementBase*> linearOrder(&ElementHelper::CompareElements);
			TInt index = iElementCache.FindInOrder( &referenceElement, linearOrder);
	
			//if eleement is in the element cache remove it to depricated list
			//Next time element is used it will be loaded from RAM
			if ( index >= 0)
			{
				CElementBase * cacheElement = iElementCache[index];
				iElementCache.Remove( index);
		
				if ( element->iReferenceCount == 0 )
				{
					delete cacheElement;
				}
			else
				{
					cacheElement->iElementState = EDepricated;
					iDepricatedElements.AppendL( cacheElement);
					RDEBUG_2("PolicyEngineServer: Depricated element list", iDepricatedElements.Count());
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyStorage::CheckCommitStateL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::CheckCommitStateL()
{
	RDEBUG( "PolicyEngineServer: CPolicyStorage::CheckCommitStateL");

	TInt state;	
	iCentRep->Get( KCommitFlag, state);
	
	if ( state == EFalse)
	{
		RDEBUG( "PolicyEngineServer: Commit flag is not set, restore backup....");
		//if commit state is "not committed" restore backup files
		RestoreBackupL();
	}

	//Check also commit state in centreptool
	RCentRepTool centRepTool;
	User::LeaveIfError( centRepTool.Connect());
	TInt err = centRepTool.CheckCommitState();
	centRepTool.Close();
	
	if ( state)
	{
		//commit server id list
		CFileMan * fileMan = CFileMan::NewL( iRFs);
	
		//create backup
		err = fileMan->Copy( KServerIdFileTmp, KServerIdFile, CFileMan::EOverWrite);	
		
		if ( err == KErrNone || err == KErrNotFound)
		{
			fileMan->Delete( KServerIdFileTmp, 0);
			err = KErrNone;	
		}
		
		delete fileMan;
	}
	
	
	//if restore ok, turn commit flag to true state which indicates that storages are in valid state
	if ( err == KErrNone)
	{
		RDEBUG( "PolicyEngineServer: Set commit flag on!");
		iCentRep->Set( KCommitFlag, ETrue);
	}

	//remove back files
	RDEBUG( "PolicyEngineServer: Remove backup!");
	RemoveBackupL();
}

// -----------------------------------------------------------------------------
// CPolicyStorage::StartCommitSessionL()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::StartCommitSessionL()
{
	//Only ONE management session can be active!
	__ASSERT_ALWAYS ( iCommitted, User::Panic( Panics::PolicyStoragePanic, KErrCorrupt));

	iCommitted = EFalse;

	//Set storage state to "Not committed", ensures rollback in error situations
	iCentRep->Set( KCommitFlag, EFalse);	
}

// -----------------------------------------------------------------------------
// CPolicyStorage::CreatePathsL()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::CreatePaths()
{
	//create directory
	RDEBUG("PolicyEngineServer: Create private path");
	TInt err = iRFs.CreatePrivatePath( EDriveC);
	
	if ( err == KErrNone || err == KErrAlreadyExists )
	{
		//create also backup path
		RDEBUG("PolicyEngineServer: Create backup path");
		err = iRFs.MkDir( KBackupPath);
	}
	
	return err;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::CommitChangesL()
// -----------------------------------------------------------------------------
//

void CPolicyStorage::CommitChangesL( TBool aAcceptCommit)
{
	RDEBUG("PolicyEngineServer: Commit changes!");

	if ( !aAcceptCommit)
	{
		//if commit is not accepted, restore files!
		RDEBUG("PolicyEngineServer: Commit is not accepted");
		CheckCommitStateL();
	}

	//Set storage state to "Committed"
	RDEBUG("PolicyEngineServer: Set commit flag state to committed");
	iCentRep->Set( KCommitFlag, ETrue);
	
	if ( aAcceptCommit)
	{
		//This removes all backups
		CheckCommitStateL();
	}
	
	
	//Commit changes
	iCommitted = ETrue;
	
	RDEBUG("PolicyEngineServer: Commit changes - end");
}

// -----------------------------------------------------------------------------
// CPolicyStorage::AddNewServerId()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::AddNewServerId( const TDesC8& aServerId)
{
	TRAPD( err,	
	iServerIdList.AppendL( aServerId.AllocL());
	SaveServerIdListL());
	
	if ( err != KErrNone)
	{
		TRAP( err,	LoadServerIdListL());
	}
	
	return err;	
}

// -----------------------------------------------------------------------------
// CPolicyStorage::RemoveServerId()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::RemoveServerId( const TDesC8& aServerId)
{
	for ( TInt i(0); i < iServerIdList.Count(); i++)
	{
		if ( *(iServerIdList[i]) == aServerId)
		{
			delete (iServerIdList[i]);
			iServerIdList.Remove( i);
			break;
		}
	}

	TRAPD( err,	SaveServerIdListL());
	
	if ( err != KErrNone)
	{
		TRAP( err,	LoadServerIdListL());
	}
	
	return err;
}

// -----------------------------------------------------------------------------
// CPolicyStorage::ResetServerIdList()
// -----------------------------------------------------------------------------
//
TInt CPolicyStorage::ResetServerIdList()
{
	iServerIdList.ResetAndDestroy();

	TRAPD( err,	SaveServerIdListL());
	
	if ( err != KErrNone)
	{
		TRAP( err,	LoadServerIdListL());
	}
	
	return err;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::PrintServerIdList()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::PrintServerIdList()
{
	for ( TInt i(0); i < iServerIdList.Count(); i++)
	{
		TPtrC8 ptr = *iServerIdList[i];

		RDEBUG8_2("CPolicyStorage::PrintServerIdList() Server id: (%S)", &ptr);
	}
}


// -----------------------------------------------------------------------------
// CPolicyStorage::SaveServerIdList()
// -----------------------------------------------------------------------------
//
TBool CPolicyStorage::IsServerIdValid( const TDesC8& aServerId)
{
	for ( TInt i(0); i < iServerIdList.Count(); i++)
	{
		if ( *(iServerIdList[i]) == aServerId)
		{
			return ETrue;
		}
	}	
	
	return EFalse;
}


// -----------------------------------------------------------------------------
// CPolicyStorage::SaveServerIdList()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::SaveServerIdListL()
{
	__UHEAP_MARK;

	//Open file stream. RFile ownership changes for CDirectFileStore
	RFile file;
	CleanupClosePushL( file);
	User::LeaveIfError( file.Replace( iRFs, KServerIdFileTmp, EFileWrite));
		
	CDirectFileStore * directFileStore = CDirectFileStore::NewL( file);
	CleanupStack::Pop( &file);
	CleanupStack::PushL( directFileStore);

	//setup write stream
	directFileStore->SetTypeL( TUidType(KDirectFileStoreLayoutUid, KNullUid, KPolicyEngineRepositoryID)) ; 
		
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*directFileStore) ;

	//Externalize
	TInt32 count = iServerIdList.Count();
	rootStream << count;
	
	for ( TInt i(0); i < count; i++)
	{
		rootStream << *(iServerIdList[i]);
	}

	rootStream.CommitL();

	directFileStore->SetRootL(rootStreamId);
	directFileStore->CommitL();

	//destroy
	CleanupStack::PopAndDestroy( 2, directFileStore);	
	
	__UHEAP_MARKEND;	
}

// -----------------------------------------------------------------------------
// CPolicyStorage::LoadServerIdList()
// -----------------------------------------------------------------------------
//
void CPolicyStorage::LoadServerIdListL()
{
	RDEBUG("CPolicyStorage: LoadServerIdListL()");

	RFile file;
	iServerIdList.ResetAndDestroy();

	//Open file
	TInt err = file.Open( iRFs, KServerIdFile, EFileRead);
	
	if ( err == KErrNotFound)
	{
		RDEBUG("CPolicyStorage: ServerID file not found!");
		return;
	}
	
	User::LeaveIfError( err);

	//Open file stream
	CDirectFileStore* directFileStore = CDirectFileStore::FromLC ( file);
	TStreamId rootStreamId = directFileStore->Root() ;
	
	//setup read stream
	RStoreReadStream rootStream ;
	rootStream.OpenLC( *directFileStore, rootStreamId);

	//Internalize
	TInt32 count;
	rootStream >> count;
	
	for ( TInt i(0); i < count; i++)
	{
		HBufC8 * buf = HBufC8::NewLC( rootStream, 200);
		iServerIdList.AppendL( buf);
		CleanupStack::Pop( buf);
	}

	PrintServerIdList();
	
	CleanupStack::PopAndDestroy( 2, directFileStore);
}



// -----------------------------------------------------------------------------
// TElementReserver::TElementReserver()
// -----------------------------------------------------------------------------
//

TElementReserver::TElementReserver()
	: iElement( NULL), iElements( NULL), iElementList( NULL)
	{	
	}

// -----------------------------------------------------------------------------
// TElementReserver::TElementReserver()
// -----------------------------------------------------------------------------
//

TElementReserver::TElementReserver( CElementBase * aElement)
	: iElement( aElement), iElements( NULL), iElementList( NULL)
	{
	//reserve element
	aElement->ReserveElement();
	}

// -----------------------------------------------------------------------------
// TElementReserver::TElementReserver()
// -----------------------------------------------------------------------------
//

TElementReserver::TElementReserver( RElementContainer* aElements)
	: iElement( NULL), iElements( aElements), iElementList( NULL)
	{
	//Reserver all elements in element list
	for (TInt i(0); i < iElements->Count(); i++)
		{
		(*iElements)[i]->iElement->ReserveElement();
		}
	}

// -----------------------------------------------------------------------------
// TElementReserver::~TElementReserver()
// -----------------------------------------------------------------------------
//

TElementReserver::~TElementReserver()
	{
	DoRelease();
	}

// -----------------------------------------------------------------------------
// TElementReserver::ReserveL()
// -----------------------------------------------------------------------------
//

void TElementReserver::ReserveL( CElementBase* aElement)
{
	//create new element list if needed
	if (!iElementList)
	{
		iElementList = new (ELeave) RElementList();
	}

	//add element to list and reserve it
	iElementList->AppendL( aElement);
	aElement->ReserveElement();
}

// -----------------------------------------------------------------------------
// TElementReserver::DoRelease()
// -----------------------------------------------------------------------------
//


void TElementReserver::Release()
	{	
	}


void TElementReserver::DoRelease()
{
	//release eleements...
	if ( iElement )
	{
		iElement->ReleaseElement();
	}
	else if ( iElements )
	{
		for (TInt i(0); i < iElements->Count(); i++)
		{
			(*iElements)[i]->iElement->ReleaseElement();
		}		
	}
	else if ( iElementList)
	{
		for (TInt i(0); i < iElementList->Count(); i++)
		{
			(*iElementList)[i]->ReleaseElement();
		}		
		
		iElementList->Close();
		delete iElementList;
		iElementList = NULL;	
	}
}

