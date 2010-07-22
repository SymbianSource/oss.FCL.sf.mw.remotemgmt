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
* Description: Implementation of policymanagement components
*
*/


// INCLUDE FILES

#include "TrustedSession.h"
#include "XACMLconstants.h"
#include "PolicyEngineServer.h"
#include "PolicyParser.h"
#include "elements.h"
#include "DataTypes.h"
#include "debug.h"
#include "ErrorCodes.h"

#include <x509cert.h>
#include <tconvbase64.h>



const TChar KCASNDelimeter = '-';
_LIT( KSecureIdString, "SECUREID");


CTrustedSession::CTrustedSession()
	: iSessionTrust( EFalse)
{
}

// -----------------------------------------------------------------------------
// CTrustedSession::~CTrustedSession()
// -----------------------------------------------------------------------------
//

CTrustedSession::~CTrustedSession()
{
	delete iCASN;
}



// -----------------------------------------------------------------------------
// CTrustedSession::NewL()
// -----------------------------------------------------------------------------
//

CTrustedSession * CTrustedSession::NewL()
{
	CTrustedSession * self = new(ELeave) CTrustedSession();
	return self;
}

// -----------------------------------------------------------------------------
// CTrustedSession::IsSessionCertificated()
// -----------------------------------------------------------------------------
//
void CTrustedSession::MakeSessionTrustL( const RMessage2& aMessage)
{
	RDEBUG( "Policy Engine: Creating session trust");

	if ( iSessionTrust )
	{
		User::Leave( KErrAlreadyExists);
	}

	//Read certificate info to member
	TPckg<TCertInfo> certInfoPack( iSessionCertInfo);
	aMessage.ReadL(0, certInfoPack, 0);	

	//turn session trust on
	iSessionTrust = ETrue;	

	RDEBUG( "Policy Engine: Session trust created");

}


// -----------------------------------------------------------------------------
// CTrustedSession::IsSessionCertificated()
// -----------------------------------------------------------------------------
//
void CTrustedSession::RemoveSessionTrust()
{
	iSessionTrust = EFalse;
	delete iCASN;
	iCASN = NULL;
}


// -----------------------------------------------------------------------------
// CTrustedSession::IsSessionCertificated()
// -----------------------------------------------------------------------------
//
TCertInfo& CTrustedSession::SessionCertificate()
{
	return iSessionCertInfo;
}



// -----------------------------------------------------------------------------
// CTrustedSession::IsSessionCertificated()
// -----------------------------------------------------------------------------
//

TBool CTrustedSession::CertificatedSession() const
{
	return iSessionTrust;
}




// -----------------------------------------------------------------------------
// CTrustedSession::CASNForSessionL()
// -----------------------------------------------------------------------------
//

const TDesC8& CTrustedSession::CASNForSessionL()
{
	RDEBUG("PolicyEngineServer: CTrustedSession::CASNForSession");

	if ( !iCASN )
	{
		iCASN =	CCertificateMaps::CasnForCertInfoL( iSessionCertInfo );
	}
	
	RDEBUG8_2("PolicyEngineServer: Session CASN: %S", iCASN);
	return *iCASN;
}

// -----------------------------------------------------------------------------
// CTrustedSession::GetMappedTargetTrustedSubjectL()
// -----------------------------------------------------------------------------
//
void CTrustedSession::GetMappedTargetTrustedSubjectL(
	RPointerArray<HBufC8>& aTargetTrustedMappedSubjectArray, 
	CElementBase * aElement, TBool aUseEditedItems )
	{	
	RElementArray attributes;	
	CleanupClosePushL( attributes );
	aElement->FindAttributesL( ESubjectMatch, attributes );
	
	for( TInt i(0); i < attributes.Count(); i++ )
		{
		CMatchObject * matchObject = (CMatchObject*)attributes[ i ];		
		TElementReserver reserver( matchObject );
		CPolicyStorage::PolicyStorage()->CheckElementL( matchObject );
		
		//get attribute ids
		TDesC8& subjectAtt = matchObject->AttributeDesignator()->GetAttributeid();
		TDesC8& value = matchObject->AttributeValue()->Data()->Value();
		
		if( subjectAtt == PolicyEngineXACML::KCertificateMapped )
			{
			//Create new subject info
			CCertificateMaps::CSubjectInfo* tempInfo = new (ELeave) CCertificateMaps::CSubjectInfo();
			CleanupStack::PushL( tempInfo );
			
			//create suject info for it
			CCertificateMaps::CreateSubjectInfoL( tempInfo, value );

			//Search corresponding subject info from certificate storage
			HBufC8* casn = tempInfo->iCASN->AllocL();
			CleanupStack::PushL( casn );
			// do not delete info object
			CCertificateMaps::CSubjectInfo* info = CPolicyEngineServer::CertificateMaps()->FindSubjectL( *casn, aUseEditedItems );		
			
			if( info )
				{
				HBufC8* temp = info->iCASN->AllocL();
				aTargetTrustedMappedSubjectArray.Append( temp );
				}
			
			CleanupStack::PopAndDestroy( casn );
			CleanupStack::PopAndDestroy( tempInfo );
			}
		}

	CleanupStack::PopAndDestroy( &attributes );
	}

