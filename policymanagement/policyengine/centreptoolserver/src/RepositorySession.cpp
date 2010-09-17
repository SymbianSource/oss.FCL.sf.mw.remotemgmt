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

#include "RepositorySession.h"
#include "IniFileHelper.h"
#include "constants.h"
#include "debug.h"
#include "PolicyEnginePrivateCRKeys.h"

#include <centralrepository.h>
#include <s32file.h>

const TUid KCentralRepositoryUID     = { 0x10202BE9 };


// -----------------------------------------------------------------------------
// CRepositoryContent::CRepositoryContent()
// -----------------------------------------------------------------------------
//

CRepositoryContent::CRepositoryContent( const TUid& aUid, HBufC* aContent)
	: iContentPtr( *aContent), iContent( aContent), iUid( aUid)
{
}

// -----------------------------------------------------------------------------
// CRepositoryContent::CRepositoryContent()
// -----------------------------------------------------------------------------
//
CRepositoryContent::CRepositoryContent( const TUid& aUid)
	: iUid( aUid)
{
}


// -----------------------------------------------------------------------------
// CRepositoryContent::~CRepositoryContent()
// -----------------------------------------------------------------------------
//

CRepositoryContent::~CRepositoryContent()
{
	ReleaseResources();	
	
	iRangeSettings.Close();
	iIndividualSettings.Close();	
	iRangeMetas.Close();
}

// -----------------------------------------------------------------------------
// CRepositoryContent::NewL()
// -----------------------------------------------------------------------------
//
CRepositoryContent * CRepositoryContent::NewL( const TUid& aUid)
{
	CRepositoryContent *self = new (ELeave) CRepositoryContent( aUid);
	CleanupStack::PushL( self);	
	
	self->ConstructL();
	
	CleanupStack::Pop( self);
	return self;
}

// -----------------------------------------------------------------------------
// CRepositoryContent::NewL()
// -----------------------------------------------------------------------------
//
CRepositoryContent * CRepositoryContent::NewL( const TUid& aUid, HBufC* aContent)
{
	CRepositoryContent *self = new (ELeave) CRepositoryContent( aUid, aContent);
	CleanupStack::PushL( self);	
	
	self->ConstructL();
	
	CleanupStack::Pop( self);
	return self;
}

// -----------------------------------------------------------------------------
// CRepositoryContent::ConstructL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::ConstructL()
{
	RDEBUG("CentRepTool: New CRepostioryContent");
	iDefaultSetting = new (ELeave) CDefaultSetting();
}


// -----------------------------------------------------------------------------
// CRepositoryContent::ReleaseResources()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::ReleaseResources()
{
	RDEBUG("CentRepTool: Release CRepository content resources");

	delete iContent;
	iContent = 0;
	
	delete iDefaultSetting;
	iDefaultSetting = 0;

	iRangeSettings.ResetAndDestroy();
	iIndividualSettings.ResetAndDestroy();
	iRangeMetas.ResetAndDestroy();
	

}

// -----------------------------------------------------------------------------
// CRepositoryContent::ReadHeaderL()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::ReadHeaderL()
{
	RDEBUG("CentRepTool: Read ini-file header(.txt format)");


	using namespace IniConstants;

	TInt index = iContentPtr.FindF( KPlatSecSection);
	User::LeaveIfError( index);
	
	//separate each section to own ptr's	
	iHeaderPtr.Set( iContentPtr.Left( index + KPlatSecStringLength));			//content before [platsec]
	iContentPtr.Set( iContentPtr.Mid( index + KPlatSecStringLength));		//content after [platsec] (platsec and settings)
	
	iDefaultMetaPtr.Set( KNullDesC);
	iOwnerPtr.Set( KNullDesC);
	
	//locate owner section and insulate it own ptr
	index = iHeaderPtr.FindF( KOwnerSection);
	TPtrC * lastPart = 0;
	
	if ( index >= 0)
	{
		iOwnerPtr.Set( iHeaderPtr.Mid( index + KOwnerStringLength));					//content after [owner]
		//pointer to the last section before [platsec] section
		lastPart = &iOwnerPtr;
	}
	else
	{
		//pointer to the last section before [platsec] section
		lastPart = &iHeaderPtr;
	}

	//locate default meta section
	index = lastPart->FindF( KDefaultMetaSection);
	
	
	if ( index >= 0)
	{
		iDefaultMetaPtr.Set( lastPart->Mid( index + KDefaultMetaStringLength));			//content after [defaultmeta]
		lastPart->Set( lastPart->Left( index));
		lastPart = &iDefaultMetaPtr;
	}

	//set last section length...
	index = lastPart->FindF( KPlatSecSection);
	User::LeaveIfError( index);
	
	lastPart->Set( lastPart->Left( index));
}


// -----------------------------------------------------------------------------
// CRepositoryContent::ReadDefaultMetaL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::ReadOptionalDataL()
{
	RDEBUG("CentRepTool: Read ini-file optional data(.txt format)");

	TPtrC owner = iOwnerPtr;
		
	//read next line from owner section
	TPtrC line;
	TIniFileHelper::NextLine( owner, line);		
	
	//read owner secure id from line
	if ( line.Length())
	{
		TLex lex( line);
		TInt32 value;
	 	User::LeaveIfError( TIniFileHelper::ReadNumber( value, lex));
	 	iOwner.iUid = value;
	}

	//read 
	TPtrC defaultMeta = iDefaultMetaPtr;
	
	do
	{
		line.Set( KNullDesC);
		TIniFileHelper::NextLine( defaultMeta, line);		
	
		//try to create RangeMeta data 
		TLex lex( line);
		CRangeMeta * rangeMeta = CRangeMeta::NewL( lex);
		
		//if the line content is not a range meta data check is the content is it default meta data 
		if ( rangeMeta)
		{
			iRangeMetas.AppendL( rangeMeta);
		}
		else
		{
			if ( !lex.Eos())
			{
				User::LeaveIfError( TIniFileHelper::ReadUNumber( iDefaultMeta, lex));
				User::LeaveIfError( !lex.Eos());			
			}
		}
		
	} while ( defaultMeta.Length());
	
}


// -----------------------------------------------------------------------------
// CRepositoryContent::ReadPlatSecL()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::ReadPlatSecL()
{
	RDEBUG("CentRepTool: Read ini-file platsec-section(.txt format)");

	using namespace IniConstants;

	TInt index = iContentPtr.FindF( KMainSection );
	User::LeaveIfError( index);

	TPtrC platSecPtr = iContentPtr.Left( index);

	do
	{
		TPtrC sectionPtr = platSecPtr;
		
		TIniFileHelper::NextLine( platSecPtr, sectionPtr);
		
		if ( !sectionPtr.Length())
		{
			break;
		}
		
		TLex lex( sectionPtr);
		CRangeSetting* rangeSetting = CRangeSetting::NewL( lex);
		
		if ( rangeSetting )
		{
			iRangeSettings.AppendL( rangeSetting);
		}
		else 
		{
			CDefaultSetting* defaultSetting = CDefaultSetting::NewL( lex);
				
			if ( defaultSetting  )
			{
				if ( iDefaultSetting && iDefaultSetting->ContainsSecuritySettings())
				{
					//there can be only one default settings in the repository -> Corrupted
					User::Leave( KErrCorrupt);
				}
				else
				{
					delete iDefaultSetting;
					iDefaultSetting = defaultSetting;	
				}
			}
			else
			{
				//line contains invalid format -> Corrupted
				User::Leave( KErrCorrupt);
			}
		}

		
	} while ( platSecPtr.Length());
	
	iContentPtr.Set( iContentPtr.Mid( index + IniConstants::KMainSectionStringLength));
}
// -----------------------------------------------------------------------------
// CRepositoryContent::FindIndividualSetting()
// -----------------------------------------------------------------------------
//
CIndividualSetting* CRepositoryContent::FindIndividualSetting( const TUint32 aSettingId)
{
	//Find individual setting which has same id and return it...
	for ( TInt i(0); i < iIndividualSettings.Count(); i++)
	{
		if ( iIndividualSettings[i]->iSettingId == aSettingId)
		{
			return iIndividualSettings[i];
		}
	}
	
	return 0;
}

