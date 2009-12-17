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
* Description:  Commmand structs for SyncML document (DTD).
*
*/


#ifndef __SMLDTD_H__
#define __SMLDTD_H__

/*************************************************************************/
/*  Definitions                                                          */
/*************************************************************************/
#include "smldef.h"
#include "xmlelement.h"
 
/** 
 * PCDATA - types of synchronization data which SyncML supports
 **/
typedef enum {
  SML_PCDATA_UNDEFINED = 0,
  SML_PCDATA_STRING,                   // String type
  SML_PCDATA_OPAQUE,                   // Opaque type
  SML_PCDATA_EXTENSION,                // Extention type - specified by PcdataExtension_t
  SML_PCDATA_CDATA                     // XML CDATA type   
} SmlPcdataType_t;

/**
 * PCDATA - types of extensions for PCData elements
 */
typedef enum {
  SML_EXT_UNDEFINED = 0,
  SML_EXT_METINF, // Meta Information
  SML_EXT_DEVINF, // Device Information
  SML_EXT_DEVINFPROP, // 1.2 CHANGES: Device Info Property 
  SML_EXT_LAST    // last codepage, needed for loops!
} SmlPcdataExtension_t;

/** 
 * PCDATA - into this structure SyncML wraps the synchronization data itself
 **/ 
typedef struct sml_pcdata_s : public CXMLElement
	{
public:
	SmlPcdataType_t       contentType;   // The type of data which a PCDATA structure contains
	SmlPcdataExtension_t  extension;     // PCData Extension type
	MemSize_t             length;        // length of the data in this PCDATA structure
	VoidPtr_t             content;       // Pointer to the data itself

public:
	IMPORT_C sml_pcdata_s();
	IMPORT_C ~sml_pcdata_s();
	IMPORT_C sml_pcdata_s( const SmlPcdataType_t aType, const SmlPcdataExtension_t aExt =  SML_EXT_UNDEFINED );
	IMPORT_C void FreeContent();
	IMPORT_C void SetDataL( const TDesC8& aData );
	IMPORT_C TPtrC8 Data() const;
	IMPORT_C void TruncateL( TInt aConsumed );
	//CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlPcdataPtr_t, SmlPcdata_t;       

/* generic list of PCData elements */
typedef struct sml_pcdata_list_s : public CBase
	{
	SmlPcdataPtr_t	data;
	struct sml_pcdata_list_s *next;

	IMPORT_C sml_pcdata_list_s();
	IMPORT_C ~sml_pcdata_list_s();
	} *SmlPcdataListPtr_t, SmlPcdataList_t;

/** 
 * Various flags which are actually declared and (EMPTY) elements in
 * SyncML. This assumes at least a 16-bit architecture for the
 * underlying OS. We need to review this if that is deemed a problem.
 **/
#define SmlArchive_f       (1<<0)	// Delete flags
#define SmlSftDel_f        (1<<1)	// Delete flags
#define SmlMoreData_f      (1<<2)	// MoreData flag
#define SmlNoResults_f     (1<<3)	// No Results flag 
#define SmlNoResp_f        (1<<4)	// No Response flag
#define SmlFinal_f         (1<<5)	// Header flag
#define SmlMetInfSharedMem_f (1<<6)	// MetInf Shared Memory Flag
#define SmlDevInfSharedMem_f (1<<7)	// DevInf Shared Memory Flag

/**
 * Chal 
 **/
typedef struct sml_chal_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           meta;

public:
	IMPORT_C sml_chal_s();
	IMPORT_C ~sml_chal_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlChalPtr_t, SmlChal_t;

/** 
 * Credentials
 **/
typedef struct sml_cred_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           meta;       // opt.
	SmlPcdataPtr_t           data; 

public:
	IMPORT_C sml_cred_s();
	IMPORT_C ~sml_cred_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	} *SmlCredPtr_t, SmlCred_t;


// 1.2 CHANGES: new common use elements
typedef struct sml_item_s *SmlItemPtr_t;

typedef struct sml_field_or_record_s : public CXMLElement
	{
public:
	SmlItemPtr_t			item;

public:
	IMPORT_C sml_field_or_record_s();
	IMPORT_C ~sml_field_or_record_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	}	*SmlFieldOrRecordPtr_t, SmlFieldOrRecord_t,
		*SmlFieldPtr_t, SmlField_t,
		*SmlRecordPtr_t, SmlRecord_t;

typedef struct sml_filter_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           meta;
	SmlFieldPtr_t			 field;		 // opt.
	SmlRecordPtr_t			 record;	 // opt.
	SmlPcdataPtr_t           filterType; // opt.

