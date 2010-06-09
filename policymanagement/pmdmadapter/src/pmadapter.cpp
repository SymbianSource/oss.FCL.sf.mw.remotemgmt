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
* Description:  Policy Management DM Adapter
*
*/


/*

./PolicyMgmt ---+--> PolicySets -+-> <X> * -+-> Policies ----> <X> * --+--> Policy
				|				 			|
				|				 			|-> PolicySets --> <X> * --+--> PolicySet
				|				 			|		
				|				 			|-> ID
				|							|				 								
				|							|-> Data
				|							|
				|				 			|-> Description
				|
				|--> Policies  -+-> <X> * --+-> ID
				|							|				 	
				|							|-> Data
				|							|
				|				 			|-> Description							
				|				 			|
				|				 			|-> Rules --> <X> * --+--> Rule
				|							 	
				|--> Rules  -+----> <X> * --+-> ID
				|							|				 	
				|							|-> Data
				|							|
				|				 			|-> Description							
				|							
				|--> Delivery -+-> Sink 
							   |
				               |--> LastStatus							 			
				
./PolicyMgmt/PolicySets/DynamicSets/X 							mapping contains the ElementID
./PolicyMgmt/PolicySets/DynamicSets/X/Policies/X 				mapping contains itself
./PolicyMgmt/PolicySets/DynamicSets/X/Policies/X/Policy 		mapping contains the data (URL to policy)
./PolicyMgmt/PolicySets/DynamicSets/X/PolicySets/X 				mapping contains itself
./PolicyMgmt/PolicySets/DynamicSets/X/PolicySets/X/PolicySet 	mapping contains the data (URL to policy set)
./PolicyMgmt/Policies/X 										mapping contains the ElementID


./PolicyMgmt/PolicySets/MetaSet 								mapping contains the data (URL to policy set)								
./PolicyMgmt/PolicySets/Settings 								mapping contains the data (URL to policy set)
*/

  
#include <badesca.h>
#include <fbs.h>
#include <tconvbase64.h>
#include "nsmldmuri.h"
#include "pmadapter.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <f32file.h>

#include <centralrepository.h>
#include "PMUtilInternalCRKeys.h"

#ifdef __TARM_SYMBIAN_CONVERGENCY
#include <dmtreenode.h>
#include <devman.h>
#include <e32property.h>
_LIT8( KSinkLeaf, "/PolicyMgmt/Delivery/Sink" ); 
const TUid KUidSmlSyncAgentCategory = { 0x10009F46 }; // temporary
const TUint KHttpsServerCertificateKey = 1234; // temporary
#else
_LIT8( KSinkLeaf, "PolicyMgmt/Delivery/Sink" ); 
#endif
#include "debug.h"

//#define __TEST_NO_ENGINE__ // causes Policy engine errors to be ignored

_LIT( KTestFileName, "c:\\test.txt" );

const TInt KFeatchLeafObjectBufferSize = 32;
const TInt KFeatchChildUriListBufferSize = 128;
const TInt KFeatchLeafObjectSize = 128;
const TInt KMaxLengthOf32bitInteger = 10; //10 = max length of 32bit integer

#define __LO_TEST__		// Large object test node "TEST"

