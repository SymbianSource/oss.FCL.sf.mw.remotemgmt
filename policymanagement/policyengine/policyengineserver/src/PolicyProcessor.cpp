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

#include "PolicyProcessor.h"
#include "PolicyStorage.h"
#include "PolicyParser.h"
#include "elements.h"
#include "ElementBase.h"
#include "DataTypes.h"
#include "ErrorCodes.h"
#include "TrustedSession.h"
#include "XACMLconstants.h"
#include "PolicyEngineServer.h"
#include "PolicyEngineClientServer.h"
#include "debug.h"
#include <hbdevicedialogsymbian.h>
#include <hbdevicenotificationdialogsymbian.h>
#include <hbtextresolversymbian.h>

#include "DMUtilClient.h"
#include "PMUtilInternalCRKeys.h"

#include <eikenv.h>
#include <centralrepository.h>

#include <s32mem.h>
// CONSTANTS
const TUid KUidPolicyEngineUi = { 0x10207817 };
const TUint KDelimeterChar = '|';
_LIT8( KUserAcceptMark, "A");

const TInt KMaxServerIdLength = 250;
const TInt KMaxLabelIdLength = 25;
const TInt KMaxCertSize = 1024;
        
_LIT(KSymbianCertificateStoreLabel, "Software certificate store");
_LIT(KDMTrustedCertLabel,"DMTrustedServer_");
_LIT8(KNoKeyFound,"");

enum TUserResponse
    {
    EUserAccept,
    EUserDeny,
    };

// -----------------------------------------------------------------------------
// RAttributeContainer::AppendL()
// -----------------------------------------------------------------------------
//
void RAttributeContainer::AppendL( AttributeContainerHelper::TAttributeTypes aAttributeTypes, CAttribute* aAttribute)
{
	//temporary to cleanup stack
	CleanupStack::PushL( aAttribute);

	//create container for attribute
	AttributeContainerHelper::TContainer * container = new (ELeave) AttributeContainerHelper::TContainer;
	CleanupStack::PushL( container);
	
	//set container properties
	container->iAttribute = aAttribute;
	container->iAttributeType = aAttributeTypes; 

	//append to list
	RArray<AttributeContainerHelper::TContainer*>::AppendL( container);
		
	//remove from cleanup
	CleanupStack::Pop( 2, aAttribute);
}

// -----------------------------------------------------------------------------
// RAttributeContainer::Close()
// -----------------------------------------------------------------------------
//
void RAttributeContainer::Close()
{
	//delete attributes and container...
	for ( TInt i(0); i < Count(); i++)
	{
		AttributeContainerHelper::TContainer * container = operator[]( i);
		delete container->iAttribute;
		delete container;
	}
	
	RArray<AttributeContainerHelper::TContainer*>::Close();
}



// -----------------------------------------------------------------------------
// CPolicyProcessor::CPolicyProcessor()
// -----------------------------------------------------------------------------
//

