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
* Description:  XML device information parser
*
*/


#ifndef __XMLDEVINFPARSER_H__
#define __XMLDEVINFPARSER_H__

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
	virtual Ret_t smlDeviceInfo( SmlDevInfDevInfPtr_t aContent ) = 0;
	};

// ------------------------------------------------------------------------------------------------
// CXMLDevInfDocHandler
// ------------------------------------------------------------------------------------------------
class CXMLDevInfDocHandler : public CBase, public MWBXMLDocumentHandler, public MWBXMLExtensionHandler
	{
public:
	IMPORT_C static CXMLDevInfDocHandler* NewL( MWBXMLDevInfCallbacks* aCallbacks );
	IMPORT_C ~CXMLDevInfDocHandler();

protected:
	void StartDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset );
	void StartDocumentL( TUint8 aVersion, const TDesC8& aPublicIdStr, TUint32 aCharset );
	void EndDocumentL();
	void StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& aAttributes );
	void EndElementL( TWBXMLTag aTag );
	void CodePageSwitchL( TUint8 aPage );
	void CharactersL( const TDesC8& aBuffer );
	void DocumentChangedL();

	void Ext_IL( TWBXMLTag aTag, const TDesC8& aData );	
	void Ext_TL( TWBXMLTag aTag, TUint32 aData );
	void ExtL( TWBXMLTag aTag );
	void OpaqueL( const TDesC8& aData );

	void AddElementL( CXMLElement* aElement );

private:
	void ConstructL();
	CXMLDevInfDocHandler( MWBXMLDevInfCallbacks* aCallbacks );

private:
	CNSmlStack<CXMLElement>* iCmdStack;
	CNSmlStack<CXMLElement>* iCleanupStack;
	MWBXMLDevInfCallbacks* iCallbacks;
	}; 

#endif // __XMLDEVINFPARSER_H__
