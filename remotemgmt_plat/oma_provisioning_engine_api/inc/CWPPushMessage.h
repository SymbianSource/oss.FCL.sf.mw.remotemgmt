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
* Description:  CWPPushMessage helps in saving/restoring a push message.
*
*/


#ifndef CWPPUSHMESSAGE_H
#define CWPPUSHMESSAGE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMsvStore;
class CHTTPResponse;
class RWriteStream;

// CONSTANTS

// Authentication result: Authenticated
const TInt KWPAuthResultAuthenticated = 0;

// Authentication result: PIN required, but not supplied
const TInt KWPAuthResultPinRequired = 1;

// Authentication result: Authentication failed
const TInt KWPAuthResultAuthenticationFailed = 2;

// Authentication result: No authentication contained in message
const TInt KWPAuthResultNoAuthentication = 3;

// Security parameter NETWPIN
const TUint8 KSECNETWPIN = 0x00;

// Security parameter USERPIN
const TUint8 KSECUSERPIN = 0x01;

// Security parameter USERNETWPIN
const TUint8 KSECUSERNETWPIN = 0x02;

// Security parameter USERPINMAC
const TUint8 KSECUSERPINMAC = 0x03;

// No security parameter
const TUint8 KSECNONE = 0xff;


// CLASS DECLARATION

