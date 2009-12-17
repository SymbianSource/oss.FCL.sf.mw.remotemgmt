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
* Description:  Factory for phone interface.
*
*/


//  INCLUDE FILES
#include <e32std.h>
#include "WPPhoneFactory.h"
#include "MWPPhone.h"
#include "CWPPhone.h"
#include "CWPPhoneEmul.h"
#include "WPEngineDebug.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPPhoneFactory::CreateL
// -----------------------------------------------------------------------------
//
EXPORT_C MWPPhone* WPPhoneFactory::CreateL()
    {
    DEBUG( "WPPhoneFactory: Creating phone" );
    MWPPhone* phone = CreateLC();
    CleanupStack::Pop();
    DEBUG( "WPPhoneFactory: Created" );
    return phone;
    }

// -----------------------------------------------------------------------------
// WPPhoneFactory::CreateLC
// -----------------------------------------------------------------------------
//
EXPORT_C MWPPhone* WPPhoneFactory::CreateLC()
    {
#ifdef __WINS__
    return CWPPhoneEmul::NewLC();
#else
    return CWPPhone::NewLC();
#endif
    }

//  End of File  