// -----------------------------------------------------------------------------
// CRepositoryContent::WriteFileL()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::WriteFileL( RFile& aFile)
{
	TBuf<600> buffer;

	TIniFileHelper ini;

	ini.StartWrite( aFile);
	
	ini.WriteToFile( iHeaderPtr);
	ini.LineFeed();
	
	if ( iDefaultSetting)
	{
		ini.WriteToFile( iDefaultSetting->SecurityString( buffer));
		ini.LineFeed();
	}

	for ( TInt i(0); i < iRangeSettings.Count(); i++)
	{
		ini.WriteToFile( iRangeSettings[i]->SettingDefinition( buffer));
		ini.WriteToFile( iRangeSettings[i]->SecurityString( buffer));
		ini.LineFeed();
	}
	
	ini.WriteToFile( IniConstants::KMainSection);
	ini.LineFeed();
	
	for ( TInt i(0); i < iIndividualSettings.Count(); i++)
	{
		ini.WriteToFile( iIndividualSettings[i]->SettingDefinition( buffer));
		ini.WriteToFile( iIndividualSettings[i]->SecurityString( buffer));
		ini.LineFeed();
	}
	
	ini.FinishWrite();
}


// -----------------------------------------------------------------------------
// CRepositoryContent::CheckRangeValidity()
// -----------------------------------------------------------------------------
//

TBool CRepositoryContent::CheckRangeValidity()
{
	RDEBUG("CentRepTool: Check range setting validity");

	//evaluate range setting validity (range setting start value must be greater than previous setting end value)
	TLinearOrder<CRangeSetting> linearOrder( &TIniFileHelper::CompareElements);
	iRangeSettings.Sort( linearOrder);
	
	TBool rangesValid = ETrue;
	
	TInt rangeCount = iRangeSettings.Count();
	for ( TInt i(0); i < rangeCount; i++)
	{
		if ( i + 1 < rangeCount)
		{
			if ( iRangeSettings[i]->iEnd >= iRangeSettings[i+1]->iStart)
			{
				rangesValid = EFalse;
				break;
			}
		}
	}	
	
	return rangesValid;
}

// -----------------------------------------------------------------------------
// CRepositoryContent::CreateMaskBackupL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::CreateMaskBackupL( RPointerArray<CRangeMeta>& aSettings, TUint32 aCompareValue, TUint32 aMask)
{
	RDEBUG_3("CentRepTool: Turn backup flag on for mask ( %d mask:%d)", aCompareValue, aMask);

	TInt rangeCount = iRangeMetas.Count();

	//search existing settings
	for ( TInt i(0); i < rangeCount; i++)
	{
		CRangeMeta * setting = iRangeMetas[i];
		
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}		
		
		if ( setting->iStart == aCompareValue && setting->iMask == aMask)
		{
			aSettings.AppendL( setting);	
		}
	}		
	
	//create new one, if setting doesn't exist
	if ( !aSettings.Count() )
	{	
		CRangeMeta * setting = CRangeMeta::NewL( aCompareValue, 0, aMask, KBackupBitMask);
		TInt err = KErrNone;
		TRAP(err, aSettings.AppendL( setting ));
		if(err != KErrNone)
		    {
		    delete setting;
			User::Leave( err );
		    }
		else
		    {
		    TRAP(err, iRangeMetas.AppendL( setting));
		    if(err != KErrNone)
		        {
		        TInt indx = aSettings.Find( setting );
		        if(indx != KErrNotFound)
		            {
		            aSettings.Remove(indx);
					User::Leave( err );
		            }
		        
		        }
		    }
	}
}


// -----------------------------------------------------------------------------
// CRepositoryContent::CreateRangeBackupL()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::CreateRangeBackupL( RPointerArray<CRangeMeta>& aSettings, TUint32 aRangeStart, TUint32 aRangeEnd)
{
	RDEBUG_3("CentRepTool: Create new range setting (%d -> %d)", aRangeStart, aRangeEnd);

	//evaluate range setting validity (range setting start value must be greater than previous setting end value)
	//evaluate range setting validity (range setting start value must be greater than previous setting end value)
	TLinearOrder<CRangeMeta> linearOrder( &TIniFileHelper::CompareElements);
	iRangeMetas.Sort( linearOrder);
	

	TInt rangeCount = iRangeMetas.Count();
	for ( TInt i(0); i < rangeCount; i++)
	{
		if ( i + 1 < rangeCount)
		{
			if ( iRangeMetas[i]->iEnd >= iRangeMetas[i+1]->iStart)
			{
				User::Leave( KErrCorrupt);
			}
		}
	}	
	
	//search existing settings
	for ( TInt i(0); i < rangeCount; i++)
	{
		CRangeMeta * setting = iRangeMetas[i];
		
		if ( setting->Type() != ERangeSetting )
		{
			continue;
		}		
		
		if ( setting->iStart == aRangeStart && setting->iEnd == aRangeEnd)
		{
			aSettings.AppendL( setting);	
		}
	}
	
	//create new one, if setting doesn't exist
	if ( !aSettings.Count() )
	{	
		CRangeMeta * setting = CRangeMeta::NewL( aRangeStart, aRangeEnd, 0, KBackupBitMask);
		aSettings.Append( setting );
		iRangeMetas.Append( setting);
	}	
	
	iRangeMetas.Sort( linearOrder);

}



// -----------------------------------------------------------------------------
// CRepositoryContent::FindRangeSetting()
// -----------------------------------------------------------------------------
//

void CRepositoryContent::CreateRangeSettingsL( RPointerArray<CRangeSetting>& aSettings, TUint32 aRangeStart, TUint32 aRangeEnd)
{
	RDEBUG_3("CentRepTool: Create new range setting (%d -> %d)", aRangeStart, aRangeEnd);

	//evaluate range setting validity (range setting start value must be greater than previous setting end value)
	//evaluate range setting validity (range setting start value must be greater than previous setting end value)
	TLinearOrder<CRangeSetting> linearOrder( &TIniFileHelper::CompareElements);
	iRangeSettings.Sort( linearOrder);
	

	TInt rangeCount = iRangeSettings.Count();
	for ( TInt i(0); i < rangeCount; i++)
	{
		if ( i + 1 < rangeCount)
		{
			if ( iRangeSettings[i]->iEnd >= iRangeSettings[i+1]->iStart)
			{
				User::Leave( KErrCorrupt);
			}
		}
	}	

	//search existing settings
	for ( TInt i(0); i < rangeCount; i++)
	{
		CRangeSetting * setting = iRangeSettings[i];
		
		if ( setting->Type() != ERangeSetting )
		{
			continue;
		}		
		
		if ( setting->iStart == aRangeStart && setting->iEnd == aRangeEnd)
		{
			aSettings.AppendL( setting);	
		}
	}
	
	//create new one, if setting doesn't exist
	if ( !aSettings.Count() )
	{	
		CRangeSetting * setting = CRangeSetting::NewL( aRangeStart, aRangeEnd, 0);
		TInt err = KErrNone;
		TRAP(err, aSettings.AppendL( setting ));
		if(err != KErrNone)
		    {
		    delete setting;
			User::Leave( err );
		    }
		else
		    {
		    TRAP(err, iRangeSettings.AppendL( setting));
		    if(err != KErrNone)
		        {
		        TInt indx = aSettings.Find( setting );
		        if(indx != KErrNotFound)
		            {
		            aSettings.Remove(indx);
					User::Leave( err );
		            }

		        }
		    }
	}	
	
	iRangeSettings.Sort( linearOrder);

}