// ------------------------------------------------------------------------------------------------
// CPmAdapter* CPmAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CPmAdapter* CPmAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CPmAdapter::NewL(): begin");

	CPmAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop( self );
	RDEBUG("CPmAdapter::NewL(): end");
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter* CPmAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CPmAdapter* CPmAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	CPmAdapter* self = new ( ELeave ) CPmAdapter(aDmCallback);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::CPmAdapter()
// ------------------------------------------------------------------------------------------------
CPmAdapter::CPmAdapter(TAny* aEcomArguments) : CSmlDmAdapter(aEcomArguments)
	{
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::ConstructL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::ConstructL()
	{
	RDEBUG("CPmAdapter::ConstructL(): begin");
#ifndef __TEST_NO_ENGINE__
	RDEBUG("CPmAdapter::ConstructL()	-> connecting RPolicyEngine ... ");
	TInt err = iEngine.Connect();
	User::LeaveIfError( err );
	RDEBUG("CPmAdapter::ConstructL()	-> connecting RPolicyEngine ... DONE!");
	RDEBUG("CPmAdapter::ConstructL()	-> opening RPolicyManagement ... ");
	User::LeaveIfError( iManagement.Open( iEngine ) );
	RDEBUG("CPmAdapter::ConstructL()	-> opening RPolicyManagement ... DONE!");
	CheckAndAddCertL();
#else
#endif
	RDEBUG("CPmAdapter::ConstructL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::CheckAndAddCertL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::CheckAndAddCertL()
	{
	RDEBUG("CPmAdapter::CheckAndAddCertL()");
	TInt errx = KErrNone;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	// nothing	
#else	
	CRepository *re = NULL;
	TRAP( errx, re = CRepository::NewL ( KCRUidPolicyManagementUtilInternalKeys ) );
#endif	

	if (errx == KErrNone )
		{
		TCertInfo info ;
		TPckg<TCertInfo> pcert( info );
		TInt len( 0 );
		
#ifdef __TARM_SYMBIAN_CONVERGENCY
		errx = RProperty::Get( KUidSmlSyncAgentCategory, KHttpsServerCertificateKey, pcert );
		len = pcert.Length();
#else		
		errx = re->Get( KSyncMLSessionCertificate, pcert, len ) ;
#endif	
	
		if ( errx == KErrNone )
			{
			if ( len > 0 )
				{
				RDEBUG_3("CPmAdapter::CheckAndAddCertL() : Got reposotry key len %d: %S", pcert.Length(), &pcert );
			
				TInt err( iManagement.AddSessionTrust( info ) );
				if ( err == KErrNone )
					{
					RDEBUG("CPmAdapter::CheckAndAddCertL() : Trust added!" );
					}
				else
					{
					RDEBUG_2("CPmAdapter::CheckAndAddCertL() : WARNING Failed to add trust %d", err );
					}
				}
			else
				{
				RDEBUG("CPmAdapter::CheckAndAddCertL() : WARNING certificate length is 0 or less - it doesnt exist" );	
				}
			}
		else
			{
			if ( errx != KErrNotFound )
				{
				RDEBUG_2("CPmAdapter::CheckAndAddCertL() : ERROR Failed to get repository key %d", errx );
				}
			else
				{
				RDEBUG_2("CPmAdapter::CheckAndAddCertL() : WARNING certificate key not found: %d", errx );	
				}
			}
#ifdef __TARM_SYMBIAN_CONVERGENCY
		// nothing
#else				
		delete re ;
#endif		
		}
	else
		{
		RDEBUG_2("CPmAdapter::CheckAndAddCertL() : WARNING could not create repository: %d", errx );	
		}
	}
	
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::~CPmAdapter()
// ------------------------------------------------------------------------------------------------
CPmAdapter::~CPmAdapter()
	{
	RDEBUG("CPmAdapter::~CPmAdapter()");
	delete iLastStatus ;
	iManagement.Close();
	iEngine.Close();
	iAtomiced.ResetAndDestroy();
	}

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::DDFVersionL( CBufBase& aDDFVersion )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	aDDFVersion.InsertL( 0, KNSmlPMDDFVersion );
	}


// ------------------------------------------------------------------------------------------------
//  CPmAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "CPmAdapter::DDFStructureL(): begin" );
	
	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();
	
	/*
	Node: ./PolicyMgmt
	Management object is recognized by node next to root named PolicyMgmt.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& pmNode = aDDF.AddChildObjectL( KNSmlPMNodeName );
	FillNodeInfoL(pmNode, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMDescription );
	
#ifdef __LO_TEST__
	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	/*
	Node: ./PolicyMgmt/TEST
	Management object is recognized by node next to root named PolicyMgmt.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get, Replace
	Values:  N/A
	*/
	MSmlDmDDFObject& testNode = pmNode.AddChildObjectL( KTestNodeName() );
	FillNodeInfoL( testNode, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
		MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMDescription );
#endif
		
	/*
	Node: ./PolicyMgmt/PolicySets
	This node is root for all policy sets. 
	The return value is as usual with Nodes (i.e. the list of names of child nodes in practice.)
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policySetsNode = pmNode.AddChildObjectL( KNSmlPMPolicySetsNodeName );
	FillNodeInfoL( policySetsNode, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPolicySetsNodeDescription );
	
	/*
	Node: ./PolicyMgmt/PolicySets/<X>
	This dynamic node is placeholder for policy sets. 
	Policy set consists of other policy set references, policy references, id, description and data. Add command requires policy set data (XACML) in data parameter.
	Support: Mandatory	
	Occurs: ZeroOrMore 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& dynamicSetsDynaNode = policySetsNode.AddChildObjectGroupL();
	FillNodeInfoL( dynamicSetsDynaNode, accessTypesGet, MSmlDmDDFObject::EZeroOrMore, 
		MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMDynamicSetsDynamicNodeDescription );
		
	/*
	Node: ./PolicyMgmt/PolicySets/<X>/Policies
	This node is root for policy references inside a policy set.
	Support: Mandatory
	Occurs: One 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& dynamicSetsPoliciesNode = 
		dynamicSetsDynaNode.AddChildObjectL( KNSmlPMPolicySetPoliciesNodeName );
	FillNodeInfoL( dynamicSetsPoliciesNode, accessTypesGet, MSmlDmDDFObject::EOne, 
		MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPolicySetPoliciesNodeDescription );

	/*
	Node: ./PolicyMgmt/PolicySets/<X>/Policies/<X>
	This dynamic node is placeholder for policy references.
	Support: Mandatory
	Occurs: ZeroOrMore 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& dynamicSetsPoliciesDynaNode = 
		dynamicSetsPoliciesNode.AddChildObjectGroupL();//( KNSmlPMDynamicNode() );
	FillNodeInfoL( dynamicSetsPoliciesDynaNode, accessTypesGet, 
		MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMPolicySetPoliciesDynamicNodeDescription );
		
	/*
	Node: ./PolicyMgmt/PolicySets/<X>/Policies/<X>/Policy
	This leaf holds a URI reference to actual policy in the management tree.
	Support: Mandatory
	Occurs: One 
	Format: Chr
	Access Types: Get
	Values:  Valid url to policy ids in PolicyMgmt/Policies/<X>/

		Example value refers to policy 3sdxA, which is random imaginary policy node name.
		./PolicyMgmt/Policies/3sdxA/
		
	*/
	MSmlDmDDFObject& dynamicSetsPoliciesPolicyNode = 
		dynamicSetsPoliciesDynaNode.AddChildObjectL( KNSmlPMPolicySetPoliciesPolicyNodeName );
	FillNodeInfoL( dynamicSetsPoliciesPolicyNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPolicySetPoliciesPolicyNodeDescription );

	/*
	Node: ./PolicyMgmt/PolicySets/<X>/PolicySets
	This node is root for policy set references inside a policy set.
	Support: Mandatory
	Occurs: One 
	Format: Node
	Access Types: Get
	Values:  N/A

	*/
	MSmlDmDDFObject& dynamicSetsPolicySetsNode = 
		dynamicSetsDynaNode.AddChildObjectL( KNSmlPMPolicySetPolicySetsNodeName );
	FillNodeInfoL( dynamicSetsPolicySetsNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPolicySetPolicySetsNodeDescription );

	/*
	Node: ./PolicyMgmt/PolicySets/<X>/PolicySets/<X>
	This dynamic node is placeholder for policy set references.
	Support: Mandatory
	Occurs: ZeroOrMore 
	Format: Node
	Access Types: Get
	Values:  N/A

	*/
	MSmlDmDDFObject& dynamicSetsPolicySetsDynaNode = 
		dynamicSetsPolicySetsNode.AddChildObjectGroupL();// KNSmlPMDynamicNode );
	FillNodeInfoL( dynamicSetsPolicySetsDynaNode, accessTypesGet, 
		MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMPolicySetPolicySetsDynamicNodeDescription );
		
	/*
	Node: ./PolicyMgmt/PolicySets/<X>/PolicySets/<X>/PolicySet
	This is URI of actual policy set in the management tree, i.e. PolicyMgmt/PolicySets/<X>.
	Support: Mandatory
	Occurs: One 
	Format: chr
	Access Types: Get
	Values:  Valid url to policy set ids in ./PolicyMgmt/PolicySets/<X>/

		Example value references to policy set SetX10:
		./PolicyMgmt/PolicySets/SetX10/
	*/
	MSmlDmDDFObject& dynamicSetsPolicySetsPolicySetNode = 
		dynamicSetsPolicySetsDynaNode.AddChildObjectL( 
			KNSmlPMPolicySetPolicySetsPolicySetNodeName );
	FillNodeInfoL( dynamicSetsPolicySetsPolicySetNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPolicySetPolicySetsPolicySetNodeDescription );

	/*
	Node: ./PolicyMgmt/PolicySets/<X>/ID
	ID of the policy set from the original XACML file. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& policySetIdNode = 
		dynamicSetsDynaNode.AddChildObjectL( KNSmlPMPolicySetIDNodeName );
	FillNodeInfoL( policySetIdNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPolicySetIDNodeDescription );

	/*
	Node: ./PolicyMgmt/PolicySets/<X>/Data

	Data of the policy set in XACML format.
	Support: Mandatory
	Occurs: One
	Format: Xml
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& policySetDataNode = 
		dynamicSetsDynaNode.AddChildObjectL( KNSmlPMPolicySetDataNodeName );
	FillNodeInfoL( policySetDataNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EXml, KNSmlPMPolicySetDataNodeDescription );
		
	/*
	Node: ./PolicyMgmt/PolicySets/<X>/Description

	Description of policy set from the original XACML file is reflected in this leaf.
	Support: Optional
	Occurs: One
	Format: Chr
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& policySetDescNode = 
		dynamicSetsDynaNode.AddChildObjectL( KNSmlPMPolicySetDescriptionNodeName );
	FillNodeInfoL( policySetDescNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPolicySetDescriptionNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies
	This node is root for all policies in system.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policiesNode = pmNode.AddChildObjectL( KNSmlPMPoliciesNodeName );
	FillNodeInfoL( policiesNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPoliciesNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>
	This dynamic node is placeholder for policies. Policy consists of id, description and data.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policiesDynaNode = policiesNode.AddChildObjectGroupL();// KNSmlPMDynamicNode );
	FillNodeInfoL( policiesDynaNode, accessTypesGet, 
		MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMPoliciesDynaNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>/ID
	This leaf node holds the ID of the policy.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policiesIDNode = 
		policiesDynaNode.AddChildObjectL( KNSmlPMPoliciesIDNodeName );
	FillNodeInfoL( policiesIDNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPoliciesIDNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>/Data
	This leaf node gives access to the data of the policy in XACML format.
	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policiesDataNode = 
		policiesDynaNode.AddChildObjectL( KNSmlPMPoliciesDataNodeName );
	FillNodeInfoL( policiesDataNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPoliciesDataNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>/Description
	Description of policy from the original XACML file is reflected in this leaf.
	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& policiesDescNode = 
		policiesDynaNode.AddChildObjectL( KNSmlPMPoliciesDescNodeName );
	FillNodeInfoL( policiesDescNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPoliciesDescNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>/Rules
	This node is root for rule references inside a policy.
	Support: Mandatory
	Occurs: One 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& policiesRulesNode = 
		policiesDynaNode.AddChildObjectL( KNSmlPMPoliciesRulesNodeName );
	FillNodeInfoL( policiesRulesNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPoliciesRulesNodeDescription );

	/*
	Node: ./PolicyMgmt/Policies/<X>/Rules/<X>
	This dynamic node is placeholder for rule references.
	Support: Mandatory
	Occurs: ZeroOrMore 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& policiesRulesDynaNode = 
		policiesRulesNode.AddChildObjectGroupL();// KNSmlPMDynamicNode() );
	FillNodeInfoL( policiesRulesDynaNode, accessTypesGet, 
		MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMPoliciesRulesDynaNodeDescription );
	
	/*
	Node: ./PolicyMgmt/Policies/<X>/Rules/<X>/Rule
	This leaf holds a URI reference to actual rule in the management tree.
	Status: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject&  policiesRuleRef = 
		policiesRulesDynaNode.AddChildObjectL( KNSmlPMPoliciesRulesRuleNodeName );
	FillNodeInfoL( policiesRuleRef, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMPoliciesRulesRuleNodeDescription );
	
	/*
	Node: ./PolicyMgmt/Rules
	This node is root for all rules in system.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& rulesNode = 
		pmNode.AddChildObjectL( KNSmlPMRulesNodeName );
	FillNodeInfoL( rulesNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMRulesNodeDescription );
	
	/*
	Node: ./PolicyMgmt/Rules/<X>
	This dynamic node is placeholder for rules. Rule consists of id, description and data. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& rulesDynaNode = 
		rulesNode.AddChildObjectGroupL();// KNSmlPMDynamicNode );
	FillNodeInfoL( rulesDynaNode, accessTypesGet, 
		MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlPMRulesDynaNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Policies/<X>/ID
	This leaf node holds the ID of the rules.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& rulesIDNode = 
		rulesDynaNode.AddChildObjectL( KNSmlPMRulesIDNodeName );
	FillNodeInfoL( rulesIDNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMRulesIDNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Rules/<X>/Data
	This leaf node gives access to the data of the rule in XACML format.
	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& rulesDataNode = 
		rulesDynaNode.AddChildObjectL( KNSmlPMRulesDataNodeName );
	FillNodeInfoL( rulesDataNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMRulesDataNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Rules/<X>/Description
	Description of rule from the original XACML file is reflected in this leaf.
	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& rulesDescNode = 
		rulesDynaNode.AddChildObjectL( KNSmlPMRulesDescNodeName );
	FillNodeInfoL( rulesDescNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMRulesDescNodeDescription );
		
	TSmlDmAccessTypes accessTypesExec;
	accessTypesExec.SetExec();
	
	/*
	Node: ./PolicyMgmt/Delivery
	This node holds the nodes that let DM Server to bring PolicyManagement 
	command packages to device and observe status of such action. 
	Support: Mandatory
	Occurs: One 
	Format: Node
	Access Types: Get
	Values:  N/A
	*/
	MSmlDmDDFObject& pdNode = 
		pmNode.AddChildObjectL( KNSmlPMPolicyDeliveryNodeName );
	FillNodeInfoL( pdNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlPMPolicyDeliveryNodeDescription );
	
	/*
	Node: PolicyMgmt/Delivery/Sink
	This node is used by DM Server to bring PolicyManagement 
	command packages to device. 
	The packages are delivered in XML.
	Support: Mandatory
	Occurs: One 
	Format: Xml
	Access Types: Exec
	Values:  N/A
	*/
	MSmlDmDDFObject& sinkNode = pdNode.AddChildObjectL( KNSmlPMDeliverySinkNodeName );
	FillNodeInfoL( sinkNode, accessTypesExec, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EXml, KNSmlPMDeliverySinkNodeDescription );
		
	/*
	Node: ./PolicyMgmt/Delivery/LastStatus
	This node stores status of last executed delivery command.
	
	Support: Mandatory
	Occurs: One 
	Format: chr
	Access Types: Get
	Values:  See below
	*/
	MSmlDmDDFObject& ldsNode = 
		pdNode.AddChildObjectL( KNSmlPMLastDeliveryStatusNodeName );
	FillNodeInfoL( ldsNode, accessTypesGet, 
		MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlPMLastDeliveryStatusNodeDescription );
	
	RDEBUG( "CPmAdapter::DDFStructureL(): end" );
	}

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, 
						const TDesC8& /*aObject*/, 
						const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG_2("CPmAdapter::UpdateLeafObjectL(): '%S' begin - WARNING No nodes with A or R access", &aURI );
	TError ret( EError );
	Callback().SetStatusL( aStatusRef,  ret );
	RDEBUG("CPmAdapter::UpdateLeafObjectL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef)
// ------------------------------------------------------------------------------------------------
void CPmAdapter::DeleteObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, TInt aStatusRef)
	{
	RDEBUG( "CPmAdapter::DeleteObjectL(): ERROR Delete NOT SUPPORTED" );
	TError ret( EError );
	Callback().SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC& aType, 
//						CBufBase& aObject, TInt aResultsRef, TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, 
						TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG8_3("CPmAdapter::FetchLeafObjectL('%S','%S'): begin", &aURI, &aLUID);
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
#endif
	
	TError ret( EOk );
	CBufBase* object = CBufFlat::NewL( KFeatchLeafObjectBufferSize );
	CleanupStack::PushL( object );
	switch ( cnt )
		{
		case 1:
		case 2: 
			{	
 			RDEBUG( "CPmAdapter::FetchLeafObjectL(): WARNING Nonexisting Level 1 or 2 leaf requested" );
			ret = EError ;	
#ifdef __LO_TEST__
			if ( mapping == KTestNodeName )
				{
				RFs fs ;
				User::LeaveIfError( fs.Connect() ) ;
				CleanupClosePushL( fs );
				RFile file ;
				User::LeaveIfError( file.Open( fs, KTestFileName, EFileRead ) );
				CleanupClosePushL( file );
				TInt size;
				User::LeaveIfError( file.Size( size ) );
				HBufC8 *data = HBufC8::NewLC( size );
				
				TPtr8 ptr( data->Des() );
				User::LeaveIfError( file.Read( ptr ) );
				TBase64 B64Coder;
				HBufC8 *target = HBufC8::NewLC( ( size * 6 ) / 3 );
				TPtr8 targetPtr( target->Des() );
				B64Coder.PortableEncode( *data, targetPtr ); 
				object->InsertL( 0, ptr );
				CleanupStack::PopAndDestroy( 3, &fs );
				ret = EOk;
				}
#endif
			break;
			}
		case 3:
			{
			if ( mapping == KNSmlPMLastDeliveryStatusNodeName )
 				{
 				object->InsertL( 0, ((iLastStatus == NULL) ? KNullDesC8() : *iLastStatus) );
 				}
 			else
 				{
				RDEBUG ( "CPmAdapter::FetchLeafObjectL(): WARNING Nonexisting Level 3 leaf requested" );
				ret = EError ;	
 				}
			break;
			}
		case 4:
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg ( uriPtrc ) );
#else				
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg ( aURI ) );
#endif	
			TPtrC8 parentMapping ( NSmlDmURI::LastURISeg( parent ) );
			GetElementInfoL( parentMapping, mapping, object, ret );		
			break;
			} 
		case 5:
			{
			RDEBUG ( "CPmAdapter::FetchLeafObjectL(): WARNING Nonexisting Level 5 leaf requested" );
			ret = EError ;
			break;
			} 
		case 6:
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg ( uriPtrc ) );
#else				
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg ( aURI ) );
#endif	
			TPtrC8 parentMapping ( NSmlDmURI::LastURISeg( parent ) );
			ret = EOk ;

			if ( mapping == KNSmlPMPolicySetPoliciesPolicyNodeName )
				{
				object->InsertL( 0, parentMapping ) ;
				}
			else if ( mapping == KNSmlPMPolicySetPolicySetsPolicySetNodeName )
				{
				object->InsertL( 0, parentMapping ) ;
				}
			else if ( mapping == KNSmlPMPoliciesRulesRuleNodeName )
				{
				object->InsertL( 0, parentMapping ) ;
				}
			else
				{
				ret = EError ;	
				}
			break;
			}	
		default: 
			{
			RDEBUG_2("CPmAdapter::FetchLeafObjectL(): WARNING Nonexisting Level %d leaf requested", cnt );
			ret = EError ;
			break;
			}
		}
	if ( ret == EOk )
		{
		Callback().SetResultsL( aResultsRef, *object, aType );
		}
	Callback().SetStatusL( aStatusRef, ret );
	CleanupStack::PopAndDestroy( object ) ; 
	RDEBUG("CPmAdapter::FetchLeafObjectL(): end");
}


