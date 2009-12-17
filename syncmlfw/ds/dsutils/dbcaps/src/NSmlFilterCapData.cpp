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
* Description:  Source for TNSmlFilterCapData class
*
*/



// INCLUDE FILES
#include "nsmldbcaps.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlFilterCapData::TNSmlFilterCapData
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlFilterCapData::TNSmlFilterCapData( TNSmlFilterCapTag aTag,
    const TDesC8& aValue ) : iTag(aTag), iValue(aValue)
	{
	}

// -----------------------------------------------------------------------------
// TNSmlFilterCapData::Compare
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TNSmlFilterCapData::Compare( const TNSmlFilterCapData& aFirst,
    const TNSmlFilterCapData& aSecond )
	{
	if( aFirst.iTag == aSecond.iTag )
		{
		return aFirst.iValue.Compare(aSecond.iValue);
		}
	return aFirst.iTag - aSecond.iTag;
	}

//  End of File
