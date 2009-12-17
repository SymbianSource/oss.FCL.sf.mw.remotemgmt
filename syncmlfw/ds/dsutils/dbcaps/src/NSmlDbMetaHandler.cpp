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
* Description:  Source for CNSmlDbMetaHandler class
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
// CNSmlDbMetaHandler::CNSmlDbMetaHandler
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMetaHandler::CNSmlDbMetaHandler() : iMetInf(0)
	{
	//_DBG_FILE("CNSmlDbMetaHandler::CNSmlDbMetaHandler(): begin");
	//_DBG_FILE("CNSmlDbMetaHandler::CNSmlDbMetaHandler(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::CNSmlDbMetaHandler
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDbMetaHandler::CNSmlDbMetaHandler( sml_metinf_metinf_s* aMetInf )
 : iMetInf(aMetInf)
	{
	//_DBG_FILE("CNSmlDbMetaHandler::CNSmlDbMetaHandler(): begin");
	//_DBG_FILE("CNSmlDbMetaHandler::CNSmlDbMetaHandler(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::Format
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::Format() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::Format(): begin");
	if( iMetInf && iMetInf->format )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::Format(): end");
		return iMetInf->format->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::Format(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::Type
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::Type() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::Type(): begin");
	if( iMetInf && iMetInf->type )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::Type(): end");
		return iMetInf->type->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::Type(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::Mark
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::Mark() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::Mark(): begin");
	if( iMetInf && iMetInf->mark )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::Mark(): end");
		return iMetInf->mark->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::Mark(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::Size() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::Size(): begin");
	if( iMetInf && iMetInf->size )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::Size(): end");
		return iMetInf->size->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::Size(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::NextNonce
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::NextNonce() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::NextNonce(): begin");
	if( iMetInf && iMetInf->nextnonce )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::NextNonce(): end");
		return iMetInf->nextnonce->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::NextNonce(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::Version() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::Version(): begin");
	if( iMetInf && iMetInf->version )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::Version(): end");
		return iMetInf->version->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::Version(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::MaxMsgSize
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::MaxMsgSize() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::MaxMsgSize(): begin");
	if( iMetInf && iMetInf->maxmsgsize )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::MaxMsgSize(): end");
		return iMetInf->maxmsgsize->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::MaxMsgSize(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::MaxObjSize
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::MaxObjSize() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::MaxObjSize(): begin");
	if( iMetInf && iMetInf->maxobjsize )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::MaxObjSize(): end");
		return iMetInf->maxobjsize->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::MaxObjSize(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::MemShared
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::MemShared() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::MemShared(): begin");
	if( iMetInf && iMetInf->mem && iMetInf->mem->shared )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::MemShared(): end");
		return iMetInf->mem->shared->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::MemShared(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::MemFreeMem
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::MemFreeMem() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::MemFreeMem(): begin");
	if( iMetInf && iMetInf->mem && iMetInf->mem->free )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::MemFreeMem(): end");
		return iMetInf->mem->free->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::MemFreeMem(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::MemFreeID
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::MemFreeID() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::MemFreeID(): begin");
	if( iMetInf && iMetInf->mem && iMetInf->mem->freeid )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::MemFreeID(): end");
		return iMetInf->mem->freeid->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::MemFreeID(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::AnchorNext
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::AnchorNext() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::AnchorNext(): begin");
	if( iMetInf && iMetInf->anchor && iMetInf->anchor->next )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::AnchorNext(): end");
		return iMetInf->anchor->next->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::AnchorNext(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::AnchorLast
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlDbMetaHandler::AnchorLast() const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::AnchorLast(): begin");
	if( iMetInf && iMetInf->anchor && iMetInf->anchor->last )
		{
		//_DBG_FILE("CNSmlDbMetaHandler::AnchorLast(): end");
		return iMetInf->anchor->last->Data();
		}
	//_DBG_FILE("CNSmlDbMetaHandler::AnchorLast(): end");
	return TPtrC8();
	}

// -----------------------------------------------------------------------------
// CNSmlDbMetaHandler::GetEmiL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDbMetaHandler::GetEmiL( CDesC8Array& aEMI ) const
	{
	//_DBG_FILE("CNSmlDbMetaHandler::GetEmiL(): begin");
	aEMI.Reset();
	if( iMetInf )
		{
		for( SmlPcdataListPtr_t list = iMetInf->emi; list; list = list->next )
			{
			if( list->data )
				{
				aEMI.AppendL(list->data->Data());
				}
			}
		}
	//_DBG_FILE("CNSmlDbMetaHandler::GetEmiL(): end");
	}

//  End of File
