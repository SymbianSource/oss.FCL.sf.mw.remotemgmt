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
* Description:  Multiple bearers
*
*/



// INCLUDE FILES
#include <sysutil.h>

#include "nsmlroam.h"


// --------------------------------
// CNSmlRoamTable::NewLC( RDbNamedDatabase& aDatabase )
// Creates new instance of the CNSmlRoamTable
// --------------------------------
//
EXPORT_C CNSmlRoamTable* CNSmlRoamTable::NewLC( RDbNamedDatabase& aDatabase )
	{
	CNSmlRoamTable* self = new ( ELeave ) CNSmlRoamTable( aDatabase );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// --------------------------------
// CNSmlRoamTable::NewL( RDbNamedDatabase& aDatabase )
// Creates new instance of the CNSmlRoamTable
// --------------------------------
//
EXPORT_C CNSmlRoamTable* CNSmlRoamTable::NewL(RDbNamedDatabase& aDatabase)
	{
	CNSmlRoamTable* self = NewLC( aDatabase );
	CleanupStack::Pop();
	return self;
	}

// --------------------------------
// CNSmlRoamTable::ConstructL()
// Second constructor
// --------------------------------
//
void CNSmlRoamTable::ConstructL()
	{
	RDbTable dbTable;
	CleanupClosePushL( dbTable );

	TInt result = dbTable.Open(iDatabase, KTable);

	if (result != KErrNone)
		{
		User::LeaveIfError( iDatabase.Execute( KSQLCreateTable ) );		
		User::LeaveIfError( iDatabase.Execute( KSQLCreateIndex ) );
		User::LeaveIfError( dbTable.Open(iDatabase, KTable) );
		}

	iColSet = dbTable.ColSetL();

	CleanupStack::PopAndDestroy(); //dbTable

	}

// --------------------------------
// CNSmlRoamTable::CNSmlRoamTable(RDbNamedDatabase& aDatabase)
// Constructor
// --------------------------------
//
CNSmlRoamTable::CNSmlRoamTable(RDbNamedDatabase& aDatabase)
: iDatabase(aDatabase)
	{
	}

// --------------------------------
// CNSmlRoamTable::~CNSmlRoamTable()
// Destructor
// --------------------------------
//
EXPORT_C CNSmlRoamTable::~CNSmlRoamTable()
	{
	delete iColSet;
	}

// --------------------------------
// CNSmlRoamTable::PutItemsL( TInt aProfileId, CNSmlIAPArray* aItems )
// Put items from array to table for profile id
// --------------------------------
//
EXPORT_C void CNSmlRoamTable::PutItemsL( TInt aProfileId, CNSmlIAPArray* aItems )
	{
	TInt count(0);
	
	RDbView view;
	CleanupClosePushL( view );

	CNSmlRoamItem item;

	User::LeaveIfNull( aItems );

	if (! IsNewItemL(view, aProfileId))
		{
		count = view.CountL();
		
		if ( count > aItems->Count() )
			{
			RemoveAllByIdL(aProfileId);
			}
		}

	for (TInt i = 0; i < aItems->Count(); i++)
		{
		item.iProfileId= aProfileId;
		item.iPriority = i;
		item.iIAPId = aItems->At(i);
		HandleItemL( item );
		}
	
	CleanupStack::PopAndDestroy(); //view
	}

// --------------------------------
// CNSmlRoamTable::GetItemsForIdL( TInt aProfileId )
// Return array of IAPs for profile id
// --------------------------------
//
EXPORT_C CNSmlIAPArray* CNSmlRoamTable::GetItemsForIdL( TInt aProfileId )
	{
	RDbView view;
	CleanupClosePushL( view );
	
	CNSmlIAPArray* array = new (ELeave) CArrayFixFlat<TUint32>(4);
	CleanupStack::PushL( array );
	
	IsNewItemL(view, aProfileId);
	
	view.FirstL();
		
	while (view.AtRow())
		{
		view.GetL();
		array->AppendL( view.ColUint32( iColSet->ColNo( KColIapId ) ));
		view.NextL();
		}
	
	CleanupStack::Pop(); //array
	CleanupStack::PopAndDestroy(); //view
	return array;
	}

// --------------------------------
// CNSmlRoamTable::GetItemIAPL( TInt aProfileId, TInt aPriority )
// Return IAP for profile id and priority
// --------------------------------
//
EXPORT_C TUint32 CNSmlRoamTable::GetItemIAPL( TInt aProfileId, TInt aPriority )
	{
	RDbView view;
	CleanupClosePushL(view);

	TUint32 uint(0);
	TBuf<KNSmlMaxSQLLength> sqlBuf;
	sqlBuf.Format( KSQLSelectUint, aProfileId, aPriority );

	view.Prepare(iDatabase, TDbQuery( sqlBuf ));
	view.EvaluateAll();

	if ( view.IsEmptyL() )
		{		
		User::Leave( KErrNotFound );
		}
	
	view.FirstL();
	view.GetL();

	uint = view.ColUint( iColSet->ColNo( KColIapId ));
	
	CleanupStack::PopAndDestroy(); //view

	return uint;
	}

// --------------------------------
// CNSmlRoamTable::RemoveAllByIdL( TInt aProfileId )
// Removes all IAPs for profile id
// --------------------------------
//
EXPORT_C void CNSmlRoamTable::RemoveAllByIdL( TInt aProfileId )
	{
	TBuf<KNSmlMaxSQLLength> sqlBuf;
	sqlBuf.Format(KSQLDeleteForId, aProfileId);

	TInt res( iDatabase.Execute( sqlBuf ) ); 
	User::LeaveIfError( res );
#ifdef __MODULETEST
	test.iDeleted = res;
#endif
	}

// --------------------------------
// CNSmlRoamTable::AddItemL( CNSmlRoamItem& aItem )
// Adds item to IAP table
// --------------------------------
//
void CNSmlRoamTable::AddItemL( CNSmlRoamItem& aItem )
	{

	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);
	
	if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs, 12))
		{
		User::Leave( KErrDiskFull );
		}

	CleanupStack::PopAndDestroy(); //fs

	TBuf<KNSmlMaxSQLLength> sqlBuf;
	sqlBuf.Format( KSQLInsertQuery, aItem.iProfileId, aItem.iIAPId, aItem.iPriority );

	RDbView view;
	CleanupClosePushL( view );

	view.Prepare(iDatabase, TDbQuery(KSQLMatchQuery2), RDbView::EInsertOnly);
	view.InsertL();
	view.SetColL( iColSet->ColNo( KColProfileId ), aItem.iProfileId );
	view.SetColL( iColSet->ColNo( KColPriority ), aItem.iPriority );
	view.SetColL( iColSet->ColNo( KColIapId ), aItem.iIAPId );
	view.PutL();

	CleanupStack::PopAndDestroy(); //view

