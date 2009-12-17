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
* Description:  Provides ResetAndDestroy for RPointerArrays.
*
*/


#ifndef DIAGCLEANUPRESETANDDESTROY_H
#define DIAGCLEANUPRESETANDDESTROY_H

// SYSTEM INCLUDES
#include <e32def.h>                 // TAny

// FORWARD DECLARATION
template< class T >
class RPointerArray;

namespace DiagFwInternal
    {

    /**
    * Cleanup function for RPointerArray or any other types with ResetAndDestroy()
    *   @usage
    *   RPointerArray< CDiagResultsDatabaseItem > myArray;
    *   CleanupStack::PushL( TCleanupItem( 
    *       DiagFwInternal::CleanupResetAndDestroy< RPointerArray< CDiagResultsDatabaseItem > >,
    *       &myArray ) );
    *    
    *   ...
    *
    *   CleanupStack::PopAndDestroy( &myArray );
    */
    template< class T >
    inline void CleanupResetAndDestroy( TAny* aObj );

    /**
    * Push a RPointerArray of templated type to cleanup stack
    *   @usage
    *   RPointerArray< CDiagResultsDatabaseItem > myArray;
    *   DiagFwInternal::CleanupRPointerArrayPushL< CDiagResultsDatabaseItem >( &myArray );
    *
    *   ...
    *   
    *   CleanupStack::PopAndDestroy( &myArray );
    */
    template< class T >
    inline void CleanupRPointerArrayPushL( RPointerArray< T >* aObj );

    }

// Inline methods
#include "diagcleanupresetanddestroy.inl"

#endif // DIAGCLEANUPRESETANDDESTROY_H

// End of File

