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
* Description:  Alert queue and handler
*
*/


// INCLUDE FILES
#include "NSmlAlertQueue.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNSmlParserFactory::CreateAlertParserL( TSmlUsageType aType, TSmlProtocolVersion aVersion, CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo  )
// Creates an instance of alert parser
// ---------------------------------------------------------
CNSmlMessageParserBase* NSmlParserFactory::CreateAlertParserL( TSmlUsageType aType, TSmlProtocolVersion aVersion, CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo  )
	{
	
	aAlertInfo.SetProtocolType( aType );
	
	if (aType == ESmlDataSync)
		{
		if ( aVersion == ESmlVersion1_1_2 )
			{
			//parse data sync version 1.1 alert
			return new (ELeave) CNSmlDSAlertParser11( aAlertInfo, aHistoryInfo );
			}			
		else
			{
			//parse data sync version 1.2 alert
			return new (ELeave) CNSmlDSAlertParser12( aAlertInfo, aHistoryInfo );	
			}
			
		}
	else
		{
		//parse device management version 1.1 alert
		return new (ELeave) CNSmlDMAlertParser11( aAlertInfo, aHistoryInfo );	
		}
	}

//End of File
