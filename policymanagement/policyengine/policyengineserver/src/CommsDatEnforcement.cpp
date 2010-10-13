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
* Description: Implementation of policymanagement components
*
*/


// INCLUDE FILES


#include <commdb.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <cmconnectionmethodext.h>
#include "CommsDatEnforcement.h"
#include "XACMLconstants.h"
#include "debug.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"

#include <protectdb.h>

#include <commsdattypesv1_1.h>
#include <WlanCdbCols.h>
#include <d32dbms.h>
#include <metadatabase.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

_LIT( KCommsDatEnformentPanic, "CommsDat enforcement panic");
_LIT8( KAPURI, "AP");

const TInt KDelayBeforeProtection = 1000000; // 1 Sec
const TInt KMaxProtectionRetries = 10; 

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::CCommsDatEnforcement()
// -----------------------------------------------------------------------------
//
CCommsDatEnforcement::CCommsDatEnforcement()
	{
	RDEBUG("CCommsDatEnforcement::CCommsDatEnforcement()");
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::~CCommsDatEnforcement()
// -----------------------------------------------------------------------------
//
CCommsDatEnforcement::~CCommsDatEnforcement()
    {
    RDEBUG("CCommsDatEnforcement::~CCommsDatEnforcement()");

    iCentRepServer.Close();
    iDMUtil.Close();
    }


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ConstructL()
// -----------------------------------------------------------------------------
//		
void CCommsDatEnforcement::ConstructL()
    {
    RDEBUG("CCommsDatEnforcement::ConstructL()");
    //no implementation needed
    }

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::NewL()
// -----------------------------------------------------------------------------
//		
CCommsDatEnforcement* CCommsDatEnforcement::NewL(const TDesC8& aEnforcementId)
    {
    RDEBUG("CCommsDatEnforcement::NewL()");
	CCommsDatEnforcement* self = 0;
    

    if (aEnforcementId == PolicyLanguage::Constants::EAPEnforcementPolicy)
        {
        RDEBUG("	-> EAPEnforcementPolicy");
		self = new ( ELeave ) CCommsDatEnforcement();	
		self->ConstructL();
        self->iSettingType = EAPEnforcement;
        }

    else if (aEnforcementId
            == PolicyLanguage::Constants::EWLANEnforcementPolicy)
        {
        RDEBUG("    -> EWLANEnforcementPolicy");
		self = new ( ELeave ) CCommsDatEnforcement();	
		self->ConstructL();	
        self->iSettingType = EWLANEnforcement;
        }

	return self;
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ValidEnforcementElement()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::ValidEnforcementElement(
        const TDesC8& aEnforcementId)
    {
    RDEBUG("CCommsDatEnforcement::ValidEnforcementElement()");
    if (aEnforcementId == PolicyLanguage::Constants::EAPEnforcementPolicy
            || aEnforcementId
                    == PolicyLanguage::Constants::EWLANEnforcementPolicy)
        {
        RDEBUG("   -> valid EnforcementPolicy");
        return ETrue;
        }

    return EFalse;
    }

	
// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::InitEnforcementL(TRequestStatus& aRequestStatus)
    {
    RDEBUG("CCommsDatEnforcement::InitEnforcementL()");

    RDEBUG_2("CCommsDatEnforcement@@iInitState %d )", iInitState );
    //set restore flag
    if (iAccessControlList->Count())
        {
        iRestore = EFalse;
        }
    else
        {
        iRestore = ETrue;
        iInitState++;
        }

    //in first phase open connections to centreptool
    if (iInitState == 0 || iRestore)
        {
        RDEBUG("	-> Opening connections ... ");
        User::LeaveIfError(iCentRepServer.Connect());
        User::LeaveIfError(iDMUtil.Connect());
        RDEBUG("	-> Opening connections ... DONE!");
        }

    //init each session in own cycle....
    switch (iInitState)
        {
        case 0:
            {
            //Protect the destinations
            SetProtectionL(EProtLevel1);
            

            //compelete request
            TRequestStatus * status = &aRequestStatus;
            User::RequestComplete(status, KErrNone);
            }
            break;
        case 1:
            {
            RDEBUG("CCommsDatEnforcement:InitEnforcementL Case 1");

            TRequestStatus * status = &aRequestStatus;
            User::RequestComplete(status, KErrNone);
            }
            break;
        default:
            {
            RDEBUG("**** CCommsDatEnforcement PANIC, invalid switch-case!");
            User::Panic(KCommsDatEnformentPanic, KErrAbort);
            }
            break;
        }

    iInitState++;
    }


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitReady()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::InitReady()
	{
	RDEBUG("CCommsDatEnforcement::InitReady()");
	return ( iInitState > 1 );
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitReady()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::DoEnforcementL(TRequestStatus& aRequestStatus)
    {
    RDEBUG("CCommsDatEnforcement::DoEnforcementL()");
    if (!iRestore)
        {
        CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL(
                iSettingType);

        //ACL...
        RDEBUG("	making ACL modifications for enforcement ... ");
        User::LeaveIfError(iDMUtil.SetMngSessionCertificate(
                SessionCertificate()));
        User::LeaveIfError(iDMUtil.AddACLForNode(KAPURI, EForChildrens,
                EACLDelete));
        User::LeaveIfError(iDMUtil.AddACLForNode(KAPURI, EForNode, EACLGet));
        User::LeaveIfError(iDMUtil.SetACLForNode(KAPURI, EForNode, EACLAdd));
        RDEBUG("	making ACL modifications for enforcement ... DONE!");
        }
    else
        {
        //Unprotect the destinations
        SetProtectionL(EProtLevel0);
        

        CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL(
                iSettingType);

		//ACL...
		RDEBUG("	removing ACL modifications for enforcement ... ");
		User::LeaveIfError( iDMUtil.RemoveACL( KAPURI, ETrue ) );
		RDEBUG("	removing ACL modifications for enforcement ... DONE!");

		}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone );
	}

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::EnforcementReady()
	{
	RDEBUG("CCommsDatEnforcement::EnforcementReady()");
	return iEnforcementState > 0;
	}
	
// -----------------------------------------------------------------------------
// CCommsDatEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::FinishEnforcementL(TBool aFlushSettings)
    {
    RDEBUG("CCommsDatEnforcement::FinishEnforcementL()");
    //Close sessions
    if (aFlushSettings)
        {
        iDMUtil.Flush();
        }

    //Close centrep server...
    iCentRepServer.Close();

    iDMUtil.Close();
    }

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::AccessRightList(
        RAccessControlList& aAccessControlList)
    {
    RDEBUG("CCommsDatEnforcement::AccessRightList()");
    iAccessControlList = &aAccessControlList;
    }


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ResetEnforcementL()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::ResetEnforcementL()
    {
    RDEBUG("CCommsDatEnforcement::ResetEnforcementL()");

    User::LeaveIfError(iCentRepServer.Connect());
    User::LeaveIfError(iDMUtil.Connect());

    //ACL...
    User::LeaveIfError(iDMUtil.RemoveACL(KAPURI, ETrue));

    iCentRepServer.Close();
    iDMUtil.Close();
    }



// -----------------------------------------------------------------------------
// CCommsDatEnforcement::SetProtectionL()
// Sets Protection level to all the destinations
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::SetProtectionL(TProtectionLevel aProtLevel)
    {
    RDEBUG("CCommsDatEnforcement::SetProtectionL() Start");

    RCmManagerExt cmm;
    cmm.OpenL();
    CleanupClosePushL(cmm);

    RArray<TUint32> destinations;
    cmm.AllDestinationsL(destinations);
    CleanupClosePushL(destinations);
    TInt destcount = destinations.Count();

    RCmDestinationExt destination;

    //Set the Protection level for all the destinations
    for (TInt i = 0; i < destcount; i++)
        {
        TInt err = KErrNone;
        TInt retries = 1;
        // Retry, at the max, 10 times, if transaction fails due to KErrLocked
        // This is not an optimal solution for this problem, but, due to limitations
        // from CMManager, after a lot of Development and testing effort, below code
        // is found out to be a working one for most of the cases
        do
            {
            if (retries > 1)
                {
                User::After( KDelayBeforeProtection);
                }

            RDEBUG_2(
                    "CCommsDatEnforcement::SetProtectionL() DestinationL Retry No.  %d",
                    retries);
            TRAP(err, destination = cmm.DestinationL(destinations[i]));
            retries++;
            }
        while (err == KErrLocked && retries <= KMaxProtectionRetries);
        RDEBUG_2(
                "CCommsDatEnforcement::SetProtectionL() DestinationL error is  %d",
                err);

        if ((err != KErrNone && err != KErrLocked) || (err == KErrLocked
                && retries > KMaxProtectionRetries))
            {
            User::Leave(err);
            }

        CleanupClosePushL(destination);

        RDEBUG("CCommsDatEnforcement::SetProtectionL() SetProtectionL Start");
        destination.SetProtectionL(aProtLevel);
        RDEBUG("CCommsDatEnforcement::SetProtectionL() SetProtectionL End");
        //Connection Methods are not getting unlocked, eventhough the destinations are unlocked
        // Unlocking the Connecting Methods in the destination
        if (aProtLevel == EProtLevel0)
            {
            SetCMProtectionL(destination, EProtLevel0);
            }
        RDEBUG("CCommsDatEnforcement::SetProtectionL() UpdateL  Start");

        err = KErrNone;
        retries = 1;
        do
            {
            if (retries > 1)
                {
                User::After( KDelayBeforeProtection);
                }
            RDEBUG_2(
                    "CCommsDatEnforcement::SetProtectionL() UpdateL Retry No.  %d",
                    retries);
            TRAP(err, destination.UpdateL());
            retries++;
            }
        while (err == KErrLocked && retries <= KMaxProtectionRetries);
        RDEBUG_2(
                "CCommsDatEnforcement::SetProtectionL() UpdateL error is  %d",
                err);

        if ((err != KErrNone && err != KErrLocked) || (err == KErrLocked
                && retries > KMaxProtectionRetries))
            {
            User::Leave(err);
            }
        RDEBUG("CCommsDatEnforcement::SetProtectionL() UpdateL  End");
        CleanupStack::PopAndDestroy();
        }

    CleanupStack::PopAndDestroy(2);
    RDEBUG("CCommsDatEnforcement::SetProtectionL() End");

    }
    


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::SetCMProtectionL()
// Sets Protection level to all the Connection Methods in the  destination
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::SetCMProtectionL(RCmDestinationExt& aDestination,
        TProtectionLevel aProtLevel)
    {
    RDEBUG("CCommsDatEnforcement::SetCMProtectionL() Start");
    TInt APSNAPCount = aDestination.ConnectionMethodCount();

    RCmConnectionMethodExt connection;

    if (APSNAPCount)
        {
        for (TInt j = 0; j < APSNAPCount; j++)
            {
            connection = aDestination.ConnectionMethodL(j);
            CleanupClosePushL(connection);
            if (aProtLevel == EProtLevel0)
                {
                connection.SetBoolAttributeL(ECmProtected, EFalse);
                }
            else if (aProtLevel == EProtLevel1 || aProtLevel == EProtLevel3)
                {
                connection.SetBoolAttributeL(ECmProtected, ETrue);
                }
            CleanupStack::PopAndDestroy();
            }

        }
    RDEBUG("CCommsDatEnforcement::SetCMProtectionL() End");
    }
    
    
