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
* Description:  Dynamic workspace class.
*
*/


#ifndef __XMLDYNAMICWORKSPACE_H__
#define __XMLDYNAMICWORKSPACE_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

#include "XMLWorkspace.h"

// ------------------------------------------------------------------------------------------------
// CXMLWorkspace
// ------------------------------------------------------------------------------------------------
class CXMLDynamicWorkspace : public CXMLWorkspace
	{
public:
	IMPORT_C static CXMLDynamicWorkspace* NewL();
	IMPORT_C ~CXMLDynamicWorkspace();
	
	IMPORT_C void Reset();
	IMPORT_C void WriteL( const TDesC8& aData );
	IMPORT_C void WriteL( const TUint8 aData );
	IMPORT_C TPtrC8 Buffer() const;

	IMPORT_C TInt FreeSize() const;
	IMPORT_C TInt MaxSize() const;
	IMPORT_C TInt Size() const;

private:
	void Delete( TInt aPos, TInt aLength );
	void ConstructL();
	CXMLDynamicWorkspace();

private:
	CBufFlat* iBuffer;
	};

#endif // __XMLWORKSPACE_H__