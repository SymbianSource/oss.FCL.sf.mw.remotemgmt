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



#ifndef __NSMLPMADAPTER_H__
#define __NSMLPMADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <PolicyEngineClient.h>

// Constants
const TUint KNSmlDMPMAdapterImplUid = 0x10207820;
  
_LIT8( KNSmlPMDDFVersion, "1.0" ); // the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )

_LIT8( KNSmlPMNodeName, "PolicyMgmt" );
_LIT8( KNSmlPMDescription, "Root node for policy management." );
_LIT8( KNSmlPMPolicySetsNodeName, "PolicySets" );
_LIT8( KNSmlPMPolicySetsNodeDescription, "Holds a few permanent set references and dynamic sets root node." );
_LIT8( KNSmlPMDynamicSetsDynamicNodeDescription, "This dynamic node is placeholder for policy sets." );
_LIT8( KNSmlPMPolicySetPoliciesNodeName, "Policies" );
_LIT8( KNSmlPMPolicySetPoliciesNodeDescription, "This node is root for policy references inside a policy set." );
_LIT8( KNSmlPMPolicySetPoliciesDynamicNodeDescription, "This dynamic node is placeholder for policy references" );
_LIT8( KNSmlPMPolicySetPoliciesPolicyNodeName, "Policy" );
_LIT8( KNSmlPMPolicySetPoliciesPolicyNodeDescription, "This leaf holds a URI reference to actual policy in the management tree." );
_LIT8( KNSmlPMPolicySetPolicySetsNodeName, "PolicySets" );
_LIT8( KNSmlPMPolicySetPolicySetsNodeDescription, "This node is root for policy set references inside a policy set." );
_LIT8( KNSmlPMPolicySetPolicySetsDynamicNodeDescription, "This dynamic node is placeholder for policy set references" );
_LIT8( KNSmlPMPolicySetPolicySetsPolicySetNodeName, "PolicySet" );
_LIT8( KNSmlPMPolicySetPolicySetsPolicySetNodeDescription, "This is URI of actual policy set in the management tree." );
_LIT8( KNSmlPMPolicySetIDNodeName, "ID" );
_LIT8( KNSmlPMPolicySetIDNodeDescription, "ID of the policy set from the original policy language file." );
_LIT8( KNSmlPMPolicySetDataNodeName, "Data" );
_LIT8( KNSmlPMPolicySetDataNodeDescription, "Data of the policy set in policy language format." );
_LIT8( KNSmlPMPolicySetDescriptionNodeName, "Description" );
_LIT8( KNSmlPMPolicySetDescriptionNodeDescription, "Description of policy set from the original XACML file is reflected in this leaf." );

_LIT8( KNSmlPMPoliciesNodeName, "Policies" );
_LIT8( KNSmlPMPoliciesNodeDescription, "This node is root for all policies in system." );
_LIT8( KNSmlPMPoliciesDynaNodeDescription, "This dynamic node is placeholder for policies." );
_LIT8( KNSmlPMPoliciesIDNodeName, "ID" );
_LIT8( KNSmlPMPoliciesIDNodeDescription, "This node is root for all policies in system." );
_LIT8( KNSmlPMPoliciesDataNodeName, "Data" );
_LIT8( KNSmlPMPoliciesDataNodeDescription, "This leaf node gives access to the data of the policy in XACML format." );
_LIT8( KNSmlPMPoliciesDescNodeName, "Description" );
_LIT8( KNSmlPMPoliciesDescNodeDescription, "Description of policy from the original XACML file is reflected in this leaf." );
_LIT8( KNSmlPMPoliciesRulesNodeName, "Rules" );
_LIT8( KNSmlPMPoliciesRulesNodeDescription, "Description of policy from the original XACML file is reflected in this leaf." );
_LIT8( KNSmlPMPoliciesRulesDynaNodeDescription, "This dynamic node is placeholder for rule references." );
_LIT8( KNSmlPMPoliciesRulesRuleNodeName, "Rule" );
_LIT8( KNSmlPMPoliciesRulesRuleNodeDescription, "" );

