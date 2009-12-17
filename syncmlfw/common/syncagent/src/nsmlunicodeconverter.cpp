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


#include <utf.h>
#include <nsmlunicodeconverter.h>
#include "nsmlcliagconstants.h"
// ---------------------------------------------------------
// NSmlUnicodeConverter::HBufC8InUTF8LC()
// Converts Unicode data to UTF-8
// No size limitations, this function re-allocates memory
// when needed. Note that a pointer given as a parameter is updated.
// ---------------------------------------------------------
EXPORT_C TInt NSmlUnicodeConverter::HBufC8InUTF8LC( const TDesC& aUnicodeData, HBufC8*& aUTF8Data )
	{
	TPtr8 outputBuffer = HBufC8::NewLC( KNSmlConvBufferSize )->Des(); 
	TPtrC16 remainderOfUnicodeText( aUnicodeData );
	TInt convertedDataLength( 0 );
	HBufC8* convertedData = HBufC8::NewL( aUnicodeData.Length() );
	HBufC8* oldPointer = NULL;
	TInt returnValue( 0 );
	TInt pushed( 0 );
	do
		{
		returnValue = CnvUtfConverter::ConvertFromUnicodeToUtf8( outputBuffer, remainderOfUnicodeText );
		if ( returnValue==CnvUtfConverter::EErrorIllFormedInput )
			{
			delete convertedData;
            User::Leave( KErrCorrupt );
			}
        else if ( returnValue < 0 ) 
			{
            delete convertedData;
			User::Leave( KErrGeneral );
			}
		if ( outputBuffer.Size() > 0 )
			{
			convertedDataLength += outputBuffer.Length();
			if ( convertedDataLength > convertedData->Des().MaxLength() )
				{
				oldPointer = convertedData;
				convertedData = convertedData->ReAlloc( convertedDataLength );
				if ( convertedData == NULL )
					{
					delete oldPointer;
					User::Leave( KErrNoMemory );
					}
				}
			convertedData->Des().Append( outputBuffer );
			}
		if ( returnValue > 0 )
			{
			remainderOfUnicodeText.Set( remainderOfUnicodeText.Right( returnValue ) );
			}
		}
		while ( returnValue > 0 ); 
	CleanupStack::PopAndDestroy(); //outputBuffer
	aUTF8Data = convertedData;
	CleanupStack::PushL( aUTF8Data );
	pushed++;
	return pushed;
	}
// ---------------------------------------------------------
// NSmlUnicodeConverter::HBufC16InUnicodeL()
// 
// ---------------------------------------------------------
EXPORT_C TInt NSmlUnicodeConverter::HBufC16InUnicodeL( const TDesC8& aUtf8, HBufC*& aUnicodeData )
	{
	TInt pushed;
	pushed = HBufC16InUnicodeLC( aUtf8, aUnicodeData );
	CleanupStack::Pop();
	return pushed;
	}
// ---------------------------------------------------------
// TNSmlUnicodeConverter::HBufC16InUnicodeLC()
// Converts UTF-8 data to Unicode.
// No size limitations, this function re-allocates memory
// when needed. Note that a pointer given as a parameter is updated.
// ---------------------------------------------------------
EXPORT_C TInt NSmlUnicodeConverter::HBufC16InUnicodeLC( const TDesC8& aUtf8, HBufC*& aUnicodeData )
	{
	TPtr outputBuffer = HBufC::NewLC( KNSmlConvBufferSize )->Des(); 
	TPtrC8 remainderOfUtf8 (aUtf8 );
	TInt convertedDataLength( 0 );
	HBufC16* convertedData = HBufC16::NewL( aUtf8.Length() / 2 );
	HBufC16* oldPointer = NULL;
	TInt returnValue( 0 );
	TInt pushed( 0 );
	do
		{
		returnValue = CnvUtfConverter::ConvertToUnicodeFromUtf8( outputBuffer, remainderOfUtf8 );
		if (returnValue == CnvUtfConverter::EErrorIllFormedInput )
			{
            delete convertedData;
			User::Leave( KErrCorrupt );
			}
		else if ( returnValue < 0 ) // future-proof against "TError" expanding
			{
            delete convertedData;
			User::Leave( KErrGeneral );
			}   
		if ( outputBuffer.Size() > 0 )
			{
			convertedDataLength += outputBuffer.Length();
			if ( convertedDataLength > convertedData->Des().MaxLength() )
				{
				oldPointer = convertedData;
				convertedData = convertedData->ReAlloc( convertedDataLength );
				if ( convertedData == NULL )
					{
					delete oldPointer;
					User::Leave( KErrNoMemory );
					}
				}
			convertedData->Des().Append( outputBuffer );
			}
		if ( returnValue > 0 )
			{
			remainderOfUtf8.Set( remainderOfUtf8.Right( returnValue ) );
			}
		}
	while ( returnValue > 0 );
	CleanupStack::PopAndDestroy(); //outputBuffer
	aUnicodeData = convertedData;
	CleanupStack::PushL( aUnicodeData );
	pushed++;
	return pushed;
	}
