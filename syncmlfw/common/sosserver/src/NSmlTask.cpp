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


#include "nsmlagentlog.h"
#include "nsmlsosserver.h"
#include "nsmldshostclient.h"

// --------------------------------------------------------------------------
// CNSmlTask::CNSmlTask( const TInt aProfId, TBool aNewTask )
// --------------------------------------------------------------------------
//
CNSmlTask::CNSmlTask( const TInt aProfId, TBool aNewTask, CNSmlSOSSession& aSession) 
: iProfId(aProfId), iCreatingNew(aNewTask), iSession(aSession), iDataPtr(NULL, 0, 0), iFilterBufPtr(NULL, 0, 0) 
    {    
    }

// --------------------------------------------------------------------------
// CNSmlTask::~CNSmlTask()
// --------------------------------------------------------------------------
//
CNSmlTask::~CNSmlTask()
    {
    delete iBuffer;
   	delete iFilterBuffer;
    }

// --------------------------------------------------------------------------
// void CNSmlTask::SetDataProviderId(  const TInt aTaskUID )
// --------------------------------------------------------------------------
//
void CNSmlTask::SetDataProviderId(  const TInt aTaskUID )
    {
    iTaskUID = aTaskUID;
    }


// --------------------------------------------------------------------------
// void CNSmlTask::SetCreatorId( const TInt aCreatorId )
// --------------------------------------------------------------------------
//
void CNSmlTask::SetCreatorId( const TInt aCreatorId )
    {
    iCreatorId = aCreatorId;
    }