// ------------------------------------------------------------------------------------------------
//  MSmlDmAdapter::TError CPmAdapter::InsertIfFoundL( const TDesC8 &aElementId, 
//					const TDesC8& aData, CBufBase &aObject )
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CPmAdapter::InsertIfFoundL( const TDesC8 &aElementId, const TDesC8& aData, 
	CBufBase &aObject )
	{
	RDEBUG8_3("CPmAdapter::InsertIfFoundL, elementID= %S, Data= %S", &aElementId, &aData );
	TError ret(EError);
	TElementInfo info( aElementId );
	TInt err( iManagement.GetElementL( info ) );
	if ( err == KErrNone ) 
		{
		aObject.InsertL( 0, aData );	
		ret = EOk;
		}
	else if ( err == KErrNotFound )
		{
		ret = ENotFound;
		}
	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::GetElementInfoL(const TDesC8 &aElementId, 
//					const TDesC8 &aType, CBufBase* aObject, TError &aRet)
// ------------------------------------------------------------------------------------------------
TInt CPmAdapter::GetElementInfoL(const TDesC8 &aElementId, 
		const TDesC8 &aType, CBufBase* aObject, TError &aRet)
	{
	RDEBUG8_2("CPmAdapter::GetElementInfoL, ElementID= %S", &aElementId );
	aRet = EError ;
	TElementInfo info( aElementId );
	TInt err( iManagement.GetElementL( info ) );
	if ( err == KErrNone ) 
		{
		if ( aType == KNSmlPMPolicySetIDNodeName )
			{
			aObject->InsertL( 0, info.GetElementId() );
			aRet = EOk;
			}
		else if ( aType == KNSmlPMPolicySetDataNodeName )
			{
			err = iManagement.GetXACMLDescriptionL( info );
			if ( err == KErrNone )
				{
				aObject->InsertL( 0, info.GetXACML() ) ;
				aRet = EOk;
				}
			else
				{
				RDEBUG_3("CPmAdapter::GetElementInfoL(): FAILED to get XACML element!! 0x%X (%d)", err, err );
				}
			} 		
		else if ( aType == KNSmlPMPolicySetDescriptionNodeName )
			{
			aObject->InsertL( 0, info.GetDescription() );
			aRet = EOk;
			}
		else
			{
			RDEBUG_2("CPmAdapter::GetElementInfoL(): ERROR Unknown attribute askedt!! '%S'", &aType );
			}
		}
	else
		{
		RDEBUG_2("CPmAdapter::GetElementInfoL(): GetElementL FAILED to get policy set element!! %d", err );
		}
	return err ;
	}					

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::GetElementAndAddToResponseL( const TDesC8 &aParentMapping, CBufBase &aObject, 
//					MSmlDmAdapter::TError &ret )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::GetElementAndAddToResponseL( const TDesC8 &aParentMapping, CBufBase &aObject, 
			MSmlDmAdapter::TError &ret )
	{
	RDEBUG("CPmAdapter::GetElementAndAddToResponseL");
	TElementInfo info( aParentMapping );
	TInt err( iManagement.GetElementL( info ) );
	if ( err == KErrNone ) 
		{
		ArrayToDMResponseL( info.GetChildElementArray(), aObject );
		}
	else
		{
		RDEBUG_3("CPmAdapter::GetElementAndAddToResponseL(): WARNING Failed get element %S: %d" , 
			&aParentMapping, err );
		ret = EError;
		}
	}

// ------------------------------------------------------------------------------------------------
//  CPmAdapter::ArrayToDMResponseL( const RElementIdArray &aArray, CBufBase &aObject)
// ------------------------------------------------------------------------------------------------
void CPmAdapter::ArrayToDMResponseL( const RElementIdArray &aArray, CBufBase &aObject)
	{
	RDEBUG("CPmAdapter::ArrayToDMResponseL");
	TInt count( aArray.Count() );
	for( TInt i( 0 ); i < count; i++ )
		{
		HBufC8 *el = aArray[i];
		aObject.InsertL( aObject.Size(), *el );
		if ( i + 1 < count )
			{
			aObject.InsertL( aObject.Size(), KNSmlPMSeparator8 );
			}
		}
	}
	
// ------------------------------------------------------------------------------------------------
//  CPmAdapter::ChildURIListL( const TDesC8& aURI, const TDesC& aParentLUID, 
//						const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
//						CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::ChildURIListL( const TDesC8& aURI, const TDesC8& /*aLUID*/, 
				const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
				TInt aResultsRef, TInt aStatusRef  )
	{
	RDEBUG8_2("CPmAdapter::ChildURIListL(): begin %S", &aURI);

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
#endif
	
	TError ret( EOk );		
	CBufBase *object = CBufFlat::NewL( KFeatchChildUriListBufferSize );
	CleanupStack::PushL( object );
	
	switch ( cnt )
		{
		case 1:
			{
			
			if ( NSmlDmURI::LastURISeg( aURI ) == KNSmlPMNodeName )
				{
				object->InsertL( 0, KNSmlPMListOfLeafs );
				}
			else 
				{
 				RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Level 1 invalid mapping " );
				ret = EInvalidObject ;
				}
			break;
			}
		case 2: 
			{
			
			TPtrC8 mapping ( NSmlDmURI::LastURISeg( aURI ) ) ;
			TElementType type ;
			TBool ok( ETrue ) ;
 			if ( mapping == KNSmlPMPolicySetsNodeName ) 
 				{
 				type = EPolicySets;
 				}
 			else if ( mapping == KNSmlPMPoliciesNodeName ) 
 				{
 				
 				type = EPolicies;
 				}
 			else if ( mapping == KNSmlPMRulesNodeName ) 
 				{
 				type = ERules;
 				}
 			else if ( mapping == KNSmlPMPolicyDeliveryNodeName )
 				{
 				object->InsertL( 0, KNullDesC8() ) ;
 				RDEBUG ( "CPmAdapter::ChildURIListL(): Asking policydelivery node childs" );
 				ok = EFalse;
 				}
 			else 
 				{
 				RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Level 2 invalid mapping " );
 				ok = EFalse;
 				ret = EInvalidObject;
 				}
 			if ( ok )
 				{		
 				RElementIdArray array;
				CleanupClosePushL( array );
				
				TInt err( iManagement.GetElementListL( type, array ) );
				if ( err == KErrNone )
					{	
					ArrayToDMResponseL( array, *object );
					ret = EOk;
					}
				else
					{				
					RDEBUG_2("CPmAdapter::ChildURIListL(): ERROR Policy engine error: %d", err) ;
					ret = EError;
					}
				CleanupStack::PopAndDestroy( &array );
 				}
			break;
			}
		case 3:
			{
			
			TPtrC8 mapping ( NSmlDmURI::LastURISeg ( NSmlDmURI::RemoveLastSeg( aURI ) ) ) ;
			if ( mapping == KNSmlPMPolicySetsNodeName )
				{
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMListOfDynamicSetsDynaLeafs, *object );
				}
			else if ( mapping == KNSmlPMPoliciesNodeName ) 
				{
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMListOfPoliciesDynaLeafs, *object );
				}
			else if ( mapping == KNSmlPMRulesNodeName ) 
				{
				
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMListOfRulesDynaLeafs, *object );
				}
			else if ( mapping == KNSmlPMLastDeliveryStatusNodeName )
 				{
 				
 				object->InsertL( 0, KNullDesC8() ) ;
 				RDEBUG ( "CPmAdapter::ChildURIListL(): Asking laststatus node childs" );
 				ret = EError;
 				}
 			else
				{
				RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Level 3 Unknown parent " );
				ret = EError ;	
				}
			break;
			}
		case 4:
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY
			TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
