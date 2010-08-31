/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PnpUtil implementation
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32math.h>
#include <mmtsy_names.h>
#include <asymmetric.h>         // For RInteger
#include <3des.h>               // For C3DES
#include <centralrepository.h>  // link against centralrepository.lib

#include "PnpUtilImpl.h"
#include "PnpUtilLogger.h"
#include "PnpUtilPrivateCRKeys.h" // Central repository keys


// CONSTANTS

const TInt KMaxModulusLength = 300;
const TInt KMaxPublicExponentLength = 10;

// Default token validity time
const TInt KTokenValidityTime = 600;
const TInt KTokenLength = 4;
// C3DESEncryptor and C3DESDecryptor require the buffer to be more than 4 characters
// (8 characters seems to be enough)
const TInt KTokenLengthDuringStorage = 8;

const TInt KEncryptionKeyLength = 24;

// -----------------------------------------------------------------------------
// CPnpUtilImpl::ConstructL
// -----------------------------------------------------------------------------
//
void CPnpUtilImpl::ConstructL()
    {
    LOGSTRING( "Enter to CPnpUtilImpl::ConstructL()" );

    User::LeaveIfError( iServer.Connect() );
    LOGSTRING( "CPnpUtilImpl::ConstructL() 2" );
    User::LeaveIfError( iServer.LoadPhoneModule( KMmTsyModuleName ) );
    LOGSTRING( "CPnpUtilImpl::ConstructL() 3" );
    User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
    LOGSTRING( "CPnpUtilImpl::ConstructL() 4" );
    User::LeaveIfError( iPhone.Initialise() );

    LOGSTRING( "CPnpUtilImpl::ConstructL() 5" );

    iRepository = CRepository::NewL( KCRUidPnpUtil );

    TBuf8<KEncryptionKeyLength> encryptkey;
    CreateEncryptionKeyL(encryptkey);
    

#ifndef __WINS__ // calling C3DESEncryptor::NewL crashes emulator
    iEncryptor = C3DESEncryptor::NewL( encryptkey );
    iDecryptor = C3DESDecryptor::NewL( encryptkey );
#endif 

    LOGSTRING( "Exit from CPnpUtilImpl::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CPnpUtilImpl* CPnpUtilImpl::NewLC()
    {
    LOGSTRING("CPnpUtilImpl::NewLC");
    CPnpUtilImpl* self = new (ELeave) CPnpUtilImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::CPnpUtilImpl
// -----------------------------------------------------------------------------
//
CPnpUtilImpl::CPnpUtilImpl()
    {
    LOGSTRING("CPnpUtilImpl()");
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::~CPnpUtilImpl
// -----------------------------------------------------------------------------
//
EXPORT_C CPnpUtilImpl::~CPnpUtilImpl()
    {
    LOGSTRING( "~CPnpUtilImpl" );
    delete iRepository;
    iPhone.Close();
    iServer.Close();
#ifndef __WINS__
    delete iEncryptor;
    delete iDecryptor;
#endif
    LOGSTRING( "~CPnpUtilImpl - done" );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::Version( TDes& aVersion )
    {
    LOGSTRING( "CPnpUtilImpl::Version" );

    // There is now a separate dll for HDC so there is no need to
    // read the version string from a database, we may use
    // a hard-coded value instead.

    _LIT( KTempVersion, "NPnP-MSS60-1.01" );
   
    // Max length is KMaxVersionStringLength
    if( KTempVersion().Length() > aVersion.MaxLength() )
        {
        return KErrArgument;
        }
    aVersion.Copy( KTempVersion );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::CreateNewToken
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::CreateNewToken(TUint32 aTimeout, TInt& aToken)
    {
    LOGSTRING("CPnpUtilImpl::CreateNewToken");
    TInt value(0);
    // Get current time
    TTime t;
    t.HomeTime();
    TInt64 time = t.Int64();

    // Create timeout
    if( aTimeout > 9999 )
        {
        LOGSTRING("too long timeout - KErrArgument");
        return KErrArgument;
        }

    // Deadline = currenttime + (atimeout * 1000000)
    TInt64 deadline = time + TInt64((TInt)aTimeout) * TInt64(1000000);
    TBuf<21> num;    // 20 is max length for 64 bit integer (radix 10)
    num.AppendNum(deadline);
    LOGSTRING("WriteDeadline");
    LOGTEXT(num);

    // Store deadline
    TInt result = iRepository->Set( KPnPUtilDeadline, num );
    if( result != KErrNone )
        {
        LOGSTRING2("Storing deadline failed: %i", result);
        return result;
        }
    
    value = Math::Rand(time);
    value %= (9999 - 1001);
    value += 1001;

    LOGSTRING2( "New Token: %i", value );

    // KTokenLength is too small for the buffer length because C3DESEncryptor/C3DESDecryptor
    // cannot handle small buffers
    TBuf8<KTokenLengthDuringStorage> tokenBuf;
    tokenBuf.AppendNum( value );
    tokenBuf.AppendNum( 1234 );
    // Encrypt
#ifndef __WINS__
    iEncryptor->Transform( tokenBuf );
#endif

    // Save token
    result = iRepository->Set( KPnPUtilToken, tokenBuf );

    aToken = value;

    LOGSTRING("CPnpUtilImpl::CreateNewToken - done");
    return result;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::GetTokenValidityTime
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::GetTokenValidityTime()
    {
    LOGSTRING("CPnpUtilImpl::GetTokenValidityTime");
    return KTokenValidityTime;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::GetTokenValue
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::GetTokenValue(TInt& aToken)
    {
    LOGSTRING("Enter CPnpUtilImpl::GetTokenValue");

    // Check deadline
    TBuf<21> num;
    LOGSTRING("ReadDeadline");

    TInt result = iRepository->Get( KPnPUtilDeadline, num );
    if( result != KErrNone )    
        {
        return KErrCorrupt;
        }

    LOGTEXT(num);
    TLex l(num);
    TInt64 deadline(0);
    if( l.Val(deadline)!= KErrNone )
        {
        return KErrCorrupt;
        }
    // Check validity time
    TTime t;
    t.HomeTime();
    if( t > deadline )
        {
        TTimeIntervalSeconds seconds(0);
        t.SecondsFrom( deadline, seconds );
        LOGSTRING2( "Token validity expired: %is ago", seconds.Int() );
        return KErrTimedOut;
        }

    LOGSTRING("Token not expired");

    // Get token
    // KTokenLength is too small for the buffer length because C3DESEncryptor/C3DESDecryptor
    // cannot handle small buffers
    TBuf8<KTokenLengthDuringStorage> tokenBuf;
    result = iRepository->Get( KPnPUtilToken, tokenBuf );

    if( result != KErrNone )
        {
        LOGSTRING2( "Error %i when trying to read token value", result );
        return result;
        }

    LOGSTRING("Tokenbuf:");
    LOGTEXT( tokenBuf );

#ifndef __WINS__
    iDecryptor->Transform( tokenBuf );
#endif

    TInt token(0);
    TLex8 parser( tokenBuf.Left( KTokenLength ) );
    parser.Val( token );

    if( token < 1 || token > 9999)
        {
        return KErrCorrupt;
        }

    LOGSTRING("Token OK");
    LOGSTRING2("Token: %i", token);
 
    aToken = token;

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::CreateNewNonceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::CreateNewNonceL( const TUint /*aTimeOut*/, TDes8& aNonce )
    {
    LOGSTRING("CPnpUtilImpl::CreateNewNonceL");
    if( aNonce.MaxLength() < KNonceLength )
        {
        User::Leave( KErrArgument );
        }

    TBuf8<KNonceLength> buffer;

    TTime time;
    time.HomeTime();
    TInt64 seed = time.Int64();
    for( TInt i(0); i < KNonceLength; i++ )
        {
        TChar character( RandomCharacter( seed ) );
        buffer.Append( character );
        }

    LOGSTRING("New Nonce:");
    LOGTEXT( buffer );
    aNonce.Copy( buffer );

    // Encrypt
#ifndef __WINS__
    iEncryptor->Transform( buffer );
#endif

    // Save the nonce
    User::LeaveIfError( iRepository->Set( KPnPUtilNonce, buffer ) );

    LOGSTRING("CPnpUtilImpl::CreateNewNonceL - done");
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::GetNonceValidityTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::GetNonceValidityTimeL()
    {
    // Not used
    User::Leave( KErrNotSupported );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::GetNonceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::GetNonceL( TDes8& aNonce )
    {
    LOGSTRING("CPnpUtilImpl::GetNonceL");

    if( aNonce.MaxLength() < KNonceLength )
        {
        User::Leave( KErrArgument );
        }
    TBuf8<KNonceLength> buffer;
    
    User::LeaveIfError( iRepository->Get( KPnPUtilNonce, buffer) );

    // Decrypt
#ifndef __WINS__
    if( buffer.Length() > 0 )
    	{
    	LOGTEXT( buffer );
    	iDecryptor->Transform( buffer );
    	}
#endif

    LOGSTRING("Nonce:");
    LOGTEXT( buffer );
    aNonce.Copy( buffer );

    LOGSTRING("CPnpUtilImpl::GetNonceL - done");
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::RandomCharacter
// -----------------------------------------------------------------------------
//
TUint CPnpUtilImpl::RandomCharacter( TInt64& aSeed )
    {
    // Create random number value for token
    TInt value = Math::Rand( aSeed );
    if( value < 0 )
        {
        value = -value;
        }
    value %= 61; // [0,61]
    value += 48; // [48,109]
    // split the range to ranges [48,57],[65,90] and [97,122]
    // that is: the ascii codes for numbers from 0 to 9,
    // capital alphabets from A to Z and alphabets from a to z
    if( value > 57 )
        {
        value += 8; // [65,90]
        if( value > 90 )
            {
            value += 6; // [97,122]
            }
        }
    return value;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::GetKeyInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::GetKeyInfoL( TDes8& aKeyInfo )
    {
    LOGSTRING("CPnpUtilImpl::GetKeyInfoL");
    if( aKeyInfo.MaxLength() < KMaxKeyInfoLength )
        {
        User::Leave( KErrArgument );
        }
    // read keyinfo from cenrep
    TBuf<KMaxKeyInfoLength> buffer;
    User::LeaveIfError( iRepository->Get( KPnPUtilKeyInfo, buffer) );
    aKeyInfo.Copy( buffer );
    LOGSTRING("CPnpUtilImpl::GetKeyInfoL - done");
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::VerifySignatureL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPnpUtilImpl::VerifySignatureL(
        const TDesC8& aDigestValue, const TDesC8& aSignatureValue,
        const TDesC8& aData, const TDesC8& aNonce )
    {
    LOGSTRING( "CPnpUtilImpl::VerifySignatureL" );
    LOGTEXT( aDigestValue );

    HBufC8* sendersDigest = DecodeBase64LC( aDigestValue );
    LOGSTRING( "senders digest:" );
    LogAsASCIIHexL( *sendersDigest );

    HBufC8* sendersSignature = DecodeBase64LC( aSignatureValue );
    LOGSTRING( "senders signature:" );
    LogAsASCIIHexL( *sendersSignature );
    

    // Read Modulus and PublicExponent from Cenrep
    LOGSTRING( "Read Modulus and PublicExponent from Cenrep" );
    TBuf<KMaxModulusLength> bufferMod;
    TBuf<KMaxPublicExponentLength> bufferExp;
    User::LeaveIfError( iRepository->Get( KPnPUtilModulus, bufferMod) );
    User::LeaveIfError( iRepository->Get( KPnPUtilPublicExponent, bufferExp) );
    
    TBuf8<KMaxModulusLength> bufferMod8;
    TBuf8<KMaxPublicExponentLength> bufferExp8;
    bufferMod8.Copy( bufferMod );
    bufferExp8.Copy( bufferExp );

    LOGSTRING( "Pack modulus and exponent" );
    // Look for (lousy/faulty) documentation for RInteger from the 2.1 SDK
    HBufC8* modulusBuf = PackLC( bufferMod8 );
    HBufC8* exponentBuf = PackLC( bufferExp8 );

    RInteger modulus = RInteger::NewL( *modulusBuf );
    RInteger exponent = RInteger::NewL( *exponentBuf );

    CRSAPublicKey* publicKey = CRSAPublicKey::NewLC( modulus, exponent );
    CRSAPKCS1v15Verifier* rsaVerifier = CRSAPKCS1v15Verifier::NewLC( *publicKey );

    RInteger rsaSignatureInteger = RInteger::NewL( *sendersSignature );
    CRSASignature* signature = CRSASignature::NewLC( rsaSignatureInteger );

    // Verify the digest against the signature
    TBool verified = rsaVerifier->VerifyL( *sendersDigest, *signature );

    // These are not needed anymore, destroy
    CleanupStack::PopAndDestroy( signature );
    CleanupStack::PopAndDestroy( rsaVerifier );
    CleanupStack::PopAndDestroy( publicKey );
    CleanupStack::PopAndDestroy( exponentBuf );
    CleanupStack::PopAndDestroy( modulusBuf );
    CleanupStack::PopAndDestroy( sendersSignature );

    
    if( verified ) // Signature was verified
        {
        LOGSTRING( "Signature Verified" );

        // ...Yep, verify also digest
        verified = VerifyDigestL( *sendersDigest, aData, aNonce );
        if( verified )
            {
            LOGSTRING( "Digest Verified" );
            }
        else
            {
            LOGSTRING( "Digest NOT verified" );
            }
        }
    else
        {
        // ...Nope, do not check the digest, verify already failed anyways
        LOGSTRING( "Signature NOT verified" );
        }

    CleanupStack::PopAndDestroy( sendersDigest );

    LOGSTRING( "CPnpUtilImpl::VerifySignatureL - done" );
    return verified;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::VerifyDigestL
// -----------------------------------------------------------------------------
//
TBool CPnpUtilImpl::VerifyDigestL(
        const TDesC8& aSendersDigest, const TDesC8& aData, const TDesC8& aNonce )
    {
    LOGSTRING( "VerifyDigestL" );
    LOGTEXT( aData );
    LOGTEXT( aNonce );

    CSHA1* sha1 = CSHA1::NewL();
    CleanupStack::PushL( sha1 );

    HBufC8* dataToBeHashed = HBufC8::NewLC( aData.Length() + aNonce.Length() + 1 );
    TPtr8 dataToBeHashedPtr = dataToBeHashed->Des();
    _LIT8( KColon, ":" );
    dataToBeHashedPtr.Append( aNonce );
    dataToBeHashedPtr.Append( KColon );
    dataToBeHashedPtr.Append( aData );

    LOGSTRING( "nonce:data" );
    LogAsASCIIHexL( *dataToBeHashed );

    TPtrC8 hash = sha1->Final( *dataToBeHashed );
    LOGSTRING( "computed hash:" );
    LOGTEXT( hash );
    LogAsASCIIHexL( hash );

    LOGSTRING( "senders hash:" );
    LogAsASCIIHexL( aSendersDigest );
    TBool verified( EFalse );
    if( hash.Compare( aSendersDigest ) == 0 )
        {
        verified = ETrue;
        }
    CleanupStack::PopAndDestroy( dataToBeHashed );
    CleanupStack::PopAndDestroy( sha1 );
    return verified;
    }


// -----------------------------------------------------------------------------
// CPnpUtilImpl::LogAsASCIIHexL
// -----------------------------------------------------------------------------
//
void CPnpUtilImpl::LogAsASCIIHexL( const TDesC8& aDesc )
    {
    HBufC8* asciiHexBuf = HBufC8::NewLC( aDesc.Length() * 2 );
    TPtr8 asciiHexBufPtr = asciiHexBuf->Des();
    TBuf8<2> asciiHex;
    // binary to ascii (hex)
    _LIT8( KAsciiHexFormat, "%02X" );
    TUint8 val;
    for( TInt i=0; i<aDesc.Length(); i++ )
        {
        val = aDesc[i];
        asciiHex.Format( KAsciiHexFormat, val );
        asciiHexBufPtr.Append( asciiHex );
        }

#ifdef _DEBUG
    for( TInt j(0); j < asciiHexBufPtr.Length(); j += 128 )
        {
        TPtrC8 logText = asciiHexBufPtr.Mid(j); // print in 128 byte chunks
        LOGTEXT( logText );
        }
#endif

    CleanupStack::PopAndDestroy( asciiHexBuf );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::DecodeBase64LC
// -----------------------------------------------------------------------------
//
HBufC8* CPnpUtilImpl::DecodeBase64LC( const TDesC8& aBase64EncodedDesc )
    {
    TInt lenght = aBase64EncodedDesc.Length();
    HBufC8* decoded = HBufC8::NewLC( lenght );
    TPtr8 decodedPtr = decoded->Des();

    if( lenght % 4 != 0 )
        {
        User::Leave( KErrCorrupt );
        }

    for( TInt i(0); i < lenght - 1; i++ )
        {
        // The last two character may be zero characters ('=')
        if( aBase64EncodedDesc[i] == '=' )
        {
            if( i < lenght - 2 || aBase64EncodedDesc[i+1] != '=' )
                {
                User::Leave( KErrCorrupt );
                }
            return decoded;
        }
        else if( aBase64EncodedDesc[i+1] == '=' )
        {
            if( i < lenght - 2 )
                {
                User::Leave( KErrCorrupt );
                }
            return decoded;
        }

        TChar character( DecodeCharL( aBase64EncodedDesc[i] ) );
        TChar nextCharacter( DecodeCharL( aBase64EncodedDesc[i + 1] ) );

        switch( i % 4 )
            {
            case 0:
                {
                TChar shiftedChar(
                    ( character << 2 ) |                    // bits 0 to 5 of the first character
                    ( ( nextCharacter & 0x30 ) >> 4 ) );    // bits 4 to 5 of the second character
                decodedPtr.Append( shiftedChar );
                }
                break;
            case 1:
                {
                TChar shiftedChar(
                    ( character << 4 ) |                    // 0 to 3
                    ( ( nextCharacter & 0x3C ) >> 2 ) );    // 2 to 5
                decodedPtr.Append( shiftedChar );
                }
                break;
            case 2:
                {
                TChar shiftedChar(
                    ( ( character & 0x03 ) << 6 ) |            // bits 0 to 1
                    ( nextCharacter & 0x3F ) );                // bits 0 to 5
                decodedPtr.Append( shiftedChar );
                }
                break;
            default: /* case 3, skip */
                break;
            }
        }
    return decoded;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::DecodeCharL
// -----------------------------------------------------------------------------
//
TChar CPnpUtilImpl::DecodeCharL( const TChar aCharacter )
    {
    TChar decodedChar('A');

    if( aCharacter >= 'A' && aCharacter <= 'Z' )
        {
        decodedChar = aCharacter - 'A';
        }
    else if( aCharacter >= 'a' && aCharacter <= 'z' )
        {
        decodedChar = aCharacter - 'a' + 26;
        }
    else if( aCharacter >= '0' && aCharacter <= '9' )
        {
        decodedChar = aCharacter - '0' + 52;
        }
    else if( aCharacter == '+' )
        {
        decodedChar = 62;
        }
    else if( aCharacter == '/' )
        {
        decodedChar = 63;
        }
    else if( aCharacter != '=' )
        {
        User::Leave( KErrCorrupt );
        }
    return decodedChar;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::PackLC
// -----------------------------------------------------------------------------
//
HBufC8* CPnpUtilImpl::PackLC( const TDesC8& aHex ) const
    {
    HBufC8* bin = HBufC8::NewLC( aHex.Length() / 2 );
    TPtr8 binPtr( bin->Des() );
    for( TInt i(0); i < aHex.Length() / 2; i++ )
        {
        TLex8 lex( aHex.Mid( i * 2, 2 ) );
        TUint8 byte(0);
        User::LeaveIfError( lex.Val( byte, EHex ) );
        binPtr.Append( TUint8( byte ) );
        }
    return bin;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::Imsi
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::ImsiL(RMobilePhone::TMobilePhoneSubscriberId& aImsi) const
    {
    // Get IMSI
    TRequestStatus status;
    iPhone.GetSubscriberId( status, aImsi );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::FetchHomeNetworkInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::FetchHomeNetworkInfoL()
    {
    LOGSTRING( "FetchHomeNetworkInfoL");

    // Get home (sim) MCC&MNC info
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );
#ifndef __WINS__
    TRequestStatus status( KRequestPending );
    iPhone.GetHomeNetwork( status, infoPckg );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
#else
    info.iCountryCode.Copy( _L("244") );
    info.iNetworkId.Copy( _L("05") );
#endif

    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;
    FormatMncCodeL( info.iCountryCode, info.iNetworkId, formattedMnc );
    SetHomeMncL( formattedMnc );
    SetHomeMccL( info.iCountryCode );

    LOGSTRING( "FetchHomeNetworkInfoL - done");
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::FetchNetworkInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::FetchNetworkInfoL()
    {
    LOGSTRING( "FetchNetworkInfoL");

    // Get current (network) MCC&MNC info
    TRequestStatus status;
    LOGSTRING( "FetchNetworkInfoL 1");
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );
    LOGSTRING( "FetchNetworkInfoL 2");
    status = KErrNone;
    LOGSTRING( "FetchNetworkInfoL 3");

    iPhone.GetCurrentNetwork( status, infoPckg );

    LOGSTRING( "FetchNetworkInfoL 4");
    User::WaitForRequest( status );
    LOGSTRING( "FetchNetworkInfoL 5");
    User::LeaveIfError( status.Int() );
    LOGSTRING( "FetchNetworkInfoL 6");

    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;
    FormatMncCodeL( info.iCountryCode, info.iNetworkId, formattedMnc );
    LOGSTRING( "FetchNetworkInfoL 7");
    SetNetworkMncL( formattedMnc );
    LOGSTRING( "FetchNetworkInfoL 8");
    SetNetworkMccL( info.iCountryCode );

    LOGSTRING( "FetchNetworkInfoL - done");
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::HomeMccL
// -----------------------------------------------------------------------------
//
EXPORT_C const RMobilePhone::TMobilePhoneNetworkCountryCode CPnpUtilImpl::HomeMccL() const
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mcc;
    User::LeaveIfError( iRepository->Get( KPnPUtilHomeMcc, mcc ) );
    LOGSTRING2( "HomeMccL %S", &mcc );

    return mcc;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::SetHomeMccL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::SetHomeMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc )
    {
    LOGSTRING2( "SetHomeMccL %S", &aMcc );
    User::LeaveIfError( iRepository->Set( KPnPUtilHomeMcc, aMcc ) );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::HomeMncL
// -----------------------------------------------------------------------------
//
EXPORT_C const RMobilePhone::TMobilePhoneNetworkIdentity CPnpUtilImpl::HomeMncL() const
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mnc;
    User::LeaveIfError( iRepository->Get( KPnPUtilHomeMnc, mnc ) );
    LOGSTRING2( "HomeMncL %S", &mnc );
    return mnc;    
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::SetHomeMncL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::SetHomeMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc )
    {
    LOGSTRING2( "SetHomeMncL %S", &aMnc );
    User::LeaveIfError( iRepository->Set( KPnPUtilHomeMnc, aMnc ) );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::NetworkMccL
// -----------------------------------------------------------------------------
//
EXPORT_C const RMobilePhone::TMobilePhoneNetworkCountryCode CPnpUtilImpl::NetworkMccL() const
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mcc;
    User::LeaveIfError( iRepository->Get( KPnPUtilNetworkMcc, mcc ) );
    LOGSTRING2( "NetworkMccL %S", &mcc );
    return mcc;
    }

EXPORT_C void CPnpUtilImpl::SetNetworkMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc )
    {
    LOGSTRING2( "SetNetworkMccL %S", &aMcc );
    User::LeaveIfError( iRepository->Set( KPnPUtilNetworkMcc, aMcc ) );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::NetworkMncL
// -----------------------------------------------------------------------------
//
EXPORT_C const RMobilePhone::TMobilePhoneNetworkIdentity CPnpUtilImpl::NetworkMncL() const
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mnc;
    User::LeaveIfError( iRepository->Get( KPnPUtilNetworkMnc, mnc ) );
    LOGSTRING2( "NetworkMncL %S", &mnc );
    return mnc;    
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::NetworkMncL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::SetNetworkMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc )
    {
    LOGSTRING2( "SetNetworkMncL %S", &aMnc );
    User::LeaveIfError( iRepository->Set( KPnPUtilNetworkMnc, aMnc ) );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::RegisteredInHomeNetworkL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPnpUtilImpl::RegisteredInHomeNetworkL()
    {
    // Get registeration status
    TRequestStatus status;
    RMobilePhone::TMobilePhoneRegistrationStatus regstatus;
    iPhone.GetNetworkRegistrationStatus(status, regstatus);
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
    return (regstatus == RMobilePhone::ERegisteredOnHomeNetwork);
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::OperatorLongNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::OperatorLongName(RMobilePhone::TMobilePhoneNetworkLongName& aName)
    {
    // Get home (sim) MCC&MNC info
    TRequestStatus status;
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );
    status = KErrNone;
    iPhone.GetHomeNetwork( status, infoPckg );
    User::WaitForRequest( status );
    aName.Zero();
    aName.Append( info.iLongName ); 
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::StoreAccessPoint
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::StoreAccessPoint(TUint32 /*aAP*/)
    {
    // Not used
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::FetchAccessPoint
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPnpUtilImpl::FetchAccessPoint(TUint32& /*aAP*/)
    {
    // Not used
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::FormatMncCodeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPnpUtilImpl::FormatMncCodeL(
    const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc,
    const RMobilePhone::TMobilePhoneNetworkIdentity aUnformattedMnc,
    RMobilePhone::TMobilePhoneNetworkIdentity& aFormattedMnc ) const
    {
    LOGSTRING("FormatMncCodeL");
    LOGTEXT( aMcc );
    LOGTEXT( aUnformattedMnc );

    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;

    // Check that codes only contain valid characters
    TInt mncValue(0);
    TInt mccValue(0);

    TLex mncParser( aUnformattedMnc );
    TInt err = mncParser.Val( mncValue );
    if( err != KErrNone )
        {
        User::Leave( KErrCorrupt );
        }

    TLex mccParser( aMcc );
    err = mccParser.Val( mccValue );
    if( err != KErrNone )
        {
        User::Leave( KErrCorrupt );
        }

    // In N7610 the MNC given by RMobilePhone::GetHomeNetwork is
    // always three digits
    // But for example in N6630 the MNC seems to be correctly formatted

    // The initial value is the given MNC:
    formattedMnc.Copy( aUnformattedMnc );

    // Reformat only if needed
    if( aUnformattedMnc.Length() == 3 )
        {
        // Assume two digits, exceptions follow
        formattedMnc.Copy( aUnformattedMnc.Left(2) );

        if( aMcc.Compare( _L("302") ) == 0 ||
            aMcc.Compare( _L("346") ) == 0 ||
            aMcc.Compare( _L("348") ) == 0 ||
            aMcc.Compare( _L("356") ) == 0 ||
            aMcc.Compare( _L("365") ) == 0 ||
            aMcc.Compare( _L("376") ) == 0 ||
            aMcc.Compare( _L("467") ) == 0 ||
            aMcc.Compare( _L("732") ) == 0 )
            {
            // MNC should be three digits
            formattedMnc.Copy( aUnformattedMnc );
            }
        else if( aMcc.Compare( _L("310") ) == 0 )
            {
            // MNC is always three digits in this case
            // For example: 000, 011, 110, 020, 200, ...
            formattedMnc.Copy( aUnformattedMnc );
            }
        else if( aMcc.Compare( _L("311") ) == 0 )
            {
            // 3 digit (USA) some exceptions

            // See previous case for comments
            formattedMnc.Copy( aUnformattedMnc );
            }

        else if( aMcc.Compare( _L("338") ) == 0 )
            {
            // 2 digit, but 3 digit if 180 (JAM)
            if( aUnformattedMnc.Compare( _L("180") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("342") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 51
            if( mncValue >= 510 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("344") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 31
            if( mncValue >= 310 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("352") ) == 0 )
            {
            // 2 digit, but 3 digit if smaller than 29
            if( mncValue < 300 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("358") ) == 0 )
            {
            // 2 digit, but 3 digit if smaller than 20
            if( mncValue < 300 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("360") ) == 0 )
            {
            // 2 digit, but 3 digit if 110
            if( aUnformattedMnc.Compare( _L("110") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc  );
                }
            }
        else if( aMcc.Compare( _L("362") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 92
            if( mncValue >= 920 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("366") ) == 0 )
            {
            // 2 digit, but 3 digit if 110
            if( aUnformattedMnc.Compare( _L("110") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("722") ) == 0 )
            {
            // 2 digit, but 3 digit if 310
            if( aUnformattedMnc.Compare( _L("310") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        }
    aFormattedMnc.Copy( formattedMnc );
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::RESERVED_FUNC
// -----------------------------------------------------------------------------
//
void CPnpUtilImpl::RESERVED_FUNC()
    {
    LOGSTRING("RESERVED_FUNC")
    }

// -----------------------------------------------------------------------------
// CPnpUtilImpl::CreateEncryptionKeyL
// -----------------------------------------------------------------------------
//
void CPnpUtilImpl::CreateEncryptionKeyL(TDes8& aEncryptionKey)
   {
   TBuf8<KEncryptionKeyLength> keystorage;
   User::LeaveIfError( iRepository->Get( KPnPUtilsEncryptionKey, keystorage) );
    
   if(keystorage.Compare(KNullDesC8()))
   {
     LOGSTRING("CPnpUtilImpl::CreateEncryptionKey- Compare visited");
     aEncryptionKey.Copy(keystorage);
     LOGTEXT( aEncryptionKey);
     return;
   }


   LOGSTRING("CPnpUtilImpl::CreateEncryptionKey");
    if( aEncryptionKey.MaxLength() < KEncryptionKeyLength)
        {
        User::Leave( KErrArgument );
        }

    TBuf8<KEncryptionKeyLength> buffer;

    TTime time;
    time.HomeTime();
    TInt64 seed = time.Int64();
    for( TInt i(0); i < KEncryptionKeyLength ; i++ )
        {
        TChar character( RandomCharacter( seed ) );
        buffer.Append( character );
        }

    LOGSTRING("New Encryption Key:");
    LOGTEXT( buffer );
    aEncryptionKey.Copy( buffer );

 
    // Save the nonce
    User::LeaveIfError( iRepository->Set( KPnPUtilsEncryptionKey, buffer ) );

    LOGSTRING("CPnpUtilImpl::CreateEncryptionKey- done");

   }

//  End of File  
