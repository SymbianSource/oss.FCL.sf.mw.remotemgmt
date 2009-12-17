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
* Description:  Symbian OS Server source.
*
*/


#include "nsmlsosserver.h"
#include "nsmldshostclient.h"

// --------------------------------------------------------------------------
// CNSmlDataProvider::CNSmlDataProvider( const TInt aId )
// --------------------------------------------------------------------------
//
CNSmlDataProvider::CNSmlDataProvider( const TInt aId, CNSmlSOSSession& aSession ) 
: iId(aId), iDataPtr(NULL, 0, 0), iSession(aSession) 
    {    
    }

// --------------------------------------------------------------------------
// CNSmlDataProvider::~CNSmlDataProvider()
// --------------------------------------------------------------------------
//
CNSmlDataProvider::~CNSmlDataProvider()
    {
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// void CNSmlDataProvider::FetchDataL()
// --------------------------------------------------------------------------
//
void CNSmlDataProvider::FetchDataL()
    {

    RArray<TInt> idArray;
    RArray<TInt> resultArray;
    CleanupClosePushL(resultArray);
    CleanupClosePushL(idArray);
    idArray.AppendL(iId);
    
    iSession.HostClient().CreateDataProvidersL( idArray, resultArray );

    TInt res = resultArray[0];
    User::LeaveIfError( res );

    CleanupStack::PopAndDestroy(&idArray);
    CleanupStack::PopAndDestroy(&resultArray);

	TInt err = KErrNone;
	TNSmlDPInformation* dpInfo = iSession.HostClient().DataProviderInformationL( iId, err );
    User::LeaveIfError( err );
    
    HBufC* defds = iSession.HostClient().DefaultStoreL( iId, err );
    User::LeaveIfError( err );
    
    CDesCArray* dsArray = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(dsArray);

    iSession.HostClient().ListStoresL( dsArray, iId, err );
    User::LeaveIfError( err );
    
    TInt mimeCount = dpInfo->iMimeTypes->Count();
    
    TInt mimelen = 0;
    TInt mverlen = 0;
    for (TInt mime = 0; mime < mimeCount; ++mime )
    	{
    	mimelen += ( *dpInfo->iMimeTypes )[ mime ].Size(); // mime type text length
    	mimelen += KSizeofTInt32; // mime type length value
    	mverlen += ( *dpInfo->iMimeVersions )[ mime ].Size(); // mime version text length
    	mverlen += KSizeofTInt32; // mime version length value
    	}
    	
    TInt dnamlen = dpInfo->iDisplayName->Size();
    TInt defdslen = defds->Size();

    CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( buffer );
	RBufWriteStream stream( *buffer );  
    
    CleanupClosePushL(stream); 
 
    stream.WriteInt32L( dpInfo->iVersion.iMajor );    // Version major
    stream.WriteInt32L( dpInfo->iVersion.iMinor );    // Version minor
    stream.WriteInt32L( dpInfo->iVersion.iBuild );    // Version build
    stream.WriteInt8L( (TInt8)dpInfo->iProtocolVer ); // Req protocol version

    // display name
    stream.WriteInt32L( dnamlen );
    stream << dpInfo->iDisplayName->Des();

	// mime type count
    stream.WriteInt32L( mimeCount );
	
    // mime types
    for ( TInt type = 0; type < mimeCount; ++type )
    	{
        stream.WriteInt32L( ( *dpInfo->iMimeTypes )[ type ].Size() );
        stream << ( *dpInfo->iMimeTypes )[ type ];
    	}

    // mime versions
    for ( TInt ver = 0; ver < mimeCount; ++ver )
    	{
        stream.WriteInt32L( ( *dpInfo->iMimeVersions )[ ver ].Size() );
        stream << ( *dpInfo->iMimeVersions )[ ver ];
    	}

    TInt count = dsArray->Count() + 1; // default ds + others.

    stream.WriteInt8L( count ); // Amount of datastores.

    // default datastore
    stream.WriteInt32L( defdslen );   
    stream << defds->Des();
   
    for ( TInt i(0) ; i < dsArray->Count() ; i++ )
        {
            stream.WriteInt32L( dsArray->operator[](i).Size() ); 
            stream << dsArray->operator[](i);
        }

    // Allow multible datastores
    stream.WriteInt8L( iSession.HostClient().SupportsOperationL( KUidSmlSupportMultipleStores, iId, err ) ); 
    User::LeaveIfError( err );

    stream.CommitL();
    
    CleanupStack::PopAndDestroy(&stream);
    delete iBuffer;
	iBuffer = buffer;
	CleanupStack::Pop( buffer );    
    CleanupStack::PopAndDestroy(dsArray);
    
    delete defds;
    delete dpInfo->iDisplayName;
    delete dpInfo->iMimeTypes;
    delete dpInfo->iMimeVersions;
    delete dpInfo;
    }

// --------------------------------------------------------------------------
// TInt CNSmlDataProvider::DataSize()
// --------------------------------------------------------------------------
//
TInt CNSmlDataProvider::DataSize()
    {
    return iBuffer->Size();
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlDataProvider::ReadData()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlDataProvider::ReadData()
    {
    if ( iBuffer )
        {
        iDataPtr.Set( iBuffer->Ptr(0) );    
        }    
    return iDataPtr;
    }
