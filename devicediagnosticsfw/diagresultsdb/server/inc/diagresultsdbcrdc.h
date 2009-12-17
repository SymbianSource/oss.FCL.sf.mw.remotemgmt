/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Real cleanup stack for RPointerArrays.
*                ResetAndDestroy + Close must be called, instead of just Close.
*  libraries   : 
*
*/


/**
* Template class for destroying RPointerArrays
*
* @since S60 v5.0
**/
template <class T>
class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL(T& aRef)
        { 
        CleanupStack::PushL(TCleanupItem(&Close,&aRef)); 
        }

    static void Close(TAny *aPtr) 
        {
        (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
        (STATIC_CAST(T*,aPtr))->Close();
        }
    };
    
/**
* Use for pushing RPointerArrays onto a cleanupstack.
*
* @since S60 v5.0
**/    
template <class T>
inline void CleanupResetAndDestroyClosePushL(T& aRef)
    {
    CleanupResetAndDestroyClose<T>::PushL(aRef);
    }    
