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


#ifndef __CSCPPTIMESTAMPPLUGIN_H
#define __CSCPPTIMESTAMPPLUGIN_H

// INCLUDES
#include <e32base.h>
#include "SCPPlugin.h"
#include <f32file.h>
#include <barsc.h>

#include <TerminalControl3rdPartyAPI.h>


// LOCAL CONSTANTS
const TInt KSCPMaxExpiration = 365;
const TInt KSCPMaxTolerance = 8;
const TInt KSCPMaxInterval = 1000;
const TInt KSCPMaxMinChangeValueLen = 10; // Max length of interval+tolerance parameters
const TInt KSCPMinAttempts = 3;
const TInt KSCPMaxAttempts = 100;

const TInt KSCPExpireImmediately = -1;

const TInt KSCPLastChangeTime = 1;
const TInt KSCPIntervalStartTime = 2;
const TInt KSCPUsedTolerance = 3;
const TInt KSCPFailedAttempts = 4;
const TInt KSCPExpireOnNextCall = 5;

const TInt KSCPIsAfter = 1;
const TInt KSCPIsNotAfter = 0;

const TInt KSCPTypeMinutes = 0;
const TInt KSCPTypeHours = 1;
const TInt KSCPTypeDays = 2;

_LIT( KSCPTSConfigFile, "SCPTimestampPlugin.ini");
_LIT( KDriveZ, "Z:" );
_LIT(KSCPTimestampPluginResFilename, "\\Resource\\SCPTimestampPluginLang.rsc");

_LIT(KSCPFormatScript, "FORMAT %c:");
const TInt KSCPFormatScriptMaxLen = 32;
_LIT( KSCPDosResetCommand, "DOS 1" );



// FORWARD DECLARATIONS
class CSCPParamObject;

// CLASS DECLARATION

/**
*  CSCPTimestampPlugin 
*/
class CSCPTimestampPlugin : public CSCPPlugin
    {
    public: // Constructors, Destructor
		/**
        * 2-phased constructor
        * @return Instance of Plug-in
        */
	    static CSCPTimestampPlugin* NewL();

        /**
        * Destructor
        */
	    virtual ~CSCPTimestampPlugin();

        /**
        * Event handler
        */		
	    void HandleEventL( TInt aID, CSCPParamObject& aParam, CSCPParamObject& aOutParam );
		
		void SetEventHandler( MSCPPluginEventHandler* aHandler );

    private: // Constructors, Destructor
		/**
        * Constructor
        */
	    CSCPTimestampPlugin();

		/**
        * 2nd phase constructor
        */
        void ConstructL();
        
        
    private : // Methods   
		
		/**        
        * Checks if the minimum timeout has expired since the last change
        */
		void IsChangeAllowedL( CSCPParamObject& aParam, CSCPParamObject& aRetParams );	 
		
		/**        
        * Updates the status when the password is changed
        */
        void PasswordChanged( CSCPParamObject& aParam, CSCPParamObject& aRetParams );
        
		/**
        * Updates the status when after an authentication attempt
        */
        void AuthenticationAttempt( TBool aIsSuccessful, 
                                    CSCPParamObject& aParam,
                                    CSCPParamObject& aRetParams );
                                    
		/**
        * Updates the status after successful authentication
        */
        void SuccessfulAuthenticationL( CSCPParamObject& aParam,
                                       CSCPParamObject& aRetParams );
        
        
		/**
        * Checks if the given time interval is after the saved time (aConfID)
        */
        TInt IsAfter( TInt aConfID, TInt aInterval, TInt aIntType );
            
        
		/**
        * Handles the Configuration Query event
        */
        void ConfigurationQuery(    TInt aParamID, 
                                    CSCPParamObject& aParam, 
                                    CSCPParamObject& aRetParams );
                                    
        /**
        * Initiates RFS Deep (device wipe)
        */
        void WipeDeviceL( CSCPParamObject& aRetParams );
                                    
		/**
        * Read the configuration from flash.
        */
        TInt ReadConfiguration();
        
		/**
        * Writes the configuration to flash.
        */
        TInt WriteConfiguration();                                             
        
		/**
        * Fetches the resource data from the file.
        */        
        TInt GetResource();
        
		/**
        * Loads a specified resource buffer.
        */
        HBufC16* LoadResourceL( TInt aResId );
        
        void FormatResourceString(HBufC16 &aResStr);
    private: // Data
        /* A pointer to the event handler object on the server, not owned */
        MSCPPluginEventHandler* iEventHandler;
        
        /* A pointer to the configuration object, owned */
        CSCPParamObject* iConfiguration;
        
	    /* The expiration timeout, days */
	    TInt iExpiration;
	    
	    /* Minimum change interval, hours */
	    TInt iMinInterval;

	    /* Amount of changes allowed inside change interval */
	    TInt iMinTolerance;

	    /* Maximum amount of failed attempts before device wipe */
	    TInt iMaxAttempts;  
	    
	    /* File server session ptr, not owned */
	    RFs* iFsSession;
	    
        /* Resourcefile-object */
	    RResourceFile iResFile;
	    
	    /* Resource-file indicator */
	    TBool iResOpen;   
    };

#endif // __CSCPPTIMESTAMPPLUGIN_H

// End of File