// -----------------------------------------------------------------------------
// CTrustedSession::GetTargetTrustedSubjectL()
// -----------------------------------------------------------------------------
//
HBufC8* CTrustedSession::GetTargetTrustedSubjectL( 
	CElementBase * aElement, TBool aUseEditedItems )
	{
	//find alias for certificate, find policy subjects
	RElementArray attributes;	
	CleanupClosePushL( attributes );
	aElement->FindAttributesL( ESubjectMatch, attributes );
	
	HBufC8* retVal = 0;
	
	for( TInt i(0); i < attributes.Count(); i++ )
		{
		CMatchObject * matchObject = (CMatchObject*)attributes[ i ];		
		TElementReserver reserver( matchObject );
		CPolicyStorage::PolicyStorage()->CheckElementL( matchObject );

		//get attribute ids
		TDesC8& subjectAtt = matchObject->AttributeDesignator()->GetAttributeid();
		TDesC8& value = matchObject->AttributeValue()->Data()->Value();

		//tempvalue contain trusted subject value (CASN or alias)
		HBufC8* tempValue = 0;
		
		if( subjectAtt == PolicyEngineXACML::KCertificate )
			{
			//Create new subject info
			CCertificateMaps::CSubjectInfo* tempInfo = new (ELeave) CCertificateMaps::CSubjectInfo();
			CleanupStack::PushL( tempInfo);
			
			//create suject info for it
			CCertificateMaps::CreateSubjectInfoL( tempInfo, value);

			//Search corresponding subject info from certificate storage
			tempValue = tempInfo->iCASN->AllocL();
			
			CleanupStack::PopAndDestroy( tempInfo );			 
			}
		else if( subjectAtt == PolicyLanguage::Constants::AutoCertificate )
			{
			//Create new subject info
			CCertificateMaps::CSubjectInfo* tempInfo = new (ELeave) CCertificateMaps::CSubjectInfo();
			CleanupStack::PushL( tempInfo );

			TCertInfo certInfo;
			TPckg<TCertInfo> pck( certInfo );
	
			TBase64 base64;
			User::LeaveIfError( base64.Decode( value, pck ) );	
				
			//create suject info for it
			CCertificateMaps::CreateSubjectInfoL( tempInfo, certInfo );

			//Search corresponding subject info from certificate storage
			tempValue = tempInfo->iCASN->AllocL();
			
			CleanupStack::PopAndDestroy( tempInfo );			
			}
		else if( subjectAtt == PolicyEngineXACML::KTrustedSubject)
			{
			tempValue = value.AllocL();
			}

		//evaluate temp value
		if( tempValue )
			{
			//Search corresponding subject info from certificate storage
			CCertificateMaps::CSubjectInfo* info = CPolicyEngineServer::CertificateMaps()->FindSubjectL( *tempValue, aUseEditedItems );		

			//if corresponding info found, realloc tempvalue with info->iCASN value			
			if( info )
				{
				delete tempValue;
				tempValue = NULL;
				tempValue = info->iCASN->AllocL();
				}			

			if( retVal )
				{
				//if retVal already exists and temp value doesn't match, break, delete buffers and set null to retval
				if ( *retVal != *tempValue )
					{
					delete retVal;
					retVal = 0;
					break;
					} 
				}
			else
				{
				//if retVal is not exist yet, create new one
				retVal = tempValue->AllocL();
				}
		
			delete tempValue;
			}
		}

	CleanupStack::PopAndDestroy( &attributes );
	return retVal;
	}



// -----------------------------------------------------------------------------
// CTrustedSession::IsCertificateMappingValidL()
// -----------------------------------------------------------------------------
//

TBool CTrustedSession::IsCertificateMappingValidL( CElementBase * aElement)
{
	//element is parsed and must be valid 
	__ASSERT_ALWAYS ( aElement, User::Panic(Panics::TrustedSessionManagerPanic, KErrCorrupt));

	//element type must be rule
	if ( aElement->ElementType() != ERule)
	{
		return EFalse;
	}
	


	CTarget * target = ((CRule*)aElement)->GetTarget();
	if ( !target)
	{
		return EFalse;
	}
	
	//Mapping rule target contains allways one CSubjects element
	if ( target->SubElementType( 0, 1) != ESubjects )
	{
		return EFalse;
	}
	
	CSubjects * subjects = (CSubjects*)target->Element(0);

	//Mapping subjects contains allways one CSubject element
	if ( subjects->SubElementType( 0, 1) != ESubject )
	{
		return EFalse;
	}
	
	CSubject * subject = (CSubject*)subjects->Element(0);

	//element type should be subjectmatch in all cases!
	if ( subject->SubElementType( 0, 2) != ESubjectMatch || 
		 subject->SubElementType( 1) != ESubjectMatch )
		{
			return EFalse;
		}
	
	
	//Get match objects
	CMatchObject* subject1 = (CMatchObject*) subject->Element(0);
	CMatchObject* subject2 = (CMatchObject*) subject->Element(1);
			
	//get attribute ids
	TDesC8& subjectAtt1 = subject1->AttributeDesignator()->GetAttributeid();
	TDesC8& subjectAtt2 = subject2->AttributeDesignator()->GetAttributeid();
			
	//certificate -> alias mapping
	if ( subjectAtt1 == PolicyEngineXACML::KCertificate && subjectAtt2 == PolicyEngineXACML::KAliasId)
	{
		CPolicyEngineServer::CertificateMaps()->BringUpToDateL();
	
		//Check that new alias is not already exist
		if ( CPolicyEngineServer::CertificateMaps()->IsAliasExistL( subject2->AttributeValue()->Data()->Value()))
		{
			return EFalse;
		}
		
		TDesC8& certificateValue = subject1->AttributeValue()->Data()->Value();
		
		//Leaves if certificate is not valid, otherwise just delete certificate
		CX509Certificate * cert = CCertificateMaps::ParseCertificateL( certificateValue); 
		delete cert;
			
		return ETrue;
	} 

	//certificate -> new certificate (certificate )
	if ( subjectAtt1 == PolicyEngineXACML::KCertificate && subjectAtt2 == PolicyEngineXACML::KCertificateMapped)
	{
		TDesC8& certificateValue = subject1->AttributeValue()->Data()->Value();
		TDesC8& certificateValueMapped = subject2->AttributeValue()->Data()->Value();

		//Leaves if certificate is not valid, otherwise just delete certificate
		CX509Certificate * cert = CCertificateMaps::ParseCertificateL( certificateValue); 
		delete cert;
		CX509Certificate * certMapped = CCertificateMaps::ParseCertificateL( certificateValueMapped); 
		delete certMapped;


		return ETrue;
	} 
	
	return EFalse;
}

