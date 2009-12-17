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
* Description:  XML device information DTD generator
*
*/


// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "XMLDevInfGenerator.h"
#include "WBXMLGeneratorError.h"
#include "XMLWorkspace.h"

#include "smlsyncmltags.h"
#include "smldevinftags.h"

_LIT8( KDevInfElements, 
	   "<0>|<1>|<2>|<3>|<4>|CTCap|CTType|DataStore|DataType|DevID|DevInf|DevTyp|DisplayName|DSMem|Ext|"
	   "FwV|HwV|Man|MaxGUIDSize|MaxID|MaxMem|Mod|OEM|ParamName|PropName|Rx|Rx-Pref|"
	   "SharedMem|MaxSize|SourceRef|SwV|SyncCap|SyncType|Tx|Tx-Pref|ValEnum|VerCT|"
	   "VerDTD|Xnam|Xval|UTC|SupportNumberOfChanges|SupportLargeObjs|"
// 1.2 CHANGES: new elements
	   "Property|PropParam|MaxOccur|NoTruncate|2FEmpty|Filter-Rx|FilterCap|FilterKeyword|FieldLevel|SupportHierarchicalSync" );
// changes end
_LIT8( KDevInfNamespace, "syncml:devinf");

// ------------------------------------------------------------------------------------------------
// CXMLDevInfGenerator
// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLDevInfGenerator::CreateWorkspaceL()
	{
	CXMLGenerator::CreateDynamicWorkspaceL();
	CXMLGenerator::SetNamespaceNameL(KDevInfNamespace());
	CXMLGenerator::SetTranslateTableL(KDevInfElements());
	}


// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLDevInfGenerator::InitializeL()
	{
	CXMLGenerator::SetNamespaceNameL(KDevInfNamespace());
	CXMLGenerator::SetTranslateTableL(KDevInfElements());
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLDevInfGenerator::smlDeviceInfo( SmlDevInfDevInfPtr_t aContent )
	{
	Reset();
	Workspace()->BeginTransaction();
	TRAPD(result, AppendDeviceInfoL(aContent));
	return HandleResult(result);
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent )
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
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, aContent->length), EXMLContentFormatInlineString);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList )
	{
	for( SmlPcdataListPtr_t p = aList; p && p->data; p = p->next )
		{
		AppendPCDataL(aElement, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendXmitL( TUint8 aTag, SmlDevInfXmitPtr_t aContent )
	{
	BeginElementL(aTag, ETrue);
	AppendPCDataL(EDevCTType, aContent->cttype);
	AppendPCDataL(EDevVerCT, aContent->verct);
	EndElementL(); // aTag
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendXmitListL( TUint8 aTag, SmlDevInfXmitListPtr_t aContent )
	{
	for( SmlDevInfXmitListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendXmitL(aTag, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendExtL( SmlDevInfExtPtr_t aContent )
	{
	BeginElementL(EDevExt, ETrue);
	AppendPCDataL(EDevXNam, aContent->xnam);
	AppendPCDataListL(EDevXVal, aContent->xval);
	EndElementL(); // EDevExt
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendExtListL( SmlDevInfExtListPtr_t aContent )
	{
	for( SmlDevInfExtListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendExtL(p->data);
		}
	}

// 1.2 CHANGES: new element FilterCap
// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendFilterCapL( SmlDevInfFilterCapPtr_t aContent )
	{
	BeginElementL(EDevFilterCap, ETrue);
	AppendPCDataL(EDevCTType, aContent->cttype);
	AppendPCDataL(EDevVerCT, aContent->verct);
	AppendPCDataListL(EDevFilterKeyword, aContent->filterkeyword);
	AppendPCDataListL(EDevPropName, aContent->propname);
	EndElementL(); // EDevPropParam
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendFilterCapListL( SmlDevInfFilterCapListPtr_t aContent )
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
void CXMLDevInfGenerator::AppendSyncCapL( SmlDevInfSyncCapPtr_t aContent )
	{
	BeginElementL(EDevSyncCap, ETrue);
	AppendPCDataListL(EDevSyncType, aContent->synctype);
	EndElementL(); // EDevSyncCap
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendDSMemL( SmlDevInfDSMemPtr_t aContent )
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
void CXMLDevInfGenerator::AppendPropParamL(SmlDevInfPropParamPtr_t aContent)
	{
	BeginElementL(EDevPropParam, ETrue);
	AppendPCDataL(EDevParamName, aContent->paramname);
	AppendPCDataL(EDevDataType, aContent->datatype);
	AppendPCDataListL(EDevValEnum, aContent->valenum);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	EndElementL(); // EDevPropParam
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendPropParamListL(SmlDevInfPropParamListPtr_t aContent)
	{
	for( SmlDevInfPropParamListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendPropParamL(p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendPropertyL( SmlDevInfPropertyPtr_t aContent)
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
void CXMLDevInfGenerator::AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent )
	{
	if (aContent && aContent->data)
		{
		for( SmlDevInfPropertyListPtr_t p = aContent; p && p->data; p = p->next )
			{
			AppendPropertyL(p->data);
			}
		}
	}
// Changes end

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendCTDataL( TUint8 aTag, SmlDevInfCTDataPtr_t aContent )
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
void CXMLDevInfGenerator::AppendCTDataPropL( SmlDevInfCTDataPropPtr_t aContent )
	{
	AppendCTDataL(EDevPropName, aContent->prop);
	AppendCTDataListL(aContent->param);
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendCTDataListL( SmlDevInfCTDataListPtr_t aContent )
	{
	for( SmlDevInfCTDataListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendCTDataL(EDevParamName, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendCTDataPropListL( SmlDevInfCTDataPropListPtr_t aContent )
	{
	for( SmlDevInfCTDataPropListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendCTDataPropL(p->data);
		}
	}

// 1.2 CHANGES: version 1.1 and 1.2 support
// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendCtCapL( SmlDevInfCtCapPtr_t aContent )
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
void CXMLDevInfGenerator::AppendCtCapListL( SmlDevInfCtCapListPtr_t aContent )
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

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendDatastoreL( SmlDevInfDatastorePtr_t aContent )
	{
	BeginElementL(EDevDataStore, ETrue);
	if ( aContent->sourceref )
		{
		AppendPCDataL(EDevSourceRef, aContent->sourceref);
		}
	if ( aContent->displayname )
		{
		AppendPCDataL(EDevDisplayName, aContent->displayname);
		}
	if ( aContent->maxguidsize )
		{
		AppendPCDataL(EDevMaxGUIDSize, aContent->maxguidsize);
		}
	if ( aContent->rxpref )
		{
		AppendXmitL(EDevRxPref, aContent->rxpref);
		}
	if ( aContent->rx )
		{
		AppendXmitListL(EDevRx, aContent->rx);
		}
	if ( aContent->txpref )
		{
		AppendXmitL(EDevTxPref, aContent->txpref);
		}
	if ( aContent->tx )
		{
		AppendXmitListL(EDevTx, aContent->tx);
		}
	if (iVerDtd == KDtDVersion12)
		{
		AppendCtCapListL(aContent->ctcap); // 1.2 CHANGES: CtCap moved here in 1.2
		}
	if ( aContent->dsmem )
		{
		AppendDSMemL(aContent->dsmem);
		}
	if (aContent->supportHierarchicalSync)
		{
		BeginElementL(EDevSupportHierarchicalSync);
		}
	if ( aContent->synccap )
		{
		AppendSyncCapL(aContent->synccap);
		}
	if (iVerDtd == KDtDVersion12)
		{
		if ( aContent->filterrx )
			{
			AppendXmitListL(EDevFilterRx, aContent->filterrx); // 1.2 CHANGES: new element
			}
		if ( aContent->filtercap )
			{
			AppendFilterCapListL(aContent->filtercap); // 1.2 CHANGES:new element
			}
		}
	EndElementL(); // EDevDataStore
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendDatastoreListL( SmlDevInfDatastoreListPtr_t aContent )
	{
	for( SmlDevInfDatastoreListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendDatastoreL(p->data);
		}
	}

//1.2 CHANGES: version 1.1 and 1.2 support
// in version 1.1. CtCap element inside DeviceInfo -element
// ------------------------------------------------------------------------------------------------
void CXMLDevInfGenerator::AppendDeviceInfoL( SmlDevInfDevInfPtr_t aContent )
	{

	TPtrC8 ver;
	ver.Set(aContent->verdtd->Data());

	if ( ver.FindC(_L8("1.1")) == KErrNone ) 
		{
		iVerDtd = KDtDVersion11;
		} 
	else
		{
		iVerDtd = KDtDVersion12;
		}
	BeginDocumentL(KNSmlDevInfVersion, KNSmlDevInfPublicId, KNSmlDevInfUTF8);
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

//End of File
