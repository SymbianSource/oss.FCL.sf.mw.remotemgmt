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



// INCLUDE FILES
#include <sysutil.h>

#include <nsmlconstants.h>
#include <nsmldsconstants.h>
#include "nsmldssettings.h"


//=============================================
//
//		CNSmlDSProfile
//
//=============================================

//=============================================
//		CNSmlDSProfile::NewL()
//		Creates a new instance of CNSmlDSProfile object. 
//=============================================

CNSmlDSProfile* CNSmlDSProfile::NewL( RDbNamedDatabase* aDatabase )
	{
	CNSmlDSProfile* self = CNSmlDSProfile::NewLC( aDatabase );
	CleanupStack::Pop();
	return self;
	}

//=============================================
//		CNSmlDSProfile::NewLC()
//		Creates a new instance of CNSmlDSProfile object. 
//		Pushes and leaves new instance into CleanupStack.
//=============================================
	
CNSmlDSProfile* CNSmlDSProfile::NewLC( RDbNamedDatabase* aDatabase )
	{
	CNSmlDSProfile* self = new( ELeave ) CNSmlDSProfile;
	CleanupStack::PushL( self );
	self->ConstructL( aDatabase );
	return self;
	}

//=============================================
//		CNSmlDSProfile::ConstructL()
//		Second phase constructor.
//=============================================

void CNSmlDSProfile::ConstructL( RDbNamedDatabase* aDatabase )
	{
	iDatabase = aDatabase;
	iId = KNSmlNewObject;

	User::LeaveIfError( iTableProfiles.Open( *iDatabase, KNSmlTableProfiles ) );
    iColSetProfiles = iTableProfiles.ColSetL();
	
	User::LeaveIfError( iTableAdapters.Open( *iDatabase, KNSmlTableAdapters ) );	
	iColSetAdapters = iTableAdapters.ColSetL();
	
	iDisplayName = HBufC::NewL( KNSmlMaxProfileNameLength );
	iSyncServerUsername = HBufC::NewL( KNSmlMaxUsernameLength );
	iSyncServerPassword = HBufC::NewL( KNSmlMaxPasswordLength );
	iServerURL = HBufC::NewL( KNSmlMaxURLLength );
	iServerId = HBufC::NewL( KNSmlMaxServerIdLength );
	
	iHttpAuthUsername = HBufC::NewL( KNSmlMaxHttpAuthUsernameLength );
	iHttpAuthPassword = HBufC::NewL( KNSmlMaxHttpAuthPasswordLength );

	iContentTypes = new ( ELeave ) CArrayPtrFlat<CNSmlDSContentType>( KSmlArrayGranularity );
	}

//=============================================
//		CNSmlDSProfile::~CNSmlDSProfile()
//		Destructor.
//=============================================

EXPORT_C CNSmlDSProfile::~CNSmlDSProfile()
	{
	delete iDisplayName;
	delete iSyncServerUsername;
	delete iSyncServerPassword;
	delete iServerURL;
	delete iServerId;

	delete iHttpAuthUsername;
	delete iHttpAuthPassword;

	iView.Close();
	iRSView.Close();
	iWSView.Close();
	
	delete iColSetProfiles;
	delete iColSetAdapters;

	iTableAdapters.Close();
	iTableProfiles.Close();

	iContentTypes->ResetAndDestroy();
	delete iContentTypes;
	}

//=============================================
//		CNSmlDSProfile::AddContentTypeL()
//		Appends a new content type to iContentTypes -list,
//		if it doesn't exist before.
//=============================================

 EXPORT_C CNSmlDSContentType* CNSmlDSProfile::AddContentTypeL( TInt aImplementationUID, TDesC& aClientDataSource, TDesC& aServerDataSource )
	{
	if ( ContentType( aImplementationUID ) ) // Check if type already on the list
		{
		return NULL;
		}

	iAdapterId = KNSmlNewObject;

	iContentTypes->AppendL( CNSmlDSContentType::NewLC( this->iDatabase ) );
	CleanupStack::Pop();

	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterTableId,   iAdapterId);	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterProfileId, iId);	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterImplementationId, aImplementationUID);	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterEnabled, TRUE);	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterCreatorId, iCreatorId);	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetStrValue( EDSAdapterServerDataSource, aServerDataSource );	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetStrValue( EDSAdapterClientDataSource, aClientDataSource );	
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterFilterMatchType, ESyncMLMatchDisabled );	
	
	return iContentTypes->At( iContentTypes->Count() - 1 );
	}