// -----------------------------------------------------------------------------
// CTrustedSession::AddCertificateMappingL()
// -----------------------------------------------------------------------------
//
CElementBase* CTrustedSession::CreateCertificateMappingL( const TDesC8& aAlias )
{
	using namespace PolicyLanguage::Constants;
	using namespace PolicyLanguage::NativeLanguage;

	//Check that alias is valid (not allready exists)
	if ( CPolicyEngineServer::CertificateMaps()->IsAliasExistL( aAlias))
	{
		return 0;
	}
	
	//Ensures that CASN is valid...
	CASNForSessionL();
	
	//Create new mapping policy
	//first create id for new rule, format is auto_certificate+CASN
	HBufC8 * extId = HBufC8::NewLC( AutoCertificateLength + iCASN->Length() + 1);
	TPtr8 extIdPtr = extId->Des();
	
	extIdPtr.Append( AutoCertificate);
	extIdPtr.Append( '_');
	extIdPtr.Append( *iCASN);
	
	ReplaceIllegalCharacters( extIdPtr );
	
	//Check id
	if ( !CPolicyStorage::PolicyStorage()->IsRealIdValidL( extIdPtr))
	{
		CleanupStack::PopAndDestroy( extId);
		return 0;
	}
		
	//and assign rule to element
	CRule *rule = CRule::NewL();

	CleanupStack::PushL( rule);
	rule->SetRuleIdL( extIdPtr);
	rule->SetEffectL( Rule::Permit);
	
	//target
	CTarget * target = CTarget::NewL();
	CleanupStack::PushL( target);
	rule->AddElementL( 0, target);
	CleanupStack::Pop( target);
	
	//subjects
	CSubjects * subjects = CSubjects::NewL();
	CleanupStack::PushL( subjects );
	target->AddElementL( 0, subjects);
	CleanupStack::Pop( subjects );

	//subject
	CSubject * subject = CSubject::NewL();
	CleanupStack::PushL( subject );
	subjects->AddToElementListL( subject , ETrue);
	CleanupStack::Pop( subject );
	
	
	//first subjectmatch
	CMatchObject * match = new (ELeave) CMatchObject( ESubjectMatch);
	subject->AddToElementListL( match, ETrue);
	
	match->AddAttributeL( 0, MatchObject::MatchId, 
							 Functions::FunctionStringEqualId);
	
	//designator and value for certificate
	CSubjectAttributeDesignator * designator = CSubjectAttributeDesignator::NewL();
	match->AddElementL( 0, designator);
	designator->AddAttributeL( 0, AttributeDesignators::AttributeId, AutoCertificate);
	designator->AddAttributeL( 0, AttributeDesignators::DataType, PolicyLanguage::NativeLanguage::AttributeValues::StringDataType);
	
	//serialize TCertInfo and decode it to base64 format
	TPckg<TCertInfo> pck( iSessionCertInfo);
	TBase64 base64;
	HBufC8 * base64coded = HBufC8::NewLC( pck.Length() * 3);
	TPtr8 base64Ptr = base64coded->Des();
	User::LeaveIfError( base64.PortableEncode( pck, base64Ptr));	

	CAttributeValue * aliasValue = CAttributeValue::NewL( base64Ptr, PolicyLanguage::NativeLanguage::AttributeValues::StringDataType);
	CleanupStack::PopAndDestroy( base64coded);
		
	match->AddElementL( 0, aliasValue);
	
	//second subjectmatch
	match = new (ELeave) CMatchObject( ESubjectMatch);
	subject->AddToElementListL( match, ETrue);
	
	match->AddAttributeL( 0, MatchObject::MatchId, 
							 Functions::FunctionStringEqualId);


	//designator and value for alias
	designator = CSubjectAttributeDesignator::NewL();
	match->AddElementL( 0, designator);
	designator->AddAttributeL( 0, AttributeDesignators::AttributeId, PolicyEngineXACML::KAliasId);
	designator->AddAttributeL( 0, AttributeDesignators::DataType, PolicyLanguage::NativeLanguage::AttributeValues::StringDataType);
	
	aliasValue = CAttributeValue::NewL( aAlias, PolicyLanguage::NativeLanguage::AttributeValues::StringDataType);
	match->AddElementL( 0, aliasValue);
	
	
	//Destroy objects....
	CleanupStack::Pop( rule);
	CleanupStack::PopAndDestroy( extId);
	
	
	return rule;
}


void CTrustedSession::ReplaceIllegalCharacters( TPtr8& aData )
	{
	RDEBUG("CTrustedSession::ReplaceIllegalCharacters()");
	
	TChar char1('/');
	TBuf8<10> buf;
	buf.Append( _L("_") );
	for(TInt i = 0;;i++)
		{
		TInt index = aData.Locate( char1 );
		if( index > KErrNotFound )
			{
			aData.Replace( index, 1, buf );
			}
		else
			{
			break;
			}
		}
	
	}

// -----------------------------------------------------------------------------
// CTrustedSession::CommonNameForSubject()
// -----------------------------------------------------------------------------
//

const TDesC8& CTrustedSession::CommonNameForSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems )
{
	RDEBUG("PolicyEngineServer: CTrustedSession::CommonNameForSubjectL");

	//ensures that iCASN is valid
	CASNForSessionL();
	
	if ( aTrustedSubject == *iCASN )
	{
		return iSessionCertInfo.iSubjectDNInfo.iCommonName;
	}
	
	CPolicyEngineServer::CertificateMaps()->BringUpToDateL();
	CCertificateMaps::CSubjectInfo* info = CPolicyEngineServer::CertificateMaps()->FindSubjectL( aTrustedSubject, aUseEditedItems);

	
	if (info)
	{
		return *info->iSubject;
	}
	
	return KNullDesC8;
}

// -----------------------------------------------------------------------------
// CTrustedSession::FingerPrintForSubject()
// -----------------------------------------------------------------------------
//


const TDesC8& CTrustedSession::FingerPrintForSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems)
{
	RDEBUG("PolicyEngineServer: CTrustedSession::CommonNameForSubjectL");

	//ensures that iCASN is valid
	CASNForSessionL();
	
	if ( aTrustedSubject == *iCASN )
	{
		//get finger print
		iFingerPrint.Zero();

		//convert fingerprint to plain text
		for ( TInt i(0); i < iSessionCertInfo.iFingerprint.Length(); i++)
		{
			iFingerPrint.AppendNum( iSessionCertInfo.iFingerprint[i], EHex);
		}		
		
		return iFingerPrint;
	}
	
	CPolicyEngineServer::CertificateMaps()->BringUpToDateL();
	CCertificateMaps::CSubjectInfo* info = CPolicyEngineServer::CertificateMaps()->FindSubjectL( aTrustedSubject, aUseEditedItems);
	
	if (info)
	{
		return *info->iFingerPrint;
	}
	
	return KNullDesC8;
}



// -----------------------------------------------------------------------------
// CTrustedSession::AddSessionSIDL()
// -----------------------------------------------------------------------------
//

void CTrustedSession::AddSessionSIDL( TUid aSecirityUID)
{
	//add SECUREID start mark
	iSessionSID.Zero();
	iSessionSID.Append( KSecureIdString);	
	iSessionSID.Append( aSecirityUID.Name());
	iSessionSID.UpperCase();
	
}


// -----------------------------------------------------------------------------
// CTrustedSession::SIDForSession()
// -----------------------------------------------------------------------------
//

const TDesC8& CTrustedSession::SIDForSession()
{
	return iSessionSID;
}


