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
* Description:  XML syncml DTD parser
*
*/


#ifndef __XMLSYNCMLPARSER_H__
#define __XMLSYNCMLPARSER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLSyncMLParser.h"

// ------------------------------------------------------------------------------------------------
// CXMLSyncMLDocHandler
// ------------------------------------------------------------------------------------------------
class CXMLSyncMLDocHandler : public CBase, public MWBXMLDocumentHandler, public MWBXMLExtensionHandler
	{
public:
	IMPORT_C static CXMLSyncMLDocHandler* NewL( MWBXMLSyncMLCallbacks* aCallbacks );
	IMPORT_C ~CXMLSyncMLDocHandler();

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
	CXMLSyncMLDocHandler( MWBXMLSyncMLCallbacks* aCallbacks );

private:
	CNSmlStack<CXMLElement>* iCmdStack;
	CNSmlStack<CXMLElement>* iCleanupStack;
	MWBXMLSyncMLCallbacks* iCallbacks;
	}; 

#endif // __XMLSYNCMLPARSER_H__
