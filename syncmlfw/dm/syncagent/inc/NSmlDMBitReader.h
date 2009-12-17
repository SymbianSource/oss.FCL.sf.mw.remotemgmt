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
* Description:  Bit reader utility
*
*/



#ifndef __NSMLDMBITREADER_H
#define __NSMLDMBITREADER_H

//INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION

/**
* 
*  
*/
class TNSmlDMBitreader : protected RDesReadStream
	{
public:
	TNSmlDMBitreader( const TDesC8& aData );
	TUint32 ReadUintL( TInt aBitcount );
	void ReadL( TInt aBitcount, TDes8& aBuffer );
private:
	TUint32 FetchNextBitL();
private:
	TUint8 iByte;
	TInt8 iBits;
	};


#endif  // __NSMLDMBITREADER_H
            
// End of File