//=============================================
//		CNSmlDSProfile::ContentType()
//		Returns a pointer to a CNSmlDSContentType 
//		object with given ImplementationUID.
//=============================================

EXPORT_C CNSmlDSContentType* CNSmlDSProfile::ContentType( TInt aImplementationUID ) const
	{
	for ( TInt i = 0; i < iContentTypes->Count(); i++ )
		{
		if ( iContentTypes->At( i )->IntValue( EDSAdapterImplementationId ) == aImplementationUID )
			{
			return iContentTypes->At( i );
			}
		}
		
	return NULL;
	}

//=============================================
//		CNSmlDSProfile::ContentTypeId()
//		Returns a pointer to a CNSmlDSContentType 
//		object with given content id.
//=============================================

EXPORT_C CNSmlDSContentType* CNSmlDSProfile::ContentTypeId( TInt aId ) const
	{
	for ( TInt i = 0; i < iContentTypes->Count(); i++ )
		{
		if ( iContentTypes->At( i )->IntValue( EDSAdapterTableId ) == aId )
			{
			return iContentTypes->At( i );
			}
		}
	return NULL;
	}

//=============================================
//		CNSmlDSProfile::DeleteContentTypeL()
//		Delete object with given task id.
//		
//=============================================

EXPORT_C TBool CNSmlDSProfile::DeleteContentTypeL( TInt aTaskId ) 
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetAdapterId().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	TInt err;

	for ( TInt i = 0; i < iContentTypes->Count(); i++ )
		{
		if ( iContentTypes->At( i )->IntValue( EDSAdapterTableId ) == aTaskId )
			{
//
//			Remove first from database
//
			iDatabase->Begin();

			sqlStatementPtr.Format( KDSSQLGetAdapterId, aTaskId );

			PrepareViewL( sqlStatementPtr, iView.EUpdatable );

			if ( iView.FirstL() )
				{
				iView.GetL();
				iAdapterId = iView.ColUint( iColSetAdapters->ColNo( KNSmlAdapterId ) );
				}

			sqlStatementPtr.Format( KDSSQLDeleteAdapterById, iAdapterId );
			err = iDatabase->Execute( sqlStatementPtr );
			CleanupStack::PopAndDestroy(); // sqlStatement
			if ( err < KErrNone )
				{
				iDatabase->Rollback();
				return EFalse;
				}

			CommitAndCompact();
//
//			Delete object from list
//
			CNSmlDSContentType* contentType = ContentTypeId( iAdapterId );
			delete contentType;
			contentType = 0;
//
//			Remove it also from list
//
			iContentTypes->Delete( i );
			iContentTypes->Compress();

			return ETrue;
			}
		}

	CleanupStack::PopAndDestroy(); // sqlStatement
	return EFalse;
	}


//=============================================
//		CNSmlDSProfile::SaveL()
//		Saves a profile ie. writes all profile data to the database.
//		If save is successful returns KErrNone,
//		otherwise returns negative integer (defined in EPOC error codes).
//=============================================

