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
* Description: 
*     CWPAdapter implementation.
*
*/


// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include "CWPAdapter.h"
#include "ProvisioningUIDs.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAdapter::CWPAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPAdapter::CWPAdapter()
    {
    }

// Destructor
EXPORT_C CWPAdapter::~CWPAdapter()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// CWPAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPAdapter* CWPAdapter::NewL( const TUid& aImplementationUid )
    {
    // Get the instantiation
    CWPAdapter* pushHandler = 
        REINTERPRET_CAST(CWPAdapter*,
            REComSession::CreateImplementationL(aImplementationUid,
            _FOFF(CWPAdapter, iDtor_ID_Key)));

    return pushHandler;
    }

// -----------------------------------------------------------------------------
// CWPAdapter::ListL
// -----------------------------------------------------------------------------
//
void CWPAdapter::ListL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( TUid::Uid( KProvisioningAdapterInterface ),
        aImplInfoArray );
    }

//  End of File  