// -----------------------------------------------------------------------------
// CTrustedSession::CertMatchL()
// -----------------------------------------------------------------------------
//
TBool CTrustedSession::CertMatchL( const TDesC8& aTrustedSubject1, const TDesC8& aTrustedSubject2, TBool /*aUseEditedItems*/ )
{
	TBool subjectMatch = EFalse;
	
	CCertificateMaps * maps = CPolicyEngineServer::CertificateMaps();
	maps->BringUpToDateL();


	//find subject info for asubject
	for ( TInt i = 0; i < maps->iCerts.Count(); i++)
	{
		CCertificateMaps::CSubjectInfo* info = maps->iCerts[i];
		
		 
		if ( info->Match( aTrustedSubject1, ETrue) && info->Match( aTrustedSubject2, ETrue))
		{
			//subject1 and subject2 are same instance!!
			subjectMatch = ETrue;
			break;	
		}
		
	}
	
	return subjectMatch;
}

// -----------------------------------------------------------------------------
// CTrustedSession::RoleMatchL()
// -----------------------------------------------------------------------------
//
TBool CTrustedSession::RoleMatchL( const TDesC8& aTrustedSubject, const TDesC8& aRole, TBool aUseEditedItems )
{
	//find subject info for asubject
	CCertificateMaps * cerMaps = CPolicyEngineServer::CertificateMaps();
	cerMaps->BringUpToDateL();

	TBool validRole = EFalse;
	
	for ( TInt i(0); i < cerMaps->iCerts.Count(); i++)
	{
		CCertificateMaps::CSubjectInfo* info = cerMaps->iCerts[i];
		
		if ( !CCertificateMaps::EvaluateElementValidity( aUseEditedItems, info->iState))
		{
			continue;
		}		
		
		if ( info->Match( aTrustedSubject, ETrue) && info->RoleMatchL( aRole))
		{
			//aRole is valid role for aTrustedSubject!!
			validRole = ETrue;
			break;
		}
	}
	
	
	return validRole;
}
		

// -----------------------------------------------------------------------------
// CCertificateMaps::CRolesInfo::CRolesInfo()
// -----------------------------------------------------------------------------
//
CCertificateMaps::CRolesInfo::CRolesInfo()
{
}


// -----------------------------------------------------------------------------
// CCertificateMaps::CRolesInfo::~CRolesInfo()
// -----------------------------------------------------------------------------
//
CCertificateMaps::CRolesInfo::~CRolesInfo()
{
	delete iRole;
}


// -----------------------------------------------------------------------------
// CCertificateMaps::CRolesInfo::NewL()
// -----------------------------------------------------------------------------
//
CCertificateMaps::CRolesInfo* CCertificateMaps::CRolesInfo::NewL( const TDesC8& aRole, TElementState aState)
{
	CRolesInfo * self = new (ELeave) CRolesInfo();
	
	self->iRole = aRole.AllocL();
	self->iState = aState;
	
	return self;
}

// -----------------------------------------------------------------------------
// CCertificateMaps::CSubjectInfo::CSubjectInfo()
// -----------------------------------------------------------------------------
//

CCertificateMaps::CSubjectInfo::CSubjectInfo()
{
}

// -----------------------------------------------------------------------------
// CCertificateMaps::CSubjectInfo::~CSubjectInfo()
// -----------------------------------------------------------------------------
//

CCertificateMaps::CSubjectInfo::~CSubjectInfo()
{
	delete iCASN;			
	delete iMappedToCert;		
	delete iAlias;			
	delete iSubject;		
	delete iFingerPrint;	
	iRoles.ResetAndDestroy();
	iRoles.Close();
}

// -----------------------------------------------------------------------------
// CCertificateMaps::CSubjectInfo::~CSubjectInfo()
// -----------------------------------------------------------------------------
//
TBool CCertificateMaps::CSubjectInfo::RoleMatchL( const TDesC8& aRole)
{
	TBool retVal = EFalse;

	//find roles from this subject match
	for ( TInt i(0); i < iRoles.Count(); i++)
	{
		CRolesInfo * info = iRoles[i];
		
		if ( aRole.CompareF( *info->iRole)  == 0)
		{
			retVal = ETrue;
			break;
		}
	}
	
	//and if needed check also mappedtoinfo
	if ( !retVal && iMappedToInfo)
	{
		retVal = iMappedToInfo->RoleMatchL( aRole);
	}
	
	return retVal;
}

// -----------------------------------------------------------------------------
// CCertificateMaps::CSubjectInfo::~CSubjectInfo()
// -----------------------------------------------------------------------------
//
TBool CCertificateMaps::CSubjectInfo::Match( const TDesC8& aTrustedSubject, TBool aCheckMappedCertificates)
{
	//Check CASN
	if ( iCASN )
	{
		if ( aTrustedSubject.CompareF(  *iCASN)  == 0 )
		{
			return ETrue;
		}
	}

	//Check alias
	if ( iAlias )
	{
		if ( aTrustedSubject.CompareF( *iAlias ) == 0)
		{
			return ETrue;
		}
	}
	
	if ( aCheckMappedCertificates && iMappedToInfo )
	{
		return iMappedToInfo->Match( aTrustedSubject, ETrue);
	}

	return EFalse;	
}


TRole CCertificateMaps::CSubjectInfo::CertificateRoleL( TBool aUseEditedItems )
	{
	TPtr8 ptr = iFingerPrint->Des();
	RDEBUG8_2("CSubjectInfo::CertificateRoleL() (fingerprint: %S)", &ptr );
	
	TRole retVal( ENoRole );
	
	RDEBUG8_2("	-> this certificate has %d roles", iRoles.Count() );
	for( TInt j(0); iRoles.Count(); j++ )
		{
		CRolesInfo* role = iRoles[ j ];
		
		//accept only elements which are committed...
		if( !EvaluateElementValidity( aUseEditedItems, role->iState ) )
			{
			RDEBUG("	-> found invalid element from CRolesInfo");
			continue;
			}			
		
		if( *role->iRole == PolicyEngineXACML::KRoleCorporate )
			{
			RDEBUG("	-> found KRoleCorporate");
			retVal = ECorporateRole;
			break;	
			}
		}
	
	if( ( retVal == ENoRole ) && iMappedToInfo )
		{
		RDEBUG("	-> -> searching KRoleCorporate from certificate mappings recursively .. ");
		retVal = iMappedToInfo->CertificateRoleL( aUseEditedItems );
		RDEBUG("	<- <- searching KRoleCorporate from certificate mappings recursively .. DONE");
		}
	else if( ( retVal == ENoRole ) && !iMappedToInfo )
		{
		RDEBUG("	-> not mapped - can not search recursively");
		}
		
	return retVal;
	}


