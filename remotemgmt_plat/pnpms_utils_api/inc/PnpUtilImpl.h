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
* Description:  PnpUtil header file
*
*/



#ifndef PNP_UTIL_IMPL_H
#define PNP_UTIL_IMPL_H


#include "PnpToPaosInterface.h"

class CRepository;


#ifndef _WINS
    class C3DESEncryptor;
    class C3DESDecryptor;
#endif

class CPnpUtilImpl :public CBase, public MPnpToPaosInterface
    {
    public:
        void RESERVED_FUNC();

        IMPORT_C static CPnpUtilImpl* NewLC();

        IMPORT_C ~CPnpUtilImpl();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt Version(TDes& aVersion);
    
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt CreateNewToken(TUint32 aTimeout, TInt& aToken);
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt GetTokenValidityTime();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt GetTokenValue(TInt& aToken);

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void CreateNewNonceL( const TUint aTimeOut, TDes8& aNonce );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt GetNonceValidityTimeL();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void GetNonceL( TDes8& aNonce );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void GetKeyInfoL( TDes8& aKeyInfo );

        /**
        * Commented in base class CPnPUtil
        */
        //TBool VerifySignatureL( const TDesC8& aDigestValue, const TDesC8& aSignatureValue );
        IMPORT_C TBool VerifySignatureL(
            const TDesC8& aDigestValue, const TDesC8& aSignatureValue,
            const TDesC8& aData, const TDesC8& aNonce );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void ImsiL(RMobilePhone::TMobilePhoneSubscriberId& aImsi) const;
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void FetchHomeNetworkInfoL();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void FetchNetworkInfoL();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C const RMobilePhone::TMobilePhoneNetworkCountryCode HomeMccL() const;
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void SetHomeMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C const RMobilePhone::TMobilePhoneNetworkIdentity HomeMncL() const;
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void SetHomeMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C const RMobilePhone::TMobilePhoneNetworkCountryCode NetworkMccL() const;
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void SetNetworkMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C const RMobilePhone::TMobilePhoneNetworkIdentity NetworkMncL() const;
        
        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void SetNetworkMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc );

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TBool RegisteredInHomeNetworkL();

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt OperatorLongName(RMobilePhone::TMobilePhoneNetworkLongName& aName);

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt StoreAccessPoint(TUint32 aAP);

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C TInt FetchAccessPoint(TUint32& aAP);

        /**
        * Commented in base class CPnPUtil
        */
        IMPORT_C void FormatMncCodeL(
            const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc,
            const RMobilePhone::TMobilePhoneNetworkIdentity aUnformattedMnc,
            RMobilePhone::TMobilePhoneNetworkIdentity& aFormattedMnc ) const;
        
    private:  // Data
        CPnpUtilImpl();

        void ConstructL();

        /**
        * Verifies the digest that the sender gave. The digest (hash value) is calculated
        * as follows:
        *    H( nonce:data ), where H (hash) is the sha1 operation
        * @param aSendersDigest The digest that the sender gave. In binary format
        * (decoded from base64 format).
        * @param aData The data
        * @param aNonce The original randomly created nonce.
        */
        TBool VerifyDigestL( const TDesC8& aSendersDigest, const TDesC8& aData, const TDesC8& aNonce );

        /**
        * @return a random integer in range [48,57], [65,90] or [97,122]
        * (ascii code for alphanumeric character)
        */
        TUint RandomCharacter( TInt64& aSeed );


        /**
        * Decodes given base64-encoded data. Leaves with KErrCorrupted if
        * The data is not base64-encoded.
        * @param aEncodedDesc The base64 encoded data.
        */
        HBufC8* DecodeBase64LC( const TDesC8& aEncodedDesc );

        /**
        * A helper function for base64 encoding
        * @param aCharacter
        */
        TChar DecodeCharL( const TChar aCharacter );

        /**
        * Writes the descriptor to log as hexadecimal character representation.
        */
        void LogAsASCIIHexL( const TDesC8& aDesc );

        /**
        * 
        */
        HBufC8* PackLC( const TDesC8& aHex ) const;
        
        void CreateEncryptionKeyL(TDes8& aEncryptionKey);

        CRepository* iRepository;

        // IMSI.
        //RMobilePhone::TMobilePhoneSubscriberId iIMSI;
        TBuf<15> iIMSI;
        // Home country code
        RMobilePhone::TMobilePhoneNetworkCountryCode iHomeMCC;
        // Home network code
        RMobilePhone::TMobilePhoneNetworkIdentity iHomeMNC;
        // Network country code
        RMobilePhone::TMobilePhoneNetworkCountryCode iNetworkMCC;
        // Network network code
        RMobilePhone::TMobilePhoneNetworkIdentity iNetworkMNC;
        // True if registered in home network
        TBool iRegisteredInHomeNetwork;
        RTelServer iServer;
        RMobilePhone iPhone;
#ifndef __WINS__ // encryptor is not used in emulator
        C3DESEncryptor* iEncryptor;
        C3DESDecryptor* iDecryptor;
#endif
    };

#endif // PNP_UTIL_IMPL_H
