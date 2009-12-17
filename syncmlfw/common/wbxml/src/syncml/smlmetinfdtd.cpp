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
* Description:  Meta information command struct implementations.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "smldtd.h"
#include "smlmetinfdtd.h"
#include "smlmetinftags.h"
#include "WBXMLSyncMLParser.h"
#include "WBXMLParserError.h"

// ------------------------------------------------------------------------------------------------
// External functions
// ------------------------------------------------------------------------------------------------
extern CXMLElement* AddPCDataL( SmlPcdataListPtr_t* aList );

// ------------------------------------------------------------------------------------------------
// sml_metinf_metinf_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_metinf_s::sml_metinf_metinf_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_metinf_s::~sml_metinf_metinf_s()
	{
	delete fieldLevel;	//1.2 CHANGES: Fieldlevel
	delete format;
	delete type;
	delete mark;
	delete size;
	delete nextnonce;
	delete version;
	delete maxmsgsize;
	delete maxobjsize;
	delete mem;	
	GenericListDelete( emi );
	delete anchor;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_metinf_metinf_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		//1.2 CHANGES: Fieldlevel
		case EFieldLevel:
			fieldLevel = new (ELeave) SmlPcdata_t();
			return fieldLevel;
		// Changes end

		case EMetFormat:
			format = new (ELeave) SmlPcdata_t();
			return format;

		case EMetType:
			type = new (ELeave) SmlPcdata_t();
			return type;

		case EMetMark:
			mark = new (ELeave) SmlPcdata_t();
			return mark;

		case EMetSize:
			size = new (ELeave) SmlPcdata_t();
			return size;

		case EMetNextNonce:
			nextnonce = new (ELeave) SmlPcdata_t();
			return nextnonce;

		case EMetVersion:
			version = new (ELeave) SmlPcdata_t();
			return version;

		case EMetMaxMsgSize:
			maxmsgsize = new (ELeave) SmlPcdata_t();
			return maxmsgsize;

		case EMetMaxObjSize:
			maxobjsize = new (ELeave) SmlPcdata_t();
			return maxobjsize;

		case EMetMem:
			mem = new (ELeave) SmlMetInfMem_t();
			return mem;

		case EMetEMI:
			return AddPCDataL(&emi);

		case EMetAnchor:
			anchor = new (ELeave) SmlMetInfAnchor_t();
			return anchor;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_metinf_mem_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_mem_s::sml_metinf_mem_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_mem_s::~sml_metinf_mem_s()
	{
	delete shared;
	delete free;
	delete freeid;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_metinf_mem_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EMetSharedMem:
			shared = new (ELeave) SmlPcdata_t();
			return shared;
		
		case EMetFreeMem:
			free = new (ELeave) SmlPcdata_t();
			return free;

		case EMetFreeID:
			freeid = new (ELeave) SmlPcdata_t();
			return freeid;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_metinf_anchor_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_anchor_s::sml_metinf_anchor_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_metinf_anchor_s::~sml_metinf_anchor_s()
	{
	delete last;
	delete next;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_metinf_anchor_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EMetLast:
			last = new (ELeave) SmlPcdata_t();
			return last;
		
		case EMetNext:
			next = new (ELeave) SmlPcdata_t();
			return next;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}
