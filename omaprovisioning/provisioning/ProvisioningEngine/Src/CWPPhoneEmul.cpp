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
* Description:  Phone interface class
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <mmtsy_names.h>
#include "CWPPhoneEmul.h"
#include "WPEngineDebug.h"

// CONSTANTS
/// The emulated IMSI number
_LIT( KIMSI, "0" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPPhoneEmul::CWPPhoneEmul
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPPhoneEmul::CWPPhoneEmul()
: iIMSI( KIMSI ), iCountry( KNullDesC ), iNetwork( KNullDesC )
    {
    // Country and network retrieval is never done, so it's not emulated
    }

// -----------------------------------------------------------------------------
// CWPPhone::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPPhoneEmul::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CWPPhone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPPhoneEmul* CWPPhoneEmul::NewL()
    {
    DEBUG( "CWPPhoneEmul::NewL()" );
    CWPPhoneEmul* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPPhone::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPPhoneEmul* CWPPhoneEmul::NewLC()
    {
    DEBUG( "CWPPhoneEmul::NewLC()" );
    CWPPhoneEmul* self = new(ELeave) CWPPhoneEmul; 
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Destructor
CWPPhoneEmul::~CWPPhoneEmul()
    {
    }

// -----------------------------------------------------------------------------
// CWPPhoneEmul::SubscriberId
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneSubscriberId& CWPPhoneEmul::SubscriberId() const
    {
    return iIMSI;
    }

// -----------------------------------------------------------------------------
// CWPPhone::SubscriberId
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkCountryCode& CWPPhoneEmul::Country() const
    {
    return iCountry;
    }

// -----------------------------------------------------------------------------
// CWPPhone::SubscriberId
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkIdentity& CWPPhoneEmul::Network() const
    {
    return iNetwork;
    }

//  End of File  
