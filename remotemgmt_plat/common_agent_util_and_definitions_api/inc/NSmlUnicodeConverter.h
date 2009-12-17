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
* Description:  UNICODE conversion
*
*/


#ifndef __NSMLUNICODECONVERTER_H
#define __NSMLUNICODECONVERTER_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION
class NSmlUnicodeConverter 
	{
	public: 
	IMPORT_C static TInt HBufC8InUTF8LC( const TDesC& aUnicodeData, HBufC8*& aUTF8Data );
	IMPORT_C static TInt HBufC16InUnicodeL( const TDesC8& aUtf8, HBufC*& aUnicodeData );
	IMPORT_C static TInt HBufC16InUnicodeLC( const TDesC8& aUtf8, HBufC*& aUnicodeData );
	};

#endif // __NSMLUNICODECONVERTER_H
