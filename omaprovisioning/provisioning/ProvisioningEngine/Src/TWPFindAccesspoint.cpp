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
* Description:  TWPFindNapdef crawls a logical proxy
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include "TWPFindAccesspoint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TWPFindAccesspoint::TWPFindAccesspoint
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TWPFindAccesspoint::TWPFindAccesspoint()
    {
    iNapDef = NULL;
    }

// -----------------------------------------------------------------------------
// TWPFindAccesspoint::VisitL
// -----------------------------------------------------------------------------
//
void TWPFindAccesspoint::VisitL( CWPParameter& /*aElement*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// TWPFindAccesspoint::VisitL
// -----------------------------------------------------------------------------
//
void TWPFindAccesspoint::VisitL( CWPCharacteristic& aElement )
    { 
    if( !iNapDef && aElement.Type() == KWPPxPhysical ) 
        { 
        iNapDef = &aElement;
        } 
    }

// -----------------------------------------------------------------------------
// TWPFindAccesspoint::VisitLinkL
// -----------------------------------------------------------------------------
//
void TWPFindAccesspoint::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    { 
    };



//  End of File  
