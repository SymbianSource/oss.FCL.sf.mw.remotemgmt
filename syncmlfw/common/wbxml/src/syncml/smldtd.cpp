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
* Description:  SyncML command struct implementations.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "smldtd.h"
#include "smlsyncmltags.h"
#include "smlmetinfdtd.h"
#include "smldevinfdtd.h"
#include "WBXMLSyncMLParser.h"
#include "WBXMLParserError.h"

// ------------------------------------------------------------------------------------------------
// Macros
// ------------------------------------------------------------------------------------------------
#define CHECK_NO_DUPLICATE(a) if( a ) { User::Leave(KErrParserErrorInvalidDocument); }

// ------------------------------------------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------------------------------------------
CXMLElement* AddPCDataL( SmlPcdataListPtr_t* aList )
	{
	SmlPcdataListPtr_t itemL = new (ELeave) SmlPcdataList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlPcdata_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddItemL( SmlItemListPtr_t* aList )
	{
	SmlItemListPtr_t itemL = new (ELeave) SmlItemList_t();
	CleanupStack::PushL(itemL);
	itemL->item = new (ELeave) SmlItem_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->item;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddMapItemL( SmlMapItemListPtr_t* aList )
	{
	SmlMapItemListPtr_t itemL = new (ELeave) SmlMapItemList_t();
	CleanupStack::PushL(itemL);
	itemL->mapItem = new (ELeave) SmlMapItem_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->mapItem;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddSourceListL( SmlSourceListPtr_t* aList )
	{
	SmlSourceListPtr_t itemL = new (ELeave) SmlSourceList_t();
	CleanupStack::PushL(itemL);
	itemL->source = new (ELeave) SmlSource_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->source;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddTargetRefL( SmlTargetRefListPtr_t* aList )
	{
	SmlTargetRefListPtr_t itemL = new (ELeave) SmlTargetRefList_t();
	CleanupStack::PushL(itemL);
	itemL->targetRef = new (ELeave) SmlPcdata_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->targetRef;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddSourceRefL( SmlSourceRefListPtr_t* aList )
	{
	SmlSourceRefListPtr_t itemL = new (ELeave) SmlSourceRefList_t();
	CleanupStack::PushL(itemL);
	itemL->sourceRef = new (ELeave) SmlPcdata_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->sourceRef;
	}

// ------------------------------------------------------------------------------------------------
LOCAL_D TBool CreateBasicCommandL( CXMLElement*& aElement, TUint8 aTag )
	{
	aElement = 0;
	switch( aTag )
		{
		case EMap:
			aElement = new (ELeave) SmlMap_t();
			return ETrue;

		case EAlert:
			aElement = new (ELeave) SmlAlert_t();
			return ETrue;
		
		case EAtomic:
			aElement = new (ELeave) SmlAtomic_t(SML_PE_ATOMIC_START);
			return ETrue;

		case EAdd:
		case ECopy:
		case EDelete:
		case EReplace:
		case EMove:
			aElement = new (ELeave) SmlCopy_t();
			return ETrue;

		case EGet:
		case EPut:
			aElement = new (ELeave) SmlGet_t();
			return ETrue;

		case EExec:
			aElement = new (ELeave) SmlExec_t();
			return ETrue;

		case EResults:
			aElement = new (ELeave) SmlResults_t();
			return ETrue;

		case ESearch:
			aElement = new (ELeave) SmlSearch_t();
			return ETrue;

		case ESequence:
			aElement = new (ELeave) SmlSequence_t(SML_PE_SEQUENCE_START);
			return ETrue;

		case EStatus:
			aElement = new (ELeave) SmlStatus_t();
			return ETrue;

		case ESync:
			aElement = new (ELeave) SmlSync_t();
			return ETrue;

		default:
			break;
		}
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
// sml_pcdata_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_pcdata_s::sml_pcdata_s( const SmlPcdataType_t aType, const SmlPcdataExtension_t aExt )
	: contentType(aType), extension(aExt)
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_pcdata_s::sml_pcdata_s() : contentType(SML_PCDATA_OPAQUE), extension(SML_EXT_UNDEFINED)
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_pcdata_s::~sml_pcdata_s()
	{
	FreeContent();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void sml_pcdata_s::FreeContent()
	{
	if( extension == SML_EXT_METINF )
		{
		delete SmlMetInfMetInfPtr_t(content);
		}
    else if( extension == SML_EXT_DEVINFPROP )
		{
        delete SmlDevInfPropertyListPtr_t(content);            
		}
	else
		{
		User::Free(content);
		}
	content = 0;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void sml_pcdata_s::SetDataL( const TDesC8& aData )
	{
	FreeContent();
	length = aData.Length();
	content = User::AllocL(length);
	Mem::Copy(content, aData.Ptr(), length);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 sml_pcdata_s::Data() const
	{
	return TPtrC8((TUint8*)content, length);
	}

	// ------------------------------------------------------------------------------------------------
EXPORT_C void sml_pcdata_s::TruncateL( TInt aConsumed )
	{
	HBufC8* buffer = TPtrC8((TUint8*)content + aConsumed, length - aConsumed).AllocLC();
	SetDataL(*buffer);
	CleanupStack::PopAndDestroy(); // buffer
	}

// ------------------------------------------------------------------------------------------------
// sml_pcdata_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_pcdata_list_s::sml_pcdata_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_pcdata_list_s::~sml_pcdata_list_s()
	{
	delete data;
	}

// ------------------------------------------------------------------------------------------------
// sml_chal_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_chal_s::sml_chal_s()
	{
	delete meta;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_chal_s::~sml_chal_s()
	{
	delete meta;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_chal_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t( /*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_cred_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_cred_s::sml_cred_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_cred_s::~sml_cred_s()
	{
	delete data;
	delete meta;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_cred_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EData:
			CHECK_NO_DUPLICATE(data);
			data = new (ELeave) SmlPcdata_t();
			return data;

		case EMeta:
			CHECK_NO_DUPLICATE(meta)
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// 1.2 CHANGES: new common use elements
// ------------------------------------------------------------------------------------------------
// sml_field_or_record_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_field_or_record_s::sml_field_or_record_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_field_or_record_s::~sml_field_or_record_s()
	{
	delete item;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_field_or_record_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EItem:
			CHECK_NO_DUPLICATE(item);
			item = new (ELeave) SmlItem_t();
			return item;
			
		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}


// ------------------------------------------------------------------------------------------------
// sml_filter_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_filter_s::sml_filter_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_filter_s::~sml_filter_s()
	{
	delete meta;
	delete field;
	delete record;
	delete filterType;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_filter_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t();
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EField:
			CHECK_NO_DUPLICATE(field);
			field = new (ELeave) SmlField_t();
			return field;

		case ERecord:
			CHECK_NO_DUPLICATE(record);
			record = new (ELeave) SmlRecord_t();
			return record;

		case EFilterType:
			CHECK_NO_DUPLICATE(filterType);
			filterType = new (ELeave) SmlPcdata_t();
			return filterType;
			
		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}
// Changes end

// 1.2 CHANGES: Source & Target divided to their own structs

// ------------------------------------------------------------------------------------------------
// sml_source_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_s::sml_source_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_s::~sml_source_s()
	{
	delete locURI;
	delete locName;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_source_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ELocURI:
			CHECK_NO_DUPLICATE(locURI);
			locURI = new (ELeave) SmlPcdata_t();
			return locURI;

		case ELocName:
			CHECK_NO_DUPLICATE(locName);
			locName = new (ELeave) SmlPcdata_t();
			return locName;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_target_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_target_s::sml_target_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_target_s::~sml_target_s()
	{
	delete locURI;
	delete locName;
	delete filter;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_target_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ELocURI:
			CHECK_NO_DUPLICATE(locURI);
			locURI = new (ELeave) SmlPcdata_t();
			return locURI;
			
		case ELocName:
			CHECK_NO_DUPLICATE(locName);
			locName = new (ELeave) SmlPcdata_t();
			return locName;

		case EFilter:
			CHECK_NO_DUPLICATE(filter);
			filter = new (ELeave) SmlFilter_t();
			return filter;
			
		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}
// Changes end

// ------------------------------------------------------------------------------------------------
// sml_source_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_list_s::sml_source_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_list_s::~sml_source_list_s()
	{
	delete source;
	}

//1.2 CHANGES: Source & Target parent
// ------------------------------------------------------------------------------------------------
// sml_source_or_target_parent_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_or_target_parent_s::sml_source_or_target_parent_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_or_target_parent_s::~sml_source_or_target_parent_s()
	{
	delete locURI;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_source_or_target_parent_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ELocURI:
			CHECK_NO_DUPLICATE(locURI);
			locURI = new (ELeave) SmlPcdata_t();
			return locURI;
			
		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}
// Changes end


// ------------------------------------------------------------------------------------------------
// sml_sync_hdr_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_sync_hdr_s::sml_sync_hdr_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_sync_hdr_s::~sml_sync_hdr_s()
	{
	delete version;       
	delete proto;         
	delete sessionID;     
	delete msgID;         
	delete target;
	delete source;
	delete respURI;
	delete cred;
	delete meta;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_sync_hdr_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EVerDTD:
			CHECK_NO_DUPLICATE(version);
			version = new (ELeave) SmlPcdata_t();
			return version;

		case EVerProto:
			CHECK_NO_DUPLICATE(proto);
			proto = new (ELeave) SmlPcdata_t();
			return proto;

		case ESessionID:
			CHECK_NO_DUPLICATE(sessionID);
			sessionID = new (ELeave) SmlPcdata_t();
			return sessionID;

		case EMsgID:
			CHECK_NO_DUPLICATE(msgID);
			msgID = new (ELeave) SmlPcdata_t();
			return msgID;

		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			CHECK_NO_DUPLICATE(source);
			source = new (ELeave) SmlSource_t();
			return source;

		case ERespURI:
			CHECK_NO_DUPLICATE(respURI);
			respURI = new (ELeave) SmlPcdata_t();
			return respURI;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_sync_hdr_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case ESyncHdr:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlStartMessageFuncL(this);
			return EPopAndDestroy;
		
		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_sync_hdr_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_item_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_item_s::sml_item_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_item_s::~sml_item_s()
	{
	delete target;
	delete source;
	delete sourceParent;
	delete targetParent;
	delete meta;
	delete data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_item_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			CHECK_NO_DUPLICATE(source);
			source = new (ELeave) SmlSource_t();
			return source;

// 1.2 CHANGES: SourceParent and TargetParent added
		case ESourceParent:
			CHECK_NO_DUPLICATE(sourceParent);
			sourceParent = new (ELeave) SmlSourceParent_t();
			return sourceParent;

		case ETargetParent:
			CHECK_NO_DUPLICATE(targetParent);
			targetParent = new (ELeave) SmlTargetParent_t();
			return targetParent;
// End Changes

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EData:
			CHECK_NO_DUPLICATE(data);
			data = new (ELeave) SmlPcdata_t();
			return data;

		case EMoreData:
			CHECK_NO_DUPLICATE(flags & SmlMoreData_f);
			flags |= SmlMoreData_f;
			break;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_item_s::EndElementL( TAny* /*aCallbacks*/, TUint8 aTag )
	{
	if( aTag == EMoreData )
		{
		return ENone;
		}
	return EPop;
	}

// ------------------------------------------------------------------------------------------------
// sml_item_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_item_list_s::sml_item_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_item_list_s::~sml_item_list_s()
	{
	delete item;
	}

// ------------------------------------------------------------------------------------------------
// sml_generic_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_generic_s::sml_generic_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_generic_s::~sml_generic_s()
	{
	delete cmdID;      
	delete cred;
	delete meta;
	GenericListDelete( itemList );    		
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_generic_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case EArchive:
			CHECK_NO_DUPLICATE(flags & SmlArchive_f);
			flags |= SmlArchive_f;
			break;
		
		case ESftDel:
			CHECK_NO_DUPLICATE(flags & SmlSftDel_f);
			flags |= SmlSftDel_f;
			break;

		case EItem:
			return AddItemL(&itemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_generic_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EAdd:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlAddCmdFuncL(this);
			return EPopAndDestroy;

		case ECopy:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlCopyCmdFuncL(this);
			return EPopAndDestroy;
		
		case EDelete:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlDeleteCmdFuncL(this);
			return EPopAndDestroy;
		
		case EReplace:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlReplaceCmdFuncL(this);
			return EPopAndDestroy;

		case EMove:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlMoveCmdFuncL(this);
			return EPopAndDestroy;

		case ENoResp:
		case EArchive:
		case ESftDel:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_generic_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_alert_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_alert_s::sml_alert_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_alert_s::~sml_alert_s()
	{
	delete cmdID;      
	delete cred;
	delete data;
	// FOTA
	delete correlator;
	// FOTA end	
	GenericListDelete( itemList );	
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_alert_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EData:
			CHECK_NO_DUPLICATE(data);
			data = new (ELeave) SmlPcdata_t();
			return data;
// FOTA
		case ECorrelator:
			CHECK_NO_DUPLICATE(correlator);
			correlator = new (ELeave) SmlPcdata_t();
			return correlator;
// FOTA end			

		case EItem:
			return AddItemL(&itemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_alert_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EAlert:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlAlertCmdFuncL(this);
			return EPopAndDestroy;

		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_alert_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_atomic_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_atomic_s::sml_atomic_s( SmlProtoElement_t aType )
	{
	elementType = aType;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_atomic_s::~sml_atomic_s()
	{
	delete cmdID;      
	delete meta;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_atomic_s::BeginElementL( TUint8 aTag, const TXMLElementParams& aParams )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		default:
			CXMLElement* e;
			if( CreateBasicCommandL(e, aTag) )
				{
				if( this->elementType == SML_PE_ATOMIC_START )
					{
					this->elementType = SML_PE_ATOMIC_END;
					((MWBXMLSyncMLCallbacks*)aParams.Callbacks())->smlStartAtomicFuncL(this);
					}
				else if( this->elementType == SML_PE_SEQUENCE_START )
					{
					this->elementType = SML_PE_SEQUENCE_END;
					((MWBXMLSyncMLCallbacks*)aParams.Callbacks())->smlStartSequenceFuncL(this);
					}
				}
			else
				{
				User::Leave(KWBXMLParserErrorInvalidTag);
				}
			return e;
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_atomic_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EAtomic:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlEndAtomicFuncL();
			return EPopAndDestroy;

		case ESequence:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlEndSequenceFuncL();
			return EPopAndDestroy;

		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_atomic_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_sync_s
// ------------------------------------------------------------------------------------------------


EXPORT_C sml_sync_s::sml_sync_s()
	{
	elementType = SML_PE_SYNC_START;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_sync_s::~sml_sync_s()
	{
	delete cmdID;
	delete cred;
	delete target;
	delete source;
	delete meta;
	delete noc;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_sync_s::BeginElementL( TUint8 aTag, const TXMLElementParams& aParams )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			CHECK_NO_DUPLICATE(source);
			source = new (ELeave) SmlSource_t();
			return source;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case ENumberOfChanges:
			CHECK_NO_DUPLICATE(noc);
			noc = new (ELeave) SmlPcdata_t();
			return noc;

		default:
			CXMLElement* e;
			if( CreateBasicCommandL(e, aTag) )
				{
				if( this->elementType == SML_PE_SYNC_START )
					{
					this->elementType = SML_PE_SYNC_END;
					((MWBXMLSyncMLCallbacks*)aParams.Callbacks())->smlStartSyncFuncL(this);
					}
				}
			else
				{
				User::Leave(KWBXMLParserErrorInvalidTag);
				}
			return e;
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_sync_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case ESync:
			// If Sync element did not contain any commands -> call startsync and then end sync
			if( this->elementType == SML_PE_SYNC_START )
				{
				this->elementType = SML_PE_SYNC_END;
				((MWBXMLSyncMLCallbacks*)aCallbacks)->smlStartSyncFuncL(this);
				}
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlEndSyncFuncL();
			return EPopAndDestroy;

		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_sync_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_exec_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_exec_s::sml_exec_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_exec_s::~sml_exec_s()
	{
	delete cmdID;      
	delete cred;
	delete meta;
	// FOTA
	delete correlator;
	// FOTA end
	delete item;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_exec_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;
// FOTA
		case ECorrelator:
			CHECK_NO_DUPLICATE(correlator);
			correlator = new (ELeave) SmlPcdata_t();
			return correlator;
// FOTA end			

		case EItem:
			CHECK_NO_DUPLICATE(item);
			item = new (ELeave) SmlItem_t();
			return item;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_exec_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EExec:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlExecCmdFuncL(this);
			return EPopAndDestroy;

		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_exec_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_get_put_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_get_put_s::sml_get_put_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_get_put_s::~sml_get_put_s()
	{
	delete cmdID;      
	delete lang;
	delete cred;
	delete meta;
	GenericListDelete( itemList );  	
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_get_put_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case ELang:
			CHECK_NO_DUPLICATE(lang);
			lang = new (ELeave) SmlPcdata_t();
			return lang;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EItem:
			return AddItemL(&itemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_get_put_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EGet:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlGetCmdFuncL(this);
			return EPopAndDestroy;
		
		case EPut:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlPutCmdFuncL(this);
			return EPopAndDestroy;

		case ENoResp:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_get_put_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_map_item_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_item_s::sml_map_item_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_item_s::~sml_map_item_s()
	{
	delete source;
	delete target;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_map_item_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			CHECK_NO_DUPLICATE(source);
			source = new (ELeave) SmlSource_t();
			return source;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_map_item_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_item_list_s::sml_map_item_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_item_list_s::~sml_map_item_list_s()
	{
	delete mapItem;
	}

// ------------------------------------------------------------------------------------------------
// sml_map_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_s::sml_map_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_map_s::~sml_map_s()
	{
	delete cmdID;  
	delete target;
	delete source;
	delete cred;
	delete meta;
    GenericListDelete( mapItemList ); 
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_map_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			CHECK_NO_DUPLICATE(source);
			source = new (ELeave) SmlSource_t();
			return source;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EMapItem:
			return AddMapItemL(&mapItemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_map_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EMap:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlMapCmdFuncL(this);
			return EPopAndDestroy;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_map_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_results_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_results_s::sml_results_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_results_s::~sml_results_s()
	{
	delete cmdID;
	delete msgRef;
	delete cmdRef;
	delete meta;
	delete targetRef;
	delete sourceRef;
	GenericListDelete( itemList );	
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_results_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case EMsgRef:
			CHECK_NO_DUPLICATE(msgRef);
			msgRef = new (ELeave) SmlPcdata_t();
			return msgRef;

		case ECmdRef:
			CHECK_NO_DUPLICATE(cmdRef);
			cmdRef = new (ELeave) SmlPcdata_t();
			return cmdRef;

		case ETargetRef:
			CHECK_NO_DUPLICATE(targetRef);
			targetRef = new (ELeave) SmlPcdata_t();
			return targetRef;

		case ESourceRef:
			CHECK_NO_DUPLICATE(sourceRef);
			sourceRef = new (ELeave) SmlPcdata_t();
			return sourceRef;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EItem:
			return AddItemL(&itemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_results_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EResults:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlResultsCmdFuncL(this);
			return EPopAndDestroy;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_results_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_search_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_search_s::sml_search_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_search_s::~sml_search_s()
	{
	delete cmdID;
	delete cred;
	delete target;
	GenericListDelete( sourceList );
	delete lang;
	delete meta;
	delete data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_search_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case ENoResp:
			CHECK_NO_DUPLICATE(flags & SmlNoResp_f);
			flags |= SmlNoResp_f;
			break;

		case ENoResults:
			CHECK_NO_DUPLICATE(flags & SmlNoResults_f);
			flags |= SmlNoResults_f;
			break;

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case ETarget:
			CHECK_NO_DUPLICATE(target);
			target = new (ELeave) SmlTarget_t();
			return target;

		case ESource:
			return AddSourceListL(&sourceList);

		case ELang:
			CHECK_NO_DUPLICATE(lang);
			lang = new (ELeave) SmlPcdata_t();
			return lang;

		case EMeta:
			CHECK_NO_DUPLICATE(meta);
			meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
			meta->contentType = SML_PCDATA_EXTENSION;
			meta->extension = SML_EXT_METINF;
			return meta;

		case EData:
			CHECK_NO_DUPLICATE(data);
			data = new (ELeave) SmlPcdata_t();
			return data;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_search_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case ESearch:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlSearchCmdFuncL(this);
			return EPopAndDestroy;

		case ENoResp:
		case ENoResults:
			return ENone;


		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_search_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_target_ref_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_target_ref_list_s::sml_target_ref_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_target_ref_list_s::~sml_target_ref_list_s()
	{
	delete targetRef;
	}

// ------------------------------------------------------------------------------------------------
// sml_source_ref_list_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_ref_list_s::sml_source_ref_list_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_source_ref_list_s::~sml_source_ref_list_s()
	{
	delete sourceRef;
	}

// ------------------------------------------------------------------------------------------------
// sml_status_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_status_s::sml_status_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_status_s::~sml_status_s()
	{
	delete cmdID;          
	delete msgRef;
	delete cmdRef;
	delete cmd;
	GenericListDelete( targetRefList );
	GenericListDelete( sourceRefList );
	delete cred;
	delete chal;
	delete data;

	GenericListDelete( itemList );
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_status_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ECmdID:
			CHECK_NO_DUPLICATE(cmdID);
			cmdID = new (ELeave) SmlPcdata_t();
			return cmdID;

		case EMsgRef:
			CHECK_NO_DUPLICATE(msgRef);
			msgRef = new (ELeave) SmlPcdata_t();
			return msgRef;

		case ECmdRef:
			CHECK_NO_DUPLICATE(cmdRef);
			cmdRef = new (ELeave) SmlPcdata_t();
			return cmdRef;

		case ECmd:
			CHECK_NO_DUPLICATE(cmd);
			cmd = new (ELeave) SmlPcdata_t();
			return cmd;

		case ETargetRef:
			return AddTargetRefL(&targetRefList);

		case ESourceRef:
			return AddSourceRefL(&sourceRefList);

		case ECred:
			CHECK_NO_DUPLICATE(cred);
			cred = new (ELeave) SmlCred_t();
			return cred;

		case EChal:
			CHECK_NO_DUPLICATE(chal);
			chal = new (ELeave) SmlChal_t();
			return chal;

		case EData:
			CHECK_NO_DUPLICATE(data);
			data = new (ELeave) SmlPcdata_t();
			return data;

		case EItem:
			return AddItemL(&itemList);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_status_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EStatus:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlStatusCmdFuncL(this);
			return EPopAndDestroy;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_status_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_syncml_s
// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLElement* sml_syncml_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case ESyncHdr:
			return new (ELeave) SmlSyncHdr_t();

		case ESyncBody:
			return new (ELeave) SmlSyncBody_t();

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLElement::TAction sml_syncml_s::EndElementL( TAny* /*aCallbacks*/, TUint8 /*aTag*/ )
	{
	return EPopAndDestroy;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TBool sml_syncml_s::NeedsCleanup() const
	{
	return ETrue;
	}

// ------------------------------------------------------------------------------------------------
// sml_syncml_body_s
// ------------------------------------------------------------------------------------------------
CXMLElement* sml_syncml_body_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EFinal:
			CHECK_NO_DUPLICATE(flags & SmlFinal_f);
			flags |= SmlFinal_f;
			break;

		default:
			CXMLElement* e;
			if( !CreateBasicCommandL(e, aTag) )
				{
				User::Leave(KWBXMLParserErrorInvalidTag);
				}
			return e;
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_syncml_body_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case ESyncBody:
			((MWBXMLSyncMLCallbacks*)aCallbacks)->smlEndMessageFuncL((this->flags & SmlFinal_f) != 0);
			return EPopAndDestroy;

		case EFinal:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_syncml_body_s::NeedsCleanup() const
	{
	return ETrue;
	}
