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
* Description:  Stream to memorybased chunk
*
*/



#ifndef __FOTAENGSTREAM_H__
#define __FOTAENGSTREAM_H__

// INCLUDES
#include <s32strm.h>
#include <s32buf.h>
#include <e32svr.h>

// FORWARD DECLARATIONS
class RFotaEngineSession;
class RFotaWriteStream;


const TInt  KDefaultWriteLimit( 5000000 );

// CLASS DECLARATION
/**
* Memory buffer
*
* Saves streams data to chunk.
*
* @lib    fotaengine.lib
* @since  Series 60 v3.1
*/
class TDP2StreamBuf : public TStreamBuf
{
    friend class RFotaWriteStream;

public: 

    TDP2StreamBuf ();

    /** Resets write area
    *
    * @param    None
    * @return   None
    */
    void            ResetL();

private:

    /** Handles overflow of write area
    *
    * @param    None
    * @return   None
    */
    virtual void    OverflowL();

	
    /** Handles underflow of write area
    *
    * @param    aMaxLength
    * @return   Error code
    */
    virtual TInt    UnderflowL(TInt aMaxLength);

    
    /** Handles releasing of stream/buffer
    *
    * @param    None
    * @return   None
    */
    virtual void    DoRelease();


    /**
    * Bytes written to stream
    */
    TInt            iBytesWritten;


    /**
    * Chunks sent to server
    */
    TInt            iSentChunks;
    

    /**
    * Reference to parent stream
    */
    RFotaWriteStream* iMyWriteStream;

};


/**
 *  RFotaWriteStream saves swupd to memory chunk
 *
 *  Saves written content via TDP2StreamBuf to 
 *  the chunk.
 *
 *  @lib fotaengine.lib
 *  @since S60 3.1
 */
class RFotaWriteStream : public RWriteStream
    {
    friend class RFotaEngineSession;

    friend class TDP2StreamBuf;

public:

    RFotaWriteStream ();

    /** Opens stream
    *
    * @param    aPkgId  package id
    * @return   Error code
    */
    TInt OpenL (const TInt aPkgId);

private:

    /**
     * Memory buffer
     */
    TDP2StreamBuf            iBuf;

    /**
     * Reference to parent session
     */
    RFotaEngineSession*     iFotaEngineSession;

    /**
     * Max package size. If exceeded, must throw error in this class.
     */
    TInt  iWriteLimit;
    };


#endif // __FOTAENGSTREAM_H__