_LIT8( KNSmlPMRulesNodeName, "Rules" );
_LIT8( KNSmlPMRulesNodeDescription, "This node is root for all rules in system." );
_LIT8( KNSmlPMRulesDynaNodeDescription, "This dynamic node is placeholder for rules." );
_LIT8( KNSmlPMRulesIDNodeName, "ID" );
_LIT8( KNSmlPMRulesIDNodeDescription, "This node is root for all rules in system." );
_LIT8( KNSmlPMRulesDataNodeName, "Data" );
_LIT8( KNSmlPMRulesDataNodeDescription, "This leaf node gives access to the data of the rules in XACML format." );
_LIT8( KNSmlPMRulesDescNodeName, "Description" );
_LIT8( KNSmlPMRulesDescNodeDescription, "Description of policy from the rules XACML file is reflected in this leaf." );

_LIT8( KNSmlPMPolicyDeliveryNodeName, "Delivery" );
_LIT8( KNSmlPMPolicyDeliveryNodeDescription, "This node holds the nodes that let DM Server to bring PolicyManagement command packages to device and observe status of such action." );

_LIT8( KNSmlPMDeliverySinkNodeName, "Sink" );
_LIT8( KNSmlPMDeliverySinkNodeDescription, "This node is used by DM Server to bring PolicyManagement command packages to device." );

_LIT8( KNSmlPMLastDeliveryStatusNodeName, "LastStatus" );
_LIT8( KNSmlPMLastDeliveryStatusNodeDescription, "Stores status of last executed delivery command." );
_LIT8( KNSmlPMDynamicNode, "" );

_LIT8( KNSmlPMListOfLeafs, "PolicySets/Policies/Rules/PolicyDelivery" );
_LIT8( KNSmlPMListOfDynamicSetsDynaLeafs, "Policies/PolicySets/ID/Data/Description" );
_LIT8( KNSmlPMListOfPoliciesDynaLeafs, "ID/Data/Description/Rules" );
_LIT8( KNSmlPMListOfRulesDynaLeafs, "ID/Data/Description" );
_LIT8( KNSmlPMListOfDynaPolicyLeafs, "Policy" );
_LIT8( KNSmlPMListOfDynaPolicySetLeafs, "PolicySet" );

_LIT8( KNSmlPMTextPlain, "text/plain" );
_LIT8( KNSmlPMSeparator8, "/" );

_LIT8( KTestNodeName, "TEST" ) ;

// ------------------------------------------------------------------------------------------------
// CPmCommand
// Struct is used to cache add command status referecne when policies are added 
// ------------------------------------------------------------------------------------------------
class CPmCommand : public CBase
	{
	CPmCommand( TInt aStatusRef, const TDesC8& aURI ) : iStatusRef( aStatusRef ), iURI (aURI)
		{
		}
	TInt iStatusRef ;
	TBuf8<256> iURI ;
	HBufC8 *iArgument ;
	void ConstructL(const TDesC8 &aArgument);
public:
	virtual ~CPmCommand() ;
	static CPmCommand *NewL( TInt aStatusRef, const TDesC8& aURI, const TDesC8 &aArgument );
	
	TInt StatusRef() const
		{
		return iStatusRef ;
		}
	const TDesC8 &URI()
		{
		return iURI ;
		}
	const TDesC8 &Argument() 
		{
		return *iArgument;
		}
	};

		
