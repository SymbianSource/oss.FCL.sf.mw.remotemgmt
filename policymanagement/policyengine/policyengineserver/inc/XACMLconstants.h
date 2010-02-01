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


#ifndef XACML_CONSTANTS_HEADER__
#define XACML_CONSTANTS_HEADER__

#include "PolicyEngineXACML.h"


namespace PolicyLanguage
{
	namespace Constants
	{
		//namespace attributes
//		_LIT8( NamespaceAttr,						"target-namespace");
//		_LIT8( CertificateAttr,						"certificate_attr");
		_LIT8( ValidCertificateAttr,				"valid_certificate");
		_LIT8( PolicyTargetAttr,					"policy_target");
		_LIT8( TargetTrustedSubject,				"target_trusted_subject");
	
		_LIT8( MetaPolicySetNamespace,				"meta_policy_set_namespace");
		_LIT8( SettingSetNamespace,					"setting_set_namespace");
	
		_LIT8( SettingControlNamespace,				"setting_control_namespace");
	
		_LIT8( CorporateMetaPolicy,					"corporate_meta_policy");


		_LIT8( AddPolicy,							"add");
		_LIT8( RemovePolicy,						"remove");
		_LIT8( ReplacePolicy,						"replace");

		//Pre-defined policies and policy sets
		_LIT8( RootElement,							"root_element");
		_LIT8( RolesMappingPolicy,					"roles_mapping_policy");
		_LIT8( CertMappingPolicy,					"cert_mapping_policy");
		_LIT8( AutoCertificate,						"auto_certificate");
		const TInt AutoCertificateLength			= 16;
		
		//Pre-defined policies for setting enforcement
		_LIT8( EMailEnforcementPolicy,				"email_enforcement_policy");		
		_LIT8( EWLANEnforcementPolicy,				"wlan_enforcement_policy");		
		_LIT8( EAPEnforcementPolicy,				"ap_enforcement_policy");		
		_LIT8( EDataSyncEnforcementPolicy,			"datasync_enforcement_policy");		
		_LIT8( ESyncMLEnforcementPolicy,			"syncml_enforcement_policy");		
		_LIT8( EIMEnforcementPolicy,				"im_enforcement_policy");		
		_LIT8( EDCMOEnforcementPolicy,				"dcmo_enforcement_policy");		

		//values used in RuleTargetCheck functions
		_LIT8( Any,									"any");			
		_LIT8( Subject,								"subject");			
		_LIT8( Resource,							"resource");			
		_LIT8( Action,								"action");			
		_LIT8( Environment,							"environment");			
		
	}
	
	namespace XACML
	{	
		_LIT8(CharacterSetUTF8,							"UTF-8");


	

		namespace AttributeDesignators
		{	
		_LIT8(SubjectAttributeDesignator,		"SubjectAttributeDesignator"); 
		_LIT8(ActionAttributeDesignator,		"ActionAttributeDesignator"); 
		_LIT8(ResourceAttributeDesignator,		"ResourceAttributeDesignator"); 
		_LIT8(EnvironmentAttributeDesignator,	"EnvironmentAttributeDesignator"); 

		_LIT8(AttributeId,						"AttributeId"); 
		_LIT8(DataType,							"DataType"); 
		}

		namespace AttributeValues
		{	
		_LIT8(AttributeValue,					"AttributeValue");
		_LIT8(DataType,							"DataType"); 
		}
	
		namespace MatchObject
		{	
		_LIT8(SubjectMatch,						"SubjectMatch");
		_LIT8(ActionMatch,						"ActionMatch");
		_LIT8(EnvironmentMatch,					"EnvironmentMatch");
		_LIT8(ResourceMatch,					"ResourceMatch");
		_LIT8(MatchId,							"MatchId");
		}	
		
		namespace MatchContainers
		{
		_LIT8(Subject,							"Subject");
		_LIT8(Subjects,							"Subjects");
		_LIT8(Action,							"Action");
		_LIT8(Actions,							"Actions");
		_LIT8(Environment,						"Environment");
		_LIT8(Environments,						"Environments");
		_LIT8(Resource,							"Resource");	
		_LIT8(Resources,						"Resources");	
		}
		
