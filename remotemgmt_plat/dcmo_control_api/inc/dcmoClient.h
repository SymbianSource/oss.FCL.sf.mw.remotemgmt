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
#include "dcmoclientbase.h"

class RDCMOClient :  public RDCMOClientBase
	{
	public:
	
		/**
		 * Createss DCMOClient
		 * @param None
		 * @return KErrNone Symbian error code
		 */
		 
		IMPORT_C static RDCMOClient* NewL();	
	
		/**
		 * Launches DCMOServer
		 * @param None
		 * @return KErrNone Symbian error code
		 */
		TInt OpenL();
		
		/**
		 * Close server connection
		 * @param None
		 * @return None
		 */
		void Close();
		
		/**
     * ~RDCMOClient.
     * Virtual Destructor.
     */
    virtual ~RDCMOClient();
    
		/**
		 * Get the interger type value
		 * DCMOServer or test app should use this
		 * @param aCategory
		 * @param aId
		 * @param aValue
		 * @return TDCMOStatus 
		 */
		 TDCMOStatus GetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt& aValue);
		 
		 /**
		 * Get the string type value
		 * DCMOServer or test app should use this
		 * @param aCategory
		 * @param aId
		 * @param aStrValue
		 * @return TDCMOStatus
		 */
		 TDCMOStatus GetDCMOStrAttributeValue (TDes& aCategory, TDCMONode aId, TDes& aStrValue);
		 
		 /**
		 * Set the interger type value
		 * DCMOServer or test app should use this
		 * @param aCategory
		 * @param aId
		 * @param aValue
		 * @return TDCMOStatus
		 */
		 TDCMOStatus SetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt aValue);
		 		 
		 /**
		 * Set the string type value
		 * DCMOServer or test app should use this
		 * @param aCategory
		 * @param aId
		 * @param aStrValue
		 * @return TDCMOStatus
		 */
		 TDCMOStatus SetDCMOStrAttributeValue(TDes& aCategory, TDCMONode aId, TDes& aStrValue);
		 
		 /**
		 * Get All type value
		 * DCMOServer or test app should use this
		 * @param aCategory
		 * @param aId
		 * @param aStrValue
		 * @return TDCMOStatus
		 */
		 void SearchByGroupValue(TDes& aGroup, TDes& aAdapterList);
			
		
	private:
		
};
