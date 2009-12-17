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
* Description:  Static workspace class.
*
*/


#ifndef __XMLSTATICWORKSPACE_H__
#define __XMLSTATICWORKSPACE_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

#include "XMLWorkspace.h"

// ------------------------------------------------------------------------------------------------
// CXMLWorkspace
// ------------------------------------------------------------------------------------------------
class CXMLStaticWorkspace : public CXMLWorkspace
	{
public:
	IMPORT_C static CXMLStaticWorkspace* NewL( TInt aBufferSize );
	IMPORT_C ~CXMLStaticWorkspace();
	
	IMPORT_C void Reset();
	IMPORT_C void WriteL( const TDesC8& aData );
	IMPORT_C void WriteL( const TUint8 aData );
	IMPORT_C TPtrC8 Buffer() const;

	IMPORT_C TInt FreeSize() const;
	IMPORT_C TInt MaxSize() const;
	IMPORT_C TInt Size() const;

private:
	void Delete( TInt aPos, TInt aLength );
	void ConstructL( TInt aBufferSize );
	inline TPtr8 Ptr() const;
	CXMLStaticWorkspace();

private:
	HBufC8* iBuffer;
	TUint32 iMaxLength;
	};

#include "xmlstaticworkspace.inl"

#endif // __XMLSTATICWORKSPACE_H__