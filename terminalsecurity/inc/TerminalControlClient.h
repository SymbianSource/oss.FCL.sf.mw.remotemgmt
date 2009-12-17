/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef __TERMINALCONTROLCLIENT_H__
#define __TERMINALCONTROLCLIENT_H__

// INCLUDES
#include <e32std.h>
#include <ssl.h>
#include "TerminalControl.h"
#include "TerminalControlClientServer.h"

// CONSTANTS
_LIT8( KTCPathCDrive, "c:\\" );

// MACROS

// DATA TYPES
typedef TBuf8<sizeof(TFileName)> TFileName8;

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  RTerminalControlSession class to handle terminal control sessions
*/
	
class RTerminalControlSession : public RSubSessionBase
{
	public: 
		//Session management		
	    /**
		* Connect Open client side session
		* @param aServer Reference to connected RTerminalControl
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Open(RTerminalControl &aServer, TCertInfo &aCertification );
		
		/**
		* Close Close session
        */
		IMPORT_C void Close();

		/**
		* Scan files
        */
		IMPORT_C TInt FileScan( const TDesC8& aStartPath = KTCPathCDrive(), TBool aRecursive=EFalse );
		
		/**
		* Get file scan results
        */
        IMPORT_C TInt FileScanResultsL( CBufFlat* aBuffer );
        
		/**
		* Get file scan results size
        */
		IMPORT_C TInt FileScanResultsSize( TInt &aSize );
		
		/**
		* Get file scan results size
        */
		IMPORT_C TInt FileScanResultsSize( TDes8 &aSize );
		
		/**		
		* Delete file
        */
		IMPORT_C TInt DeleteFile( const TDesC8& aFileName );
		
		/**
		* Set device lock autolock period
        */
		IMPORT_C TInt SetAutolockPeriod( TInt32 aPeriod );

		/**
		* Get device lock autolock period
        */
		IMPORT_C TInt GetAutolockPeriod( TInt32 &aPeriod );

		/**
		* Set maximum device lock autolock period
        */
		IMPORT_C TInt SetMaxAutolockPeriod( TInt32 aPeriod );

		/**
		* Get maximum device lock autolock period
        */
		IMPORT_C TInt GetMaxAutolockPeriod( TInt32 &aPeriod );
		
		/**
		* Set device lock level
        */
		IMPORT_C TInt SetDeviceLockLevel( TInt32 aLevel );

		/**
		* Get device lock level
        */
		IMPORT_C TInt GetDeviceLockLevel( TInt32 &aLevel );
		
		/**
		* Set lock code
        */
		IMPORT_C TInt SetDeviceLockCode( const TDesC8 &aCode );

		/**
		* Get lock code
        */
		IMPORT_C TInt GetDeviceLockCode( TDesC8 &aCode );

		/**
		* Wipe device (factory reset)
        */
		IMPORT_C TInt DeviceWipe( );
		
		
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
		
		/**
		* Set the minimum length for passcode.
		* 4-256 characters
		*/
		IMPORT_C TInt SetPasscodeMinLength( TInt32 aMinLength );
		
		/**
		* Get the minimum length for passcode.
		* 4-256 characters
		*/
		IMPORT_C TInt GetPasscodeMinLength( TInt32& aMinLength );
		
		/**
		* Set the maximum length for passcode 
		*/
		IMPORT_C TInt SetPasscodeMaxLength( TInt32 aMinLength );
		
		/**
		* Get the maximum length for passcode 
		*/
		IMPORT_C TInt GetPasscodeMaxLength( TInt32& aMinLength );
		
		
		/**
		* Set if upper and lower case letter are required in the passcode
		*/
		IMPORT_C TInt SetPasscodeRequireUpperAndLower( TBool aRequire );
		
		/**
		* Get if upper and lower case letter are required in the passcode
		*/
		IMPORT_C TInt GetPasscodeRequireUpperAndLower( TBool& aRequire );
		
		/**
		* Set if chars and numbers are required in the passcode
		*/
		IMPORT_C TInt SetPasscodeRequireCharsAndNumbers( TBool aRequire );
		
		/**
		* Get if chars and numbers are required in the passcode
		*/
		IMPORT_C TInt GetPasscodeRequireCharsAndNumbers( TBool& aRequire );
		
		/**
		* Set the amount of maximum repeated characters in the passcode.
		* 0 = No restriction
		* 1-4 = A single character cannot be used more than X times in the password
		*/
		IMPORT_C TInt SetPasscodeMaxRepeatedCharacters( TInt32 aMaxChars );
		
		/**
		* Get the amount of maximum repeated characters in the passcode
		* 0 = No restriction
		* 1-4 = A single character cannot be used more than X times in the password
		*/
		IMPORT_C TInt GetPasscodeMaxRepeatedCharacters( TInt32& aMaxChars );
		
		
		/**
		* Set the length of the passcode history buffer.
		* The new password cannot match the previous X passwords.
		*/
		IMPORT_C TInt SetPasscodeHistoryBuffer( TInt32 aBufferLength );
		
		/**
		* Get the length of the passcode history buffer
		* The new password cannot match the previous X passwords.
		*/
		IMPORT_C TInt GetPasscodeHistoryBuffer( TInt32& aBufferLength );
		
		
		/**
		* Set passcode expiration time. The password expires after X days.
		* 0 = No restriction.
		* 1-365 = The password expires after X days.
		* -1 = password expires immediately
		*/
		IMPORT_C TInt SetPasscodeExpiration( TInt32 aExpiration );
		
