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
* Description:  Filter sources.
*
*/



// INCLUDE FILES
#include "nsmldbcaps.h"
#include "nsmlfilter.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlFieldPropData::TNSmlFieldPropData
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlFieldPropData::TNSmlFieldPropData( TNSmlFieldPropTag aTag,
                                                 const TDesC8& aValue ) 
    : iTag(aTag), iValue(aValue)
	{
	}

// -----------------------------------------------------------------------------
// TNSmlFieldPropData::Compare
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TNSmlFieldPropData::Compare( const TNSmlFieldPropData& aFirst,
                                           const TNSmlFieldPropData& aSecond )
	{
	if( aFirst.iTag == aSecond.iTag )
		{
		return aFirst.iValue.Compare(aSecond.iValue);
		}
	return aFirst.iTag - aSecond.iTag;
	}

//  End of File