// -----------------------------------------------------------------------------
// CCertificateMaps::CCertificateMaps()
// -----------------------------------------------------------------------------
//
CCertificateMaps::CCertificateMaps()
{
	iPolicyStorage = CPolicyStorage::PolicyStorage();
	iMapsUpToDate = EFalse;
}

// -----------------------------------------------------------------------------
// CCertificateMaps::~CCertificateMaps()
// -----------------------------------------------------------------------------
//
CCertificateMaps::~CCertificateMaps()
{
	iCerts.ResetAndDestroy();
	iCerts.Close();
}

CCertificateMaps* CCertificateMaps::NewL()
{
	CCertificateMaps* self = new (ELeave) CCertificateMaps();
	
	CleanupStack::PushL( self);
	CleanupStack::Pop( self);
	
	return self;	
}

// -----------------------------------------------------------------------------
// CCertificateMaps::LoadCertificateMaps()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::LoadCertificateMapsL()
{
	RDEBUG("CCertificateMaps::LoadCertificateMaps");

	iCerts.ResetAndDestroy();

	using namespace PolicyLanguage::Constants;
	using namespace PolicyEngineXACML;

	//get Certificate mapping policy from storage (get also edited elements)
	CElementBase * aliasPolicy = iPolicyStorage->GetEditableElementL( CertMappingPolicy);

	//if eleemnt is not valid
	if ( !aliasPolicy)
	{
		return;
	}
		
	//reserve and load element
	TElementReserver certReserver( aliasPolicy);
	iPolicyStorage->CheckElementL( aliasPolicy);

	//find alias for certificate, find policy subjects
	RElementArray attributes;	
	CleanupClosePushL( attributes);
	aliasPolicy->FindAttributesL( ESubject, attributes);


	for ( TInt i = 0; i < attributes.Count(); i++)
	{
		CElementBase * element = attributes[i];
		
		//only two subjectmatch allowed
		if ( element->ElementType() == ESubject && element->ElementCount() == 2 )
		{
			//element type should be subjectmatch in all cases!
			__ASSERT_ALWAYS ( element->Element(0)->ElementType() == ESubjectMatch, User::Panic(Panics::TrustedSessionManagerPanic, KErrCorrupt));
			__ASSERT_ALWAYS ( element->Element(1)->ElementType() == ESubjectMatch, User::Panic(Panics::TrustedSessionManagerPanic, KErrCorrupt));

			//Get match objects, reserve and load them
			CMatchObject* subject1 = (CMatchObject*) element->Element(0);
			CMatchObject* subject2 = (CMatchObject*) element->Element(1);
			
			TElementReserver reserver1( subject1);
			TElementReserver reserver2( subject2);
			iPolicyStorage->CheckElementL( subject1);
			iPolicyStorage->CheckElementL( subject2);
			
			//get attribute ids
			TDesC8& subjectAtt1 = subject1->AttributeDesignator()->GetAttributeid();
			TDesC8& subjectAtt2 = subject2->AttributeDesignator()->GetAttributeid();
			
			//Create new subject info
			CSubjectInfo* info = new (ELeave) CSubjectInfo();
			CleanupStack::PushL( info);
			
			//set committed state flag
			info->iState = element->iElementState;

			//certificate -> alias mapping
			if ( subjectAtt1 == KCertificate && subjectAtt2 == KAliasId)
			{
				RDEBUG("PolicyEngineServer: Certificate mapped to Alias");
			
				//get attribute value and parse certificate
				TDesC8& binaryCert = subject1->AttributeValue()->Data()->Value();
				
				//create suject info for it
				CreateSubjectInfoL( info, binaryCert);
					
				TDesC8& alias = subject2->AttributeValue()->Data()->Value();
				info->iAlias = alias.AllocL();

				RDEBUG8_3("PolicyEngineServer: Certificate mapped: %S -> %S", info->iCASN, info->iAlias);

				iCerts.AppendL( info);
				CleanupStack::Pop( info);
			}
			else if ( subjectAtt1 == KCertificate && subjectAtt2 == KCertificateMapped)
			{
				RDEBUG("PolicyEngineServer: Certificate mapped to new certificate");
		
				//get attribute values....
				TDesC8& binaryOldCert = subject1->AttributeValue()->Data()->Value();
				TDesC8& binaryNewCert = subject2->AttributeValue()->Data()->Value();

				//create subject info for new certificate mapping
				CreateSubjectInfoL( info, binaryNewCert);

				//create subject info for mapped certificate
				CSubjectInfo* mappingInfo = new (ELeave) CSubjectInfo();
				CleanupStack::PushL( mappingInfo);
				CreateSubjectInfoL( mappingInfo, binaryOldCert);
				
				//add the mapping info to new certificate 
				info->iMappedToCert = mappingInfo->iCASN->AllocL();
				CleanupStack::PopAndDestroy( mappingInfo);

				iCerts.AppendL( info);
				CleanupStack::Pop( info);
			}
			else if  ( subjectAtt1 == AutoCertificate && subjectAtt2 == KAliasId)
			{
				RDEBUG("PolicyEngineServer: Certificate mapped to Alias");
			
				//get attribute value and parse certificate info ( base64 -> serialization -> TCertInfo)
				TDesC8& binaryCert = subject1->AttributeValue()->Data()->Value();
				
				TCertInfo certInfo;
				TPckg<TCertInfo> pck( certInfo);
	
				TBase64 base64;
				User::LeaveIfError( base64.Decode( binaryCert, pck));	
				
				//create suject info for it
				CreateSubjectInfoL( info, certInfo);
					
				TDesC8& alias = subject2->AttributeValue()->Data()->Value();
				info->iAlias = alias.AllocL();

				RDEBUG8_3("PolicyEngineServer: Certificate mapped: %S -> %S", info->iCASN, info->iAlias);

				iCerts.AppendL( info);
				CleanupStack::Pop( info);
			
			}
			else
			{
				CleanupStack::PopAndDestroy( info);		
			}
			
			reserver2.Release();
			reserver1.Release();
			
		}
	}

	CleanupStack::PopAndDestroy( &attributes); //attribute list
	certReserver.Release();
	
	//Create pointer reference mapping instead of name mapping in certificate->certificate mapping table
	for ( TInt i(0); i < iCerts.Count(); i++)
	{
		//find all CSubjectInfos which include certificate to certificate mapping
		CSubjectInfo * infoi = iCerts[i];
		
		if ( infoi->iMappedToCert )
		{
			//find corresponding certificate
			for ( TInt j(0); j < iCerts.Count(); j++)
			{
				CSubjectInfo * infoj = iCerts[j];
				
				if ( *infoj->iCASN == *infoi->iMappedToCert)
				{
					//add reference mapping....
					RDEBUG("	-> added certificate reference mapping");
					infoi->iMappedToInfo = infoj;
					break;
				}
			}
		}
	}	
				
	//get roles mapping policy from storage
	CElementBase * rolesPolicy = iPolicyStorage->GetEditableElementL( RolesMappingPolicy);

	//if eleemnt is not valid
	if ( !rolesPolicy)
	{
		return;
	}
	
	//reserve and load element
	TElementReserver rolesReserver( rolesPolicy);
	iPolicyStorage->CheckElementL( rolesPolicy);

	//find roles for certificate, find policy subjects
	RElementArray roleattributes;	
	CleanupClosePushL( roleattributes);
	rolesPolicy->FindAttributesL( ESubject, roleattributes);

	for ( TInt i = 0; i < roleattributes.Count(); i++)
	{
		CElementBase * element = roleattributes[i];
		
		//only two subjectmatch allowed
		if ( element->ElementType() == ESubject && element->ElementCount() == 2 )
		{
			//element type should be subjectmatch in all cases!
			__ASSERT_ALWAYS ( element->Element(0)->ElementType() == ESubjectMatch, User::Panic(Panics::TrustedSessionManagerPanic, KErrCorrupt));
			__ASSERT_ALWAYS ( element->Element(1)->ElementType() == ESubjectMatch, User::Panic(Panics::TrustedSessionManagerPanic, KErrCorrupt));
		
			//get and reserve elements
			CMatchObject* subject1 = (CMatchObject*)element->Element(0);
			CMatchObject* subject2 = (CMatchObject*)element->Element(1);
			
			TElementReserver reserver1( subject1);
			TElementReserver reserver2( subject2);
			iPolicyStorage->CheckElementL( subject1);
			iPolicyStorage->CheckElementL( subject2);
				
			
			//check that reloes mapping is correct
			if ( subject1->AttributeDesignator()->GetAttributeid() == KTrustedSubject &&
			     subject2->AttributeDesignator()->GetAttributeid() == KRoleId )
			{
				//map roles to certificate
				AddRolesToCertL( subject2->AttributeValue()->Data()->Value(),
								 subject1->AttributeValue()->Data()->Value(), element->iElementState);
			}
			
			reserver2.Release();
			reserver1.Release();

		}
	}
	
	CleanupStack::PopAndDestroy( &roleattributes); //attribute list
	
	rolesReserver.Release();
}