/**
*  CWPPushMessage helps in saving/restoring a push message.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/ 
class CWPPushMessage : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPPushMessage* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPPushMessage* NewLC();

        /**
        * Destructor.
        */
        ~CWPPushMessage();

    public: // New methods

        /** 
        * Stores the message to a message store.
        * @param aStore The store.
        */
        IMPORT_C void StoreL( CMsvStore& aStore ) const;

        /** 
        * Restores the message from a message store.
        * @param aStore The store.
        */
        IMPORT_C void RestoreL( CMsvStore& aStore );

        /** 
        * Externalizes the message to a stream.
        * @param aStream The stream 
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

        /** 
        * Internalizes the message from a stream.
        * @param aStream The stream.
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /** 
        * Sets the message header and body.
        * @param aHeader The message header
        * @param aBody The message body
        */
        IMPORT_C void SetL( const TDesC8& aHeader, const TDesC8& aBody );

        /** 
        * Sets the message header and body. Ownership is transferred.
        * @param aHeader The message header
        * @param aBody The message body
        */
        IMPORT_C void Set( HBufC8* aHeader, HBufC8* aBody );

        /**
        * Sets the originator of the message. 
        * @param aOrig Originator of the message
        */
        IMPORT_C void SetOriginatorL( const TDesC8& aOrig );

        /** 
        * Returns the message header.
        * @return The message header
        */
        IMPORT_C const TDesC8& Header() const;

        /** 
        * Returns the message body.
        * @return The message body
        */
        IMPORT_C const TDesC8& Body() const;

        /**
        * Returns the originator of the message.
        * @return The originator
        */
        IMPORT_C const TDesC8& Originator() const;

        /**
        * Sets the authenticated flag.
        * @param aAuthenticated ETrue to set the message authenticated.
        */
        IMPORT_C void SetAuthenticated( TBool aAuthenticated );

        /**
        * Returns ETrue if the message is authenticated.
        * @return Whether the message is authenticated.
        */
        IMPORT_C TBool Authenticated() const;

        /**
        * Sets the saved flag.
        * @param aSaved ETrue to set the message saved.
        */
        IMPORT_C void SetSaved( TBool aSaved );

        /**
        * Returns ETrue if the message is saved.
        * @return Whether the message is saved.
        */
        IMPORT_C TBool Saved() const;

        /**
        * Returns the initiator URI of the message.
        * @return Initiator URI
        */
        IMPORT_C const TDesC8& InitiatorURI() const;

        /**
        * Returns the MAC of the message.
        * @return MAC
        */
        IMPORT_C const TDesC8& MAC() const;

        /**
        * Returns the SEC of the message.
        * @return SEC
        */
        IMPORT_C TUint SEC() const;

        /**
        * Returns the push flag of the message.
        * @return Push flag
        */
        IMPORT_C TInt PushFlag() const;

        /**
        * Parse the header.
        */
        IMPORT_C void ParseHeaderL();

        /**
        * Try to authenticate the message with HMAC.
        * @param aIMSI The SIM card number
        * @param aPIN The PIN entered by user
        * @return The result of authentication
        */
        IMPORT_C TInt AuthenticateL( const TDesC& aIMSI, const TDesC& aPIN );

    private:
        /**
        * C++ default constructor.
        */
        CWPPushMessage();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /**
        * Parse the content-type header.
        * @param aPointer Lexer pointing to content-type header.
        */
        void ParseContentType( TLex8& aPointer );

        /**
        * Parse the content-type header.
        * @param aPointer Lexer pointing to content-type header.
        */
        void ParseInitiatorURI( TLex8& aPointer );

        /**
        * Parse the content-type header.
        * @param aPointer Lexer pointing to content-type header.
        */
        void ParsePushFlag( TLex8& aPointer );

        /**
        * Retrieve a Short-integer.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The short integer
        */
        TUint GetShortInteger( TLex8& aPointer ) const;

        /**
        * Retrieve a Long-integer.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The long integer.
        */
        TInt64 GetLongInteger( TLex8& aPointer ) const;

        /**
        * Retrieve a Text-string.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The string
        */
        TPtrC8 GetTextString( TLex8& aPointer ) const;

        /**
        * Retrieve a Text-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The string
        */
        TPtrC8 GetTextValue( TLex8& aPointer ) const;

        /**
        * Retrieve a Token-text.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The text string
        */
        TPtrC8 GetTokenText( TLex8& aPointer ) const;

        /**
        * Retrieve a Q-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The integer
        */
        TUint GetQValue( TLex8& aPointer ) const;

        /**
        * Retrieve a Well-known-charset value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The character set number
        */
        TInt64 GetWellKnownCharset( TLex8& aPointer ) const;

        /**
        * Retrieve a Version-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The version
        */
        TUint GetVersionValue( TLex8& aPointer ) const;

        /**
        * Retrieve a Constrained-encoding value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The constrained encoding value
        */
        TUint GetConstrainedEncoding( TLex8& aPointer ) const;

        /**
        * Retrieve an Integer-Value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The integer value
        */
        TInt64 GetIntegerValue( TLex8& aPointer ) const;

        /**
        * Check if lexer points to an Integer-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return ETrue if lexer points to an integer
        */
        TBool IsIntegerValue( TLex8& aPointer ) const;

        /**
        * Retrieve a Field-name.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return Field name
        */
        TUint GetFieldName( TLex8& aPointer ) const;

        /**
        * Retrieve a Delta-seconds-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The seconds found
        */
        TInt64 GetDeltaSecondsValue( TLex8& aPointer ) const;

        /**
        * Retrieve a No-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        */
        void GetNoValue( TLex8& aPointer ) const;

        /**
        * Retrieve a Date-value.
        * @see Wireless Session Protocol Specification WAP-230-WSP-20010705-a
        * @param aPointer Lexer pointing to content-type header.
        * @return The time value
        */
        TTime GetDateValue( TLex8& aPointer ) const;

        /**
        * Skip to content-type.
        * @return The Content-Type header
        */
        TPtrC8 ContentTypeHeader( CHTTPResponse& aResponse ) const;

        /**
        * Skip to x-wap-initiator-uri.
        * @return The X-WAP-Initiator-URI header
        */
        TPtrC8 InitiatorURIHeader( CHTTPResponse& aResponse ) const;

        /**
        * Skip to push-flag.
        * @return The Push-Flag header
        */
        TPtrC8 PushFlagHeader( CHTTPResponse& aResponse ) const;

        /**
        * Converts a HEX ASCII strings to binary.
        * @param aHex The hex ascii version
        * @return The binary version
        */
        HBufC8* PackLC( const TDesC8& aHex ) const;

        /**
        * Create a HMAC key from the IMSI.
        * @param aIMSI The IMSI
        * @param aKey Space for HMAC key.
        */
        void ConvertIMSIL( const TDesC& aIMSI, TPtr8& aKey ) const;

    private: // Data
        /// The message body. Owns.
        HBufC8* iBody;

        /// The message header. Owns.
        CHTTPResponse* iHeader;

        /// The originator of the message. Owns.
        HBufC8* iOriginator;

        /// Whether the message is authenticated
        TBool iAuthenticated;

        /// Whether the message is saved
        TBool iSaved;

        /// The initiator URI
        TPtrC8 iInitiator;

        /// The security parameter
        TUint iSEC;

        /// The MAC value. Owns.
        TPtrC8 iMAC;

        /// The Push Flag
        TInt iPushFlag;
    };

#endif /* CWPPUSHMESSAGE_H */
