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


#ifndef __DMUTIL_SESSION_H__
#define __DMUTIL_SESSION_H__

// INCLUDES
#include <e32base.h>
#include "DMUtilServer.h"

//FORWARD DECLARATIONS

class CACLStorage;

/**
*  CDMUtilSession 
*  Description.
*/

class CDMUtilSession : public CSession2
	{
	friend class CDMUtilServer;

public:
	~CDMUtilSession();
	void CreateL();

private:
	CDMUtilSession();
	CDMUtilServer& Server();

	void ServiceL( const RMessage2& aMessage );
	void DispatchMessageL( const RMessage2& aMessage );
	void ServiceError( const RMessage2& aMessage, TInt aError );
	
	void UpdateStatusFlagsL( KPolicyMngStatusFlags aUpdateCommand);
	static void SetIndicatorStateL( CRepository * aCenRep);
	static void DefinePropertiesL();
	TInt PerformRFSL();
	void MarkMMCWipeL();
private:	
	CACLStorage* iACLStorage;
	


};

#endif //__DMUTIL_SESSION_H__