void CCertificateMaps::BringUpToDateL()
{
	if ( !iMapsUpToDate)
	{
		LoadCertificateMapsL();
		iMapsUpToDate = ETrue;
	}
}


// -----------------------------------------------------------------------------
// CCertificateMaps::NewMappingsAvailable()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::NewMappingsAvailable()
{
	iMapsUpToDate = EFalse;
}
		

// -----------------------------------------------------------------------------
// CCertificateMaps::IsAliasExist()
// -----------------------------------------------------------------------------
//
TBool CCertificateMaps::IsAliasExistL( const TDesC8& aAlias)
{
	BringUpToDateL();

	//find subject info for aAlias (if info exist also alias exist)
	for ( TInt i = 0; i < iCerts.Count(); i++)
	{
		CSubjectInfo * info = iCerts[i];
		 
		//if alias match
		if ( info->iAlias)
		{
			if ( aAlias == *info->iAlias) 
				return ETrue;
		} 	
	}
	
	return EFalse;	
}


// -----------------------------------------------------------------------------
// CCertificateMaps::CertificateRoleL()
// -----------------------------------------------------------------------------
//
TRole CCertificateMaps::CertificateRoleL( const TCertInfo& aCertInfo, TBool aUseEditedItems)
	{
	RDEBUG("CCertificateMaps::CertificateRoleL()");
	BringUpToDateL();

	HBufC8* buf = CasnForCertInfoL( aCertInfo );
	TRole retVal = ENoRole;

	//find subject info for aAlias (if info exist also alias exist)
	RDEBUG_2("CCertificateMaps::CertificateRoleL() - checking %d certificates", iCerts.Count() );
	for( TInt i = 0; i < iCerts.Count(); i++)
		{
		// CSubjectInfo contains list of roles that are owned by the certificate
		CSubjectInfo* info = iCerts[ i ];
		
		//accept only elements which are committed...
		if( !EvaluateElementValidity( aUseEditedItems, info->iState ) )
			{
			RDEBUG("	-> found invalid element");
			continue;
			}
					 
		//if alias match
		if( info->iCASN->CompareF( *buf ) == 0 )
			{
			RDEBUG("	-> CASN comparison match");
			// recursive check if there is a mapping (e.g. cert_to_cert policy)
			retVal = info->CertificateRoleL( aUseEditedItems );
			if( retVal != ENoRole )
				{
				break;
				}
			}
		else
			{
			RDEBUG("	-> CASN did not match!");
			}
		}
	
	if( retVal == ECorporateRole )
		{
		RDEBUG("	-> found ECorporateRole");
		}
	else
		{
		RDEBUG("	-> no role found!");
		}

	delete buf;
	return retVal;	
	}



// -----------------------------------------------------------------------------
// CCertificateMaps::FindSubjectL()
// -----------------------------------------------------------------------------
//

CCertificateMaps::CSubjectInfo* CCertificateMaps::FindSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems)
{
	CSubjectInfo * info = NULL;
	
	BringUpToDateL();

	//find subject info for asubject
	for ( TInt i = 0; i < iCerts.Count(); i++)
	{
		info = iCerts[i];
		
		if ( !EvaluateElementValidity( aUseEditedItems, info->iState))
		{
			continue;
		}

		
		if ( info->Match( aTrustedSubject, EFalse))
		{
			return info;
		}
	}
	
	return NULL;
}


