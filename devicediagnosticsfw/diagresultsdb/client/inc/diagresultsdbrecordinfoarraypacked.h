/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains functions to pack or unpack test record info array.
*  libraries   : DiagResultsDatabase.lib DiagResultsDatabase.dll
*
*/


#ifndef DIAGNOSTICS_RESULTS_DB_RECORD_INFO_ARRAY_PACKED_H
#define DIAGNOSTICS_RESULTS_DB_RECORD_INFO_ARRAY_PACKED_H

#include "diagresultsdatabasetestrecordinfo.h"

class CBufFlat;

/**
* Pack or unpack a CBufFlat buffer. This can be used to deliver data
* across process boundaries using a flat buffer.
*
* @since S60 v5.0
**/
class TDiagResultsDbRecordInfoArrayPacked
    {
public:

    /**
    * Constructor
    * @param aBuffer Flat buffer where the array of test records is written.
    **/
    IMPORT_C TDiagResultsDbRecordInfoArrayPacked(CBufFlat*& aBuffer);

    /**
    * Packs a test record into the flat buffer.
    * @param aInfoArray An array of test records that are written into the buffer.
    **/
    IMPORT_C void PackArrayL(const CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray);
    
    /**
    * Unpack array from the buffer.
    * @param aInfoArray Contains the unpacked array after function finishes.
    **/
    IMPORT_C void UnpackArrayL(CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray);
        
private:

    // Buffer that contains data.
    CBufFlat *& iBuffer;
    
    };
    
#endif