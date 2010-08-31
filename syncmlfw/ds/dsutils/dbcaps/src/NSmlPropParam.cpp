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
* Description:  Source for CNSmlPropParam class
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
// CNSmlPropParam::CNSmlPropParam
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlPropParam::CNSmlPropParam( SmlDevInfPropParamPtr_t aParamPtr )
 : iParamPtr(aParamPtr)
	{
	//_DBG_FILE("CNSmlPropParam::CNSmlPropParam(): begin");
	//_DBG_FILE("CNSmlPropParam::CNSmlPropParam(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlPropParam::SetDisplayNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlPropParam::SetDisplayNameL( const TDesC8& aDispName )
	{
	//_DBG_FILE("CNSmlPropParam::SetDisplayNameL(): begin");
    if ( !iParamPtr->displayname )
        {
	    iParamPtr->displayname = new (ELeave) SmlPcdata_t();
	    iParamPtr->displayname->SetDataL(aDispName);
        }
    else
        {
        iParamPtr->displayname->SetDataL(aDispName);
        }
	//_DBG_FILE("CNSmlPropParam::SetDisplayNameL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlPropParam::AddValEnumL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlPropParam::AddValEnumL( const TDesC8& aValEnum )
	{
	//_DBG_FILE("CNSmlPropParam::AddValEnumL(): begin");
	SmlPcdataListPtr_t valenum = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(valenum);
	GenericListAddL(&iParamPtr->valenum, valenum);
	CleanupStack::Pop(); // valenum
	valenum->data = new (ELeave) SmlPcdata_t();
	valenum->data->SetDataL(aValEnum);
	//_DBG_FILE("CNSmlPropParam::AddValEnumL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlPropParam::SetDataTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlPropParam::SetDataTypeL( const TDesC8& aDataType )
	{
	//_DBG_FILE("CNSmlPropParam::SetDataTypeL(): begin");
    if ( !iParamPtr->datatype )
        {
	    iParamPtr->datatype = new (ELeave) SmlPcdata_t();
	    iParamPtr->datatype->SetDataL(aDataType);
        }
    else
        {
        iParamPtr->datatype->SetDataL(aDataType);
        }
	//_DBG_FILE("CNSmlPropParam::SetDataTypeL(): end");
	}

//  End of File

