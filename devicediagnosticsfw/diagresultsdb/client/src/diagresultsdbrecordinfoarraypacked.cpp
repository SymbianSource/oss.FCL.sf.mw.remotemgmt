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


// SYSTEM INCLUDE FILES
#include <s32strm.h>
#include <s32mem.h>

// USER INCLUDE FILES
#include "diagresultsdbrecordinfoarraypacked.h"
#include "diagresultsdatabasetestrecordinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TDiagResultsDbRecordInfoArrayPacked::TDiagResultsDbRecordInfoArrayPacked(
                                         CBufFlat*& aBuffer): iBuffer(aBuffer)
    {
    
    }

// ---------------------------------------------------------------------------
// Packs the array for sending across IPC. Flat buffer is used to deliver data.
// ---------------------------------------------------------------------------
//
EXPORT_C void TDiagResultsDbRecordInfoArrayPacked::PackArrayL(
        const CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray)
    {   
    RBufWriteStream writeStream ( *iBuffer );
    CleanupClosePushL( writeStream );
    writeStream.WriteInt16L( aInfoArray.Count() );
    
    for (TInt i=0; i < aInfoArray.Count(); ++i )
        {
        aInfoArray[i].ExternalizeL( writeStream );
        }
        
    CleanupStack::PopAndDestroy( &writeStream );
    }

// ---------------------------------------------------------------------------
// Unpack the array. 
// ---------------------------------------------------------------------------
//    
EXPORT_C void TDiagResultsDbRecordInfoArrayPacked::UnpackArrayL(
            CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray)
    {
    RBufReadStream readStream ( *iBuffer );
    CleanupClosePushL( readStream );
    
    //how many cells needs to be unpacked
    TInt length = readStream.ReadInt16L();
    
    for ( TInt i=0; i < length; ++i )
        {
        TDiagResultsDatabaseTestRecordInfo info = 
                    TDiagResultsDatabaseTestRecordInfo(); 
        info.InternalizeL( readStream );
        aInfoArray.AppendL( info ) ;
        }
    CleanupStack::PopAndDestroy( &readStream );
    }