#ifdef __MODULETEST
	test.iAdded++;
#endif
	}

// --------------------------------
// CNSmlRoamTable::UpdateItemL(RDbView& aView, CNSmlRoamItem& aItem )
// Updates item in the IAP table
// --------------------------------
//
void CNSmlRoamTable::UpdateItemL(RDbView& aView, CNSmlRoamItem& aItem )
	{
	
	aView.FirstL();
	aView.UpdateL();
	
	aView.SetColL( iColSet->ColNo( KColProfileId ), aItem.iProfileId );
	aView.SetColL( iColSet->ColNo( KColPriority ), aItem.iPriority );
	aView.SetColL( iColSet->ColNo( KColIapId ), aItem.iIAPId );

	aView.PutL();

#ifdef __MODULETEST	
	test.iUpdated++;
#endif
	}

// --------------------------------
// CNSmlRoamTable::HandleItemL( CNSmlRoamItem& aItem )
// Handles item addition or update
// --------------------------------
//
void CNSmlRoamTable::HandleItemL( CNSmlRoamItem& aItem )
	{
	RDbView view;
	CleanupClosePushL( view);
	
	if (IsNewItemL(view, aItem))
		{
		CleanupStack::PopAndDestroy(); //view
		AddItemL(aItem);
		}
	else
		{
		UpdateItemL(view, aItem);		
		CleanupStack::PopAndDestroy(); //aView
		}
	
	}

// --------------------------------
// CNSmlRoamTable::IsNewItemL( RDbView& aView, TInt aProfileId)
// Checks if whether profile has IAPs
// --------------------------------
//
TBool CNSmlRoamTable::IsNewItemL( RDbView& aView, TInt aProfileId)
	{
	TBuf<KNSmlMaxSQLLength> sqlBuf;
	sqlBuf.Format(KSQLMultiQuery, aProfileId);

	aView.Prepare(iDatabase, TDbQuery( sqlBuf ));
	aView.EvaluateAll();
	return aView.IsEmptyL();
	}

// --------------------------------
// CNSmlRoamTable::IsNewItemL( RDbView& aView, CNSmlRoamItem& aItem)
// Checks if whether IAP is new for profile id and priority
// --------------------------------
//
TBool CNSmlRoamTable::IsNewItemL( RDbView& aView, CNSmlRoamItem& aItem)
	{
	TBuf<KNSmlMaxSQLLength> sqlBuf;
	sqlBuf.Format( KSQLSelectUint, aItem.iProfileId, aItem.iPriority );

	aView.Prepare(iDatabase, TDbQuery( sqlBuf ));
	aView.EvaluateAll();
	return aView.IsEmptyL();
	}

#ifdef __MODULETEST
EXPORT_C CTestClass* CNSmlRoamTable::Test()
	{
	return &test;
	}
#endif


//  End of File
