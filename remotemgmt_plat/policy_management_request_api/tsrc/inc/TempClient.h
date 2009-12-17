/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



#ifndef __Temp_CLIENT_H__
#define __Temp_CLIENT_H__



#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include <badesca.h>
#include <s32mem.h>





class RTempClient : public RSessionBase
{
	public:
	    /**
		* Connect Connects client side to Screen saver server
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Connect();

	    /**
		* Close Close server connection
        */
		IMPORT_C void Close();
		
		/**
			Use this method to get screen saver list information like name,version,display name
		
			IMPORT_C void GetTempListInfoL(RSSListInfoPtrArray &aTempList);
		**/
		
		IMPORT_C void PerformRFSL();
		
	};




#endif
