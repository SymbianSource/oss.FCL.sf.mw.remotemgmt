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
* Description:  Server alerts for Data Sync and Device Management engines. 
*                
*
*/



#ifndef CSYNCMLAPPLAUNCHNOTIFIER_H
#define CSYNCMLAPPLAUNCHNOTIFIER_H

//  INCLUDES
#include <SyncMLClient.h>

#include <SyncMLNotifierParams.h>
#include "SyncMLNotifierBase.h"    // Base class
#include <SyncService.h>
#include <centralrepository.h>

// CONSTANTS
// From S60\SyncMLFota
const TUid KUidNSmlMediumTypeInternet = { 0x101F99F0 };
_LIT8( KNSmlIAPId, "NSmlIapId" );
_LIT8( KNSmlAlwaysAsk, "-1" );
const TInt  KDefaultTimeoutforNotes ( 30 );

const TUid KCRUidNSmlDSApp       = { 0x101F9A1D };
// CLASS DECLARATION
/**
*  Notifier for launching SyncML applications
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CSyncMLAppLaunchNotifier ) : public CSyncMLNotifierBase
    {
    public:  // Constructors and destructor
        enum TSanSupport
        {
        	EOff,EON
        };
        /**
        * Two-phased constructor.
        */
        static CSyncMLAppLaunchNotifier* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLAppLaunchNotifier();
        
    public:
        /**
        * Returns correct SyncService depending on the Service Id
        * @param aServiceId
        * @return CSyncService* 
        */
        CSyncService* SyncServiceL( TUint aServiceId );

    private: // New functions

        /**
        * Retrieves required parameters for the note and launching of the
        * synchronization application.
        * @param aParam Parameter structure that is forwarded to syncservice
        * @param aServerName Returns the clear-text name of the corresponding server.
        * @param aUserInteraction Returns the profile setting of whether the user
        *                         acceptance should be queried.
        * @return None
        */
        void RetrieveSyncParamsL( TSyncServiceParam& aParam,
                                  TDes& aServerName,
                                  TSmlServerAlertedAction& aUserInteraction );

        /**
        * Retrieves required parameters for the note and launching of the
        * device management application.
        * @param aParam Parameter structure that is forwarded to syncservice
        * @param aServerName Returns the clear-text name of the corresponding server.
        * @param aUserInteraction Returns the profile setting of whether the user
        *                         acceptance should be queried.
        * @return None
        */
        void RetrieveMgmtParamsL( TSyncServiceParam& aParam,
                                  TDes& aServerName,
                                  TSmlServerAlertedAction& aUserInteraction );

        /**
        * Checks if the user and/or parameters have defined that the session
        * should be accepted without querying the user.
        * @param aUserInteraction Sync profile interaction enumeration.
        * @return True, if the session is accepted automatically.
        */
        TBool IsSilent( TSmlServerAlertedAction& aUserInteraction );

    private: // Functions from base classes

        /**
        * From CSyncMLNotifierBase Called when a notifier is first loaded.        
        * @param None.
        * @return A structure containing priority and channel info.
        */
        TNotifierInfo RegisterL();

        /**
        * From CSyncMLNotifierBase The notifier has been deactivated 
        * so resources can be freed and outstanding messages completed.
        * @param None.
        * @return None.
        */
        void Cancel();

        /**
        * From CSyncMLNotifierBase Used in asynchronous notifier launch to 
        * store received parameters into members variables and 
        * make needed initializations.
        * @param aBuffer A buffer containing received parameters
        * @param aReturnVal The return value to be passed back.
        * @param aMessage Should be completed when the notifier is deactivated.
        * @return None.
        */
        void GetParamsL( const TDesC8& aBuffer,
                               TInt aReplySlot,
                         const RMessagePtr2& aMessage );

        /**
        * From CSyncMLNotifierBase Gets called when a request completes.
        * @param None.
        * @return None.
        */
        void RunL();
        
        /**
        * From RConnectionMonitor gets the staus of roaming .
        * If roaming returns true else returns false
        * @param ret
        * @return None.
        */
        void IsRoamingL(TBool& ret);
        /**
        * HandleDMSession
        * @param ret
        * @return TBool 
        */
				TBool HandleDMSessionL();
				/**
        * HandleCompleteMessage
        * @param TInt &keypress, TBool &silent, TInt &SanSupport, TInt &Timeout, TInt &CustomNotes
        * @return None.
        */
				void HandleCompleteMessageL(TInt &keypress, TBool &silent, TInt &SanSupport, TInt &Timeout, TInt &CustomNotes);
				/**
        * IsLanguageSupported
        * @param ret
        * @return True if the language is supported.
        */
				TBool IsLanguageSupportedL();
        
        void ReadRepositoryL(TInt aKey, TInt& aValue);
        
        void ShowRoamingMessageL(TInt keypress, TUint profileId);

    private:

        /**
        * C++ default constructor.
        */
        CSyncMLAppLaunchNotifier();

    private: // Data
        // Type of session queried
        TSyncMLSessionTypes iSmlProtocol;
        
        // Job identifier from the parameters
        TInt                iJobId;
        
        // Identifier of the used SyncML profile from the parameters
        TInt                iProfileId;
        
        // Sync Service for Data Sync
        CSyncService*       iDSSyncService;
        // Sync Service for Device Management
        CSyncService*       iDMSyncService;
        // Is always ask selected as accesspoint for current profile
        TBool               iAlwaysAsk;
       // Alert mode
        TInt iUimode;        
        // DM Sync Server        
        TBuf<KSyncMLMaxProfileNameLength> DMSyncServerName;
        // UI behaviour 
    		TSmlServerAlertedAction uiAction;
    		// Sync Service param 
   			TSyncServiceParam param;
   			// Stringholder
    		HBufC* stringholder;  
    		// Central Respository   
    		CRepository* centrep; 
        // To store the bearer type
        TInt iBearerType;
    };


#endif      // CSYNCMLAPPLAUNCHNOTIFIER_H   
            
// End of File
