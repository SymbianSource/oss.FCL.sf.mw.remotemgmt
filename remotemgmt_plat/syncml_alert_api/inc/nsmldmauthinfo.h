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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*/


#ifndef __NSMLDMAUTHINFO_H__
#define __NSMLDMAUTHINFO_H__

#include <e32base.h>
#include <SyncMLDef.h>
#include <s32strm.h>

/**
*  CNSmlDMAuthInfo class 
*  Container for the DM athentication data
*
*  @lib NSmlPrivateApi.lib
*  @since Series ?XX ?SeriesXX_version
*/
class CNSmlDMAuthInfo : public CBase
    {
    public:
		
		/**
		* C++ default constructor.
	    */
	    IMPORT_C CNSmlDMAuthInfo();
	    
	    /**
        * Destructor.
        */
	    IMPORT_C virtual ~CNSmlDMAuthInfo();

		/**
        * Reads data from the given stream.
        * @since Series ?XX ?SeriesXX_version
        * @param aStream Reference to stream data is read from
        * @return -
        */
	    virtual void InternalizeL( RReadStream& aStream );
	    
	    /**
        * Writes data to the given stream.
        * @since Series ?XX ?SeriesXX_version
        * @param aStream Reference to stream data is written to
        * @return -
        */
	    virtual void ExternalizeL( RWriteStream& aStream ) const;
	    
	    /**
        * Returns the size of the container data
        * @since Series ?XX ?SeriesXX_version
        * @param -
        * @return Size of container data
        */
	    TInt DataSize() const;
	    
	public:
	    TInt iProfileId;
	    HBufC8* iServerNonce;
	    HBufC8* iClientNonce;
	    TUint iAuthPref;
	};

#endif