		namespace Rule
		{
		_LIT8(Rule,								"Rule");
		_LIT8(RuleId,							"RuleId");
		_LIT8(Effect,							"Effect");
		_LIT8(Permit,							"Permit");
		_LIT8(Deny,								"Deny");
		
		_LIT8(Target,							"Target");
		}
		
		namespace Policy
		{
		_LIT8(Policy,							"Policy");
		_LIT8(PolicyId,							"PolicyId");
		_LIT8(RuleCombiningAlgId,				"RuleCombiningAlgId");
		}
		
		namespace PolicySet
		{
		_LIT8(PolicySet,						"PolicySet");
		_LIT8(PolicySetId,						"PolicySetId");
		_LIT8(PolicyCombiningAlgId,				"PolicyCombiningAlgId");
		}
		
		namespace Expressions
		{
		_LIT8(Apply,							"Apply");
		_LIT8(Condition,						"Condition");
		_LIT8(FunctionId,						"FunctionId");
		}
		
		namespace Functions
		{
			_LIT8(TrustedRoleMatch,				"urn:nokia:names:s60:corporate:1.0:function:TrustedRoleMatch");
			_LIT8(TrustedSubjectMatch,			"urn:nokia:names:s60:corporate:1.0:function:TrustedSubjectMatch");
												 
			_LIT8(FunctionStringEqualId,		"urn:oasis:names:tc:xacml:1.0:function:string-equal");
			_LIT8(FunctionStringNotEqualId,		"urn:oasis:names:tc:xacml:1.0:function:string-not-equal");
			_LIT8(FunctionBooleanEqualId,		"urn:oasis:names:tc:xacml:1.0:function:boolean-equal");

			_LIT8(FunctionOr,					"urn:oasis:names:tc:xacml:1.0:function:or");
			_LIT8(FunctionAnd,					"urn:oasis:names:tc:xacml:1.0:function:and");
			_LIT8(FunctionNot,					"urn:oasis:names:tc:xacml:1.0:function:not");

			_LIT8(FunctionCertificatedSession,	"urn:nokia:names:s60:corporate:1.0:function:CertificatedSession");
			_LIT8(FunctionUserAcceptCorpPolicy, "urn:nokia:names:s60:corporate:1.0:function:user-accepts-initial-trustedadmin-policy");
			_LIT8(FunctionRuleTargetStructure, 	"urn:nokia:names:s60:corporate:1.0:function:rule-target-structure");

		}
		
		namespace CombiningAlgorithms
		{
			_LIT8(RuleDenyOverrides,			"urn:oasis:names:tc:xacml:1.0:rule-combining-algorithm:deny-overrides");
			_LIT8(PolicyDenyOverrides,			"urn:oasis:names:tc:xacml:1.0:policy-combining-algorithm:deny-overrides");
			_LIT8(RulePermitOverrides,			"urn:oasis:names:tc:xacml:1.0:rule-combining-algorithm:permit-overrides");
			_LIT8(PolicyPermitOverrides,		"urn:oasis:names:tc:xacml:1.0:policy-combining-algorithm:permit-overrides");
		}
		
		namespace Description
		{
			_LIT8(Description,					"Description");
		}
	}

	namespace NativeLanguage
	{	
		namespace AttributeDesignators
		{	
		_LIT8(SubjectAttributeDesignator,		"sa"); 
		_LIT8(ActionAttributeDesignator,		"aa"); 
		_LIT8(ResourceAttributeDesignator,		"ra"); 
		_LIT8(EnvironmentAttributeDesignator,	"ea"); 

		_LIT8(AttributeId,						"aid"); 
		_LIT8(DataType,							"dt"); 
		}

		namespace AttributeValues
		{	
		_LIT8(AttributeValue,					"av");
		_LIT8(DataType,							"dt"); 
		
		_LIT8(StringDataType,					"str");
		_LIT8(BooleanDataType,					"boo");
		_LIT8(BooleanTrue,						"tru");
		_LIT8(BooleanFalse,						"fal");
		}
	
