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
* Description:  Device information document generator implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "WBXMLDevInfGenerator.h"
#include "WBXMLGeneratorError.h"
#include "XMLWorkspace.h"

#include "smlsyncmltags.h"
#include "smldevinftags.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLDevInfGenerator
// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLDevInfGenerator::CreateWorkspaceL()
	{
	CWBXMLGenerator::CreateDynamicWorkspaceL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLDevInfGenerator::smlDeviceInfo( SmlDevInfDevInfPtr_t aContent )
	{
	Reset();
	Workspace()->BeginTransaction();
	TRAPD(result, AppendDeviceInfoL(aContent));
	return HandleResult(result);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}

	if( aContent->contentType == SML_PCDATA_OPAQUE )
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, aContent->length));
		}
	else
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, aContent->length), EWBXMLContentFormatInlineString);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList )
	{
	for( SmlPcdataListPtr_t p = aList; p && p->data; p = p->next )
		{
		AppendPCDataL(aElement, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendXmitL( TUint8 aTag, SmlDevInfXmitPtr_t aContent )
	{
	BeginElementL(aTag, ETrue);
	AppendPCDataL(EDevCTType, aContent->cttype);
	AppendPCDataL(EDevVerCT, aContent->verct);
	EndElementL(); // aTag
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendXmitListL( TUint8 aTag, SmlDevInfXmitListPtr_t aContent )
	{

	for( SmlDevInfXmitListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendXmitL(aTag, p->data);
		}
	}


// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendExtL( SmlDevInfExtPtr_t aContent )
	{
	BeginElementL(EDevExt, ETrue);
	AppendPCDataL(EDevXNam, aContent->xnam);
	AppendPCDataListL(EDevXVal, aContent->xval);
	EndElementL(); // EDevExt
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendExtListL( SmlDevInfExtListPtr_t aContent )
	{
	for( SmlDevInfExtListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendExtL(p->data);
		}
	}

// 1.2 CHANGES: new element FilterCap
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendFilterCapL( SmlDevInfFilterCapPtr_t aContent )
	{
	BeginElementL(EDevFilterCap, ETrue);
	AppendPCDataL(EDevCTType, aContent->cttype);
	AppendPCDataL(EDevVerCT, aContent->verct);
	AppendPCDataListL(EDevFilterKeyword, aContent->filterkeyword);
	AppendPCDataListL(EDevPropName, aContent->propname);
	EndElementL(); // EDevPropParam
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendFilterCapListL( SmlDevInfFilterCapListPtr_t aContent )
	{
	if (aContent && aContent->data)
		{
		for( SmlDevInfFilterCapListPtr_t p = aContent; p && p->data; p = p->next )
			{
			AppendFilterCapL(p->data);
			}
		}
	}
// Changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendSyncCapL( SmlDevInfSyncCapPtr_t aContent )
	{
	BeginElementL(EDevSyncCap, ETrue);
	AppendPCDataListL(EDevSyncType, aContent->synctype);
	EndElementL(); // EDevSyncCap
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendDSMemL( SmlDevInfDSMemPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(EDevDSMem, ETrue);
	if (aContent->shared)
		{
		BeginElementL(EDevSharedMem);
		}
	AppendPCDataL(EDevMaxMem, aContent->maxmem);
	AppendPCDataL(EDevMaxID, aContent->maxid);
	EndElementL(); // EDevDSMem
	}

// 1.2 CHANGES: new elements PropParam and Property
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPropParamL(SmlDevInfPropParamPtr_t aContent)
	{
	BeginElementL(EDevPropParam, ETrue);
	AppendPCDataL(EDevParamName, aContent->paramname);
	AppendPCDataL(EDevDataType, aContent->datatype);
	AppendPCDataListL(EDevValEnum, aContent->valenum);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	EndElementL(); // EDevPropParam
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPropParamListL(SmlDevInfPropParamListPtr_t aContent)
	{
	if (aContent && aContent->data)
		{
		for( SmlDevInfPropParamListPtr_t p = aContent; p && p->data; p = p->next )
			{
			AppendPropParamL(p->data);
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPropertyL( SmlDevInfPropertyPtr_t aContent)
	{
	BeginElementL(EDevProperty, ETrue);
	AppendPCDataL(EDevPropName, aContent->propname);
	AppendPCDataL(EDevDataType, aContent->datatype);
	AppendPCDataL(EDevMaxOccur, aContent->maxoccur);
	AppendPCDataL(EDevMaxSize, aContent->maxsize);
	if (aContent->notruncate)
		{
		BeginElementL(EDevNoTruncate);
		}
	AppendPCDataListL(EDevValEnum, aContent->valenum);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	AppendPropParamListL(aContent->propparam);
	EndElementL(); // EDevProperty
	}
	

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent )
	{
	for( SmlDevInfPropertyListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendPropertyL(p->data);
		}
	}
// Changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCTDataL( TUint8 aTag, SmlDevInfCTDataPtr_t aContent )
	{
	AppendPCDataL(aTag, aContent->name);
	if( aContent->valenum )
		{
		AppendPCDataListL(EDevValEnum, aContent->valenum);
		}
	else
		{
		AppendPCDataL(EDevDataType, aContent->datatype);
		AppendPCDataL(EDevMaxSize, aContent->size);  //1.2 CHANGES: EDevSize -> EDevMaxSize
		}
	AppendPCDataL(EDevDisplayName, aContent->dname);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCTDataPropL( SmlDevInfCTDataPropPtr_t aContent )
	{
	AppendCTDataL(EDevPropName, aContent->prop);
	AppendCTDataListL(aContent->param);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCTDataListL( SmlDevInfCTDataListPtr_t aContent )
	{
	for( SmlDevInfCTDataListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendCTDataL(EDevParamName, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCTDataPropListL( SmlDevInfCTDataPropListPtr_t aContent )
	{
	for( SmlDevInfCTDataPropListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendCTDataPropL(p->data);
		}
	}

// 1.2 CHANGES: version 1.1 and 1.2 support
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCtCapL( SmlDevInfCtCapPtr_t aContent )
	{
	if (iVerDtd == KDtDVersion12) 
		{
		BeginElementL(EDevCTCap, ETrue);
		AppendPCDataL(EDevCTType, aContent->cttype);
		AppendPCDataL(EDevVerCT, aContent->verct);
		if (aContent->fieldlevel) 
			{
			BeginElementL(EDevFieldLevel);
			}
		AppendPropertyListL(aContent->property);  
		EndElementL(); // EDevCTCap	
		} 
	else
		{
		AppendPCDataL(EDevCTType, aContent->cttype);
		AppendCTDataPropListL(aContent->prop);
		}
	}
// changes end


// 1.2 CHANGES: version 1.1 and 1.2 support
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendCtCapListL( SmlDevInfCtCapListPtr_t aContent )
	{
	if (iVerDtd == KDtDVersion11) 
		{
		BeginElementL(EDevCTCap, ETrue);
		}
	for( SmlDevInfCtCapListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendCtCapL(p->data);
		}
	if (iVerDtd == KDtDVersion11)
		{
		EndElementL(); // EDevCtCap
		}
	}
// changes end

//1.2 CHANGES: version 1.1 and 1.2 support
// new elements supportHierarchicalSync, FilterRx and FilterCap in version 1.2
// CtCap inside Datastore -element in version 1.2
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendDatastoreL( SmlDevInfDatastorePtr_t aContent )
	{
	BeginElementL(EDevDataStore, ETrue);
	AppendPCDataL(EDevSourceRef, aContent->sourceref);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	AppendPCDataL(EDevMaxGUIDSize, aContent->maxguidsize);
	AppendXmitL(EDevRxPref, aContent->rxpref);
	AppendXmitListL(EDevRx, aContent->rx);
	AppendXmitL(EDevTxPref, aContent->txpref);
	AppendXmitListL(EDevTx, aContent->tx);
	if ( iVerDtd == KDtDVersion12 )
		{
		AppendCtCapListL(aContent->ctcap);
		}
	AppendDSMemL(aContent->dsmem);
	if (( iVerDtd == KDtDVersion12 ) && ( aContent->supportHierarchicalSync ))
		{
		BeginElementL(EDevSupportHierarchicalSync);
		}
	AppendSyncCapL(aContent->synccap);
	if ( iVerDtd == KDtDVersion12 )
		{		
		AppendXmitListL(EDevFilterRx, aContent->filterrx); 
		AppendFilterCapListL(aContent->filtercap); 
		}
	EndElementL(); // EDevDataStore
	}
// changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendDatastoreListL( SmlDevInfDatastoreListPtr_t aContent )
	{
	for( SmlDevInfDatastoreListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendDatastoreL(p->data);
		}
	}

//1.2 CHANGES: version 1.1 and 1.2 support
// in version 1.1. CtCap element inside DeviceInfo -element
// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfGenerator::AppendDeviceInfoL( SmlDevInfDevInfPtr_t aContent )
	{

	TPtrC8 ver;
	ver.Set(aContent->verdtd->Data());

	if ( ver.FindC(_L8("1.1")) == KErrNone ) 
		{
		iVerDtd = KDtDVersion11;
		BeginDocumentL(KNSmlDevInfVersion, KNSmlDevInfPublicId, KNSmlDevInfUTF8);
		} 
	else
		{
		iVerDtd = KDtDVersion12;
		BeginDocumentL(KNSmlDevInfVersion, KNSmlDevInfPublicId12, KNSmlDevInfUTF8);
		}
	
	BeginElementL(EDevDevInf, ETrue);
	AppendPCDataL(EDevVerDTD, aContent->verdtd);
	AppendPCDataL(EDevMan, aContent->man);
	AppendPCDataL(EDevMod, aContent->mod);
	AppendPCDataL(EDevOEM, aContent->oem);
	AppendPCDataL(EDevFwV, aContent->fwv);
	AppendPCDataL(EDevSwV, aContent->swv);
	AppendPCDataL(EDevHwV, aContent->hwv);
	AppendPCDataL(EDevDevID, aContent->devid);
	AppendPCDataL(EDevDevTyp, aContent->devtyp);
	if( aContent->flags & SmlUTC_f )
		{
		BeginElementL(EDevUTC);
		}
	if( aContent->flags & SmlSupportLargeObjects_f )
		{
		BeginElementL(EDevSupportLargeObjs);
		}
	if( aContent->flags & SmlSupportNumberOfChanges_f )
		{
		BeginElementL(EDevSupportNumberOfChanges);
		}
	AppendDatastoreListL(aContent->datastore);
	if (iVerDtd == KDtDVersion11)
		{
		AppendCtCapListL(aContent->ctcap);
		}
	AppendExtListL(aContent->ext);
	EndElementL(); // EDevDevInf
	}
// changes end
