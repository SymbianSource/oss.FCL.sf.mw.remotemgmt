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
* Description:  Source for CNSmlDevInfProp class
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
// CNSmlDevInfProp::CNSmlDevInfProp
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDevInfProp::CNSmlDevInfProp( SmlDevInfPropertyPtr_t aPropPtr )
 : iPropPtr(aPropPtr)
	{
	//_DBG_FILE("CNSmlDevInfProp::CNSmlDevInfProp(): begin");
	//_DBG_FILE("CNSmlDevInfProp::CNSmlDevInfProp(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::~CNSmlDevInfProp
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDevInfProp::~CNSmlDevInfProp()
	{
	//_DBG_FILE("CNSmlDevInfProp::~CNSmlDevInfProp(): begin");
	//_DBG_FILE("CNSmlDevInfProp::~CNSmlDevInfProp(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::AddParamLC
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlPropParam* CNSmlDevInfProp::AddParamLC( const TDesC8& aParam )
	{
	//_DBG_FILE("CNSmlDevInfProp::AddParamL(): begin");
	SmlDevInfPropParamListPtr_t param = new (ELeave) SmlDevInfPropParamList_t();
	CleanupStack::PushL(param);
	GenericListAddL(&iPropPtr->propparam, param);
	CleanupStack::Pop(); // param
	param->data = new (ELeave) SmlDevInfPropParam_t();
	param->data->paramname = new (ELeave) SmlPcdata_t();
	param->data->paramname->SetDataL(aParam);
	CNSmlPropParam* p = new (ELeave) CNSmlPropParam(param->data);
	CleanupStack::PushL(p);
	//_DBG_FILE("CNSmlDevInfProp::AddParamL(): end");
	return p;
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::SetDisplayNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::SetDisplayNameL( const TDesC8& aDispName )
	{
    if (!iPropPtr->displayname )
        {
	    iPropPtr->displayname = new (ELeave) SmlPcdata_t();
	    iPropPtr->displayname->SetDataL(aDispName);
        }
    else
        {
	    iPropPtr->displayname->SetDataL(aDispName);
        }
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::AddValEnumL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::AddValEnumL( const TDesC8& aValEnum )
	{
	//_DBG_FILE("CNSmlDevInfProp::AddValEnumL(): begin");
	SmlPcdataListPtr_t valenum = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(valenum);
	GenericListAddL(&iPropPtr->valenum, valenum);
	CleanupStack::Pop(); // valenum
	valenum->data = new (ELeave) SmlPcdata_t();
	valenum->data->SetDataL(aValEnum);
	//_DBG_FILE("CNSmlDevInfProp::AddValEnumL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::SetDataTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::SetDataTypeL( const TDesC8& aDataType )
	{
    if ( !iPropPtr->datatype )
        {
	    iPropPtr->datatype = new (ELeave) SmlPcdata_t();
	    iPropPtr->datatype->SetDataL(aDataType);
        }
    else
        {
        iPropPtr->datatype->SetDataL(aDataType);
        }
	}

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::SetMaxOccurL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::SetMaxOccurL( const TDesC8& aMaxOccur )
    {
    if ( !iPropPtr->maxoccur )
        {
	    iPropPtr->maxoccur = new (ELeave) SmlPcdata_t();
	    iPropPtr->maxoccur->SetDataL(aMaxOccur);
        }
    else
        {
        iPropPtr->maxoccur->SetDataL(aMaxOccur);
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::SetMaxSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::SetMaxSizeL( const TDesC8& aMaxSize )
    {
    if ( !iPropPtr->maxsize )
        {
	    iPropPtr->maxsize = new (ELeave) SmlPcdata_t();
	    iPropPtr->maxsize->SetDataL(aMaxSize);
        }
    else
        {
        iPropPtr->maxsize->SetDataL(aMaxSize);
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDevInfProp::SetNoTruncateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDevInfProp::SetNoTruncateL()
    {
    if ( !iPropPtr->notruncate )
        {
	    iPropPtr->notruncate = new (ELeave) SmlPcdata_t();
        iPropPtr->notruncate->SetDataL(TPtrC8());
        }
    else
        {
        iPropPtr->notruncate->SetDataL(TPtrC8());
        }
    }

//  End of File
