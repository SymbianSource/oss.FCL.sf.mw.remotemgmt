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
* Description:  SyncML document parser class.
*
*/


#ifndef __WBXMLSYNCMLPARSER_H__
#define __WBXMLSYNCMLPARSER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLHandler.h"
#include "xmlelement.h"
#include "NSmlStack.h"
#include "smldtd.h"

// ------------------------------------------------------------------------------------------------
// MWBXMLSyncMLCallbacks
// ------------------------------------------------------------------------------------------------

class MWBXMLSyncMLCallbacks
	{
public:
	virtual Ret_t smlStartMessageFuncL( SmlSyncHdrPtr_t aContent ) = 0;
	virtual Ret_t smlEndMessageFuncL( Boolean_t final ) = 0;

	virtual Ret_t smlStartSyncFuncL( SmlSyncPtr_t aContent ) = 0;
	virtual Ret_t smlEndSyncFuncL() = 0;

	virtual Ret_t smlStartAtomicFuncL( SmlAtomicPtr_t aContent ) = 0;
	virtual Ret_t smlEndAtomicFuncL() = 0;

	virtual Ret_t smlStartSequenceFuncL( SmlSequencePtr_t aContent ) = 0;
	virtual Ret_t smlEndSequenceFuncL() = 0;

	virtual Ret_t smlAddCmdFuncL( SmlAddPtr_t aContent ) = 0;
	virtual Ret_t smlAlertCmdFuncL( SmlAlertPtr_t aContent ) = 0;
	virtual Ret_t smlDeleteCmdFuncL( SmlDeletePtr_t aContent ) = 0;
	virtual Ret_t smlGetCmdFuncL( SmlGetPtr_t aContent ) = 0;
	virtual Ret_t smlPutCmdFuncL( SmlPutPtr_t aContent ) = 0;
	virtual Ret_t smlMapCmdFuncL( SmlMapPtr_t aContent ) = 0;
	virtual Ret_t smlResultsCmdFuncL( SmlResultsPtr_t aContent ) = 0;
	virtual Ret_t smlStatusCmdFuncL( SmlStatusPtr_t aContent ) = 0;
	virtual Ret_t smlReplaceCmdFuncL( SmlReplacePtr_t aContent ) = 0;
	virtual Ret_t smlCopyCmdFuncL( SmlCopyPtr_t param ) = 0;
	virtual Ret_t smlMoveCmdFuncL( SmlMovePtr_t param ) = 0;	//1.2 CHANGES: Move command added
	virtual Ret_t smlExecCmdFuncL( SmlExecPtr_t aContent ) = 0;
	virtual Ret_t smlSearchCmdFuncL( SmlSearchPtr_t aContent ) = 0;
	};

// ------------------------------------------------------------------------------------------------
// CWBXMLSyncMLDocHandler
// ------------------------------------------------------------------------------------------------
class CWBXMLSyncMLDocHandler : public CBase, public MWBXMLDocumentHandler, public MWBXMLExtensionHandler
	{
public:
	IMPORT_C static CWBXMLSyncMLDocHandler* NewL( MWBXMLSyncMLCallbacks* aCallbacks );
	IMPORT_C ~CWBXMLSyncMLDocHandler();

private:
	void ConstructL();
	CWBXMLSyncMLDocHandler( MWBXMLSyncMLCallbacks* aCallbacks );
	void AddElementL( CXMLElement* aElement );

	// from MWBXMLDocumentHandler
	void StartDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset );
	void StartDocumentL( TUint8 aVersion, const TDesC8& aPublicIdStr, TUint32 aCharset );
	void EndDocumentL();
	void StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& aAttributes );
	void EndElementL( TWBXMLTag aTag );
	void CodePageSwitchL( TUint8 aPage );
	void CharactersL( const TDesC8& aBuffer );
	void DocumentChangedL();

	// from MWBXMLExtensionHandler
	void Ext_IL( TWBXMLTag aTag, const TDesC8& aData );	
	void Ext_TL( TWBXMLTag aTag, TUint32 aData );
	void ExtL( TWBXMLTag aTag );
	void OpaqueL( const TDesC8& aData );

private:
	CNSmlStack<CXMLElement>* iCmdStack;
	CNSmlStack<CXMLElement>* iCleanupStack;
	MWBXMLSyncMLCallbacks* iCallbacks;
	}; 

#endif // __WBXMLSYNCMLPARSER_H__
