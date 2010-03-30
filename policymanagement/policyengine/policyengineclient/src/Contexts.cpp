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
* Description: Implementation of policymanagement components
*
*/


#include "RequestContext.h"
#include "ManagementContext.h"
#include "SettingEnforcementInfo.h"
#include "EnforcementConstants.h"

#include <PolicyEngineXACML.h>
#include <centralrepository.h>
#include <delimitedpath8.h>
#include <DMCert.h>
#include <PolicyEngineClient.h>
_LIT( KSecureIdString, "SECUREID");


// ----------------------------------------------------------------------------------------
// TManagementResponse 
// ----------------------------------------------------------------------------------------


EXPORT_C TParserResponse::TParserResponse()
{
}

	
EXPORT_C TParserResponse::TParserResponse( TDes8& aReturnMessage)
{
	iReturnMessage = aReturnMessage;
}
	
EXPORT_C void TParserResponse::Zero()
{
	
}

EXPORT_C const TDesC8& TParserResponse::GetReturnMessage()
{
	return iReturnMessage;
}


// ----------------------------------------------------------------------------------------
// TRequestContext 
// ----------------------------------------------------------------------------------------

EXPORT_C TRequestContext::TRequestContext()
	: iRequestDescription(NULL)
{	
}

EXPORT_C TRequestContext::~TRequestContext()
{
	delete iRequestDescription;
}

EXPORT_C void TRequestContext::AddSubjectAttributeL( const TDesC8& aAttributeId, const TUid& aSecureId)
{
	TBuf8<20> secureIdString;
	
	//create UID string
	secureIdString.Append( KSecureIdString);
	secureIdString.Append( aSecureId.Name());
	secureIdString.UpperCase();
	
	//and UID string to request.....
	AddSubjectAttributeL( aAttributeId, secureIdString, PolicyEngineXACML::KStringDataType);
}



EXPORT_C void TRequestContext::AddSubjectAttributeL( const TDesC8& aAttributeId, const TCertInfo& aCertInfo)
{
	TInt certLength = aCertInfo.iIssuerDNInfo.iCountry.Length();
	certLength += aCertInfo.iIssuerDNInfo.iOrganizationUnit.Length();
	certLength += aCertInfo.iIssuerDNInfo.iOrganization.Length();
	certLength += aCertInfo.iIssuerDNInfo.iCommonName.Length();
	certLength += aCertInfo.iSerialNo.Length() * 2;
	certLength += aCertInfo.iFingerprint.Length() * 2;
	
	HBufC8 *casn = HBufC8::NewLC( certLength + 5);
	TPtr8 ptr = casn->Des();
	const TChar KCASNDelimeter = '-';

	
	if ( aCertInfo.iIssuerDNInfo.iCommonName.Length())
	{
		ptr.Append( aCertInfo.iIssuerDNInfo.iCommonName);
		ptr.Append( KCASNDelimeter);
	}
	
	if ( aCertInfo.iIssuerDNInfo.iOrganization.Length())
	{
		ptr.Append( aCertInfo.iIssuerDNInfo.iOrganization);
		ptr.Append( KCASNDelimeter);
	}
	
	if ( aCertInfo.iIssuerDNInfo.iOrganizationUnit.Length())
	{
		ptr.Append( aCertInfo.iIssuerDNInfo.iOrganizationUnit);
		ptr.Append( KCASNDelimeter);
	}
	
	if ( aCertInfo.iIssuerDNInfo.iCountry.Length())
	{
		ptr.Append( aCertInfo.iIssuerDNInfo.iCountry);
		ptr.Append( KCASNDelimeter);
	}
	
	if ( aCertInfo.iSerialNo.Length())
	{
		//convert fingerprint to plain text
		for ( TInt i(0); i < aCertInfo.iSerialNo.Length(); i++)
		{
			ptr.AppendNumFixedWidth( aCertInfo.iSerialNo[i], EHex, 2);
		}	
		ptr.Append( KCASNDelimeter);
	}

	if ( aCertInfo.iFingerprint.Length())
	{
		//convert fingerprint to plain text
		for ( TInt i(0); i < aCertInfo.iFingerprint.Length(); i++)
		{
			ptr.AppendNumFixedWidth( aCertInfo.iFingerprint[i], EHex, 2);
		}	
	}	

	AddSubjectAttributeL( aAttributeId, *casn, PolicyEngineXACML::KStringDataType);
	
	CleanupStack::PopAndDestroy( casn);
}


