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
* Description:  Class definition of CDiagResultsDbRecordEngineParam
*
*/


#include <s32std.h> 

#include "diagresultsdbrecordengineparam.h"


// ---------------------------------------------------------------------------
// NewL.Used when creating the object from a stream that contains the
// serialized object.
// ---------------------------------------------------------------------------
//        
EXPORT_C CDiagResultsDbRecordEngineParam* CDiagResultsDbRecordEngineParam::NewL (
                                        RReadStream& aStream )
    {
    CDiagResultsDbRecordEngineParam* self = 
                     new( ELeave ) CDiagResultsDbRecordEngineParam();
    
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//  
EXPORT_C CDiagResultsDbRecordEngineParam* CDiagResultsDbRecordEngineParam::NewL
                                            ( 
                                            RArray<TUid>*   aInitialUids,
                                            TBool           aDependencyExecution
                                            )
    {
    CDiagResultsDbRecordEngineParam* self = 
                     new( ELeave ) CDiagResultsDbRecordEngineParam( 
                                                        aInitialUids, 
                                                        aDependencyExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    


// ---------------------------------------------------------------------------
// NewLC.
// ---------------------------------------------------------------------------
//   
EXPORT_C CDiagResultsDbRecordEngineParam* CDiagResultsDbRecordEngineParam::NewLC (
                                          RArray<TUid>*   aInitialUids,
                                          TBool           aDependencyExecution
                                          )
    {
    CDiagResultsDbRecordEngineParam* self = 
                                new( ELeave ) CDiagResultsDbRecordEngineParam( 
                                        aInitialUids, 
                                        aDependencyExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ConstructL.
// ---------------------------------------------------------------------------
//
void CDiagResultsDbRecordEngineParam::ConstructL ( RReadStream& aStream )
    {
    InternalizeL( aStream );
    }
    
// ---------------------------------------------------------------------------
// ConstructL.
// ---------------------------------------------------------------------------
//
void CDiagResultsDbRecordEngineParam::ConstructL ()
    {
    if ( !iExecutionUids )
        {
        User::Leave( KErrArgument );
        }
    
    }
    
  
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDbRecordEngineParam::~CDiagResultsDbRecordEngineParam()
    {
    iExecutionUids->Close();
    delete iExecutionUids;
    iExecutionUids = NULL;
    }

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
//    
CDiagResultsDbRecordEngineParam::CDiagResultsDbRecordEngineParam()
    {
    
    }

// ---------------------------------------------------------------------------
// Return execution uid array.
// ---------------------------------------------------------------------------
//   
EXPORT_C const RArray<TUid>& CDiagResultsDbRecordEngineParam::ExecutionsUidArray() const
    {
    return *iExecutionUids;
    }
    
// ---------------------------------------------------------------------------
// Return dependency execution.
// ---------------------------------------------------------------------------
//       
EXPORT_C TBool CDiagResultsDbRecordEngineParam::DependencyExecution() const
    {
    return iDependencyExecution;
    }
    
// ---------------------------------------------------------------------------
// Externalize this object into a stream.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CDiagResultsDbRecordEngineParam::ExternalizeL(
                                             RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iExecutionUids->Count() );
    
    for (TInt i=0; i < iExecutionUids->Count(); ++i )
        {
        aStream.WriteInt32L( (*iExecutionUids)[i].iUid );
        }
        
    aStream.WriteUint8L ( iDependencyExecution );   
    }


// ---------------------------------------------------------------------------
// Internalize the object from a stream.
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbRecordEngineParam::InternalizeL( RReadStream& aStream )
    {
    iExecutionUids = new (ELeave) RArray<TUid>(10);
    
    TInt length = aStream.ReadInt16L();
    
    for ( TInt i=0; i < length; ++i )
        {
        
        iExecutionUids->AppendL( TUid::Uid( aStream.ReadInt32L() ) );
        }
    
    iDependencyExecution = aStream.ReadUint8L();     
    }


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CDiagResultsDbRecordEngineParam::CDiagResultsDbRecordEngineParam (
                                RArray<TUid>*   aInitialUids,
                                TBool           aDependencyExecution )
    {
    iExecutionUids = aInitialUids;
    iDependencyExecution = aDependencyExecution;
    }
    
  
    

