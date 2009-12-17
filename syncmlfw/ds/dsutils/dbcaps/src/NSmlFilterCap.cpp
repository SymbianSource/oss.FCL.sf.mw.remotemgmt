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
* Description:  Sources
*
*/



// INCLUDE FILES
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlFilterCap::CNSmlFilterCap
// -----------------------------------------------------------------------------
//
CNSmlFilterCap::CNSmlFilterCap( SmlDevInfFilterCapPtr_t aFilterCapPtr )
 : iFilterCapPtr(aFilterCapPtr)
    {
    }

// -----------------------------------------------------------------------------
// CNSmlFilterCap::~CNSmlFilterCap
// -----------------------------------------------------------------------------
//
CNSmlFilterCap::~CNSmlFilterCap()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlFilterCap::AddFilterKeywordL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlFilterCap::AddFilterKeywordL( const TDesC8& aKeyword )
    {
	SmlPcdataListPtr_t keyword = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(keyword);
	GenericListAddL(&iFilterCapPtr->filterkeyword, keyword);
	CleanupStack::Pop(); // keyword
	keyword->data = new (ELeave) SmlPcdata_t();
	keyword->data->SetDataL(aKeyword);
    }

// -----------------------------------------------------------------------------
// CNSmlFilterCap::AddPropNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlFilterCap::AddPropNameL( const TDesC8& aPropName )
    {
    SmlPcdataListPtr_t propname = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(propname);
	GenericListAddL(&iFilterCapPtr->propname, propname);
	CleanupStack::Pop(); // propname
	propname->data = new (ELeave) SmlPcdata_t();
	propname->data->SetDataL(aPropName);
    }

//  End of File
