/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PnpProvUtil header file - This header file is used by any 
*                applications using PNPMS and Provisioning utilities
*
*/


#ifndef PNP_PROV_UTIL_H
#define PNP_PROV_UTIL_H

#include <e32base.h>

// Connection reason for launching online support
enum TConnectReason
{
	EUnKnown = 0,
	EFirstBoot,
	ENewSIM,
	EUserTriggered,
	EApplicationTriggered,
	ERemoteTriggered
};

//Interactive or NonInteractive mode of onlinesupport startup
enum TStartMode
{
	EStartInteractive = 0,
	EStartNonInteractive
};

//PnPMS servers type
enum TServers
{
	EHelpPortal = 0,
	ENSA,
	EPnPMSNokia,
	EWDSGlobal
};


/**
 * CPnpProvUtil class provides interface for application to set UID and 
 * provisioning adapters to set the settings application ID.
 * PNPMS client get these values before activating service.
 * This class can be used for launching online support in different modes
 * of operation.
 *
 * @lib PnpUtil.lib
 * @since Series 60 3.2.3
 */


class CPnpProvUtil : public CBase
    {
   
    public:  // Constructors and destructor
        
       /**
        * Two-phased constructor.
        */
        IMPORT_C static CPnpProvUtil* NewL();

       /**
        * Two-phased constructor.
        */
        IMPORT_C static CPnpProvUtil* NewLC();

       /**
        * Destructor.
        */
        ~CPnpProvUtil();
        
     public: // New methods
    
       /**
        * Set the UID of application that launched PnPMS service for downloading 
        * configuration settings.
        * @param  aUid - Uid of the application.
        * @return None
        * @since  Series 60 3.2.3
        */  
        IMPORT_C void SetApplicationUidL(TUint32 aUid); 
        
       /** 
        * Get the UID of the application to be launched. This UID is same 
        * as the one that is set using SetApplicationUidL method.
        * @param  None
        * @return Get UID of application to be launched
        *         Default return value is Zero in case if there is no 
        *         call to SetApplicationUidL method
        * @since  Series 60 3.2.3
        */         
        IMPORT_C TUint32 GetApplicationUidL();
        
       /**
        * Set Provisioning application settings ID. Provisioning adapters set 
        * this value and indicate PNPMS client that application settings are 
        * saved successfully.
        * @param  appID - Provisioning settings application ID
        * @return None
        * @since  Series 60 3.2.3
        */
        IMPORT_C void SetProvAdapterAppIdL(TDesC& appID);
        
        /** 
         * Get Provisioning application settings ID. This value is used by 
         * PNPMS client before launching application to confirm application 
         * settings are downloaded successfully.
         * @param  appID - Reference of array is passed which has all the 
         *                 application IDs set by Provisioning adapters
         * @return None
         * @since  Series 60 3.2.3
         */
         IMPORT_C void GetProvAdapterAppIdsL(RPointerArray<HBufC>& appID);
        
        
        /**
         * Launches Online Support in different modes of PnPMS operation.
         * @param aUri           - URI of PnPMS server
         *        aConnectReason - connection reason will be provided by whoever 
         *                         launches PnPMS
         *        aMode          - PnPMS application can be started in different 
         *				     mode                         
         *        aTokenOverride - Token override
         *        aQueryString   - Help query string
         * @return None
         * @since Series 60 3.2.3
         */
         IMPORT_C void LaunchOnlineSupportL(TDesC& aUri, 
                                            TConnectReason aConnectReason, 
                                            TStartMode aMode, 
                                            TBool aTokenOverride, 
                                            TDesC& aQueryString);
         
        /** 
         * Construct Uri with PnPUtil parameters.
         * @param aUri    - URI of PnPMS server to append PnPUtil parameters     
         *        aServer - PnPMS server type
         * @return None
         * @since Series 60 3.2.3
         */
         IMPORT_C void ConstructUriWithPnPUtilL(TDes& aUri, TServers aServer);
                
      private:
        /**
         * C++ default constructor.
         */
         CPnpProvUtil();

        /** 
         *By default Symbian 2nd phase constructor is private.
         */
         void ConstructL();
        
        /** 
         * Parse application IDs set by provisioning adapters.
         * @param  aValue - String containing application IDs.
         *         array  - Reference to array of application IDs parsed.
         * @return None 
         * @since Series 60 3.2.3
         */
         void ParseValueL(TDesC& aValue, RPointerArray<HBufC>& array );
        
        /**
         * Set Cenrep value to key specified as parameter.
         * @param  aRepositoryUid - Central repository UID
         *         aKey           - repository key 
         *         aSetValue         - value set to repository key
         * @return None
         * @since Series 60 3.2.3
         */
         void SetCenrepValueL(TUid aRepositoryUid, TUint32 aKey, TDesC& aSetValue);

       /**
         * Get Cenrep value of key specified as parameter to SetCenrepValue.
         * @param  aRepositoryUid - Central repository UID
         *         aKey          - repository key 
         *         aGetValue     - value retrived from repository is stored in aGetvalue
         * @return None
         * @since Series 60 3.2.3
         */
         void GetCenrepValueL(TUid aRepositoryUid, TUint32 aKey, TDes& aGetvalue);
        
        /**
         * Construct command Line String for starting online support.
         * @param aUri           - URI of PNPMS server
         *        aConnectReason - connection reason will be provided by whoever 
         *                         launches PNPMS
         *        aMode          - PnPMS application can be started in different 
         *				     mode
         *        aTokenOverride - Token override
         *        aQueryString   - Help query string
         *        aCmdString     - appended command line string
         * @return None
         * @since Series 60 3.2.3
         */
         void ConstructCmdLineStringL(TDesC& aUri, 
                                        TConnectReason aConnectReason, 
                                        TStartMode aMode, TBool aTokenOverride, 
                                        TDesC& aQueryString, TDes& aCmdString);
          
    };

#endif PNP_PROV_UTIL_H