// -----------------------------------------------------------------------------
// CRepositoryContent::CreateMaskSettingsL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::CreateMaskSettingsL( RPointerArray<CRangeSetting>& aSettings, TUint32 aCompareValue, TUint32 aMask)
{
	RDEBUG_3("CentRepTool: Create new mask setting ( %d mask:%d)", aCompareValue, aMask);

	TInt rangeCount = iRangeSettings.Count();

	//search existing settings
	for ( TInt i(0); i < rangeCount; i++)
	{
		CRangeSetting * setting = iRangeSettings[i];
		
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}		
		
		if ( setting->iStart == aCompareValue && setting->iMask == aMask)
		{
			aSettings.AppendL( setting);	
		}
	}		
	
	//create new one, if setting doesn't exist
	if ( !aSettings.Count() )
	{	
		CRangeSetting * setting = CRangeSetting::NewL( aCompareValue, 0, aMask);
		TInt err = KErrNone;
		TRAP(err, aSettings.AppendL( setting ));
		if(err != KErrNone)
		    {
		    delete setting;
			User::Leave( err );
		    }
		else
		    {
		    TRAP(err, iRangeSettings.AppendL( setting));
		    if(err != KErrNone)
		        {
		        TInt indx = aSettings.Find( setting );
		        if(indx != KErrNotFound)
		            {
		            aSettings.Remove(indx);
					User::Leave( err );
		            }

		        }
		    }
	}
	
	
}


// -----------------------------------------------------------------------------
// CRepositoryContent::ReadMainL()
// Read main secttion (individual settings) from CentRep ini.
// -----------------------------------------------------------------------------
//

void CRepositoryContent::ReadMainL()
{
	do
	{
		//get next setting from inifile
		TPtrC settingPtr = iContentPtr;
		TIniFileHelper::NextLine( iContentPtr, settingPtr);
		
		//Create new CIndividual setting
		TLex lex( settingPtr);
		
		if ( lex.Eos())
		{
			break;
		}

		CIndividualSetting * setting = CIndividualSetting::NewL( lex);		
		
		if ( setting )
		{
			//add settting to settings list
			AddIndividualSettingL( setting);
		}
		else
		{	
			//file is corrupted if setting is not valid
			User::Leave( KErrCorrupt);
		}

		//repeat until contetn left
	} while ( iContentPtr.Length());
}


// -----------------------------------------------------------------------------
// CRepositoryContent::FindSettingOrCreateL()
// -----------------------------------------------------------------------------
//
CIndividualSetting* CRepositoryContent::FindSettingOrCreateL( const TUint32& aSettingId)
{
	//try to find element with same id
	TLinearOrder<CIndividualSetting> linearOrder( &CIndividualSetting::CompareElements);
	CIndividualSetting* referenceElement = new (ELeave) CIndividualSetting( aSettingId);
	
	TInt index = iIndividualSettings.FindInOrder( referenceElement, linearOrder);
	
	CIndividualSetting * element = NULL;
	if ( index >= 0)
	{
		//if element exists return it and delete reference element...
		element = iIndividualSettings[index];
		delete referenceElement;
	}
	else
	{	
		//...or if not use reference elemetn and add it to setting list...
		element = referenceElement;
		AddIndividualSettingL( element);
	}
	
	//...return element
	return element;
	
}

// -----------------------------------------------------------------------------
// CRepositoryContent::AddElementL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::AddIndividualSettingL( CIndividualSetting* aSetting)
{
	if ( aSetting->ContainsSecuritySettings())
	{
		iSingleSecuritySettingCount++;	
	}
	
	TLinearOrder<CIndividualSetting> linearOrder( &CIndividualSetting::CompareElements);
	iIndividualSettings.InsertInOrder( aSetting, linearOrder);
}


// -----------------------------------------------------------------------------
// CRepositoryContent::CheckAccess()
// -----------------------------------------------------------------------------
//
TBool CRepositoryContent::CheckAccess( const RMessage2& aMessage, TUint32 aSettingId, TAccessType aAccessType)
{
	TBool retVal( ETrue);

	for ( TInt i(0); i < iRangeSettings.Count(); i++)
	{
		CRangeSetting* setting = iRangeSettings[ i];	
		
		if ( setting->Mask())
		{
			TUint32 masked = aSettingId & setting->Mask();
			if ( masked == setting->Start())
			{
				retVal = setting->CheckAccess( aMessage, aAccessType);
				if ( !retVal )
				{
					return EFalse;
				}
			}
		}
		else
		{
			if ( setting->Start() <= aSettingId && aSettingId <= setting->End())
			{
				retVal = setting->CheckAccess( aMessage, aAccessType);
				if ( !retVal )
				{
					return EFalse;
				}
			}
		}
	}

	if ( iDefaultSetting)	
	{
		retVal = iDefaultSetting->CheckAccess( aMessage, aAccessType);
	}
	
	return retVal;
}





// -----------------------------------------------------------------------------
// CRepositorySession::CRepositorySession()
// -----------------------------------------------------------------------------
//

CRepositorySession::CRepositorySession( TUid aRepositoryId)
	: iRepositoryId( aRepositoryId), iRepositoryInUse( EFalse)
{
}

// -----------------------------------------------------------------------------
// CRepositorySession::~CRepositorySession()
// -----------------------------------------------------------------------------
//

CRepositorySession::~CRepositorySession()
{
	//if session is not committed, restore file!


	//release resources....
	ReleaseResources();

	//close file server sessions
	iFile.Close();
	iFs.Close();
	
}


// -----------------------------------------------------------------------------
// CRepositorySession::ReleaseResources()
// -----------------------------------------------------------------------------
//
void CRepositorySession::ReleaseResources()
{
	delete iRepContent;
	iRepContent = 0;
	
	}


// -----------------------------------------------------------------------------
// CRepositorySession::NewL()
// -----------------------------------------------------------------------------
//