CPolicyProcessor::CPolicyProcessor()
	: CActive( EPriorityStandard)
{
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::NewL()
// -----------------------------------------------------------------------------
//

CPolicyProcessor * CPolicyProcessor::NewL()
{
	CPolicyProcessor * self = new(ELeave) CPolicyProcessor();
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
	
}
	
// -----------------------------------------------------------------------------
// CPolicyProcessor::~CPolicyProcessor()
// -----------------------------------------------------------------------------
//

CPolicyProcessor::~CPolicyProcessor()
{  
    delete iCertBuffer;
    iCertBuffer=NULL;
    delete iStore;
    ////
    ResetRequestContext();
	delete iRequestBuffer;

	CPolicyEngineServer::RemoveActiveObject( this);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ConstructL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::ConstructL()
{
	CPolicyEngineServer::AddActiveObjectL( this);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::AddCertificateToStoreL()
// -----------------------------------------------------------------------------
//

TInt CPolicyProcessor::AddCertificateToStoreL(const RMessage2& aMessage)
    {
    RDEBUG("CPolicyProcessor::AddCertificateToStore" );
    TInt size = aMessage.Int1();
    TBuf<KMaxCertSize> certData;    
    
    aMessage.ReadL(0, certData, 0);     
    iCertBuffer = HBufC8::NewL(size);     
    iCertBuffer->Des().Copy(certData);
    
    //check if the trust already exists with same server
    TInt result = CheckCurrentServerIdFromCR();     
    if(result == KErrInUse)
        {       
        return KErrInUse;    
        }
    
    iStore = CCertStore::NewL();
    iStore->InitializeCertStore();
    TInt res = iStore->AddCert(iCertBuffer);
    if(res == KErrNone)
        {        
        //update the new counter value in CR
        iStore->UpdateLabelCounterInCR();  
        // return the newly generated label
        aMessage.WriteL( 2, iStore->GetCurrentCertLabel()->Des() );     
        }
    
    return res; 
    }



// -----------------------------------------------------------------------------
// CPolicyProcessor::RemoveCertificateFromStoreL()
// -----------------------------------------------------------------------------
//
TInt CPolicyProcessor::RemoveCertificateFromStoreL(const RMessage2& aMessage)
    {
    RDEBUG("CPolicyProcessor::RemoveCertificateFromStoreL" );
    TBuf<128> certLabel;
    aMessage.ReadL(0, certLabel, 0);   
       
    HBufC *label = HBufC::NewL(aMessage.GetDesLength(0));
    label->Des().Copy(certLabel);  
    TInt result = CheckCurrentServerIdFromCR();
    
    //check if the trust already exists with same server
    if(result == KErrInUse)
        {
        RDEBUG("CPolicyProcessor::RemoveCertificateFromStore -14" );
        return KErrInUse;    
        }
  
    if ( iStore != NULL )
       {
       delete iStore;
       iStore = NULL;
       }
    iStore = CCertStore::NewL();
    iStore->InitializeCertStore();    
    
    TInt res;
    if(label->Des() == KNullDesC)
        {
        // If NULL is passed for removing label then remove the current label
        HBufC* rLabel = iStore->GetCurrentCertLabel()->AllocL();
        res = iStore->RemoveCert(*rLabel);
        delete rLabel;
        }
    else
        {
        res = iStore->RemoveCert(*label); 
        }
 
    delete label;
    label = NULL;
    return res;
    }

// -----------------------------------------------------------------------------
// CPolicyProcessor::CheckCurrentServerIdFromCR()
// -----------------------------------------------------------------------------
//
TInt CPolicyProcessor::CheckCurrentServerIdFromCR()
    {
    RDEBUG("CPolicyProcessor::CheckCurrentServerIdFromCR" );
    TBuf8<KMaxServerIdLength>  serverIdCR;         
    TRAPD( err, 
           {
           CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
           CleanupStack::PushL( rep );
           rep->Get( KTrustedServerId, serverIdCR );
           CleanupStack::PopAndDestroy( rep );
           } );
  
    if(err!=KErrNone)
        {
        return err;
        }    
    TBool result = CPolicyStorage::PolicyStorage()->IsServerIdValid(serverIdCR);    
    if(result)
        {
        return KErrInUse;
        }
    else
        {
        return KErrNone;
        }
        
    }

// -----------------------------------------------------------------------------
// CPolicyProcessor::HandleErrorL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::HandleErrorL( const TDesC8& aText	)
{
	RDEBUG("CPolicyProcessor::HandleErrorL");
	//error handling for processor
	RDEBUG_2("XACML request indeterminate: %S", &aText);
	User::Leave( KErrAbort);	
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ResetRequestContext()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ResetRequestContext()
{
	if ( iDeleteAttributes )
	{
		//delete subject attributes and closes RArray
		for ( TInt  i(0); i < iSubjects.Count(); i++)
		{
			CAttribute * attribute = iSubjects[i];
			delete attribute;
		}

		//delete actions attributes and closes RArray
		for ( TInt  i(0); i < iActions.Count(); i++)
		{
			CAttribute * attribute = iActions[i];
			delete attribute;
		}

		//delete resources attributes and closes RArray
		for ( TInt  i(0); i < iResources.Count(); i++)
		{
			CAttribute * attribute = iResources[i];
			delete attribute;
		}

		//delete environments attributes and closes RArray
		for ( TInt  i(0); i < iEnvironments.Count(); i++)
		{
			CAttribute * attribute = iEnvironments[i];
			delete attribute;
		}
	}
	
	iSubjects.Close();
	iActions.Close();
	iResources.Close();
	iEnvironments.Close();
}



// -----------------------------------------------------------------------------
// CPolicyProcessor::ResetRequestContext()
// -----------------------------------------------------------------------------
//
TPtrC8 CPolicyProcessor::NextToken( TPtrC8& aText)
{
	if ( !aText.Length() ) return aText;

	//remove first delimiter
	if ( aText[0] == KMessageDelimiterChar)
	{
		aText.Set( aText.Mid(1));
	}
		
	//find next delimiter
	TPtrC8 retVal = aText;
	TInt index = aText.Locate( KMessageDelimiterChar);		    	
		
	if ( index != KErrNotFound )
	{
		//set retVal to point token and aText to remaining part
		retVal.Set( aText.Left( index));
		aText.Set( aText.Mid( index ));
	}
	
	return retVal;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ExecuteRequestL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ExecuteRequestL( RAttributeContainer &aAttributes, TMatchResponse &aResponse)
{
	iDeleteAttributes = EFalse;
	iUseEditedElements = ETrue;

	//reset request context
	ResetRequestContext();

	for ( TInt i(0); i < aAttributes.Count(); i++)
	{
		RAttributeList * list = 0;
	
		using namespace AttributeContainerHelper;
		TContainer * container = aAttributes[i];
	
		switch ( container->iAttributeType )
		{
			case ESubjectAttributes :
			{
				list = &iSubjects;
				break;		
			}
			case EActionAttributes :
			{
				list = &iActions;
				break;		
			}
			case EResourceAttributes :
			{
				list = &iResources;
				break;		
			}
			case EEnvironmentAttributes :
			{
				list = &iEnvironments;
				break;		
			}
			default:
				User::Panic( PolicyParserPanic, KErrCorrupt);
			break;
		}
		
		//append attribute to attribute list
		list->AppendL( container->iAttribute);
	}
	
	//make request
	DoRequestExecutionL( aResponse, ETrue);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ExecuteRequestL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ExecuteRequestL( const RMessage2& aMessage)
{
	iUseEditedElements = EFalse;
	iDeleteAttributes = ETrue;


	RDEBUG("PolicyEngineServer: CPolicyProcessor::ExecuteRequestL");
	
	
	//reset request context
	ResetRequestContext();
	
	iMessage = &aMessage;
	iProcessorState = 0;
	
	//Add object to active scheduler
	if ( !IsAdded())
	{
		CActiveScheduler::Add( this);
	}
	
	//complete own request
	SetActive();
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::RunL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::RunL()
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::RunL");
	
	CPolicyEngineServer::SetActiveSubSession( this);	

	if ( iProcessorState == 0)
	{
		//Read request context from client
		delete iRequestBuffer;
		iRequestBuffer = NULL;
		iRequestBuffer = HBufC8::NewL( iMessage->GetDesLength(0));
		TPtr8 msg = iRequestBuffer->Des();
		iMessage->ReadL(0, msg, 0);	
		TPtrC8 ptr = msg;

		TBool continueLoop( ETrue);
		
		//read msg from client, msg contain request attributes
		while ( continueLoop )
		{
			TPtrC8 type = NextToken( ptr);	
			RAttributeList * list;
	
			//identifie attribute type (subject, action, resource or env)
			if ( type == _L8("S"))
			{
				list = &iSubjects;
			}
			else
			if ( type == _L8("A"))
			{
				list = &iActions;
			}
			else
			if ( type == _L8("R"))
			{
				list = &iResources;
			}
			else
			if ( type == _L8("E"))
			{
				list = &iEnvironments;
			}
			else
			{
				//no more attributes
				continueLoop = EFalse;
			}
		
			if ( continueLoop )
			{
				//read attribute id, value and datatype
				TPtrC8 aAttributeId( NextToken( ptr));
				TPtrC8 aAttributeValue( NextToken( ptr));
				TPtrC8 aDataType( CPolicyParser::ConvertValues( EXACML, NextToken( ptr)));
		
				//types, which contains default values must be converted to native language
				if ( aDataType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType )
				{
					aAttributeValue.Set( CPolicyParser::ConvertValues( EXACML, aAttributeValue));
				}

				CAttribute * attribute = CAttribute::NewL( aAttributeId, 
													   aAttributeValue, 
													   aDataType);
			
				//append attribute to attribute list
				list->AppendL( attribute);
			}
		}

		iProcessorState++;		
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
		return;	
	}

	if ( CPolicyEngineServer::StatusFlags()->iProcessorActive && !CPolicyEngineServer::StatusFlags()->iUiActive )
	{
		//Start new execution only if no other executions active or
		//if execution is ongoing in state
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
		return;		
	}
	
	//tell other processors that processor is reserved
	CPolicyEngineServer::StatusFlags()->iProcessorActive = ETrue;
	
	//make request
	TMatchResponse matchResponse;
	RDEBUG("PolicyEngineServer: RunL - start request execution");
	TRAPD( err, DoRequestExecutionL( matchResponse, EFalse));
	RDEBUG("PolicyEngineServer: RunL - stop request execution");
	
	if ( err != KErrNone)
	{
		matchResponse = EIndeterminate;
	}
	
	//set default response
	TResponse response;
	response.SetResponseValue( EResponseIndeterminate);
	
	
	//convert internal response to external response
	switch ( matchResponse )
	{
		case EDeny :
		{
			RDEBUG("PolicyEngineServer: Request executed - response deny");
			response.SetResponseValue( EResponseDeny);
			break;
		}
		case EPermit :
		{
			RDEBUG("PolicyEngineServer: Request executed - response permit");
			response.SetResponseValue( EResponsePermit);
			break;
		}
		case ENotApplicable :
		{
			RDEBUG("PolicyEngineServer: Request executed - response not applicable");
			response.SetResponseValue( EResponseNotApplicable);
			break;
		}
		case EIndeterminate :
		{
			RDEBUG("PolicyEngineServer: Request executed - response indeterminate");
			response.SetResponseValue( EResponseIndeterminate);
			break;
		}
		default:
		{
			RDEBUG("PolicyEngineServer: Request executed - response indeterminate");
			response.SetResponseValue( EResponseIndeterminate);
			break;
		}
	}	
	
	//tell other processors that processor is free
	CPolicyEngineServer::StatusFlags()->iProcessorActive = EFalse;
		
	CompleteMessage( KErrNone, response);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::DoCancel()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::DoCancel()
{
	RunError( KErrAbort);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::RunError()
// -----------------------------------------------------------------------------
//
TInt CPolicyProcessor::RunError( TInt /*aError*/)
{
	TResponse resp;
	resp.SetResponseValue( EResponseIndeterminate);
	CompleteMessage( KErrAbort, resp );
	
	return KErrNone;	
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::CompleteMessage()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::CompleteMessage( TInt aError, TResponse aResponse)
	{
	RDEBUG_2("PolicyEngineServer: CPolicyProcessor::CompleteMessage( %d )", aError );

	delete iRequestBuffer; 	
	iRequestBuffer = NULL;

	//write response to msg (to client side)
	TPckg< TResponse> pack( aResponse);
	TRAPD( err, iMessage->WriteL(1, pack) );	
	if( err != KErrNone )
		{
		iMessage->Complete( err );
		}
	else
		{
		iMessage->Complete( aError );
		}

	RDEBUG("PolicyEngineServer: CPolicyProcessor::CompleteMessage - end");
	}


// -----------------------------------------------------------------------------
// CPolicyProcessor::DoRequestExecutionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::DoRequestExecutionL( TMatchResponse &aResponse, TBool iUseEditableElements)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::DoRequestExecutionL");



	//get root element, execution starts from root  
	CElementBase * rootElement = 0;
	
	if ( iUseEditableElements )
	{
		rootElement = CPolicyStorage::PolicyStorage()->GetEditableElementL( PolicyLanguage::Constants::RootElement);
	}
	else
	{
		rootElement = CPolicyStorage::PolicyStorage()->GetElementL( PolicyLanguage::Constants::RootElement);
	}
	
	aResponse = EIndeterminate;
	
	//if root found
	if ( rootElement )
	{
		//reserve element from storage and check match to root policy
		TElementReserver rootReserver( rootElement);
		aResponse = rootElement->MatchL( this);
		rootReserver.Release();
		//execution runs recursive inside MatchL-function to correct branches and elements in policy system
		//response is result of many element combination
		RDEBUG("PolicyEngineServer: DoRequestExecutionL - Match evaluated");
	}
	
	RDEBUG("PolicyEngineServer: CPolicyProcessor::DoRequestExecutionL - end");
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::FunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::FunctionL( const TDesC8 &aFunctionId, RParameterList& aParams, CAttributeValue* aResponseElement)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::FunctionL");

	//executes correct function call
	if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionAnd)
	{
		RDEBUG("PolicyEngineServer: AND-function start");
		AndFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: AND-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionOr)
	{
		RDEBUG("PolicyEngineServer: OR-function start");
		OrFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: OR-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionNot)
	{
		RDEBUG("PolicyEngineServer: NOT-function start");
		NotFunctionL( aParams, aResponseElement);		
		RDEBUG("PolicyEngineServer: NOT-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionRuleTargetStructure)
	{
		RDEBUG("PolicyEngineServer: Rule target structure-function start");
		RuleTargetStructureFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: Rule target structure-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionUserAcceptCorpPolicy)
	{		
		RDEBUG("PolicyEngineServer: User accept corporate-function start");
		CorporateUserAcceptFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: User accept corporate-function end");
	} 
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionCertificatedSession)
	{
		RDEBUG("PolicyEngineServer: Certificate for session-function start");
		CertificateForSessionL( aResponseElement);
		RDEBUG("PolicyEngineServer: Certificate for session-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId || 
			  aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId)
	{
		RDEBUG("PolicyEngineServer: Equal-function start");
		EqualFunctionsL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: Equal-function end");
	}
	else
	{
		RDEBUG("PolicyEngineServer: Function is not allowed!");
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::BooleanFunction()
// -----------------------------------------------------------------------------
//

TBool CPolicyProcessor::MatchFunctionL( const TDesC8 &aFunctionId, CDataType * data1, CDataType * data2)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::MatchFunctionL");

	//data1 is policy context value
	//data2 is request context value

	TBool retVal(EFalse);

	//execute function
	if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId ||
		 aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId )
	{
		TInt value = data1->Compare( data2);
		retVal = ( 0 == value);
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedSubjectMatch)
	{
		//compare trusted subjects...
		retVal = iTrustedSession->CertMatchL( data1->Value(), data2->Value(), iUseEditedElements);
	} 
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedRoleMatch)
	{
		//compare roles and subjects...
		retVal = iTrustedSession->RoleMatchL( data2->Value(), data1->Value(), iUseEditedElements);
	}
	else
	{
		User::Panic(PolicyParserPanic, KErrCorrupt);
	}
	
	
	
	return retVal;
}
	
// -----------------------------------------------------------------------------
// CPolicyProcessor::Subjects()
// -----------------------------------------------------------------------------
//	
CPolicyProcessor::RAttributeList * CPolicyProcessor::Subjects()
{
	return &iSubjects;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Actions()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Actions()
{
	return &iActions;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Resources()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Resources()
{
	return &iResources;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Environments()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Environments()		
{
	return &iEnvironments;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::SetTargetElement()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::SetTargetElement( CElementBase * aElement)
{
	iManagementTargetPolicy = aElement;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::SetSessionTrust()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::SetSessionTrust( CTrustedSession * aTrustedSession)
{
	iTrustedSession = aTrustedSession;
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::MakeBooleanResponseL()
// -----------------------------------------------------------------------------
//


void CPolicyProcessor::MakeBooleanResponseL( const TBool aValue, CAttributeValue* aResponseElement )
{
	if ( aValue )
	{
		RDEBUG("PolicyEngineServer: Boolean response TRUE");
		aResponseElement->SetDataL( PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue, 
								   PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType);
	}
	else
	{
		RDEBUG("PolicyEngineServer: Boolean response FALSE");
		aResponseElement->SetDataL( PolicyLanguage::NativeLanguage::AttributeValues::BooleanFalse, 
								   PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType);
	}	
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::GetCertCounterValue()
// -----------------------------------------------------------------------------
//
TInt CPolicyProcessor::GetCertCounterValue()
    {
    RDEBUG("CPolicyProcessor::GetCertCounterValue" );

    TInt counter = -1;
    
    TRAPD( err, 
       {
       CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
       CleanupStack::PushL( rep );
       rep->Get( KCertificateCounter, counter );
       CleanupStack::PopAndDestroy( rep );
       } );
   
    return counter;
    }


// -----------------------------------------------------------------------------
// CPolicyProcessor::UpdateSilentTrustServerId()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::UpdateSilentTrustServerId()
    { 
    RDEBUG("CPolicyProcessor::UpdateSilentTrustServerId" );
    //get server id and store in cenrep
    //connect to DM util client...
    RDMUtil dmutil;
    dmutil.Connect();
    CleanupClosePushL( dmutil);
    
    //..and get server id
    TBuf8<KMaxServerIdLength> serverid;
    dmutil.GetDMSessionServerId( serverid);
    CleanupStack::PopAndDestroy( &dmutil); 
    
    TInt ret;
    TRAPD( err, 
      {
        CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
        CleanupStack::PushL( rep );
        ret = rep->Set( KTrustedServerId, serverid );
        CleanupStack::PopAndDestroy( rep );
      } );

    }

// -----------------------------------------------------------------------------
// CPolicyProcessor::CorporateUserAcceptFunctionL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::CorporateUserAcceptFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::CorporateUserAcceptFunctionL");
	
	if ( !aParams.Count())
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}

	//Get trusted subject
	CAttributeValue * attributeValue = aParams[0];
	TDesC8& trustedSubject = attributeValue->Data()->Value();

	//resolve name and fingerprint
	const TDesC8& name = iTrustedSession->CommonNameForSubjectL( trustedSubject, iUseEditedElements);
	const TDesC8& fingerPrint = iTrustedSession->FingerPrintForSubjectL( trustedSubject, iUseEditedElements);
	TPtrC8 ptr = fingerPrint.Left(4); // send only first 4 digits.
	
	RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &fingerPrint);
	RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &ptr);
	
	RDEBUG8_2("CPolicyProcessor::fingerPrint: %S", &fingerPrint);
	RDEBUG8_2("CPolicyProcessor::fingerPrint: %S", &ptr);
	    
	TUserResponse response = EUserDeny;
	
    TBool showUserScreen = ETrue;
      
    if(GetCertCounterValue() > 0)
        {   
        if ( iStore != NULL )
                {
                delete iStore;
                iStore = NULL;
                }
        iStore = CCertStore::NewL();
        iStore->InitializeCertStore();
        HBufC* certLabel = iStore->GetCurrentCertLabel();       
        const TDesC8& fingerPrintSilent = iStore->RetrieveCertFPrint(*certLabel);      
        
        // Compare the fingerprints of cert stored with received from server  
        TInt result = fingerPrintSilent.Compare(fingerPrint); 
        RDEBUG_2("CorporateUserAcceptFunction FP Match ( %d )", result ); 
        
        if(result==KErrNone)
            {
            UpdateSilentTrustServerId();
            response = EUserAccept;
            showUserScreen = EFalse;
            
            // call device dialog to show notification                
            _LIT(KDialogText, "Silent Trust Done!");
            _LIT(KDialogTitle, "TRUST");
            _LIT(KDialogIcon, "qtg_large_info");                
            CHbDeviceNotificationDialogSymbian* dialog = CHbDeviceNotificationDialogSymbian::NewL();
            CleanupStack::PushL(dialog);
            dialog->SetTextL(KDialogText);
            dialog->SetTitleL(KDialogTitle);
            dialog->SetIconNameL(KDialogIcon);
            dialog->ShowL();
            CleanupStack::PopAndDestroy(dialog);            
            }
               
        }

    if(showUserScreen)
        {       
        CProcessorClient *client = new CProcessorClient();
        TInt res = client->LaunchDialog(ptr, name);
        
        if(res == 0)
            response = EUserAccept;
        else
            response = EUserDeny; 
        }


	MakeBooleanResponseL( response == EUserAccept, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::OrFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::OrFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	TBool valueTrue = EFalse;
	for ( TInt i(0); i < aParams.Count(); i++)
	{
		CAttributeValue * attribute = aParams[i];
		__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));

	
		if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			HandleErrorL( ParserErrors::IncombatibleDataType);	
		}
		
		if ( attribute->Data()->Value() == PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue )
		{
			valueTrue = ETrue;
			break;
		} 	
	}
	
	MakeBooleanResponseL( valueTrue, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::EqualFunctionsL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::EqualFunctionsL( const RParameterList& aParams, CAttributeValue* aResponseElement )
	{
	TInt count = aParams.Count();
	if( count <= 1 )
		{
		MakeBooleanResponseL( EFalse, aResponseElement );
		}
	else
		{
		__ASSERT_ALWAYS ( aParams.Count() == 2, User::Panic(PolicyParserPanic, KErrCorrupt));

		CAttributeValue * value1 = aParams[0];
		CAttributeValue * value2 = aParams[1];
		__ASSERT_ALWAYS ( value1, User::Panic(PolicyParserPanic, KErrCorrupt));
		__ASSERT_ALWAYS ( value2, User::Panic(PolicyParserPanic, KErrCorrupt));

		TInt value = value1->Data()->Compare( value2->Data());

		MakeBooleanResponseL( 0 == value, aResponseElement );
		}
	}



// -----------------------------------------------------------------------------
// CPolicyProcessor::AndFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::AndFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	TBool valueTrue = ETrue;
	for ( TInt i(0); i < aParams.Count(); i++)
	{
		CAttributeValue * attribute = aParams[i];
		__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));
	
		if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			HandleErrorL( ParserErrors::IncombatibleDataType);	
		}
		
		if ( attribute->Data()->Value() != PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue )
		{
			valueTrue = EFalse;
			break;
		} 	
	}
	
	MakeBooleanResponseL( valueTrue, aResponseElement);

}

// -----------------------------------------------------------------------------
// CPolicyProcessor::NotFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::NotFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	if ( aParams.Count() != 1)
	{
		HandleErrorL( ParserErrors::IncombatibleParameters);	
	}
	
	CAttributeValue * attribute = aParams[0];
	__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));


	if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
	{
		HandleErrorL( ParserErrors::IncombatibleDataType);	
	}
		
	MakeBooleanResponseL( aParams[0]->Data()->Value() != PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue, aResponseElement);

}

// -----------------------------------------------------------------------------
// CPolicyProcessor::RuleTargetStructureFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::RuleTargetStructureFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	//We expect that param count is 3n...
	if ( (aParams.Count() % 3) != 0)
	{
		HandleErrorL( ParserErrors::IncombatibleParameters);
	}
	
	if ( !iManagementTargetPolicy )
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}

	TBool validRule = EFalse;	
	
	//this operation is only valid for rules
	if ( iManagementTargetPolicy->ElementType() == ERule )
	{
		CRule * rule = (CRule*)iManagementTargetPolicy;
		CTarget * target = rule->GetTarget();
		__ASSERT_ALWAYS ( target, User::Panic(PolicyParserPanic, KErrCorrupt));
	
		for ( TInt i = 0; i < aParams.Count(); i++)
		{
			if ( aParams[i]->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::StringDataType)
			{
				HandleErrorL( ParserErrors::IncombatibleDataType);	
			}
		}
	
		//get subject/actions/resource/environment and compare their match content to parameters
		RMatchObjectArray container;
		CleanupClosePushL( container);
		target->GetMatchContainersL( container);

		validRule = ETrue;

		//read one subject/action/resource/environment group at time
		for ( TInt i = 0; i < aParams.Count() / 3 && validRule; i++)
		{
			validRule = EFalse;
			for ( TInt j = 0; j < container.Count() && !validRule; j++)
			{
				validRule = container[j]->CheckMatchObject( aParams[ i*3]->Data()->Value(),
								  							aParams[ (i*3)+1]->Data()->Value(),
														 	aParams[ (i*3)+2]->Data()->Value());
			}
		}		
	
		CleanupStack::PopAndDestroy( &container);
	}
	
	MakeBooleanResponseL( validRule, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::CertificateForSessionL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::CertificateForSessionL( CAttributeValue* aResponseElement )
{

	if ( !iTrustedSession)
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}
	
	MakeBooleanResponseL( iTrustedSession->CertificatedSession(), aResponseElement);
}


// -----------------------------------------------------------------------------
// CProcessorClient::CProcessorClient()
// -----------------------------------------------------------------------------
//

CProcessorClient::CProcessorClient()
    : CActive(EPriorityNormal)
    {
    CActiveScheduler::Add( this );
    iWait = new( ELeave ) CActiveSchedulerWait;
    iCompletionCode = KErrNone;
    }


// -----------------------------------------------------------------------------
// CProcessorClient::~CProcessorClient()
// -----------------------------------------------------------------------------
CProcessorClient::~CProcessorClient()
    {
    delete iWait;
    }


// -----------------------------------------------------------------------------
// CProcessorClient::DataReceived()
// -----------------------------------------------------------------------------
void CProcessorClient::DataReceived(CHbSymbianVariantMap& aData)
{
    _LIT(KResponse, "keyResponse");
    const CHbSymbianVariant* key = aData.Get(KResponse);
        
    if(key)
    {
    TInt *res = key->Value<TInt>();
    iCompletionCode = *res;        
    iUserResponse = *res;
    }
}


// -----------------------------------------------------------------------------
// CProcessorClient::DeviceDialogClosed()
// -----------------------------------------------------------------------------
void CProcessorClient::DeviceDialogClosed(TInt aCompletionCode)
    {
    iCompletionCode = aCompletionCode;
    //iDevDialog->Cancel();
    TRequestStatus* status(&iStatus);
    User::RequestComplete(status, KErrNone);
    }


// -----------------------------------------------------------------------------
// CProcessorClient::DoCancel()
// -----------------------------------------------------------------------------
void CProcessorClient::DoCancel()
{
    if (iWait && iWait->IsStarted() && iWait->CanStopNow()) 
    {
        iCompletionCode = KErrCancel;
        iWait->AsyncStop();
    }  
}


// -----------------------------------------------------------------------------
// CProcessorClient::RunL()
// -----------------------------------------------------------------------------
void CProcessorClient::RunL()
{
    if (iWait) 
    {
        iWait->AsyncStop();
    }    
}


// -----------------------------------------------------------------------------
// CProcessorClient::LaunchDialog()
// -----------------------------------------------------------------------------
TInt CProcessorClient::LaunchDialog(const TDesC8& aFringerPrint,
        const TDesC8& aServerName)
    {
    _LIT(KHbNotifier,"com.nokia.hb.policymanagementdialog/1.0");
    _LIT(KFingerPrint, "fingerprint");
    _LIT(KServerdisplayname, "serverdisplayname");

    RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &aFringerPrint);
    RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &aServerName);

    CHbSymbianVariantMap* varMap = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL(varMap);

    TBuf<10> fingerBuf;
    fingerBuf.Copy(aFringerPrint);

    TInt serverNameLen = aServerName.Length();
    TBuf<50> serverName;
    serverName.Copy(aServerName);

    RDEBUG_2("CPolicyProcessor::16 fingerPrint: %S", &fingerBuf);
    RDEBUG_2("CPolicyProcessor::16 serverName : %S", &serverName);

    CHbSymbianVariant* fingerprintid = CHbSymbianVariant::NewL(&fingerBuf,
            CHbSymbianVariant::EDes);

    CHbSymbianVariant* serverdisplayname = CHbSymbianVariant::NewL(
            &serverName, CHbSymbianVariant::EDes);

    RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &fingerprintid);
    RDEBUG_2("CPolicyProcessor::fingerPrint: %S", &serverdisplayname);

    varMap->Add(KFingerPrint, fingerprintid);
    varMap->Add(KServerdisplayname, serverdisplayname);

    iDevDialog = CHbDeviceDialogSymbian::NewL();
    TInt err1 = iDevDialog->Show(KHbNotifier, *varMap, this);
    TInt err = WaitUntilDeviceDialogClosed();

    CleanupStack::PopAndDestroy();

    if (iDevDialog)
        {
        iDevDialog->Cancel();
        delete iDevDialog;
        iDevDialog = NULL;
        }
    
    if (err == 0)
        {
            LaunchTrustNotificationDialog(aServerName);
         }
    
    return iUserResponse;
    }


