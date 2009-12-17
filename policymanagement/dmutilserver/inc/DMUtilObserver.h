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


#ifndef __DMUTIL_OBSERVER_HEADER__
#define __DMUTIL_OBSERVER_HEADER__

// INCLUDES

#include <e32base.h>

// CONSTANTS

enum TPolicyManagementEventTypes
{
	EPolicyChangedEvent = 0,
};

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CActiveObserver;

// CLASS DECLARATION

class TDMUtilPassiveObserver
{
	public:
		/**
		* Check is policies changed after the function was called last time 
	    * @return ETrue if policies is changed after the function was called last time 
        */
		static IMPORT_C TBool IsPolicyChangedL();

		/**
		* Check is RFS performed after the function was called last time 
	    * @return ETrue if RFS is performed after the function was called last time 
        */
		static IMPORT_C TBool IsRFSPerformedL();
	private:
};

/**
* Callback interface for active observer
*/
class MActiveCallBackObserver
{
	public:
		/**
		* Called when observed event is launched.
        */
		virtual void EventLaunchedL() = 0;
	private:
};

class CDMUtilActiveObserver : public CBase
{
	public:
		/**
		* Constructor for CDMUtilActiveObserver
	    * @param aType Event to be observed
	    * @return New CDMUtilActiveObserver object
        */
		IMPORT_C static CDMUtilActiveObserver* NewL( TPolicyManagementEventTypes aType);
	
		/**
		* Subscribe event, callback interface implementation is called when event launched 
	    * @param aDynamicCallBackObserver Object which implements callback interface
        */		
        IMPORT_C void SubscribeEventL( MActiveCallBackObserver* aDynamicCallBackObserver);
		/**
		* Unsubscribe event
        */		
		IMPORT_C void UnSubscribeEvent();

		/**
		* Destructor
        */		
		virtual ~CDMUtilActiveObserver();
	private:	
		CDMUtilActiveObserver();	
	private:
	 	CActiveObserver* iActiveObserver;
};


#endif // __DMUTIL_OBSERVER_HEADER__