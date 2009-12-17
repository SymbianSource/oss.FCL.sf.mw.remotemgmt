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
* Description:  wbxml to xml converter
*
*/


#ifndef __WBXML2XMLCONVERTER_H__
#define __WBXML2XMLCONVERTER_H__

#include "WBXMLSyncMLParser.h"
#include "WBXMLParser.h"
#include "smlmetinfdtd.h"
#include "XMLSyncMLGenerator.h"

class CWbxml2XmlConverter : public CBase, public MWBXMLSyncMLCallbacks
	{
public:
	IMPORT_C static CWbxml2XmlConverter* NewL();
	IMPORT_C static CWbxml2XmlConverter* NewLC();
	IMPORT_C void ConvertL( const TUint8* aPtr, TInt32 aLength ); 
	IMPORT_C void ConvertL( RReadStream& aSource ); 
	IMPORT_C TPtrC8 Document() const;
	IMPORT_C ~CWbxml2XmlConverter();

protected:
	void doConvertL( RReadStream& aSource );
	void ConstructL();
	
	Ret_t smlStartMessageFuncL(SmlSyncHdrPtr_t pContent);
	Ret_t smlEndMessageFuncL(Boolean_t final);
	Ret_t smlStartSyncFuncL(SmlSyncPtr_t pContent);
	Ret_t smlEndSyncFuncL();
	Ret_t smlStartAtomicFuncL(SmlAtomicPtr_t pContent);
	Ret_t smlEndAtomicFuncL();
	Ret_t smlStartSequenceFuncL(SmlSequencePtr_t pContent);
	Ret_t smlEndSequenceFuncL();
	Ret_t smlAddCmdFuncL(SmlAddPtr_t pContent);
	Ret_t smlReplaceCmdFuncL(SmlReplacePtr_t pContent);
	Ret_t smlAlertCmdFuncL(SmlAlertPtr_t pContent);
	Ret_t smlDeleteCmdFuncL(SmlDeletePtr_t pContent);
	Ret_t smlGetCmdFuncL(SmlGetPtr_t pContent);
	Ret_t smlPutCmdFuncL(SmlPutPtr_t pContent);
	Ret_t smlMapCmdFuncL(SmlMapPtr_t pContent);
	Ret_t smlResultsCmdFuncL(SmlResultsPtr_t pContent);
	Ret_t smlStatusCmdFuncL(SmlStatusPtr_t pContent);
	Ret_t smlCopyCmdFuncL(SmlCopyPtr_t pContent);
	Ret_t smlMoveCmdFuncL(SmlMovePtr_t pContent);	//1.2 CHANGES: Move command
	Ret_t smlExecCmdFuncL(SmlExecPtr_t pContent);
	Ret_t smlSearchCmdFuncL(SmlSearchPtr_t pContent);

private:
	CXMLSyncMLGenerator* iGenerator;
	};

#endif





