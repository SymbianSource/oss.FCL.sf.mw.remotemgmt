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
* Description:  Command structs for Meta Information document (DTD).
*
*/


#ifndef __SMLMETINFDTD_H__
#define __SMLMETINFDTD_H__

//************************************************************************
//*  Includes
//************************************************************************
#include "smldef.h"
#include "smldtd.h"
#include "xmlelement.h"

//************************************************************************
//*  Definitions                                                          
//************************************************************************
typedef struct sml_metinf_anchor_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t	last; /* optional */
	SmlPcdataPtr_t	next;

public:
	IMPORT_C sml_metinf_anchor_s();
	IMPORT_C ~sml_metinf_anchor_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlMetInfAnchorPtr_t, SmlMetInfAnchor_t;

typedef struct sml_metinf_mem_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t	shared; /* optional */
	SmlPcdataPtr_t	free;
	SmlPcdataPtr_t	freeid;

public:
	IMPORT_C sml_metinf_mem_s();
	IMPORT_C ~sml_metinf_mem_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams()	);
	} *SmlMetInfMemPtr_t, SmlMetInfMem_t;

typedef struct sml_metinf_metinf_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t	fieldLevel;/* opt.*/ //1.2 CHANGES: fieldLevel
	SmlPcdataPtr_t	format;    /* opt. */
	SmlPcdataPtr_t	type;      /* opt. */
	SmlPcdataPtr_t	mark;      /* opt. */
	SmlPcdataPtr_t	size;      /* opt. */
	SmlPcdataPtr_t  nextnonce; /* opt. */
	SmlPcdataPtr_t	version; 
	SmlPcdataPtr_t	maxmsgsize; /* optional */
	SmlPcdataPtr_t	maxobjsize; /* optional */
	SmlMetInfMemPtr_t	mem;	/* optional */
	SmlPcdataListPtr_t emi; /* optional */
	SmlMetInfAnchorPtr_t anchor; /* opt. */

public:
	IMPORT_C sml_metinf_metinf_s();
	IMPORT_C ~sml_metinf_metinf_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlMetInfMetInfPtr_t, SmlMetInfMetInf_t;
 
#endif // __SMLMETINFDTD_H__
