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
* Description:  DCMO Generic Controller
*
*/

#ifndef __DCMO_GENERICCONTROL_H__
#define __DCMO_GENERICCONTROL_H__

// INCLUDES

#include <e32base.h>
#include "dcmoconst.h"

// CONSTANTS

_LIT(KDCMOHardware, 		"Hardware");
_LIT(KDCMOIO, 					"IO");
_LIT(KDCMOConnectivity, "Connectivity");
_LIT(KDCMOSoftware, 		"Software");
_LIT(KDCMOService, 			"Service");

const TInt KDCMOPropertyNumber = 1;
const TInt KDCMOMainKeyNumber = 3;
const TInt KDCMOActualKeyNumber = 4;
const TInt KDCMODescriptionNumber = 5;
const TInt KDCMOGroupNumber = 6;

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  CDCMOGenericControl
*  Description.
*/

class CDCMOGenericControl: public CBase
	{
public:
	/**
	 * Createss CDCMOGenericControl
	 * Default Constructor
	 */
	CDCMOGenericControl();
	/**
	 * Destructor
	 */
	~CDCMOGenericControl();			
	
  /**
	 * Get the interger type value
	 * @param aCategoryNumber
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus 
	 */
	TDCMOStatus GetIntAttributeL(TInt aCategoryNumber, TDCMONode aId, TInt& aValue);
	
	 /**
	 * Get the string type value
	 * @param aCategoryNumber
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus GetStrAttributeL(TInt aCategoryNumber, TDCMONode aId, TDes& aStrValue);
	
	/**
	 * Set the interger type value
	 * @param aCategoryNumber
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus SetIntAttributeL(TInt aCategoryNumber, TDCMONode aId, TInt aValue);
	
	/**
	 * Set the string type value
	 * @param aCategoryNumber
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus SetStrAttributeL(TInt aCategoryNumber, TDCMONode aId, const TDes& aStrValue);

private:
	
	/**
	 * Get the Group value
	 * @param aStrValue
	 * @return TInt
	 */
	TInt GetGroupValueL(TDes& aStrValue);
	
	};
	
#endif //__DCMO_GENERICCONTROL_H__