#else
			TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
#endif					

			TPtrC8 parentMapping ( NSmlDmURI::LastURISeg( parent ) );
			TPtrC8 grandParent ( NSmlDmURI::RemoveLastSeg( parent ) ) ;
			TPtrC8 grandParentMapping( NSmlDmURI::LastURISeg( grandParent ) );
			
			if ( grandParentMapping == KNSmlPMPolicySetsNodeName )
				{
				if ( mapping == KNSmlPMPolicySetPoliciesNodeName 
					|| mapping == KNSmlPMPolicySetPolicySetsNodeName)
					{
					GetElementAndAddToResponseL( parentMapping, *object, ret );
					}
				else 
					{
					RDEBUG("CPmAdapter::ChildURIListL(): WARNING Level 4 Asks children although there are none" );
					object->InsertL( 0, KNullDesC8 );
					}
				}
			else if ( grandParentMapping == KNSmlPMPoliciesNodeName ) 
				{
				if ( mapping == KNSmlPMPoliciesRulesNodeName )
					{
					GetElementAndAddToResponseL( parentMapping, *object, ret );
					}
				else
					{
					RDEBUG("CPmAdapter::ChildURIListL(): WARNING Level 4 Asks (Policies) leaf children although there are none");
					object->InsertL( 0, KNullDesC8 );
					}
				}
			else if ( grandParentMapping == KNSmlPMRulesNodeName ) 
				{
				RDEBUG("CPmAdapter::ChildURIListL(): WARNING Level 4 Asks Rules leaf children although there are none");
				object->InsertL( 0, KNullDesC8 );
				}
			else
				{
				RDEBUG("CPmAdapter::ChildURIListL(): WARNING Level 4 Unknown parent " );
				ret = EInvalidObject ;	
				}
			break;
			} 
		case 5:
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
#else
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
#endif					

			if ( parent == KNSmlPMPolicySetPoliciesNodeName )
				{
				
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMPolicySetPoliciesPolicyNodeName, *object );
				}
			else if ( parent == KNSmlPMPolicySetPolicySetsNodeName )
				{
				
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMPolicySetPolicySetsPolicySetNodeName, *object );
				}
			else if ( parent == KNSmlPMPoliciesRulesNodeName )
				{
				
				ret = InsertIfFoundL( NSmlDmURI::LastURISeg( aURI ), 
					KNSmlPMPoliciesRulesRuleNodeName, *object );
				}
			else
				{
				RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Level 5 Unkown parent " );
				ret = ENotFound;
				}
			break;
			} 
		case 6:
			{
			RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Level 6 " );
			ret = EInvalidObject ;
			break;
			}
		default: 
			{
			RDEBUG ( "CPmAdapter::ChildURIListL(): WARNING Unknown Level" );
			ret = EError ;
			}
		}
	
	Callback().SetStatusL( aStatusRef, ret );
	
	if( ret==CSmlDmAdapter::EOk )
		{
		Callback().SetResultsL( aResultsRef, *object, KNullDesC8 );
		}
	CleanupStack::PopAndDestroy( object ); 
	RDEBUG("CPmAdapter::ChildURIListL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
//							      const TDesC8& aArgument, const TDesC8& /*aType*/,
//								  TInt aStatusRef ) 
// ------------------------------------------------------------------------------------------------
void CPmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
							      const TDesC8& aArgument, const TDesC8& /*aType*/,
								  TInt aStatusRef ) 
	{
	RDEBUG8_2("CPmAdapter::ExecuteCommandL(): begin '%S'", &aURI );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
#endif

	CSmlDmAdapter::TError ret ( CSmlDmAdapter::EInvalidObject );

	if ( cnt == 3 ) 
		{		
		// SymbianConvergency, KSinkLeaf modified -> should be ok to use aURI directly...
		if ( aURI == KSinkLeaf )
			{
			TElementId id  ;
			TParserResponse resp ;
			
	#ifdef __TEST_NO_ENGINE__
			TInt err( KErrNone );
	#else
			TInt err( iManagement.ExecuteOperation( aArgument, resp ) );
	#endif

			if ( err == KErrNone )
				{
				RDEBUG8_2("		-> ExecuteOperation OK: %S", &resp.GetReturnMessage() );
				SetLastStatusL( resp.GetReturnMessage() );
				if ( iInAtomic )
					{
#ifdef __TARM_SYMBIAN_CONVERGENCY
					iAtomiced.AppendL( CPmCommand::NewL( aStatusRef, uriPtrc, aArgument ) );
#else						
					iAtomiced.AppendL( CPmCommand::NewL( aStatusRef, aURI, aArgument ) );
#endif					
					}
				ret = EOk ;
				RDEBUG8_2("CPmAdapter::ExecuteCommandL(): SUCCESS adding policy or set or rule!! '%S'", &resp.GetReturnMessage()  );
				}
			else
				{
				RDEBUG_2("			-> Execute operation NOT OK: %d", err );
				RDEBUG8_2("			-> Execute operation NOT OK: %S", &resp.GetReturnMessage() );
				SetLastStatusL( resp.GetReturnMessage() );
				RDEBUG_2("CPmAdapter::ExecuteCommandL(): FAILED adding policy or set or rule!! %d", err );
				ret = EError ;
				}
			}
		else
			{
			RDEBUG("CPmAdapter::ExecuteCommandL(): ERROR leaf!!" );
			ret = EError ;
			}
		}
	else
		{
		RDEBUG(" CPmAdapter::ExecuteCommandL(): ERROR unsupported command target!" );
		}
	Callback().SetStatusL( aStatusRef, ret);
	
	
	RDEBUG("CPmAdapter::ExecuteCommandL(): end" );
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
//								  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
//								  TInt /*aStatusRef*/ )	
// ------------------------------------------------------------------------------------------------
void CPmAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
								  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
								  TInt /*aStatusRef*/ )	
	{
	RDEBUG("CPmAdapter::ExecuteCommandL() (stream): begin ( not supported - leaving )");
	User::Leave( KErrNotSupported )	;
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
//								  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
//								  TInt /*aStatusRef*/ )	
// ------------------------------------------------------------------------------------------------
void CPmAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
									RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
		 							TInt /*aStatusRef*/ ) 
	{
	RDEBUG("CPmAdapter::UpdateLeafObjectL() (stream): begin ( not supported - leaving )");
	User::Leave( KErrNotSupported )	;
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/,
//							   const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/,
//							   const TDesC8& /*aType*/, TInt /*aStatusRef*/ )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/,
							   const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/,
							   const TDesC8& /*aType*/, TInt /*aStatusRef*/ )
	{
	RDEBUG("CPmAdapter::CopyCommandL(): begin ( not supported - leaving )");
	User::Leave( KErrNotSupported )	;
	}
				
