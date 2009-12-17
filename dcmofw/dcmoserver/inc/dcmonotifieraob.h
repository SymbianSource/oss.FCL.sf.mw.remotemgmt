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
* Description:  DCMO Notifier Aob
*
*/

#ifndef __DCMO_NOTIFIERAOB_H__
#define __DCMO_NOTIFIERAOB_H__

// INCLUDES

#include <AknGlobalMsgQuery.h>

/**
* An active class to show the global note.
*
*/
class CDCMONotifierAob : public CActive
	{
public:
    static CDCMONotifierAob* NewL( );
    
  /**
	 * Destructor
	 */
		~CDCMONotifierAob();
		
	/**
	 * Show the Notification
	 * @param aString , to be shown
	 * @return 
	 */
		void ShowNotifierL( TDesC& aString );
		
		/**
     * From CActive,DoCancel.
     */
		void DoCancel();

		/**
     * From CActive,RunL.
     */
	  void RunL();

		/**
     * From CActive,RunError.
     */
		TInt RunError(TInt aError);
		
private:  
	/**
	 * Createss CDCMOGenericControl
	 * Default Constructor
	 */  
		CDCMONotifierAob( );
		
	/**
	 * second phase constructor
	 */
    void ConstructL();

private:		
		CAknGlobalMsgQuery* iGlobalMsgQuery;

	};
#endif //__DCMO_NOTIFIERAOB_H__
