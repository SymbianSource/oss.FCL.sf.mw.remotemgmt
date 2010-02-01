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
* Description:  Define constant values for XACML elements which are used from client side
*
*/


#ifndef POLICY_ENGINE_XACML_HEADER__
#define POLICY_ENGINE_XACML_HEADER__

namespace PolicyEngineXACML
{
	//Attributes
	_LIT8( KActionId,							"urn:oasis:names:tc:xacml:1.0:action:action-id");
	_LIT8( KSubjectId,							"urn:oasis:names:tc:xacml:1.0:subject:subject-id");
	_LIT8( KResourceId,							"urn:oasis:names:tc:xacml:1.0:resource:resource-id");
	_LIT8( KTrustedSubject,						"trusted_subject");
	
	//Certifiacte mapping attribute ids
	_LIT8( KCertificate,						"certificate_id");	
	_LIT8( KCertificateMapped,					"mapped_certificate_id");	
	_LIT8( KAliasId,							"alias_id");
	_LIT8( KRoleId,								"urn:nokia:names:s60:corporate:1.0:subject:role_id");						
	
	//Predefined datatypes and values
	_LIT8( KStringDataType,						"http://www.w3.org/2001/XMLSchema#string");
	_LIT8( KBooleanTrue,						"http://www.w3.org/2001/XMLSchema#true");
	_LIT8( KBooleanFalse,						"http://www.w3.org/2001/XMLSchema#false");
	_LIT8( KBooleanDataType,					"http://www.w3.org/2001/XMLSchema#boolean");	

	//Prefefined roles
	_LIT8( KRoleCorporate,						"trustedadmin");

	//predefined resources
	_LIT8( KCustomizationManagement,			"CustomizationManagement");
	_LIT8( KApplicationManagement,				"ApplicationManagement");
	_LIT8( KTerminalSecurityManagement,			"TerminalSecurityManagement");
	_LIT8( KTerminalSecurityAPIAccess,			"TerminalSecurityAPIAccess");
	_LIT8( KEMailEnforcement,					"EMailEnforcement");
	_LIT8( KSyncMLEnforcement,					"SyncMLEnforcement");
	_LIT8( KDataSyncEnforcement,				"DataSyncEnforcement");
	_LIT8( KAccessPointEnforcement,				"AccessPointEnforcement");
	_LIT8( KWLANEnforcement,					"WLANEnforcement");
	_LIT8( KInstantMessagingEnforcemnt,			"InstantMessagingEnforcemnt");
	_LIT8( KThirdPartySecureIds, 				"ThirdPartySecureIds");
	_LIT8( KDeviceLockPasscodeVisibility,		"DeviceLockPasscodeVisibility");
	_LIT8( KDCMOEnforcement,					"DCMOEnforcement");
		
}

#endif