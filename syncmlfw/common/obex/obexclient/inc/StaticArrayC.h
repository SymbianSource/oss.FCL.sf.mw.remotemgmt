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
* Description:  SyncML Obex client
*
*/


#ifndef __STATIC_ARRAY_C_H__
#define __STATIC_ARRAY_C_H__

/*! 
  @class TStaticArrayC
  
  @discussion This templated class provides a type, and size, safe method of
  using static arrays.
  */
template <class T>
class TStaticArrayC
    {
public:

/*!
  @function operator[]
  
  @discussion Return an element from the array.
  @param aIndex the index of the element to return
  @result a reference to the object
  */
    inline const T& operator[](TInt aIndex) const;

    /*!
      @enum TPanicCode
  
      @discussion Panic code
      @value EIndexOutOfBounds index is out of bounds
      */
    enum TPanicCode
        { 
        EIndexOutOfBounds = 1 
        };

/*!
  @function Panic
  
  @discussion Generate a panic.
  @param aPanicCode the reason code for the panic
  */
    inline void Panic(TPanicCode aPanicCode) const;

public:
    /*! @var iArray the arrat of elements */
    const T* iArray;

    /*! @var iCount the number of elements */
    TInt iCount;

    };

/*!
  @function CONSTRUCT_STATIC_ARRAY_C
  
  @discussion Initalise a global constant of type TStaticArrayC<>.
  @param aValue the underlying const array of T
  */
#define CONSTRUCT_STATIC_ARRAY_C(aValue) \
        {   \
        aValue,    \
        sizeof(aValue) / sizeof(*aValue)  \
        }  \


#include "StaticArrayC.inl"

#endif //   __STATIC_ARRAY_C_H__