// --------------------------------------------------------------------------
// TInt CNSmlTask::FetchDataL( const TInt aTaskId )
// --------------------------------------------------------------------------
//
TInt CNSmlTask::FetchDataL( const TInt aTaskId )
    {
    TInt ret(KErrNotFound);
    CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
    if ( prof )
        {
        CleanupStack::PushL(prof);

        CNSmlDSContentType* ctype = prof->ContentTypeId( aTaskId ); // Table id used when fetching data.
        // content type is deleted in CNSmlDSProfile destructor.
        if ( ctype )
            {
            TInt namelen = ctype->StrValue( EDSAdapterDisplayName ).Size();
            TInt rdblen =  ctype->StrValue( EDSAdapterServerDataSource ).Size();
            TInt ldblen =  ctype->StrValue( EDSAdapterClientDataSource ).Size();
            
            CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	        CleanupStack::PushL( buffer );
	        RBufWriteStream stream( *buffer );  
	        
            CleanupClosePushL(stream);

            stream.WriteInt32L( rdblen );
            stream << ctype->StrValue( EDSAdapterServerDataSource );
            stream.WriteInt32L( ldblen );
            stream << ctype->StrValue( EDSAdapterClientDataSource );

			iTaskUID = ctype->IntValue( EDSAdapterImplementationId );
            stream.WriteInt32L( iTaskUID );
                        
            stream.WriteInt8L( ctype->IntValue( EDSAdapterEnabled ) );
            stream.WriteInt32L( ctype->IntValue( EDSAdapterCreatorId ));

            stream.WriteInt32L( namelen );
            stream << ctype->StrValue( EDSAdapterDisplayName );

            stream.WriteInt8L( ctype->IntValue( EDSAdapterSyncType ) );

            CleanupStack::PopAndDestroy(&stream);
            
            delete iBuffer;
	        iBuffer = buffer;
	        CleanupStack::Pop( buffer );    
	            
            ret = KErrNone;
            }
        CleanupStack::PopAndDestroy(prof);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// TInt CNSmlTask::DataSize()
// --------------------------------------------------------------------------
//
TInt CNSmlTask::DataSize()
    {
    return iBuffer->Size();
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlTask::ReadData()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlTask::ReadData()
    {
    if ( iBuffer )
        {
        iDataPtr.Set( iBuffer->Ptr(0) );    
        }    
    return iDataPtr;
    }

// --------------------------------------------------------------------------
// TInt CNSmlTask::FilterSize()
// --------------------------------------------------------------------------
//
TInt CNSmlTask::FilterSize()
    {
    if ( iFilterBuffer )
        {
        return iFilterBuffer->Size();    
        }
    return 0;
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlTask::ReadFilter()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlTask::ReadFilter()
    {
    if ( iFilterBuffer )
        {
        iFilterBufPtr.Set(iFilterBuffer->Ptr(0));    
        }    
    return iFilterBufPtr;
    }
    
// --------------------------------------------------------------------------
// TInt CNSmlTask::UpdateDataL( TInt& aId, const TDesC8& aData )
// --------------------------------------------------------------------------
//
TInt CNSmlTask::UpdateDataL( TInt& aId, const TDesC8& aData )
    {
    CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
    CleanupStack::PushL(prof);
   
    RDesReadStream stream;
    stream.Open(aData);
    CleanupClosePushL(stream);
   
    TInt maxlen(0);

    maxlen = stream.ReadInt32L();
    HBufC* serverDS = HBufC::NewLC( stream, maxlen );

    maxlen = stream.ReadInt32L();
    HBufC* clientDS = HBufC::NewLC( stream, maxlen );

    CNSmlDSContentType* ctype;

    if ( iCreatingNew )
        {
        ctype = prof->AddContentTypeL( iTaskUID, *clientDS, *serverDS ); // Implementation UID used when creating/updating task.
    
        if ( !ctype )
        	{
        	User::Leave( KErrAlreadyExists);
        	}
        }
    else
        {
        ctype = prof->ContentType( iTaskUID );
    
        if ( !ctype )
        	{
        	User::Leave( KErrNotFound );
        	}
        }
        
    // Save profile to create id for a new task.
    TInt ret = prof->SaveL();
    
    CleanupStack::PopAndDestroy(clientDS);
    CleanupStack::PopAndDestroy(serverDS);
 
    User::LeaveIfError( ret );
 
    ctype->SetIntValue( EDSAdapterEnabled, stream.ReadInt8L() );
	ctype->SetIntValue( EDSAdapterCreatorId, stream.ReadInt32L() );

    maxlen = stream.ReadInt32L();
    ctype->SetStrValue( EDSAdapterDisplayName, HBufC::NewLC(stream,maxlen)->Des() );    
    CleanupStack::PopAndDestroy(); // display name

    ctype->SetIntValue( EDSAdapterSyncType, stream.ReadInt8L() );
    
    ctype->SetIntValue( EDSAdapterFilterMatchType, stream.ReadInt8L() );    
    
    if ( (TSmlProtocolVersion)prof->IntValue( EDSProfileProtocolVersion ) == ESmlVersion1_2 )  
    	{
    	maxlen = stream.ReadInt32L();
	    RWriteStream filterStream = ctype->FilterWriteStreamL();
	    CleanupClosePushL(filterStream);
	    
	    filterStream.WriteInt32L(stream.ReadInt32L());

	    if ( maxlen )
	    	{
	    	filterStream.WriteL( stream );
	    	}
	            
	    CleanupStack::PopAndDestroy(&filterStream);
	    ctype->WriteStreamCommitL();
    	}

    ret = prof->SaveL();

    aId = ctype->IntValue( EDSAdapterTableId );

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(prof);

    return ret;
    }
    
// --------------------------------------------------------------------------
// TInt CNSmlTask::GetSupportedFiltersL()
// --------------------------------------------------------------------------
//        
TInt CNSmlTask::GetSupportedFiltersL()
	{
    CNSmlDSHostClient& dsClient = iSession.HostClient();    
    
    TInt resultCode(0);
    TSyncMLFilterMatchType matchType = ESyncMLMatchDisabled;
    TSyncMLFilterChangeInfo changeInfo = ESyncMLDefault;

	RPointerArray<CSyncMLFilter>* filterArray;
	
	// Initialize data providers.
    RArray<TInt> idArray;
    RArray<TInt> resultArray;
    CleanupClosePushL(resultArray);
    CleanupClosePushL(idArray);
    idArray.AppendL(iTaskUID);
    
    dsClient.CreateDataProvidersL( idArray, resultArray );

    TInt res = resultArray[0];
    User::LeaveIfError( res );

    CleanupStack::PopAndDestroy(&idArray);
    CleanupStack::PopAndDestroy(&resultArray);
	
	filterArray = dsClient.SupportedServerFiltersL( iTaskUID, matchType, changeInfo, resultCode );

	if ( filterArray )
		{
		CleanupStack::PushL(filterArray);
		PackFilterArrayL( *filterArray, (TInt)matchType, (TInt)changeInfo );
		filterArray->ResetAndDestroy();
		CleanupStack::PopAndDestroy(filterArray);
		}
	else // No filters, write NULL values to the filter buffer.
		{
		    
		CBufFlat* filterBuffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	    CleanupStack::PushL( filterBuffer );
	    RBufWriteStream stream( *filterBuffer );  
	            
	    CleanupClosePushL(stream);
	    
	    stream.WriteInt8L( changeInfo );
	    stream.WriteInt8L( matchType );
	    stream.WriteInt32L( 0 ); // filter count

		CleanupStack::PopAndDestroy(&stream);		
		
		delete iFilterBuffer;
	    iFilterBuffer = filterBuffer;
	    CleanupStack::Pop( filterBuffer );    
		}		

	return resultCode;
	}
	
// --------------------------------------------------------------------------
// void CNSmlTask::PackFilterArrayL( RPointerArray<CSyncMLFilter>* aFilterArray )
// --------------------------------------------------------------------------
// 	
void CNSmlTask::PackFilterArrayL( RPointerArray<CSyncMLFilter>& aFilterArray, TInt aMatchType, TInt aChangeInfo )
	{
	TInt count = aFilterArray.Count();

    CBufFlat* filterBuffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	CleanupStack::PushL( filterBuffer );
	RBufWriteStream stream( *filterBuffer );  
    CleanupClosePushL(stream);
    
    stream.WriteInt8L( aChangeInfo );
    stream.WriteInt8L( aMatchType );
    
    stream.WriteInt32L( count );

	for ( TInt i = 0 ; i < count ; i++ )
		{
		aFilterArray[i]->ExternalizeL( stream );
 		}

	CleanupStack::PopAndDestroy(&stream);
	delete iFilterBuffer;
	iFilterBuffer = filterBuffer;
	CleanupStack::Pop( filterBuffer );  
	}	

// --------------------------------------------------------------------------
// void CNSmlTask::PrepareFilterBufferL()
// --------------------------------------------------------------------------
// 
TInt CNSmlTask::PrepareFilterBufferL( const TInt aTaskId )
    {    
    CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
    if ( prof )
        {
        CleanupStack::PushL(prof);

        CNSmlDSContentType* ctype = prof->ContentTypeId( aTaskId ); // Table id used when fetching data.
        // content type is deleted in CNSmlDSProfile destructor.
        if ( ctype )
            {                                        
			TSyncMLFilterMatchType matchType(ESyncMLMatchDisabled);
			TSyncMLFilterChangeInfo changeInfo(ESyncMLDefault);			
			
    		TInt resultCode(0);
    		
    		// Initialize data providers.
    		RArray<TInt> idArray;
    		RArray<TInt> resultArray;
    		CleanupClosePushL(resultArray);
    		CleanupClosePushL(idArray);
		    idArray.AppendL(iTaskUID);
            
    		iSession.HostClient().CreateDataProvidersL( idArray, resultArray );
            
    		TInt res = resultArray[0];
    		User::LeaveIfError( res );

    		CleanupStack::PopAndDestroy(&idArray);
    		CleanupStack::PopAndDestroy(&resultArray);

			// Filters
			RReadStream filterStream = ctype->FilterReadStreamL();
			CleanupClosePushL( filterStream );
			RPointerArray<CSyncMLFilter>* filterArray = new ( ELeave ) RPointerArray<CSyncMLFilter>();
			CleanupStack::PushL( filterArray );
			TInt count(0);
			TRAPD(error, count = filterStream.ReadInt32L() );
			if ( error == KErrNone )
				{
				for ( TInt i(0) ; i < count ; i++ )
					{
					filterArray->AppendL( CSyncMLFilter::NewLC( filterStream ) );
					CleanupStack::Pop();
					}
				}
			else if ( prof->IntValue( EDSProfileProtocolVersion ) == ESmlVersion1_2 )
				{
				CleanupStack::Pop(); // filterArray;
				filterArray->ResetAndDestroy();
				delete filterArray;
				filterArray = iSession.HostClient().SupportedServerFiltersL( iTaskUID, matchType, changeInfo, resultCode );				
				CleanupStack::PushL( filterArray );
				}
		    
			iSession.HostClient().CheckServerFiltersL( iTaskUID, *filterArray, changeInfo, resultCode );
			
            CNSmlDSAgentLog* agentLog = CNSmlDSAgentLog::NewLC();
						
			TRAPD( err, agentLog->SetAgentLogKeyL( iTaskUID, 
									   			   ctype->StrValue( EDSAdapterClientDataSource ),
									               prof->StrValue( EDSProfileServerId ),
									               ctype->StrValue( EDSAdapterServerDataSource ) ) );
            
            if ( err == KErrNone )
				{
				RPointerArray<CNSmlDbCaps> dbCaps;
				CArrayFix<TNSmlFilterCapData>* filterInfoArr = new (ELeave) CArrayFixFlat<TNSmlFilterCapData> (1);
							
				agentLog->GetServerDeviceInfoL( dbCaps );				
				CleanupClosePushL( dbCaps );
				
				agentLog->GetServerFilterInfoL( filterInfoArr );				
				CleanupStack::PushL( filterInfoArr );			
				
				CNSmlDbCaps* dbCapab = NULL;
				if ( dbCaps.Count() )
					{
					dbCapab = dbCaps[0];	
					// Send arrays to the data provider for update.					
					iSession.HostClient().CheckSupportedServerFiltersL( iTaskUID, *dbCapab, *filterInfoArr, *filterArray, changeInfo, resultCode );				
					}								
								
				// Read updated array.				
				PackFilterArrayL( *filterArray, ctype->IntValue( EDSAdapterFilterMatchType ), (TInt) changeInfo );
				
				CleanupStack::PopAndDestroy(filterInfoArr);
            	CleanupStack::PopAndDestroy(&dbCaps);	
				}
				
            CleanupStack::PopAndDestroy(agentLog);
            
            filterArray->ResetAndDestroy();            
			CleanupStack::PopAndDestroy(filterArray);            	
            CleanupStack::PopAndDestroy(&filterStream);            
            }
        else
            {
            GetSupportedFiltersL();    
            }
        CleanupStack::PopAndDestroy(prof);
        }
    return FilterSize();
    }
