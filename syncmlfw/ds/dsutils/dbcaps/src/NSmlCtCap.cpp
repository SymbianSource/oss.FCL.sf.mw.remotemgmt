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
* Description:  Source for CNSmlCtCap class
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
// CNSmlCtCap::CNSmlCtCap
// -----------------------------------------------------------------------------
//
CNSmlCtCap::CNSmlCtCap( SmlDevInfCtCapPtr_t aCtCapPtr ) : iCtCapPtr(aCtCapPtr)
	{
	//_DBG_FILE("CNSmlCtCap::CNSmlCtCap(): begin");
	//_DBG_FILE("CNSmlCtCap::CNSmlCtCap(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlCtCap::~CNSmlCtCap
// -----------------------------------------------------------------------------
//
CNSmlCtCap::~CNSmlCtCap()
	{
	//_DBG_FILE("CNSmlCtCap::~CNSmlCtCap(): begin");
	//_DBG_FILE("CNSmlCtCap::~CNSmlCtCap(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlCtCap::SetCtTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCtCap::SetCtTypeL( const TDesC8& aCtType )
	{
	//_DBG_FILE("CNSmlCtCap::SetCtTypeL(): begin");
	iCtCapPtr->cttype = new (ELeave) SmlPcdata_t();
	iCtCapPtr->cttype->SetDataL(aCtType);
	//_DBG_FILE("CNSmlCtCap::SetCtTypeL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlCtCap::SetVerCtL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCtCap::SetVerCtL( const TDesC8& aVerCt )
    {
    iCtCapPtr->verct = new (ELeave) SmlPcdata_t();
	iCtCapPtr->verct->SetDataL(aVerCt);
    }

// -----------------------------------------------------------------------------
// CNSmlCtCap::SetFieldLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCtCap::SetFieldLevelL()
    {
    iCtCapPtr->fieldlevel = new (ELeave) SmlPcdata_t();
	iCtCapPtr->fieldlevel->SetDataL(TPtrC8());
    }

// -----------------------------------------------------------------------------
// CNSmlCtCap::AddDevInfPropLC
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDevInfProp* CNSmlCtCap::AddDevInfPropLC( const TDesC8& aProp )
	{
	//_DBG_FILE("CNSmlCtCap::AddPropL(): begin");
	SmlDevInfPropertyListPtr_t prop = new (ELeave) SmlDevInfPropertyList_t();
	CleanupStack::PushL(prop);
	GenericListAddL(&iCtCapPtr->property, prop);
	CleanupStack::Pop(); // prop
	prop->data = new (ELeave) SmlDevInfProperty_t();
	prop->data->propname = new (ELeave) SmlPcdata_t();
	prop->data->propname->SetDataL(aProp);
	CNSmlDevInfProp* p = new (ELeave) CNSmlDevInfProp(prop->data);
	CleanupStack::PushL(p);
	//_DBG_FILE("CNSmlCtCap::AddPropL(): end");
	return p;
	}

//  End of File
