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
* Description: Implementation of DM Device Dialogs
* 	This is part of syncmlfw.
*/

#ifndef __DMDEVDIALOG_SESSION_H__
#define __DMDEVDIALOG_SESSION_H__

#include "dmdevicedialogserver.h"

class CDmDeviceDialogSession: public CSession2
{
	friend class CDmDeviceDialogServer;
	public:
		/**
		 * Destructor
		 */
		virtual ~CDmDeviceDialogSession();
						
	private:
		/**
		 * Constructor
		 */ 
		CDmDeviceDialogSession();
		
		/**
		 * Provides DM Device dialog Server reference
		 * @param None.
		 * @return CDmDeviceDialogServer reference
		 */ 
		CDmDeviceDialogServer& Server();
		
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
		TBool CheckClientSecureId( const RMessage2 &aMessage );		
		
	private:			
		RMessage2 iMessage;		 		
};


#endif
