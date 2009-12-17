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


#ifndef __XMLSYNCMLGENERATOR_H__
#define __XMLSYNCMLGENERATOR_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "XMLGenerator.h"

// ------------------------------------------------------------------------------------------------
// Toolkit includes
// ------------------------------------------------------------------------------------------------
#include "smldef.h"
#include "smldtd.h"
#include "smlmetinfdtd.h"

#include "WBXMLDevInfParser.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
const TInt KNSmlXmlBufferSafeTreshold = 0;

// ------------------------------------------------------------------------------------------------
// CNSmlSyncMLGenerator
// ------------------------------------------------------------------------------------------------
class CXMLSyncMLGenerator : public CXMLGenerator, public MWBXMLDevInfCallbacks
	{
public:
	IMPORT_C void CreateWorkspaceL();
	IMPORT_C void InitializeL();

	IMPORT_C Ret_t smlStartMessage( SmlSyncHdrPtr_t aContent );	
	IMPORT_C Ret_t smlEndMessage( Boolean_t aFinal );	

	IMPORT_C Ret_t smlStartSync( SmlSyncPtr_t aContent );	
	IMPORT_C Ret_t smlEndSync();	

	IMPORT_C Ret_t smlAddCmd( SmlAddPtr_t aContent );	
	IMPORT_C Ret_t smlReplaceCmd( SmlReplacePtr_t aContent );	

	IMPORT_C Ret_t smlCopyCmd( SmlCopyPtr_t aContent );	
	// FOTA
	IMPORT_C Ret_t smlExecCmd( SmlExecPtr_t aContent );
	// FOTA end	
	IMPORT_C Ret_t smlMoveCmd( SmlMovePtr_t aContent );	//1.2 CHANGES: Move command
	IMPORT_C Ret_t smlDeleteCmd( SmlDeletePtr_t aContent );	
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
protected:

	// SyncML
	void AppendSyncHrdL( SmlSyncHdrPtr_t aContent );
	void AppendSyncL( SmlSyncPtr_t aContent );
	void AppendAddCmdL( SmlAddPtr_t aContent );	
	void AppendReplaceCmdL( SmlReplacePtr_t aContent );	
	void AppendCopyCmdL( SmlCopyPtr_t aContent );
	// FOTA
	void AppendExecCmdL( SmlExecPtr_t aContent );	
	// FOTA end
	void AppendMoveCmdL( SmlMovePtr_t aContent ); //1.2 CHANGES: Move command
	void AppendDeleteCmdL( SmlDeletePtr_t aContent );	
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
	void AppendItemL( SmlPcdataPtr_t aContent ); //1.2 CHANGES: Filter support
	void AppendStartAtomicL( SmlAtomicPtr_t aContent );	
	void AppendStartSequenceL( SmlSequencePtr_t aContent );	
	void AppendEndAtomicL();	
	void AppendEndSequenceL();	
	void AppendStartAtomicOrSequenceL( SmlAtomicPtr_t aContent );
	void AppendFilterL( SmlFilterPtr_t aContent ); //1.2 CHANGES: Filter support
	void AppendSourceParentL( SmlSourceParentPtr_t aContent ); //1.2 CHANGES: Source&Target parent
	void AppendTargetParentL( SmlTargetParentPtr_t aContent ); //1.2 CHANGES: Source&Target parent

//1.2 CHANGES: Property & Param.
    void AppendPropParamL( SmlDevInfPropParamPtr_t aContent);
	void AppendPropParamListL( SmlDevInfPropParamListPtr_t aContent);
	void AppendPropertyL( SmlDevInfPropertyPtr_t aContent );
	void AppendPropertyListL( SmlDevInfPropertyListPtr_t aContent );
// Changes end

	// MetInf
	void AppendMetaL( SmlPcdataPtr_t aContent );
	void AppendMetaL( SmlMetInfMetInfPtr_t aMeta );
	void AppendMetMemL( SmlMetInfMemPtr_t aContent );
	void AppendMetAnchorL( SmlMetInfAnchorPtr_t aContent );

	// Misc
	void doSmlStartMessageL( SmlSyncHdrPtr_t aContent );
	void doSmlEndMessageL( Boolean_t aFinal );
	void DoAppendTargetL( SmlTargetPtr_t aContent );
	void DoAppendSourceL( SmlSourcePtr_t aContent ); //1.2 CHANGES: Source & Target separated
	void DoAppendGenericCmdL( SmlGenericCmdPtr_t aContent, Flag_t aMask );
	// FOTA
	void DoAppendExecCmdL( SmlExecPtr_t aContent, Flag_t aMask );
	// FOTA
	void DoAppendMetaL( SmlMetInfMetInfPtr_t aMeta );
	void DoAppendPutGetL( SmlGetPtr_t aContent );

	void AppendItemBeginL( SmlItemPtr_t aContent );
	void AppendItemEndL( TBool aMoreData = EFalse );
	void AppendItemDataL( SmlItemPtr_t aContent, TInt aAmount = -1 );
	void FreeItemSizeAndMetaIfNeeded( SmlItemPtr_t aItem );
	void DoAppendSourceOrTargetParentL(SmlSourceOrTargetParentPtr_t aContent); //1.2 CHANGES: Source & Target parent

	// from MWBXMLDevInfCallbacks	
	IMPORT_C Ret_t smlDeviceInfoL( SmlDevInfDevInfPtr_t aContent );

private:
	TInt iItemIndex;
	TInt iDataConsumed;
	TBool iUseTruncation;
	TBool iItemWasTruncated;
	TBuf8<64> iBuffer;
	};

#endif // __XMLSYNCMLGENERATOR_H__