		namespace MatchObject
		{	
		_LIT8(SubjectMatch,						"sm");
		_LIT8(ActionMatch,						"am");
		_LIT8(EnvironmentMatch,					"em");
		_LIT8(ResourceMatch,					"rm");
		_LIT8(MatchId,							"mid");
		}
		
		
		namespace MatchContainers
		{
		_LIT8(Subject,							"su");
		_LIT8(Subjects,							"sus");
		_LIT8(Action,							"ac");
		_LIT8(Actions,							"acs");
		_LIT8(Environment,						"en");
		_LIT8(Environments,						"ens");
		_LIT8(Resource,							"re");	
		_LIT8(Resources,						"res");		
		}

		namespace Rule
		{
		_LIT8(Rule,								"ru");
		_LIT8(RuleId,							"ruid");
		_LIT8(Effect,							"ef");
		_LIT8(Permit,							"pe");
		_LIT8(Deny,								"de");
		
		_LIT8(Target,							"ta");
		}
		
		namespace Policy
		{
		_LIT8(Policy,							"po");
		_LIT8(PolicyId,							"poid");
		_LIT8(RuleCombiningAlgId,				"rcaid");
		}

		namespace PolicySet
		{
		_LIT8(PolicySet,						"ps");
		_LIT8(PolicySetId,						"psid");
		_LIT8(PolicyCombiningAlgId,				"pcid");
		}

		namespace Expressions
		{
		_LIT8(Apply,							"app");
		_LIT8(FunctionId,						"fid");
		_LIT8(Condition,						"con");
		}


		namespace Functions
		{
			_LIT8(TrustedRoleMatch,				"trm");
			_LIT8(TrustedSubjectMatch,			"tsm");

			_LIT8(FunctionStringEqualId,		"seid");
			_LIT8(FunctionStringNotEqualId,		"sneid");
			_LIT8(FunctionBooleanEqualId,		"beid");

			_LIT8(FunctionOr,					"orid");
			_LIT8(FunctionAnd,					"andid");
			_LIT8(FunctionNot,					"notid");

			_LIT8(FunctionCertificatedSession,	"csid");
			_LIT8(FunctionUserAcceptCorpPolicy, "uaicpid");
			_LIT8(FunctionRuleTargetStructure, 	"rtsid");
		}
		
		namespace CombiningAlgorithms
		{
			_LIT8(RuleDenyOverrides,			"rdor");
			_LIT8(PolicyDenyOverrides,			"pdor");
			_LIT8(RulePermitOverrides,			"rpor");
			_LIT8(PolicyPermitOverrides,		"ppor");
		}			
		
		namespace Description
		{
			_LIT8(Description,					"desc");
		}								
	}	

	namespace AttributeDesignators
	{
	const TPtrC8 SubjectAttributeDesignator[2] = 
	{ XACML::AttributeDesignators::SubjectAttributeDesignator(), NativeLanguage::AttributeDesignators::SubjectAttributeDesignator()};
	const TPtrC8 ActionAttributeDesignator[2] = 
	{ XACML::AttributeDesignators::ActionAttributeDesignator(), NativeLanguage::AttributeDesignators::ActionAttributeDesignator()};
	const TPtrC8 ResourceAttributeDesignator[2] = 
	{ XACML::AttributeDesignators::ResourceAttributeDesignator(), NativeLanguage::AttributeDesignators::ResourceAttributeDesignator()};
	const TPtrC8 EnvironmentAttributeDesignator[2] = 
	{ XACML::AttributeDesignators::EnvironmentAttributeDesignator(), NativeLanguage::AttributeDesignators::EnvironmentAttributeDesignator()};
	const TPtrC8 AttributeId[2] = 
	{ XACML::AttributeDesignators::AttributeId(), NativeLanguage::AttributeDesignators::AttributeId()};
	const TPtrC8 DataType[2] = 
	{ XACML::AttributeDesignators::DataType(), NativeLanguage::AttributeDesignators::DataType()};
	}
	
