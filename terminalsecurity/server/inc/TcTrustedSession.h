/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef _TRUSTED_SESSION_HEADER_
#define _TRUSTED_SESSION_HEADER_

// INCLUDES

#include <e32base.h>
#include <ssl.h>
#include <PolicyEngineClient.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CTcFileScan;

// CLASS DECLARATION

class CTcTrustedSession : public CObject
{
public:
    virtual ~CTcTrustedSession();

    enum TTcAccessLevel
        {
        EAccessLevelNone        = 0,
        EAccessLevelApplication = 1,
        EAccessLevelDMAdapter   = 2
        };

public:
	static CTcTrustedSession*   NewL            ( TSecureId &aSID, TCertInfo &aCert );
	static CTcTrustedSession*   NewL            ( TSecureId &aSID );
	void                        SetAccessLevel  ( TInt aAccessLevel );

    TInt                        Certificate     ( TCertInfo& aCertInfo );
    TSecureId                   SecureId        ( );
	TInt                        AccessLevel     ( );

	TInt                        FileScanL           ( const TDesC8 &aFileName,
	                                                  TBool aRecursive );
	HBufC8*                     FileScanResultsL    ( );

private:
    void                        SetCertificate      ( TCertInfo& aCertInfo );
    void                        ConstructL          ( TSecureId &aSID, TCertInfo &aCert );
    void                        ConstructL          ( TSecureId &aSID );
    void                        DoConstructL        ( );
    void                        UpdateAccessLevelL  ( );
    TInt                        CheckTerminalControlPolicyL ( );
    TInt                        CheckTerminalControl3rdPartySecureIDL ( );
    TBool                       CertificatesAreEqual( const TCertInfo &aCert1, const TCertInfo &aCert2 );
    
private:
    RPolicyEngine   iPE;
    RPolicyRequest  iPR;

    TBool           iHasCertificate;
	TCertInfo       iCertificate;
	TSecureId       iSecureId;
	TInt            iAccessLevel;
    CTcFileScan     *iFileScan;

public:
	CBufBase        *iProcessList;
	CBufBase        *iProcessData;	
    TBool           iRebootRequested;
    TBool           iWipeRequested;
};

#endif //_TRUSTED_SESSION_HEADER_
