/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class declaration for DiagFwInternal::TEvent
*
*/


#ifndef DIAGENGINEEVENTBASIC_H
#define DIAGENGINEEVENTBASIC_H

// SYSTEM INCLUDES
#include <e32base.h>            // CBase

// USER INCLUDES
#include "diagengineevents.h"

namespace DiagFwInternal
    {
/**
* CEventBasic
*
* Basic event with just event ID.
*/
NONSHARABLE_CLASS( CEventBasic ) : public CBase
    {
public:
    /**
    * C++ Constructor
    * 
    * @param aType - Type of event.
    * @see DiagFwInternal::TEvent
    */
    CEventBasic( TEvent aType );

    /**
    * Get Type
    * 
    * @see DiagFwInternal::TEvent
    * @return Type of event.
    */
    TEvent GetType() const;

    /**
    * Get name of the event
    * 
    * @return Name of the event
    */
    const TDesC& ToString() const;

private:    // private data
    /**
    * iType - event type.
    * @see DiagFwInternal::TEvent
    */
    TEvent iType;
    };    
    } // namespace DiagFwInternal

#endif // ifndef DIAGENGINEEVENTBASIC_H

// End of File

