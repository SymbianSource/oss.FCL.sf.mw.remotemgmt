/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef __TERMINALCONTROL3RDPARTYAPI_H__
#define __TERMINALCONTROL3RDPARTYAPI_H__

#include <e32std.h>
#include <TerminalControl.h>

class RTerminalControl3rdPartySession : public RSubSessionBase
	{
	public:

	    enum TTerminalControl3rdPartyMessages
	 	    {
	 	    // Autolock timeout. Values in minutes.  When != 0 the lock is enabled, When = 0 the lock is of
	 	    ETimeout = 1000,        
	 	    
	 	    // Maximum autolock timeout value. 
	 	    // 0 = Not in use
	 	    EMaxTimeout,            
	 	    // Passcode. Return value not supported
	 	    EPasscode,              
	 	    // 0 = passcode free. When != 0 the passcode can not be changed
	 	    EPasscodePolicy,        
	 	    // Lock the terminal immediately. Return value not supported
	 	    ELock,                  
	 	    // When != 0 the Device is wiped
	 	    EDeviceWipe,            

// enums below valid when the __SAP_DEVICE_LOCK_ENHANCEMENTS flag is on, otherwise KErrNotSupported is returned
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

		    // 4-256 characters
		    EPasscodeMinLength,		 
		    // 4-256 characters
		    EPasscodeMaxLength,		 
		    // 0 = No restriction, 1 = Both upper and lower case letters 
		    // are required in the password
		    EPasscodeRequireUpperAndLower,	 
		    // 0 = No restriction, 1 = Both characters and numbers are 
		    // required in the password
		    EPasscodeRequireCharsAndNumbers, 
		    // 0 = No restriction, 1-4 = A single character cannot be used 
		    // more than X times in the password	
		    EPasscodeMaxRepeatedCharacters,  

		    // 0 = No restriction, 1-X = The new password cannot match 
		    // the previous X passwords
		    EPasscodeHistoryBuffer,		 

		    // 0 = No restriction, 1-365 = The password expires after X days, 
		    // and must be changed by the user
		    // -1 = password expires immediately
		    EPasscodeExpiration,            

		    // 0-HistoryBuffer = The user can change the password only X times before 
		    // EPasscodeMinChangeInterval hours have passed (default 0).
		    EPasscodeMinChangeTolerance,     
		    // 0 = No restriction, 1-1000 = The user cannot change the password
		    // more than EPasscodeMinChangeTolerance times before X hours have 
		    // passed since the previous change.
		    EPasscodeMinChangeInterval,      

		    // 0 = No restriction, 1 = The password cannot match a string in the set of strings 
		    // given by the next three calls.
		    EPasscodeCheckSpecificStrings,   
		    // Disallow the specific string/strings given.
		    // Individual strings can be separated with ";".
		    EPasscodeDisallowSpecific,	 
		    // Re-allow the specific string/strings given. 
		    // Individual strings can be separated with ";".
		    EPasscodeAllowSpecific,          
		    // Clear the specific string -buffer
		    EPasscodeClearSpecificStrings,	 

		    // 0 = No restriction, 3-100 = The device is "hard" reset after the user has 
		    // consecutively failed X times to answer the password query
		    EPasscodeMaxAttempts,		 
		    
		    // 0 = No restriction, 1 = The password cannot contain two consecutive numbers.	    
	    	EPasscodeConsecutiveNumbers,
	    	
		    // 0 = No restriction, 1-255 = The password should contain at least 
		    // X number of special characters.	
		    EPasscodeMinSpecialCharacters,  
	    	
	    	EPasscodeDisallowSimple,

//#endif

	 	    ELast = 2000
	 	    };

	public:

	    /**
		* Connect Open client side session
		* @param aServer Reference to connected RTerminalControl
	   	* @return KErrNone Symbian error code
       	   */
		IMPORT_C TInt Open(RTerminalControl &aServer);

		/**
		* Close Close session
   		 */
		IMPORT_C void Close();

		/**
		SetDeviceLockParameter
		@param aType see TTerminalControl3rdPartyMessages (documentation) for supported values
		@param aData possible data for the message
		@return KErrNone Symbian error code
		*/

		IMPORT_C TInt SetDeviceLockParameter(TInt aType, const TDesC8& aData);

		/**
		GetDeviceLockParameterSize
		@param aType see TTerminalControl3rdPartyMessages (documentation) for supported values
		@return Size of parameter bytes or KErrNone Symbian error code in case of type not exist or in case of error
		*/

		IMPORT_C TInt GetDeviceLockParameterSize(TInt aType);

		/**
		GetDeviceLockParameter
		@param aType see TTerminalControl3rdPartyMessages (documentation) for supported values
		@param aData returns current parameter data 
		@return KErrNone Symbian error code
		*/

		IMPORT_C TInt GetDeviceLockParameter(TInt aType, TDes8& aData);

	};

#endif //__TERMINALCONTROL3RDPARTYAPI_H__