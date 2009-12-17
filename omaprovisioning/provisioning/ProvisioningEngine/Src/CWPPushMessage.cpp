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
* Description:  Helps in saving/loading a push message.
*
*/


//  INCLUDE FILES
#include <e32base.h>
#include <msvstore.h>
#include <hash.h>
#include <chttpresponse.h>
#include <centralrepository.h>
#include "ProvisioningVariant.hrh"
#include "CWPPushMessage.h"
#include "ProvisioningDebug.h"
#include "ProvisioningUIDs.h"
#include "CWPEngine.pan"
#include "ProvisioningInternalCRKeys.h"

// CONSTANTS

/// Short integers must be masked with this to get real number
const TUint KShortIntegerMask = 0x7f;

/// In q-values if the number is above this number, there are more bytes.
const TUint KQValueContinuation = 0x80;

/// Quotes strings start with this constant.
const TUint KQuotedStringStart = 34;

/// Quotes strings start with this constant.
const TUint KQuotedTextStringStart = 127;

/// Maximum length of a long integer.
const TUint KMaxLongIntegerLength = 30;

/// Types of coding a value in a parameter
enum TParameterCodingType {
    EQValue,
    EWellKnownCharset,
    EVersionValue,
    EIntegerValue,
    ETextString,
    EFieldName,
    EShortInteger,
    EConstrainedEncoding,
    EDeltaSecondsValue,
    ENoValue,
    ETextValue,
    EDateValue
    };

/// Well-known parameter assignments and their mappings to value types
/// (WAP-230-WSP-20010705-a, table 38)
const TParameterCodingType KParameterTypes[] = 
    {
    EQValue,
    EWellKnownCharset,
    EVersionValue,
    EIntegerValue,
    ENoValue,
    ETextString,
    ETextString,
    EFieldName,
    EShortInteger,
    EConstrainedEncoding,
    ETextString,
    ETextString,
    ETextString,
    ETextString,
    EDeltaSecondsValue,
    ETextString,
    ENoValue,
    EShortInteger,
    ETextValue,
    EDateValue,
    EDateValue,
    EDateValue,
    EIntegerValue,
    ETextValue,
    ETextValue,
    ETextValue,
    ETextValue,
    ETextValue,
    ETextValue,
    ETextValue
    };

/// Date-values are seconds from this date
_LIT( KDateValueStart, "19700101:000000.000000" );

/// WSP constant for field SEC
const TInt KWSPHeaderSEC = 0x11;

/// WSP constant for field MAC
const TInt KWSPHeaderMAC = 0x12;

/// Number of BCD digits in byte
const TInt KNumDigitsInByte = 2;

/// Number of bits in half-byte
const TInt KNumBitsInNibble = 4;

/// Ascii code for zero
const TUint8 KZero = '0';

/// Padding half-byte
const TUint8 KPadNibble = 0xf;

/// First nibble
const TUint8 KFirstNibble = 0x1;

/// Parity bit number in first nibble
const TUint KParityBitNum = 3;

/// Content type code for OMA Provisioning messages
const TUint8 KContentType = 0xb6;