EXPORT_C void TRequestContext::AddSubjectAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{	
	TInt size = aAttributeId.Length() + aAttributeValue.Length() + aDataType.Length();

	ReAllocL( size + 5);
	
	TPtr8 ptr = iRequestDescription->Des();
	
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( 'S' );
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeId);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeValue);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aDataType);
}

EXPORT_C void TRequestContext::AddActionAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	TInt size = aAttributeId.Length() + aAttributeValue.Length() + aDataType.Length();

	ReAllocL( size + 5);
	
	TPtr8 ptr = iRequestDescription->Des();

	ptr.Append( KMessageDelimiterChar );
	ptr.Append( 'A' );
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeId);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeValue);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aDataType);
}

EXPORT_C void TRequestContext::AddResourceAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	TInt size = aAttributeId.Length() + aAttributeValue.Length() + aDataType.Length();

	ReAllocL( size + 5);
	
	TPtr8 ptr = iRequestDescription->Des();

	ptr.Append( KMessageDelimiterChar );
	ptr.Append( 'R' );
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeId);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeValue);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aDataType);
}

EXPORT_C void TRequestContext::AddEnvironmentAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	TInt size = aAttributeId.Length() + aAttributeValue.Length() + aDataType.Length();

	ReAllocL( size + 5);
	
	TPtr8 ptr = iRequestDescription->Des();

	ptr.Append( KMessageDelimiterChar );
	ptr.Append( 'E' );
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeId);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aAttributeValue);
	ptr.Append( KMessageDelimiterChar );
	ptr.Append( aDataType);
}

void TRequestContext::ReAllocL( TInt aAdditionalLength)
{
	if ( !iRequestDescription )
	{
		iRequestDescription = HBufC8::NewL( aAdditionalLength);
	}
	else
	{
		iRequestDescription = iRequestDescription->ReAllocL( iRequestDescription->Length() + aAdditionalLength);
	}
}

const TDesC8 & TRequestContext::RequestDescription()
{
	return *iRequestDescription;
}


// ----------------------------------------------------------------------------------------
// TResponse 
// ----------------------------------------------------------------------------------------


EXPORT_C void TResponse::SetResponseValue(TResponseValue aValue)
{
	iValue = aValue;
}

EXPORT_C TResponseValue TResponse::GetResponseValue()
{
	return iValue;
}

// ----------------------------------------------------------------------------------------
// TElementInfo 
// ----------------------------------------------------------------------------------------


EXPORT_C TElementInfo::TElementInfo( const TDesC8& aElementId )
	: iElementId ( aElementId), iDescription(0), iXACMLContent(0)
{
}

EXPORT_C TElementInfo::~TElementInfo()
{
	iChildElements.Close();
	delete iDescription;
	delete iXACMLContent;
}
		
EXPORT_C const TDesC8& TElementInfo::GetElementId() const
{
	return iElementId;
}
		
	
EXPORT_C const TDesC8& TElementInfo::GetDescription() const
{
	if ( iDescription )
	{
		return *iDescription;
	}
	
	return KNullDesC8;
}
		
		
EXPORT_C const RElementIdArray& TElementInfo::GetChildElementArray() const
{
	return iChildElements;	
}



EXPORT_C const TDesC8& TElementInfo::GetXACML() const
{
	if ( iXACMLContent )
	{
		return *iXACMLContent;
	}
	
	return KNullDesC8;	
}


EXPORT_C void RElementIdArray::Close()
{
	for ( TInt i = 0; i < Count(); i++)
	{
		delete operator[](i);
	}

	RArray<HBufC8*>::Close();
}

void RElementIdArray::SetListL( const TDesC8& aChilds)
{
	TInt index = 0;
	TPtrC8 ptr = aChilds;	
	
	while ( 0 <= ( index = ptr.Locate( KMessageDelimiterChar)))
	{
		AppendL( ptr.Left( index).AllocL());
		ptr.Set( ptr.Mid(index + 1));
	}	
}


// ----------------------------------------------------------------------------------------
// CSettingEnforcementInfo 
// ----------------------------------------------------------------------------------------

CSettingEnforcementInfo::CSettingEnforcementInfo()
{
}

EXPORT_C CSettingEnforcementInfo::~CSettingEnforcementInfo()
{
	delete iRepository;	
}

void CSettingEnforcementInfo::ConstructL()
{
	const TUid TPolicyEngineRepositoryID = { 0x10207815 };
	iRepository = CRepository::NewL( TPolicyEngineRepositoryID );
}
		
