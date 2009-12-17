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
* Description:  SyncML document generator class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "WBXMLSyncMLGenerator.h"
#include "WBXMLGeneratorError.h"
#include "XMLWorkspace.h"

#include "smlsyncmltags.h"
#include "smlmetinftags.h"
#include "smldevinftags.h" //1.2 CHANGES: Property

// ------------------------------------------------------------------------------------------------
// CWBXMLSyncMLGenerator
// ------------------------------------------------------------------------------------------------
//1.2 CHANGES: publicId, Streaming
EXPORT_C CWBXMLSyncMLGenerator::CWBXMLSyncMLGenerator( TInt32 aPublicId ) :
	iPublicId( aPublicId ), iContentLength(-1)
	{
	}

EXPORT_C void CWBXMLSyncMLGenerator::CreateWorkspaceL( TInt aBufferSize )
	{
	CWBXMLGenerator::CreateStaticWorkspaceL(aBufferSize);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlStartMessage( SmlSyncHdrPtr_t aContent )
	{
	Reset();
	Workspace()->BeginTransaction();
	TRAPD(result, DoSmlStartMessageL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlEndMessage( Boolean_t aFinal )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, DoSmlEndMessageL(aFinal));
	return HandleResult(result); // No safe treshold as this is end of packet
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlStartSync( SmlSyncPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendSyncL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlEndSync()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, EndElementL());
	return HandleResult(result);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlAddCmd( SmlAddPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendAddCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlReplaceCmd( SmlReplacePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendReplaceCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlCopyCmd( SmlCopyPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendCopyCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlDeleteCmd( SmlDeletePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendDeleteCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

//1.2 CHANGES: Move command
// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlMoveCmd( SmlMovePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendMoveCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}
// Changes end

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlAlertCmd( SmlAlertPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendAlertCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlGetCmd( SmlPutPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendGetCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlPutCmd( SmlPutPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendPutCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlMapCmd( SmlMapPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendMapCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlResultsCmd( SmlResultsPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendResultsCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlStatusCmd( SmlStatusPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStatusCmdL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlStartAtomic( SmlAtomicPtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStartAtomicL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlEndAtomic()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendEndAtomicL());
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlStartSequence( SmlSequencePtr_t aContent )
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendStartSequenceL(aContent));
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C Ret_t CWBXMLSyncMLGenerator::smlEndSequence()
	{
	Workspace()->BeginTransaction();
	TRAPD(result, AppendEndSequenceL());
	return HandleResult(result, KNSmlBufferSafeTreshold);
	}

// Data chunk functionality

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLSyncMLGenerator::SetTruncate( TBool aEnabled )
	{
	iUseTruncation = aEnabled;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CWBXMLSyncMLGenerator::ItemIndex() const
	{
	return iItemIndex;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CWBXMLSyncMLGenerator::DataConsumed() const
	{
	return iDataConsumed;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CWBXMLSyncMLGenerator::WasTruncated()
	{
	TBool was(iItemWasTruncated);
	iItemWasTruncated = EFalse;
	return was;
	}

//1.2 CHANGES: Streaming
EXPORT_C void CWBXMLSyncMLGenerator::SetContentLength(TInt aContentLength)
	{
	iContentLength = aContentLength;
	}
//end changes
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoSmlStartMessageL( SmlSyncHdrPtr_t aContent )
	{
	//1.2 CHANGES: Publicid
	BeginDocumentL(KNSmlSyncMLVersion, iPublicId, KNSmlSyncMLUTF8);
	//Changes end
	BeginElementL(ESyncML, ETrue);
	AppendSyncHrdL(aContent);
	BeginElementL(ESyncBody, ETrue);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoSmlEndMessageL( Boolean_t aFinal )
	{
	if( aFinal )
		{
		BeginElementL(EFinal);
		}
	EndElementL(); // SyncBody
	EndElementL(); // SyncML
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendSyncHrdL( SmlSyncHdrPtr_t aContent )
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

// 1.2 CHANGES: Source and Target separated
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoAppendTargetL( SmlTargetPtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	AppendPCDataL(ELocName, aContent->locName);
	AppendFilterL(aContent->filter); //1.2 CHANGES: Filter
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoAppendSourceL( SmlSourcePtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	AppendPCDataL(ELocName, aContent->locName);
	}
// Changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendTargetL( SmlTargetPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendSourceL( SmlSourcePtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ESource, ETrue);
// 1.2 CHANGES: Source and Target separated
	DoAppendSourceL(aContent);
// Changes end
	EndElementL(); // ETarget
	}

//1.2 CHANGES: Source & Target parent
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoAppendSourceOrTargetParentL( SmlSourceOrTargetParentPtr_t aContent )
	{
	AppendPCDataL(ELocURI, aContent->locURI);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendSourceParentL( SmlSourceParentPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendTargetParentL( SmlTargetParentPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendCredL( SmlCredPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendSyncL( SmlSyncPtr_t aContent )
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
void CWBXMLSyncMLGenerator::DoAppendGenericCmdL( SmlGenericCmdPtr_t aContent, Flag_t aMask )
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

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendAddCmdL( SmlAddPtr_t aContent )
	{
	BeginElementL(EAdd, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EAdd
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendReplaceCmdL( SmlReplacePtr_t aContent )
	{
	BeginElementL(EReplace, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EReplace
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendCopyCmdL( SmlCopyPtr_t aContent )
	{
	BeginElementL(ECopy, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // ECopy
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendDeleteCmdL( SmlDeletePtr_t aContent )
	{
	BeginElementL(EDelete, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f|SmlArchive_f|SmlSftDel_f);
	EndElementL(); // EDelete
	}

//1.2 CHANGES: Move command
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMoveCmdL( SmlMovePtr_t aContent )
	{
	BeginElementL(EMove, ETrue);
	DoAppendGenericCmdL(aContent, SmlNoResp_f);
	EndElementL(); // EMove
	}
// Changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendAlertCmdL( SmlAlertPtr_t aContent )	
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
void CWBXMLSyncMLGenerator::AppendGetCmdL( SmlGetPtr_t aContent )
	{
	BeginElementL(EGet, ETrue);
	DoAppendPutGetL(aContent);
	EndElementL(); // EPut
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendPutCmdL( SmlPutPtr_t aContent )
	{
	BeginElementL(EPut, ETrue);
	DoAppendPutGetL(aContent);
	EndElementL(); // EPut
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoAppendPutGetL( SmlPutPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendMapCmdL( SmlMapPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendResultsCmdL( SmlResultsPtr_t aContent )
	{
	BeginElementL(EResults, ETrue);
	AppendPCDataL(ECmdID, aContent->cmdID);
	AppendPCDataL(EMsgRef, aContent->msgRef);
	AppendPCDataL(ECmdRef, aContent->cmdRef);
	AppendMetaL(aContent->meta);
	AppendPCDataL(ETargetRef, aContent->targetRef);
	AppendPCDataL(ESourceRef, aContent->sourceRef);
	AppendItemListL(aContent->itemList);
	EndElementL(); // EResults
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendStatusCmdL( SmlStatusPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendStartAtomicOrSequenceL( SmlAtomicPtr_t aContent )
	{
	AppendPCDataL(ECmdID, aContent->cmdID);
	if( aContent->flags & SmlNoResp_f )
		{
		BeginElementL(ENoResp);
		}
	AppendMetaL(aContent->meta);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendStartAtomicL( SmlAtomicPtr_t aContent )
	{
	BeginElementL(EAtomic, ETrue);
	AppendStartAtomicOrSequenceL(aContent);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendEndAtomicL()
	{
	EndElementL(); // EAtomic
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendStartSequenceL( SmlSequencePtr_t aContent )
	{
	BeginElementL(ESequence, ETrue);
	AppendStartAtomicOrSequenceL(aContent);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendEndSequenceL()
	{
	EndElementL(); // ESequence
	}

//1.2 CHANGES: Field
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendFieldL( SmlFieldPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(EField, ETrue);
	AppendItemL( aContent->item );
	EndElementL();
	}

//1.2 CHANGES: Record
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendRecordL( SmlRecordPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(ERecord, ETrue);
	AppendItemL( aContent->item );
	EndElementL();
	}

//1.2 CHANGES: Filter support
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendFilterL( SmlFilterPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	BeginElementL(EFilter, ETrue);
	AppendMetaL( aContent->meta );
	AppendFieldL( aContent->field );
	AppendRecordL( aContent->record );

	AppendPCDataL( EFilterType, aContent->filterType );
	EndElementL();
	}
//Changes end

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent, TInt aAmount )
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
//1.2 CHANGES: Property
	else if( aContent->extension == SML_EXT_DEVINFPROP )
		{
		BeginElementL(aElement, ETrue);
    	AppendPropertyListL(SmlDevInfPropertyListPtr_t(aContent->content));        
		EndElementL();
  		}
// Changes end
	else if( aContent->contentType == SML_PCDATA_OPAQUE )
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, contentLength));
		}
	else
		{
		AddElementL(aElement, TPtrC8((TUint8*)aContent->content, contentLength), EWBXMLContentFormatInlineString);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList )
	{
	for( SmlPcdataListPtr_t p = aList; p && p->data; p = p->next )
		{
		AppendPCDataL(aElement, p->data);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::FreeItemSizeAndMetaIfNeeded( SmlItemPtr_t aItem )
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
void CWBXMLSyncMLGenerator::AppendItemListL( SmlItemListPtr_t aList )
	{
	iItemWasTruncated = EFalse;

	for( SmlItemListPtr_t p = aList, iItemIndex = 0; p && p->item; p = p->next, iItemIndex++ )
		{
		Workspace()->BeginTransaction();

		TRAPD(err, AppendItemL(p->item));

		if( ((Workspace()->FreeSize() < KNSmlBufferSafeTreshold) || (err == KErrTooBig)) && iUseTruncation )
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
//1.2 CHANGES: Streaming
					if (iContentLength  == -1)
						{
						iBuffer.Num(TInt(item->data->length));
						}
					else
						{
						iBuffer.Num(iContentLength);
						iContentLength = -1;
						}
//end changes					
					SmlMetInfMetInfPtr_t(item->meta->content)->size->SetDataL(iBuffer);
					
					}
				}

			Workspace()->BeginTransaction();
			TRAPD(err, AppendItemBeginL(item));
			FreeItemSizeAndMetaIfNeeded(item);

			if( (err != KErrNone) || (Workspace()->FreeSize() < 3*KNSmlBufferSafeTreshold) )
				{
				// Not enough space to add rest of item data --> Rollback changes
				Workspace()->Rollback();
				User::Leave(KErrTooBig);
				}
			Workspace()->Commit();
		
			// At this point size was really sent --> mark it down
			item->generatorFlags |= SML_PE_SIZE_SENT;

			// Set dataconsume length
			iDataConsumed = Workspace()->FreeSize() - 2*KNSmlBufferSafeTreshold - 1;
			
			// Now we write data in buffer
			AppendItemDataL(item, iDataConsumed);

			// End item nicely
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
void CWBXMLSyncMLGenerator::AppendItemBeginL( SmlItemPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendItemEndL( TBool aMoreData )
	{
	//if( aContent->flags & SmlMoreData_f )
	if( aMoreData )
		{
		BeginElementL(EMoreData);
		}
	EndElementL(); // EItem
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendItemDataL( SmlItemPtr_t aContent, TInt aAmount )
	{
    AppendPCDataL(EData, aContent->data, aAmount);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendItemL( SmlItemPtr_t aContent )
	{
	AppendItemBeginL(aContent);
	AppendItemDataL(aContent);
	AppendItemEndL();
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendTargetRefListL( SmlTargetRefListPtr_t aList )
	{
	AppendPCDataListL(ETargetRef, (SmlPcdataListPtr_t)aList);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendSourceRefListL( SmlSourceRefListPtr_t aList )
	{
	AppendPCDataListL(ESourceRef, (SmlPcdataListPtr_t)aList);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendChalL( SmlChalPtr_t aContent )
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
void CWBXMLSyncMLGenerator::AppendMapItemListL( SmlMapItemListPtr_t aList )
	{
	for( SmlMapItemListPtr_t p = aList; p && p->mapItem; p = p->next )
		{
		AppendMapItemL(p->mapItem);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMapItemL( SmlMapItemPtr_t aContent )
	{
	BeginElementL(EMapItem, ETrue);
	AppendTargetL(aContent->target);
	AppendSourceL(aContent->source);
	EndElementL(); // EMapItem
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMetaL( SmlPcdataPtr_t aContent )
	{
	if( !aContent )
		{
		return;
		}
	SmlMetInfMetInfPtr_t meta = (SmlMetInfMetInfPtr_t)aContent->content;
	AppendMetaL(meta);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMetaL( SmlMetInfMetInfPtr_t aMeta )
	{
	BeginElementL(EMeta, ETrue);
	DoAppendMetaL(aMeta);
	EndElementL(); // EMeta
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::DoAppendMetaL( SmlMetInfMetInfPtr_t aMeta )
	{
	SwitchCodePageL(ECodePageMetInf);
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

	SwitchCodePageL(ECodePageSyncML);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMetMemL( SmlMetInfMemPtr_t aContent )
	{
	BeginElementL(EMetMem, ETrue);
	AppendPCDataL(EMetSharedMem, aContent->shared);
	AppendPCDataL(EMetFreeMem, aContent->free);
	AppendPCDataL(EMetFreeID, aContent->freeid);
	EndElementL(); // EMetMem
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendMetAnchorL( SmlMetInfAnchorPtr_t aContent )
	{
	BeginElementL(EMetAnchor, ETrue);
	AppendPCDataL(EMetLast, aContent->last);
	AppendPCDataL(EMetNext, aContent->next);
	EndElementL(); // EMetAnchor
	}

// 1.2 CHANGES: new elements propparam and property
// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendPropParamL(SmlDevInfPropParamPtr_t aContent)
	{
	BeginElementL(EDevPropParam, ETrue);
	AppendPCDataL(EDevParamName, aContent->paramname);
	AppendPCDataL(EDevDataType, aContent->datatype);
	AppendPCDataListL(EDevValEnum, aContent->valenum);
	AppendPCDataL(EDevDisplayName, aContent->displayname);
	EndElementL(); // EDevPropParam
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLGenerator::AppendPropParamListL(SmlDevInfPropParamListPtr_t aContent)
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
void CWBXMLSyncMLGenerator::AppendPropertyL( SmlDevInfPropertyPtr_t aContent)
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
void CWBXMLSyncMLGenerator::AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent )
	{
    SwitchCodePageL(ECodePageDevInf);
	for( SmlDevInfPropertyListPtr_t p = aContent; p && p->data; p = p->next )
		{
		AppendPropertyL(p->data);
		}
    SwitchCodePageL(ECodePageSyncML);
	}

// Changes end

// End of File
