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




#ifndef _POLICY_MANAGER_HEADER__
#define _POLICY_MANAGER_HEADER__

// INCLUDES

#include "PolicyEngineServer.h"
#include "PolicyStorage.h"
#include "PolicyEngineClientServer.h"
#include "OperationParser.h"
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CElementBase;
class CTrustedSession;
class CPolicyProcessor;
class CSettingEnforcementManager;

// CLASS DECLARATION

class CPolicyManager : public CActive
{
	public:
	
		static CPolicyManager * NewL( CPolicyProcessor * aPolicyProcessor);
		void ConstructL();
		CPolicyManager( CPolicyProcessor * aPolicyProcessor);
		~CPolicyManager();
	
		void ExecuteOperation( CTrustedSession * aSession, const RMessage2& aMessage);

		void NewElementL( const TInt& aLineOffset, const TDesC8& aTargetElement, const TDesC8& aElementDescription, const TBool& aUseBearerCertificate);
		void ReplaceElementL( const TInt& aLineOffset, const TDesC8& aTargetElement, const TDesC8& aElementDescription );
		void RemoveElementL( const TDesC8& aTargetElement);

		void GetElementListL( const RMessage2& aMessage);
		void GetElementInfoL( const RMessage2& aMessage);
		void GetElementXACMLDescriptionL( const RMessage2& aMessage);
		void IsServerIdValidL( const RMessage2& aMessage);
		void GetCertificateRoleL( const RMessage2& aMessage);
	protected:	
		// Derived from CActiveObject
		void RunL();
		void DoCancel();
		TInt RunError( TInt aError);		
	private:
		void CommitChangesL();
	
		void ReAssignElementL( const TDesC8& aTargetElement, CElementBase * aElement);
		void AssignElementL( const TDesC8& aTargetElement, CElementBase * aElement);
		void IsAssigmentAcceptableL( CElementBase * aParentElement, CElementBase * aElement, TBool aNewElement);

		void IsRemoveAcceptableL( CElementBase * aParentElement, CElementBase * aElement);
	
		void MakeEnforcementL(TInt aError);
		void CompleteOperation( TInt aError);
		
		void UpdateStateFlagsL();
		void AddSilentDMOperationServerIdL();
		TBool IsCurrentServerIdTrustedL();
		
	private:
		CPolicyProcessor* iPolicyProcessor;
		CTrustedSession* iTrustedSession;
		CSettingEnforcementManager* iSEManager;

		//for management operations which need two calls from client side
		HBufC8 * iActiveXACMLContent;
		HBufC8 * iActiveDescription;
		HBufC8 * iActiveChildList;
		HBufC8 * iActiveElementList;
		
		//management handling
		TBuf8<KMaxReturnMessageLength> iMsg;
		TBool iUseBearerCertificate;
		TBool iCertificateUpdates;
		CPolicyStorage* iPolicyStorage;
		
		//active object related
		const RMessage2 * iMessage;
		TInt iOperationStatus;
		HBufC8* iCurrentManagementCommand;
		COperationParser* iOperationParser;
};


#endif