EXPORT_C CSettingEnforcementInfo* CSettingEnforcementInfo::NewL()
{
	CSettingEnforcementInfo * self = new (ELeave) CSettingEnforcementInfo();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	
	return self;
}
		
EXPORT_C TInt CSettingEnforcementInfo::EnforcementActive( KSettingEnforcements aEnforcementType, TBool& aEnforcementActive)
{
	const TUint32 TSettingEnforcementFlags  = 0x00000100;
	TUint32 id = TSettingEnforcementFlags + aEnforcementType;

	TInt err = iRepository->Get( id, aEnforcementActive);	
	
	if ( err == KErrNotFound )
	{
		err = KErrNone;
		aEnforcementActive = EFalse;
	}

	return err;
}
EXPORT_C void CSettingEnforcementInfo::EnforcementActiveL( const TDesC8 & aUri, TInt& aEnforcementActive)
    {

    KSettingEnforcements enforcementType;
    TDelimitedPathParser8 pathParser;
    pathParser.Parse(aUri);
    TPtrC8 segmentName;
    TPtrC8 resourceType;
    TBool enforcementActive;
aEnforcementActive = EResponseNotApplicable;
    User::LeaveIfError( pathParser.Peek(segmentName));


        if((0 == segmentName.Compare(KSyncML12URI))||(0 == segmentName.Compare(KSyncMLURI)))
            {
            enforcementType = ESyncMLEnforcement;
            resourceType.Set(PolicyEngineXACML::KSyncMLEnforcement());
            }
        else if (0 == segmentName.Compare(KDataSyncURI))
            {
            enforcementType = EDataSyncEnforcement;
            resourceType.Set(PolicyEngineXACML::KDataSyncEnforcement);
            }
        else if (0 == segmentName.Compare(KEmailURI))
            {
            enforcementType = EEMailEnforcement;
            resourceType.Set(PolicyEngineXACML::KEMailEnforcement);
            }
        else if (0 == segmentName.Compare(KAPURI))
            {
            if (aUri.Find(KWLANURI)!= KErrNotFound)
                {
                enforcementType = EWLANEnforcement;
                resourceType.Set(PolicyEngineXACML::KWLANEnforcement);
                }
            else
                {
                enforcementType = EAPEnforcement;
                resourceType.Set(PolicyEngineXACML::KAccessPointEnforcement);
                }
            }
        else if (0 == segmentName.Compare(KIMURI))
            {
            enforcementType = EIMEnforcement;
            resourceType.Set(PolicyEngineXACML::KInstantMessagingEnforcemnt);
            }
        else if (0 == segmentName.Compare(KCustomizationURI))
            {
            enforcementType = ECustomization;
            resourceType.Set(PolicyEngineXACML::KCustomizationManagement);
            }
        else if (0 == segmentName.Compare(KTerminalSecurityURI))
            {
            enforcementType = ETerminalSecurity;
            resourceType.Set(PolicyEngineXACML::KTerminalSecurityManagement);
            }
        else if ((0 == segmentName.Compare(KApplicationManagementURI))||(0 == segmentName.Compare(KSCOMOURI)))
            {
            enforcementType = EApplicationManagement;
            resourceType.Set(PolicyEngineXACML::KApplicationManagement);
            }
       else if(0 == segmentName.Compare(KDCMOURI))
            {
            enforcementType = EDCMOEnforcement;
            resourceType.Set(PolicyEngineXACML::KDCMOEnforcement);
            }
		else
		{
		
		return;
		}
    User::LeaveIfError(EnforcementActive(enforcementType,enforcementActive));
   if(enforcementActive)
   {
        RDMCert dmcert;
        TCertInfo ci;
        dmcert.Get( ci );

        // Policy Engine Request
        TRequestContext context;
        TResponse response;
        context.AddSubjectAttributeL(
                PolicyEngineXACML::KTrustedSubject,
                ci
        );
        context.AddResourceAttributeL(
                PolicyEngineXACML::KResourceId,
                resourceType,
                PolicyEngineXACML::KStringDataType
        );
        RPolicyEngine   policyEngine;
        RPolicyRequest  policyRequest;
        User::LeaveIfError(policyEngine.Connect());
        User::LeaveIfError(policyRequest.Open( policyEngine ));
        User::LeaveIfError(policyRequest.MakeRequest( context, response ));
        TResponseValue resp = response.GetResponseValue();
        
        if (resp == EResponsePermit)
            {
        aEnforcementActive = EResponsePermit;
            }
        else
            {
        aEnforcementActive = EResponseDeny;
            }
}


    }
		
