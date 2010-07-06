/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/

#ifndef __FMS_SESSION_H__
#define __FMS_SESSION_H__

#include "fmsserver.h"

class CFMSSession: public CSession2
{
	friend class CFMSServer;
	public:
		/**
		 * Destructor
		 */
		virtual ~CFMSSession();		
	
	private:
		/**
		 * Constructor
		 */ 
		CFMSSession();
		
		/**
		 * Provides FMS Server reference
		 * @param None.
		 * @return CFMSServer reference
		 */ 
		CFMSServer& Server();
		
		/**
		 * Called by the framework to handle the request 	 
		 * @param aMessage
		 * @return None
		 */
		void ServiceL(const RMessage2& aMessage);
		
		/**
		 * Called from ServiceL to handle the request 
		 * @param aMessage
		 * @return None
		 */
		void DispatchMessageL(const RMessage2& aMessage);
		
		
		/**
		 * Called when ServiceL leaves
		 * @param aMessage
		 * @param aError
		 * @return None
		 */
		void ServiceError(const RMessage2 &aMessage, TInt aError);
		
		/**
		 * Checks whether the session started client is secured or not	 
		 * @param aMessage
		 * @return TBool
		 */
		TBool CheckClientSecureIdL( const RMessage2 &aMessage );
		
		/**
		 * Finds the bearer id for the corresponding IAP Id		 
		 * @param aIapId
		 * @return TUint32, bearer id
		 */
		TUint32 FindBearerIdL(TInt aIapId);
		
		/**
		 * Writes Dummy file in case of unsupported monitoring		 
		 * @param None.
		 * @return CFMSServer reference
		 */
		void WriteDummyFile();
		
		/**
		 * Checks whether the phone is in 2G or 3G mode
		 * sets the member variable iWcdmaBearer
		 * @param None.
		 * @return CFMSServer reference
		 */
		void PhoneModeL();
		
		/**
		 * 
		 * Handles the service based on bearer type
		 * @param aMessage.
		 * @return None
		 */
			void HandleBasedOnBearertypeL(const RMessage2& aMessage);
	private:	
		TBool iStopServer;
		TBool iWlanbearer;
		TBool iWcdmaBearer;
};


#endif