CRepositorySession * CRepositorySession::NewL( TUid aRepositoryId)
{
	CRepositorySession * self = new(ELeave) CRepositorySession( aRepositoryId);
	
	CleanupStack::PushL( self);
//	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CRepositorySession::CheckCommitStateL()
// -----------------------------------------------------------------------------
//

void CRepositorySession::CheckCommitStateL()
{
	//Get commit flag state from centrep
	CRepository * iCentRep = CRepository::NewLC( KPolicyEngineRepositoryID );
	
	TInt state;
	__ASSERT_ALWAYS( KErrNone == iCentRep->Get( KCommitFlag, state), User::Panic( IniConstants::KCentRepToolPanic, KErrCorrupt));
	
	if ( state == EFalse)
	{
		//restore backup if changes are not committed
		RestoreBackupL();
	}
	
	//remove backup
	RemoveBackupL();	
	
	CleanupStack::PopAndDestroy();	//CRepository
}

// -----------------------------------------------------------------------------
// CRepositorySession::RestoreBackupL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RestoreBackupL()
{

	using namespace IniConstants;

	RFs rfs;
	User::LeaveIfError( rfs.Connect() );
	CleanupClosePushL( rfs);

	//Open directory and get file list
	RDir dir;
	CleanupClosePushL( dir);
	
	TInt err = dir.Open( rfs, KBackupPath, KEntryAttNormal);
	
	if ( err == KErrPathNotFound)
	{
		CreatePath();
		err = dir.Open( rfs, KBackupPath, KEntryAttNormal);
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

			TBuf<KPathLength> backupFile;
			backupFile.Append( KBackupPath);
			TBuf<KPathLength> name;
			name.Append( KDriveC);
		

			//repository backup
			//original file name and location
			name.Append( entry.iName);
		
			//backup file name and location
			backupFile.Append( entry.iName);
			//copy file to work directory
			CFileMan * fileMan = CFileMan::NewL( rfs);
			CleanupStack::PushL( fileMan);
			
			//Copy file to work directory	
			User::LeaveIfError( fileMan->Copy( backupFile, name));
	
			CleanupStack::PopAndDestroy();  //fileMan		
		}	
	} while ( err == KErrNone);
	
	CleanupStack::PopAndDestroy(2); //RFs
}

// -----------------------------------------------------------------------------
// CRepositorySession::RemoveBackupL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RemoveBackupL()
{
	using namespace IniConstants;

	RFs rfs;
	User::LeaveIfError( rfs.Connect() );
	CleanupClosePushL( rfs);

	//Open directory and get file list
	RDir dir;
	CleanupClosePushL( dir);
	
	TInt err = dir.Open( rfs, KBackupPath, KEntryAttNormal);
	
	if ( err == KErrPathNotFound)
	{
		CreatePath();
		err = dir.Open( rfs, KBackupPath, KEntryAttNormal);
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
			
			TBuf<KPathLength> name;
			name.Append( KBackupPath);
			name.Append( entry.iName);	

			User::LeaveIfError( rfs.Delete( name));
		}		
	} while ( err == KErrNone );
	
	CleanupStack::PopAndDestroy(2); //RFs, RDir
}



// -----------------------------------------------------------------------------
// CRepositorySession::InitRepositorySessionL()
// -----------------------------------------------------------------------------
//

void CRepositorySession::InitRepositorySessionL()
{
	using namespace IniConstants;

	//Establish connection	
	User::LeaveIfError( iFs.Connect() );

	//Buffer for file name (without extension)
	iRepName.Zero();
	iRepName.NumFixedWidth( iRepositoryId.iUid, EHex, KUidLengthRep);

	//open repository file
	if ( KErrNone != ReadRAML( EFalse))
	{
		CreateAndReadROML();
	}
	

	//buffer for path and file name
	TBuf<KPathLength> repFileName;
	repFileName.Append( KDriveC);						//drive and path
	TInt pathLength = repFileName.Length();
	repFileName.Append( iRepName);						//file name 
	repFileName.Append( KRepositoryFileExtensionCre);	//and extension

	//create timer
	RTimer timer;
	User::LeaveIfError( timer.CreateLocal());
	
	//open or create RAM file for 	
	TInt counter = 0;
	TInt err = KErrNone;

	do
	{
		err = iFile.Open( iFs, repFileName, EFileShareExclusive|EFileRead|EFileWrite );	
		CheckRepositoryState( err);
		
 		if ( err != KErrNone)
 		{
 			iFile.Close();
 		
 			//or if file is reserved, wait a moment...
			TRequestStatus status;
			timer.After( status, KFileOpenPeriod);
			User::WaitForRequest( status);
		}
		
	} while ( counter++ < KFileOpenAttemps && ( err != KErrNone));
			
	//close timer
	timer.Close();
	
	User::LeaveIfError( err);
	

	//create backup file...
	if ( err == KErrNone)
	{
		MakeBackupL( iFile);
	}		

	//file is not opened
	User::LeaveIfError( err);
}

// -----------------------------------------------------------------------------
// CRepositorySession::ReadRAML()
// -----------------------------------------------------------------------------
//
void CRepositorySession::CheckRepositoryState( TInt& aFileOpenResult)
{
	if ( aFileOpenResult != KErrNone)
	{
		return;
	}

	//check is repository in use	
	CRepository* rep = 0;
	TRAPD( err, rep = CRepository::NewL( iRepositoryId));
	delete rep;
	
	if ( err == KErrNone )
	{
		iRepositoryInUse = ETrue; 
	}
}


// -----------------------------------------------------------------------------
// CRepositorySession::ReadRAML()
// -----------------------------------------------------------------------------
//
TInt CRepositorySession::ReadRAML( TBool aOnlyHeader)
{
	RDEBUG("CentRepTool: Read repository from RAM-drive");


	using namespace IniConstants;

	RFile file;

	//buffer for path and file name
	TBuf<KPathLength> repFileName;
	repFileName.Append( KDriveC);						//drive and path
	TInt pathLength = repFileName.Length();
	repFileName.Append( iRepName);						//file name 
	repFileName.Append( KRepositoryFileExtensionCre);	//and extension

	//Open file
	TInt err = file.Open( iFs, repFileName, EFileRead);
	RDEBUG_2("CentRepTool: Open repository - %S",  &repFileName);
	
	if ( err != KErrNone)
	{
		return err;
	}

	//Open file stream
	CDirectFileStore* directFileStore = CDirectFileStore::FromLC ( file);
	TStreamId rootStreamId = directFileStore->Root() ;
	
	//setup read stream
	RStoreReadStream rootStream ;
	rootStream.OpenLC( *directFileStore, rootStreamId);

	//Internalize the repository
	iRepContent = CRepositoryContent::NewL( iRepositoryId);
	iRepContent->ReadStreamL( rootStream, aOnlyHeader);

	CleanupStack::PopAndDestroy( 2, directFileStore);
	
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CRepositorySession::CreateROMFileL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::CreateAndReadROML()
{
	using namespace IniConstants;

	//read content from ROM
	iRepContent = ReadROMFileL( ETrue);

	__UHEAP_MARK;

	//buffer for path and file name
	TBuf<KPathLength> tmpFileName;
	tmpFileName.Append( KBackupPath);						//drive and path
	TInt pathLength = tmpFileName.Length();
	tmpFileName.Append( iRepName);						//file name 
	tmpFileName.Append( KRepositoryFileExtensionTmp);	//and extension
	
	RFile file;
	CleanupClosePushL( file);

	//Create RAM file. 
	User::LeaveIfError( file.Replace( iFs, tmpFileName, EFileShareExclusive|EFileRead|EFileWrite ));
	
	//Open file stream. RFile ownership changes for CDirectFileStore
	CDirectFileStore * directFileStore = CDirectFileStore::NewL( file);
	CleanupStack::Pop( &file);
	CleanupStack::PushL( directFileStore);

	//setup write stream
	directFileStore->SetTypeL( TUidType(KDirectFileStoreLayoutUid, KNullUid, KCentralRepositoryUID)) ; 
		
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*directFileStore) ;

	//Externalize .cre file....
	iRepContent->WriteStreamL( rootStream);
	rootStream.CommitL();

	directFileStore->SetRootL(rootStreamId);
	directFileStore->CommitL();

	//destroy
	CleanupStack::PopAndDestroy( 2, directFileStore);

	
	//copy file to work directory
	CFileMan * fileMan = CFileMan::NewL( iFs);
	CleanupStack::PushL( fileMan);
			
	//buffer for path and file name
	TBuf<KPathLength> repFileName;
	repFileName.Append( KDriveC);						//drive and path
	pathLength = repFileName.Length();
	repFileName.Append( iRepName);						//file name 
	repFileName.Append( KRepositoryFileExtensionCre);	//and extension

	//Copy file to work directory	
	User::LeaveIfError( fileMan->Copy( tmpFileName, repFileName));
	User::LeaveIfError( fileMan->Delete( tmpFileName));
	
	CleanupStack::PopAndDestroy( fileMan);  //fileMan

	__UHEAP_MARKEND;
}


