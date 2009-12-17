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
* Description:  Command structs for device information DTD.
*
*/


#ifndef __SML_DEVINFDTD_H__
#define __SML_DEVINFDTD_H__

//************************************************************************
//*  Includes
//************************************************************************
#include "smldef.h"
#include "smldtd.h"
#include "xmlelement.h"

//************************************************************************
//*  Flags
//************************************************************************
#define SmlUTC_f						(1<<0)
#define SmlSupportNumberOfChanges_f     (1<<1)
#define SmlSupportLargeObjects_f		(1<<2)

//************************************************************************
//*  Definitions
//************************************************************************

typedef struct sml_devinf_xmit_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t  cttype;
    SmlPcdataPtr_t  verct;

public:
	IMPORT_C sml_devinf_xmit_s();
	IMPORT_C ~sml_devinf_xmit_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfXmitPtr_t, SmlDevInfXmit_t;

typedef struct sml_devinf_xmitlist_s : public CBase
	{
public:
    SmlDevInfXmitPtr_t      data;
    struct sml_devinf_xmitlist_s  *next;

public:
	IMPORT_C sml_devinf_xmitlist_s();
	IMPORT_C ~sml_devinf_xmitlist_s();
	} *SmlDevInfXmitListPtr_t, SmlDevInfXmitList_t;

typedef struct sml_devinf_ext_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t      xnam; 
    SmlPcdataListPtr_t  xval; /* optional */

public:
	IMPORT_C sml_devinf_ext_s();
	IMPORT_C ~sml_devinf_ext_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfExtPtr_t, SmlDevInfExt_t;

typedef struct sml_devinf_extlist_s : public CBase
	{
public:
    SmlDevInfExtPtr_t   data;
    struct sml_devinf_extlist_s *next;

public:
	IMPORT_C sml_devinf_extlist_s();
	IMPORT_C ~sml_devinf_extlist_s();
	} *SmlDevInfExtListPtr_t, SmlDevInfExtList_t;

// 1.2 CHANGES: new element FilterCap
typedef struct sml_devinf_filtercap_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t  cttype;
    SmlPcdataPtr_t  verct;
	SmlPcdataListPtr_t  filterkeyword; /* optional */
	SmlPcdataListPtr_t  propname; /* optional */

public:
	IMPORT_C sml_devinf_filtercap_s();
	IMPORT_C ~sml_devinf_filtercap_s();
	CXMLElement * BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfFilterCapPtr_t, SmlDevInfFilterCap_t;

typedef struct sml_devinf_filtercaplist_s : public CBase
	{
public:
	SmlDevInfFilterCapPtr_t data;
	struct sml_devinf_filtercaplist_s *next;

public:
	IMPORT_C sml_devinf_filtercaplist_s();
	IMPORT_C ~sml_devinf_filtercaplist_s();
	} *SmlDevInfFilterCapListPtr_t, SmlDevInfFilterCapList_t;
// Changes end

typedef struct sml_devinf_synccap_s : public CXMLElement
	{
public:
    SmlPcdataListPtr_t  synctype;

public:
	IMPORT_C sml_devinf_synccap_s();
	IMPORT_C ~sml_devinf_synccap_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfSyncCapPtr_t, SmlDevInfSyncCap_t;

typedef struct sml_devinf_dsmem_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t  shared; /* optional, flag */
    SmlPcdataPtr_t  maxmem; /* optional */
    SmlPcdataPtr_t  maxid;  /* optional */

public:
	IMPORT_C sml_devinf_dsmem_s();
	IMPORT_C ~sml_devinf_dsmem_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfDSMemPtr_t, SmlDevInfDSMem_t;

// 1.2 CHANGES: new elements PropParam, Property
typedef struct sml_devinf_propparam_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t			paramname;
	SmlPcdataPtr_t			datatype; /*  optional */
	SmlPcdataListPtr_t    valenum;  /*optional*/ 
	SmlPcdataPtr_t		    displayname; /*optional*/

public:
	IMPORT_C sml_devinf_propparam_s();
	IMPORT_C ~sml_devinf_propparam_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfPropParamPtr_t, SmlDevInfPropParam_t;

typedef struct sml_devinf_propparamlist_s : public CBase
	{
public:
	SmlDevInfPropParamPtr_t  data;
	struct sml_devinf_propparamlist_s *next;

public:
	IMPORT_C sml_devinf_propparamlist_s();
	IMPORT_C ~sml_devinf_propparamlist_s();
	} *SmlDevInfPropParamListPtr_t, SmlDevInfPropParamList_t;

typedef struct sml_devinf_property_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t		propname;
	SmlPcdataPtr_t		datatype;  /* optional */
	SmlPcdataPtr_t		maxoccur; /* optional */
	SmlPcdataPtr_t		maxsize; /* optional */
	SmlPcdataPtr_t     	notruncate; /* optional */
	SmlPcdataListPtr_t			valenum; /* optional */
	SmlPcdataPtr_t				displayname; /* optional */
	SmlDevInfPropParamListPtr_t			propparam; /* optional */

public:
	IMPORT_C sml_devinf_property_s();
	IMPORT_C ~sml_devinf_property_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfPropertyPtr_t, SmlDevInfProperty_t;
// Changes end

typedef struct sml_devinf_ctdata_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t		name;
    SmlPcdataPtr_t		dname; /* optional, display name */
    SmlPcdataListPtr_t  valenum;
    SmlPcdataPtr_t		datatype;
    SmlPcdataPtr_t		size;

public:
	IMPORT_C sml_devinf_ctdata_s();
	IMPORT_C ~sml_devinf_ctdata_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	void SetDataL( const TDesC8& aData );
	} *SmlDevInfCTDataPtr_t, SmlDevInfCTData_t;

