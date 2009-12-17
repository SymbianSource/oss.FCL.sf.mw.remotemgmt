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
#include "nsmlroam.h"
#include "NSmlTransportHandler.h"

// --------------------------------------------------------------------------
// CNSmlConnection::CNSmlConnection( const TInt aProfId, const TInt aTransId )
// --------------------------------------------------------------------------
//
CNSmlConnection::CNSmlConnection( const TInt aProfId, const TInt aTransId, CNSmlSOSSession& aSession ) 
: iProfId(aProfId), iTransId(aTransId), iSession(aSession), iDataPtr(NULL, 0, 0)  
    {    
    }

// --------------------------------------------------------------------------
// CNSmlConnection::~CNSmlConnection()
// --------------------------------------------------------------------------
//
CNSmlConnection::~CNSmlConnection()
    {
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// TInt CNSmlConnection::FetchDataL()
// --------------------------------------------------------------------------
//
TInt CNSmlConnection::FetchDataL()
    {
    TInt ret(KErrNotFound);
    if ( iProfId < KMaxDataSyncID ) // DS Profile
        { 
        CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
        if ( prof )
            {
            CleanupStack::PushL(prof);

            // Check if transport id differs from one saved to profile.
            if ( iTransId != prof->IntValue( EDSProfileTransportId ) )
                {
                CleanupStack::PopAndDestroy(prof);
                return KErrNotFound;
                }

            // String lengths. (+1 is because of zero-length strings which are added to streams also).
            TInt urllen = prof->StrValue( EDSProfileServerURL ).Size();
            TInt unlen = prof->StrValue( EDSProfileHttpAuthUsername ).Size();
            TInt pwlen = prof->StrValue( EDSProfileHttpAuthPassword ).Size();

            CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	        CleanupStack::PushL( buffer );
	        RBufWriteStream stream( *buffer );  
            CleanupClosePushL(stream);

            stream.WriteInt32L( urllen );
            stream << prof->StrValue( EDSProfileServerURL );

            stream.WriteInt32L( prof->IntValue(EDSProfileIAPId ) );

            CNSmlRoamTable* roamTable = CNSmlRoamTable::NewL( *iSession.DSSettings().Database() );
            CleanupStack::PushL( roamTable );

            CNSmlIAPArray* array = roamTable->GetItemsForIdL( iProfId );
            CleanupStack::PushL( array );
            
            for ( TInt i(0) ; i < array->Count() ; i++ )
                {
                stream.WriteInt32L( array->At(i) );   
                }
            
            TInt iapCount = array->Count() + 1;

            while ( iapCount < KMaxIAPCount )
                {
                stream.WriteInt32L(0); // write zero values to IAP ids that doesn't exist.
                iapCount++;
                }

            CleanupStack::PopAndDestroy(array); 
            CleanupStack::PopAndDestroy(roamTable);

            stream.WriteInt32L( prof->IntValue(EDSProfileAutoChangeIAP ) );
            
            stream.WriteInt32L( prof->IntValue(EDSProfileHttpAuthUsed ) );

            stream.WriteInt32L( unlen );
            stream << prof->StrValue( EDSProfileHttpAuthUsername );

            stream.WriteInt32L( pwlen );
            stream << prof->StrValue( EDSProfileHttpAuthPassword );

            stream.CommitL();

            CleanupStack::PopAndDestroy(&stream);
            
            delete iBuffer;
	        iBuffer = buffer;
	        CleanupStack::Pop( buffer );    
            
            CleanupStack::PopAndDestroy(prof);
            ret = KErrNone;
            }
        }
    else    // DM Profile
        {
        CNSmlDMProfile* prof = iSession.DMSettings().ProfileL( iProfId );
        if ( prof )
            {
            CleanupStack::PushL(prof);        

            if ( iTransId != prof->IntValue( EDMProfileTransportId ) )
                {
                CleanupStack::PopAndDestroy(prof);
                return KErrNotFound;
                }

            TInt urllen = prof->StrValue( EDMProfileServerURL ).Size();
			TInt unlen = prof->StrValue( EDMProfileHttpAuthUsername ).Size();
            TInt pwlen = prof->StrValue( EDMProfileHttpAuthPassword ).Size();
     
            CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	        CleanupStack::PushL( buffer );
	        RBufWriteStream stream( *buffer );  
	        
            CleanupClosePushL(stream);

            stream.WriteInt32L( urllen );
            stream << prof->StrValue( EDMProfileServerURL );

            stream.WriteInt32L( prof->IntValue( EDMProfileIAPId ) );

			stream.WriteInt32L( prof->IntValue(EDMProfileHttpAuthUsed ) );

            stream.WriteInt32L( unlen );
            stream << prof->StrValue( EDMProfileHttpAuthUsername );

            stream.WriteInt32L( pwlen );
            stream << prof->StrValue( EDMProfileHttpAuthPassword );

            stream.CommitL();            

            CleanupStack::PopAndDestroy(&stream);
            
            delete iBuffer;
	        iBuffer = buffer;
	        CleanupStack::Pop( buffer );    
	        
            CleanupStack::PopAndDestroy(prof);
            ret = KErrNone;
            }
        }    
    return ret;
    }

// --------------------------------------------------------------------------
// TInt CNSmlConnection::DataSize()
// --------------------------------------------------------------------------
//
TInt CNSmlConnection::DataSize()
    {
    return iBuffer->Size();
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlConnection::ReadData()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlConnection::ReadData()
    {
    if ( iBuffer )
        {
        iDataPtr.Set(iBuffer->Ptr(0));    
        }    
    return iDataPtr;
    }

// --------------------------------------------------------------------------
// TInt CNSmlConnection::UpdateDataL( const TDesC8& aData )
// --------------------------------------------------------------------------
//
TInt CNSmlConnection::UpdateDataL( const TDesC8& aData )
    {
    TInt ret(KErrNotFound);
    if ( iProfId < KMaxDataSyncID ) // DS Profile
        {        
        CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
        CleanupStack::PushL(prof);

        RDesReadStream stream;
        stream.Open(aData);
        CleanupClosePushL(stream);
       
        TInt maxlen(0);
        TInt pc(0);

        // Transport Id
        TInt transportId = stream.ReadInt32L();
        prof->SetIntValue( EDSProfileTransportId, transportId );
        
        CNSmlTransportHandler* tHandler = CNSmlTransportHandler::NewL( ESmlDataSync );
        CleanupStack::PushL(tHandler);

        tHandler->CreateL( transportId );
        CSyncMLTransportPropertiesArray& propArray = tHandler->Array();

        CNSmlIAPArray* array = new (ELeave) CArrayFixFlat<TUint32>(1);
        CleanupStack::PushL( array );

        // Server URI
        maxlen = stream.ReadInt32L();
        prof->SetStrValue( EDSProfileServerURL, HBufC::NewLC(stream,maxlen)->Des() ); pc++;

        for ( TInt i(0) ; i < propArray.Count() ; i++ )
            {
            if ( !propArray.At(i).iName.Compare( KNSmlIAPId() ) )
                {            
                prof->SetIntValue( EDSProfileIAPId, stream.ReadInt32L() );
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlIAPId2() ) )
                {
                TInt IAPId2 = stream.ReadInt32L();	
                if ( IAPId2 ) // Add IAP id only if it has a value.
                    {
                    array->AppendL( IAPId2 ); 
                    }   
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlIAPId3() ) )
                {
                TInt IAPId3 = stream.ReadInt32L();	
                if ( IAPId3 ) // Add IAP id only if it has a value.
                    {
                    array->AppendL( IAPId3 ); 
                    }   
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlIAPId4() ) )
                {
                TInt IAPId4 = stream.ReadInt32L();	
                if ( IAPId4 ) // Add IAP id only if it has a value.
                    {
                    array->AppendL( IAPId4 ); 
                    }   
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlUseRoaming() ) )
                {
                prof->SetIntValue( EDSProfileAutoChangeIAP, stream.ReadInt32L() );
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPAuth() ) )
                {
                prof->SetIntValue( EDSProfileHttpAuthUsed, stream.ReadInt32L() );
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPUsername() ) )
                {
                maxlen = stream.ReadInt32L();
                prof->SetStrValue( EDSProfileHttpAuthUsername, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPPassword() ) )
                {
                maxlen = stream.ReadInt32L();
                prof->SetStrValue( EDSProfileHttpAuthPassword, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
                }
            }
    
        if ( array->Count() ) // Check if array has values.
            {
            CNSmlRoamTable* roamTable = CNSmlRoamTable::NewL( *iSession.DSSettings().Database() );
            CleanupStack::PushL( roamTable );

            roamTable->PutItemsL( iProfId, array );
            CleanupStack::PopAndDestroy(roamTable); 
            }
       
        ret = prof->SaveL();
    
        CleanupStack::PopAndDestroy(pc);
        CleanupStack::PopAndDestroy(array); 
        CleanupStack::PopAndDestroy(tHandler); 
        CleanupStack::PopAndDestroy(&stream);
        CleanupStack::PopAndDestroy(prof);

        }
    else    // DM Profile
        {
        CNSmlDMProfile* prof = iSession.DMSettings().ProfileL( iProfId );
        CleanupStack::PushL(prof);

        RDesReadStream stream;
        stream.Open(aData);
        CleanupClosePushL(stream);
       
        TInt maxlen(0);
        TInt pc(0);

        // Transport Id
        TInt transportId = stream.ReadInt32L();

        prof->SetIntValue( EDMProfileTransportId, transportId );

        CNSmlTransportHandler* tHandler = CNSmlTransportHandler::NewL( ESmlDevMan );
        CleanupStack::PushL(tHandler);
        tHandler->CreateL( transportId );
        CSyncMLTransportPropertiesArray& propArray = tHandler->Array();

        // Server URI
        maxlen = stream.ReadInt32L();
        prof->SetStrValue( EDMProfileServerURL, HBufC::NewLC(stream,maxlen)->Des() ); pc++;

        for ( TInt i(0) ; i < propArray.Count() ; i++ )
            {
            if ( !propArray.At(i).iName.Compare( KNSmlIAPId() ) )
                {            
                prof->SetIntValue( EDMProfileIAPId, stream.ReadInt32L() );
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPAuth() ) )
                {
                prof->SetIntValue( EDMProfileHttpAuthUsed, stream.ReadInt32L() );
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPUsername() ) )
                {
                maxlen = stream.ReadInt32L();
                prof->SetStrValue( EDMProfileHttpAuthUsername, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
                }
            else if ( !propArray.At(i).iName.Compare( KNSmlHTTPPassword() ) )
                {
                maxlen = stream.ReadInt32L();
                prof->SetStrValue( EDMProfileHttpAuthPassword, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
                }
            }
        ret = prof->SaveL();

        CleanupStack::PopAndDestroy(pc);
        CleanupStack::PopAndDestroy(tHandler); 
        CleanupStack::PopAndDestroy(&stream);
        CleanupStack::PopAndDestroy(prof);
        }   
    return ret;
    }
