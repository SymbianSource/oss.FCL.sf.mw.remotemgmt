/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for transport handler and related classes
*
*/



// INCLUDE FILES
#include <s32strm.h>

#include "NSmlTransportHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSyncMLTransportPropertyInfo::InternalizeL
// Reads the datatype and name from stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TSyncMLTransportPropertyInfo::InternalizeL( RReadStream& aStream )
	{
	iDataType = (TSyncMLTransportPropertyDataType)aStream.ReadUint8L();
	aStream >> iName;
	}
	
// -----------------------------------------------------------------------------
// TSyncMLTransportPropertyInfo::ExternalizeL
// Writes the datatype and name to stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TSyncMLTransportPropertyInfo::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteUint8L( iDataType );
	aStream << iName;
	}
