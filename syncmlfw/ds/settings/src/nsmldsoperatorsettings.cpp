/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DS Operator Settings
*
*/

// INTERNAL INCLUDES
#include "nsmldsoperatorsettings.h"

// EXTERNAL INCLUDES
#include <centralrepository.h>      //CRepository
#include <nsmloperatordatacrkeys.h> // KCRUidOperatorDatasyncInternalKeys

// CONSTANTS
const TInt KMaxLength = 256;
const TInt KBufferLength = 256;
const TInt KValueLength = 50;
_LIT( KSpace, " " );

//-----------------------------------------------------------------------------
//      CNSmlDSOperatorSettings::NewL
//-----------------------------------------------------------------------------
EXPORT_C CNSmlDSOperatorSettings* CNSmlDSOperatorSettings::NewL()
    {
    CNSmlDSOperatorSettings* self = CNSmlDSOperatorSettings::NewLC();
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
//      CNSmlDSOperatorSettings::NewLC
//-----------------------------------------------------------------------------
EXPORT_C CNSmlDSOperatorSettings* CNSmlDSOperatorSettings::NewLC()
    {
    CNSmlDSOperatorSettings* self = new( ELeave ) CNSmlDSOperatorSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//-----------------------------------------------------------------------------
//      CNSmlDSOperatorSettings::ConstructL
//-----------------------------------------------------------------------------
void CNSmlDSOperatorSettings::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys );
    }

//-----------------------------------------------------------------------------
//      CNSmlDSOperatorSettings::~CNSmlDSOperatorSettings
//-----------------------------------------------------------------------------
EXPORT_C CNSmlDSOperatorSettings::~CNSmlDSOperatorSettings()
    {
    delete iRepository;
    }

//-----------------------------------------------------------------------------
//      CNSmlDSOperatorSettings::CNSmlDSOperatorSettings
//-----------------------------------------------------------------------------
CNSmlDSOperatorSettings::CNSmlDSOperatorSettings()
    {
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings::IsOperatorProfileL
// Is current profile Operator specific profile.
//-----------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSOperatorSettings::IsOperatorProfileL( 
    const TDesC& aServerId )
    {
    if( aServerId.Length() != 0 )
        {
        TBuf8< KMaxLength > value;
        TBuf< KMaxLength > valueUtf16;
        
        iRepository->Get( KNsmlOpDsOperatorSyncServerId, value );
        valueUtf16.Copy( value );
        if ( aServerId.Compare( valueUtf16 ) == 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings::CustomSwvValueLC
// SW version of Operator specific profile.
//-----------------------------------------------------------------------------
EXPORT_C HBufC8* CNSmlDSOperatorSettings::CustomSwvValueLC()
    {
    return GetValueLC( KNsmlOpDsDevInfoSwVValue );
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings:::CustomModValueLC
// Mod value of Operator specific profile.
//-----------------------------------------------------------------------------
EXPORT_C HBufC8* CNSmlDSOperatorSettings::CustomModValueLC()
    {
    return GetValueLC( KNsmlOpDsDevInfoModValue );
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings:::CustomManValueLC
// Man value of Operator specific profile.
//-----------------------------------------------------------------------------
EXPORT_C HBufC8* CNSmlDSOperatorSettings::CustomManValueLC()
    {
    return GetValueLC( KNsmlOpDsDevInfoManValue );
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings:::PopulateStatusCodeListL
// Populates server status code list
//----------------------------------------------------------------------------
EXPORT_C void CNSmlDSOperatorSettings::PopulateStatusCodeListL( 
    RArray< TInt >& aArray )
    {
    InitializeServerStatusCodeListL( aArray );
    }

// ----------------------------------------------------------------------------
// CNSmlDSOperatorSettings::InitializeServerStatusCodeListL
// This function reads list of configured server status codes from cenrep
// ----------------------------------------------------------------------------
void CNSmlDSOperatorSettings::InitializeServerStatusCodeListL( 
    RArray< TInt >& aArray )
    {    
    TBuf< KBufferLength > tempStr;
    iRepository->Get( KNsmlOpDsSyncMLStatusCodeList, tempStr );
    TInt commaPos = KErrNotFound;

    while ( ( commaPos = tempStr.Locate( ',' ) ) > KErrNotFound )
        {
        tempStr.Replace( commaPos, 1, KSpace );
        }

    TLex lex( tempStr );
    while( !lex.Eos() )
        {
        TPtrC token = lex.NextToken();
        TLex temp( token );
        TInt val;
        temp.Val( val );      
        aArray.Append( val );
        }
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings:::SyncErrorReportingEnabled
// Returns ETrue if SyncML Error Reporting is enabled
//----------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSOperatorSettings::SyncErrorReportingEnabled() 
    {
    TBool errorReportingEnabled = EFalse;
    iRepository->Get( KNsmlOpDsSyncMLErrorReporting, errorReportingEnabled );
    return errorReportingEnabled;
    }

//-----------------------------------------------------------------------------
// CNSmlDSOperatorSettings:::HttpErrorReportingEnabled
// Returns ETrue if HTTP Error Reporting is enabled
//----------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSOperatorSettings::HttpErrorReportingEnabled() 
    {
    TBool errorReportingEnabled = EFalse;
    iRepository->Get( KNsmlOpDsHttpErrorReporting, errorReportingEnabled );
    return errorReportingEnabled;
    }

// ----------------------------------------------------------------------------
// CNSmlDSOperatorSettings::GetValueLC
// This function Get Repository key value of Operator specific profile.
// ----------------------------------------------------------------------------
HBufC8* CNSmlDSOperatorSettings::GetValueLC( const TUint32 aKey )
    {
    HBufC8* buf = HBufC8::NewLC( KValueLength );
    TPtr8 ptr = buf->Des();

    TInt actualLength;
    TInt err = iRepository->Get( aKey, ptr, actualLength );
    if( err == KErrOverflow )
        {
        CleanupStack::Pop( buf );
        buf = buf->ReAllocL( actualLength );
        CleanupStack::PushL( buf );
        ptr.Set( buf->Des() );
        // ignore return error
        iRepository->Get( aKey, ptr );
        }
    return buf;
    }
//  End of File