// -----------------------------------------------------------------------------
// CRepositorySession::CommitRepositoryL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::CommitRepositoryL()
{
	RDEBUG("CentRepTool: CommitRepositoryL");


	using namespace IniConstants;

	//buffer for path and file name
	TBuf<KPathLength> tmpFileName;
	tmpFileName.Append( KBackupPath);						//drive and path
	tmpFileName.Append( KTempFile);						//file name 

	//Create RAM file
	RFile file;
	TInt err = file.Create( iFs, tmpFileName, EFileShareExclusive|EFileRead|EFileWrite );
	
	if ( err == KErrAlreadyExists)
	{
		User::LeaveIfError( file.Open( iFs, tmpFileName, EFileShareExclusive|EFileRead|EFileWrite ));
		User::LeaveIfError( file.SetSize(0));
	}
	
	//Open file stream
	CDirectFileStore * directFileStore = CDirectFileStore::NewLC( file);

	//setup write stream
	directFileStore->SetTypeL( TUidType(KDirectFileStoreLayoutUid, KNullUid, KCentralRepositoryUID)) ; 
		
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*directFileStore) ;

	//Externalize .cre file....
	iRepContent->WriteStreamL( rootStream);
	rootStream.CommitL();

	directFileStore->SetRootL(rootStreamId);
	directFileStore->CommitL();

	RDEBUG("CentRepTool: Repository streamed to temp-file");

	//destroy
	CleanupStack::PopAndDestroy( 2, directFileStore);

	//copy temp file to repository file
	User::LeaveIfError( file.Open( iFs, tmpFileName, EFileRead));
	CleanupClosePushL( file);
	
	TInt size = 0;
	User::LeaveIfError( file.Size( size));
	
	HBufC8 * buf = HBufC8::NewLC( size);
	TPtr8 ptr = buf->Des();
	User::LeaveIfError( file.Read( ptr));
	
	//seek to start of the file
	TInt seekOffset(0);

	User::LeaveIfError( iFile.Seek( ESeekStart, seekOffset));
	User::LeaveIfError( iFile.Write( *buf));
	User::LeaveIfError( iFile.SetSize( size));
	
	User::LeaveIfError( iFile.Flush());

	RDEBUG_2("CentRepTool: Persist repository file (.cre) committed from temp file (size: %d bytes)", size);


	//enforce central repository to re-read setting from .cre file...
	if ( iRepositoryInUse)
	{
		RDEBUG("CentRepTool: Repository is already used by central repository - ");
		
		//create repository session...
		CRepository* repository = CRepository::NewLC( iRepositoryId);
	
		//in next phase try to modifie CentRep setting. Because .cre file is locked by CRepositorySession
		//CentRep cannot make update and causes that repostiory goes to inconsistent state. Inconsistent
		//state ensures that new security settings are readed from drive before any other operation
		TInt errx = repository->Create( 0, 0);
		if ( errx == KErrAlreadyExists )
		{
			errx = repository->Set( 0, 0);
		}
	
		RDEBUG_2("CentRepTool: Enforce Central Repository file inconsistent state (CR state %d, must be != KErrNone)", errx);
		CleanupStack::PopAndDestroy( repository);
	}
	
	CleanupStack::PopAndDestroy( 2, &file);
}





// -----------------------------------------------------------------------------
// CRepositorySession::ReadFileL()
// -----------------------------------------------------------------------------
//
CRepositoryContent* CRepositorySession::ReadROMFileL( TBool aReadSettings)
{
	using namespace IniConstants;

	RDEBUG("CentRepTool: Read repository from ROM-drive");

	CRepositoryContent* retVal = 0;
	
	//buffer for path and file name
	TBuf<KPathLength> repFileName;
	repFileName.Append( KDriveZ);						//drive and path
	TInt pathLength = repFileName.Length();
	repFileName.Append( iRepName);				//file name 
	repFileName.Append( KRepositoryFileExtensionTxt);	//and extension

	//ROM file should always be ready to open
	RFile file;
	TInt err = file.Open( iFs, repFileName, EFileRead);	

	if ( err == KErrNone )
	{
		RDEBUG("CentRepTool: ROM repository found (.txt)");
		RDEBUG_2("CentRepTool: Open repository - %S",  &repFileName);

		CleanupClosePushL( file);

		//read file content and set iContentPtr
		TIniFileHelper ini;
		HBufC16 * content = ini.ReadFileL( file);
	
		retVal = CRepositoryContent::NewL( iRepositoryId, content);
		CleanupStack::PushL( retVal);
	
		//read sections from ini file
		retVal->ReadHeaderL();
		retVal->ReadOptionalDataL();
		retVal->ReadPlatSecL();
		
		if (aReadSettings)
		{
			retVal->ReadMainL();
		}
	
		CleanupStack::Pop( retVal);
		CleanupStack::PopAndDestroy( &file);
		
	}
	else
	{
		RDEBUG("CentRepTool: ROM repository not found (.txt), try .cre-repository");

		repFileName.Replace( repFileName.Length() - 4, 4, KRepositoryFileExtensionCre);
		User::LeaveIfError( file.Open( iFs, repFileName, EFileRead));
		
		RDEBUG_2("CentRepTool: Open repository - %S",  &repFileName);

		retVal = CRepositoryContent::NewL( iRepositoryId);
		CleanupStack::PushL( retVal);

		//Open file stream
		CDirectFileStore* directFileStore = CDirectFileStore::FromLC ( file);
		TStreamId rootStreamId = directFileStore->Root() ;
	
		//setup read stream
		RStoreReadStream rootStream ;
		rootStream.OpenLC( *directFileStore, rootStreamId);

		//Internalize the repository
		retVal->ReadStreamL( rootStream, !aReadSettings);

		CleanupStack::PopAndDestroy( 2, directFileStore);
		CleanupStack::Pop( retVal);
	
	}
	
	RDEBUG("CentRepTool: Repository read successfully");
	
	return retVal;
}





// -----------------------------------------------------------------------------
// CRepositorySession::MakeBackupL()
// -----------------------------------------------------------------------------
//

void CRepositorySession::MakeBackupL( RFile& aFile)
{
	using namespace IniConstants;

	//Because file is already opened, CFileMan cannot be used!
	//seek to start of the file
	TInt seekOffset(0);
	aFile.Seek( ESeekStart, seekOffset);
	
	TInt size;
	User::LeaveIfError( aFile.Size(size));

	//create buffer for content
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 ptr = buf->Des();
	
	//read file
	aFile.Read( ptr);

	
	//backup file name
	TBuf<KPathLength> backupFile;
	iFs.PrivatePath( backupFile);
	backupFile.Append( iRepName);							//file name 
	backupFile.Append( KRepositoryFileExtensionCre);		//and extension
	
	//create backup file
	RFile backup;
	CleanupClosePushL( backup);
	
	TInt err = backup.Replace( iFs, backupFile, EFileWrite);
	if ( err == KErrPathNotFound)
	{
		err = CreatePath();
	}

	User::LeaveIfError( err);	
	
	//write to file
	User::LeaveIfError( backup.Write( ptr));
	
	//ensures that if file close fails, error is detected
	User::LeaveIfError( backup.Flush());
	
	CleanupStack::PopAndDestroy(2);  //backup, HBufC	
	
}

