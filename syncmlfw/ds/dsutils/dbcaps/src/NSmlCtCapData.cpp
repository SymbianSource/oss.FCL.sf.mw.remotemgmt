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
* Description:  Source for TNSmlCtCapData class
*
*/


// INCLUDE FILES
#include <badesca.h>

#include <nsmldebug.h>
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"
#include "smldevinftags.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlCtCapData::TNSmlCtCapData
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlCtCapData::TNSmlCtCapData( TNSmlCtCapTag aTag, const TDesC8& aValue )
 : iTag(aTag), iValue(aValue)
	{
	}

// -----------------------------------------------------------------------------
// TNSmlCtCapData::Compare
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TNSmlCtCapData::Compare( const TNSmlCtCapData& aFirst, const TNSmlCtCapData& aSecond )
	{
	if( aFirst.iTag == aSecond.iTag )
		{
		return aFirst.iValue.Compare(aSecond.iValue);
		}
	return aFirst.iTag - aSecond.iTag;
	}

//  End of File