// ------------------------------------------------------------------------------------------------
// CPmAdapter 
// ------------------------------------------------------------------------------------------------
class CPmAdapter : public CSmlDmAdapter
	{
public:
	static CPmAdapter* NewL( MSmlDmCallback* aDmCallback );
	static CPmAdapter* NewLC( MSmlDmCallback* aDmCallback );

	virtual ~CPmAdapter();

	// Pure virtual methods from CSmlDmAdapter
	void DDFVersionL( CBufBase& aVersion );
	void DDFStructureL( MSmlDmDDFObject& aDDF );
	void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
									const TDesC8& aObject, const TDesC8& aType,
									TInt aStatusRef );
	void UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
									RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
		 							TInt /*aStatusRef*/ ) ;
	
	void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID,
								TInt aStatusRef );

	void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
								   const TDesC8& aType, TInt aResultsRef,
								   TInt aStatusRef );
								   								
	void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID,
									   const TDesC8& aType, TInt aResultsRef,
									   TInt aStatusRef ) ;
	
	void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID,
					const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
					TInt aResultsRef, TInt aStatusRef );
					
	void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID,
								 TInt aStatusRef );			

	/**
	The function implements execute command. The information about the success
	of the command should be returned by calling SetStatusL function of
	MSmlDmCallback callback interface. This makes it possible to buffer the
	commands.
	However, all the status codes for buffered commands must be returned at
	the latest when the CompleteOutstandingCmdsL() of adapter is called.
	@param aURI			URI of the command
	@param aLUID			LUID of the object (if the adapter have earlier
							returned LUID to the DM Module).   
	@param aArgument		Argument for the command
	@param aType			MIME type of the object 
	@param aStatusRef		Reference to correct command, i.e. this reference
							must be used when calling the SetStatusL of this
							command.
	@publishedPartner
	@prototype
	*/
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
							      const TDesC8& aArgument, const TDesC8& aType,
								  TInt aStatusRef );

	/**
	The function implements execute command. The information about the
	success of the command should be returned by calling SetStatusL function
	of MSmlDmCallback callback interface. This makes it possible to buffer the
	commands.
	However, all the status codes for buffered commands must be returned at
	the latest when the CompleteOutstandingCmdsL() of adapter is called.
	@param aURI			URI of the command
	@param aLUID			LUID of the object (if the adapter have earlier
							returned LUID to the DM Module).   
	@param aStream		Argument for the command. Adapter should create
							write stream and return, when data is written to
							stream by DM agent, StreamCommittedL() is called by
							DM engine
	@param aType			MIME type of the object 
	@param aStatusRef		Reference to correct command, i.e. this reference
							must be used when calling the SetStatusL of this
							command.
	@publishedPartner
	@prototype
	*/
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
								  RWriteStream*& aStream, const TDesC8& aType,
								  TInt aStatusRef ) ;

	/**
	The function implements copy command. The information about the success of
	the command should be returned by calling SetStatusL function of
	MSmlDmCallback callback interface. This makes it possible to buffer the
	commands.
	However, all the status codes for buffered commands must be returned at
	the latest when the CompleteOutstandingCmdsL() of adapter is called.
	@param aTargetURI		Target URI for the command
	@param aSourceLUID	LUID of the target object (if one exists, and if the adapter
							has	earlier returned a LUID to the DM Module).   
	@param aSourceURI		Source URI for the command
	@param aSourceLUID	LUID of the source object (if the adapter has
							earlier returned a LUID to the DM Module).   
	@param aType			MIME type of the objects
	@param aStatusRef		Reference to correct command, i.e. this reference
							must be used when calling the SetStatusL of this
							command.
	@publishedPartner
	@prototype
	*/
	void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
							   const TDesC8& aSourceURI, const TDesC8& aSourceLUID,
							   const TDesC8& aType, TInt aStatusRef ) ;

	/**
	The function indicates start of Atomic command.
	@publishedPartner
	@prototype
	*/
	void StartAtomicL();
	
	/**
	The function indicates successful end of Atomic command. The adapter
	should commit all changes issued between StartAtomicL() and
	CommitAtomicL()
	@publishedPartner
	@prototype
	*/
	void CommitAtomicL();
	
	/**
	The function indicates unsuccessful end of Atomic command. The adapter
	should rollback all changes issued between StartAtomicL() and
	RollbackAtomicL(). If rollback fails for a command, adapter should use
	SetStatusL() to indicate it.
	@publishedPartner
	@prototype
	*/
	 void RollbackAtomicL() ;
	
	/**
    Returns ETrue if adapter supports streaming otherwise EFalse.
	@param aItemSize size limit for stream usage
    @return TBool ETrue for streaming support
	@publishedPartner
	@prototype
    */
	TBool StreamingSupport( TInt& aItemSize );
	
	/**
    Called when stream returned from UpdateLeafObjectL or ExecuteCommandL has
	been written to and committed. Not called when fetching item.
	@publishedPartner
	@prototype
    */	
	#ifdef __TARM_SYMBIAN_CONVERGENCY	
	virtual void StreamCommittedL( RWriteStream& aStream );
  #else
	virtual void StreamCommittedL();
  #endif	
	
	/**
	The function tells the adapter that all the commands of the message that
	can be passed to the adapter have now been passed.  This indciates that
	the adapter must supply status codes and results to any buffered commands.
	This must be done at latest by the time this function returns.
	This function is used at the end of SyncML messages, and during processing
	of Atomic.   In the case of Atomic processing, the function will be
	followed by a call to CommitAtomicL or RollbackAtomicL.
	@publishedPartner
	@prototype
	*/
	void CompleteOutstandingCmdsL() ;
									 	 