typedef struct sml_devinf_ctdatalist_s : public CBase
	{
public:
    SmlDevInfCTDataPtr_t       data;
    struct sml_devinf_ctdatalist_s *next;

public:
	IMPORT_C sml_devinf_ctdatalist_s();
	IMPORT_C ~sml_devinf_ctdatalist_s();
	} *SmlDevInfCTDataListPtr_t, SmlDevInfCTDataList_t;

typedef struct sml_devinf_ctdataprop_s : public CXMLElement
	{
public:
    SmlDevInfCTDataPtr_t    prop;
    SmlDevInfCTDataListPtr_t param;

public:
	IMPORT_C sml_devinf_ctdataprop_s();
	IMPORT_C ~sml_devinf_ctdataprop_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	void SetDataL( const TDesC8& aData );
	} *SmlDevInfCTDataPropPtr_t, SmlDevInfCTDataProp_t;

typedef struct sml_devinf_propertylist_s : public CBase
	{
public:
    SmlDevInfPropertyPtr_t    data;
    struct sml_devinf_propertylist_s *next;

public:
	IMPORT_C sml_devinf_propertylist_s();
	IMPORT_C ~sml_devinf_propertylist_s();
	} *SmlDevInfPropertyListPtr_t, SmlDevInfPropertyList_t;

typedef struct sml_devinf_ctdataproplist_s : public CBase
	{
public:
    SmlDevInfCTDataPropPtr_t    data;
    struct sml_devinf_ctdataproplist_s *next;

public:
	IMPORT_C sml_devinf_ctdataproplist_s();
	IMPORT_C ~sml_devinf_ctdataproplist_s();
	} *SmlDevInfCTDataPropListPtr_t, SmlDevInfCTDataPropList_t;

typedef enum 
	{
	SML_PROPNAME,
	SML_PARAMNAME
	} SmlPreviousElement_t;

typedef struct sml_devinf_ctcap_s : public CXMLElement
	{
private:
		SmlPreviousElement_t			  prevelem;
		CXMLElement* iParent;

private:
	CXMLElement* GetPreviousElem() const;

public:
	SmlPcdataPtr_t		cttype;
	SmlDevInfCTDataPropListPtr_t  prop; 
	// 1.2 CHANGES : new elements
	SmlPcdataPtr_t		verct; 
	SmlPcdataPtr_t      fieldlevel; 
    SmlDevInfPropertyListPtr_t property; 
	// changes end
	
public:
	IMPORT_C sml_devinf_ctcap_s();
	IMPORT_C sml_devinf_ctcap_s( CXMLElement* aParent );
	IMPORT_C ~sml_devinf_ctcap_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfCtCapPtr_t, SmlDevInfCtCap_t;

typedef struct sml_devinf_ctcaplist_s : public CXMLElement
	{
public:
    SmlDevInfCtCapPtr_t       data;
    struct sml_devinf_ctcaplist_s *next;

public:
	IMPORT_C sml_devinf_ctcaplist_s();
	IMPORT_C ~sml_devinf_ctcaplist_s();
	} *SmlDevInfCtCapListPtr_t, SmlDevInfCtCapList_t;

typedef struct sml_devinf_datastore_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t          sourceref;
    SmlPcdataPtr_t          displayname;   /* optional */
    SmlPcdataPtr_t          maxguidsize;   /* optional */
    SmlDevInfXmitPtr_t      rxpref;
    SmlDevInfXmitListPtr_t  rx;            /* optional */
    SmlDevInfXmitPtr_t      txpref;
    SmlDevInfXmitListPtr_t  tx;            /* optional */
	SmlDevInfDSMemPtr_t     dsmem;         /* optional */
	SmlDevInfSyncCapPtr_t   synccap;
	// 1.2 CHANGES : new / moved  elements
    SmlPcdataPtr_t      supportHierarchicalSync; 
	SmlDevInfCtCapListPtr_t    ctcap;  
	SmlDevInfXmitListPtr_t  filterrx;	/* optional */
 	SmlDevInfFilterCapListPtr_t filtercap; /* optional */
	// changes end

public:
	IMPORT_C sml_devinf_datastore_s();
	IMPORT_C ~sml_devinf_datastore_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlDevInfDatastorePtr_t, SmlDevInfDatastore_t;

typedef struct sml_devinf_datastorelist_s : public CXMLElement
	{
public:
    SmlDevInfDatastorePtr_t data;
    struct sml_devinf_datastorelist_s *next;

public:
	IMPORT_C sml_devinf_datastorelist_s();
	IMPORT_C ~sml_devinf_datastorelist_s();
	} *SmlDevInfDatastoreListPtr_t, SmlDevInfDatastoreList_t;

typedef struct sml_devinf_devinf_s : public CXMLElement
	{
public:
    SmlPcdataPtr_t      verdtd;
    SmlPcdataPtr_t      man; /* optional */
    SmlPcdataPtr_t      mod; /* optional */
    SmlPcdataPtr_t      oem; /* optional */
    SmlPcdataPtr_t      fwv; /* optional */
    SmlPcdataPtr_t      swv; /* optional */
    SmlPcdataPtr_t      hwv; /* optional */
    SmlPcdataPtr_t      devid;
    SmlPcdataPtr_t      devtyp;
	Flag_t				flags;
    SmlDevInfDatastoreListPtr_t datastore;
	SmlDevInfCtCapListPtr_t    ctcap;  
    SmlDevInfExtListPtr_t       ext;

public:
	IMPORT_C sml_devinf_devinf_s();
	IMPORT_C ~sml_devinf_devinf_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlDevInfDevInfPtr_t, SmlDevInfDevInf_t;

#endif // __SML_DEVINFDTD_H__
