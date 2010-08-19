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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __ABMSMLHELPER_H__
#define __ABMSMLHELPER_H__

#include <SyncMLDef.h>
#include <SyncMLTransportProperties.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>

class SmlHelper
        {
public:

        // NOTE: this enum must match with property array described in 
        // SyncML_Sync_Agent_Client_API_Specification.doc.
        enum TNSmlSyncTransportProperties
            {
            EPropertyIntenetAccessPoint = 0,
            EPropertyHttpUsed = 5,
            EPropertyHttpUserName = 6,
            EPropertyHttpPassword = 7
            };

        /**
         * Fetch the default IAP used by the currently active DM profile
         * @param aDefaultIAP on successfull return contains the default IAP number
         */

        static void GetDefaultIAPFromDMProfileL(TInt& aDefaultIAP);

        static void GetConnectionPropertyNameL(RSyncMLSession& aSyncSession,
                TDes8& aText, TInt aPropertyPos);

        static TInt StrToInt(const TDesC& aText, TInt& aNum);
        };
	
#endif

//  End of File
