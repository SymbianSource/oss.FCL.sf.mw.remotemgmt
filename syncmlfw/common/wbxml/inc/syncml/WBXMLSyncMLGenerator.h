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
* Description:  SyncML document generator class.
*
*/


#ifndef __WBXMLSYNCMLGENERATOR_H__
#define __WBXMLSYNCMLGENERATOR_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "WBXMLGenerator.h"

// ------------------------------------------------------------------------------------------------
// Toolkit includes
// ------------------------------------------------------------------------------------------------
#include "smldef.h"
#include "smldtd.h"
#include "smlmetinfdtd.h"
#include "smldevinfdtd.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
const TInt KNSmlBufferSafeTreshold = 8;

// ------------------------------------------------------------------------------------------------
// CNSmlSyncMLGenerator
// ------------------------------------------------------------------------------------------------
class CWBXMLSyncMLGenerator : public CWBXMLGenerator
	{
public:
	IMPORT_C CWBXMLSyncMLGenerator(  TInt32 aPublicId );	//1.2 CHANGES: publicId

	IMPORT_C void CreateWorkspaceL( TInt aBufferSize );

	IMPORT_C Ret_t smlStartMessage( SmlSyncHdrPtr_t aContent );	
	IMPORT_C Ret_t smlEndMessage( Boolean_t aFinal );	

	IMPORT_C Ret_t smlStartSync( SmlSyncPtr_t aContent );	
	IMPORT_C Ret_t smlEndSync();	

	IMPORT_C Ret_t smlAddCmd( SmlAddPtr_t aContent );	
	IMPORT_C Ret_t smlReplaceCmd( SmlReplacePtr_t aContent );	

	IMPORT_C Ret_t smlCopyCmd( SmlCopyPtr_t aContent );	
	IMPORT_C Ret_t smlDeleteCmd( SmlDeletePtr_t aContent );	
	IMPORT_C Ret_t smlMoveCmd( SmlMovePtr_t aContent );	// 1.2 CHANGES: Move command
	IMPORT_C Ret_t smlAlertCmd( SmlAlertPtr_t aContent );	
	IMPORT_C Ret_t smlGetCmd( SmlGetPtr_t aContent );	
	IMPORT_C Ret_t smlPutCmd( SmlPutPtr_t aContent );	
	IMPORT_C Ret_t smlMapCmd( SmlMapPtr_t aContent );	
	IMPORT_C Ret_t smlResultsCmd( SmlResultsPtr_t aContent );	
	IMPORT_C Ret_t smlStatusCmd( SmlStatusPtr_t aContent );	

	IMPORT_C Ret_t smlStartAtomic( SmlAtomicPtr_t aContent );
	IMPORT_C Ret_t smlEndAtomic();
	IMPORT_C Ret_t smlStartSequence( SmlSequencePtr_t aContent );
	IMPORT_C Ret_t smlEndSequence();

	// Data chunk functionality
	IMPORT_C void SetTruncate( TBool aEnabled );
	IMPORT_C TBool WasTruncated();
	IMPORT_C TInt ItemIndex() const;
	IMPORT_C TInt DataConsumed() const;
	//1.2 Streaming: large item size
	IMPORT_C void SetContentLength(TInt aContentLength);
	//end changes
private:

	// SyncML
	void AppendSyncHrdL( SmlSyncHdrPtr_t aContent );
	void AppendSyncL( SmlSyncPtr_t aContent );
	void AppendAddCmdL( SmlAddPtr_t aContent );	
	void AppendReplaceCmdL( SmlReplacePtr_t aContent );	
	void AppendCopyCmdL( SmlCopyPtr_t aContent );	
	void AppendDeleteCmdL( SmlDeletePtr_t aContent );
	void AppendMoveCmdL( SmlMovePtr_t aContent ); //1.2 CHANGES: Move command
	void AppendAlertCmdL( SmlAlertPtr_t aContent );	
	void AppendGetCmdL( SmlGetPtr_t aContent );	
	void AppendPutCmdL( SmlPutPtr_t aContent );	
	void AppendMapCmdL( SmlMapPtr_t aContent );	
	void AppendResultsCmdL( SmlResultsPtr_t aContent );	
	void AppendStatusCmdL( SmlStatusPtr_t aContent );	
	void AppendPCDataL( TUint8 aElement, SmlPcdataPtr_t aContent, TInt aAmount = -1 );
	void AppendPCDataListL( TUint8 aElement, SmlPcdataListPtr_t aList );
	void AppendItemListL( SmlItemListPtr_t aList );
	void AppendTargetL( SmlTargetPtr_t aContent );
	void AppendSourceL( SmlSourcePtr_t aContent );
	void AppendCredL( SmlCredPtr_t aContent );
	void AppendTargetRefListL( SmlTargetRefListPtr_t aList );
	void AppendSourceRefListL( SmlSourceRefListPtr_t aList );
	void AppendChalL( SmlChalPtr_t aContent );
	void AppendMapItemListL( SmlMapItemListPtr_t aList );
	void AppendMapItemL( SmlMapItemPtr_t aContent );
	void AppendItemL( SmlItemPtr_t aContent ); 
	void AppendStartAtomicL( SmlAtomicPtr_t aContent );	
	void AppendStartSequenceL( SmlSequencePtr_t aContent );	
	void AppendEndAtomicL();	
	void AppendEndSequenceL();	
	void AppendStartAtomicOrSequenceL( SmlAtomicPtr_t aContent );
	void AppendFilterL( SmlFilterPtr_t aContent ); //1.2 CHANGES: Filter support
	void AppendSourceParentL( SmlSourceParentPtr_t aContent ); //1.2 CHANGES: Source&Target parent
	void AppendTargetParentL( SmlTargetParentPtr_t aContent ); //1.2 CHANGES: Source&Target parent
	void AppendFieldL( SmlFieldPtr_t aContent ); //1.2 CHANGES: Field
	void AppendRecordL( SmlRecordPtr_t aContent ); //1.2 CHANGES: Record

	//1.2 CHANGES: Property
	void AppendPropParamL( SmlDevInfPropParamPtr_t aContent);
	void AppendPropParamListL( SmlDevInfPropParamListPtr_t aContent);
	void AppendPropertyL( SmlDevInfPropertyPtr_t aContent );
	void AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent );
	//Changes end

	// MetInf
	void AppendMetaL( SmlPcdataPtr_t aContent );
	void AppendMetaL( SmlMetInfMetInfPtr_t aMeta );
	void AppendMetMemL( SmlMetInfMemPtr_t aContent );
	void AppendMetAnchorL( SmlMetInfAnchorPtr_t aContent );

	// Misc
	void DoSmlStartMessageL( SmlSyncHdrPtr_t aContent );
	void DoSmlEndMessageL( Boolean_t aFinal );
	void DoAppendTargetL( SmlTargetPtr_t aContent );
	void DoAppendSourceL( SmlSourcePtr_t aContent );
	void DoAppendGenericCmdL( SmlGenericCmdPtr_t aContent, Flag_t aMask );
	void DoAppendMetaL( SmlMetInfMetInfPtr_t aMeta );
	void DoAppendPutGetL( SmlGetPtr_t aContent );

	void AppendItemBeginL( SmlItemPtr_t aContent );
	void AppendItemEndL( TBool aMoreData = EFalse );
	void AppendItemDataL( SmlItemPtr_t aContent, TInt aAmount = -1 );
	void FreeItemSizeAndMetaIfNeeded( SmlItemPtr_t aItem );
	void DoAppendSourceOrTargetParentL(SmlSourceOrTargetParentPtr_t aContent); //1.2 CHANGES: Source & Target parent

private:
	TInt32 iPublicId;	//1.2 CHANGES: publicId
	TInt iItemIndex;
	TInt iDataConsumed;
	TBool iUseTruncation;
	TBool iItemWasTruncated;
	TBuf8<64> iBuffer;
	//1.2 Streaming: large item size
	TInt iContentLength;
	//end changes
	};

#endif // __WBXMLSYNCMLGENERATOR_H__
