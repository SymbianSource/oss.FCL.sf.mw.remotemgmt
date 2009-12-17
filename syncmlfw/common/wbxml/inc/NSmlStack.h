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
* Description:  Command stack for WBXML.
*
*/


#ifndef __NSMLSTACK_H__
#define __NSMLSTACK_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// CNSmlStack 
// ------------------------------------------------------------------------------------------------

template <class T>
class CNSmlStack  : public CBase
	{
public:
	static inline CNSmlStack* NewL();
	inline ~CNSmlStack();

	inline T* Pop();
	inline T* Top();
	inline void Push( T* aItem );
	inline TInt Count();
	inline void Reset();
	inline void ResetAndDestroy();

protected:
	inline CNSmlStack<T>();

private:
	RPointerArray<T>* iStack;
	};

#include "nsmlstack.inl"

#endif // __NSMLSTACK_H__