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

#ifndef __FMS_CLIENT_H__
#define __FMS_CLIENT_H__

#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include "fmsclientserver.h"


class RFMSClient : public RSessionBase
	{
	public:
		/**
		 * Launches FMSServer
		 * @param None
		 * @return KErrNone Symbian error code
		 */
		IMPORT_C TInt OpenL();
		
		/**
		 * Close server connection
		 * @param None
		 * @return None
		 */
		IMPORT_C void Close();
		
		/**
		 * Sending the parameters to FMSServer
		 * FotaServer or test app should use this
		 * @param aReason
		 * @param aBearer
		 * @param aDrive
		 * @param aSize
		 * @return None
		 */
		IMPORT_C void NotifyForResumeL(const TFmsIpcCommands& aReason, 
		  const TInt& aBearer, const TDriveNumber& aDrive, const TInt& aSize);						
		
		
		 /**
		  * phone call active check to FMSServer
		  * @param aCallActive, on return holds call status
		  * @return TInt, error code
		  */
		 IMPORT_C TInt IsPhoneCallActive(TInt& aCallActive);
		 
		 
		 IMPORT_C void NotifyForUpdateL(const TFmsIpcCommands& aReason, TUint aLevel = 2);
		 
	     /**
	         * Cancels the request on FMSServer
	         * @param None
	         * @return None
	         */
         IMPORT_C void Cancel();
		 
	private:
		/* 
		 * FotaStartUp uses this at the time of reboot 
		 * to trigger FMS server
		 * @param None
		 * @return None
		 */
		 void SendInterruptParamsL();
};

#endif
