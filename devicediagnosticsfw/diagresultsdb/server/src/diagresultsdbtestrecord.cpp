/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of 
*
*/


#include "diagresultsdatabasetestrecordinfo.h"
#include "diagresultsdbtestrecord.h"


// ---------------------------------------------------------------------------
// Constructor. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecord::CDiagResultsDbTestRecord( TBool aReadOnly, 
        TUid aRecordUid ): iRecordUid(aRecordUid), iReadOnly(aReadOnly)
	{
	/*
	RecordInfo().iRecordId = iRecordUid;
	RecordInfo().iDbUid = iDbUid;
	RecordInfo().iCompleted = EFalse;
	*/
	}

// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecord* CDiagResultsDbTestRecord::NewL( TBool aReadOnly, 
        TUid aRecordUid )
	{
	CDiagResultsDbTestRecord* testrecord =	
	    new (ELeave) CDiagResultsDbTestRecord( aReadOnly, aRecordUid  );
	CleanupStack::PushL( testrecord );
	testrecord->ConstructL();
	CleanupStack::Pop();
	return testrecord;
	}

// ---------------------------------------------------------------------------
// ConstructL. Set test record overview's DRM and home times.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecord::ConstructL()
	{
	//Time when this test record was created.
//	iTestRecordInfo.iDrmStartTime = 
	                            //TDiagResultsDatabaseTestRecordInfo::DRMTimeL();
	//iTestRecordInfo.iStartTime.HomeTime();
	}

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
// 
TBool CDiagResultsDbTestRecord::ReadOnly() const
    {
    return iReadOnly;
    }

// ---------------------------------------------------------------------------
// Destructor. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecord::~CDiagResultsDbTestRecord()
    {
    iTestArray.ResetAndDestroy();
    iTestArray.Close();
    }

// ---------------------------------------------------------------------------
// AddL. 
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbTestRecord::AddL( CDiagResultsDatabaseItem* aItem )
    {
    for (TInt i=0; i < iTestArray.Count(); ++i)
        {
        
        //Found same item inside the array. Delete the found item.
        if ( aItem->TestUid() == iTestArray[i]->TestUid() )
            {
            CDiagResultsDatabaseItem* item = iTestArray[i];
            iTestArray.Remove( i );
            delete item;
            item = 0;
            break;
            }
        }
        
    iTestArray.AppendL( aItem );
    }

// ---------------------------------------------------------------------------
// Operator [].
// ---------------------------------------------------------------------------
//     
const CDiagResultsDatabaseItem& CDiagResultsDbTestRecord::operator[] (TInt aIndex) const
    {
    return *(iTestArray[aIndex]);
    }

// ---------------------------------------------------------------------------
// Getter. This can be also NULL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDatabaseItem* CDiagResultsDbTestRecord::GetItem(TInt aIndex) const
    {
    return iTestArray[aIndex];
    }

// ---------------------------------------------------------------------------
// Count().
// ---------------------------------------------------------------------------
// 
TInt CDiagResultsDbTestRecord::Count() const
    {
    return iTestArray.Count();
    }

// ---------------------------------------------------------------------------
// RemoveL().
// ---------------------------------------------------------------------------
//         
void CDiagResultsDbTestRecord::RemoveL( TInt aIndex )
    {
    iTestArray.Remove( aIndex );
    }


// ---------------------------------------------------------------------------
// RemoveL().
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbTestRecord::RemoveL( TUid aTestUid )
    {
     for ( TInt i = 0; i < iTestArray.Count(); ++i )
        {        
        if ( iTestArray[i]->TestUid() == aTestUid )
            {
            iTestArray.Remove( i );
            }            
        }
    }

// ---------------------------------------------------------------------------
// Find test record
// ---------------------------------------------------------------------------
//  
TInt CDiagResultsDbTestRecord::FindTestRecord( 
                                const CDiagResultsDatabaseItem& aItem ) const
    {
    TInt index = -1;
    
    for (TInt i = 0; i < iTestArray.Count(); ++i)
        {
        
        if ( iTestArray[i]->TestUid() == aItem.TestUid() )
            {
            index = i;
            break;
            }
        }
     
    return index;
    }

// ---------------------------------------------------------------------------
// Find test record
// ---------------------------------------------------------------------------
//  
CDiagResultsDatabaseItem* CDiagResultsDbTestRecord::FindTestRecord( TUid aUid )
    {
     for (TInt i = 0; i < iTestArray.Count(); ++i)
        {        
        if ( iTestArray[i]->TestUid() == aUid )
            {
            return iTestArray[i];
            }
        }
        
    return NULL;
    }
//End of file
