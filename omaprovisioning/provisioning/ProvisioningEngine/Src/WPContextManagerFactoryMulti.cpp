/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Multiple context manager.
*
*/


//  INCLUDE FILES

#include <centralrepository.h>
#include "ProvisioningVariant.hrh"
#include "WPContextManagerFactory.h"
#include "CWPBindingContextManager.h"
#include "CWPMultiContextManager.h"
#include "ProvisioningInternalCRKeys.h"


// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPContextManagerFactory::CreateL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MWPContextManager* WPContextManagerFactory::CreateL()
	{

    TInt value( 0 );

    CRepository* repository = CRepository::NewLC( KCRUidOMAProvisioningLV );
    User::LeaveIfError( repository->Get( KOMAProvisioningLVFlag, value ) );
    CleanupStack::PopAndDestroy(); // repository

    MWPContextManager* manager = NULL;
    if( value & EWPMultipleCxBinding )
        {
        manager = CWPBindingContextManager::NewL();
        }
    else
        {
        manager = CWPMultiContextManager::NewL();
        }

    return manager;
	}

//  End of File  