// -----------------------------------------------------------------------------
// CProcessorClient::WaitUntilDeviceDialogClosed()
// -----------------------------------------------------------------------------
TInt CProcessorClient::WaitUntilDeviceDialogClosed()
    {
    iCompletionCode = KErrInUse;
    if (!IsActive() && iWait && !iWait->IsStarted())
    {
            iStatus = KRequestPending;
            SetActive();
            iWait->Start();
            }
    return iCompletionCode;
    }

void CProcessorClient::LaunchTrustNotificationDialog(const TDesC8& aServerName)
{
    _LIT(KFileName, "deviceupdates_");
    _LIT(KPath, "z:/resource/qt/translations/");
    _LIT(KDialogIcon, "note_info.svg");
    
    TBool result = HbTextResolverSymbian::Init(KFileName, KPath);
    
    if (result) {
        _LIT(KTrustEstablished,"txt_device_update_dpophead_trust_established");
        _LIT(KServerID,"txt_deviceupdate_dpopinfo_trust_establised_with_1");
        
        HBufC* trustEstablishedText = HbTextResolverSymbian::LoadL(KTrustEstablished);
        CleanupStack::PushL(trustEstablishedText);
 
        HBufC* serveridbuf = HBufC::NewLC(aServerName.Length());
        TPtr serveridbufptr = serveridbuf->Des();
        serveridbufptr.Copy(aServerName);
   
        HBufC* serverid = HbTextResolverSymbian::LoadL(KServerID,*serveridbuf);
        CleanupStack::PushL(serverid);

        CHbDeviceNotificationDialogSymbian::NotificationL(KDialogIcon, *trustEstablishedText, *serverid);
        
        CleanupStack::PopAndDestroy(3); //trustEstablishedText,serveridbuf,serverid
    }
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::TCombiningAlgorith()
// -----------------------------------------------------------------------------
//
TCombiningAlgorith::TCombiningAlgorith( const TDesC8 &aAlgorithId)
{
	//set properties 
	iResultReady = iAtLeastOneError = iPotentialDeny = EFalse;
	iPotentialPermit = iAtLeastOneDeny = iAtLeastOnePermit = EFalse;

	//set algorithm specific properties....
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RuleDenyOverrides)	
	{
		iAlgorithm = ERuleDenyOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else 
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyDenyOverrides)	
	{
		iAlgorithm = EPolicyDenyOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RulePermitOverrides)	
	{
		iAlgorithm = ERulePermitOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else 
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyPermitOverrides)	
	{
		iAlgorithm = EPolicyPermitOverrides;
		iCurrentResponse = ENotApplicable;
	}
	
	
	iResultReady = EFalse;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::AddInput()
// -----------------------------------------------------------------------------
//
		
TBool TCombiningAlgorith::AddInput( const TMatchResponse& aInput, const TMatchResponse& aEffect)
{
	//for rule algorithms, also "aEffect" has effect to result

	//if result is ready return it...	
	if ( iResultReady )
	{
		return iResultReady;	
	}

	//select algorithm...
	switch ( iAlgorithm)
	{
		case ERuleDenyOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EDeny )
			{
				iCurrentResponse = EDeny;
				iResultReady = ETrue;		
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
				if ( aEffect == EDeny )
				{
					iPotentialDeny = ETrue;
				}
			}
			else if ( aInput == EPermit)
			{
				iAtLeastOnePermit = ETrue;
			}
		}
		break;
		case ERulePermitOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EPermit )
			{
				iCurrentResponse = EPermit;
				iResultReady = ETrue;		
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
				if ( aEffect == EPermit )
				{
					iPotentialPermit = ETrue;
				}
			}
			else if ( aInput == EDeny)
			{
				iAtLeastOneDeny = ETrue;
			}
		}
	}
	
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::AddInput()
// -----------------------------------------------------------------------------
//

