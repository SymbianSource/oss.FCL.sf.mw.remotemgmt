/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES

#include "TcTrustedSession.h"
#include "TcFileScan.h"
#include "TerminalControlClientServer.h"
#include <DMCert.h>
#include <SettingEnforcementInfo.h>
#include <PolicyEngineXACML.h>
#include "debug.h"
#include <featmgr.h>
// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS


// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTcTrustedSession::~CTcTrustedSession
// -----------------------------------------------------------------------------
CTcTrustedSession::~CTcTrustedSession()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::~CTcTrustedSession");

    if( iFileScan != 0 )
        {
        delete iFileScan;
        }

    delete iProcessList;
    delete iProcessData;

	iPR.Close();
	iPE.Close();
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::ConstructL()
// -----------------------------------------------------------------------------
void CTcTrustedSession::ConstructL(TSecureId &aSID, TCertInfo &aCert)
    {
	RDEBUG("TerminalControl: CTcTrustedSession::ConstructL 1");

    DoConstructL();

    SetCertificate( aCert );

    iSecureId        = aSID;
    UpdateAccessLevelL();
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::ConstructL()
// -----------------------------------------------------------------------------
void CTcTrustedSession::ConstructL(TSecureId &aSID)
    {
	RDEBUG("TerminalControl: CTcTrustedSession::ConstructL 2");

    DoConstructL();

    iHasCertificate = EFalse;
    iSecureId        = aSID;
    UpdateAccessLevelL();
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::DoConstructL()
// -----------------------------------------------------------------------------
void CTcTrustedSession::DoConstructL()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::DoConstructL");

    User::LeaveIfError( iPE.Connect() );
    User::LeaveIfError( iPR.Open( iPE ) );

    iAccessLevel     = CTcTrustedSession::EAccessLevelNone;
	iProcessList     = 0;
	iProcessData     = 0;
	iFileScan        = 0;
	iRebootRequested = EFalse;
	iWipeRequested   = EFalse;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::NewL()
// -----------------------------------------------------------------------------
CTcTrustedSession * CTcTrustedSession::NewL(TSecureId &aSID, TCertInfo &aCert)
{
	RDEBUG("TerminalControl: CTcTrustedSession::NewL");

	CTcTrustedSession * self = new(ELeave) CTcTrustedSession();
	CleanupStack::PushL( self );

	self->ConstructL( aSID, aCert );

	CleanupStack::Pop( self );
	return self;
}

// -----------------------------------------------------------------------------
// CTcTrustedSession::NewL()
// -----------------------------------------------------------------------------
CTcTrustedSession * CTcTrustedSession::NewL(TSecureId &aSID)
{
	RDEBUG("TerminalControl: CTcTrustedSession::NewL");

	CTcTrustedSession * self = new(ELeave) CTcTrustedSession();
	CleanupStack::PushL( self );

	self->ConstructL( aSID );

	CleanupStack::Pop( self );
	return self;
}

// -----------------------------------------------------------------------------
// CTcTrustedSession::SetAccessLevel()
// -----------------------------------------------------------------------------
void CTcTrustedSession::SetAccessLevel( TInt aAccessLevel )
    {
	RDEBUG_2("TerminalControl: CTcTrustedSession::SetAccessLevel: %d", (TInt)aAccessLevel);

    iAccessLevel = aAccessLevel;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::Certificate()
// -----------------------------------------------------------------------------
TInt CTcTrustedSession::Certificate( TCertInfo& aCertInfo )
    {
	RDEBUG("TerminalControl: CTcTrustedSession::Certificate");

    if( iHasCertificate )
        {
        TPckg<TCertInfo> certp1( aCertInfo );
        TPckg<TCertInfo> certp2( iCertificate );
        certp1.Copy( certp2 );
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::SetCertificate()
// -----------------------------------------------------------------------------
void CTcTrustedSession::SetCertificate( TCertInfo& aCertInfo )
    {
	RDEBUG("TerminalControl: CTcTrustedSession::SetCertificate");

    TPckg<TCertInfo> certp1( aCertInfo );
    TPckg<TCertInfo> certp2( iCertificate );
    certp2.Copy( certp1 );

    iHasCertificate = ETrue;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::SecureId()
// -----------------------------------------------------------------------------
TSecureId CTcTrustedSession::SecureId( )
    {
	RDEBUG("TerminalControl: CTcTrustedSession::SecureId");

    return iSecureId;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::AccessLevel()
// -----------------------------------------------------------------------------
TInt CTcTrustedSession::AccessLevel()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::AccessLevel");

    return iAccessLevel;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::FileScanL()
// -----------------------------------------------------------------------------
TInt CTcTrustedSession::FileScanL(const TDesC8 &aFileName, TBool aRecursive)
    {
	RDEBUG("TerminalControl: CTcTrustedSession::FileScanL");

    if( iFileScan == 0 )
        {
        iFileScan = CTcFileScan::NewL(); //new (ELeave) CTcFileScan();
        }
        
    iFileScan->FileScanL( aFileName, aRecursive );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::FileScanResultsL()
// -----------------------------------------------------------------------------
HBufC8* CTcTrustedSession::FileScanResultsL()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::FileScanResultsL");

    if( iFileScan == 0 )
        {
        return 0;
        }
        
    return iFileScan->GetCopyOfResultsL();
    }

// -----------------------------------------------------------------------------
// CTcTrustedSession::UpdateAccessLevelL()
// -----------------------------------------------------------------------------
void CTcTrustedSession::UpdateAccessLevelL()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::UpdateAccessLevelL");

	TInt aleave = KErrNone;
	TInt aerr   = KErrNone;  

	TInt accessLevel = CTcTrustedSession::EAccessLevelNone; 
	SetAccessLevel( accessLevel );
	//
	// Check third party application SID
	//		
	TRAP( aleave, CheckTerminalControl3rdPartySecureIDL() );
	if( KErrNone == aleave )
		{
		RDEBUG("TerminalControl: CTcTrustedSession::UpdateAccessLevelL granting application access rights.");
		accessLevel |= CTcTrustedSession::EAccessLevelApplication;
		}
		
	if( iHasCertificate )
	{
		aleave = KErrNone;
		aerr = KErrNone;

		//
		// Check third party application SID
		//
		TRAP( aleave, aerr = CheckTerminalControlPolicyL() );
		
		if( (KErrNone == aleave) && (KErrNone == aerr ))
			{
			RDEBUG("TerminalControl: CTcTrustedSession::UpdateAccessLevelL granting adapter access rights.");
			accessLevel |= CTcTrustedSession::EAccessLevelDMAdapter;
			}
	}

    SetAccessLevel( accessLevel );
    }

// -------------------------------------------------------------------------------------
// CTcTrustedSession::CheckTerminalControlPolicyL
// -------------------------------------------------------------------------------------   
TInt CTcTrustedSession::CheckTerminalControlPolicyL()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::CheckTerminalControlPolicyL");

    if( ! iHasCertificate )
        {
    	RDEBUG("TerminalControl: CTcTrustedSession::CheckTerminalControlPolicyL failed. There is no certificate.");
        return KErrAccessDenied;
        }

    /*
    If terminal control is not enforced (non-exclusive access), should we allow all connections?
    CSettingEnforcementInfo* enf = new (ELeave) CSettingEnforcementInfo();
    TBool enfed;
    enf->EnforcementActive( ETerminalSecurity, enfed ); //  change enf type when avail.
    delete enf;

    if( !enfed )
        {
        return KErrNone;
        }
    else
    */
        {
        //
        // Note: If certificate is not found, we return failure
        // Currently, main terminal control features are available through secure connection
        //
        TCertInfo ci;
        
        RDMCert dmcert;
        TInt ret = dmcert.Get( ci );

        if( KErrNone != ret )
            {
        	RDEBUG_2("**** TerminalControl: CTcTrustedSession::CheckTerminalControlPolicyL DMCert.Get failed with %d.", ret);
            return ret;
            }

        if( ! CertificatesAreEqual( iCertificate, ci ) )        
            {
    		RDEBUG("**** TerminalControl: CTcTrustedSession::CheckTerminalControlPolicyL ERROR Certificate mismatch!" );
            User::Leave( KErrArgument );
            }

        // Policy Engine Request
        TRequestContext context;
        TResponse response;
        context.AddSubjectAttributeL(
            PolicyEngineXACML::KTrustedSubject, ci
            );
        context.AddResourceAttributeL(
            PolicyEngineXACML::KResourceId,
            PolicyEngineXACML::KTerminalSecurityManagement,
            PolicyEngineXACML::KStringDataType
            );
        User::LeaveIfError( iPR.MakeRequest( context, response ) );
        TResponseValue resp = response.GetResponseValue();
		RDEBUG_2("TerminalControl: CTcTrustedSession::CheckTerminalControlPolicyL Policy check returned %d", (TInt)resp);
        switch( resp )
            {
            case EResponseDeny:
    	    case EResponseIndeterminate:
    	    case EResponseNotApplicable:
    	    default:
        	    return KErrAccessDenied;
    	    case EResponsePermit:
            	return KErrNone;

    	    }
        }
    }

// -------------------------------------------------------------------------------------
// CTcTrustedSession::CheckTerminalControl3rdPartySecureIDL
// -------------------------------------------------------------------------------------   
TInt CTcTrustedSession::CheckTerminalControl3rdPartySecureIDL()
    {
	RDEBUG("TerminalControl: CTcTrustedSession::CheckTerminalControl3rdPartySecureIDL");
    
    TRequestContext context;
    TResponse response;
    context.AddSubjectAttributeL(
        PolicyEngineXACML::KSubjectId, iSecureId
        );
    context.AddResourceAttributeL(
        PolicyEngineXACML::KResourceId,
        PolicyEngineXACML::KThirdPartySecureIds,
        PolicyEngineXACML::KStringDataType
        );
    User::LeaveIfError( iPR.MakeRequest( context, response ) );
    TResponseValue resp = response.GetResponseValue();
	RDEBUG_2("TerminalControl: CTcTrustedSession::CheckTerminalControl3rdPartySecureIDL Policy check returned %d", (TInt)resp);
    switch( resp )
        {
        case EResponsePermit:
        	return KErrNone;
        case EResponseDeny:
	    	case EResponseIndeterminate:
	    	case EResponseNotApplicable:
	    	default:
	    		User::Leave( KErrAccessDenied );
	    }
	    
	  return KErrAccessDenied;
    }

// -------------------------------------------------------------------------------------
// CTcTrustedSession::CertificatesAreEqual
// -------------------------------------------------------------------------------------   
TBool CTcTrustedSession::CertificatesAreEqual( const TCertInfo &aCert1, const TCertInfo &aCert2 )
    {
	RDEBUG("TerminalControl: CTcTrustedSession::CertificatesAreEqual");

    if( (aCert1.iFingerprint.Compare( aCert2.iFingerprint ) != 0) ||
        (aCert1.iSerialNo.Compare( aCert2.iSerialNo ) != 0) ||
        (aCert1.iPublicKey.Compare( aCert2.iPublicKey ) != 0) ||            
        (aCert1.iVersionNo    != aCert2.iVersionNo ) ||
        (aCert1.iPkAlg        != aCert2.iPkAlg ) ||
        (aCert1.iStartValDate != aCert2.iStartValDate ) ||
        (aCert1.iEndValDate   != aCert2.iEndValDate ) ||
        (aCert1.iDigAlg       != aCert2.iDigAlg ) ||
        (aCert1.iDNSName.Compare( aCert2.iDNSName ) != 0) ||

        (aCert1.iSubjectDNInfo.iCountry.Compare( aCert2.iSubjectDNInfo.iCountry ) != 0 ) ||
        (aCert1.iSubjectDNInfo.iOrganizationUnit.Compare( aCert2.iSubjectDNInfo.iOrganizationUnit ) != 0 ) ||
        (aCert1.iSubjectDNInfo.iOrganization.Compare( aCert2.iSubjectDNInfo.iOrganization ) != 0 ) ||
        (aCert1.iSubjectDNInfo.iCommonName.Compare( aCert2.iSubjectDNInfo.iCommonName ) != 0 ) ||
        (aCert1.iSubjectDNInfo.iLocality.Compare( aCert2.iSubjectDNInfo.iLocality ) != 0 ) ||

        (aCert1.iIssuerDNInfo.iCountry.Compare( aCert2.iIssuerDNInfo.iCountry ) != 0 ) ||
        (aCert1.iIssuerDNInfo.iOrganizationUnit.Compare( aCert2.iIssuerDNInfo.iOrganizationUnit ) != 0 ) ||
        (aCert1.iIssuerDNInfo.iOrganization.Compare( aCert2.iIssuerDNInfo.iOrganization ) != 0 ) ||
        (aCert1.iIssuerDNInfo.iCommonName.Compare( aCert2.iIssuerDNInfo.iCommonName ) != 0 ) ||
        (aCert1.iIssuerDNInfo.iLocality.Compare( aCert2.iIssuerDNInfo.iLocality ) != 0 )
        )
        {
        RDEBUG("**** TerminalControl: CTcTrustedSession::CertificatesAreEqual Certificates do NOT match");
        return EFalse;            
        }

    RDEBUG("TerminalControl: CTcTrustedSession::CertificatesAreEqual Certificates do match");
    return ETrue;
    }

// ----------------------------------------------------------------------------------------
// End of file
