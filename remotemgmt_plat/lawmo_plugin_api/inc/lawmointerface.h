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
* Description:  LAWMO Plugin Interface definition
*
*/

#ifndef _MLAWMOPLUGININTERFACE_H__
#define _MLAWMOPLUGININTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include <lawmocallbackinterface.h>

// UID of this interface
const TUid KLAWMOPluginInterfaceUid = {0x2002EA1F}; 

/**
	An LAWMO abstract class being representative of the
	concrete class which the client wishes to use.

	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */


class MLAWMOPluginInterface
	{
public:
	
	virtual void WipeL() = 0;  
	};
 
class CLAWMOPluginInterface : public CBase, public MLAWMOPluginInterface
	{
public:
	// Instantiates an object of this type	
	/**
   * Create instance of CLAWMOPluginInterface
   * @param aUid , implementation Uid
   * @return Instance of CLAWMOPluginInterface
   */
	static CLAWMOPluginInterface* NewL(TUid aUid, MLawmoPluginWipeObserver* aObserver);
	/**
   * C++ Destructor
   */
	virtual ~CLAWMOPluginInterface();
	
private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;

	};

#include "lawmointerface.inl"

#endif  // _MLAWMOPLUGININTERFACE_H__