public:
	IMPORT_C sml_filter_s();
	IMPORT_C ~sml_filter_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	}	*SmlFilterPtr_t, SmlFilter_t;
// Changes end

// 1.2 CHANGES: Source & Target divided to their own structs
/** 
 * Source location
 **/
typedef struct sml_source_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           locURI;
	SmlPcdataPtr_t           locName;    // opt.

public:
	IMPORT_C sml_source_s();
	IMPORT_C ~sml_source_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	}	*SmlSourcePtr_t, SmlSource_t;

/** 
 * Target location
 **/
typedef struct sml_target_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           locURI;
	SmlPcdataPtr_t           locName;    // opt.
	SmlFilterPtr_t			 filter;	 // opt.

public:
	IMPORT_C sml_target_s();
	IMPORT_C ~sml_target_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	}	*SmlTargetPtr_t, SmlTarget_t;
// Changes end

typedef struct sml_source_list_s : public CBase
	{
public:
	SmlSourcePtr_t           source;
	struct sml_source_list_s  *next;

public:
	IMPORT_C sml_source_list_s();
	IMPORT_C ~sml_source_list_s();
	} *SmlSourceListPtr_t, SmlSourceList_t;

//1.2 CHANGES: Source & Target parent
/** 
 * Source & Target parent
 **/
typedef struct sml_source_or_target_parent_s : public CXMLElement
	{
public:
	SmlPcdataPtr_t           locURI;

public:
	IMPORT_C sml_source_or_target_parent_s();
	IMPORT_C ~sml_source_or_target_parent_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	}	*SmlSourceOrTargetParentPtr_t, SmlSourceOrTargetParent_t,
		*SmlSourceParentPtr_t, SmlSourceParent_t,
		*SmlTargetParentPtr_t, SmlTargetParent_t;
// Changes end


/**
 * ==============================
 * SyncML Message Header Elements
 * ==============================
 **/

/** 
 * SyncML header
 * As the header is needed for each SyncML message, it's also the parameter
 * of the startMessage call.
 **/
typedef struct sml_sync_hdr_s : public CXMLElement
	{
public:
	SmlProtoElement_t	elementType; // Internal Toolkit Field
	SmlPcdataPtr_t  version;       
	SmlPcdataPtr_t  proto;         
	SmlPcdataPtr_t  sessionID;     
	SmlPcdataPtr_t  msgID;         
	Flag_t          flags;      // NoResp
	SmlTargetPtr_t  target;
	SmlSourcePtr_t  source;
	SmlPcdataPtr_t  respURI;    // opt.
	SmlCredPtr_t    cred;       // opt.
	SmlPcdataPtr_t    meta;     // opt.

public:
	IMPORT_C sml_sync_hdr_s();
	IMPORT_C ~sml_sync_hdr_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlSyncHdrPtr_t, SmlSyncHdr_t;

// SyncML Body and SyncML container is not needed, as there are function calls
// (smlStartMessage(), smlEndMessage()) that let the framework know when to start and end
// the SyncML document



/**
 * =========================
 * Data description elements
 * =========================
 **/


/** 
 * Data in SyncML is encapsulated in an "item" element. 
 **/
typedef struct sml_item_s : public CXMLElement
	{
public:
	Flag_t                generatorFlags;      // internal DO NOT TOUCH!

	SmlTargetPtr_t        target;     // opt.
	SmlSourcePtr_t        source;     // opt.
// 1.2 CHANGES: SourceParent and TargetParent added
	SmlSourceParentPtr_t  sourceParent;// opt.
	SmlTargetParentPtr_t  targetParent;// opt.
// Changes end
	SmlPcdataPtr_t        meta;       // opt.
	SmlPcdataPtr_t        data;       // opt.
	Flag_t                flags;      // opt. for MoreData

public:
	IMPORT_C sml_item_s();
	IMPORT_C ~sml_item_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlItemPtr_t, SmlItem_t;

typedef struct sml_item_list_s : public CBase
	{
public:
	SmlItemPtr_t          item;
	struct sml_item_list_s    *next;

public:
	IMPORT_C sml_item_list_s();
	IMPORT_C ~sml_item_list_s();
	} *SmlItemListPtr_t, SmlItemList_t;

/**
 * ==============================================
 * SyncML Commands (Protocol Management Elements)
 * ==============================================
 **/

// 1.2 CHANGES: Move command added
/**
 * Generic commands:
 * Add, Copy, Replace, Delete, Move
 **/
typedef struct sml_generic_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;      
	Flag_t                flags;      // NoResp, Archive (Delete), SftDel (Delete)
	SmlCredPtr_t          cred;       // opt.
	SmlPcdataPtr_t        meta;       // opt.
	SmlItemListPtr_t      itemList;

public:
	IMPORT_C sml_generic_s();
	IMPORT_C ~sml_generic_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	}	*SmlAddPtr_t, SmlAdd_t, 
		*SmlCopyPtr_t, SmlCopy_t,
		*SmlReplacePtr_t, SmlReplace_t,
		*SmlDeletePtr_t, SmlDelete_t,
		*SmlMovePtr_t, SmlMove_t,
		*SmlGenericCmdPtr_t, SmlGenericCmd_t;

// Changes end

/**
 * Alert command:
 **/
typedef struct sml_alert_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;      
	Flag_t                flags;      // NoResp
	SmlCredPtr_t          cred;       // opt.
	SmlPcdataPtr_t        data;       // opt.
	// FOTA
	SmlPcdataPtr_t        correlator;	// opt.
	// FOTA end	
	SmlItemListPtr_t      itemList;

public:
	IMPORT_C sml_alert_s();
	IMPORT_C ~sml_alert_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlAlertPtr_t, SmlAlert_t;
 
 
/**
 * Atomic/Sequence command:
 **/
typedef struct sml_atomic_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;     
	Flag_t                flags;      // NoResp
	SmlPcdataPtr_t        meta;       // opt.

public:
	IMPORT_C sml_atomic_s(SmlProtoElement_t aType);
	IMPORT_C ~sml_atomic_s();
	
	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	}	*SmlAtomicPtr_t, SmlAtomic_t,
		*SmlSequencePtr_t, SmlSequence_t;


