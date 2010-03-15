/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stream for storing firmware update package
*
*/



// INCLUDE FILES
#include <centralrepository.h>
#include "fotadiskstoragePrivateCRKeys.h"
#include "fotaengstream.h"
#include "fotaengine.h"
#include "fotaenginedebug.h"

// ======================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// RFotaWriteStream::RFotaWriteStream()
// ---------------------------------------------------------------------------
RFotaWriteStream::RFotaWriteStream() : RWriteStream (), iWriteLimit(0)
    {
    iFotaEngineSession  = 0;
    iBuf.iMyWriteStream = this;
    }

// ---------------------------------------------------------------------------
// RFotaWriteStream::OpenL()
// Opens stream
// ---------------------------------------------------------------------------
TInt RFotaWriteStream::OpenL(const TInt /*aPkgId*/)
    {
    FLOG(_L( "[RFotaEngineSession] RFotaWriteStream::OpenL() >>" ) );

    // Get write limit 
    TInt            writelimit( KDefaultWriteLimit );
    CRepository*    centrep( NULL);
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaDiskStorage ) );
    if(err) FLOG(_L(" writelimit not defined in centrep") );
    if ( centrep ) 
        {
        err = centrep->Get(  KFotaDiskSpaceReservationKey, writelimit );
        }
    iWriteLimit = writelimit;
    delete centrep;

    iBuf.ResetL();
    Attach ( &iBuf );  // set sink

    FLOG(_L( "[RFotaEngineSession] RFotaWriteStream::OpenL() <<" ) );
    return 0;
    }


// ---------------------------------------------------------------------------
// TDP2StreamBuf::Reset()
// Redefine write area.
// ---------------------------------------------------------------------------
void TDP2StreamBuf::ResetL()
    {
  	FLOG(_L("TDP2StreamBuf::ResetL() >>" ));
    // Define write area (chunk)
    TArea   a (EWrite) ;
    TInt    limitedwritearea(-1);
    TInt    chunksize = iMyWriteStream->iFotaEngineSession->iChunk.Size();
   // TInt    writelimit = iMyWriteStream->iWriteLimit;
/*	08-nov-06 flexible mem handling overrides this
    // If writelimit is being exceeded, raise error
    if ( iBytesWritten > writelimit ) 
        {
        User::Leave ( KErrOverflow );
        }
*/        
/*		08-nov-06 flexible mem handling overrides this
    // Ensure that we cant go beyond write limit
    if ( (iSentChunks+1) * chunksize > writelimit )
        {
        limitedwritearea = writelimit - iBytesWritten;

        // 0-size writearea would crash writestream base classes
        if ( limitedwritearea == 0 )
            {
            limitedwritearea += 1;
            }        
        }
*/

    // Define write area
    TUint8* p1 = iMyWriteStream->iFotaEngineSession->iChunk.Base();
    TUint8* p2;
    p2 = p1 + chunksize;
    
    /* // dead code
    if ( limitedwritearea == -1 ) 
        {
        p2 = p1 + chunksize;
        }
    else
        {
        p2 = p1 + limitedwritearea;
        }
        */
    SetBuf  (a,p1 , p2 );
  	FLOG(_L("TDP2StreamBuf::ResetL() <<" ));
    }

// ---------------------------------------------------------------------------
// TDP2StreamBuf::DoRelease()
// Stream is released. This is called when data has been comitted to stream.
// ---------------------------------------------------------------------------
void TDP2StreamBuf::DoRelease()
    {
    FLOG(_L("TDP2StreamBuf::DoRelease() >>" ));
    TRAPD(err,  OverflowL() );
    if ( err ) FLOG(_L(" DoRelease overflow err %d"),err);
    this->iMyWriteStream->iFotaEngineSession->ReleaseChunkHandle();
    FLOG(_L("TDP2StreamBuf::DoRelease() <<" ));
    }


// ---------------------------------------------------------------------------
// TDP2StreamBuf::TDP2StreamBuf()
// ---------------------------------------------------------------------------
TDP2StreamBuf::TDP2StreamBuf() : iBytesWritten(0), iSentChunks(0)
                    ,iMyWriteStream(0)
    {
    }


// ---------------------------------------------------------------------------
// TDP2StreamBuf::UnderflowL(TInt aMaxLength)
// ---------------------------------------------------------------------------
TInt TDP2StreamBuf::UnderflowL(TInt aMaxLength)
    {
    FLOG(_L("TDP2StreamBuf::UnderflowL %d"), aMaxLength );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// TDP2StreamBuf::OverflowL()
// Send buffer data to server. Reset buffer.
// ---------------------------------------------------------------------------
void TDP2StreamBuf::OverflowL()
    {
    FLOG(_L("TDP2StreamBuf::OverflowL >>") );
    // Send pointers to data to server. Then reset.
    TUint8* p1 = iMyWriteStream->iFotaEngineSession->iChunk.Base();
    TUint8* p2 = Ptr(EWrite);                           // start point of 
                                                        // write area
    FLOG(_L("TDP2StreamBuf::OverflowL ptrs 0x%x  0x%x"),p1,p2 );
    TPtr8   data(p1, p2-p1, p2-p1 );
    iBytesWritten += p2-p1;
    if ( p2-p1 >0 )
        {
        iMyWriteStream->iFotaEngineSession->SendChunkL(p1,p2);
        iSentChunks++;
        }
    ResetL();
    FLOG(_L("TDP2StreamBuf::OverflowL <<") );
    }

// End of file
