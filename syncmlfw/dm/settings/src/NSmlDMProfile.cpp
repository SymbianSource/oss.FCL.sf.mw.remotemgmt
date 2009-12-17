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



// INCLUDE FILES
#include <sysutil.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldmconstants.h>

#include "nsmldmsettings.h"


//------------------------------------------------------------------------------
//
//		CNSmlDMProfile
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//		CNSmlDMProfile::NewL()
//		Creates a new instance of CNSmlDMProfile object. 
//------------------------------------------------------------------------------

CNSmlDMProfile* CNSmlDMProfile::NewL( RDbNamedDatabase* aDatabase )
	{
	CNSmlDMProfile* self = CNSmlDMProfile::NewLC( aDatabase );
	CleanupStack::Pop();
	return self;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::NewLC()
//		Creates a new instance of CNSmlDMProfile object. 
//		Pushes and leaves new instance into CleanupStack.
//------------------------------------------------------------------------------
	
CNSmlDMProfile* CNSmlDMProfile::NewLC( RDbNamedDatabase* aDatabase )
	{
	CNSmlDMProfile* self = new( ELeave ) CNSmlDMProfile;
	CleanupStack::PushL( self );
	self->ConstructL( aDatabase );
	return self;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::ConstructL()
//		Second phase constructor.
//------------------------------------------------------------------------------

void CNSmlDMProfile::ConstructL( RDbNamedDatabase* aDatabase )
	{
	iDatabase = aDatabase;
	iId = ( TUint ) KNSmlNewObject;

	User::LeaveIfError( iTableProfiles.Open( *iDatabase, KNSmlDMTableProfiles ) );
    iColSetProfiles = iTableProfiles.ColSetL();
	
	iDisplayName = HBufC::NewL( KNSmlDmMaxProfileNameLength );
	iDMServerUsername = HBufC::NewL( KNSmlDmMaxUsernameLength );
	iDMServerPassword = HBufC::NewL( KNSmlDmMaxPasswordLength );
	iClientPassword   = HBufC::NewL( KNSmlDmSharedSecretMaxLength );
	iServerId         = HBufC::NewL( KNSmlDmServerIdMaxLength );
	iServerURL        = HBufC::NewL( KNSmlDmMaxURLLength );
	iHttpAuthUsername = HBufC::NewL( KNSmlDmMaxHttpAuthUsernameLength );
	iHttpAuthPassword = HBufC::NewL( KNSmlDmMaxHttpAuthPasswordLength );
	iProfileLock = EFalse;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::~CNSmlDMProfile()
//		Destructor.
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMProfile::~CNSmlDMProfile()
	{
	delete iDisplayName;
	delete iDMServerUsername;
	delete iDMServerPassword;
	delete iClientPassword;
	delete iServerId;
	delete iServerURL;
	delete iHttpAuthUsername;
	delete iHttpAuthPassword;

	iView.Close();
	iWSView.Close();
	iRSView.Close();
	
	if(iColSetProfiles)
	    {
	    delete iColSetProfiles;
	    }
	    

	iTableProfiles.Close();
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::SaveL()
//		Saves a profile ie. writes all profile data to the database.
//		If save is succesfull returns KErrNone,
//		otherwise returns negative integer (defined in EPOC error codes).
//------------------------------------------------------------------------------

EXPORT_C TInt CNSmlDMProfile::SaveL()
	{
	_DBG_FILE("CNSmlDMProfile::SaveL(): begin");

	if ( iDatabase->InTransaction() )
		{
		return ( KErrAccessDenied );
		}	

	iDatabase->Begin();
	CNSmlDMCrypt* crypt = new(ELeave) CNSmlDMCrypt;
	CleanupStack::PushL(crypt);

	if ( iId == ( TUint ) KNSmlNewObject )
		{
		// Check OOD before saving a New profile
		RFs fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL(fs);

		if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs, KNSmlDmSettingsFatMinSize))
			{
			iDatabase->Rollback();
			CleanupStack::PopAndDestroy(2); // crypt, fs
			return KErrDiskFull;
			// do not write
			}
		CleanupStack::PopAndDestroy(); //fs

		//new profile
		iTableProfiles.InsertL();
		
		TableProfilesSetColDesL( KNSmlDMProfileDisplayName,	*iDisplayName );
		TableProfilesSetColIntL( KNSmlDMProfileIAPId, iIAPId );
		TableProfilesSetColIntL( KNSmlDMProfileTransportId, iTransportId );

		TableProfilesSetColDesL( KNSmlDMProfileServerUsername, *iDMServerUsername );
		TableProfilesSetColDesL( KNSmlDMProfileServerPassword, crypt->EncryptedL( *iDMServerPassword) );	
		TableProfilesSetColDesL( KNSmlDMProfileClientPassword,            crypt->EncryptedL( *iClientPassword) );	

		TableProfilesSetColDesL( KNSmlDMProfileServerURL, *iServerURL );
		TableProfilesSetColDesL( KNSmlDMProfileServerId,   *iServerId );

		TableProfilesSetColIntL( KNSmlDMProfileServerAlertedAction,		iServerAlertAction );
		TableProfilesSetColIntL( KNSmlDMProfileAuthenticationRequired, iAuthenticationRequired );
		TableProfilesSetColIntL( KNSmlDMProfileCreatorId,				iCreatorId );
		TableProfilesSetColIntL( KNSmlDMProfileDeleteAllowed,			iDeleteAllowed );
		TableProfilesSetColIntL( KNSmlDMProfileSessionId,				iDMSessionId );
		TableProfilesSetColIntL( KNSmlDMProfileHidden, iProfileHidden );

		TableProfilesSetColIntL( KNSmlDMProfileHttpAuthUsed, iHttpAuthUsed );
		TableProfilesSetColDesL( KNSmlDMProfileHttpAuthUsername, *iHttpAuthUsername );
		TableProfilesSetColDesL( KNSmlDMProfileHttpAuthPassword, crypt->EncryptedL( *iHttpAuthPassword ) );
		TableProfilesSetColIntL(KNSmlDMProfileLock,iProfileLock);
			
		iId = iTableProfiles.ColUint( iColSetProfiles->ColNo( KNSmlDMProfileId) ); // Use autoincrement
		iId+= KMaxDataSyncID;
		iTableProfiles.SetColL( iColSetProfiles->ColNo( KNSmlDMProfileId ), iId );
				
		iTableProfiles.PutL();
		}
	else
		{
		//existing profile
		HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
		TPtr sqlStatementPtr = sqlStatement->Des();

		sqlStatementPtr.Format( KDMSQLGetProfile, iId );

		PrepareViewL( sqlStatementPtr, iView.EUpdatable );

		CleanupStack::PopAndDestroy();  // sqlStatement

		if ( iView.FirstL() )
			{
			iView.GetL();
			iView.UpdateL();

			ViewSetColDesL( KNSmlDMProfileDisplayName, *iDisplayName );
			ViewSetColIntL( KNSmlDMProfileTransportId, iTransportId );
			ViewSetColIntL( KNSmlDMProfileIAPId, iIAPId );

			ViewSetColDesL( KNSmlDMProfileServerUsername, *iDMServerUsername );
			ViewSetColDesL( KNSmlDMProfileServerPassword, crypt->EncryptedL( *iDMServerPassword ) );
			ViewSetColDesL( KNSmlDMProfileClientPassword,            crypt->EncryptedL( *iClientPassword) );	

			ViewSetColDesL( KNSmlDMProfileServerURL, *iServerURL );
			ViewSetColDesL( KNSmlDMProfileServerId, *iServerId );

			ViewSetColIntL( KNSmlDMProfileServerAlertedAction,		iServerAlertAction );
			ViewSetColIntL( KNSmlDMProfileAuthenticationRequired, iAuthenticationRequired );
			ViewSetColIntL( KNSmlDMProfileCreatorId, iCreatorId );
			ViewSetColIntL( KNSmlDMProfileDeleteAllowed, iDeleteAllowed );
			ViewSetColIntL( KNSmlDMProfileSessionId, iDMSessionId);
			ViewSetColIntL( KNSmlDMProfileHidden, iProfileHidden );

			ViewSetColIntL( KNSmlDMProfileHttpAuthUsed, iHttpAuthUsed );
			ViewSetColDesL( KNSmlDMProfileHttpAuthUsername, *iHttpAuthUsername );
			ViewSetColDesL( KNSmlDMProfileHttpAuthPassword, crypt->EncryptedL( *iHttpAuthPassword ) );
			ViewSetColIntL( KNSmlDMProfileLock, iProfileLock );
			
			iView.PutL();
			}
		else
			{
			iDatabase->Rollback();
			CleanupStack::PopAndDestroy();  //crypt

			return KErrNotFound;
			}

		}
		 
	CleanupStack::PopAndDestroy();  //crypt
	CommitAndCompact();
	
	_DBG_FILE("CNSmlDMProfile::SaveL(): end");

	return KErrNone;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::IsNew()
//		Returns true if profile is new, otherwise false.
//------------------------------------------------------------------------------

EXPORT_C TBool CNSmlDMProfile::IsNew() const
	{
	return ( iId == ( TUint ) KNSmlNewObject );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::StrValue()
//		Returns a given string value.
//------------------------------------------------------------------------------

EXPORT_C const TDesC& CNSmlDMProfile::StrValue( TNSmlDMProfileData aProfileItem ) const
	{	
	HBufC* result = 0;
	switch ( aProfileItem )
		{
		case ( EDMProfileDisplayName ) :
			{
			result = iDisplayName;
			}
		break;

		case ( EDMProfileServerUsername ) :
			{
			result = iDMServerUsername;
			}
		break;

		case ( EDMProfileServerPassword ) :
			{
			result = iDMServerPassword;
			}
		break;

		case ( EDMProfileClientPassword ) :
			{
			result = iClientPassword;
			}
		break;
		
		case ( EDMProfileServerURL ) :
			{
			result = iServerURL;
			}
		break;
		
		case ( EDMProfileServerId ) :
			{
			result = iServerId;
			}
		break;
	
		case ( EDMProfileHttpAuthUsername ) :
			{
			result = iHttpAuthUsername;
			}
		break;
		
		case ( EDMProfileHttpAuthPassword ) :
			{
			result = iHttpAuthPassword;
			}
		break;

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return *result; 
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::IntValue()
//		Returns a given int value.
//------------------------------------------------------------------------------

EXPORT_C TInt CNSmlDMProfile::IntValue( TNSmlDMProfileData aProfileItem ) const
	{	
	TInt result( 0 );
	switch ( aProfileItem )
		{
		case ( EDMProfileId ) :
			{
			result = iId;
			}
		break;
		
		case ( EDMProfileIAPId ) :
			{
			result = iIAPId;
			}
		break;

		case ( EDMProfileTransportId ) :
			{
			result = iTransportId;
			}
		break;

		case ( EDMProfileSessionId ) :
			{
			result = iDMSessionId;
			}
		break;
		
		case ( EDMProfileServerAlertAction ) :
			{
			result = iServerAlertAction;
			}
		break;
		
		case ( EDMProfileAuthenticationRequired ) :
			{
			result = iAuthenticationRequired;
			}
		break;

		case ( EDMProfileCreatorId ) :
			{
			result = iCreatorId;
			}
		break;

		case ( EDMProfileDeleteAllowed ) :
			{
			result = iDeleteAllowed;
			}
		break;

		case ( EDMProfileHttpAuthUsed ) :
			{
			result = iHttpAuthUsed;
			}
		break;
		
		case ( EDMProfileHidden ) :
			{
			result = iProfileHidden;
			}
		break;
		case EDMProfileLock :
		    {
		    	result = iProfileLock;
		    }
		break;    
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return result;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::SetStrValue()
//		Sets a given string value.
//------------------------------------------------------------------------------

EXPORT_C void CNSmlDMProfile::SetStrValue( TNSmlDMProfileData aProfileItem, const TDesC& aNewValue )
	{
	switch ( aProfileItem )
		{
		case ( EDMProfileDisplayName ) :
			{
			*iDisplayName = aNewValue;
			}
		break;

		case ( EDMProfileServerUsername ) :
			{
			*iDMServerUsername = aNewValue;
			}
		break;

		case ( EDMProfileServerPassword ) :
			{
			*iDMServerPassword = aNewValue;
			}
		break;

		case ( EDMProfileClientPassword ) :
			{
			*iClientPassword = aNewValue;
			}
		break;
		
		case ( EDMProfileServerURL ) :
			{
			*iServerURL = aNewValue;
			}
		break;
		
		case ( EDMProfileServerId ) :
			{
			*iServerId = aNewValue;
			}
		break;

		case ( EDMProfileHttpAuthUsername ) :
			{
			*iHttpAuthUsername = aNewValue;
			}
		break;
		
		case ( EDMProfileHttpAuthPassword ) :
			{
			*iHttpAuthPassword = aNewValue;
			}
		break;
		
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}	

//------------------------------------------------------------------------------
//		CNSmlDMProfile::SetIntValue()
//		Sets a given int value.
//------------------------------------------------------------------------------

EXPORT_C void CNSmlDMProfile::SetIntValue( TNSmlDMProfileData aProfileItem, const TInt aNewValue )
	{
	switch ( aProfileItem )
		{
		case ( EDMProfileId ) :
			{
			iId = aNewValue;
			}
		break;

		case ( EDMProfileIAPId ) :
			{
			iIAPId = aNewValue;
			}
		break;

		case ( EDMProfileTransportId ) :
			{
			iTransportId = aNewValue;
			}
		break;

		case ( EDMProfileSessionId ) :
			{
			iDMSessionId = aNewValue;
			}
		break;
		
		case ( EDMProfileServerAlertAction ) :
			{
			iServerAlertAction = aNewValue;
			}
		break;
		
		case ( EDMProfileAuthenticationRequired ) :
			{
			iAuthenticationRequired = aNewValue;
			}
		break;
		
		case ( EDMProfileCreatorId ) :
			{
			iCreatorId = aNewValue;
			}
		break;
		
		case ( EDMProfileDeleteAllowed ) :
			{
			iDeleteAllowed = aNewValue;
			}
		break;
		case ( EDMProfileHidden ) :
			{
			iProfileHidden = aNewValue;
			}
		break;
		
		case ( EDMProfileHttpAuthUsed ) :
			{
			iHttpAuthUsed = aNewValue;
			}
		break;
		case ( EDMProfileLock ) :
			{
			iProfileLock = aNewValue;
			}
		break;	
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::EqualData()
//		Compares all this profile data to
//		given profiles's data.
//------------------------------------------------------------------------------

EXPORT_C TBool CNSmlDMProfile::EqualData( CNSmlDMProfile* aprofile ) const
	{
	if ( aprofile == NULL )
		{
		return EFalse;
		}

	if ( *iDisplayName != *aprofile->iDisplayName )
		{
		return EFalse;
		}

	if ( iTransportId != aprofile->iTransportId)
		{
		return EFalse;
		}

	if ( iIAPId != aprofile->iIAPId )
		{
		return EFalse;
		}

	if ( *iClientPassword != *aprofile->iClientPassword )
		{
		return EFalse;
		}

	if ( *iServerURL != *aprofile->iServerURL )
		{
		return EFalse;
		}

	if ( *iServerId != *aprofile->iServerId )
		{
		return EFalse;
		}

	if ( iDMSessionId != aprofile->iDMSessionId )
		{
		return EFalse;
		}

	if ( iServerAlertAction != aprofile->iServerAlertAction )
		{
		return EFalse;
		}

	if ( iAuthenticationRequired != aprofile->iAuthenticationRequired )
		{
		return EFalse;
		}

	if ( iCreatorId != aprofile->iCreatorId)
		{
		return EFalse;
		}

	if ( iDeleteAllowed != aprofile->iDeleteAllowed )
		{
		return EFalse;
		}
	
	if ( iHttpAuthUsed != aprofile->iHttpAuthUsed)
		{
		return EFalse;
		}
	
	if ( *iHttpAuthUsername != *aprofile->iHttpAuthUsername )
		{
		return EFalse;
		}
	
	if ( *iHttpAuthPassword != *aprofile->iHttpAuthPassword )
		{
		return EFalse;
		}
	if ( iProfileLock != aprofile->iProfileLock )
		{
		return EFalse;
		}	
		
	return ETrue;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::IsNullL()
//		Returns true if profile data is NULL, 
//      otherwise false.
//------------------------------------------------------------------------------

EXPORT_C TBool CNSmlDMProfile::IsNullL( const TDesC& aFieldName ) 
	{
	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	if ( iView.FirstL() )
		{
		iView.GetL();
		if ( iView.IsColNull( iColSetProfiles->ColNo( aFieldName ) ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::HasLogL()
//		Returns true if profile has Log-data, 
//      otherwise false.
//------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDMProfile::HasLogL() 
	{
	_DBG_FILE("CNSmlDMProfile::HasLogL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	if ( iView.FirstL() )
		{
		iView.GetL();
		if ( iView.IsColNull( iColSetProfiles->ColNo( KNSmlDMProfileLog ) ) )
			{
			return EFalse;
			}
		}

	_DBG_FILE("CNSmlDMProfile::HasLogL(): end");

	return ETrue;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::HasLastSyncL() 
//		Returns true if profile has Last sync data, 
//      otherwise false.
//------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDMProfile::HasLastSyncL() 
	{
	_DBG_FILE("CNSmlDMProfile::HasLogL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	if ( iView.FirstL() )
		{
		iView.GetL();
		if ( iView.IsColNull( iColSetProfiles->ColNo( KNSmlDMLastSync ) ) )
			{
			return EFalse;
			}
		}

	_DBG_FILE("CNSmlDMProfile::HasLogL(): end");

	return ETrue;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::LogReadStreamL()
//		Reads log-data from database
//------------------------------------------------------------------------------
EXPORT_C RReadStream& CNSmlDMProfile::LogReadStreamL()
	{
	_DBG_FILE("CNSmlDMProfile::LogReadStreamL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDMSQLGetProfile, iId);

	User::LeaveIfError(iRSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EReadOnly));
	User::LeaveIfError(iRSView.EvaluateAll());

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iRSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDMProfileLog()); // Ordinal position of long column
	delete colSet;
	colSet = NULL;
	
	// get row
	if( iRSView.FirstL() )
		{
		iRSView.GetL();
		iRs.OpenL(iRSView, col);
		}

	_DBG_FILE("CNSmlDMProfile::LogReadStreamL(): end");

	return iRs;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::LogWriteStreamL()
//      Opens stream to database for writing
//------------------------------------------------------------------------------

EXPORT_C RWriteStream& CNSmlDMProfile::LogWriteStreamL()
	{
	_DBG_FILE("CNSmlDMProfile::LogWriteStreamL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDMSQLGetProfile, iId);
	
	// create a view on the database
	User::LeaveIfError(iWSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EUpdatable));
	User::LeaveIfError(iWSView.EvaluateAll());

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iWSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDMProfileLog()); // Ordinal position of long column
	delete colSet;
	colSet = NULL;
	
	// get row
	iDatabase->Begin();	
	if( iWSView.FirstL() )
		{
		iWSView.GetL();
		iWSView.UpdateL();
		iWs.OpenL(iWSView, col);
		}

	_DBG_FILE("CNSmlDMProfile::LogWriteStreamL(): end");

	return iWs;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::WriteStreamCommitL()
//	
//		
//------------------------------------------------------------------------------
EXPORT_C void CNSmlDMProfile::WriteStreamCommitL()
	{
	_DBG_FILE("CNSmlDMProfile::WriteStreamCommitL(): begin");

	iWSView.PutL();
	iWSView.Close();
	CommitAndCompact();

	_DBG_FILE("CNSmlDMProfile::WriteStreamCommitL(): end");
	}


//------------------------------------------------------------------------------
//		CNSmlDMProfile::LastSyncReadStreamL()
//		Reads last sync-data from database
//------------------------------------------------------------------------------
EXPORT_C RReadStream& CNSmlDMProfile::LastSyncReadStreamL()
	{
	_DBG_FILE("CNSmlDMProfile::LastSyncReadStreamL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDMSQLGetProfile, iId);

	User::LeaveIfError(iRSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EReadOnly));
	User::LeaveIfError(iRSView.EvaluateAll());

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iRSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDMLastSync()); // Ordinal position of long column
	delete colSet;
	colSet = NULL;
	
	// get row
	if( iRSView.FirstL() )
		{
		iRSView.GetL();
		iRs.OpenL(iRSView, col);
		}

	_DBG_FILE("CNSmlDMProfile::LastSyncReadStreamL(): end");

	return iRs;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::LastSyncWriteStreamL()
//		writes last sync-data to database
//------------------------------------------------------------------------------
EXPORT_C RWriteStream& CNSmlDMProfile::LastSyncWriteStreamL()
	{
	_DBG_FILE("CNSmlDMProfile::LastSyncWriteStreamL(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDMSQLGetProfile, iId);
	
	// create a view on the database
	User::LeaveIfError(iWSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EUpdatable));
	User::LeaveIfError(iWSView.EvaluateAll());

	CleanupStack::PopAndDestroy();  // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iWSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDMLastSync()); // Ordinal position of long column
	delete colSet;
	colSet = NULL;
	
	// get row
	iDatabase->Begin();	
	if( iWSView.FirstL() )
		{
		iWSView.GetL();
		iWSView.UpdateL();
		iWs.OpenL(iWSView, col);
		}

	_DBG_FILE("CNSmlDMProfile::LastSyncWriteStreamL(): end");

	return iWs;

	}	

//------------------------------------------------------------------------------
//		CNSmlDMProfile::LastSyncStreamCommitL()

//------------------------------------------------------------------------------
EXPORT_C void CNSmlDMProfile::LastSyncStreamCommitL()
	{
	_DBG_FILE("CNSmlDMProfile::LastSyncStreamCommitL(): begin");

	iWSView.PutL();
	iWSView.Close();
	CommitAndCompact();

	_DBG_FILE("CNSmlDMProfile::LastSyncStreamCommitL(): end");
	}
	
//------------------------------------------------------------------------------
//		CNSmlDMProfile::PrepareViewL()
//		Closes and prepares the view
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess )
	{
	iView.Close();
	User::LeaveIfError( iView.Prepare( *iDatabase, TDbQuery( aSql ), aAccess ) );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::CommitAndCompact
//		Commits update and compacts the database
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::CommitAndCompact() 
	{
	iDatabase->Commit();
	iDatabase->Compact();
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::TableProfilesSetColIntL
//	
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::TableProfilesSetColIntL( const TDesC& aFieldName, TInt aValue)
	{
	iTableProfiles.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::TableProfilesSetColDesL
//	
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::TableProfilesSetColDesL( const TDesC& aFieldName, TDesC& aValue)
	{
	iTableProfiles.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::ViewSetColIntL
//	
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::ViewSetColIntL( const TDesC& aFieldName, TInt aValue)
	{
	iView.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::ViewSetColDesL
//	
//		
//------------------------------------------------------------------------------
void CNSmlDMProfile::ViewSetColDesL( const TDesC& aFieldName, TDesC& aValue)
	{
	iView.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::NonceLC()
//		Gets nonce.
//		If length of the returned string is 0,
//		nonce was not found.
//------------------------------------------------------------------------------

EXPORT_C HBufC* CNSmlDMProfile::NonceLC( TNSmlDMProfileData aProfileItem)
	{
	_DBG_FILE("CNSmlDMProfile::NonceLC(): begin");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + KNSmlDmMaxIntegerLength );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, iId );
	
	CNSmlDMProfile::PrepareViewL( sqlStatementPtr, iView.EReadOnly );

	CleanupStack::PopAndDestroy();  // sqlStatement

	if(iView.Unevaluated())
		{
		iView.EvaluateAll(); 
		}

	HBufC* stringValue = NULL;

	if ( iView.FirstL() )
		{
			iView.GetL();

			switch ( aProfileItem )
			{
			case ( EDMProfileServerNonce ) :
				{
				RDbColReadStream in;
				in.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ) );
				stringValue = HBufC::NewLC( iView.ColLength( iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ) ) );
				TPtr ptr = stringValue->Des();
				in.ReadL( ptr, iView.ColLength( iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ) ) );
				CleanupStack::Pop(); // stringValue
				CleanupStack::PopAndDestroy(); //in
				}
			break;

			case ( EDMProfileClientNonce ) :
				{
				RDbColReadStream in;
				in.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileClientNonce ) );
				stringValue = HBufC::NewLC( iView.ColLength( iColSetProfiles->ColNo( KNSmlDMProfileClientNonce ) ) );
				TPtr ptr = stringValue->Des();
				in.ReadL( ptr, iView.ColLength( iColSetProfiles->ColNo( KNSmlDMProfileClientNonce ) ) );
				CleanupStack::Pop(); // stringValue
				CleanupStack::PopAndDestroy(); //in
				}
			break;

			default:
				{
				User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
				}
			}

		}
	_DBG_FILE("CNSmlDMProfile::NonceLC(): end");

	CleanupStack::PushL( stringValue );
	return stringValue;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::SetNonceL()
//		Sets nonce.
//------------------------------------------------------------------------------

EXPORT_C void CNSmlDMProfile::SetNonceL( TNSmlDMProfileData aProfileItem, const TDesC& aNewValue )
	{	

	_DBG_FILE("CNSmlDMProfile::SetNonceL begins");

	HBufC* sqlStatement = HBufC::NewLC( KDMSQLGetProfile().Length() + 16 );
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDMSQLGetProfile, iId );
	
	PrepareViewL( sqlStatementPtr, iView.EUpdatable );

	CleanupStack::PopAndDestroy();  // sqlStatement
		
	iDatabase->Begin();
	RDbColWriteStream out;
	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		switch ( aProfileItem )
		{
		case ( EDMProfileServerNonce ) :
			{
			out.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ) );
			break;
			}
		case ( EDMProfileClientNonce ) :
			{
			out.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileClientNonce ) );
			break;
			}
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
		out.WriteL( aNewValue );
		out.Close();
		iView.PutL();
		}
	else
		{
		//new row
		PrepareViewL( KDMSQLGetProfile, iView.EUpdatable );
		iView.InsertL();
		switch ( aProfileItem )
		{
		case ( EDMProfileServerNonce ) :
			{
			out.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ) );
			break;
			}
		case ( EDMProfileClientNonce ) :
			{
			out.OpenLC( iView, iColSetProfiles->ColNo( KNSmlDMProfileClientNonce ) );
			break;
			}
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
		out.WriteL( aNewValue );
		out.Close();
		iView.SetColL( iColSetProfiles->ColNo( KNSmlDMProfileServerNonce ), aNewValue );		
		iView.PutL();
		}
	
	CommitAndCompact();
	CleanupStack::PopAndDestroy(); // out

	_DBG_FILE("CNSmlDMProfile::SetNonceL ends");

	}

//------------------------------------------------------------------------------
//		CNSmlDMProfile::ResetDatabaseL()
//	
//------------------------------------------------------------------------------
	

void CNSmlDMProfile::ResetDatabaseL( RDbNamedDatabase* aDatabase )
    {
       iDatabase=aDatabase;
      User::LeaveIfError( iTableProfiles.Open( *iDatabase, KNSmlDMTableProfiles ) );
      iColSetProfiles = iTableProfiles.ColSetL();
    }
    
//------------------------------------------------------------------------------
//		CNSmlDMProfile::ResetID()
//	
//------------------------------------------------------------------------------
   
void CNSmlDMProfile::ResetID()
    {
     iId = ( TUint ) KNSmlNewObject;
    }

//------------------------------------------------------------------------------
//		CNSmlDMProfile::CloseDbHandles()
//	
//------------------------------------------------------------------------------    
void CNSmlDMProfile::CloseDbHandles()
    {
    if(iColSetProfiles)
        {
        delete iColSetProfiles;
        }
    iColSetProfiles = NULL;
    iTableProfiles.Close();
   
    }
//  End of File
