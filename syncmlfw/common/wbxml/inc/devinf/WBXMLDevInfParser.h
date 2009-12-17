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
* Description:  Device Information document parser class.
*
*/


#ifndef __WBXMLDEVINFPARSER_H__
#define __WBXMLDEVINFPARSER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLHandler.h"
#include "xmlelement.h"
#include "NSmlStack.h"
#include "smldevinfdtd.h"
#include "smldevinftags.h"

// ------------------------------------------------------------------------------------------------
// MWBXMLDevInfCallbacks
// ------------------------------------------------------------------------------------------------
class MWBXMLDevInfCallbacks
	{
public:
	virtual Ret_t smlDeviceInfoL( SmlDevInfDevInfPtr_t aContent ) = 0;
	};

// ------------------------------------------------------------------------------------------------
// CWBXMLDevInfDocHandler
// ------------------------------------------------------------------------------------------------
class CWBXMLDevInfDocHandler : public CBase, public MWBXMLDocumentHandler, public MWBXMLExtensionHandler
	{
public:
	IMPORT_C static CWBXMLDevInfDocHandler* NewL( MWBXMLDevInfCallbacks* aCallbacks );
	IMPORT_C ~CWBXMLDevInfDocHandler();

private:
	void ConstructL();
	CWBXMLDevInfDocHandler( MWBXMLDevInfCallbacks* aCallbacks );
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
	MWBXMLDevInfCallbacks* iCallbacks;
	}; 

#endif // __WBXMLDEVINFPARSER_H__