		/**
		* Get passcode expiration time. The password expires after X days.
		* 0 = No restriction.
		* 1-365 = The password expires after X days.
		*/
		IMPORT_C TInt GetPasscodeExpiration( TInt32& aExpiration );
		
		
		/**
		* Set the minumum change tolerance for passcode change.
		* The user can change the password only X times before 
		* PasscodeMinChangeInterval hours have passed.
		*/
		IMPORT_C TInt SetPasscodeMinChangeTolerance( TInt32 aMinTolerance );
		
		/**
		* Get the minumum change tolerance for passcode change.
		* The user can change the password only X times before 
		* PasscodeMinChangeInterval hours have passed.
		*/
		IMPORT_C TInt GetPasscodeMinChangeTolerance( TInt32& aMinTolerance );
		
		/**
		* Set passcode minumum change iterval.
		* 0 = No restriction.
		* 1-1000 = The user cannot change the password more than 
		* 		   PasscodeMinChangeTolerance times before X hours 
		*		   have passed since the previous change.
		*/
		IMPORT_C TInt SetPasscodeMinChangeInterval( TInt32 aMinInterval);
		
		/**
		* Get passcode minumum change iterval.
		* 0 = No restriction.
		* 1-1000 = The user cannot change the password more than 
		* 		   PasscodeMinChangeTolerance times before X hours 
		*		   have passed since the previous change.
		*/
		IMPORT_C TInt GetPasscodeMinChangeInterval( TInt32& aMinInterval );
		
		
		/**
		* Set if the passcode is checked not to contain specific strings.
		*/
		IMPORT_C TInt SetPasscodeCheckSpecificStrings( TBool aCheckSpecificStrings );
		
		/**
		* Check if the passcode is checked not to contain specific strings.
		*/
		IMPORT_C TInt GetPasscodeCheckSpecificStrings( TBool& aCheckSpecificStrings );
		
		
		/**
		* Disallow the specific string/strings given.
		* Individual strings can be separated with ";".
		*/
		IMPORT_C TInt DisallowSpecificPasscodeString( const TDesC8 &aString );
		
		
		/**
		* Re-allow the specific string/strings given.
		* Individual strings can be separated with ";".
		*/
		IMPORT_C TInt AllowSpecificPasscodeString( const TDesC8 &aString );
		

		/**
		* Clears the specific passcode strings.
		*/
		IMPORT_C TInt ClearSpecificPasscodeStrings();
		
		
		/**
		* Set the amount how many times the user may attempt to enter the passcode.
		* 0 = No restriction
		* 3-100 = The device is "hard" reset after the user has consecutively 
		* failed X times to answer the password query
		*/
		IMPORT_C TInt SetPasscodeMaxAttempts( TInt32 aMaxAttempts );
		
		/**
		* Set the amount how many times the user may attempt to enter the passcode.
		* 0 = No restriction
		* 3-100 = The device is "hard" reset after the user has consecutively 
		* failed X times to answer the password query
		*/
		IMPORT_C TInt GetPasscodeMaxAttempts( TInt32& aMaxAttempts );
		
		/**
		* 0 = No restriction, 1 = The password cannot contain two consecutive numbers
		*/
		IMPORT_C TInt SetPasscodeConsecutiveNumbers( TBool aConsecutiveNumbers );
		
		/**
		* 0 = No restriction, 1 = The password cannot contain two consecutive numbers
		*/
		IMPORT_C TInt GetPasscodeConsecutiveNumbers( TBool& aConsecutiveNumbers );

        /**
        * Set the amount of minimum special characters in the passcode.
        * 0 = No restriction
        * 1-255 = Password should contain at least X number of special characters.
        */
        IMPORT_C TInt SetPasscodeMinSpecialCharacters( TInt32 aMinChars );
        
        /**
        * Get the amount of minimum special characters in the passcode.
        * 0 = No restriction
        * 1-255 = Password should contain at least X number of special characters.
        */
        IMPORT_C TInt GetPasscodeMinSpecialCharacters( TInt32& aMinChars );
        /**
        * 0 = No restriction, 1 = The password cannot be a simple string
        */
        IMPORT_C TInt SetPasscodeDisallowSimple( TBool aDisallowSimple );
        		
        /**
        * 0 = No restriction, 1 = The password cannot be a simple string
        */
        IMPORT_C TInt GetPasscodeDisallowSimple( TBool& aDisallowSimple );
//#endif
// --------- Enhanced features END --------------------
		

        /**
		* Get process list
        */
		IMPORT_C TInt GetProcessListL( CBufFlat* aBuffer );

        /**
		* Get process list size
        */
		IMPORT_C TInt GetProcessListSize( TInt &aSize );

        /**
		* Get process list size
        */
		IMPORT_C TInt GetProcessListSize( TDes8 &aSizeBuf );

        /**
		* Get process information
        */
		IMPORT_C TInt GetProcessDataL( CBufFlat* aBuffer, const TDesC8& aProcessName, const TDesC8 &aData );

        /**
		* Reboot device, restart actually happens after subsession is closed
        */
		IMPORT_C TInt RebootDevice( TBool aRestartDevice=ETrue );

        /**
		* Start local operation (new process)
        */
		IMPORT_C TInt StartNewProcess( TBool aIsUid, const TDesC8& aProcessName );

        /**
		* Stop local operation (in process list)
        */
		IMPORT_C TInt StopProcess( TBool aIsUid, const TDesC8& aProcessName );

};

#endif //__TERMINALCONTROLCLIENT_H__
