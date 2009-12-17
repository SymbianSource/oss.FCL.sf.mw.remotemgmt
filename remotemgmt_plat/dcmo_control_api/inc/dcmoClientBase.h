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
* Description:  header file for DCMO Client
*
*/

#include <e32base.h>
#include <f32file.h>
#include "dcmoconst.h"

// needed for creating server thread.

class RDCMOClientBase : public RSessionBase
{
public:

	/**
	 * Opens RDCMOClientBase
	 * @param None
	 * @return KErrNone Symbian error code
	 */
	virtual TInt OpenL() = 0;
	/**
	 * Closes server connection
	 * @param None
	 * @return None
	 */
	virtual void Close()  = 0;
	/**
	 * Get the interger type value
	 * DCMOServer or test app should use this
	 * @param aCategory
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus 
	 */
	virtual TDCMOStatus GetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt& aValue) = 0;
	
	/**
	 * Get the string type value
	 * DCMOServer or test app should use this
	 * @param aCategory
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */
	virtual TDCMOStatus GetDCMOStrAttributeValue(TDes& aCategory, TDCMONode aId, TDes& aStrValue) = 0;
	
	/**
	 * Set the interger type value
	 * DCMOServer or test app should use this
	 * @param aCategory
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus
	 */
	virtual TDCMOStatus SetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt aValue) = 0;
	
	/**
	 * Set the string type value
	 * DCMOServer or test app should use this
	 * @param aCategory
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */	 
	virtual TDCMOStatus SetDCMOStrAttributeValue(TDes& aCategory, TDCMONode aId, TDes& aStrValue) = 0;	

	 /**
		* Get All type value
		* DCMOServer or test app should use this
		* @param aCategory
		* @param aId
		* @param aStrValue
		* @return TDCMOStatus
		*/
	virtual void SearchByGroupValue(TDes& aGroup, TDes& aAdapterList) = 0;

};

