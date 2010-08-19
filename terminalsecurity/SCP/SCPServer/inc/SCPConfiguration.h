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


#ifndef SCPCONFIGURATION_H
#define SCPCONFIGURATION_H

//  INCLUDES
#include <bldvariant.hrh>
#include "SCPServerInterface.h"

// DATA TYPES

// Parameter types
enum TSCPParamType
    {
    EParTypeInt = 0,
    EParTypeDesc
	};

// LOCAL CONSTANTS

// Encryption
_LIT8( KSCPEncryptionKeyMod, "overview" );
const TInt KSCPDesBlockSize( 8 );
_LIT( KSCPCryptSuffix, "SCP" );

// The settings-file name
_LIT( KDriveC, "C:" );
_LIT( KSCPSettingsFilePathAndName, "\\private\\10207836\\config.ini" );
_LIT( KSCPSettingsFileName, "config.ini" );

// Definitions for the structures that control the parameter-parsing

const TInt KParamIDMaxTimeout = 1;
const TInt KParamIDSecCode = 2;
const TInt KParamIDEnhSecCode = 3;
const TInt KParamIDConfigFlag = 4;
const TInt KParamIDFailedAttempts = 5;
const TInt KParamIDBlockedAtTime = 6;

const TInt KParamIDBlockedInOOS = 7;
// ID number for the encrypted security code.
const TInt KParamIDCryptoCode = 8;


// Total number of Param IDs
const TInt KTotalParamIDs = 8;

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
#define SCP_PARAMETERS_WithFlag { &iConfigFlag, \
                         &iMaxTimeout, \
                         &iSecCode, \
                         &iEnhSecCode, \
                         &iFailedAttempts, \
                         &iBlockedAtTime, \
                         &iBlockedInOOS, \
                         &iCryptoCode \
						 };

#define SCP_PARAMIDS_WithFlag    {  KParamIDConfigFlag, \
                           KParamIDMaxTimeout, \
                           KParamIDSecCode, \
                           KParamIDEnhSecCode, \
                           KParamIDFailedAttempts, \
                           KParamIDBlockedAtTime, \
                           KParamIDBlockedInOOS, \
                           KParamIDCryptoCode \
						   };

#define SCP_PARAMTYPES_WithFlag { EParTypeInt, \
                         EParTypeInt, \
                         EParTypeDesc, \
                         EParTypeDesc, \
                         EParTypeInt, \
                         EParTypeDesc, \
                         EParTypeInt, \
                         EParTypeDesc\
						};

//#else // !__SAP_DEVICE_LOCK_ENHANCEMENTS                   

// Definitions for the structures that control the parsing
#define SCP_PARAMETERS_WithOutFlag { &iConfigFlag, \
                         &iMaxTimeout, \
                         &iSecCode, \
                         &iBlockedInOOS \
                         };

#define SCP_PARAMIDS_WithOutFlag    {  KParamIDConfigFlag, \
                           KParamIDMaxTimeout, \
                           KParamIDSecCode, \
                           KParamIDBlockedInOOS \
                        };

#define SCP_PARAMTYPES_WithOutFlag { EParTypeInt, \
                         EParTypeInt, \
                         EParTypeDesc, \
                         EParTypeInt \
                       };
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS                   

// CLASS DECLARATION

/**
*  TSCPConfiguration
*  A class, which handles the configuration data for the SCP server component
*/
class TSCPConfiguration
	{		
	public:  // Methods
           
        // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        TSCPConfiguration( RFs* aRfs );

        // New methods
        
        /**
        * Initializes the configuration object. Must be called before using any other methods.        
        */
        void Initialize();
        
        /**
        * Reads the setup information form flash.
        * @return TInt: A generic status code indicating the operation status.
        */
        TInt ReadSetupL();
        
        /**
        * Writes the setup to flash.
        * @return TInt: A generic status code indicating the operation status.
        */
        TInt WriteSetupL(); 
        
    private: // Methods
    
        /**
        * Returns the key to use for symmetric encryption.        
        */
        void GetKeyL( TDes8& aKey );
        
        /**
        * Encrypts or decrypts the input string in aInput, saving the output in aOutput.
        * If the input is not an exact multiple of a DES block, 0-padding is used. The
        * output buffer must be large enough to store the output blocks.        
        */
        void TransformStringL( TBool aEncrypt, TDes& aInput, TDes& aOutput );              
        
        /**
        * Function to check for any ECOM plugin implementing the MDMEncryptionUtilInterface exist in ROM only.
        * If any plugin is implemented then Encrypts or decrypts the input string in aInput, saving the output in aOutput.
        * If the input is not an exact multiple of a DES block, 0-padding is used. The
        * output buffer must be large enough to store the output blocks.
        * Returns   ETrue if plugin is implemented & exists in ROM only.
        *           EFalse if plugin is not implemented.        
        */
        TBool NativeTransform(TBool aEncrypt, TDes& aInput, TDes& aOutput);        

	public:  // Data
	    /** Configuration valid -flag */        
        TInt iConfigFlag;    
        /** The Autolock max.timeout */        
        TInt iMaxTimeout;
        /** The security code stored on the server */
        TSCPSecCode iSecCode;  
        /** The name of the main configuration file */
        TFileName iConfigFileName; 
        TInt iBlockedInOOS;
       
        /** A ptr to a connected RFs, not owned */
        RFs* iFsSession;
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
        /** The enchanced security code stored on the server */
        TBuf<KSCPMaxEnhCodeLen> iEnhSecCode;
        /** The amount of consecutive authentication failures */
        TInt iFailedAttempts;
        /** The time since the code has been blocked */
        TBuf<KSCPMaxInt64Length> iBlockedAtTime;
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS       
// Member to hold the encrypted/decrypted security code from Crpto HW encryption algorithm
        TSCPCryptoCode iCryptoCode;

    };

#endif      // SCPCONFIGURATION_H   
            
// End of File

