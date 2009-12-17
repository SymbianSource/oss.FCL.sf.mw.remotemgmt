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
#include <nsmlconstants.h>
#include "nsmldssettings.h"
#include <nsmldsconstants.h>


//=============================================
//
//		CNSmlDSContentType
//
//=============================================

//=============================================
//		CNSmlDSContentType::NewL()
//		Creates a new instance of CNSmlDSContentType object. 
//=============================================

CNSmlDSContentType* CNSmlDSContentType::NewL(RDbNamedDatabase* aDatabase )
	{
	CNSmlDSContentType* self = CNSmlDSContentType::NewLC( aDatabase );
	CleanupStack::Pop();
	return self;
	}

//=============================================
//		CNSmlDSContentType::NewLC()
//		Creates a new instance of CNSmlDSContentType object. 
//		Pushes and leaves new instance into CleanupStack.
//=============================================
	
CNSmlDSContentType* CNSmlDSContentType::NewLC(RDbNamedDatabase* aDatabase )
	{
	CNSmlDSContentType* self = new( ELeave ) CNSmlDSContentType;
	CleanupStack::PushL( self );
	self->ConstructL();
	self->iDatabase = aDatabase;
	self->iId = KNSmlNewObject;

	return self;
	}

//=============================================
//		CNSmlDSContentType::ConstructL()
//		Second phase constructor.
//=============================================

void CNSmlDSContentType::ConstructL()
	{
	iDisplayName = HBufC::NewL( KNSmlMaxAdapterDisplayNameLength );
	iServerDataSource = HBufC::NewL( KNSmlMaxRemoteNameLength );
	iClientDataSource = HBufC::NewL( KNSmlMaxLocalNameLength );
	}

//=============================================
//		CNSmlDSContentType::~CNSmlDSContentType()
//		Destructor.
//=============================================

CNSmlDSContentType::~CNSmlDSContentType()
	{
	delete iDisplayName;
	delete iServerDataSource;
	delete iClientDataSource;
	iView.Close();
	iRSView.Close();
	iWSView.Close();
	}


//=============================================
//		CNSmlDSContentType::()
//		
//=============================================

//=============================================
//		CNSmlDSContentType::IntValue()
//		Returns a given int value.
//=============================================

