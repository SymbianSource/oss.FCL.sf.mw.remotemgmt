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


#ifndef __DMUTIL_ACTIVEOBSERVER_HEADER__
#define __DMUTIL_ACTIVEOBSERVER_HEADER__

// INCLUDES

#include "DMUtilObserver.h"
#include <e32property.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

class CActiveObserver : public CActive
{
	public:
		static CActiveObserver* NewL( TPolicyManagementEventTypes aType);
	
		void SubscribeEventL( MActiveCallBackObserver* aDynamicCallBackObserver);
		void UnSubscribeEvent();
		void DoCancel();

		virtual ~CActiveObserver();
	protected:
		void RunL();
		TInt RunError(TInt aError);
	private:	
		CActiveObserver( TPolicyManagementEventTypes aType);	
		TUint32 SubscribeKey( TPolicyManagementEventTypes aType);		
	private: 
		RPointerArray<MActiveCallBackObserver> iSubcribers;
		TPolicyManagementEventTypes type;
		RProperty iProperty;
		TBool iSubscribed;
};


#endif // __DMUTIL_ACTIVEOBSERVER_HEADER__