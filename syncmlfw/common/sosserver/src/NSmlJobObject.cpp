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
* Description:  Symbian OS Server source.
*
*/



// INCLUDE FILES
#include "nsmlsosserver.h"


// --------------------------------------------------------------------------
// CNSmlJobObject::CNSmlJobObject() 
// --------------------------------------------------------------------------
//
CNSmlJobObject::CNSmlJobObject() 
    {
    }

// --------------------------------------------------------------------------
// CNSmlJobObject::~CNSmlJobObject()
// --------------------------------------------------------------------------
//
CNSmlJobObject::~CNSmlJobObject()
    {
    }

// --------------------------------------------------------------------------
// TInt CNSmlJobObject::JobId() const
// --------------------------------------------------------------------------
//
TInt CNSmlJobObject::JobId() const
    { 
    return iJobId;   
    }

// --------------------------------------------------------------------------
// void CNSmlJobObject::SetJobId( const TInt aJobId )
// --------------------------------------------------------------------------
//
void CNSmlJobObject::SetJobId( const TInt aJobId )
    {
    iJobId = aJobId;
    }
