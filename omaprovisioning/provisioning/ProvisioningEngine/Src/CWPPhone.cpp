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
#include "CWPPhone.h"
#include "WPEngineDebug.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPPhone::CWPPhone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPPhone::CWPPhone()
    {
    }

// -----------------------------------------------------------------------------
// CWPPhone::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPPhone::ConstructL()
    {
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );

    RMobilePhone phone;
    User::LeaveIfError( server.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( phone.Open( server, KMmTsyPhoneName ) );
    CleanupClosePushL( phone );
    User::LeaveIfError( phone.Initialise() );

    TRequestStatus status;
    phone.GetSubscriberId( status, iIMSI );
    User::WaitForRequest( status );

    if(status!=KErrNone)
    {
    
    	CleanupStack::PopAndDestroy( 2 ); // phone, server
    	User::LeaveIfError( status.Int() );	
    }    

    DVA( "CWPPhone::ConstructL(): IMSI: %S", &iIMSI );

    status = KErrNone;
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );
    phone.GetHomeNetwork( status, infoPckg );
    User::WaitForRequest( status );
    iCountry = info.iCountryCode;
    iNetwork = info.iNetworkId;
    DVA( "CWPPhone::ConstructL(): Country: %S", &iCountry );
    DVA( "CWPPhone::ConstructL(): Network: %S", &iNetwork );

    CleanupStack::PopAndDestroy( 2 ); // phone, server
    }

// -----------------------------------------------------------------------------
// CWPPhone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPPhone* CWPPhone::NewL()
    {
    DEBUG( "CWPPhone::NewL()" );
    CWPPhone* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CWPPhone::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPPhone* CWPPhone::NewLC()
    {
    DEBUG( "CWPPhone::NewLC()" );
    CWPPhone* self = new(ELeave) CWPPhone; 
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Destructor
CWPPhone::~CWPPhone()
    {
    }

// -----------------------------------------------------------------------------
// CWPPhone::SubscriberId
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneSubscriberId& CWPPhone::SubscriberId() const
    {
    return iIMSI;
    }

// -----------------------------------------------------------------------------
// CWPPhone::Country
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkCountryCode& CWPPhone::Country() const
    {
    return iCountry;
    }

// -----------------------------------------------------------------------------
// CWPPhone::Network
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkIdentity& CWPPhone::Network() const
    {
    return iNetwork;
    }

//  End of File  