EXPORT_C TInt CNSmlDSProfile::SaveL()
	{
	
	if ( iDatabase->InTransaction() )
		{
		return ( KErrAccessDenied );
		}	

	iDatabase->Begin();

	CNSmlDSCrypt* crypt = new( ELeave ) CNSmlDSCrypt;
	CleanupStack::PushL( crypt );

	if ( iId == KNSmlNewObject )
		{
		// Check OOD before saving a New profile
		RFs fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL(fs);

		if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs, KNSmlDsSettingsFatMinSize))
			{
			CleanupStack::PopAndDestroy(2); // fs, crypt
			iDatabase->Rollback();
			return KErrDiskFull;
			// do not write
			}
		CleanupStack::PopAndDestroy(); //fs

		//new profile
		iTableProfiles.InsertL();
		
		TableProfilesSetColDesL( KNSmlDSProfileDisplayName,	*iDisplayName );
		TableProfilesSetColIntL( KNSmlDSProfileTransportId, iTransportId );
		TableProfilesSetColIntL( KNSmlDSProfileIAPId, iIAPId );

		TableProfilesSetColDesL( KNSmlDSProfileSyncServerUsername, *iSyncServerUsername );
		TableProfilesSetColDesL( KNSmlDSProfileSyncServerPassword, crypt->EncryptedL( *iSyncServerPassword) );	

		TableProfilesSetColDesL( KNSmlDSProfileServerURL,	*iServerURL );
		TableProfilesSetColDesL( KNSmlDSProfileServerId,	*iServerId );

		TableProfilesSetColIntL( KNSmlDSProfileServerAlertedAction, iServerAlertedAction );
		TableProfilesSetColIntL( KNSmlDSProfileDeleteAllowed, iDeleteAllowed );
		TableProfilesSetColIntL( KNSmlDSProfileHidden, iProfileHidden );

		TableProfilesSetColIntL( KNSmlDSProfileHttpAuthUsed, iHttpAuthUsed );
		TableProfilesSetColDesL( KNSmlDSProfileHttpAuthUsername, *iHttpAuthUsername );
		TableProfilesSetColDesL( KNSmlDSProfileHttpAuthPassword, crypt->EncryptedL( *iHttpAuthPassword) );	
		TableProfilesSetColIntL( KNSmlDSProfileAutoChangeIAP, iAutoChangeIAP );
		TableProfilesSetColIntL( KNSmlDSProfileCreatorID, iCreatorId );

		HBufC* v = GetVisibilityStrL();
		CleanupStack::PushL(v);
		TableProfilesSetColDesL( KNSmlDSProfileVisibilityStr, *v);
		CleanupStack::PopAndDestroy(); // v

		TableProfilesSetColIntL( KNSmlDSProfileProtocolVersion, iProtocolVersion );

        TBuf<64> sqlStatement;
        sqlStatement.Format( KDSSQLCountProfiles );
    	PrepareViewL( sqlStatement, iView.EReadOnly );

		iId = iTableProfiles.ColUint( iColSetProfiles->ColNo( KNSmlDSProfileId) ); // Use autoincrement

  		iTableProfiles.PutL();
		
		for ( TInt i = 0; i < iContentTypes->Count(); i++ )
			{
			if( iContentTypes->At(i)->StrValue( EDSAdapterClientDataSource ).Compare( 
				iContentTypes->At(i)->StrValue( EDSAdapterServerDataSource ) ) == 0 )
				{
				iDatabase->Rollback();
				CleanupStack::PopAndDestroy();  //crypt
				return KErrArgument;			
				}
			InsertTableAdaptersL( i );
												
			}
		}
	else
		{
		//existing profile
		TBuf<64> sqlStatement;
		sqlStatement.Format( KDSSQLGetProfile, iId );

		PrepareViewL( sqlStatement, iView.EUpdatable );
		if ( iView.FirstL() )
			{
			iView.GetL();
			iView.UpdateL();

			ViewSetColDesL( KNSmlDSProfileDisplayName, *iDisplayName );
			ViewSetColIntL( KNSmlDSProfileTransportId, iTransportId );
			ViewSetColIntL( KNSmlDSProfileIAPId, iIAPId );
			ViewSetColDesL( KNSmlDSProfileSyncServerUsername, *iSyncServerUsername );
			ViewSetColDesL( KNSmlDSProfileSyncServerPassword, crypt->EncryptedL( *iSyncServerPassword ) );
			ViewSetColDesL( KNSmlDSProfileServerURL, *iServerURL );
			ViewSetColDesL( KNSmlDSProfileServerId, *iServerId );
			
			ViewSetColIntL( KNSmlDSProfileServerAlertedAction, iServerAlertedAction );
			ViewSetColIntL( KNSmlDSProfileDeleteAllowed, iDeleteAllowed );
			ViewSetColIntL( KNSmlDSProfileHidden, iProfileHidden );

			ViewSetColIntL( KNSmlDSProfileHttpAuthUsed, iHttpAuthUsed );
			ViewSetColDesL( KNSmlDSProfileHttpAuthUsername, *iHttpAuthUsername );
			ViewSetColDesL( KNSmlDSProfileHttpAuthPassword, crypt->EncryptedL( *iHttpAuthPassword) );	
			ViewSetColIntL( KNSmlDSProfileAutoChangeIAP, iAutoChangeIAP );
			ViewSetColIntL( KNSmlDSProfileCreatorID, iCreatorId );
            
			HBufC* v = GetVisibilityStrL();
			CleanupStack::PushL(v);
			ViewSetColDesL( KNSmlDSProfileVisibilityStr, *v);
			CleanupStack::PopAndDestroy(); // v

			ViewSetColIntL( KNSmlDSProfileProtocolVersion, iProtocolVersion );

			iView.PutL();
			}
		else
			{
			iDatabase->Rollback();
			CleanupStack::PopAndDestroy();  //crypt
			return KErrNotFound;
			}

		for ( TInt i = 0; i < iContentTypes->Count(); i++ )
			{
			if( iContentTypes->At(i)->StrValue( EDSAdapterClientDataSource ).Compare( 
				iContentTypes->At(i)->StrValue( EDSAdapterServerDataSource ) ) == 0 )
				{
				iDatabase->Rollback();
				CleanupStack::PopAndDestroy();  //crypt
				return KErrArgument;			
				}
			if ( iContentTypes->At( i )->IntValue( EDSAdapterTableId ) == KNSmlNewObject )
				{
				//existing profile & new contentType
				InsertTableAdaptersL( i );
				}
			else
				{
				//existing profile & existing contentType
				sqlStatement.Format( KDSSQLFindContentType, iContentTypes->At( i )->IntValue( EDSAdapterTableId ) );

				PrepareViewL( sqlStatement, iView.EUpdatable );
				if ( iView.FirstL() )
					{
					iView.GetL();
					iView.UpdateL();

					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterEnabled ), ( TInt ) iContentTypes->At( i )->IntValue( EDSAdapterEnabled ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterImlementationUID ), ( TInt ) iContentTypes->At( i )->IntValue( EDSAdapterImplementationId ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterCreatorID ), ( TInt ) iContentTypes->At( i )->IntValue( EDSAdapterCreatorId ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterDisplayName ), iContentTypes->At( i )->StrValue( EDSAdapterDisplayName ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterClientDataSource ), iContentTypes->At( i )->StrValue( EDSAdapterClientDataSource ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterServerDataSource ), iContentTypes->At( i )->StrValue( EDSAdapterServerDataSource ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterSyncType ), ( TInt ) iContentTypes->At( i )->IntValue( EDSAdapterSyncType ) );
					iView.SetColL( iColSetAdapters->ColNo( KNSmlAdapterFilterMatchType ), ( TInt ) iContentTypes->At( i )->IntValue( EDSAdapterFilterMatchType ) );
					iView.PutL();
					}
				else
					{
					iDatabase->Rollback();
					CleanupStack::PopAndDestroy();  //crypt
					return KErrNotFound;
					}
				}

			}
		}
		 
	CleanupStack::PopAndDestroy();  //crypt
	CommitAndCompact();

	return KErrNone;
	}

