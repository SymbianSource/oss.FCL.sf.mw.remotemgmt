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
* Description: 
*     Contains the implementation for RSCPClient, the client side interface
*     for SCP Server.
*
*/


#ifndef SCPCLIENT_H
#define SCPCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <bldvariant.hrh>
#include <SCPServerInterface.h>
#include <etelmm.h>

#include <SCPParamObject.h>

// Code change types. Forced change is used when password expires.
const TInt KSCPNormalChange = 1;
const TInt KSCPForcedChange = 2;

/**
*  Implements the public interface towards the SCP Server
*  Contains methods for controlling the server operation
*/
class RSCPClient : public RSessionBase
    {
    public:  // Methods

        // Constructors and destructor
        
        /**
        * C++ default constructor.
        */        
        IMPORT_C RSCPClient();

        // New methods
       
        /**
        * The standard connection method, starts the server process, if necessary       
        * @return A system error code indicating the result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success, 
        * <BR> otherwise an error indication
        */
        IMPORT_C TInt Connect();
        
        /**
        * Version query
        * @return The version number of the client DLL
        * <BR><B>Name of return value:</B> Version
        * <BR><B>Type of return value:</B> TVersion
        * <BR><B>Range of return value:</B> Three integer values
        * <BR><B>Contents of return value:</B> A TVersion-object, 
        * <BR>which contains the Major, Minor and Build version numbers
        */        
        IMPORT_C TVersion Version() const;
        
        /**
        * A method for querying the stored ISA code
        * @param aCode A buffer for storing the code
        * @return The result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success, 
        * <BR>otherwise an error indication
        */
        IMPORT_C TInt GetCode( TSCPSecCode& aCode );             
        
        /**
        * Used to store the new code, the user has set, in SCP
        * @param aCode A descriptor containing the new code
        * @return The result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success, 
        * <BR>otherwise an error indication
        */
        IMPORT_C TInt StoreCode( TSCPSecCode& aCode ); 
        
        /**
        * Used to change the system security code, uses the stored code 
        * as the current one.
        * @param aNewCode A descriptor containing the new code
        * @return The result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success, 
        * <BR>otherwise an error indication
        */
        IMPORT_C TInt ChangeCode( TDes& aNewCode );   
        
        /**
        * Used to lock the phone, uses the stored code for verifying the request 
        * @param aLocked ETrue to lock the phone, EFalse to unlock.    
        * @return The result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success,
        * <BR> otherwise an error indication
        */        
        IMPORT_C TInt SetPhoneLock( TBool aLocked ); 
        
        /**
        * Query the current state of the system lock.
        * @param aState Will contain the state after a successful call.
        * @return The result of the operation
        * <BR><B>Name of return value:</B> Result
        * <BR><B>Type of return value:</B> Integer
        * <BR><B>Range of return value:</B> System wide error code
        * <BR><B>Contents of return value:</B> KErrNone on success, 
        * <BR> otherwise an error indication
        */        
        IMPORT_C TInt GetLockState( TBool& aState );
        
        /**
        * A method for retrieving the operational status of the server in relation
        * to the given command.        
        * @param aCommand The command, whose status will be queried
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> The query will return ETrue 
        * in case the operation is actually in progress, or if it was already 
        * completed but not acknowledged via this call. This history information is
        * kept to ensure, that interested parties receive the information regardless
        * of the execution schedule.
        * Otherwise EFalse will be returned.        
        */        
        IMPORT_C TBool QueryAdminCmd( TSCPAdminCommand aCommand );
        
        /**
        * Get the value of an identified parameter stored on the server.
        * @param aParamID The ID of the requested parameter.
        * @param aTimeout The value to be set.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.
        *
        * NOTE: FUNCTION IS DEPRECATED. Use SetParamValue( TInt aParamID, TDes& aValue, const TUint32 aCallerID ) instead.
        */
        IMPORT_C TInt SetParamValue( TInt aParamID, TDes& aValue );

        /**
        * Get the value of an identified parameter stored on the server.
        * @param aParamID The ID of the requested parameter.
        * @param aValue Will contain the requested value after a successful call.
        * @return The status, see below
        * <BR><B>Name of return value:</B> Operation status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system wide error code
        * <BR><B>Contents of return value:</B> The status code of the operation.        
        */        
        IMPORT_C TInt GetParamValue( TInt aParamID, TDes& aValue ); 
    
        enum TSCPButtonConfig
            {
            SCP_OK,
            SCP_OK_CANCEL,
            SCP_OK_ETEL,
            SCP_OK_CANCEL_ETEL
            };            
        
        /**
        * Request the security code from the user.
        * 
        */        
        IMPORT_C TInt SecCodeQuery( RMobilePhone::TMobilePassword& aPassword, 
                                    TSCPButtonConfig aButtonsShown, 
                                    TBool aECSSupport,
                                    TInt aFlags );
        
        /**
        * Prompt the user to change the security code.
        */        
        IMPORT_C TInt ChangeCodeRequest(); 
        
        /**
        * Checks the server configuration and reports the status.
        * @param aMode The check mode, initial or complete. The initial mode only checks if the
        * configuration has already been validated.        
        */        
        IMPORT_C TInt CheckConfiguration( TInt aMode );

		/**
		* This function can be used to cleanup the parameter values stored by any
		* application, initialize the parameters to default values. The call notifies
		* the stakeholders (Other applications that may be using the same parameters) about the
		* change (value being revoked to default)
		*
		* @param:
		*   aEvent - One of the external events that lead to cleanup (Currently application
		* Uninstallation is the only event that is supported)
		*   aAppIDs - The UID's of the application(s) that were uninstalled/removed
		*/
        IMPORT_C TInt PerformCleanupL(RArray<TUid>& aAppIDs);
        
     /**
      * Get the value of an identified parameter stored on the server.
      * @param aParamID The ID of the requested parameter.
      * @param aValue The value to be set.
      * @param aCallerID The SID of the calling application.
      *
      * NOTE: Currently TCServer is the only component that is capable of calling this function
      * TCServer is assured to send the correct caller id always.
      *
      * @return The status, see below
      * <BR><B>Name of return value:</B> Operation status
      * <BR><B>Type of return value:</B> TInt
      * <BR><B>Range of return value:</B> A system wide error code
      * <BR><B>Contents of return value:</B> The status code of the operation.
      *
      * NOTE: FUNCTION IS DEPRECATED. Use SetParamValue( TInt aParamID, TDes& aValue, const TUint32 aCallerID ) instead.
      */
      IMPORT_C TInt SetParamValue( TInt aParamID, TDes& aValue, TUint32 aCallerID );        


	  IMPORT_C TInt SetAutoLockPeriod( TInt aValue );
    private: // Methods

        /**
        * Request the security code from the user (encapsulates functionalities of SecCodeQuery()).
        *
        */
    	TInt SetSecurityCodeL(RMobilePhone::TMobilePassword& aPassword, TSCPButtonConfig aButtonsShown,
                             TBool aECSSupport, TInt aFlags, TInt& aResFileSCP, TInt& aResFileSecUi);

        /**
        * Request the new code from the user and try to change the code
        */         
        TInt GetNewCodeAndChange( TDes& aOldCode, TInt aMode, TSCPSecCode* aNewDOSCode = NULL, HBufC** aNewCodePptr = NULL );
    
        /**
        * Process the server's response-commands
        */         
        void ProcessServerCommandsL( TDes8& aInParams, 
                                    CSCPParamObject** aOutParams = NULL,
                                    TBool isNotifierEvent = EFalse );
        
        /**
        * Show UI controls based on server commands
        */          
        void ShowUIL( CSCPParamObject& aContext );
        
        /**
        * Fetch the limit-values for code length
        */          
        void FetchLimits( TInt& aMin, TInt& aMax );
		void InformAutolockTask();
        TBool isFlagEnabled;        
    };

#endif      //SCPCLIENT_H   
            
// End of File