private:
	CPmAdapter(TAny* aEcomArguments);
	void ConstructL( );
	
	/**
	 * Converts given integer to string, leaving returned
	 * HBufC8 into cleanupstack.
	 */
	HBufC8* IntToDes8LC( const TInt aLuid );
	
	/**
	 * Puts given elementid array to aObject
	 */
	void ArrayToDMResponseL( const RElementIdArray &aArray, CBufBase &aObject);
	
	/**
	 * Finds element matching the mapping, and puts found elementid to aObject,
	 * and succes code to aRet
	 */
	 void GetElementAndAddToResponseL( const TDesC8 &aParentMapping, 
			CBufBase &aObject, MSmlDmAdapter::TError &aRet );
	
	
	/**
	FillNodeInfoL Sets the given node properties to given node.
	@param aNode The node to be modified
	@param aAccTypes The SyncML access types to be set to the node.
	@param aOccurrence The SyncML occurance of the node
	@param aScope The SyncML scope of the node
	@param aFormat The SyncML format of the node
	@param aDescription The description of the node
	*/
	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
						MSmlDmDDFObject::TOccurence aOccurrence, 
						MSmlDmDDFObject::TScope aScope, 
						MSmlDmDDFObject::TDFFormat aFormat,
						const TDesC8& aDescription);

	/**
	GetElementInfoL Finds ID, Description or XACML Data (defined by aType) of 
	the element (defined by aElementId) and puts result into aObject,
	and operation status to aRet.
	@param aElementId The element id whose data is to be collected
	@param aType	The tree node name identifying type of data to be found
	@param aObject	The buffer into which the data is put
	@param	aRet	The operation status in SML error codes
	@return	Symbian Error code
	*/
	TInt GetElementInfoL(const TDesC8 &aElementId, 
		const TDesC8 &mapping /*aType*/, CBufBase* object, TError &ret);
		
	/**
	 * SetLastStatusL Helper method to set the last status
	 */
	void SetLastStatusL( const TDesC8 &aStatus );
	
	/**
	 * CheckAndAddCertL Reads cert from repository, and adds policy management
	 * trust.
	 */
	void CheckAndAddCertL( );
	
	/**
	 * InsertIfFoundL if given policy element is found, places aData in aObject 
	 * and returns EOk, otherwise ENotFound or EError
	 */
	TError InsertIfFoundL( const TDesC8 &aElementId, const TDesC8& aData, CBufBase &aObject );
private:
	// The policy engine;
	RPolicyEngine iEngine ;
	
	// The management session
	RPolicyManagement iManagement ;
	
	// are we inside atomic
	TBool iInAtomic;
	
	// holds the commands that have come inside last atomic
	RPointerArray<CPmCommand> iAtomiced ;
	
	// Holds the last policy command status
	HBufC8 *iLastStatus;
};

#endif // __NSMLPMADAPTER_H__
