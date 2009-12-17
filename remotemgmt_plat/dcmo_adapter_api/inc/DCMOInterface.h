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
* Description:  DCMO Interface definition
*
*/

#ifndef _MDCMOINTERFACE_H__
#define _MDCMOINTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include "dcmoconst.h"

// UID of this interface
const TUid KDCMOInterfaceUid = {0x2001FD40};

/**
	An DCMO abstract class being representative of the
	concrete class which the client wishes to use.

	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */


class MDCMOInterface
	{
public:
	// The interface for passing initialisation parameters
	// to the derived class constructor.
	struct TDCMOInterfaceInitParams
		{
		TUid  uid;
		const TDesC* descriptor;
		};

	// Pure interface methods
	// Representative of a method provided on the interface by 
	// the interface definer.
	
	/**
   * Gets Integer type Attribute value from the plug-in Adapter.
   * @param aId TDCMONode type.
   * @param aValue the integer value.
   * @return A TDCMOStatus value from the plug-in adapter.
 	 */	
	virtual TDCMOStatus  GetDCMOPluginIntAttributeValueL( TDCMONode aId, TInt& aValue) = 0;	
	/**
   * Gets String type Attribute value from the plug-in Adapter.
   * @param aId TDCMONode type.
   * @param aStrValue the string.
   * @return A TDCMOStatus value from the plug-in adapter.
 	 */	
	virtual TDCMOStatus  GetDCMOPluginStrAttributeValueL( TDCMONode aId, TDes& aStrValue) = 0;
	/**
   * Sets Integer type Attribute value to the plug-in Adapter.
   * @param aId TDCMONode type.
   * @param aValue the integer value.
   * @return A TDCMOStatus value from the plug-in adapter.
 	 */	
	virtual TDCMOStatus  SetDCMOPluginIntAttributeValueL( TDCMONode aId, TInt aValue) = 0;
	/**
   * Sets String type Attribute value from the plug-in Adapter.
   * @param aId TDCMONode type.
   * @param aStrValue the string.
   * @return A TDCMOStatus value from the plug-in adapter.
 	 */	
	virtual TDCMOStatus  SetDCMOPluginStrAttributeValueL( TDCMONode aId, const TDes& aStrValue) = 0;
	/**
   * Gets the localized name from plug-in Adapter.
   * @param aLocName HBufC type.
	 * @prototype
 	 */	
	virtual void GetLocalizedNameL (HBufC*& aLocName) = 0;
	
	};
 
class CDCMOInterface : public CBase, public MDCMOInterface
	{
public:
	// Instantiates an object of this type	
	/**
   * Create instance of CDCMOInterface
   * @param aUid , implementation Uid
   * @return Instance of CDCMOInterface
   */
	static CDCMOInterface* NewL(TUid aUid);
	/**
   * Create instance of CDCMOInterface
   * @param aInitParms of TDCMOInterfaceInitParams type.
   * @return Instance of CDCMOInterface
   */
	static CDCMOInterface* NewL(TDCMOInterfaceInitParams aInitParms);
	/**
   * C++ Destructor
   */
	virtual ~CDCMOInterface();
	
private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;

	};

#include "dcmointerface.inl"

#endif  // _MDCMOINTERFACE_H__