// -----------------------------------------------------------------------------
// CCertificateMaps::AddRolesToCertL()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::AddRolesToCertL( const TDesC8& aRole, const TDesC8& aSubject, TElementState aElementState)
	{
	RDEBUG("CCertificateMaps::AddRolesToCertL");
	RDEBUG("	-> ");
	
	for( TInt i = 0; i < iCerts.Count(); i++)
		{
		CSubjectInfo * info = iCerts[i];
		
		//if CASN match
		if( info->iCASN )
			{
			if( aSubject == *info->iCASN )
				{
				RDEBUG8_3("PolicyEngineServer: Role mapped: %S -> %S", &aSubject, &aRole );
				info->iRoles.AppendL( CRolesInfo::NewL( aRole, aElementState ) );
				break;
				}
			}
				
		//if alias match
		if( info->iAlias )
			{
			if ( aSubject == *info->iAlias)
				{
				RDEBUG8_3("PolicyEngineServer: Role mapped: %S -> %S", &aSubject, &aRole );
				info->iRoles.AppendL( CRolesInfo::NewL( aRole, aElementState ) );
				break;
				}
			} 		
		}
	}


HBufC8* CCertificateMaps::CasnForCertInfoL( const TCertInfo& aCertInfo)
{
	TInt certLength = aCertInfo.iIssuerDNInfo.iCountry.Length();
	certLength += aCertInfo.iIssuerDNInfo.iOrganizationUnit.Length();
	certLength += aCertInfo.iIssuerDNInfo.iOrganization.Length();
	certLength += aCertInfo.iIssuerDNInfo.iCommonName.Length();
	certLength += aCertInfo.iSerialNo.Length() * 2;
	certLength += aCertInfo.iFingerprint.Length() * 2;
	
	HBufC8* casn = HBufC8::NewL( certLength + 5 );
	TPtr8 ptr = casn->Des();
	
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
		TInt serialLength = aCertInfo.iSerialNo.Length();
		TPtr8 serialNum( (unsigned char *)(ptr.Ptr() + ptr.Length()), serialLength * 2, serialLength * 2);
	
		//convert fingerprint to plain text
		for ( TInt i(0); i < aCertInfo.iSerialNo.Length(); i++)
		{
			ptr.AppendNumFixedWidth( aCertInfo.iSerialNo[i], EHex, 2);
		}	
			
		serialNum.UpperCase();
		ptr.Append( KCASNDelimeter);
	}

	if ( aCertInfo.iFingerprint.Length())
	{
		HBufC8* fingerPrint = HBufC8::NewLC( aCertInfo.iFingerprint.Length() * 2);
		TPtr8 fingerPrintPtr = fingerPrint->Des();
		CreateFingerPrint( fingerPrintPtr, aCertInfo);
		ptr.Append( fingerPrintPtr);
		CleanupStack::PopAndDestroy( fingerPrint);
	}


	return casn;
}


// -----------------------------------------------------------------------------
// CCertificateMaps::CreateFingerPrint()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::CreateFingerPrint( TDes8& aFingerPrint, const TCertInfo& aCertInfo)
{
	//convert fingerprint to plain text
	for ( TInt i(0); i < aCertInfo.iFingerprint.Length(); i++)
	{
			TBuf8<2> hexBuf;
			hexBuf.AppendNumFixedWidth( aCertInfo.iFingerprint[i], EHex, 2);
			hexBuf.UpperCase();
	
			aFingerPrint.Append(hexBuf);
	}	
}



// -----------------------------------------------------------------------------
// CCertificateMaps::CreateSubjectInfoL()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::CreateSubjectInfoL( CSubjectInfo * aInfo, const TCertInfo& aCertInfo)
{
	aInfo->iCASN = CasnForCertInfoL( aCertInfo);
	aInfo->iSubject = aCertInfo.iSubjectDNInfo.iCommonName.AllocL();
	
	aInfo->iFingerPrint = HBufC8::NewL(  aCertInfo.iFingerprint.Length() * 2);
	TPtr8 ptr = aInfo->iFingerPrint->Des();
	CreateFingerPrint( ptr, aCertInfo);
}



// -----------------------------------------------------------------------------
// CCertificateMaps::CreateSubjectInfoL()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::CreateSubjectInfoL( CSubjectInfo * aInfo, const TDesC8& aCertificate)
{
	//parse certificate
	CX509Certificate * cert = ParseCertificateL( aCertificate); 
	CleanupStack::PushL( cert);
				
	//parse subject info part (CASN, issued to)
	ParseSubjectInfoL( aInfo, cert);

	//destroy certificate				
	CleanupStack::PopAndDestroy( cert);
}

// -----------------------------------------------------------------------------
// CCertificateMaps::ParseCertificateL()
// -----------------------------------------------------------------------------
//
CX509Certificate* CCertificateMaps::ParseCertificateL( const TDesC8& aCertificate)
{
		RDEBUG("PolicyEngineServer: Certificate parsing");

		TPtrC8 certPtr = aCertificate;
		CX509Certificate* returnValue = 0;
				
		//if value contains CData marks it is binary coded, otherwise base64 coded
		if ( ParserUtility::ContainsCDataField( certPtr) )
		{
			RDEBUG("PolicyEngineServer: Binary format certificate detected, remove CDATA marks...");
			//remove cdata marks and create subject info
			ParserUtility::RemoveCDataMarksL( certPtr);
			RDEBUG_2("PolicyEngineServer: Binary format:", &aCertificate);

#ifdef _DEBUG	//
			TPtrC8 debugPtr = aCertificate;
			while ( debugPtr.Length())
			{
				TInt size = 100;
				if ( debugPtr.Length() < 100 ) size = debugPtr.Length();
				
				TPtrC8 temp = debugPtr.Left( size);
				debugPtr.Set( debugPtr.Mid( size));
				RDEBUG_2("%S", &temp);
			}
#endif	//#ifdef __DEBUG
			returnValue = CX509Certificate::NewL( certPtr);
		}
		else
		{
			RDEBUG("PolicyEngineServer: BASE64 format certificate detected, convert to binary format");
			RDEBUG("PolicyEngineServer: BASE64 format:");
#ifdef _DEBUG
			TPtrC8 debugPtr = aCertificate;
			while ( debugPtr.Length())
			{
				TInt size = 64;
				if ( debugPtr.Length() < 64 ) size = debugPtr.Length();
				
				TPtrC8 temp = debugPtr.Left( size);
				debugPtr.Set( debugPtr.Mid( size));
				RDEBUG_2("%S", &temp);
			}
#endif	//#ifdef __DEBUG
			//convert base64 coding to binary format
			TBase64 base64;
			
			HBufC8 * base64Cert = HBufC8::NewLC( certPtr.Length());
			TPtr8 base64Ptr = base64Cert->Des();
			User::LeaveIfError( base64.Decode( certPtr, base64Ptr));
					
			RDEBUG("PolicyEngineServer: Converting....");
			RDEBUG_2("PolicyEngineServer: Binary format (size %d bytes):", base64Ptr.Length());

#ifdef _DEBUG
			debugPtr.Set( base64Ptr);
			while ( debugPtr.Length())
			{
				TInt size = 100;
				if ( debugPtr.Length() < 100 ) size = debugPtr.Length();
				
				TPtrC8 temp = debugPtr.Left( size);
				debugPtr.Set( debugPtr.Mid( size));
				RDEBUG_2("%S", &temp);
			}
#endif	//#ifdef __DEBUG

			returnValue = CX509Certificate::NewL( base64Ptr);
			CleanupStack::PopAndDestroy( base64Cert);
		}


		RDEBUG("PolicyEngineServer: Certificate Ok!");
		return returnValue;
}