// -----------------------------------------------------------------------------
// CRepositorySession::CreatePath()
// -----------------------------------------------------------------------------
//
TInt CRepositorySession::CreatePath()
{
	//create private path
	
	RFs rfs;
	TInt err = rfs.Connect();
	
	if ( err == KErrNone )
	{
		err = rfs.CreatePrivatePath( EDriveC);
	}
	
	rfs.Close();
	
	return err;
}





// -----------------------------------------------------------------------------
// CRepositorySession::SetSecurityIdForSetting()
// -----------------------------------------------------------------------------
//

void CRepositorySession::SetSecurityIdForSettingL( const RMessage2& aMessage)
{
	__UHEAP_MARK;

	TIniFileHelper ini;

	//read setting id from RMessage
	TUint32 settingUid;
	TPckg<TUint32> settingUidPack( settingUid);
	aMessage.ReadL(0, settingUidPack);
	
	//read SID from RMessage
	TUid applicationUid;
	TPckg<TUid> applicationUidPack( applicationUid);
	aMessage.ReadL(1, applicationUidPack);

	CIndividualSetting * setting = iRepContent->FindIndividualSetting( settingUid);
	
	if ( !setting )
	{
		User::Leave( KErrNotFound);
	}

	User::LeaveIfError( setting->AddSid( applicationUid));

	
	__UHEAP_MARKEND;
}



// -----------------------------------------------------------------------------
// CRepositorySession::SetSecurityIdForRange()
// -----------------------------------------------------------------------------
//

void CRepositorySession::SetSecurityIdForRangeL( const RMessage2& aMessage)
{
	//read range start from RMessage
	TUint32 rangeStart;
	TPckg<TUint32> rangeStartPack( rangeStart);
	aMessage.ReadL(0, rangeStartPack);

	//read range end from RMessage
	TUint32 rangeEnd;
	TPckg<TUint32> rangeEndPack( rangeEnd);
	aMessage.ReadL(1, rangeEndPack);
	
	//read SID from RMessage
	TUid applicationUid;
	TPckg<TUid> applicationUidPack( applicationUid);
	aMessage.ReadL(2, applicationUidPack);


	//Find range setting
	RPointerArray<CRangeSetting> settings;
	CleanupClosePushL( settings);
	iRepContent->CreateRangeSettingsL( settings, rangeStart, rangeEnd);
	
	//Add SIDs for setting inside of the range
	for ( TInt i(0); i < settings.Count(); i++)
	{
		User::LeaveIfError( settings[i]->AddSid( applicationUid));
	}
	
	
	CleanupStack::PopAndDestroy( &settings);
}

// -----------------------------------------------------------------------------
// CRepositorySession::CleanSecurityIdForSetting()
// -----------------------------------------------------------------------------
//

void CRepositorySession::RestoreSettingL( const RMessage2& aMessage)
{
	using namespace IniConstants;

	__UHEAP_MARK;

	TIniFileHelper ini;

	//read setting id from RMessage
	TUint32 settingUid;
	TPckg<TUint32> settingUidPack( settingUid);
	aMessage.ReadL(0, settingUidPack);
	
	CIndividualSetting * setting = iRepContent->FindIndividualSetting( settingUid);
	
	if ( !setting )
	{
		User::Leave( KErrNotFound);
	}
	
	

	User::LeaveIfError( setting->RemoveSid());

	
	__UHEAP_MARKEND;
	
}



// -----------------------------------------------------------------------------
// CRepositorySession::WriteStreamL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::WriteStreamL( RWriteStream& aStream )
{
	RDEBUG("CentRepTool: Streaming");
	RDEBUG("CentRepTool: initialization data");

	//read initialization data
	aStream << IniConstants::KPersistsVersion;
	aStream << iUid;
	aStream << iOwner;

	//write single policies	
	TInt32 numElements = SingleSecuritySettingsCount();			//Count	
	RDEBUG_2("CentRepTool: Single policies (count %d)", numElements);
	aStream << numElements;	

	for ( TInt i = 0; i < numElements; i++)
	{
		if ( iIndividualSettings[i]->ContainsSecuritySettings())
		{
			iIndividualSettings[i]->ExternalizePlatSecL( aStream);		
		}
	}
		
	//read range policies
	numElements = iRangeSettings.Count();	
	aStream << numElements;	
	RDEBUG_2("CentRepTool: Range policies (count %d)", numElements);
			
	for ( TInt i = 0; i < iRangeSettings.Count(); i++)
	{
		aStream << *(iRangeSettings[i]);
	}

	RDEBUG("CentRepTool: Default policies");

	//read default policies (read and write)...	
	aStream << (*iDefaultSetting);

	RDEBUG("CentRepTool: Default meta data");
	//read default meta	
	aStream << iDefaultMeta;

	//read range meta data
	numElements = iRangeMetas.Count();	
	RDEBUG_2("CentRepTool: Range metadata (count %d)", numElements);
	aStream << numElements;	
			
	for ( TInt i = 0; i < iRangeMetas.Count(); i++)
	{
		aStream << *(iRangeMetas[i]);
	}

	//read time stamp
	aStream << iTimeStamp.Int64();
	RDEBUG_2("CentRepTool: Timestamp %d", iTimeStamp.Int64());

	//read setting values
	numElements = iIndividualSettings.Count();	
	aStream << numElements;	
	RDEBUG_2("CentRepTool: Individual settings (count %d)", numElements);

	for ( TInt i = 0; i < iIndividualSettings.Count(); i++)
	{
		iIndividualSettings[i]->ExternalizeDataL( aStream);
	}	
	
	RDEBUG("CentRepTool: Streaming finished");
}


// -----------------------------------------------------------------------------
// CRepositoryContent::SingleSecurituSettingsCount()
// -----------------------------------------------------------------------------
//
TInt CRepositoryContent::SingleSecuritySettingsCount()
{
	return iSingleSecuritySettingCount;
}

// -----------------------------------------------------------------------------
// CRepositoryContent::ReadStreamL()
// -----------------------------------------------------------------------------
//
void CRepositoryContent::ReadStreamL( RReadStream& aStream, TBool aOnlyHeader)
{
	RDEBUG("CentRepTool: Streaming");
	RDEBUG("CentRepTool: initialization data");

	//read initialization data
	TUint8 version;
	aStream >> version;
	aStream >> iUid;
	aStream >> iOwner;

	//read single policies	
	TUint32 countUint32;
	aStream >> countUint32;	

	RDEBUG_2("CentRepTool: Single policies (count %d)", countUint32);

	for ( TInt i = 0; i < countUint32; i++)
	{
		CIndividualSetting* singlePolicy = new(ELeave) CIndividualSetting();
		singlePolicy->InternalizePlatSecL( aStream);
		AddIndividualSettingL( singlePolicy);
	}
		
	//read range policies
	TInt32 countInt32;
	aStream >> countInt32;	
			
	RDEBUG_2("CentRepTool: Range policies (count %d)", countInt32);

	for ( TInt i = 0; i < countInt32; i++)
	{
		CRangeSetting* rangePolicy = new(ELeave) CRangeSetting();
		aStream >> *rangePolicy;
		User::LeaveIfError( iRangeSettings.Append( rangePolicy));
	}
		

	//read default policies (read and write)...
	RDEBUG("CentRepTool: Default policies");
	aStream >> (*iDefaultSetting);

	//read default meta	
	RDEBUG("CentRepTool: Default meta data");
	aStream >> iDefaultMeta;

	//read range meta data
	aStream >> countInt32;	
	RDEBUG_2("CentRepTool: Range metadata (count %d)", countInt32);
	
	for ( TInt i = 0; i < countInt32; i++)
	{
		CRangeMeta* rangeMeta = new(ELeave) CRangeMeta();
		aStream >> (*rangeMeta);
		User::LeaveIfError( iRangeMetas.Append( rangeMeta));
	}
	
	//read time stamp
	TInt64 timeStampInt ;
	aStream >> timeStampInt ;
	iTimeStamp = timeStampInt ;
	RDEBUG_2("CentRepTool: Timestamp %d", iTimeStamp.Int64());

	if ( !aOnlyHeader)
	{
		//read setting values
		aStream >> countInt32;	
		RDEBUG_2("CentRepTool: Individual settings (count %d)", countInt32);
	
		for ( TInt i = 0; i < countInt32; i++)
		{
			//find setting id from stream and check is there already setting for that id
			TUint32 settingId = 0;
			aStream >> settingId;
			CIndividualSetting* setting = FindSettingOrCreateL( settingId);
		
			//get setting data
			setting->InternalizeDataL( aStream);
		}
	}

	RDEBUG("CentRepTool: Streaming finished");
}




