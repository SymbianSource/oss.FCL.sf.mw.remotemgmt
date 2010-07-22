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




#ifndef _POLICY_PROCESSOR_HEADER__
#define _POLICY_PROCESSOR_HEADER__

// INCLUDES

#include "PolicyEngineServer.h"
#include "ElementBase.h"
#include "RequestContext.h"
#include "PolicyStorage.h"
#include <e32base.h>
#include <MSVAPI.H>
#include <hbsymbianvariant.h>
#include <hbdevicedialogsymbian.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CDataType;
class CAttribute;
class CAttributeValue;
class CTrustedSession;

typedef RArray<CAttributeValue*> RParameterList;

// CLASS DECLARATION

namespace AttributeContainerHelper
{
	enum TAttributeTypes
	{
		ESubjectAttributes = 0,
		EActionAttributes,
		EResourceAttributes,
		EEnvironmentAttributes 
	};

	class TContainer
	{
		public:
			CAttribute * iAttribute;
			TAttributeTypes iAttributeType;
		private:
	};
}

class RAttributeContainer : public RArray<AttributeContainerHelper::TContainer*>
{
	public: 
		void AppendL( AttributeContainerHelper::TAttributeTypes aAttributeTypes, CAttribute* aAttribute);
		void Close();
	private:
};

class CPolicyProcessor : public CActive
{
	public:
		typedef RArray<CAttribute*> RAttributeList;
	
		static CPolicyProcessor * NewL();
		CPolicyProcessor();
		~CPolicyProcessor();
		void ConstructL();
	
		void ResetRequestContext();
	
		void ExecuteRequestL( RAttributeContainer& aAttributes, TMatchResponse &aResponse);
		void ExecuteRequestL( const RMessage2& aMessage);

	
		//Request context	
		TRequestContext * RequestContext();
		
		RAttributeList * Subjects();
		RAttributeList * Actions();
		RAttributeList * Resources();
		RAttributeList * Environments();		
	
		//Execution functions	
		static void HandleErrorL( const TDesC8 &aText);

		TBool MatchFunctionL( const TDesC8 &aFunctionId, CDataType * aData1, CDataType * aData2);
		void FunctionL( const TDesC8 &aFunctionId, RParameterList& aParams, CAttributeValue* aResponseElement);
		
		//For management functions
		void SetTargetElement( CElementBase * aElement);
		void SetSessionTrust( CTrustedSession * aTrustedSession);	
	protected:
		//From CActive
		void RunL();
		void DoCancel();
		TInt RunError( TInt aError);		
	private:
		void DoRequestExecutionL(TMatchResponse &aResponse, TBool iUseEditableElements);
		TPtrC8 NextToken( TPtrC8& aText);
		
		void OrFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement );
		void AndFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement );
		void NotFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement );

		void CertificateForSessionL( CAttributeValue* aResponseElement );
		void CorporateUserAcceptFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement );
		void RuleTargetStructureFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement );
		void EqualFunctionsL( const RParameterList& aParams, CAttributeValue* aResponseElement );
		
		void MakeBooleanResponseL( const TBool aValue, CAttributeValue* aResponseElement );
		
		void CompleteMessage( TInt aError, TResponse aResponse);
	private:
		CElementBase * iManagementTargetPolicy;
		CTrustedSession * iTrustedSession;
				
		RAttributeList iSubjects;
		RAttributeList iActions;
		RAttributeList iResources;
		RAttributeList iEnvironments;
		TBool iDeleteAttributes;
		TBool iUseEditedElements;
		
		//RunL handling
		const RMessage2 * iMessage;
		HBufC8 * iRequestBuffer;		
		TInt iProcessorState;
};	


//Class to launch PolicyEngine Dialog
class CProcessorClient: public CActive,public MHbDeviceDialogObserver
{
public:
    CProcessorClient();
    ~CProcessorClient();
    TInt LaunchDialog(const TDesC8& aFringerPrint, const TDesC8& aServerName);
    // from MHbDeviceDialogObserver
    void DataReceived(CHbSymbianVariantMap& aData);
    void DeviceDialogClosed(TInt aCompletionCode);

protected:
    // from CActive
    void DoCancel();
    void RunL();
    TInt WaitUntilDeviceDialogClosed();
    void LaunchTrustNotificationDialog(const TDesC8& aServerName);
private:
    CHbDeviceDialogSymbian* iDevDialog;
    CActiveSchedulerWait* iWait;
    TInt iCompletionCode;
    TInt iUserResponse;
};




class TCombiningAlgorith
{
	public:
		TCombiningAlgorith( const TDesC8 &aAlgorithId);
		
		TBool AddInput( const TMatchResponse& aInput, const TMatchResponse& aEffect);
		TBool AddInput( const TMatchResponse& aInput);
		TBool ResultReady();
		TMatchResponse Result();
	private: 
		enum TCombingAlgorithm
		{
			ERuleDenyOverrides = 0,
			EPolicyDenyOverrides,
			ERulePermitOverrides,
			EPolicyPermitOverrides,
		};
	private:
		TCombingAlgorithm iAlgorithm;
		TMatchResponse iCurrentResponse;
		TBool iResultReady;
		TBool iAtLeastOneError;
		TBool iPotentialDeny;
		TBool iPotentialPermit;
		TBool iAtLeastOneDeny;
		TBool iAtLeastOnePermit;
};


#endif