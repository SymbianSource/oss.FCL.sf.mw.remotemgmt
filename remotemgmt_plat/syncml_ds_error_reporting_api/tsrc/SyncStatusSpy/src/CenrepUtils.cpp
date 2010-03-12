/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

// INCLUDE FILES

#include <bautils.h>				// BaflUtils
#include <utf.h>
#include <centralrepository.h>
#include <NsmlOperatorErrorCRKeys.h>

#include "CenrepUtils.h"

_LIT( KPrivateFile, "C:\\logs\\Sync\\SyncStatus.txt" );

// ================= MEMBER FUNCTIONS =========================================


void CCenrepUtils::WriteLogL( TUint32 aKey, TLogKey aValue  )
	{
	RFs fs;
	User::LeaveIfError( fs.Connect() );  // create connect to fileserver
	
	CleanupClosePushL( fs );
	RFile privateFile;
	CleanupClosePushL( privateFile );

	TInt ret = privateFile.Open( fs, KPrivateFile, EFileShareExclusive|EFileWrite ); // open file
	if( ret == KErrNotFound )  // if file does not exist, create it
		{
		privateFile.Create( fs, KPrivateFile, EFileShareExclusive|EFileWrite );
		}

	TInt value = 0;
    CRepository* rep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    rep->Get( aKey, value );
    delete rep;
	HBufC8* dateBuf8 = HBufC8::NewLC( 64 );
	TPtr8 ptrDateBuf8 = dateBuf8->Des();
	HBufC8* timeBuf8 = HBufC8::NewLC( 64 );
	TPtr8 ptrTimeBuf8 = timeBuf8->Des();

	GetDateAndTimeL( ptrDateBuf8, ptrTimeBuf8 );
	HBufC8* buf8 = HBufC8::NewLC( 512 );
	TPtr8 ptrBuf8 = buf8->Des();

	if( aValue == EErrorCode )
		{
		ptrBuf8.Format( _L8("[%S ] [ErrorCode: %d]\r\n"), &ptrTimeBuf8, value );
		}
	else if( aValue == EProfileId )
		{
		ptrBuf8.Format( _L8("[%S ] [SyncProfileId: %d]\r\n"), &ptrTimeBuf8, value );
		}
	else if( aValue == ESyncType )
        {
        ptrBuf8.Format( _L8("[%S ] [SyncType: %d]\r\n"), &ptrTimeBuf8, value );
        }
	else if( aValue == ESyncInitiation )
		{
		ptrBuf8.Format( _L8("[%S ] [SyncInitiation: %d]\r\n"), &ptrTimeBuf8, value );
		}

	TInt size = 0;
	privateFile.Size( size );
	privateFile.Write( size, ptrBuf8 );

	CleanupStack::PopAndDestroy( 5, &fs );//privateFile, dateBuf8, timeBuf8, buf8
    }

void CCenrepUtils::GetDateAndTimeL( TDes8& aDateBuffer, TDes8& aTimeBuffer )
	{
	TTime time;
	time.HomeTime();
	HBufC* dateBuffer = HBufC::NewLC( 64 );
	TPtr ptrDateBuffer = dateBuffer->Des();
	HBufC* timeBuffer = HBufC::NewLC( 64 );
	TPtr ptrTimeBuffer = timeBuffer->Des();
	time.FormatL( ptrDateBuffer, _L( "%D%M%Y%/0%1%/1%2%/2%3%/3" ) );
	time.FormatL( ptrTimeBuffer, _L( "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" ) );
	CnvUtfConverter::ConvertFromUnicodeToUtf8( aDateBuffer, ptrDateBuffer );
	CnvUtfConverter::ConvertFromUnicodeToUtf8( aTimeBuffer, ptrTimeBuffer );
	CleanupStack::PopAndDestroy( 2 ); // dateBuffer, timeBuffer
	}

void CCenrepUtils::CreateSyncLogDirL()
	{
	RFs fs;
	User::LeaveIfError( fs.Connect() );  // create connect to fileserver
	CleanupClosePushL( fs );
	if( !BaflUtils::FolderExists( fs, _L( "C:\\logs\\Sync\\" ) ) )
        {
        fs.MkDirAll( _L( "C:\\logs\\Sync\\" ) );
        }
	CleanupStack::PopAndDestroy();
	}

void CCenrepUtils::ClearSyncLogL()
	{
	RFs fs;
	User::LeaveIfError( fs.Connect() );  // create connect to fileserver
	CleanupClosePushL( fs );
	RFile privateFile;
	CleanupClosePushL( privateFile );
	privateFile.Replace( fs, KPrivateFile, EFileShareExclusive|EFileWrite );
	CleanupStack::PopAndDestroy( 2 );
	}
// End of File