// -----------------------------------------------------------------------------
// CCertificateMaps::ParseSubjectInfo()
// -----------------------------------------------------------------------------
//

void CCertificateMaps::ParseSubjectInfoL( CSubjectInfo* aInfo, CX509Certificate* aCertificate)
{
	RDEBUG("PolicyEngineServer: CTrustedSession::ParseSubjectInfoL");


	//Parse distinguished name for subject (DN)
	aInfo->iSubject = ParseDNL( &aCertificate->SubjectName(), ETrue);
	RDEBUG8_2("PolicyEngineServer: Certificate subject: %S", aInfo->iSubject);

	//Parse issuer distinguished name(DN)
	HBufC8* ca = ParseDNL( &aCertificate->IssuerName(), EFalse);	
	
	//get serial number
	TBuf<64> serialNo;
	TPtrC8 serialPtr = aCertificate->SerialNumber();

	//convert serial no to plain text
	for ( TInt i(0); i < serialPtr.Length(); i++)
	{
		serialNo.AppendNumFixedWidth( serialPtr[i], EHex, 2);
	}
	
	serialNo.UpperCase();

	//get finger print
	TBuf8<40> fingerPrint;
	TPtrC8 fingerPtr = aCertificate->Fingerprint();
	
	//convert fingerprint to plain text
	for ( TInt i(0); i < fingerPtr.Length(); i++)
	{
		fingerPrint.AppendNumFixedWidth( fingerPtr[i], EHex, 2);
	}
	
	fingerPrint.UpperCase();
	aInfo->iFingerPrint = fingerPrint.AllocL();

	//joint issuer DN and Serial number (issuer DN + serial number + fingerprint + two delimeters= CASN) 
	ca = ca->ReAllocL( ca->Length() + serialNo.Length() + fingerPrint.Length() + 2);
	TPtr8 caPtr = ca->Des();
	caPtr.Append( serialNo);
	caPtr.Append( KCASNDelimeter);
	caPtr.Append( fingerPrint);
	
	aInfo->iCASN = ca;
	
	RDEBUG8_2("PolicyEngineServer: Certificate fingerprint: %S", aInfo->iFingerPrint);
	RDEBUG8_2("PolicyEngineServer: Certificate CASN: %S", ca);

}

// -----------------------------------------------------------------------------
// CCertificateMaps::ParseDNL()
// -----------------------------------------------------------------------------
//
	
HBufC8* CCertificateMaps::ParseDNL( const CX500DistinguishedName * aDN, TBool aOnlyCommonName)
{
	// DN country string
	TBuf8<4> country;
	// DN organisation unit
	TBuf8<64> organizationUnit;
	// DN orginasation
	TBuf8<64> organization;
	// DN common name
	TBuf8<64> commonName;
	// DN locality
	TBuf8<128> locality;	

	TDes8*	destination=NULL;
	TBool	found = EFalse;

	for ( TInt i(0); i < aDN->Count(); i++)
	{
		found = EFalse;
		const CX520AttributeTypeAndValue& attribute = aDN->Element(i);
	
		found = ETrue;
		if (!attribute.Type().Compare(KX520CountryName))
		{
			destination = &country;
		}
		else if(!attribute.Type().Compare(KX520OrganizationalUnitName))
		{
			destination = &organizationUnit;
		} 
		else if(!attribute.Type().Compare(KX520OrganizationName))
		{
			destination = &organization;
		}
		else if(!attribute.Type().Compare(KX520CommonName))
		{
			destination = &commonName;
		}
		else if(!attribute.Type().Compare(KX520LocalityName))
		{
			destination = &locality;
		}
		else
		{
			found = EFalse;
		}

		if ( found )
		{
			HBufC* value = attribute.ValueL();
			if ( value )
			{
				destination->Copy( *value);
			}
			
			delete value;
		}
	}
	
	HBufC8* retVal = NULL;
	
	if ( aOnlyCommonName)
	{
		//parse common name
		if ( commonName.Length())
		{
			retVal = commonName.AllocL();
		}
	}
	else
	{
		//create buffer for CA part
		TInt size = commonName.Length() + organization.Length() + organizationUnit.Length() + country.Length();
		retVal = HBufC8::NewL( size + 4);
		TPtr8 caPtr = retVal->Des();

		if ( commonName.Length())
		{
			caPtr.Append( commonName);
			caPtr.Append( KCASNDelimeter);
		}

		if ( organization.Length())
		{
			caPtr.Append( organization);
			caPtr.Append( KCASNDelimeter);
		}

		if ( organizationUnit.Length())
		{
			caPtr.Append( organizationUnit);
			caPtr.Append( KCASNDelimeter);
		}

		if ( country.Length())
		{
			caPtr.Append( country);
			caPtr.Append( KCASNDelimeter);
		}
	}
	
	return retVal;
}
	
	
// -----------------------------------------------------------------------------
// CCertificateMaps::ParseDNL()
// -----------------------------------------------------------------------------
//
TBool CCertificateMaps::EvaluateElementValidity( const TBool aUseEditedElements, const TElementState aState)
{
	if ( aUseEditedElements)
	{	
		//accept only elements which are committed...
		if ( aState == EDepricated || aState == EDeletedEditableElement)
		{
			return EFalse;
		}	
	}
	else
	{
		//ignore removed and depricated elements if not committed elements are in use
		if ( aState == EEditedElement ||  aState == EDeletedEditableElement || aState == EDepricated || aState == ENewElement)
		{
			return EFalse;
		}
	}	
	
	return ETrue;
}

	
