/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provisioning context list
*
*/


//  INCLUDE FILES
#include "CWPCxAppUi.h"

#include <avkon.hrh>
#include <hlplch.h>
#include <ProvisioningCx.rsg>
#include "CWPCxView.h"
#include "CWPCxContainer.h" 
#include "provisioningCx.hrh"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CWPCxAppUi::ConstructL()
// ----------------------------------------------------------
//
void CWPCxAppUi::ConstructL()
    {
    #ifdef __SERIES60_32__
    	BaseConstructL( EAknEnableSkin | EAknEnableMSK);
    #else
    	BaseConstructL( EAknEnableSkin);
    #endif

    CWPCxView* view1 = new (ELeave) CWPCxView;

    CleanupStack::PushL( view1 );
    view1->ConstructL();
    AddViewL( view1 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view1
    }

// Destructor
CWPCxAppUi::~CWPCxAppUi()
    {
    }

// ----------------------------------------------------
// CWPCxAppUi::HandleCommandL
// ----------------------------------------------------
//
void CWPCxAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppHelpContextL() );
            break;
            }
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        default:
            break;      
        }
    }

// End of File  
