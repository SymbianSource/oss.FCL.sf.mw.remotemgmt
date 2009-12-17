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



// INCLUDE FILES
#include "nsmldmbitreader.h"

// ---------------------------------------------------------
// TNSmlDMBitreader::TNSmlDMBitreader()
// Constructor
// ---------------------------------------------------------
TNSmlDMBitreader::TNSmlDMBitreader( const TDesC8& aData ) : RDesReadStream(aData), iByte(0), iBits(0)
	{
	}

// ---------------------------------------------------------
// TNSmlDMBitreader::ReadUintL()
// 
// ---------------------------------------------------------
TUint32 TNSmlDMBitreader::ReadUintL( TInt aBitcount )
	{
	if( (aBitcount < 0) || (aBitcount > 32) )
		{
		User::Leave(KErrOverflow);
		}

	TUint32 value(0);

	while( aBitcount-- )
		{
		value <<= 1;
		value |= FetchNextBitL();
		}
	return value;
	}

// ---------------------------------------------------------
// TNSmlDMBitreader::ReadL()
// 
// ---------------------------------------------------------
void TNSmlDMBitreader::ReadL( TInt aBitcount, TDes8& aBuffer )
	{
	if( (aBitcount>>3) > aBuffer.MaxLength() )
		{
		User::Leave(KErrOverflow);
		}
	aBuffer.Zero();
	for( TInt i = 0; i < (aBitcount>>3); i++ )
		{
		aBuffer.Append((TUint8)ReadUintL(8));
		}
	}
// ---------------------------------------------------------
// TNSmlDMBitreader::FetchNextBitL()
// 
// ---------------------------------------------------------
TUint32 TNSmlDMBitreader::FetchNextBitL()
	{
	if( --iBits < 0 )
		{
		iByte = ReadUint8L();
		iBits = 7;
		}
	return (iByte>>iBits)&0x1;
	}


//  End of File  