// -----------------------------------------------------------------------------
// CRepositorySession::CleanSecurityIdForRange()
// -----------------------------------------------------------------------------
//

void CRepositorySession::RestoreRangeL( const RMessage2& aMessage)
{

	//read range start from RMessage
	TUint32 rangeStart;
	TPckg<TUint32> rangeStartPack( rangeStart);
	aMessage.ReadL(0, rangeStartPack);

	//read range end from RMessage
	TUint32 rangeEnd;
	TPckg<TUint32> rangeEndPack( rangeEnd);
	aMessage.ReadL(1, rangeEndPack);
	
	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);

	for ( TInt i(0); i < iRepContent->iRangeSettings.Count(); i++)
	{
		CRangeSetting * setting = iRepContent->iRangeSettings[i];
		if ( setting->Type() != ERangeSetting )
		{
			continue;
		}
		
		if ( (setting->iStart > rangeStart && setting->iStart < rangeEnd ) ||
			 (setting->iEnd > rangeStart && setting->iEnd < rangeEnd ))
		{
			delete setting;
			iRepContent->iRangeSettings.Remove( i--);
		}
	}
			 
	for ( TInt i(0); i < romContent->iRangeSettings.Count(); i++)
	{
		CRangeSetting * setting = romContent->iRangeSettings[i];
		if ( setting->Type() != ERangeSetting )
		{
			continue;
		}
		
		if ( (setting->iStart > rangeStart && setting->iStart < rangeEnd ) ||
			 (setting->iEnd > rangeStart && setting->iEnd < rangeEnd ))
		{
			iRepContent->iRangeSettings.AppendL( setting);
			romContent->iRangeSettings.Remove( i--);
		}
	}
		
	CleanupStack::PopAndDestroy( romContent);	
}



// -----------------------------------------------------------------------------
// CRepositorySession::AddSidForDefaultsL()
// -----------------------------------------------------------------------------
//

void CRepositorySession::AddSidForDefaultsL( const RMessage2& aMessage)
{
	using namespace IniConstants;

	TIniFileHelper ini;

	//read SID from RMessage
	TUid applicationUid;
	TPckg<TUid> applicationUidPack( applicationUid);
	aMessage.ReadL(0, applicationUidPack);


	if ( iRepContent->iDefaultSetting )
	{
		iRepContent->iDefaultSetting->AddSid( applicationUid);
	}

	for ( TInt i(0); i < iRepContent->iRangeSettings.Count(); i++)
	{
		iRepContent->iRangeSettings[i]->AddSid( applicationUid);
	}

/*	for ( TInt i(0); i < iIndividualSettings.Count(); i++)
	{
		iIndividualSettings[i]->AddSID( applicationUid);
	}
*/		
}

// -----------------------------------------------------------------------------
// CRepositorySession::RestoreDefaultsL()
// -----------------------------------------------------------------------------
//


void CRepositorySession::RestoreDefaultsL( const RMessage2& /*aMessage*/)
{
	using namespace IniConstants;

	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);
	
	//delete RAM policies	
	delete iRepContent->iDefaultSetting;
	iRepContent->iRangeSettings.ResetAndDestroy();
	
	//..and append new policies from rom (default, range and mask policies)
	iRepContent->iDefaultSetting = romContent->iDefaultSetting;
	romContent->iDefaultSetting = 0;

	for ( TInt i(0); i < romContent->iRangeSettings.Count(); i++)
	{
		CRangeSetting * setting = romContent->iRangeSettings[i];
		iRepContent->iRangeSettings.AppendL( setting);
		romContent->iRangeSettings.Remove( i--);
	}
			 

		
	CleanupStack::PopAndDestroy( romContent);
	
}

// -----------------------------------------------------------------------------
// CRepositorySession::SetSecurityIdForMaskL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::SetSecurityIdForMaskL( const RMessage2& aMessage )
	{
	RDEBUG("CRepositorySession::SetSecurityIdForMaskL() ... ");
	
	//read range start from RMessage
	TUint32 compareValue;
	TPckg<TUint32> compareValuePack( compareValue );
	aMessage.ReadL(0, compareValuePack );

	//read range end from RMessage
	TUint32 mask;
	TPckg<TUint32> maskPack( mask );
	aMessage.ReadL( 1, maskPack );
	
	//read SID from RMessage
	TUid applicationUid;
	TPckg<TUid> applicationUidPack( applicationUid );
	aMessage.ReadL( 2, applicationUidPack );

	//Find range setting
	RPointerArray<CRangeSetting> settings;
	CleanupClosePushL( settings );
	iRepContent->CreateMaskSettingsL( settings, compareValue, mask );
	
	if( settings.Count() == 0 )
		{
		RDEBUG("	**** settings count was 0 !");
		}
	
	//Add SIDs for setting inside of the range
	for ( TInt i( 0 ); i < settings.Count(); i++ )
		{
		RDEBUG_3("	Adding sid: %d/%d", i, settings.Count() );
		User::LeaveIfError( settings[ i ]->AddSid( applicationUid ) );
		}
	
	CleanupStack::PopAndDestroy( &settings );	
	RDEBUG("CRepositorySession::SetSecurityIdForMaskL() ... DONE!");
	}


// -----------------------------------------------------------------------------
// CRepositorySession::RestoreMaskL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RestoreMaskL( const RMessage2& aMessage)
{
	__UHEAP_MARK;

	//read range start from RMessage
	TUint32 compareValue;
	TPckg<TUint32> compareValuePack( compareValue);
	aMessage.ReadL(0, compareValuePack);

	//read range end from RMessage
	TUint32 mask;
	TPckg<TUint32> maskPack( mask);
	aMessage.ReadL(1, maskPack);
	
	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);

	//remove old mask settings
	for ( TInt i(0); i < iRepContent->iRangeSettings.Count(); i++)
	{
		CRangeSetting * setting = iRepContent->iRangeSettings[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iMask == mask && setting->iStart == compareValue  )
		{
			delete setting;
			iRepContent->iRangeSettings.Remove( i--);
		}
	}
			 
	//restore original settings from rom..
	for ( TInt i(0); i < romContent->iRangeSettings.Count(); i++)
	{
		CRangeSetting * setting = romContent->iRangeSettings[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iMask == mask && setting->iStart == compareValue  )
		{
			iRepContent->iRangeSettings.AppendL( setting);
			romContent->iRangeSettings.Remove( i--);
		}
	}
		
	CleanupStack::PopAndDestroy( romContent);	
	
	__UHEAP_MARKEND;
	
	

}

