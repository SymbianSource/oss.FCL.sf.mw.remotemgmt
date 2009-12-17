/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Multiple bearers
*
*/


#include "nsmlroam.h"

// --------------------------------
// CNSmlRoamItem::CNSmlRoamItem()
// Constructor
// --------------------------------
//
CNSmlRoamItem::CNSmlRoamItem()
	{
	}

// --------------------------------
// CNSmlRoamItem::CNSmlRoamItem(TInt aProfileId, TUint32 aIAPId, TInt aPriority)
// Constructor
// --------------------------------
//
CNSmlRoamItem::CNSmlRoamItem(TInt aProfileId, TUint32 aIAPId, TInt aPriority)
: iProfileId( aProfileId), iIAPId( aIAPId), iPriority( aPriority)
	{
	}

// --------------------------------
// CNSmlRoamItem::~CNSmlRoamItem()
// Destructor
// --------------------------------
//
CNSmlRoamItem::~CNSmlRoamItem()
	{
	}

// --------------------------------
// CNSmlRoamItem::ProfileId()
// Returns profile id
// --------------------------------
//
TInt CNSmlRoamItem::ProfileId() { return iProfileId; }

// --------------------------------
// CNSmlRoamItem::IapId()
// Returns IAP id
// --------------------------------
//
TUint32 CNSmlRoamItem::IapId() { return iIAPId; }

// --------------------------------
// CNSmlRoamItem::Priority()
// Returns priority
// --------------------------------
//
TInt CNSmlRoamItem::Priority() { return iPriority; }


