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
* Description:  DCMO Message Box
*
*/

#ifndef __DCMO_MESSAGEBOX_H__
#define __DCMO_MESSAGEBOX_H__

// INCLUDES

#include <hbdevicemessageboxsymbian.h>

/**
* Message Box class to show the global note.
*
*/
class CDCMOMessageBox : public CBase, public MHbDeviceMessageBoxObserver
	{
public:
    static CDCMOMessageBox* NewL( );
    
  /**
	 * Destructor
	 */
		~CDCMOMessageBox();
		
	/**
	 * Show the Notification
	 * @param aString , to be shown
	 * @return 
	 */
		void ShowMessageL( TDesC& aString );
		
public: // from MHbDeviceMessageBoxObserver
    void MessageBoxClosed(const CHbDeviceMessageBoxSymbian *aMessageBox,
        CHbDeviceMessageBoxSymbian::TButtonId aButton);
		
		/**
	 * Gets the Message Box status
	 * @param None
	 * @return ETrue if the user pressed ok button, else EFalse.
	 */
		static TBool IsMsgBoxClosed();
		
		/**
	 * Sets the Message Box status
	 * @param ETrue if the user pressed ok button, else EFalse.
	 * @return None
	 */
		static void SetMsgBoxStatus(TBool aStatus);
		
private:  
	/**
	 * Createss CDCMOMessageBox
	 * Default Constructor
	 */  
		CDCMOMessageBox( );		

private:		
		CHbDeviceMessageBoxSymbian* iMessageBox;
		static TBool iMsgBoxClosed;
	};
#endif //__DCMO_MESSAGEBOX_H__
