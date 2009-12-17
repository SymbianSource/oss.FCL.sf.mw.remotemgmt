/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  XML syncml DTD generator
*
*/


// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "XMLSyncMLGenerator.h"
#include "WBXMLGeneratorError.h"
#include "XMLWorkspace.h"
#include "XMLDevInfGenerator.h"
#include "WBXMLDevInfParser.h"
#include "WBXMLParser.h"

#include "smlsyncmltags.h"
#include "smlmetinftags.h"

#include <s32mem.h>

// FOTA : Correlator added after 'Move'
_LIT8( KSyncMLElements, 
	   "<0>|<1>|<2>|<3>|<4>|Add|Alert|Archive|Atomic|Chal|Cmd|CmdID|CmdRef|Copy|Cred|Data|Delete|"
	   "Exec|Final|Get|Item|Lang|LocName|LocURI|Map|MapItem|Meta|MsgID|MsgRef|"
	   "NoResp|NoResults|Put|Replace|RespURI|Results|Search|Sequence|SessionID|"
	   "SftDel|Source|SourceRef|Status|Sync|SyncBody|SyncHdr|SyncML|Target|"
	   "TargetRef|<reserved>|VerDTD|VerProto|NumberOfChanges|MoreData|Field|Filter|Record|FilterType|"
	   "SourceParent|TargetParent|Move|Correlator|Data" );

_LIT8( KSyncMLNamespace, "SYNCML:SYNCML");


_LIT8( KMetInfElements, 
	   "<0>|<1>|<2>|<3>|<4>|Anchor|EMI|Format|FreeID|FreeMem|Last|Mark|MaxMsgSize|Mem|MetInf|Next|"
	   "NextNonce|SharedMem|Size|Type|Version|MaxObjSize|FieldLevel" );

_LIT8( KMetInfNamespace, "syncml:metinf");

// HYBRID 1.2 CHANGE
_LIT8( KDevInfGen, "./devinf" );
// Changes end

_LIT8( KDevInfElements, 
	   "<0>|<1>|<2>|<3>|<4>|CTCap|CTType|DataStore|DataType|DevID|DevInf|DevTyp|DisplayName|DSMem|Ext|"
	   "FwV|HwV|Man|MaxGUIDSize|MaxID|MaxMem|Mod|OEM|ParamName|PropName|Rx|Rx-Pref|"
	   "SharedMem|MaxSize|SourceRef|SwV|SyncCap|SyncType|Tx|Tx-Pref|ValEnum|VerCT|"
	   "VerDTD|Xnam|Xval|UTC|SupportNumberOfChanges|SupportLargeObjs|"
// 1.2 CHANGES: new elements
	   "Property|PropParam|MaxOccur|NoTruncate|Empty|Filter-Rx|FilterCap|FilterKeyword|FieldLevel|SupportHierarchicalSync" );
// changes end

