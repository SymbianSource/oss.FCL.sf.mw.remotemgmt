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
* Description:  Symbian OS Server source.
*
*/



// INCLUDE FILES
#include "nsmlsosserver.h"


// --------------------------------------------------------------------------
// CNSmlJob::CNSmlJob()
// --------------------------------------------------------------------------
//
CNSmlJob::CNSmlJob() : iSyncType((TSmlSyncType)-1)
	{
    iPackage = NULL;
	iUimode = KErrNotFound;
	iIapId = KErrNotFound;
	}

// --------------------------------------------------------------------------
// CNSmlJob::~CNSmlJob()
// --------------------------------------------------------------------------
//
CNSmlJob::~CNSmlJob()
	{
    delete iCtInfo;
    delete iPackage;
    iTaskIds.Close();
	}

// --------------------------------------------------------------------------
// TInt CNSmlJob::JobId() const
// --------------------------------------------------------------------------
//
TInt CNSmlJob::JobId() const
    {
    return iJobId;
    }

// --------------------------------------------------------------------------
// TInt CNSmlJob::ProfileId() const
// --------------------------------------------------------------------------
//    
TInt CNSmlJob::ProfileId() const
    {
    return iProfId;        
    }

// --------------------------------------------------------------------------
// TInt CNSmlJob::CreatorId() const
// --------------------------------------------------------------------------
//    
TInt CNSmlJob::CreatorId() const
    {
    return iCreatorId;        
    }

// --------------------------------------------------------------------------
// TInt CNSmlJob::TransportId() const
// --------------------------------------------------------------------------
//    
TInt CNSmlJob::TransportId() const
    {
    return iTransportId;    
    }

// --------------------------------------------------------------------------
// void CNSmlJob::TaskIds( RArray<TInt>& aTasks ) const
// --------------------------------------------------------------------------
//    
void CNSmlJob::TaskIds( RArray<TInt>& aTasks ) const
    {
    aTasks.Reset();
    for ( TInt i = 0; i < iTaskIds.Count(); ++i )
    	{
    	aTasks.Append( iTaskIds[i] );
    	}
    }

// --------------------------------------------------------------------------
// TSmlUsageType CNSmlJob::UsageType() const
// --------------------------------------------------------------------------
    
TSmlUsageType CNSmlJob::UsageType() const
    {
    return iUsageType;        
    }

// --------------------------------------------------------------------------
// TNSmlJobType CNSmlJob::JobType() const
// --------------------------------------------------------------------------
//
TNSmlJobType CNSmlJob::JobType() const
    {
    return iJobType;
    }    

// --------------------------------------------------------------------------
// TSmlSyncType CNSmlJob::SyncType() const
// --------------------------------------------------------------------------
//
TSmlSyncType CNSmlJob::SyncType() const
    {
    return iSyncType;
    }    

// --------------------------------------------------------------------------
// const CNSmlSOSSession* CNSmlJob::OwnerSession() const
// --------------------------------------------------------------------------
//
const CNSmlSOSSession* CNSmlJob::OwnerSession() const
    {
    return iSession;
    }

// --------------------------------------------------------------------------
// CNSmlSOSSession* CNSmlJob::OwnerSessionNonConst() const
// --------------------------------------------------------------------------
//
CNSmlSOSSession* CNSmlJob::OwnerSessionNonConst() const
    {
    return iSession;
    }

// --------------------------------------------------------------------------
// TBool CNSmlJob::JobRunning() const
// --------------------------------------------------------------------------
//
TBool CNSmlJob::JobRunning() const 
    {
    return iRunning;
    }
    
// --------------------------------------------------------------------------
// void CNSmlJob::SetJobId( const TInt aJobId )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetJobId( const TInt aJobId )
    {
    iJobId = aJobId;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetProfId( const TInt aProfId )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetProfId( const TInt aProfId )
    {
    iProfId = aProfId;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetCreatorId( const TInt aCreatorId )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetCreatorId( const TInt aCreatorId )
    {
    iCreatorId = aCreatorId;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetUsageType( const TSmlUsageType aUsageType )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetUsageType( const TSmlUsageType aUsageType ) 
    {
    iUsageType = aUsageType;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetJobType( const TNSmlJobType aJobType )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetJobType( const TNSmlJobType aJobType ) 
    {
    iJobType = aJobType;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetSyncType( const TSmlSyncType aSyncType ) 
// --------------------------------------------------------------------------
//
void CNSmlJob::SetSyncType( const TSmlSyncType aSyncType ) 
    {
    iSyncType = aSyncType;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetTransportId( const TInt aTransportId )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetTransportId( const TInt aTransportId ) 
    {
    iTransportId = aTransportId;
    }

// --------------------------------------------------------------------------
// void CNSmlJob::AddTaskIdL( const TInt aTaskId ) 
// --------------------------------------------------------------------------
//
void CNSmlJob::AddTaskIdL( const TInt aTaskId ) 
    {
    iTaskIds.AppendL( aTaskId );
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetOwnerSession( CNSmlSOSSession* aSession ) 
// --------------------------------------------------------------------------
//
void CNSmlJob::SetOwnerSession( CNSmlSOSSession* aSession ) 
    {
    iSession = aSession; 
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetJobRunning( const TBool aRunning )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetJobRunning( const TBool aRunning ) 
    {
    iRunning = aRunning; 
    }

// --------------------------------------------------------------------------
// void CNSmlJob::SetJobIapId( const TInt aIapId )
// --------------------------------------------------------------------------
//
void CNSmlJob::SetJobIapId(const TInt aIapId)
    {
    iIapId = aIapId;
    }

// --------------------------------------------------------------------------
// TInt CNSmlJob::GetJobIapId()
// --------------------------------------------------------------------------
//
TInt CNSmlJob::GetJobIapId() const
    {
    return iIapId;
    }

//  End of File
