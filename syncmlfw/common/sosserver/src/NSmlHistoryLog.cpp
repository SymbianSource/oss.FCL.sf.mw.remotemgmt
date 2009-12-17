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

// --------------------------------------------------------------------------
// CNSmlHistoryLog::CNSmlHistoryLog( const TInt aProfId )
// --------------------------------------------------------------------------
//
CNSmlHistoryLog::CNSmlHistoryLog( const TInt aProfId, CNSmlSOSSession& aSession ) 
: iProfId(aProfId), iSession(aSession), iDataPtr(NULL, 0, 0)
    {    
    }

// --------------------------------------------------------------------------
// CNSmlHistoryLog::~CNSmlHistoryLog()
// --------------------------------------------------------------------------
//
CNSmlHistoryLog::~CNSmlHistoryLog()
    {
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// TInt CNSmlHistoryLog::FetchDataL()
// --------------------------------------------------------------------------
//
TInt CNSmlHistoryLog::FetchDataL()
    {
    TInt ret(KErrNotFound);
    if ( iProfId < KMaxDataSyncID ) // DS Profile
        {
        CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
        if ( prof )
            {
            CleanupStack::PushL(prof);

            CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	        CleanupStack::PushL( buffer );
	        RBufWriteStream stream( *buffer );  
	                    
            RReadStream logStream = prof->LogReadStreamL(); 
            CleanupClosePushL(logStream);
            
            TInt size = logStream.Source()->SizeL();              
            logStream.ReadL( stream, size );

            CleanupStack::PopAndDestroy(&logStream);
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
            
            CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	        CleanupStack::PushL( buffer );
	        RBufWriteStream stream( *buffer ); 
	        
            RReadStream logStream = prof->LogReadStreamL(); 
            CleanupClosePushL(logStream);          
       	        
	        TInt size = logStream.Source()->SizeL();              
            logStream.ReadL( stream, size );
            
            CleanupStack::PopAndDestroy(&logStream);
            
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
// TInt CNSmlHistoryLog::DataSize()
// --------------------------------------------------------------------------
//
TInt CNSmlHistoryLog::DataSize()
    {
    return iBuffer->Size();
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlHistoryLog::ReadData()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlHistoryLog::ReadData()
    {
    if ( iBuffer )
        {
        iDataPtr.Set( iBuffer->Ptr(0) );    
        }    
    return iDataPtr;
    }

// --------------------------------------------------------------------------
// void CNSmlHistoryLog::ResetL()
// --------------------------------------------------------------------------
//
void CNSmlHistoryLog::ResetL()
    {
    if ( iProfId < KMaxDataSyncID ) // DS Profile
        {
        CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
        if ( prof )
            {
            CleanupStack::PushL(prof);

            RWriteStream logStream = prof->LogWriteStreamL(); 
            CleanupClosePushL(logStream);
           
            logStream.WriteL( TPtrC() );
            CleanupStack::PopAndDestroy(&logStream);

            prof->WriteStreamCommitL();

            CleanupStack::PopAndDestroy(prof);
            }
        }
    else    // DM Profile
        {
        CNSmlDMProfile* prof = iSession.DMSettings().ProfileL( iProfId );
        if ( prof )
            {
            CleanupStack::PushL(prof);

            RWriteStream logStream = prof->LogWriteStreamL(); 
            CleanupClosePushL(logStream);
       
            logStream.WriteL( TPtrC() );
            CleanupStack::PopAndDestroy(&logStream);

            prof->WriteStreamCommitL();

            CleanupStack::PopAndDestroy(prof);
           }
        }    
    }