	namespace AttributeValues
	{	
	const TPtrC8 AttributeValue[2] = 
	{ XACML::AttributeValues::AttributeValue(), NativeLanguage::AttributeValues::AttributeValue()};
	const TPtrC8 DataType[2] = 
	{ XACML::AttributeValues::DataType(), NativeLanguage::AttributeValues::DataType()};
	const TPtrC8 StringDataType[2] = 
	{ PolicyEngineXACML::KStringDataType(), NativeLanguage::AttributeValues::StringDataType()};
	const TPtrC8 BooleanDataType[2] = 
	{ PolicyEngineXACML::KBooleanDataType(), NativeLanguage::AttributeValues::BooleanDataType()};
	const TPtrC8 BooleanTrue[2] = 
	{ PolicyEngineXACML::KBooleanTrue(), NativeLanguage::AttributeValues::BooleanTrue()};
	const TPtrC8 BooleanFalse[2] = 
	{ PolicyEngineXACML::KBooleanFalse(), NativeLanguage::AttributeValues::BooleanFalse()};
	}

	namespace MatchObject
	{	
	const TPtrC8 SubjectMatch[2] = 
	{ XACML::MatchObject::SubjectMatch(), NativeLanguage::MatchObject::SubjectMatch()};
	const TPtrC8 ActionMatch[2] = 
	{ XACML::MatchObject::ActionMatch(), NativeLanguage::MatchObject::ActionMatch()};
	const TPtrC8 EnvironmentMatch[2] = 
	{ XACML::MatchObject::EnvironmentMatch(), NativeLanguage::MatchObject::EnvironmentMatch()};
	const TPtrC8 ResourceMatch[2] = 
	{ XACML::MatchObject::ResourceMatch(), NativeLanguage::MatchObject::ResourceMatch()};
	const TPtrC8 MatchId[2] = 
	{ XACML::MatchObject::MatchId(), NativeLanguage::MatchObject::MatchId()};
	}
	
	namespace MatchContainers
	{
	const TPtrC8 Subject[2] = 
	{ XACML::MatchContainers::Subject(), NativeLanguage::MatchContainers::Subject()};
	const TPtrC8 Subjects[2] = 
	{ XACML::MatchContainers::Subjects(), NativeLanguage::MatchContainers::Subjects()};
	const TPtrC8 Action[2] = 
	{ XACML::MatchContainers::Action(), NativeLanguage::MatchContainers::Action()};
	const TPtrC8 Actions[2] = 
	{ XACML::MatchContainers::Actions(), NativeLanguage::MatchContainers::Actions()};
	const TPtrC8 Environment[2] = 
	{ XACML::MatchContainers::Environment(), NativeLanguage::MatchContainers::Environment()};
	const TPtrC8 Environments[2] = 
	{ XACML::MatchContainers::Environments(), NativeLanguage::MatchContainers::Environments()};
	const TPtrC8 Resource[2] = 
	{ XACML::MatchContainers::Resource(), NativeLanguage::MatchContainers::Resource()};
	const TPtrC8 Resources[2] = 
	{ XACML::MatchContainers::Resources(), NativeLanguage::MatchContainers::Resources()};
	}	

	namespace Rule
	{
	const TPtrC8 Rule[2] = 
	{ XACML::Rule::Rule(), NativeLanguage::Rule::Rule()};
	const TPtrC8 RuleId[2] = 
	{ XACML::Rule::RuleId(), NativeLanguage::Rule::RuleId()};
	const TPtrC8 Effect[2] = 
	{ XACML::Rule::Effect(), NativeLanguage::Rule::Effect()};
	const TPtrC8 Permit[2] = 
	{ XACML::Rule::Permit(), NativeLanguage::Rule::Permit()};
	const TPtrC8 Deny[2] = 
	{ XACML::Rule::Deny(), NativeLanguage::Rule::Deny()};
	const TPtrC8 Target[2] = 
	{ XACML::Rule::Target(), NativeLanguage::Rule::Target()};
	}

	namespace Policy
	{
	const TPtrC8 Policy[2] = 
	{ XACML::Policy::Policy(), NativeLanguage::Policy::Policy()};
	const TPtrC8 PolicyId[2] = 
	{ XACML::Policy::PolicyId(), NativeLanguage::Policy::PolicyId()};
	const TPtrC8 RuleCombiningAlgId[2] = 
	{ XACML::Policy::RuleCombiningAlgId(), NativeLanguage::Policy::RuleCombiningAlgId()};
	}

