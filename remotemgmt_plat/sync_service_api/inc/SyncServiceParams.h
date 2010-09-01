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
* Description: definition of syncservice constants/exported methods
* 	This is part of remotemgmt_plat.
*/


#ifndef SYNCSERVICEPARAMS_H
#define SYNCSERVICEPARAMS_H

// INCLUDE FILES

#include <e32base.h>

// CLASS DECLARATION

// CONSTANTS
const TUid KUidSyncParameterValue = {0x101F8769};

const TInt KSyncServCmdNone = -1;
const TInt KSyncServCmdStartSync = 1;
const TInt KSyncServCmdShowProgress = 2;

const TInt KSyncServDisabled    = 0;
const TInt KSyncServEnabled     = 1;

//_LIT( KDevManServiceName, "101F6DE5_NSmlDMSync" );
_LIT( KDataSyncServiceName, "101F6DE4_NSmlDSSync" );

/**
*  TSyncParameters  
*
*  @lib syncservice.lib
*  @since Series 60 3.0
*/
class TSyncParameters
    {
    public:
    
    	/**
		* TSyncParameters
		*/
        inline TSyncParameters();

    	/**
		* TSyncParameters
		*/
        inline TSyncParameters( const TSyncParameters& aParams );

    	/**
		* ExternalizeL
		*/
        inline void ExternalizeL( RWriteStream& aStream ) const;

    	/**
		* InternalizeL
		*/
        inline void InternalizeL( RReadStream& aStream );

    public:
        TInt    iCommand;
        TInt    iJobId;
        TInt    iProfileId;
        TInt     iSilent;
    };

// ----------------------------------------------------------------------------
// TSyncParameters::TSyncParameters
// ----------------------------------------------------------------------------
//
inline TSyncParameters::TSyncParameters()
:   iCommand( KSyncServCmdNone ),
    iJobId( 0 ),
    iProfileId( 0 )
      ,iSilent(-1)
    {}
    
// ----------------------------------------------------------------------------
// TSyncParameters::TSyncParameters( aParams )
// ----------------------------------------------------------------------------
//
inline TSyncParameters::TSyncParameters( const TSyncParameters& aParams )
:   iCommand( aParams.iCommand ),
    iJobId( aParams.iJobId ),
    iProfileId( aParams.iProfileId )
      ,iSilent(aParams.iSilent)
    {}

// ----------------------------------------------------------------------------
// TSyncParameters::ExternalizeL
// ----------------------------------------------------------------------------
//
inline void TSyncParameters::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iCommand );
    aStream.WriteInt32L( iJobId );
    aStream.WriteInt32L( iProfileId );
    aStream.WriteInt32L( iSilent );
    }
    
// ----------------------------------------------------------------------------
// TSyncParameters::InternalizeL
// ----------------------------------------------------------------------------
// 
inline void TSyncParameters::InternalizeL( RReadStream& aStream )
    {
    iCommand = aStream.ReadInt32L();
    iJobId = aStream.ReadInt32L();
    iProfileId = aStream.ReadInt32L();
    iSilent = aStream.ReadInt32L();
    }
    
#endif //SYNCSERVICEPARAMS_H
    
// End of File