/// Provisioning message format version when stored in Messaging Store
const TInt KProvisioningMsgVersion = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPPushMessage::CWPPushMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPPushMessage::CWPPushMessage()
: iInitiator( KNullDesC8 ), iSEC( KSECNONE )
    {
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPPushMessage::ConstructL()
    {
    iHeader = CHTTPResponse::NewL();
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPPushMessage* CWPPushMessage::NewL()
    {
    CWPPushMessage* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPPushMessage* CWPPushMessage::NewLC()
    {
    CWPPushMessage* self = new( ELeave ) CWPPushMessage;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// Destructor
CWPPushMessage::~CWPPushMessage()
    {
    delete iBody;
    delete iHeader;
    delete iOriginator;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::StoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::StoreL( CMsvStore& aStore ) const
    {
    TUid uid;
    uid.iUid = KProvisioningMessageStreamUid;

    aStore.Remove( uid );

    RMsvWriteStream stream;
    stream.AssignLC( aStore, uid );
    ExternalizeL( stream );
    CleanupStack::PopAndDestroy(); // stream
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::RestoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::RestoreL( CMsvStore& aStore )
    {
    RMsvReadStream stream;
    stream.OpenLC( aStore, TUid::Uid( KProvisioningMessageStreamUid ) );
    InternalizeL( stream );
    CleanupStack::PopAndDestroy(); // stream

    ParseHeaderL();
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::ExternalizeL( RWriteStream& aStream ) const
    {
    // Save version
    aStream.WriteInt32L( KProvisioningMsgVersion );

    // Save the header
    aStream.WriteInt32L( iHeader->Response().Length() );
    aStream << iHeader->Response();
    aStream.WriteInt8L( iAuthenticated );
    aStream.WriteInt8L( iSaved );

    // Save the body
    aStream.WriteInt32L( iBody->Length() );
    aStream << *iBody;

    // The sender
    if( iOriginator )
        {
        aStream.WriteInt32L( iOriginator->Length() );
        aStream << *iOriginator;
        }
    else
        {
        aStream.WriteInt32L( KNullDesC8().Length() );
        }

    aStream.CommitL();
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::InternalizeL( RReadStream& aStream )
    {
    // Restore the header
    TInt version( aStream.ReadInt32L() );

    TInt length( aStream.ReadInt32L() );
    HBufC8* header = HBufC8::NewLC( aStream, length );
    iAuthenticated = aStream.ReadInt8L();
    iSaved = aStream.ReadInt8L();

    // Restore the body
    length = aStream.ReadInt32L();
    HBufC8* body = HBufC8::NewL( aStream, length );

    // Store header and body
    CleanupStack::Pop(); // header
    Set( header, body );

    if( version == KProvisioningMsgVersion )
        {
        length = aStream.ReadInt32L();
        if( length > 0 )
            {
            delete iOriginator;
            iOriginator = NULL;
            iOriginator = HBufC8::NewL( aStream, length );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::SetL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::SetL( const TDesC8& aHeader, const TDesC8& aBody )
    {
    HBufC8* header = aHeader.AllocLC();
    HBufC8* body = aBody.AllocL();
    CleanupStack::Pop(); // header
    Set( header, body );
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::Set
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::Set( HBufC8* aHeader, HBufC8* aBody )
    {
    __ASSERT_DEBUG( aHeader && aBody, Panic( EWPNullMessage ) );
    iHeader->AddResponse( aHeader );

    delete iBody;
    iBody = aBody;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::SetOriginatorL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::SetOriginatorL( const TDesC8& aOrig )
    {
    HBufC8* orig = aOrig.AllocL();
    delete iOriginator;
    iOriginator = orig;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::Header
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPPushMessage::Header() const
    {
    if( iHeader )
        {
        return iHeader->Response();
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::Body
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPPushMessage::Body() const
    {
    if( iBody )
        {
        return *iBody;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::Originator
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPPushMessage::Originator() const
    {
    if( iOriginator )
        {
        return *iOriginator;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::SetAuthenticated
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::SetAuthenticated( TBool aAuthenticated )
    {
    iAuthenticated = aAuthenticated;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::Authenticated
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPPushMessage::Authenticated() const
    {
    return iAuthenticated;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::SetSaved
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::SetSaved( TBool aSaved )
    {
    iSaved = aSaved;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::Saved
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPPushMessage::Saved() const
    {
    return iSaved;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::InitiatorURI
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPPushMessage::InitiatorURI() const
    {
    return iInitiator;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::MAC
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPPushMessage::MAC() const
    {
    return iMAC;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::SEC
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CWPPushMessage::SEC() const
    {
    return iSEC;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::PushFlag
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPPushMessage::PushFlag() const
    {
    return iPushFlag;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::ParseHeader
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPPushMessage::ParseHeaderL()
    {
    // CHTTPResponse panicks if it is used for parsing a zero-length 
    // descriptor.
    if( iHeader->Response().Length() > 0 )
        {
        // We use lexer for safety.
        TLex8 contentType( ContentTypeHeader( *iHeader ) );
        if( !contentType.Eos() )
            {
            ParseContentType( contentType );
            }
        
        TLex8 initiatorURI( InitiatorURIHeader( *iHeader ) );
        if( !initiatorURI.Eos() )
            {
            ParseInitiatorURI( initiatorURI );
            }
        
        TLex8 pushFlag( PushFlagHeader( *iHeader ) );
        if( !pushFlag.Eos() )
            {
            ParsePushFlag( pushFlag );
            }
        }
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::ParseInitiatorURI
// -----------------------------------------------------------------------------
//
void CWPPushMessage::ParseInitiatorURI( TLex8& aPointer )
    {
    iInitiator.Set( GetTextString( aPointer ) );
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::ParsePushFlag
// -----------------------------------------------------------------------------
//
void CWPPushMessage::ParsePushFlag( TLex8& aPointer )
    {
    TInt64 pushFlag( GetIntegerValue( aPointer ) );

    iPushFlag = I64LOW(pushFlag);
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::ParseContentType
// -----------------------------------------------------------------------------
//
void CWPPushMessage::ParseContentType( TLex8& aPointer )
    {
    // Go through the whole content type header.
    while( !aPointer.Eos() )
        {
        // Each parameter might be well-known (integer) or unknown (text)
        if( IsIntegerValue( aPointer ) )
            {
            // For well-known parameters, the token is an integer value
            TUint paramToken( I64LOW( GetIntegerValue( aPointer ) ) );

            // These are filled with results from parsing.
            TInt resultInteger( 0 );
            TPtrC8 resultString;
            
            // Make sure paramToken fits into KParameterTypes table
            if( paramToken 
                < sizeof(KParameterTypes)/sizeof(TParameterCodingType))
                {
                // Get the coding and use it to determine how we should decode 
                // the next parameter value. We actually ignore all results 
                // except short integer (SEC) and text-value (MAC), but the 
                // rest of the parameters have to be parsed anyway.
                TParameterCodingType coding( KParameterTypes[paramToken] );

                switch( coding )
                    {
                    case EQValue:
                        GetQValue( aPointer );
                        break;

                    case EWellKnownCharset:
                        GetWellKnownCharset( aPointer );
                        break;

                    case EVersionValue:
                        GetVersionValue( aPointer );
                        break;

                    case EIntegerValue:
                        GetIntegerValue( aPointer );
                        break;

                    case ETextString:
                        GetTextString( aPointer );
                        break;

                    case EFieldName:
                        GetFieldName( aPointer );
                        break;

                    case EShortInteger:
                        resultInteger = GetShortInteger( aPointer );
                        break;

                    case EConstrainedEncoding:
                        GetConstrainedEncoding( aPointer );
                        break;

                    case EDeltaSecondsValue:
                        GetDeltaSecondsValue( aPointer );
                        break;

                    case ENoValue:
                        GetNoValue( aPointer );
                        break;

                    case ETextValue:
                        resultString.Set( GetTextValue( aPointer ) );
                        break;

                    case EDateValue:
                        GetDateValue( aPointer );
                        break;

                    default:
                        break;
                    }

                // We have a result. We're actually only interested in
                // SEC and MAC parameters, so we save them here.
                switch( paramToken )
                    {
                    case KWSPHeaderSEC:
                        iSEC = resultInteger;
                        break;

                    case KWSPHeaderMAC:
                        iMAC.Set( resultString );
                        break;

                    default:
                        break;
                    }
                }
            }
        else
            {
            // Unknown parameter. Its name is in text, and the value
            // might be an integer or text.
            GetTokenText( aPointer );
            if( IsIntegerValue( aPointer ) )
                {
                GetIntegerValue( aPointer );
                }
            else
                {
                GetTextValue( aPointer );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetQValue
// -----------------------------------------------------------------------------
//
TUint CWPPushMessage::GetQValue( TLex8& aPointer ) const
    {
    // q-value is an integer. It is coded as 7 bits per byte.
    // The highest bit determines if the number continues.
    TUint result( 0 );
    TBool lastDigit( EFalse );

    while( !aPointer.Eos() && !lastDigit )
        {
        TInt one( aPointer.Get() );

        result = (result << 7) || (one & ~KQValueContinuation);

        if( (one & KQValueContinuation) == 0 )
            {
            lastDigit = ETrue;
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetDateValue
// -----------------------------------------------------------------------------
//
TTime CWPPushMessage::GetDateValue( TLex8& aPointer ) const
    {
    // Date-value is a long integer and represents seconds
    // since KDateValueStart.
    TInt64 result( GetLongInteger( aPointer ) );

    TTime start( KDateValueStart );
    TTimeIntervalSeconds delta;

    delta = I64LOW(result);

    start += delta;
    
    return start;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetNoValue
// -----------------------------------------------------------------------------
//
void CWPPushMessage::GetNoValue( TLex8& aPointer ) const
    {
    // We're not checking anything here. No value is no value.
    aPointer.Get();
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetDeltaSecondsValue
// -----------------------------------------------------------------------------
//
TInt64 CWPPushMessage::GetDeltaSecondsValue( TLex8& aPointer ) const
    {
    return GetIntegerValue( aPointer );
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetConstrainedEncoding
// -----------------------------------------------------------------------------
//
TUint CWPPushMessage::GetConstrainedEncoding( TLex8& aPointer ) const
    {
    // Constrained encoding can be extension media or short integer
    TUint result( 0 );

    if( !aPointer.Eos() )
        {
        TUint first( aPointer.Peek() );

        if( first > KShortIntegerMask )
            {
            result = GetShortInteger( aPointer );
            }
        else
            {
            // Just skip the text version
            GetTokenText( aPointer );
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetFieldName
// -----------------------------------------------------------------------------
//
TUint CWPPushMessage::GetFieldName( TLex8& aPointer ) const
    {
    // Field name can be a short integer or text.
    TUint result( 0 );

    if( !aPointer.Eos() )
        {
        TUint first( aPointer.Peek() );

        if( first > KShortIntegerMask )
            {
            result = GetShortInteger( aPointer );
            }
        else
            {
            // Only well-known fields are read
            GetTokenText( aPointer );
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetTokenText
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::GetTokenText( TLex8& aPointer ) const
    {
    // Token text is just characters with an end-of-string marker.
    aPointer.Mark();

    while( !aPointer.Eos() && aPointer.Get() != EKeyNull )
        {
        // Do nothing
        }
    
    return aPointer.MarkedToken();
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetIntegerValue
// -----------------------------------------------------------------------------
//
TInt64 CWPPushMessage::GetIntegerValue( TLex8& aPointer ) const
    {
    // Integer value can be a short integer or a long integer.
    // Short integer is always >KShortIntegerMask.
    TInt64 result( 0 );

    if( !aPointer.Eos() )
        {
        TUint first( aPointer.Peek() );

        if( first > KShortIntegerMask )
            {
            result = GetShortInteger( aPointer );
            }
        else
            {
            result = GetLongInteger( aPointer );
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::IsIntegerValue
// -----------------------------------------------------------------------------
//
TBool CWPPushMessage::IsIntegerValue( TLex8& aPointer ) const
    {
    // Integer values either are above KShortIntegerMask or
    // their first byte is <=KMaxLongIntegerLength.
    TBool result( EFalse );

    if( !aPointer.Eos() )
        {
        TUint first( aPointer.Peek() );

        if( first > KShortIntegerMask || first <= KMaxLongIntegerLength )
            {
            result = ETrue;
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetLongInteger
// -----------------------------------------------------------------------------
//
TInt64 CWPPushMessage::GetLongInteger( TLex8& aPointer ) const
    {
    // Long integer has length as first byte.
    TInt64 result( 0 );

    TInt length( aPointer.Get() );
    
    for( TInt i( 0 ); i < length; i++ )
        {
        result = (result << 8) + TInt( aPointer.Get() );
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetTextValue
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::GetTextValue( TLex8& aPointer ) const
    {
    // Text-value can be quoted, so skip that first.
    if( aPointer.Peek() == KQuotedStringStart )
        {
        aPointer.Inc();
        }
    aPointer.Mark();

    // It is null-terminated
    while( aPointer.Get() != EKeyNull )
        {
        // Do nothing
        }

    // We don't want to have NULL in the resulting descriptor, so
    // back that out.
    TPtrC8 result( aPointer.MarkedToken() );
    result.Set( result.Left( Max( 0, result.Length()-1 ) ) );
    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetWellKnownCharset
// -----------------------------------------------------------------------------
//
TInt64 CWPPushMessage::GetWellKnownCharset( TLex8& aPointer ) const
    {
    return GetIntegerValue( aPointer );
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetVersionValue
// -----------------------------------------------------------------------------
//
TUint CWPPushMessage::GetVersionValue( TLex8& aPointer ) const
    {
    // Version-value is a short integer or text. Handle that.
    TUint result( 0 );

    if( !aPointer.Eos() )
        {
        TUint first( aPointer.Peek() );
        
        if( first > KShortIntegerMask )
            {
            result = GetShortInteger( aPointer );
            }
        else
            {
            GetTextString( aPointer );
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetShortInteger
// -----------------------------------------------------------------------------
//
TUint CWPPushMessage::GetShortInteger( TLex8& aPointer ) const
    {
    return aPointer.Get() & KShortIntegerMask;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::GetTextString
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::GetTextString( TLex8& aPointer ) const
    {
    // Text-string can be quoted.
    if( aPointer.Peek() == KQuotedTextStringStart )
        {
        aPointer.Inc();
        }
    aPointer.Mark();

    while( aPointer.Get() != EKeyNull )
        {
        // Nothing
        }

    // We don't want to have NULL in the resulting descriptor, so
    // back that out.
    aPointer.UnGet();
    TPtrC8 result( aPointer.MarkedToken() );
    aPointer.Inc();
    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::ContentTypeHeader
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::ContentTypeHeader( CHTTPResponse& aResponse ) const
    {
    // We use CHTTPResponse to find first the content-type header.
    TPtrC8 contentType( KNullDesC8 );
    TPtrC8 result( KNullDesC8 );

    if( aResponse.FindBinaryDescField( EHttpContentType, contentType )
        && contentType.Length() > 0 )
        {
        result.Set( contentType );

        if( result[0] == KContentType )
            {
            result.Set( result.Mid(1) );
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPPushMessage::InitiatorURIHeader
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::InitiatorURIHeader( CHTTPResponse& aResponse ) const
    {
    // We use CHTTPResponse to find first the content-type header.
    TPtrC8 initiatorURI( KNullDesC8 );
    TPtrC8 result( KNullDesC8 );

    if( aResponse.FindBinaryDescField( EHttpXWapInitiatorURI, initiatorURI )
        && initiatorURI.Length() > 0 )
        {
        result.Set( initiatorURI );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::PushFlagHeader
// -----------------------------------------------------------------------------
//
TPtrC8 CWPPushMessage::PushFlagHeader( CHTTPResponse& aResponse ) const
    {
    // We use CHTTPResponse to find first the content-type header.
    TPtrC8 pushFlag( KNullDesC8 );
    TPtrC8 result( KNullDesC8 );

    if( aResponse.FindBinaryDescField( EHttpPushFlag, pushFlag )
        && pushFlag.Length() > 0 )
        {
        result.Set( pushFlag );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::AuthenticateL
// The method first checks if there is security information in the message
// with aMessage.SEC(). If yes, the authentication code described in OMA
// Provisioning Bootstrap specification is calculated and compared to the
// one in message header. If the codes match, authentication has been 
// performed succesfully.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPPushMessage::AuthenticateL( const TDesC& aIMSI, 
                                            const TDesC& aPIN )
    {
    FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL:" ) );
    
    TInt result( KWPAuthResultAuthenticated );

    // Retrieve the MAC from message
    ParseHeaderL();

    // The HMAC is in ASCII HEX format. Convert to binary.
    HBufC8* headerMac = PackLC( MAC() );

    // Create space for key
    HBufC8* key = HBufC8::NewLC( Max( 1, aIMSI.Length() + aPIN.Length() ) );
    TPtr8 keyPtr( key->Des() );
    // Check if only NETWPIN authentication is allowed, from central repository.
    TInt value( 0 );
	CRepository* repository = CRepository::NewLC( KOMAProvAuthenticationLV );
	User::LeaveIfError( repository->Get( KOMAProvAuthenticationLVFlag, value ) );
	CleanupStack::PopAndDestroy(); // repository

    // We support only security type NETWPIN
    switch( SEC() )
        {
        case KSECNETWPIN:
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KSECNETWPIN" ) );
            // Get the key to be used in HMAC calculation
            ConvertIMSIL( aIMSI, keyPtr );
            break;
            }

        case KSECUSERPIN:
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KSECUSERPIN" ) );
            if (EAuthNETWPINOnly != value)
            	{
            	if( aPIN.Length() == 0 )
                	{
                	result = KWPAuthResultPinRequired;
                	FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultPinRequired" ) );
                	}
            	keyPtr.Copy( aPIN );
            	}
            else
               	{
               	result = KWPAuthResultAuthenticationFailed;
               	FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultAuthenticationFailed" ) );
               	}
            break;
            }

        case KSECUSERNETWPIN:
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KSECUSERNETWPIN" ) );
            if (EAuthNETWPINOnly != value)
            	{
            	if( aPIN.Length() == 0 )
                	{
	                result = KWPAuthResultPinRequired;
	                FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultPinRequired" ) );
    	            }

        	    ConvertIMSIL( aIMSI, keyPtr );
	            keyPtr.Append( aPIN );
            	}
            else
            	{
				result = KWPAuthResultAuthenticationFailed;   
				FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultAuthenticationFailed" ) );         		
            	}
            break;
            }

        case KSECUSERPINMAC:
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KSECUSERPINMAC" ) );
            if (EAuthNETWPINOnly != value)
            	{
	            if( aPIN.Length() == 0 )
    	            {
        	        result = KWPAuthResultPinRequired;
        	        FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultPinRequired" ) );
            	    }
            	keyPtr.Copy( aPIN );
            	}
            else
           		{
				result = KWPAuthResultAuthenticationFailed;            	    	
				FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultAuthenticationFailed" ) );
           	    }
            break;
            }

        default:
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: default" ) );
            if (EAuthNETWPINOnly == value || EAuthNoSecurity == value )
            	{
            	result = KWPAuthResultAuthenticationFailed;
            	FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultAuthenticationFailed" ) );
            	}
            else
            	{
            	result = KWPAuthResultNoAuthentication;	
            	FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultNoAuthentication" ) );
            	}            	
            break;
            }
        }
         
    if( result == KWPAuthResultAuthenticated )
        {
        FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: KWPAuthResultAuthenticated" ) );
        CMessageDigest* digest = CSHA1::NewL();
        CleanupStack::PushL( digest );

        if( SEC() == KSECUSERPINMAC )
            {
            // key C is a concatenation of pin K and digest m
            TPtrC8 K( key->Left( key->Length()/2 ) );
            TPtrC8 m( key->Right( key->Length()/2 ) );

            // M' = HMAC-SHA(K, A)
            CHMAC* hmac = CHMAC::NewL( K, digest );
            CleanupStack::Pop( digest );
            CleanupStack::PushL( hmac );
            TPtrC8 MM( hmac->Hash( Body() ) );
    
            // Create m' (renamed to mm)
            HBufC8* mm = HBufC8::NewLC( m.Length() );
            TPtr8 ptr( mm->Des() );
            for( TInt i( 0 ); i < m.Length(); i++ )
                {
                ptr.Append( (MM[i]%10)+KZero );
                }

            // Compare the MACs and mark the message as authenticated
            if( *mm != m )
                {
                result = KWPAuthResultAuthenticationFailed;
                }
            CleanupStack::PopAndDestroy(); // mm
            }
        else
            {
            FLOG( _L( "[Provisioning] CWPPushMessage::AuthenticateL: not KWPAuthResultAuthenticated" ) );
            // Create the HMAC from body
            CHMAC* hmac = CHMAC::NewL( *key, digest );
            CleanupStack::Pop( digest );
            CleanupStack::PushL( hmac );
    
            // Compare the MACs and mark the message as authenticated
            if( headerMac->Length() == 0 
                || hmac->Hash( Body() ) != *headerMac )
                {
                result = KWPAuthResultAuthenticationFailed;
                }
            }
        CleanupStack::PopAndDestroy(); // hmac
        }
            
    CleanupStack::PopAndDestroy( 2 ); // key, headerMac

    return result;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::PackL
// -----------------------------------------------------------------------------
//
HBufC8* CWPPushMessage::PackLC( const TDesC8& aHex ) const
    {
    HBufC8* bin = HBufC8::NewLC( aHex.Length()/2 );
    TPtr8 binPtr( bin->Des() );
    for( TInt i( 0 ); i < aHex.Length()/2; i++ )
        {
        TLex8 lex( aHex.Mid( i*2, 2 ) );
        TUint8 byte( 0 );
        User::LeaveIfError( lex.Val( byte, EHex ) );
        binPtr.Append( TUint8( byte ) );
        }

    return bin;
    }

// -----------------------------------------------------------------------------
// CWPPushMessage::ConvertIMSIL
// -----------------------------------------------------------------------------
//
void CWPPushMessage::ConvertIMSIL( const TDesC& aIMSI, TPtr8& aKey ) const
    {
    TUint8 parity( TUint8((aIMSI.Length() % 2) << KParityBitNum) );

    if( aIMSI.Length() == 0 )
        {
        aKey.Append( (KPadNibble<<KNumBitsInNibble) + KFirstNibble + parity );
        return;
        }

    // First byte contains just a header and one digit
    TInt first( aIMSI[0] - KZero );
    aKey.Append( (first<<KNumBitsInNibble) | KFirstNibble | parity );

    // Use semi-octet or BCD packing of IMSI. It means that one byte contains
    // two decimal numbers, each in its own nibble.
    for( TInt i( 1 ); i < aIMSI.Length(); i += KNumDigitsInByte )
        {
        first = aIMSI[i] - KZero;
        TInt second( 0 );

        if( aIMSI.Length() == i+1 )
            {
            second = KPadNibble;
            }
        else
            {
            second = aIMSI[i+1] - KZero;
            }

        aKey.Append( (second<<KNumBitsInNibble) + first );
        }
    }



//  End of File  