TBool TCombiningAlgorith::AddInput( const TMatchResponse& aInput)
{
	//for policy algorithms

	//if result is ready return it...	
	if ( iResultReady )
	{
		return iResultReady;	
	}

	switch ( iAlgorithm)
	{
		case EPolicyDenyOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EDeny || aInput == EIndeterminate)
			{
				iCurrentResponse = EDeny;
				iResultReady = ETrue;		
			}
			else if ( aInput == EPermit)
			{
				iAtLeastOnePermit = ETrue;
			}
		}
		break;
		case EPolicyPermitOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EPermit ) 
			{
				iCurrentResponse = EPermit;
				iResultReady = ETrue;		
			}
			else if ( aInput == EDeny)
			{
				iAtLeastOneDeny = ETrue;
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
			}
		}
		break;
	}
	
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::ResultReady()
// -----------------------------------------------------------------------------
//

TBool TCombiningAlgorith::ResultReady()
{
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::Result()
// -----------------------------------------------------------------------------
//

TMatchResponse TCombiningAlgorith::Result()
{
	//default...
	TMatchResponse response( ENotApplicable);
	
	if ( !iResultReady)
	{
		//if result is not ready
		switch ( iAlgorithm)
		{
			case ERuleDenyOverrides:
			{	
				//algorithm specific handling.... 
				if ( iPotentialDeny ) response = EIndeterminate; else
				if ( iAtLeastOnePermit ) response = EPermit; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
			case ERulePermitOverrides:
			{
				//algorithm specific handling.... 
				if ( iPotentialPermit ) response = EIndeterminate; else
				if ( iAtLeastOneDeny ) response = EDeny; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
			case EPolicyDenyOverrides:
			{	
				//algorithm specific handling.... 
				if ( iAtLeastOnePermit ) response = EPermit;
			}
			break;
			case EPolicyPermitOverrides:
			{
				//algorithm specific handling.... 
				if ( iAtLeastOneDeny ) response = EDeny; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
		}
		
		iCurrentResponse = response;
		iResultReady = ETrue;
		
	}
	else
	{
		//and if result is ready return it
		response = iCurrentResponse;
	}
	
#ifdef _DEBUG
	switch ( response)
	{
		case EDeny:
			RDEBUG("PolicyEngineServer: DENY");
		break;
		case EPermit:
			RDEBUG("PolicyEngineServer: PERMIT");
		break;
		case EIndeterminate:
			RDEBUG("PolicyEngineServer: INDETERMINATE");
		break;
		default:
			RDEBUG("PolicyEngineServer: NOT APPLICABLE");
		break;
	};
#endif	
	
	return response;
}


void CCertStore::ConstructL()
    {
    RDEBUG("CCertStore::ConstructL" );

    }

CCertStore::CCertStore():CActive(EPriorityStandard)
    {
    RDEBUG("CCertStore::CCertStore" );
    iCertState = EReady;
    iCertStore = NULL;
   
    CActiveScheduler::Add(this);
    iWait = new CActiveSchedulerWait();
    }

CCertStore::~CCertStore()
    {
    RDEBUG("CCertStore::~CCertStore" );

    delete iWait;     
    delete iCertLabel;
    iCertLabel = NULL;    
    delete iCertBuffer;
    iCertBuffer = NULL;    
    delete iCertStore;
    iCertStore = NULL;
  
    }

CCertStore* CCertStore::NewL()
    { 
    RDEBUG("CCertStore::NewL" );

    CCertStore* self = new(ELeave) CCertStore();   
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }


void CCertStore::InitializeCertStore()
    {
    RDEBUG("CCertStore::InitializeCertStore");
    if(iCertStore == NULL)
        {
        RFs* fs = new RFs();
        fs->Connect();
        iCertStore = CUnifiedCertStore::NewL(*fs, ETrue);         
         
        iCertStore->Initialize(iStatus);
        iCertState = EInitializeStore;    
       
        SetActive();   
        iWait->Start();
        TInt res = iStatus.Int();
        }     
    }



// Checks for the existence of a certificate
 // -----------------------------------------------------------------------------
 TBool CCertStore::CheckCertInStore( const TDesC& aLabel )
     {
     RDEBUG("CCertStore::CheckCertInStore");

    // Create filter
    CCertAttributeFilter* filter = CCertAttributeFilter::NewL();
    TCertLabel label( aLabel );
    filter->SetLabel( label ); 
    filter->SetFormat(EX509Certificate);
    filter->SetOwnerType(ECACertificate);
    iCertState = EExistsInStore;
    
    iCertStore->List( iCertInfoArray, *filter, iStatus );   
    SetActive();    
    iWait->Start();
    
    delete filter;
    TBool retVal = ( iCertInfoArray.Count() > 0 ); 
    
    if(retVal)
      {
      iCertInfoRetrieved = iCertInfoArray[0];
      }            
    return retVal;    
    }
 

TInt CCertStore::RemoveCert(const TDesC& aCertLabel)
    { 
    RDEBUG("CCertStore::RemoveCert");
    if(CheckCertInStore(aCertLabel))
        {        
        MCTWritableCertStore& writableStore = iCertStore->WritableCertStore(iStoreIndex);         
        writableStore.Remove(*iCertInfoRetrieved, iStatus);        
        iCertState = ERemoveCert;       
        WaitUntilOperationDone();               
        return KErrNone;             
        }
    else
        return KErrNotFound;                 
    }


TInt CCertStore::GenerateNewCertLabel()
    {
    RDEBUG("CCertStore::GenerateNewCertLabel");
    TInt result = KErrNone;
    TInt counter=0;
   
    TRAPD( err, 
            {
            CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
            CleanupStack::PushL( rep );           
            result = rep->Get( KCertificateCounter, counter );
            RDEBUG_2("CCertStore::GenerateNewCertLabel( %d )", result );
            CleanupStack::PopAndDestroy( rep );
            } );
    if( err != KErrNone )
        {
        result = err;
        return err;
        }   
    iCertLabel=HBufC::NewL(KMaxLabelIdLength);     
    // use central repository to generte new label each time for addition
    *iCertLabel = KDMTrustedCertLabel;
    TPtr ptr = iCertLabel->Des();
    counter++;
    ptr.AppendNum(counter);
    iLabelCounter = counter;
 
    return result;
    }




void CCertStore::UpdateLabelCounterInCR()
    {
    RDEBUG("CCertStore::UpdateLabelCounterInCR");

    TInt res;
    //update the CR with new counter value
    TRAPD( err1, 
           {
           CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
           CleanupStack::PushL( rep );
           res = rep->Set( KCertificateCounter, iLabelCounter );
           RDEBUG_2("CCertStore::UpdateLabelCounterInCR( %d )", res );
           CleanupStack::PopAndDestroy( rep );
           } );

    }


HBufC* CCertStore::GetCurrentCertLabel()
    {  
    RDEBUG("CCertStore::GetCurrentCertLabel");

    TInt counter;
    TRAPD( err, 
           {
           CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
           CleanupStack::PushL( rep );
           TInt res = rep->Get( KCertificateCounter, counter );           
           CleanupStack::PopAndDestroy( rep );
           } );
    if(iCertLabel)
        {
        delete iCertLabel;
        iCertLabel = NULL;
        }
    iCertLabel=HBufC::NewL(KMaxLabelIdLength);     
    // use central repository to generte new label each time for addition
    *iCertLabel = KDMTrustedCertLabel;
    TPtr ptr = iCertLabel->Des();   
    ptr.AppendNum(counter);
    return iCertLabel;
    }


TInt CCertStore::AddCert(HBufC8 *aCertBuffer)
    {  
    RDEBUG("CCertStore::AddCert");

    GenerateNewCertLabel();  
    //create object of writable certificate store 
    MCTWritableCertStore& writableStore = iCertStore->WritableCertStore(iStoreIndex);
  
    writableStore.Add(*iCertLabel,EX509Certificate, ECACertificate, NULL, NULL, *aCertBuffer,EFalse,iStatus);
    iCertState = EAddCert;  
    //SetActive();
    WaitUntilOperationDone();    
    return iStatus.Int();      
    }


void CCertStore::WaitUntilOperationDone()
    {
    SetActive();
    iWait->Start();
    }


//Sets the writable certificate store index used for addition and removal
void CCertStore::GetWritableCertStoreIndex()
    {
    RDEBUG("CCertStore::GetWritableCertStoreIndex");

    //count total availiable cert stores.
    TInt count = iCertStore->WritableCertStoreCount();                
    
    TBool found = EFalse;
    TInt i = 0;
    TPtrC pElementID(KSymbianCertificateStoreLabel());
    
    for (i = 0; (i < count) && (!found); i++)
      {
      // Select the first store with the specified label.
     const TDesC& storeLabel = iCertStore->WritableCertStore(i).Token().Label();
     if (storeLabel == pElementID)
            {
            found = ETrue;
            iStoreIndex = i;
            }                
      }

    }

const TDesC8&  CCertStore::RetrieveCertFPrint(const TDesC& aCertLabel)
    {
    RDEBUG("CCertStore::RetrieveCertFPrint");

    if(CheckCertInStore(aCertLabel))
        {
        MCTWritableCertStore& WritableStore = iCertStore->WritableCertStore(iStoreIndex);        
        
        iCertData = HBufC8::NewMaxL(iCertInfoRetrieved->Size());        
        TPtr8 ptr = iCertData->Des();
        
        WritableStore.Retrieve(*iCertInfoRetrieved,ptr,iStatus);    
        
        iCertState = EGetFPrint;
        
        WaitUntilOperationDone();
        RDEBUG("CCertStore::RetrieveCertFPrint");
        return iFingerPrint;
        
        }
    else
      
        RDEBUG("CCertStore::RetrieveCertFPrint-- not Found");
        return TDesC8(KNoKeyFound) ;
   
    
    
    }

void CCertStore::RunL()
    {
    RDEBUG("CCertStore::RunL");

    TInt result = iStatus.Int();
        if (iStatus == KErrNone) 
            {
            switch (iCertState) {              
                    
                case EInitializeStore:
                    {                                 
                    GetWritableCertStoreIndex();
                    iWait->AsyncStop();  
                    break;
                    }
                case ERemoveCert:                    
                case EAddCert: 
                case EExistsInStore:    
                    {                   
                    iWait->AsyncStop();                       
                    break;
                    }   
                    
               case EGetFPrint:
                   {
                   iWait->AsyncStop();
                    CX509Certificate* certR = CX509Certificate::NewL(*iCertData);  
                                       
                    HBufC8* fingerprint = certR->Fingerprint().AllocL(); 
                
                    TBuf8<20> fingerP;                
                    fingerP.Copy(fingerprint->Des());
                    
                                   
                    iFingerPrint.Zero();
                    
                    //convert fingerprint to plain text
                    for ( TInt i(0); i < fingerP.Length(); i++)
                      {
                      iFingerPrint.AppendNumFixedWidth(fingerP[i] , EHex,2);
                      iFingerPrint.UpperCase();
                      }                      
                  
                   delete fingerprint;
                   delete certR;
                   certR = NULL;
                   break;                   
                   }                   
              
              default:
                    break;       
            } 
            
           }
        else
            {
            if(iWait)
                {
                iWait->AsyncStop();
                }
            }
                
          
    }

void CCertStore::DoCancel()
    {
    RDEBUG("CCertStore::DoCancel");

    if (iWait && iWait->IsStarted() && iWait->CanStopNow()) 
        {        
        iWait->AsyncStop();
        }  
    switch( iCertState )
       {
          case EInitializeStore:          
               {
               iCertStore->CancelInitialize();
               break;
               }
       }  

    }