// ------------------------------------------------------------------------------------------------
// CXMLSyncMLGenerator
// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLSyncMLGenerator::CreateWorkspaceL()
	{
	CXMLGenerator::CreateDynamicWorkspaceL();
	// HYBRID 1.2 CHANGE
	CXMLGenerator::SetTranslateTableL(KSyncMLElements());
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLSyncMLGenerator::InitializeL()
	{
	// HYBRID 1.2 CHANGE
	CXMLGenerator::SetTranslateTableL(KSyncMLElements());
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlStartMessage( SmlSyncHdrPtr_t aContent )
	{
	// HYBRID 1.2 CHANGE
	HBufC8* version=NULL;
	TRAPD(result, version = HBufC8::NewL( KSyncMLNamespace().Length() + aContent->version->Data().Length()));
	if(version)
	    {
    	TPtr8 versionPtr = version->Des();
    	versionPtr = KSyncMLNamespace();
    	versionPtr.Append ( aContent->version->Data() );
    	TRAP(result,CXMLGenerator::SetNamespaceNameL( versionPtr ));
    	delete version;
	    }

	Reset();
	Workspace()->BeginTransaction();
	TRAP(result, doSmlStartMessageL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlEndMessage( Boolean_t aFinal )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, doSmlEndMessageL(aFinal));
	return HandleResult(result); // No safe treshold as this is end of packet!
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlStartSync( SmlSyncPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendSyncL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlEndSync()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, EndElementL());
	return HandleResult(result);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlAddCmd( SmlAddPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendAddCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlReplaceCmd( SmlReplacePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendReplaceCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlCopyCmd( SmlCopyPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendCopyCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// FOTA
// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlExecCmd( SmlExecPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendExecCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}
// FOTA end
//1.2 CHANGES: Move command
// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlMoveCmd( SmlMovePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendMoveCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}
//Changes end

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlDeleteCmd( SmlDeletePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendDeleteCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlAlertCmd( SmlAlertPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendAlertCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlGetCmd( SmlPutPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendGetCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlPutCmd( SmlPutPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendPutCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlMapCmd( SmlMapPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendMapCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlResultsCmd( SmlResultsPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendResultsCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlStatusCmd( SmlStatusPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStatusCmdL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlStartAtomic( SmlAtomicPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStartAtomicL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlEndAtomic()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendEndAtomicL());
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlStartSequence( SmlSequencePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStartSequenceL(aContent));
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlEndSequence()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendEndSequenceL());
	return HandleResult(result, KNSmlXmlBufferSafeTreshold);
	}

// Data chunk functionality

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLSyncMLGenerator::SetTruncate( TBool aEnabled )
	{
	iUseTruncation = aEnabled;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLSyncMLGenerator::ItemIndex() const
	{
	return iItemIndex;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLSyncMLGenerator::DataConsumed() const
	{
	return iDataConsumed;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CXMLSyncMLGenerator::WasTruncated()
	{
	TBool was(iItemWasTruncated);
	iItemWasTruncated = EFalse;
	return was;
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::doSmlStartMessageL( SmlSyncHdrPtr_t aContent )
	{
	BeginDocumentL(KNSmlSyncMLVersion, KNSmlSyncMLPublicId, KNSmlSyncMLUTF8);
	BeginElementL(ESyncML, ETrue);
	AppendSyncHrdL(aContent);
	BeginElementL(ESyncBody, ETrue);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::doSmlEndMessageL( Boolean_t aFinal )
	{
	if( aFinal )
		{
		BeginElementL(EFinal);
		}
	EndElementL(); // SyncBody
	EndElementL(); // SyncML
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendSyncHrdL( SmlSyncHdrPtr_t aContent )
	{
	BeginElementL(ESyncHdr, ETrue);
	AppendPCDataL(EVerDTD, aContent->version);
	AppendPCDataL(EVerProto, aContent->proto);
	AppendPCDataL(ESessionID, aContent->sessionID);
	AppendPCDataL(EMsgID, aContent->msgID);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
	AppendPCDataL(ERespURI, aContent->respURI);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendCredL(aContent->cred);
	AppendMetaL(aContent->meta);
	EndElementL(); // ESyncHdr
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendTargetL( SmlTargetPtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	AppendPCDataL(ELocName, aContent->locName);
	AppendFilterL(aContent->filter); //1.2 CHANGES: Filter support
	}

//1.2 CHANGES: source & Target separated
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendSourceL( SmlSourcePtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	AppendPCDataL(ELocName, aContent->locName);
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendTargetL( SmlTargetPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ETarget, ETrue);
	DoAppendTargetL(aContent);
	EndElementL(); // ETarget
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendSourceL( SmlSourcePtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ESource, ETrue);
	DoAppendSourceL(aContent);	//1.2 CHANGES: Source & Target separated
	EndElementL(); // ETarget
	}

//1.2 CHANGES: Source & Target parent
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendSourceOrTargetParentL( SmlSourceOrTargetParentPtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendSourceParentL( SmlSourceParentPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ESourceParent, ETrue);
	DoAppendSourceOrTargetParentL(aContent);
	EndElementL(); // ESourceParent
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendTargetParentL( SmlTargetParentPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ETargetParent, ETrue);
	DoAppendSourceOrTargetParentL(aContent);
	EndElementL(); // ETargetParent
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendCredL( SmlCredPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ECred, ETrue);
	AppendMetaL(aContent->meta);
	AppendPCDataL(EData, aContent->data);
	EndElementL(); // ECred
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendSyncL( SmlSyncPtr_t aContent )
	{
	BeginElementL(ESync, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendCredL(aContent->cred);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
	AppendMetaL(aContent->meta);
	AppendPCDataL(ENumberOfChanges, aContent->noc);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendGenericCmdL( SmlGenericCmdPtr_t aContent, Flag_t aMask )
	{
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( (aContent->flags & SmlNoResp_f) & aMask )
		{
		BeginElementL(ENoResp);
		}
	if( (aContent->flags & SmlArchive_f) & aMask )
		{
		BeginElementL(EArchive);
		}
	if( (aContent->flags & SmlSftDel_f) & aMask )
		{
		BeginElementL(ESftDel);
		}
	AppendCredL(aContent->cred);
	AppendMetaL(aContent->meta);
	AppendItemListL(aContent->itemList);
	}
// FOTA
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendExecCmdL( SmlExecPtr_t aContent, Flag_t aMask )
	{
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( (aContent->flags & SmlNoResp_f) & aMask )
		{
		BeginElementL(ENoResp);
		}
	if( (aContent->flags & SmlArchive_f) & aMask )
		{
		BeginElementL(EArchive);
		}
	if( (aContent->flags & SmlSftDel_f) & aMask )
		{
		BeginElementL(ESftDel);
		}
	AppendCredL(aContent->cred);
	AppendMetaL(aContent->meta);
	AppendPCDataL(ECorrelator, aContent->correlator);
	AppendItemL(aContent->item);
	}
// FOTA end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendAddCmdL( SmlAddPtr_t aContent )
	{
	BeginElementL(EAdd, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EAdd
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendReplaceCmdL( SmlReplacePtr_t aContent )
	{
	BeginElementL(EReplace, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EReplace
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendCopyCmdL( SmlCopyPtr_t aContent )
	{
	BeginElementL(ECopy, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // ECopy
	}
// FOTA
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendExecCmdL( SmlExecPtr_t aContent )
	{
	BeginElementL(EExec, ETrue);
	DoAppendExecCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EExec
	}
// FOTA end

//1.2 CHANGES: Move command
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMoveCmdL( SmlMovePtr_t aContent )
	{
	BeginElementL(EMove, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EMove
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendDeleteCmdL( SmlDeletePtr_t aContent )
	{
	BeginElementL(EDelete, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f|SmlArchive_f|SmlSftDel_f);
	EndElementL(); // EDelete
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendAlertCmdL( SmlAlertPtr_t aContent )	
	{
	BeginElementL(EAlert, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendCredL(aContent->cred);
	AppendPCDataL(EData, aContent->data);
// FOTA	
	AppendPCDataL(ECorrelator, aContent->correlator);
// FOTA end
	AppendItemListL(aContent->itemList);
	EndElementL(); // EAlert
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendGetCmdL( SmlGetPtr_t aContent )
	{
	BeginElementL(EGet, ETrue);
	DoAppendPutGetL(aContent);
	EndElementL(); // EPut
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendPutCmdL( SmlPutPtr_t aContent )
	{
	BeginElementL(EPut, ETrue);
	DoAppendPutGetL(aContent);
	EndElementL(); // EPut
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendPutGetL( SmlPutPtr_t aContent )
	{
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendPCDataL(ELang, aContent->lang);
	AppendCredL(aContent->cred);
	AppendMetaL(aContent->meta);
	AppendItemListL(aContent->itemList);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CXMLSyncMLGenerator::smlDeviceInfoL( SmlDevInfDevInfPtr_t aContent )
	{
	BeginElementL(EData, ETrue);
	CXMLDevInfGenerator* devgen = new (ELeave) CXMLDevInfGenerator();
	CleanupStack::PushL(devgen);
	devgen->InitializeL();
	devgen->CreateWorkspaceL();
	devgen->SetIndentLevel(IndentLevel());
	devgen->smlDeviceInfo(aContent);
	Workspace()->WriteL(devgen->Document());
	CleanupStack::PopAndDestroy(); // devgen
	EndElementL(); // EPut
	return KErrNone;
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMapCmdL( SmlMapPtr_t aContent )
	{
	BeginElementL(EMap, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
	AppendCredL(aContent->cred);
	AppendMetaL(aContent->meta);
	AppendMapItemListL(aContent->mapItemList);
	EndElementL(); // EMap
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendResultsCmdL( SmlResultsPtr_t aContent )
	{
	BeginElementL(EResults, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	AppendPCDataL(EMsgRef, aContent->msgRef);
	AppendPCDataL(ECmdRef, aContent->cmdRef);
	AppendMetaL(aContent->meta);
	AppendPCDataL(ETargetRef, aContent->targetRef);
	AppendPCDataL(ETargetRef, aContent->sourceRef);
	AppendItemListL(aContent->itemList);
	EndElementL(); // EResults
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendStatusCmdL( SmlStatusPtr_t aContent )
	{
	BeginElementL(EStatus, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	AppendPCDataL(EMsgRef, aContent->msgRef);
	AppendPCDataL(ECmdRef, aContent->cmdRef);
	AppendPCDataL(ECmd, aContent->cmd);
	if( aContent->targetRefList )
		{
		AppendTargetRefListL(aContent->targetRefList);
		}
	if( aContent->sourceRefList )
		{
		AppendSourceRefListL(aContent->sourceRefList);
		}
	AppendCredL(aContent->cred);
	AppendChalL(aContent->chal);
	AppendPCDataL(EData, aContent->data);
	if( aContent->itemList )
		{
		AppendItemListL(aContent->itemList);
		}
	EndElementL(); // EStatus
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendStartAtomicOrSequenceL( SmlAtomicPtr_t aContent )
	{
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendMetaL(aContent->meta);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendStartAtomicL( SmlAtomicPtr_t aContent )
	{
	BeginElementL(EAtomic, ETrue);
	AppendStartAtomicOrSequenceL(aContent);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendEndAtomicL()
	{
	EndElementL(); // EAtomic
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendStartSequenceL( SmlSequencePtr_t aContent )
	{
	BeginElementL(ESequence, ETrue);
	AppendStartAtomicOrSequenceL(aContent);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendEndSequenceL()
	{
	EndElementL(); // ESequence
	}

//1.2 CHANGES: Filter support
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendFilterL( SmlFilterPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(EFilter, ETrue);
	AppendMetaL( aContent->meta );
	if( aContent->field )
		{
		BeginElementL(EField, ETrue);
		AppendItemL( aContent->field->item );
		EndElementL();
		}
	if( aContent->record )
		{
		BeginElementL(ERecord, ETrue);
		AppendItemL( aContent->record->item );
		EndElementL();
		}
	AppendPCDataL( EFilterType, aContent->filterType );
	EndElementL();
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent, TInt aAmount )
	{
	if( !aContent )
		{
		return;
		}

	TInt contentLength(aAmount == -1 ? aContent->length : aAmount);

	if( aContent->extension == SML_EXT_METINF )
		{
		BeginElementL(aElement, ETrue);
		DoAppendMetaL(SmlMetInfMetInfPtr_t(aContent->content));
		EndElementL();
		}
//1.2 CHANGES: Properties
	else if( aContent->extension == SML_EXT_DEVINFPROP )
		{
		BeginElementL(aElement, ETrue);
        SmlDevInfCtCapPtr_t ct = (SmlDevInfCtCapPtr_t) aContent->content;
		AppendPropertyListL( ct->property );
		EndElementL();
		}
// Changes end
	else if( aContent->contentType == SML_PCDATA_OPAQUE )
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, contentLength));
		}
	else
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, contentLength), EXMLContentFormatInlineString);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList )
	{
	for( SmlPcdataListPtr_t p = aList; p && p->data; p = p->next )
		{
		AppendPCDataL(aElement, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::FreeItemSizeAndMetaIfNeeded( SmlItemPtr_t aItem )
	{
	if( aItem->meta )
		{
		// if size element was created by generator -> delete it
		if( aItem->generatorFlags & SML_PE_SIZE_CREATED_BY_GENERATOR )
			{
			aItem->generatorFlags &= ~SML_PE_SIZE_CREATED_BY_GENERATOR;
			delete SmlMetInfMetInfPtr_t(aItem->meta->content)->size;
			SmlMetInfMetInfPtr_t(aItem->meta->content)->size = 0;
			}

		// if meta element was created by generator -> delete it
		if( aItem->generatorFlags & SML_PE_META_CREATED_BY_GENERATOR )
			{
			aItem->generatorFlags &= ~SML_PE_META_CREATED_BY_GENERATOR;
			delete aItem->meta;
			aItem->meta = 0;
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemListL( SmlItemListPtr_t aList )
	{
	iItemWasTruncated = EFalse;

	for( SmlItemListPtr_t p = aList, iItemIndex = 0; p && p->item; p = p->next, iItemIndex++ )
		{
		Workspace()->BeginTransaction();

		TRAPD(err, AppendItemL(p->item));

		if( ((Workspace()->FreeSize() < KNSmlXmlBufferSafeTreshold) || (err == KErrTooBig)) && iUseTruncation )
			{
			// Rollback everything that was written into buffer
			Workspace()->Rollback();

			SmlItemPtr_t item(p->item);
			
			// if this item does not have MoreData flag on --> put it on and add size to meta
			if( !(item->generatorFlags & SML_PE_SIZE_SENT) )
				{	
				if( !item->meta )
					{
					item->generatorFlags |= SML_PE_META_CREATED_BY_GENERATOR;
					item->meta = new (ELeave) SmlPcdata_t(/*SML_PCDATA_EXTENSION, SML_EXT_METINF*/);
					item->meta->contentType = SML_PCDATA_EXTENSION;
					item->meta->extension = SML_EXT_METINF;
					item->meta->content = new (ELeave) SmlMetInfMetInf_t();
					}
				
				if( !SmlMetInfMetInfPtr_t(item->meta->content)->size )
					{
					item->generatorFlags |= SML_PE_SIZE_CREATED_BY_GENERATOR;
					SmlMetInfMetInfPtr_t(item->meta->content)->size = new (ELeave) SmlPcdata_t();
					iBuffer.Num(TInt(item->data->length));
					SmlMetInfMetInfPtr_t(item->meta->content)->size->SetDataL(iBuffer);
					}
				}

			Workspace()->BeginTransaction();
			TRAPD(err, AppendItemBeginL(item));
			FreeItemSizeAndMetaIfNeeded(item);
			if( (err != KErrNone) || (Workspace()->FreeSize() < 3*KNSmlXmlBufferSafeTreshold) )
				{
				Workspace()->Rollback();
				User::Leave(KErrTooBig);
				}
			Workspace()->Commit();

			item->generatorFlags |= SML_PE_SIZE_SENT;

			// Set dataconsume length
			iDataConsumed = Workspace()->FreeSize() - 2*KNSmlXmlBufferSafeTreshold;
			
			// Now we most of item data in buffer
			AppendItemDataL(item, iDataConsumed);

			// Now we most of item data in buffer
			AppendItemEndL(ETrue);

			iItemWasTruncated = ETrue;
			break;
			}
		else if( err != KErrNone )
			{
			// Other error --> remove changes
			Workspace()->Rollback();
			User::Leave(err);
			}
	
		// Item added ok -> commit this item
		Workspace()->Commit();
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemBeginL( SmlItemPtr_t aContent )
	{
	BeginElementL(EItem, ETrue);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
//1.2 Changes: SourceTarget & sourceParent 
	AppendSourceParentL(aContent->sourceParent);
	AppendTargetParentL(aContent->targetParent);
//Changes end
	AppendMetaL(aContent->meta);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemEndL( TBool aMoreData )
	{
	//if( aContent->flags & SmlMoreData_f )
	if( aMoreData )
		{
		BeginElementL(EMoreData);
		}
	EndElementL(); // EItem
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemDataL( SmlItemPtr_t aContent, TInt aAmount )
	{
	if( aContent->data && aContent->source && aContent->source->locURI )
		{
		TPtrC8 locURI((TUint8*)aContent->source->locURI->content, aContent->source->locURI->length);
		// HYBRID 1.2 CHANGE
		if( locURI.Find(KDevInfGen()) != KErrNotFound )
			{
			CWBXMLParser* parser = CWBXMLParser::NewL();
			CleanupStack::PushL(parser);
			CWBXMLDevInfDocHandler* dh = CWBXMLDevInfDocHandler::NewL(this);
			CleanupStack::PushL(dh);
			parser->SetDocumentHandler(dh);
			parser->SetExtensionHandler(dh);
			RMemReadStream mrs(aContent->data->content, aContent->data->length);
			parser->SetDocumentL(mrs);

			while( parser->ParseL() == KWBXMLParserErrorOk )
                ;
			CleanupStack::PopAndDestroy(2); 
			return;
			}
		}
    AppendPCDataL(EData, aContent->data, aAmount);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemL( SmlItemPtr_t aContent )
	{
	AppendItemBeginL(aContent);
	AppendItemDataL(aContent);
	AppendItemEndL((aContent->flags & SmlMoreData_f) == SmlMoreData_f);
	}

//1.2 CHANGES: Filter support
// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendItemL( SmlPcdataPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	SmlItemPtr_t item = (SmlItemPtr_t)aContent->content;
	AppendItemL(item);
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendTargetRefListL( SmlTargetRefListPtr_t aList )
	{
	AppendPCDataListL(ETargetRef, (SmlPcdataListPtr_t)aList);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendSourceRefListL( SmlSourceRefListPtr_t aList )
	{
	AppendPCDataListL(ESourceRef, (SmlPcdataListPtr_t)aList);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendChalL( SmlChalPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(EChal, ETrue);
	AppendMetaL(aContent->meta);
	EndElementL(); // EChal
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMapItemListL( SmlMapItemListPtr_t aList )
	{
	for( SmlMapItemListPtr_t p = aList; p && p->mapItem; p = p->next )
		{
		AppendMapItemL(p->mapItem);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMapItemL( SmlMapItemPtr_t aContent )
	{
	BeginElementL(EMapItem, ETrue);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
	EndElementL(); // EMapItem
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMetaL( SmlPcdataPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	SmlMetInfMetInfPtr_t meta = (SmlMetInfMetInfPtr_t)aContent->content;
	AppendMetaL(meta);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMetaL( SmlMetInfMetInfPtr_t aMeta )
	{
	BeginElementL(EMeta, ETrue);
	DoAppendMetaL(aMeta);
	EndElementL(); // EMeta
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::DoAppendMetaL( SmlMetInfMetInfPtr_t aMeta )
	{
	SwitchCodePageL(ECodePageMetInf);
	CXMLGenerator::SetNamespaceNameL(KMetInfNamespace(), CXMLGenerator::EAppendToEveryElement);
	CXMLGenerator::SetTranslateTableL(KMetInfElements());

	// FOTA
	if ( aMeta )
	// FOTA end
		{
		AppendPCDataL(EFieldLevel, aMeta->fieldLevel); // 1.2 CHANGES: FieldLevel
		AppendPCDataL(EMetFormat, aMeta->format);
		AppendPCDataL(EMetType, aMeta->type);
		AppendPCDataL(EMetMark, aMeta->mark);
		AppendPCDataL(EMetSize, aMeta->size);
	
		if( aMeta->anchor )
			{
			AppendMetAnchorL(aMeta->anchor);
			}

		AppendPCDataL(EMetVersion, aMeta->version);
		AppendPCDataL(EMetNextNonce, aMeta->nextnonce);
		AppendPCDataL(EMetMaxMsgSize, aMeta->maxmsgsize);
		AppendPCDataL(EMetMaxObjSize, aMeta->maxobjsize);

		if( aMeta->emi )
			{
			AppendPCDataListL(EMetEMI, aMeta->emi);
			}
		if( aMeta->mem )
			{
			AppendMetMemL(aMeta->mem);
			}
		}

	SwitchCodePageL(ECodePageSyncML);
	CXMLGenerator::SetNamespaceNameL(KSyncMLNamespace());
	CXMLGenerator::SetTranslateTableL(KSyncMLElements());
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMetMemL( SmlMetInfMemPtr_t aContent )
	{
	BeginElementL(EMetMem, ETrue);
	AppendPCDataL(EMetSharedMem, aContent->shared);
	AppendPCDataL(EMetFreeMem, aContent->free);
	AppendPCDataL(EMetFreeID, aContent->freeid);
	EndElementL(); // EMetMem
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLGenerator::AppendMetAnchorL( SmlMetInfAnchorPtr_t aContent )
	{
	BeginElementL(EMetAnchor, ETrue);
	AppendPCDataL(EMetLast, aContent->last);
	AppendPCDataL(EMetNext, aContent->next);
	EndElementL(); // EMetAnchor
	}

// 1.2 CHANGES: Properties and params for filter.
void CXMLSyncMLGenerator::AppendPropParamL( SmlDevInfPropParamPtr_t aContent)
    {
	BeginElementL(EDevPropParam, ETrue);
	AppendPCDataL(EDevParamName, aContent->paramname);
	AppendPCDataL(EDevDataType, aContent->datatype);
	AppendPCDataListL(EDevValEnum, aContent->valenum);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	EndElementL(); // EDevPropParam
    }
void CXMLSyncMLGenerator::AppendPropParamListL( SmlDevInfPropParamListPtr_t aContent)
    {
	if (aContent && aContent->data)
		{
		for( SmlDevInfPropParamListPtr_t p = aContent; p && p->data; p = p->next )
			{
			AppendPropParamL(p->data);
			}
		}
    }
void CXMLSyncMLGenerator::AppendPropertyL( SmlDevInfPropertyPtr_t aContent )
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
void CXMLSyncMLGenerator::AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent )
    {
    SwitchCodePageL(ECodePageDevInf);
    CXMLGenerator::SetTranslateTableL(KDevInfElements());
	for( SmlDevInfPropertyListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendPropertyL(p->data);
		}
    SwitchCodePageL(ECodePageSyncML);
    CXMLGenerator::SetNamespaceNameL(KSyncMLNamespace());
    CXMLGenerator::SetTranslateTableL(KSyncMLElements());
    }
// Changes end

//End of File