// ------------------------------------------------------------------------------------------------
// CPmAdapter::StartAtomicL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::StartAtomicL()	
	{	
	RDEBUG("CPmAdapter::StartAtomicL(): begin");
	iInAtomic = ETrue ;
	RDEBUG("CPmAdapter::StartAtomicL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::CommitAtomicL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::CommitAtomicL()
	{	
	RDEBUG("CPmAdapter::CommitAtomicL(): begin");
	iInAtomic = EFalse ;
	RDEBUG("CPmAdapter::CommitAtomicL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::RollbackAtomicL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::RollbackAtomicL()
	{	
	RDEBUG("CPmAdapter::RollbackAtomicL(): begin");
	TInt count( iAtomiced.Count() );
	for ( TInt i(0); i < count; i++ )
		{
		CPmCommand *pm = iAtomiced[i];
		if( pm ) 
			{
			Callback().SetStatusL( pm->StatusRef(), ERollbackFailed );	
			}
		}
	RDEBUG("CPmAdapter::RollbackAtomicL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::StreamingSupport( TInt& /*aItemSize*/ )
// ------------------------------------------------------------------------------------------------
TBool CPmAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("CPmAdapter::StreamingSupport(): (no streaming support)");
	return EFalse; 	
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::StreamCommittedL()
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CPmAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CPmAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CPmAdapter::StreamCommittedL(): begin");
	RDEBUG("CPmAdapter::StreamCommittedL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::CompleteOutstandingCmdsL()
// ------------------------------------------------------------------------------------------------
void CPmAdapter::CompleteOutstandingCmdsL()	
	{	
	RDEBUG("CPmAdapter::CompleteOutstandingCmdsL(): begin");
	RDEBUG("CPmAdapter::CompleteOutstandingCmdsL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CPmAdapter::AddNodeObjectL( const TDesC8& aURI, const TDesC& aParentLUID, TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
void CPmAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aParentLUID*/, TInt aStatusRef )
	{
	RDEBUG("CPmAdapter::AddNodeObjectL(): begin");
	TError ret( EError );
	RDEBUG("CPmAdapter::AddNodeObjectL(): ERROR Add method not supported by any node or leaf!!");
	Callback().SetStatusL( aStatusRef, ret );
	RDEBUG("CPmAdapter::AddNodeObjectL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID,
//									   const TDesC8& aType, TInt aResultsRef,
//									   TInt aStatusRef ) 
// ------------------------------------------------------------------------------------------------
void CPmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
									   const TDesC8& aType, TInt aResultsRef,
									   TInt aStatusRef ) 
	{
	RDEBUG8_2("CPmAdapter::FetchLeafObjectSizeL(): begin '%S'", &aURI );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
#else
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
#endif	
	
	TError ret( EError );
	TPtrC8 parentMapping ( NSmlDmURI::LastURISeg( parent ) );
	CBufBase *object = CBufFlat::NewL( KFeatchLeafObjectSize );
	GetElementInfoL( parentMapping, mapping, object, ret );
	if ( ret == EOk )
		{
		HBufC8 *size = IntToDes8LC( object->Size() );
		object->Reset();
		object->InsertL( 0, *size );
		CleanupStack::PopAndDestroy( size ); 
		Callback().SetResultsL( aResultsRef, *object, aType );
		}
	Callback().SetStatusL( aStatusRef, ret );
	RDEBUG("CPmAdapter::FetchLeafObjectSizeL(): end" );
	}

// ------------------
//
// -----------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	//
	IMPLEMENTATION_PROXY_ENTRY( KNSmlDMPMAdapterImplUid, CPmAdapter::NewL )
	};

// ------------------------------------------------------------------------------------------------
// ImplementationGroupProxy( TInt& aTableCount )
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// ------------------------------------------------------------------------------------------------
// CPmAdapter::IntToDes8LC( const TInt aLuid )
// ------------------------------------------------------------------------------------------------
HBufC8* CPmAdapter::IntToDes8LC( const TInt aLuid )
	{
	HBufC8* buf = HBufC8::NewLC( KMaxLengthOf32bitInteger ); //10 = max length of 32bit integer
	TPtr8 ptrBuf = buf->Des();
	ptrBuf.Num( aLuid );
	return buf;
	}

// -------------------------------------------------------------------------------------
// CPmAdapter::SetLastStatusL( const TDesC8 &aStatus )
// -------------------------------------------------------------------------------------
void CPmAdapter::SetLastStatusL( const TDesC8 &aStatus )
	{
	delete iLastStatus;
	iLastStatus = NULL ;
	iLastStatus = aStatus.AllocL();
	}

// -------------------------------------------------------------------------------------
// CPmAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CPmAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, 
										MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat, 
										const TDesC8& aDescription)
	{
	aNode.SetAccessTypesL( aAccTypes );
	aNode.SetOccurenceL( aOccurrence );
	aNode.SetScopeL( aScope );
	aNode.SetDFFormatL( aFormat );
	if( aFormat != MSmlDmDDFObject::ENode )
		{
		aNode.AddDFTypeMimeTypeL( KNSmlPMTextPlain );
		}
	aNode.SetDescriptionL( aDescription );
	}

// -------------------------------------------------------------------------------------
// CPmAdapter::ConstructL( const TDesC8 &aArgument )
// -------------------------------------------------------------------------------------
void CPmCommand::ConstructL( const TDesC8 &aArgument )
	{
	iArgument = aArgument.AllocL();
	}

// -------------------------------------------------------------------------------------
// CPmAdapter::~CPmCommand()
// -------------------------------------------------------------------------------------
CPmCommand::~CPmCommand() 
	{
	delete iArgument;
	}

// -------------------------------------------------------------------------------------
// CPmAdapter::NewL( TInt aStatusRef, const TDesC8& aURI, const TDesC8 &aArgument )
// -------------------------------------------------------------------------------------
CPmCommand *CPmCommand::NewL( TInt aStatusRef, const TDesC8& aURI, const TDesC8 &aArgument )
	{
	CPmCommand *self = new ( ELeave ) CPmCommand( aStatusRef, aURI );
	self->ConstructL( aArgument );
	return self ;
	}
	



// End of File