	namespace PolicySet
	{
	const TPtrC8 PolicySet[2] = 
	{ XACML::PolicySet::PolicySet(), NativeLanguage::PolicySet::PolicySet()};
	const TPtrC8 PolicySetId[2] = 
	{ XACML::PolicySet::PolicySetId(), NativeLanguage::PolicySet::PolicySetId()};
	const TPtrC8 PolicyCombiningAlgId[2] = 
	{ XACML::PolicySet::PolicyCombiningAlgId(), NativeLanguage::PolicySet::PolicyCombiningAlgId()};
	}

	namespace Expressions
	{
	const TPtrC8 Apply[2] = 
	{ XACML::Expressions::Apply(), NativeLanguage::Expressions::Apply()};
	const TPtrC8 Condition[2] = 
	{ XACML::Expressions::Condition(), NativeLanguage::Expressions::Condition()};
	const TPtrC8 FunctionId[2] = 
	{ XACML::Expressions::FunctionId(), NativeLanguage::Expressions::FunctionId()};
	}

	namespace Functions
	{
		const TPtrC8 TrustedRoleMatch[2] = 
		{ XACML::Functions::TrustedRoleMatch(), NativeLanguage::Functions::TrustedRoleMatch()};
		const TPtrC8 TrustedSubjectMatch[2] = 
		{ XACML::Functions::TrustedSubjectMatch(), NativeLanguage::Functions::TrustedSubjectMatch()};
		const TPtrC8 FunctionStringEqualId[2] = 
		{ XACML::Functions::FunctionStringEqualId(), NativeLanguage::Functions::FunctionStringEqualId()};
		const TPtrC8 FunctionStringNotEqualId[2] = 
		{ XACML::Functions::FunctionStringNotEqualId(), NativeLanguage::Functions::FunctionStringNotEqualId()};
		const TPtrC8 FunctionBooleanEqualId[2] = 
		{ XACML::Functions::FunctionBooleanEqualId(), NativeLanguage::Functions::FunctionBooleanEqualId()};
	
		const TPtrC8 FunctionOr[2] = 
		{ XACML::Functions::FunctionOr(), NativeLanguage::Functions::FunctionOr()};
		const TPtrC8 FunctionAnd[2] = 
		{ XACML::Functions::FunctionAnd(), NativeLanguage::Functions::FunctionAnd()};
		const TPtrC8 FunctionNot[2] = 
		{ XACML::Functions::FunctionNot(), NativeLanguage::Functions::FunctionNot()};


		const TPtrC8 FunctionCertificatedSession[2] = 
		{ XACML::Functions::FunctionCertificatedSession(), NativeLanguage::Functions::FunctionCertificatedSession()};
		const TPtrC8 FunctionUserAcceptCorpPolicy[2] = 
		{ XACML::Functions::FunctionUserAcceptCorpPolicy(), NativeLanguage::Functions::FunctionUserAcceptCorpPolicy()};
		const TPtrC8 FunctionRuleTargetStructure[2] = 
		{ XACML::Functions::FunctionRuleTargetStructure(), NativeLanguage::Functions::FunctionRuleTargetStructure()};
	}
	
	
	namespace CombiningAlgorithms
	{
		const TPtrC8 RuleDenyOverrides[2] = 
		{ XACML::CombiningAlgorithms::RuleDenyOverrides(), NativeLanguage::CombiningAlgorithms::RuleDenyOverrides	()};
		const TPtrC8 PolicyDenyOverrides[2] = 
		{ XACML::CombiningAlgorithms::PolicyDenyOverrides(), NativeLanguage::CombiningAlgorithms::PolicyDenyOverrides ()};
		const TPtrC8 RulePermitOverrides[2] = 
		{ XACML::CombiningAlgorithms::RulePermitOverrides(), NativeLanguage::CombiningAlgorithms::RulePermitOverrides	()};
		const TPtrC8 PolicyPermitOverrides[2] = 
		{ XACML::CombiningAlgorithms::PolicyPermitOverrides(), NativeLanguage::CombiningAlgorithms::PolicyPermitOverrides ()};
	}
	
	namespace Description
	{
		const TPtrC8 Description[2] = 
		{ XACML::Description::Description(), NativeLanguage::Description::Description()};
	}

}




#endif