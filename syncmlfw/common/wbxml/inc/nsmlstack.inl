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
* Description:  Inline methods for WBXML CNSmlStack.
*
*/


#ifndef __NSMLSTACK_INL__
#define __NSMLSTACK_INL__


// ------------------------------------------------------------------------------------------------
// CNSmlStack
// ------------------------------------------------------------------------------------------------
template<class T>
inline CNSmlStack<T>::CNSmlStack()
	{
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline CNSmlStack<T>::~CNSmlStack()
	{
	iStack->ResetAndDestroy();
	delete iStack;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline CNSmlStack<T>* CNSmlStack<T>::NewL()
	{
	CNSmlStack<T>* self = new (ELeave) CNSmlStack<T>();
	CleanupStack::PushL(self);
	self->iStack = new (ELeave) RPointerArray<T>();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline T* CNSmlStack<T>::Pop()
	{
	T* temp = iStack->operator[](iStack->Count() - 1);
	iStack->Remove(iStack->Count() - 1);
	return temp;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline T* CNSmlStack<T>::Top()
	{
	if( Count() > 0 )
		{
		return iStack->operator[](iStack->Count() - 1);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CNSmlStack<T>::Push( T* aItem )
	{
	iStack->Append(aItem);
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline TInt CNSmlStack<T>::Count()
	{
	return iStack->Count();
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CNSmlStack<T>::Reset()
	{
	iStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CNSmlStack<T>::ResetAndDestroy()
	{
	iStack->ResetAndDestroy();
	}

#endif // __NSMLSTACK_INL__