// -----------------------------------------------------------------------------
// CRepositorySession::RemoveBackupForMaskL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RemoveBackupForMaskL( const RMessage2& aMessage)
{
	//read range start from RMessage
	TUint32 compareValue;
	TPckg<TUint32> compareValuePack( compareValue);
	aMessage.ReadL(0, compareValuePack);

	//read range end from RMessage
	TUint32 mask;
	TPckg<TUint32> maskPack( mask);
	aMessage.ReadL(1, maskPack);
	
	//Find range setting
	RPointerArray<CRangeMeta> settings;
	CleanupClosePushL( settings);
	iRepContent->CreateMaskBackupL( settings, compareValue, mask);
	
	//Add SIDs for setting inside of the range
	for ( TInt i(0); i < settings.Count(); i++)
	{
		//turn backup bit OFF.
		TUint32& meta( settings[i]->iMeta);	
		meta = meta & (~KBackupBitMask);
	}
	
	
	CleanupStack::PopAndDestroy( &settings);	
}

// -----------------------------------------------------------------------------
// CRepositorySession::RestoreMaskBackupL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RestoreMaskBackupL( const RMessage2& aMessage)
{
	__UHEAP_MARK;

	//read range start from RMessage
	TUint32 compareValue;
	TPckg<TUint32> compareValuePack( compareValue);
	aMessage.ReadL(0, compareValuePack);

	//read range end from RMessage
	TUint32 mask;
	TPckg<TUint32> maskPack( mask);
	aMessage.ReadL(1, maskPack);
	
	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);

	//remove old mask settings
	for ( TInt i(0); i < iRepContent->iRangeMetas.Count(); i++)
	{
		CRangeMeta * setting = iRepContent->iRangeMetas[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iMask == mask && setting->iStart == compareValue  )
		{
			delete setting;
			iRepContent->iRangeMetas.Remove( i--);
		}
	}
			 
	//restore original settings from rom..
	for ( TInt i(0); i < romContent->iRangeMetas.Count(); i++)
	{
		CRangeMeta * setting = romContent->iRangeMetas[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iMask == mask && setting->iStart == compareValue  )
		{
			iRepContent->iRangeMetas.AppendL( setting);
			romContent->iRangeMetas.Remove( i--);
		}
	}
		
	CleanupStack::PopAndDestroy( romContent);	
	
	__UHEAP_MARKEND;	
}


void CRepositorySession::RemoveBackupForRangeL( const RMessage2& aMessage)
{
	//read range start from RMessage
	TUint32 startValue;
	TPckg<TUint32> startPack( startValue);
	aMessage.ReadL(0, startPack);

	//read range end from RMessage
	TUint32 endValue;
	TPckg<TUint32> endPack( endValue);
	aMessage.ReadL(1, endPack);
	
	//Find range setting
	RPointerArray<CRangeMeta> settings;
	CleanupClosePushL( settings);
	iRepContent->CreateRangeBackupL( settings, startValue, endValue);
	
	//Add SIDs for setting inside of the range
	for ( TInt i(0); i < settings.Count(); i++)
	{
		//turn backup bit OFF.
		TUint32& meta( settings[i]->iMeta);	
		meta = meta & (~KBackupBitMask);
	}
	
	
	CleanupStack::PopAndDestroy( &settings);	
}

void CRepositorySession::RestoreRangeBackupL( const RMessage2& aMessage)
{
	__UHEAP_MARK;

	//read range start from RMessage
	TUint32 startValue;
	TPckg<TUint32> startPack( startValue);
	aMessage.ReadL(0, startPack);

	//read range end from RMessage
	TUint32 endValue;
	TPckg<TUint32> endPack( endValue);
	aMessage.ReadL(1, endPack);
	
	
	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);

	//remove old mask settings
	for ( TInt i(0); i < iRepContent->iRangeMetas.Count(); i++)
	{
		CRangeMeta * setting = iRepContent->iRangeMetas[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iStart == startValue && setting->iEnd == endValue  )
		{
			delete setting;
			iRepContent->iRangeMetas.Remove( i--);
		}
	}
			 
	//restore original settings from rom..
	for ( TInt i(0); i < romContent->iRangeMetas.Count(); i++)
	{
		CRangeMeta * setting = romContent->iRangeMetas[i];
		if ( setting->Type() != EMaskSetting )
		{
			continue;
		}
		
		if ( setting->iStart == startValue && setting->iEnd == endValue  )
		{
			iRepContent->iRangeMetas.AppendL( setting);
			romContent->iRangeMetas.Remove( i--);
		}
	}
		
	CleanupStack::PopAndDestroy( romContent);	
	
	__UHEAP_MARKEND;	
}

// -----------------------------------------------------------------------------
// CRepositorySession::RemoveDefaultBackup()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RemoveDefaultBackup()
{
	using namespace IniConstants;

	TIniFileHelper ini;

	//turn backup bit OFF.
	TUint32& meta( iRepContent->iDefaultMeta);	
	meta = meta & (~KBackupBitMask);


	for ( TInt i(0); i < iRepContent->iRangeMetas.Count(); i++)
	{
		//turn backup bit OFF.
		TUint32& meta( iRepContent->iRangeMetas[i]->iMeta);	
		meta = meta & (~KBackupBitMask);

	}	
}

// -----------------------------------------------------------------------------
// CRepositorySession::RestoreDefaultBackupL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::RestoreDefaultBackupL()
{
	using namespace IniConstants;

	CRepositoryContent* romContent = ReadROMFileL( EFalse);
	CleanupStack::PushL( romContent);
	
	//delete RAM policies	
	iRepContent->iDefaultMeta = romContent->iDefaultMeta;
	
	for ( TInt i(0); i < romContent->iRangeMetas.Count(); i++)
	{
		CRangeMeta * setting = romContent->iRangeMetas[i];
		iRepContent->iRangeMetas.AppendL( setting);
		romContent->iRangeMetas.Remove( i--);
	}
			 
	CleanupStack::PopAndDestroy( romContent);
}

// -----------------------------------------------------------------------------
// CRepositorySession::RestoreDefaultBackupL()
// -----------------------------------------------------------------------------
//
void CRepositorySession::CheckAccessL( const RMessage2& aMessage)
{
	using namespace IniConstants;

	TUint32 settingid;
	TPckg<TUint32> idPack( settingid);
	aMessage.ReadL(0, idPack);

	TAccessType type;	
	TPckg<TAccessType> atPack( type);
	aMessage.ReadL(1, atPack);

	//Establish connection	
	TInt err = iFs.Connect();
	if( err != KErrNone )
		{
		RDEBUG_2("**** CRepositorySession::CheckAccess - failed to connect to RFs: %d", err );
		}

	//delete old repositories
	delete iRepContent;
	iRepContent = 0;


	//Buffer for file name (without extension)
	iRepName.Zero();
	iRepName.NumFixedWidth( iRepositoryId.iUid, EHex, KUidLengthRep);

	//open repository file (only headers)
	if ( KErrNone != ReadRAML( ETrue))
	{
		iRepContent = ReadROMFileL( EFalse);
	}

	TBool retVal = iRepContent->CheckAccess( aMessage, settingid, type);		
	
	TPckg<TBool> rvPack( retVal);
	aMessage.WriteL(2, rvPack);
}
