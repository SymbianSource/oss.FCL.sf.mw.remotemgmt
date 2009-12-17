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
* Description:  Document and extension handler interfaces.
*
*/


#ifndef __WBXMLDOCUMENTHANDLER_H__
#define __WBXMLDOCUMENTHANDLER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

#include "WBXMLDefs.h"

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class CWBXMLAttributes;

// ------------------------------------------------------------------------------------------------
// MWBXMLDocumentHandler
// ------------------------------------------------------------------------------------------------
class MWBXMLDocumentHandler
	{
public:
	virtual void StartDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset ) = 0;
	virtual void StartDocumentL( TUint8 aVersion, const TDesC8& aPublicIdStr, TUint32 aCharset ) = 0;
	virtual void EndDocumentL() = 0;
	virtual void StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& aAttributes ) = 0;
	virtual void EndElementL( TWBXMLTag aTag ) = 0;
	virtual void CodePageSwitchL( TUint8 aPage ) = 0;
	virtual void CharactersL( const TDesC8& aBuffer ) = 0;
	virtual void DocumentChangedL() = 0;
	};

// ------------------------------------------------------------------------------------------------
// MWBXMLExtensionHandler
// ------------------------------------------------------------------------------------------------
class MWBXMLExtensionHandler
	{
public:
	virtual void Ext_IL( TWBXMLTag aTag, const TDesC8& aData ) = 0;
	virtual void Ext_TL( TWBXMLTag aTag, TUint32 aData ) = 0;
	virtual void ExtL( TWBXMLTag aTag ) = 0;
	virtual void OpaqueL( const TDesC8& aData ) = 0;
	};

#endif // __WBXMLDOCUMENTHANDLER_H__