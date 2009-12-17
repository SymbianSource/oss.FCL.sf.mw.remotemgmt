/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Alert queue and handler
*
*/


// INCLUDE FILES
#include "NSmlAlertQueue.h"

// CONSTANTS
const TInt CNSmlMessageItem::iOffset = _FOFF(CNSmlMessageItem,iDlink);


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNSmlMessageItem::NewL( const TDesC8& aMessage, TSmlUsageType aType, 
// TSmlProtocolVersion aVersion, TSmlTransportId aBearerType )
// Two phase constructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlMessageItem* CNSmlMessageItem::NewL( 
    const TDesC8& aMessage, 
    TSmlUsageType aType, 
    TSmlProtocolVersion aVersion, 
    TSmlTransportId aBearerType )
	{
	CNSmlMessageItem* self = new (ELeave) CNSmlMessageItem();
	CleanupStack::PushL(self);
	self->ConstructL(aMessage, aType, aVersion, aBearerType );
	CleanupStack::Pop(); //self
	return self;
	}

// ---------------------------------------------------------
// CNSmlMessageItem::CNSmlMessageItem()
// Constructor
// ---------------------------------------------------------
CNSmlMessageItem::CNSmlMessageItem()
: iMessage(NULL)
	{
	
	}

// ---------------------------------------------------------
// CNSmlMessageItem::~CNSmlMessageItem()
// Destructor
// ---------------------------------------------------------
CNSmlMessageItem::~CNSmlMessageItem()
	{
	delete iMessage;
	iMessage = NULL;
	}

// ---------------------------------------------------------
// CNSmlMessageItem::ConstructL( const TDesC8& aMessage, 
// TSmlUsageType aType, TSmlProtocolVersion aVersion, TSmlTransportId aBearerType )
// Second constructor
// ---------------------------------------------------------
void CNSmlMessageItem::ConstructL( 
    const TDesC8& aMessage, 
    TSmlUsageType aType, 
    TSmlProtocolVersion aVersion, 
    TSmlTransportId aBearerType )
	{
	iUsageType = aType;
	iProtocol = aVersion; 
	iMessage = aMessage.AllocL();
	iBearerType = aBearerType;	
	}

// ---------------------------------------------------------
// CNSmlMessageItem::Message()
// Returns reference to message buffer
// ---------------------------------------------------------
EXPORT_C TDesC8& CNSmlMessageItem::Message()
	{
	return *iMessage;
	}
