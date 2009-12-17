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


#ifndef __CENTREPTOOLCLIENT_H__
#define __CENTREPTOOLCLIENT_H__

// INCLUDES

#include <e32std.h>
#include "CentRepToolClientServer.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  RPolicyEngine class to establish PolicyEngine server connection
*/

class RCentRepTool : public RSessionBase
{
	public:
	    /**
		* Connect Connects client side to policy engine server
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Connect();
		
	    /**
		* Close Close server connection
        */
		IMPORT_C void Close();

		IMPORT_C TInt PerformRFS();
		IMPORT_C TInt CheckCommitState();
	private:	
};

/**
*  RPolicyManagement class to handle policy management sessions
*/
	
class RSettingManagement : public RSubSessionBase
{
	public:
		IMPORT_C RSettingManagement();
	
		//Session management		
	    /**
		* Connect Open client side session
		* @param aServer Reference to connected RPolicyEngine
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Open( TUid aRepositoryId, RCentRepTool &aServer);
		
		/**
		* Close Close session
        */
		IMPORT_C void Close();
		
		IMPORT_C void InitSession( TRequestStatus& iRequestStatus);
		
		IMPORT_C TInt SetSIDWRForSetting( TUint32 aSettingId, TUid aSecurityId); 
		IMPORT_C TInt RestoreSetting( TUint32 aSettingId); 
		
		IMPORT_C TInt SetSIDWRForRange( TUint32 aRangeStart, TUint32 aRangeEnd, TUid aSecurityId);
		IMPORT_C TInt RestoreRange( TUint32 aRangeStart, TUint32 aRangeEnd); 

		IMPORT_C TInt SetSIDWRForMask( TUint32 aCompareValue, TUint32 aMask, TUid aSecurityId);
		IMPORT_C TInt RestoreMask( TUint32 aCompareValue, TUint32 aMask); 
	
		IMPORT_C TInt AddSIDWRForDefaults( TUid aSecurityId);
		IMPORT_C TInt RestoreDefaults();

		IMPORT_C TInt RemoveBackupFlagForMask( TUint32 aCompareValue, TUint32 aMask);
		IMPORT_C TInt RestoreBackupFlagForMask( TUint32 aCompareValue, TUint32 aMask);

		IMPORT_C TInt RemoveBackupFlagForRange( TUint32 aRangeStart, TUint32 aRangeEnd);
		IMPORT_C TInt RestoreBackupFlagForRange( TUint32 aRangeStart, TUint32 aRangeEnd);
		
		IMPORT_C TInt RemoveBackupFlagForDefaults();
		IMPORT_C TInt RestoreBackupFlagForDefaults();

		IMPORT_C TBool CheckAccess( TUint32 aSettingId, TAccessType aAccessType);

		IMPORT_C TInt Flush();
	private:
		TBool iConnected;
};

class RAccessCheck : public RSubSessionBase
{
	public:
		IMPORT_C TInt Open( TUid aRepositoryId, RCentRepTool &aServer);
		IMPORT_C void Close();

		IMPORT_C TInt CheckAccess( TUint32 aSettingId, TAccessType aAccessType, TBool& aAccessValue);
	private:
};



#endif //__POLICYENGINECLIENT_H__