/**
 * Sync command:
 **/
typedef struct sml_sync_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;      
	Flag_t                flags;      // NoResp
	SmlCredPtr_t          cred;       // opt.
	SmlTargetPtr_t        target;     // opt.
	SmlSourcePtr_t        source;     // opt.
	SmlPcdataPtr_t        meta;       // opt.
	SmlPcdataPtr_t        noc;        // opt.
	
public:
	IMPORT_C sml_sync_s();
	IMPORT_C ~sml_sync_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlSyncPtr_t, SmlSync_t;


/**
 * Exec command:
 **/
typedef struct sml_exec_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType;
	SmlPcdataPtr_t        cmdID;      
	Flag_t                flags;      // NoResp
	SmlCredPtr_t          cred;       // opt.
	SmlPcdataPtr_t        meta;       // opt.
	// FOTA
	SmlPcdataPtr_t        correlator;	// opt.
	// FOTA end	
	SmlItemPtr_t          item;

public:
	IMPORT_C sml_exec_s();
	IMPORT_C ~sml_exec_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlExecPtr_t, SmlExec_t;


/**
 * Get and Put command:
 **/
typedef struct sml_get_put_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;      
	Flag_t                flags;      // NoResp
	SmlPcdataPtr_t        lang;       // opt.
	SmlCredPtr_t          cred;       // opt.
	SmlPcdataPtr_t        meta;       // opt.
	SmlItemListPtr_t         itemList;

public:
	IMPORT_C sml_get_put_s();
	IMPORT_C ~sml_get_put_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	}	*SmlPutPtr_t, SmlPut_t,
		*SmlGetPtr_t, SmlGet_t;


/**
 * Map command:
 **/ 
typedef struct sml_map_item_s : public CXMLElement
	{
public:
	SmlTargetPtr_t           target;
	SmlSourcePtr_t           source;
	TInt 					 atomicId;

public:
	IMPORT_C sml_map_item_s();
	IMPORT_C ~sml_map_item_s();
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams()	);
	} *SmlMapItemPtr_t, SmlMapItem_t;

typedef struct sml_map_item_list_s : public CBase
	{
public:
	SmlMapItemPtr_t          	mapItem;
	struct sml_map_item_list_s  *next;

public:
	IMPORT_C sml_map_item_list_s();
	IMPORT_C ~sml_map_item_list_s();
	} *SmlMapItemListPtr_t, SmlMapItemList_t;