//=============================================
//		CNSmlDSProfile::IsNew()
//		Returns true if profile is new, otherwise false.
//=============================================

EXPORT_C TBool CNSmlDSProfile::IsNew() const
	{
	return ( iId == KNSmlNewObject );
	}

//=============================================
//		CNSmlDSProfile::SetHiddenL()
//		Returns true if update done otherwise false.
//=============================================

EXPORT_C TBool CNSmlDSProfile::SetHiddenL(TBool aHidden)
	{
	TBool ret = EFalse;

	if (iId < 1) 
		{
		return ret;			
		}

	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EUpdatable );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	if ( iView.FirstL() )
		{
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSetProfiles->ColNo( KNSmlDSProfileHidden ), aHidden );
		iView.PutL();
	
		ret = ETrue;

		// Also profile value set for SaveL()
		SetIntValue( EDSProfileHidden, aHidden);
		}

	return ret;
	}

//=============================================
//		CNSmlDSProfile::IsNullL()
//		Returns true if profile data is NULL, 
//      otherwise false.
//=============================================

EXPORT_C TBool CNSmlDSProfile::IsNullL( const TDesC& aFieldName ) 
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
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

//=============================================
//		CNSmlDSProfile::StrValue()
//		Returns a given string value.
//=============================================

EXPORT_C const TDesC& CNSmlDSProfile::StrValue( TNSmlDSProfileData aProfileItem ) const
	{	
	HBufC* result = 0;
	switch ( aProfileItem )
		{
		case ( EDSProfileDisplayName ) :
			{
			result = iDisplayName;
			}
		break;

		case ( EDSProfileSyncServerUsername ) :
			{
			result = iSyncServerUsername;
			}
		break;

		case ( EDSProfileSyncServerPassword ) :
			{
			result = iSyncServerPassword;
			}
		break;

		case ( EDSProfileServerURL ) :
			{
			result = iServerURL;
			}
		break;
		
		case ( EDSProfileServerId ) :
			{
			result = iServerId;
			}
		break;

		case ( EDSProfileHttpAuthUsername ) :
			{
			result = iHttpAuthUsername;
			}
		break;

		case ( EDSProfileHttpAuthPassword ) :
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

//=============================================
//		CNSmlDSProfile::IntValue()
//		Returns a given int value.
//=============================================

EXPORT_C TInt CNSmlDSProfile::IntValue( TNSmlDSProfileData aProfileItem ) const
	{	
	TInt result( 0 );
	switch ( aProfileItem )
		{
		case ( EDSProfileId ) :
			{
			result = iId;
			}
		break;
		
		case ( EDSProfileIAPId ) :
			{
			result = iIAPId;
			}
		break;

		case ( EDSProfileTransportId ) :
			{
			result = iTransportId;
			}
		break;
	
		case ( EDSProfileServerAlertedAction ) :
			{
			result = iServerAlertedAction;
			}
		break;
		
		case ( EDSProfileDeleteAllowed ) :
			{
			result = iDeleteAllowed;
			}
		break;

		case ( EDSProfileHidden ) :
			{
			result = iProfileHidden;
			}
		break;

		case ( EDSProfileHttpAuthUsed ) :
			{
			result = iHttpAuthUsed;
			}
			break;
			
		case ( EDSProfileAutoChangeIAP ) :
			{
			result = iAutoChangeIAP;
			}
			break;
		
		case ( EDSProfileProtocolVersion ) :
			{
			result = iProtocolVersion;
			}
			break;

		case ( EDSProfileCreatorId ) :
			{
			result = iCreatorId;
			}
			break;
		
		case ( EDSProfileDefaultProfile ) :
            {
            result = iDefaultProfile;
            }
		    break;

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return result;
	}

//=============================================
//		CNSmlDSProfile::SetStrValue()
//		Sets a given string value.
//=============================================

EXPORT_C void CNSmlDSProfile::SetStrValue( TNSmlDSProfileData aProfileItem, const TDesC& aNewValue )
	{
	switch ( aProfileItem )
		{
		case ( EDSProfileDisplayName ) :
			{
			*iDisplayName = aNewValue;
			}
		break;

		case ( EDSProfileSyncServerUsername ) :
			{
			*iSyncServerUsername = aNewValue;
			}
		break;

		case ( EDSProfileSyncServerPassword ) :
			{
			*iSyncServerPassword = aNewValue;
			}
		break;

		case ( EDSProfileServerURL ) :
			{
			*iServerURL = aNewValue;
			}
		break;

		case ( EDSProfileServerId ) :
			{
			*iServerId = aNewValue;
			}
		break;

		case ( EDSProfileHttpAuthUsername ) :
			{
			*iHttpAuthUsername = aNewValue;
			}
			break;
			
		case ( EDSProfileHttpAuthPassword ) :
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

//=============================================
//		CNSmlDSProfile::SetIntValue()
//		Sets a given int value.
//=============================================

EXPORT_C void CNSmlDSProfile::SetIntValue( TNSmlDSProfileData aProfileItem, const TInt aNewValue )
	{
	switch ( aProfileItem )
		{
		case ( EDSProfileId ) :
			{
			iId = aNewValue;
			}
		break;

		case ( EDSProfileIAPId ) :
			{
			iIAPId = aNewValue;
			}
		break;

		case ( EDSProfileTransportId ) :
			{
			iTransportId = aNewValue;
			}
		break;
		
		case ( EDSProfileServerAlertedAction ) :
			{
			iServerAlertedAction = aNewValue;
			}
		break;
		
		case ( EDSProfileDeleteAllowed ) :
			{
			iDeleteAllowed = aNewValue;
			}
		break;
		
		case ( EDSProfileHidden ) :
			{
			iProfileHidden = aNewValue;
			}
		break;
		
		case ( EDSProfileHttpAuthUsed ) :
			{
			iHttpAuthUsed = aNewValue;
			}
			break;
			
		case ( EDSProfileAutoChangeIAP ) :
			{
			iAutoChangeIAP = aNewValue;
			}
			break;
			
		case ( EDSProfileProtocolVersion ) :
			{
			iProtocolVersion = aNewValue;
			}
			break;

		case ( EDSProfileCreatorId ) :
			{
			iCreatorId = aNewValue;
			}
			break;
			
		case ( EDSProfileDefaultProfile ) :
            {
            iDefaultProfile = aNewValue;
            }
		    break;
		    
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}

//=============================================
//		CNSmlDSProfile::Visibility()
//		Returns the visibility of the given 
//		dataitem.
//=============================================

EXPORT_C TNSmlDSFieldVisibility CNSmlDSProfile::Visibility( TNSmlDSProfileData aProfileItem ) const
	{

	TInt index = (TInt)aProfileItem;

	if (index >= iVisibilityArray.Count() || index < 0) 
		{
		// unknown field. TNSmlDSProfileData-enum has been changed without changing 
		// the size of iVisibilityArray, or negative 
		// values has been assigned to some of those fields
		User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
		}

	return iVisibilityArray[index];
	}

//=============================================
//		CNSmlDSProfile::SetVisibility()
//		Sets the visibility of the given 
//		dataitem.
//=============================================

EXPORT_C void CNSmlDSProfile::SetVisibility( TNSmlDSProfileData aProfileItem,
							const TNSmlDSFieldVisibility aVisibility )
	{

	TInt index = (TInt)aProfileItem;

	if (index >= iVisibilityArray.Count() || index < 0) 
		{
		// unknown field. TNSmlDSProfileData-enum has been changed without changing 
		// the size of iVisibilityArray, or negative 
		// values has been assigned to some of those fields
		User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
		}

	iVisibilityArray[index] = aVisibility;
	}

//=============================================
//		CNSmlDSProfile::InitVisibilityArray()
//		Method for initializing the 
//		visibilityArray, i.e. parses the given 
//		string's characters and maps them to 
//		dataitem's visibilities.
//=============================================

void CNSmlDSProfile::InitVisibilityArray( const TDesC& aVisibilityStr )
	{

	if ( aVisibilityStr.Length() != iVisibilityArray.Count())
		{
		// The given string must be equal in size to visibilityarray, otherwise
		// there is something wrong with the format
		User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
		}

	for (TInt i=0; i < aVisibilityStr.Length(); ++i)
		{
		iVisibilityArray[i] = CharToVisibility( aVisibilityStr[i] );
		}

	}

//=============================================
//		CNSmlDSProfile::GetVisibilityStrL()
//		Return the visibility array as a 
//		string (the form in which it is written 
//		and read from the database).
//=============================================

HBufC* CNSmlDSProfile::GetVisibilityStrL() const
	{

	HBufC* result = HBufC::NewL(KNSmlDSVisibilityArraySize);
	TPtr ptr = result->Des();

	for (TInt i=0; i < iVisibilityArray.Count(); ++i)
		{
		ptr += VisibilityToChar( iVisibilityArray[i] );
		}

	return result;
	}
	
//=============================================
//		CNSmlDSProfile::CharToVisibility()
//		Turns the given char into 
//		TNSmlDSFieldVisibility.
//=============================================
TNSmlDSFieldVisibility CNSmlDSProfile::CharToVisibility( const TText aCh ) const
	{
	switch ( aCh )
		{
		case KNSmlDSNormalChar:
			{
			return EVisibilityNormal;
			}
		case KNSmlDSReadOnlyChar:
			{
			return EVisibilityReadOnly;
			}
		case KNSmlDSHiddenChar:
			{
			return EVisibilityHidden;
			}
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			return EVisibilityNormal;
			}
		}
	}


//=============================================
//		CNSmlDSProfile::VisibilityToChar()
//		Turns the given visibility into char.
//=============================================
TPtrC CNSmlDSProfile::VisibilityToChar( const TNSmlDSFieldVisibility aVisibility ) const
	{
	switch (aVisibility)
		{
		case EVisibilityNormal:
			{
			return KNSmlDSVisibilityNormal();
			}
		case EVisibilityReadOnly:
			{
			return KNSmlDSVisibilityReadOnly();
			}
		case EVisibilityHidden:
			{
			return KNSmlDSVisibilityHidden();
			}
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			return KNullDesC();
			}
		}
	}

//=============================================
//		CNSmlDSProfile::HasLogL()
//		Returns true if profile has Log-data, 
//      otherwise false.
//=============================================

EXPORT_C TBool CNSmlDSProfile::HasLogL() 
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLGetProfile, iId );

	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	if ( iView.FirstL() )
		{
		iView.GetL();
		if ( iView.IsColNull( iColSetProfiles->ColNo( KNSmlDSProfileLog ) ) )
			{
			return EFalse;
			}
		}
	return ETrue;
	}

//=============================================
//		CNSmlDSProfile::LogReadStreamL()
//		Reads log-data from database
//=============================================
EXPORT_C RReadStream& CNSmlDSProfile::LogReadStreamL()
{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDSSQLGetProfile, iId);

	User::LeaveIfError(iRSView.Prepare(*iDatabase, TDbQuery( sqlStatementPtr ), RDbRowSet::EReadOnly));
	User::LeaveIfError(iRSView.EvaluateAll());
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iRSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDSProfileLog()); // Ordinal position of long column
	delete colSet;
	colSet = NULL;
	
	// get row
	if( iRSView.FirstL() )
		{
		iRSView.GetL();
		iRs.OpenL(iRSView, col);
		}

	return iRs;
}

