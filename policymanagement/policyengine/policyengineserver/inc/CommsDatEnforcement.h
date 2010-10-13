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


#ifndef _COMMSDAT_ENFORCEMENT_HEADER__
#define _COMMSDAT_ENFORCEMENT_HEADER__

// INCLUDES
#include <cmmanagerdef.h>
#include "SettingEnforcementManager.h"

#include "CentRepToolClient.h"
#include "DMUtilClient.h"
#include <e32base.h>
#include <SettingEnforcementInfo.h>
#include <commdb.h>

using namespace CMManager;
class RCmDestinationExt;

class CCommsDatEnforcement : public CEnforcementBase
    {
public:
    //construction
    CCommsDatEnforcement();
    virtual ~CCommsDatEnforcement();
    static CCommsDatEnforcement* NewL(const TDesC8& aEnforcementId);
    void ConstructL();

    //from MEnforcementBase
    void InitEnforcementL(TRequestStatus& aRequestStatus);
    void DoEnforcementL(TRequestStatus& aRequestStatus);
    TBool InitReady();
    TBool EnforcementReady();
    void FinishEnforcementL(TBool aFlushSettings);
    void ResetEnforcementL();

    void AccessRightList(RAccessControlList& aAccessControlList);

    static TBool ValidEnforcementElement(const TDesC8& aEnforcementId);
    TInt CheckTableL(const TDesC& aTableName);

    /**Sets Protection level to all the destinations
     * @param aProtLevel Protection level to be set on the destinations
     */
    void SetProtectionL(TProtectionLevel aProtLevel);
    
    /**Sets Protection level to all the Connection Methods in the  destination
     * @param destination Destination in which the Protection is set on the Connection Methods
     * @param aProtLevel Protection level to be set on the Connection Methods
     */
    void SetCMProtectionL(RCmDestinationExt& aDestination,
            TProtectionLevel aProtLevel);

private:
    //active state 
    TInt iInitState;
    TInt iEnforcementState;
    TBool iRestore;

    //editor list
    RAccessControlList* iAccessControlList;

    //CentRep tool 
    RCentRepTool iCentRepServer;

    //ACL editing
    RDMUtil iDMUtil;

    KSettingEnforcements iSettingType;

    };


#endif