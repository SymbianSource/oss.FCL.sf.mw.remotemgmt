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


#ifndef __NSMLROAM_H_
#define __NSMLROAM_H_

#include <e32std.h>
#include <e32base.h>
#include <f32file.h> 
#include <s32file.h> 
#include <d32dbms.h>

//#define __MODULETEST

#ifdef __MODULETEST
#include "../test/DSSettingsTester/inc/NSmlTest.h"
#endif

_LIT(KColProfileId, "profileid");
_LIT(KColIapId, "iapid");
_LIT(KColPriority, "priority");
_LIT(KTable, "iaptable");

_LIT(KSQLCreateTable, "CREATE TABLE iaptable ( profileid INTEGER NOT NULL, priority INTEGER NOT NULL, iapid UNSIGNED INTEGER )");
_LIT(KSQLCreateIndex, "CREATE UNIQUE INDEX iapindex  ON iaptable ( profileid, priority)");
_LIT(KSQLDeleteForId, "DELETE FROM iaptable WHERE profileid = %d");
_LIT(KSQLInsertQuery, "INSERT INTO iaptable ( profileid, priority, iapid) VALUES (%d, %d, %d)");
_LIT(KSQLMatchQuery2, "SELECT profileid, priority, iapid FROM iaptable");
_LIT(KSQLSelectUint, "SELECT  profileid, priority, iapid FROM iaptable WHERE profileid = %d and priority = %d");
_LIT(KSQLMultiQuery, "SELECT  profileid, priority, iapid FROM iaptable WHERE profileid = %d");

const TUint8 KNSmlMaxSQLLength = 100;

class CNSmlRoamTable;
		
class CNSmlRoamItem : public CBase
	{
	public:
		/**
        * Constructor
        * @return -
		*/
		CNSmlRoamItem();

		/**
        * Constructor
        * @return -
		*/
		CNSmlRoamItem(TInt aProfileId, TUint32 aIAPId, TInt aPriority);
		
		/**
        * Destructor
        * @return -
		*/
		virtual ~CNSmlRoamItem();
		
		/**
        * TInt ProfileId()
        * @return - Profile id
		*/
		inline TInt ProfileId();

		/**
        * TUint32 IapId()
        * @return - IAP id
		*/
		inline TUint32 IapId();

		/**
        * TInt Priority()
        * @return - Priority
		*/
		inline TInt Priority();
			
	private:
		friend class CNSmlRoamTable;
		TInt iProfileId;
		TUint32 iIAPId;
		TInt iPriority;
	};

typedef CArrayPtr<CNSmlRoamItem> CNSmlRoamItemArray;
typedef CArrayFixFlat<TUint32> CNSmlIAPArray;


class CNSmlRoamTable : public CBase
	{
	public:

		/**
        * Constructor
		* NewLC(RDbNamedDatabase& aDatabase)
		* @param - Reference to RDBNamedDatabase
        * @return -  CNSmlRoamTable instance pointer
		*/
		IMPORT_C static CNSmlRoamTable* NewLC(RDbNamedDatabase& aDatabase);
		
		/**
        * Constructor
		* NewL(RDbNamedDatabase& aDatabase)
		* @param - Reference to RDBNamedDatabase
        * @return - CNSmlRoamTable instance pointer
		*/
		IMPORT_C static CNSmlRoamTable* NewL(RDbNamedDatabase& aDatabase);
		
		/**
        * Destructor
        * @return - 
		*/
		IMPORT_C virtual ~CNSmlRoamTable();
		
		/**
        * void PutItemsL(TInt aProfileId, CIapArray* aItems )
		* Adds IAPs from array for profile id
		* @param TInt aProfileId - Profile id
		* @param CNSmlIAPArray* aItems - pointer to (TUint32) IAP array 
        * @return -
		*/
		IMPORT_C void PutItemsL(TInt aProfileId, CNSmlIAPArray* aItems );

		/**
        * CIapArray* GetItemsForIdL(TInt aProfileId)
		* Gets IAPs for profile in an array
		* @param TInt aProfileId - Profile id
		* @return - pointer to array of IAPs
		*/
		IMPORT_C CNSmlIAPArray* GetItemsForIdL(TInt aProfileId);
		
		/**
        * TUint32 GetItemIAPL(TInt aProfileId, TInt aPriority)
		* Get IAP for profile and priority
		* @param TInt aProfileId - Profile id
		* @param TInt aPriority - Priority
		* @return - IAP id
		*/
		IMPORT_C TUint32 GetItemIAPL(TInt aProfileId, TInt aPriority);
		
		/**
        * void RemoveAllByIdL(TInt aProfileId)
		* Removes all IAPs for profile id 
		* @param TInt aProfileId - Profile id
		* @return -
		*/
		IMPORT_C void RemoveAllByIdL(TInt aProfileId);

#ifdef __MODULETEST	
		IMPORT_C CTestClass* Test();
#endif
	private:

		/**
        * Second construstor
		* @return -
		*/
		void ConstructL();

		/**
		* Constructor
		* CNSmlRoamTable( RDbNamedDatabase& aDatabase )
		* @param RDbNamedDatabase& aDatabase - The database to be used.
        * @return -
		*/
		CNSmlRoamTable( RDbNamedDatabase& aDatabase );
		
		/**
		* Adds item to table
		* AddItemL( CNSmlRoamItem& aItem )
		* @param CNSmlRoamItem& aItem - Container for profile id, IAP id and priority
        * @return -
		*/
		void AddItemL( CNSmlRoamItem& aItem );
		
		/**
		* Replaces item to table
		* void UpdateItemL(RDbView& aView, CNSmlRoamItem& aItem )
		* @param RDbView& aView - view to table
		* @param - CNSmlRoamItem& aItem - Container for profile id, IAP id and priority
        * @return -
		*/
		void UpdateItemL(RDbView& aView, CNSmlRoamItem& aItem );

		/**
		* Handles the insertion of the item
		* void HandleItemL(CNSmlRoamItem& aItem);
		* @param CNSmlRoamItem& aItem - Container for profile id, IAP id and priority
        * @return -
		*/
		void HandleItemL(CNSmlRoamItem& aItem);

		/**
		* Checks whether items for Profile id exists
		* TBool IsNewItemL(RDbView& aView, TInt aProfileId);
		* @param RDbView& aView - View to table
		* @param TInt aProfileId - Profile id
        * @return - ETrue if new item
		*/
		TBool IsNewItemL(RDbView& aView, TInt aProfileId);

		/**
		* Checks whether item already exists on table
		* TBool IsNewItemL(RDbView& aView, TInt aProfileId);
		* @param RDbView& aView - View to table
		* @param CNSmlRoamItem& aItem - Container for profile id, IAP id and priority
        * @return - ETrue if new item
		*/
		TBool IsNewItemL(RDbView& aView, CNSmlRoamItem& aItem );
		
	private:		
		RDbNamedDatabase& iDatabase;
		CDbColSet* iColSet;

#ifdef __MODULETEST
		CTestClass test;
#endif
	};

#endif