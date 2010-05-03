/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  E/D Interface definition
*
*/

#ifndef _MDMEncryptionUtilInterface_H__
#define _MDMEncryptionUtilInterface_H__

#include <e32base.h>
#include <ecom/ecom.h>

// UID of this interface
const TUid KDMEncryptionUtilInterfaceUid = {0x2002C2F4};

/**
	An CDMEncryptionUtil abstract class being representative of the
	concrete class which the client wishes to use.

	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */


class MDMEncryptionUtilInterface
	{
public:

	// Pure interface methods
	// Representative of a method provided on the interface by 
	// the interface definer.
	
	/**
   * Encrypts the data.
   * @param aInput - is the input string which has to be encrypted.
   * @param aOutput - the encrypted information is stored parameter.
   * @return Return is KErrNone if successful or any System wide error code.
   * 
   */	
	virtual TInt  EncryptL( TDes& aInput, TDes& aOutput) = 0;	
	/**
   * Decrypts the data.
   * @param Input - is the input string which has to be decrypted.
   * @param aOutput - the decrypted information is stored in this parameter.
   * @return Return is KErrNone if successful or any System wide error code.
 	 */	
	virtual TInt DecryptL( TDes& aInput, TDes& aOutput) = 0;
	
	};

#endif  // _MDMEncryptionUtilInterface_H_