EXPORT_C TInt CNSmlDSContentType::IntValue( TNSmlDSContentTypeData aType ) const
	{	
	TInt result( 0 );

	switch ( aType )
		{
		case ( EDSAdapterTableId ) :
			{
			result = iId;
			}
		break;

		case ( EDSAdapterProfileId ) :
			{
			result = iProfileId;
			}
		break;

		case ( EDSAdapterImplementationId ) :
			{
			result = iImplementationId;
			}
		break;
		
		case ( EDSAdapterEnabled ) :
			{
			result = iEnabled;
			}
		break;
		
		case ( EDSAdapterCreatorId ) :
			{
			result = iCreatorId;
			}
		break;
		
		case ( EDSAdapterSyncType ) :
			{
			result = iSyncType;
			}
		break;

		case ( EDSAdapterFilterMatchType ) :
			{
			result = iFilterMatchType;
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
//		CNSmlDSContentType::SetIntValue()
//		Sets a given int value.
//=============================================

EXPORT_C void CNSmlDSContentType::SetIntValue( TNSmlDSContentTypeData aType, const TInt aNewValue)
	{
	switch ( aType )
		{
		case ( EDSAdapterTableId ) :
			{
			iId = aNewValue;
			}
		break;

		case ( EDSAdapterProfileId ) :
			{
			iProfileId = aNewValue;
			}
		break;

		case ( EDSAdapterImplementationId ) :
			{
			iImplementationId = aNewValue;
			}
		break;

		case ( EDSAdapterEnabled ) :
			{
			iEnabled = (TNSmlDSEnabled) aNewValue;
			}
		break;
		
		case ( EDSAdapterCreatorId ) :
			{
			iCreatorId = aNewValue;
			}
		break;
		
		case ( EDSAdapterSyncType ) :
			{
			iSyncType = aNewValue;
			}
		break;
		
		case ( EDSAdapterFilterMatchType ) :
			{
			iFilterMatchType = ( TSyncMLFilterMatchType ) aNewValue;
			}
		break;

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}

//=============================================
//		CNSmlDSContentType::StrValue()
//		Returns a given string value.
//=============================================

EXPORT_C const TDesC& CNSmlDSContentType::StrValue( TNSmlDSContentTypeData aProfileItem ) const
	{	
	HBufC* result = 0;
	switch ( aProfileItem )
		{
		case ( EDSAdapterDisplayName ) :
			{
			result = iDisplayName;
			}
		break;

		case ( EDSAdapterServerDataSource ) :
			{
			result = iServerDataSource;
			}
		break;

		case ( EDSAdapterClientDataSource ) :
			{
			result = iClientDataSource;
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
//		CNSmlDSContentType::SetStrValue()
//		Sets a given string value.
//=============================================

EXPORT_C void CNSmlDSContentType::SetStrValue( TNSmlDSContentTypeData aProfileItem, const TDesC& aNewValue )
	{
	switch ( aProfileItem )
		{
		case ( EDSAdapterDisplayName ) :
			{
			*iDisplayName = aNewValue;
			}
		break;

		case ( EDSAdapterServerDataSource ) :
			{
			*iServerDataSource = aNewValue;
			}
		break;

		case ( EDSAdapterClientDataSource ) :
			{
			*iClientDataSource = aNewValue;
			}
		break;

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}	


//=============================================
//		CNSmlDSContentType::FilterReadStreamL()
//		Reads log-data from database
//=============================================
EXPORT_C RReadStream& CNSmlDSContentType::FilterReadStreamL()
{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLFindContentType().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDSSQLFindContentType, iId);

	User::LeaveIfError(iRSView.Prepare(*iDatabase, TDbQuery( sqlStatementPtr ), RDbRowSet::EReadOnly));
	User::LeaveIfError(iRSView.EvaluateAll());
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iRSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlAdapterFilter); // Ordinal position of long column
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
//		CNSmlDSContentType::FilterWriteStreamL()
//      Writes given stream-data to database
//=============================================

EXPORT_C RWriteStream& CNSmlDSContentType::FilterWriteStreamL()
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLFindContentType().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KDSSQLFindContentType, iId);
	
	// create a view on the database
	User::LeaveIfError(iWSView.Prepare(*iDatabase, TDbQuery(sqlStatementPtr), RDbRowSet::EUpdatable));
	User::LeaveIfError(iWSView.EvaluateAll());
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	// Get the structure of rowset
	CDbColSet* colSet = iWSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlAdapterFilter); // Ordinal position of long column
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
//		CNSmlDSContentType::WriteStreamCommitL()
//	
//		
//=============================================
EXPORT_C void CNSmlDSContentType::WriteStreamCommitL()
	{
	iWSView.PutL();
	iWSView.Close();
	
	iDatabase->Commit();
	iDatabase->Compact();
	}

//=============================================
//		CNSmlDSContentType::HasFilterL()
//		Returns true if contenttype has filter, 
//=============================================

EXPORT_C TBool CNSmlDSContentType::HasFilterL() 
	{
	HBufC* sqlStatement = HBufC::NewLC( KDSSQLFindContentType().Length() + KNSmlDsSettingsMaxIntegerLength);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format( KDSSQLFindContentType, iId );

	iView.Prepare( *iDatabase, TDbQuery( sqlStatementPtr ), iView.EReadOnly );
		
	CleanupStack::PopAndDestroy(); // sqlStatement
	
	if ( iView.FirstL() )
		{
		iView.GetL();
		CDbColSet* colSet = iView.ColSetL();
		CleanupStack::PushL( colSet );
		if ( iView.IsColNull( colSet->ColNo( KNSmlAdapterFilter ) ) )
			{
			CleanupStack::PopAndDestroy(); //colSet
			return EFalse;
			}
		CleanupStack::PopAndDestroy(); //colSet
		}
	return ETrue;
	}
	
//Class CNSmlDSContentSettingType implementation


//=============================================
//		CNSmlDSContentSettingType::NewL()
//		Creates a new instance of CNSmlDSContentType object. 
//=============================================

CNSmlDSContentSettingType* CNSmlDSContentSettingType::NewL( )
	{
	CNSmlDSContentSettingType* self = CNSmlDSContentSettingType::NewLC( );
	CleanupStack::Pop();
	return self;
	}

//=============================================
//		CNSmlDSContentSettingType::NewLC()
//		Creates a new instance of CNSmlDSContentSettingType object. 
//		Pushes and leaves new instance into CleanupStack.
//=============================================
	
CNSmlDSContentSettingType* CNSmlDSContentSettingType::NewLC( )
	{
	CNSmlDSContentSettingType* self = new( ELeave ) CNSmlDSContentSettingType;
	CleanupStack::PushL( self );
	self->ConstructL();
	
	return self;
	}

//=============================================
//		CNSmlDSContentType::ConstructL()
//		Second phase constructor.
//=============================================

void CNSmlDSContentSettingType::ConstructL()
	{
	iImplementationId = HBufC::NewL( KNSmlMaxAdapterIdLength );
	iProfileName = HBufC::NewL( KNSmlMaxProfileNameLength );
	iServerId = HBufC::NewL( KNSmlMaxServerIdLength );
	iServerDataSource = HBufC::NewL( KNSmlMaxRemoteNameLength );
	}

//=============================================
//		CNSmlDSContentSettingType::~CNSmlDSContentSettingType()
//		Destructor.
//=============================================

CNSmlDSContentSettingType::~CNSmlDSContentSettingType()
	{
	delete iImplementationId;
	delete iProfileName;
	delete iServerId;
	delete iServerDataSource;
	
	}
	
//=============================================
//		CNSmlDSContentSettingType::StrValue()
//		Returns a given string value.
//=============================================
const TDesC& CNSmlDSContentSettingType::StrValue(TNSmlXMLContentData aType ) const
	{	
	HBufC* content = 0;
	switch ( aType )
		{
		case ( EXMLDSAdapaterId ) :
			{
			content = iImplementationId;
			break;
			}
	
		case ( EXMLDSProfileName ) :
			{
			content = iProfileName;
			break;
			}
	
		case ( EXMLDSServerId ) :
			{
			content = iServerId;
			break;
			}
	
		case ( EXMLDSServerDataSource ) :
			{
			content = iServerDataSource;
			break;
			}

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return *content; 
	}

//=============================================
//		CNSmlDSContentSettingType::SetStrValue()
//		Sets a given string value.
//=============================================

void CNSmlDSContentSettingType::SetStrValue( TNSmlXMLContentData aType, const TDesC& aNewValue )
	{
	switch ( aType )
		{
		case ( EXMLDSAdapaterId ) :
			{
			*iImplementationId = aNewValue;
			break;
			}
		

		case ( EXMLDSProfileName ) :
			{
			*iProfileName = aNewValue;
			break;
			}
	

		case ( EXMLDSServerId ) :
			{
			*iServerId = aNewValue;
			break;
			}
		
		
		case ( EXMLDSServerDataSource ) :
			{
			*iServerDataSource = aNewValue;
			break;
			}
	

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	}		


//  End of File
