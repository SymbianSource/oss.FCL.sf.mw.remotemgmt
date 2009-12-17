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
* Description:  Source for CNSmlDbMeta class
*
*/


// INCLUDE FILES
#include <badesca.h>

#include <nsmldebug.h>
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"
#include "smldevinftags.h"


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// AddPCDataL
// -----------------------------------------------------------------------------
//
CXMLElement* AddPCDataL( SmlPcdataListPtr_t* aList )
	{
	SmlPcdataListPtr_t itemL = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlPcdata_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDbMeta::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMeta* CNSmlDbMeta::NewL()
	{
	//_DBG_FILE("CNSmlDbMeta::NewL(): begin");
	CNSmlDbMeta* self = CNSmlDbMeta::NewLC();
	CleanupStack::Pop(); // self
	//_DBG_FILE("CNSmlDbMeta::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMeta* CNSmlDbMeta::NewLC()
	{
	//_DBG_FILE("CNSmlDbMeta::NewLC(): begin");
	CNSmlDbMeta* self = new (ELeave) CNSmlDbMeta();
	CleanupStack::PushL(self);
	self->ConstructL();
	//_DBG_FILE("CNSmlDbMeta::NewLC(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::CNSmlDbMeta
// -----------------------------------------------------------------------------
//
CNSmlDbMeta::CNSmlDbMeta()
	{
	//_DBG_FILE("CNSmlDbMeta::CNSmlDbMeta(): begin");
	//_DBG_FILE("CNSmlDbMeta::CNSmlDbMeta(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlDbMeta::ConstructL()
	{
	//_DBG_FILE("CNSmlDbMeta::ConstructL(): begin");
	//_DBG_FILE("CNSmlDbMeta::ConstructL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::~CNSmlDbMeta
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMeta::~CNSmlDbMeta()
	{
	//_DBG_FILE("CNSmlDbMeta::~CNSmlDbMeta(): begin");
	delete iMetInf;
	//_DBG_FILE("CNSmlDbMeta::~CNSmlDbMeta(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::MetInfL
// -----------------------------------------------------------------------------
//
EXPORT_C sml_metinf_metinf_s* CNSmlDbMeta::MetInfL() const
	{
	//_DBG_FILE("CNSmlDbMeta::MetInf(): begin");
	//_DBG_FILE("CNSmlDbMeta::MetInf(): end");
	sml_metinf_metinf_s* temp = iMetInf;
	iMetInf = 0;
	return temp;
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::Clear
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::Clear()
	{
	//_DBG_FILE("CNSmlDbMeta::Clear(): begin");
	delete iMetInf;
	iMetInf = 0;
	//_DBG_FILE("CNSmlDbMeta::Clear(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::CreateHandlerL
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMetaHandler* CNSmlDbMeta::CreateHandlerL() const
	{
	//_DBG_FILE("CNSmlDbMeta::CreateHandlerL(): begin");
	//_DBG_FILE("CNSmlDbMeta::CreateHandlerL(): end");
	return new (ELeave) CNSmlDbMetaHandler(iMetInf);
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetFormatL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetFormatL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetFormatL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->format )
		{
		iMetInf->format = new (ELeave) SmlPcdata_t();
		}
	iMetInf->format->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetFormatL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetTypeL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetTypeL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->type )
		{
		iMetInf->type = new (ELeave) SmlPcdata_t();
		}
	iMetInf->type->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetTypeL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMarkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMarkL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMarkL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->mark )
		{
		iMetInf->mark = new (ELeave) SmlPcdata_t();
		}
	iMetInf->mark->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMarkL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetSizeL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetSizeL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->size )
		{
		iMetInf->size = new (ELeave) SmlPcdata_t();
		}
	iMetInf->size->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetSizeL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetNextNonceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetNextNonceL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetNextNonceL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->nextnonce )
		{
		iMetInf->nextnonce = new (ELeave) SmlPcdata_t();
		}
	iMetInf->nextnonce->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetNextNonceL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetVersionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetVersionL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetVersionL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->version )
		{
		iMetInf->version = new (ELeave) SmlPcdata_t();
		}
	iMetInf->version->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetVersionL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMaxMsgSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMaxMsgSizeL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMaxMsgSizeL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->maxmsgsize )
		{
		iMetInf->maxmsgsize = new (ELeave) SmlPcdata_t();
		}
	iMetInf->maxmsgsize->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMaxMsgSizeL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMaxObjSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMaxObjSizeL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMaxObjSizeL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->maxobjsize )
		{
		iMetInf->maxobjsize = new (ELeave) SmlPcdata_t();
		}
	iMetInf->maxobjsize->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMaxObjSizeL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMemSharedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMemSharedL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMemSharedL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->mem )
		{
		iMetInf->mem = new (ELeave) sml_metinf_mem_s();
		}
	if( !iMetInf->mem->shared )
		{
		iMetInf->mem->shared = new (ELeave) SmlPcdata_t();
		}
	iMetInf->mem->shared->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMemSharedL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMemFreeMemL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMemFreeMemL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMemFreeMemL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->mem )
		{
		iMetInf->mem = new (ELeave) sml_metinf_mem_s();
		}
	if( !iMetInf->mem->free )
		{
		iMetInf->mem->free = new (ELeave) SmlPcdata_t();
		}
	iMetInf->mem->free->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMemFreeMemL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetMemFreeIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetMemFreeIDL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetMemFreeIDL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->mem )
		{
		iMetInf->mem = new (ELeave) sml_metinf_mem_s();
		}
	if( !iMetInf->mem->freeid )
		{
		iMetInf->mem->freeid = new (ELeave) SmlPcdata_t();
		}
	iMetInf->mem->freeid->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetMemFreeIDL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetAnchorNextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetAnchorNextL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetAnchorNextL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->anchor )
		{
		iMetInf->anchor = new (ELeave) sml_metinf_anchor_s();
		}
	if( !iMetInf->anchor->next )
		{
		iMetInf->anchor->next = new (ELeave) SmlPcdata_t();
		}
	iMetInf->anchor->next->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetAnchorNextL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetAnchorLastL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetAnchorLastL( const TDesC8& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetAnchorLastL(): begin");
	MakeSureMetInfExistsL();
	if( !iMetInf->anchor )
		{
		iMetInf->anchor = new (ELeave) sml_metinf_anchor_s();
		}
	if( !iMetInf->anchor->last )
		{
		iMetInf->anchor->last = new (ELeave) SmlPcdata_t();
		}
	iMetInf->anchor->last->SetDataL(aValue);
	//_DBG_FILE("CNSmlDbMeta::SetAnchorLastL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::SetEmiL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMeta::SetEmiL( const CDesC8Array& aValue )
	{
	//_DBG_FILE("CNSmlDbMeta::SetVersionL(): begin");
	MakeSureMetInfExistsL();
	for( TInt i = 0; i < aValue.MdcaCount(); i++ )
		{
		CXMLElement* e = AddPCDataL(&iMetInf->emi);
		e->SetDataL(aValue[i]);
		}
	//_DBG_FILE("CNSmlDbMeta::SetVersionL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMeta::MakeSureMetInfExistsL
// -----------------------------------------------------------------------------
//
void CNSmlDbMeta::MakeSureMetInfExistsL()
	{
	//_DBG_FILE("CNSmlDbMeta::MakeSureMetInfExistsL(): begin");
	if( !iMetInf )
		{
		iMetInf = new (ELeave) sml_metinf_metinf_s();
		}
	//_DBG_FILE("CNSmlDbMeta::MakeSureMetInfExistsL(): end");
	}

//  End of File