//=============================================
//		CNSmlDSProfile::LogWriteStreamL()
//      Writes given stream-data to database
//=============================================

EXPORT_C RWriteStream& CNSmlDSProfile::LogWriteStreamL()
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetProfile().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDSSQLGetProfile, iId);
	
	// create a view on the database
	User::LeaveIfError(iWSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EUpdatable));
	User::LeaveIfError(iWSView.EvaluateAll());
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iWSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDSProfileLog()); // Ordinal position of long column
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

	return iWs;
	}


//=============================================
//		CNSmlDSProfile::WriteStreamCommitL()
//	
//		
//=============================================
EXPORT_C void CNSmlDSProfile::WriteStreamCommitL()
	{
	iWSView.PutL();
	iWSView.Close();
	CommitAndCompact();
	}

//=============================================
//		CNSmlDSProfile::PrepareViewL()
//		Closes and prepares the view
//		
//=============================================
void CNSmlDSProfile::PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess )
	{
	iView.Close();
	User::LeaveIfError( iView.Prepare( *iDatabase, TDbQuery( aSql ), aAccess ) );
	}

//=============================================
//		CNSmlDSProfile::UpdateToDeleteAllowedL()
//	
//		
//=============================================
void CNSmlDSProfile::UpdateToDeleteAllowedL()
	{
	iView.GetL();
	iView.UpdateL();
	iView.SetColL( iColSetProfiles->ColNo( KNSmlDSProfileDeleteAllowed ), ( TInt ) EDSAllowed );
	iView.PutL();
	}

