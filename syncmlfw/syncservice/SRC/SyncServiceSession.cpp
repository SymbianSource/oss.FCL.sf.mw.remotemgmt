/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server side DS and DM sync services
*
*/


// INCLUDE FILES

#include <SyncServiceSession.h>
#include <SyncServiceParams.h>
#include "SyncServiceDebug.h"

#include <eikenv.h>
#include <eikappui.h>
#include <s32mem.h>

const TInt KMemoryStoreBuffSize = 127;

// ----------------------------------------------------------------------------
// CSyncServiceSession::CSyncServiceSesson
// ----------------------------------------------------------------------------
//
EXPORT_C CSyncServiceSession::CSyncServiceSession( )
: iDoc( CEikonEnv::Static()->EikAppUi()->Document() )
	{
    FLOG( _L( "[CSyncServiceSession] CSyncServiceSession" ) );
    }

// ----------------------------------------------------------------------------
// CSyncServiceSession::~CSyncServiceSesson
// ----------------------------------------------------------------------------
//
CSyncServiceSession::~CSyncServiceSession()
	{
	FLOG( _L( "[CSyncServiceSession] ~CSyncServiceSession" ) );
	}

// ----------------------------------------------------------------------------
// CSyncServiceSession::CreateL
// ----------------------------------------------------------------------------
//
void CSyncServiceSession::CreateL()
	{
	FLOG( _L( "[CSyncServiceSession] CreateL" ) );
	
	CAknAppServiceBase::CreateL();		
	}
	
// ----------------------------------------------------------------------------
// CSyncServiceSession::ServiceL
// ----------------------------------------------------------------------------
//
void CSyncServiceSession::ServiceL( const RMessage2& aMessage )
	{
	FLOG( _L( "[CSyncServiceSession] ServiceL" ) );
	
	switch( aMessage.Function() )
		{
		case ESyncServiceCommand:
			OpenEmbeddedL( aMessage );
			break;
		default:
			CAknAppServiceBase::ServiceL( aMessage );
			break;
		}		
	}
	
// ----------------------------------------------------------------------------
// CSyncServiceSession::ServiceError
// ----------------------------------------------------------------------------
//
void CSyncServiceSession::ServiceError( const RMessage2& aMessage, TInt aError )
	{
	FLOG( _L( "[CSyncServiceSession] ServiceError" ) );
	
	CAknAppServiceBase::ServiceError( aMessage, aError );		
	}
	
// ----------------------------------------------------------------------------
// CSyncServiceSession::OpenEmbeddedL
// ----------------------------------------------------------------------------
//
void CSyncServiceSession::OpenEmbeddedL( const RMessage2& aMessage )
	{
	FLOG( _L( "[CSyncServiceSession] OpenEmbeddedL" ) );

	TSyncParameters params;
	params.iCommand = aMessage.Int0();
	params.iJobId = aMessage.Int1();
	params.iProfileId = aMessage.Int2();
    params.iSilent = aMessage.Int3();
    CBufStore* store = CBufStore::NewLC( KMemoryStoreBuffSize );
    RStoreWriteStream outStream;
    TStreamId id = outStream.CreateLC( *store );
    params.ExternalizeL( outStream );
    store->CommitL();
    CleanupStack::PopAndDestroy();//outStream
	
    CStreamDictionary* dic = CStreamDictionary::NewLC();
    dic->AssignL( KUidSyncParameterValue, id );
    outStream.CreateLC( *store );
    dic->ExternalizeL( outStream );
    store->CommitL();
    CleanupStack::PopAndDestroy();// outstream

    // Restore the document from this store
    RStoreReadStream readStream;
    readStream.OpenLC( *store, id );
    iDoc->RestoreL( *store, *dic );

    CleanupStack::PopAndDestroy(3);// dic, store, readStream
    aMessage.Complete( KErrNone );	
	}

// End of File
