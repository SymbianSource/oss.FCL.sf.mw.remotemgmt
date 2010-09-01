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
* Description:  Client-side service implementations
*
*/


#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include <AknServerApp.h>
#include <SyncServiceConst.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apaidpartner.h>
#else
#include <apaid.h>
#endif


struct  TSyncServiceParam
 	{
 	TUint   iJobId;
 	TUint   iProfileId;
 	TUint   iServiceId;
 	TUint   iSilent;
 	};
 	
/**
*  RSyncService  
*
*  Lower level client side service class
*
*  @lib syncservice.lib
*  @since Series 60 3.0
*/
class RSyncService : public RAknAppServiceBase
	{
	public: 
		/**
		* Sends the synchronisation start command to the
		* server side with parameters aParams
		*/
		void StartSyncL( TSyncServiceParam aParam );
		
		/**
		* Sends command that enables or disables the SyncML session
		* progress notes to the server side.
		* @param aEnabled Should the progress note be enabled or disabled.
		*/
		void EnableProgressNoteL( TBool aEnabled );

	private: // From RApaAppServiceBase
		/**
		* Returns the uid of the service
		*/
		TUid ServiceUid() const;
	};

/**
*  CSyncService  
*
*  Higher level client side service class
*
*  @lib syncservice.lib
*  @since Series 60 3.0
*/
class CSyncService : public CBase
	{
	public:

        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSyncService* NewL( MAknServerAppExitObserver* aObserver,
		                                    TUint aServiceId );
		
        /**
        * Destructor.
        */
        virtual ~CSyncService();
		
    public:

		/**
		* StartSyncL
		*
		* @param aParam Parameters for the service so that it
		*               knows which service command to issue
        * @return None
		*/
    	IMPORT_C TUid StartSyncL( TSyncServiceParam aParam );

		/**
		* Sends a command to the sync or devman application to
		* indicate that the progress note should be enabled
		* or disabled.
		* @param aEnabled Should the progress not be shown.
        * @return None
		*/
        IMPORT_C void EnableProgressNoteL( TBool aEnabled );
        
	private:
	
        /**
        * C++ default constructor.
        */
		CSyncService();
	
		/**
		* 2nd phase constructor
		* First calls Discover to find the right application with aParams.
		* Then calls iService to connect to right application.
		* Creates an instance of CApaServerAppExitMonitor		
		* Creates an instance of CSyncService.
		* Sets itself as observer for the service.
		*
		* @param aObserver  pointer of MAknServerAppExitObserver to be passed to iMonitor
		* @param aServiceId parameters for the service so that it
		*                   knows which application to start	
        * @return None
		*/
		void ConstructL(  MAknServerAppExitObserver* aObserver,
		                  TUint aServiceId );

		/**
		* DiscoverL
		* Finds the right application id to be started and returns it.
		* Connects RApaLsSession and initializes
		* it with GetServerApps command. Then gets all the services that
		* implement the service by using GetServiceImplementationsLC. 
		* Calls TryMatchL for each service and if there's a match,
		* the appId is returned. If no implementation is found, 0 is returned.
		*
		* @param aServiceId parameters for the service so that it
		*                   knows which application to start	
        * @return None
		*/
    	TUid DiscoverL( TUint aServiceId );

		/**
		* TryMatchL
		* Reads the service type information from OpaqueData.
		* Sets aAppUi as the application id.
		*
		* @param aInfo      Application service information.
        * @param aAppUid    Application Uid
		* @param aServiceId Parameters for the service so that it
		*                   knows which application to start
        * @return Boolean   ETrue if not found, returns EFalse.
		*/
    	TBool TryMatchL( const TApaAppServiceInfo aInfo,
    	                 TUid& aAppUid,
    	                 TUint aServiceId );

	private:
		// Lower level client-side service implementation
		RSyncService 		        iService;
		// Monitor for checking events from the server side
		CApaServerAppExitMonitor* 	iMonitor;
		TSyncServiceParam           iParameters;
		TUid                        iUid;
		TUint                       iServiceId;
	};

#endif  // SYNCSERVICE_H

// End of File