//=============================================
//		CNSmlDSProfile::CommitAndCompact
//		Commits update and compacts the database
//		
//=============================================
void CNSmlDSProfile::CommitAndCompact() 
	{
	iDatabase->Commit();
	iDatabase->Compact();
	}

//=============================================
//		CNSmlDSProfile::ResetContentTypesL()
//	
//		
//=============================================
void CNSmlDSProfile::ResetContentTypesL()
	{
	iView.GetL();
	iContentTypes->AppendL( CNSmlDSContentType::NewLC( this->iDatabase ) );
	CleanupStack::Pop();
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterTableId, iView.ColUint( iColSetAdapters->ColNo( KNSmlAdapterId ) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterProfileId, iView.ColInt( iColSetAdapters->ColNo( KNSmlAdapterProfileId ) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterImplementationId, iView.ColUint( iColSetAdapters->ColNo( KNSmlAdapterImlementationUID ) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterEnabled, iView.ColInt( iColSetAdapters->ColNo( KNSmlAdapterEnabled) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterCreatorId, iView.ColInt( iColSetAdapters->ColNo( KNSmlAdapterCreatorID) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetStrValue( EDSAdapterDisplayName, iView.ColDes( iColSetAdapters->ColNo( KNSmlAdapterDisplayName) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetStrValue( EDSAdapterServerDataSource, iView.ColDes( iColSetAdapters->ColNo( KNSmlAdapterServerDataSource) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetStrValue( EDSAdapterClientDataSource, iView.ColDes( iColSetAdapters->ColNo( KNSmlAdapterClientDataSource) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterSyncType, iView.ColInt( iColSetAdapters->ColNo( KNSmlAdapterSyncType) ) );
	( iContentTypes->At( iContentTypes->Count() -1 ) )->SetIntValue( EDSAdapterFilterMatchType, iView.ColInt( iColSetAdapters->ColNo( KNSmlAdapterFilterMatchType) ) );
	}

//=============================================
//		CNSmlDSProfile::TableProfilesSetColIntL
//	
//		
//=============================================
void CNSmlDSProfile::TableProfilesSetColIntL( const TDesC& aFieldName, TInt aValue)
	{
	iTableProfiles.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//=============================================
//		CNSmlDSProfile::TableProfilesSetColDesL
//	
//		
//=============================================
void CNSmlDSProfile::TableProfilesSetColDesL( const TDesC& aFieldName, TDesC& aValue)
	{
	iTableProfiles.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//=============================================
//		CNSmlDSProfile::ViewSetColIntL
//	
//		
//=============================================
void CNSmlDSProfile::ViewSetColIntL( const TDesC& aFieldName, TInt aValue)
	{
	iView.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//=============================================
//		CNSmlDSProfile::ViewSetColDesL
//	
//		
//=============================================
void CNSmlDSProfile::ViewSetColDesL( const TDesC& aFieldName, TDesC& aValue)
	{
	iView.SetColL( iColSetProfiles->ColNo( aFieldName ), aValue );
	}

//=============================================
//		CNSmlDSProfile::InsertTableAdaptersL
//		Insert adapterdata to database
//		
//=============================================
void CNSmlDSProfile::InsertTableAdaptersL( TInt aInd)
	{
	iTableAdapters.InsertL();
	
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterProfileId ), iId );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterImlementationUID ), ( TInt ) iContentTypes->At( aInd )->IntValue( EDSAdapterImplementationId ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterEnabled ), iContentTypes->At( aInd )->IntValue( EDSAdapterEnabled ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterCreatorID ), iContentTypes->At( aInd )->IntValue( EDSAdapterCreatorId ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterDisplayName ), iContentTypes->At( aInd )->StrValue( EDSAdapterDisplayName ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterServerDataSource ), iContentTypes->At( aInd )->StrValue( EDSAdapterServerDataSource ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterClientDataSource ), iContentTypes->At( aInd )->StrValue( EDSAdapterClientDataSource ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterSyncType ), iContentTypes->At( aInd )->IntValue( EDSAdapterSyncType ) );
	iTableAdapters.SetColL( iColSetAdapters->ColNo( KNSmlAdapterFilterMatchType ), iContentTypes->At( aInd )->IntValue( EDSAdapterFilterMatchType ) );

	TInt id = iTableAdapters.ColUint( iColSetAdapters->ColNo(KNSmlAdapterId ));
	
	iTableAdapters.PutL();
	
	iContentTypes->At( aInd )->SetIntValue( EDSAdapterTableId,  id);
	}

//=============================================
//		CNSmlDSProfile::GetContentTypesL()
//		Member iContentTypes is resetted and appended
//		with CNSmlDSContentType objects.
//		Depending on parameter aMode method appends all or active content types.
//=============================================

void CNSmlDSProfile::GetContentTypesL( const TNSmlDSContentTypeGetMode aMode )
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetAllContentTypes().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	switch ( aMode )
		{
		case ( EDSAllContentTypes ) :
			{
			sqlStatementPtr.Format( KDSSQLGetAllContentTypes, iId );
			}
		break;
		
		default:
			{
			CleanupStack::PopAndDestroy(); // sqlStatement

			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}		

	iContentTypes->Reset();
	
	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	if ( iView.FirstL() )
		{
		ResetContentTypesL();
		while ( iView.NextL() )
			{
			ResetContentTypesL();
			}
		}
	}
	

//=============================================
//		CNSmlDSProfile::GetContentTypeL( TInt aImplementationUID )
//		Gets contenttype with given impl. id.
//=============================================
CNSmlDSContentType* CNSmlDSProfile::GetContentTypeL( TInt aImplementationUID )
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLGetAllContentTypesByImplementationId().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();
	sqlStatementPtr.Format( KDSSQLGetAllContentTypesByImplementationId, iId, aImplementationUID );
	
	PrepareViewL( sqlStatementPtr, iView.EReadOnly );
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	if ( iView.FirstL() )
		{
		ResetContentTypesL();
		}

	return iContentTypes->At( iContentTypes->Count() - 1 );
	}

//  End of File
