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
* Description:  Function Definition of CleanupResetAndDestroy
*
*/


// CLASS DECLARATION

// SYSTEM INCLUDE FILES
#include <e32base.h>                // CleanupStack

// USER INCLUDE FILES

namespace DiagFwInternal
    {

    template< class T >
    inline void CleanupResetAndDestroy( TAny* aObj )
        {
        if ( aObj )
            {
            static_cast< T* >( aObj )->ResetAndDestroy();
            }
        }

    template< class T >
    inline void CleanupRPointerArrayPushL( RPointerArray< T >* aObj )
        {
        if ( aObj )
            {
            CleanupStack::PushL( TCleanupItem(
                CleanupResetAndDestroy< RPointerArray< T > >,
                aObj ) );
            }
        }
    }

// End of File