typedef struct sml_map_s : public CXMLElement
	{
public:
	SmlProtoElement_t	  elementType; // InternalToolkit Field
	SmlPcdataPtr_t        cmdID;   
	SmlTargetPtr_t        target;
	SmlSourcePtr_t        source;
	SmlCredPtr_t          cred;       // opt.
	SmlPcdataPtr_t        meta;       // opt.
	SmlMapItemListPtr_t   mapItemList;

public:
	IMPORT_C sml_map_s();
	IMPORT_C ~sml_map_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlMapPtr_t, SmlMap_t;



/**
 * Results command:
 **/
typedef struct sml_results_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;
	SmlPcdataPtr_t        msgRef;     // opt.
	SmlPcdataPtr_t        cmdRef;
	SmlPcdataPtr_t        meta;       // opt.
	SmlPcdataPtr_t        targetRef;  // opt.
	SmlPcdataPtr_t        sourceRef;  // opt.
	SmlItemListPtr_t      itemList;
	
public:
	IMPORT_C sml_results_s();
	IMPORT_C ~sml_results_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlResultsPtr_t, SmlResults_t;


/**
 * Search command:
 **/
typedef struct sml_search_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID; 
	Flag_t                flags;      // NoResp, NoResults
	SmlCredPtr_t          cred;       // opt.
	SmlTargetPtr_t        target;     // opt.
	SmlSourceListPtr_t    sourceList;
	SmlPcdataPtr_t        lang;       // opt.
	SmlPcdataPtr_t        meta;
	SmlPcdataPtr_t        data;

public:
	IMPORT_C sml_search_s();
	IMPORT_C ~sml_search_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlSearchPtr_t, SmlSearch_t;


/**
 * Status command:
 **/

typedef struct sml_target_ref_list_s : public CBase
	{
public:
	SmlPcdataPtr_t              targetRef;
	struct sml_target_ref_list_s    *next;

public:
	IMPORT_C sml_target_ref_list_s();
	IMPORT_C ~sml_target_ref_list_s();
	} *SmlTargetRefListPtr_t, SmlTargetRefList_t;

typedef struct sml_source_ref_list_s : public CBase
	{
public:
	SmlPcdataPtr_t              sourceRef;
	struct sml_source_ref_list_s    *next;

public:
	IMPORT_C sml_source_ref_list_s();
	IMPORT_C ~sml_source_ref_list_s();
	} *SmlSourceRefListPtr_t, SmlSourceRefList_t;

typedef struct sml_status_s : public CXMLElement
	{
public:
	SmlProtoElement_t	      elementType; // Internal Toolkit Field
	SmlPcdataPtr_t        cmdID;          
	SmlPcdataPtr_t        msgRef; // Opt.
	SmlPcdataPtr_t        cmdRef;
	SmlPcdataPtr_t        cmd;
	SmlTargetRefListPtr_t targetRefList;  // opt.
	SmlSourceRefListPtr_t sourceRefList;  // opt.
	SmlCredPtr_t          cred;     // opt.
	SmlChalPtr_t          chal;           // opt.
	SmlPcdataPtr_t        data;
	SmlItemListPtr_t      itemList;       // opt.

public:
	IMPORT_C sml_status_s();
	IMPORT_C ~sml_status_s();

	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlStatusPtr_t, SmlStatus_t;

/**
 * a little helper for typecasting
 **/
typedef struct sml_unknown_proto_element_s : public CXMLElement
	{
public:
	SmlProtoElement_t	elementType;  // Internal Toolkit Field
	} *SmlUnknownProtoElementPtr_t, SmlUnknownProtoElement_t;

// ------------------------------------------------------------------------------------------------
// Own additions
// ------------------------------------------------------------------------------------------------
typedef struct sml_syncml_body_s : public CXMLElement
	{
public:
	Flag_t flags;      // NoResp, NoResults

public:
	TBool NeedsCleanup() const;
	CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlSyncBodyPtr_t, SmlSyncBody_t;

typedef struct sml_syncml_s : public CXMLElement
	{
public:
	IMPORT_C virtual TBool NeedsCleanup() const;
	IMPORT_C virtual CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
	IMPORT_C virtual CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	} *SmlSyncMLPtr_t, SmlSyncML_t;

// ------------------------------------------------------------------------------------------------
// Generic list adder
// ------------------------------------------------------------------------------------------------
template<class T>
void GenericListAddL( T* aList, T aNewItem )
	{
	if( *aList == 0 )
		{
		*aList = aNewItem;
		}
	else
		{
		T c;
		for( c = *aList; c->next; c = c->next )
			{};	
		c->next = aNewItem;
		}
	}

// ------------------------------------------------------------------------------------------------
// Generic list deleter
// ------------------------------------------------------------------------------------------------
template<class T>
void GenericListDelete( T* aList )
	{	
	T* c = aList;
	
	while ( c )
	    {
	    T* tmp = c;
	    c = c->next;
	    delete tmp;
	    tmp = NULL;    
	    }

	}
	

#endif // __SMLDTD_H__
