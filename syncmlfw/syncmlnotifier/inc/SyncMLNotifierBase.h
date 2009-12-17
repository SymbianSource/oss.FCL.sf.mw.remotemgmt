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
* Description:  Declares SyncML notifiers base class.
*
*/



#ifndef CSYNCMLNOTIFIERBASE_H
#define CSYNCMLNOTIFIERBASE_H

//  INCLUDES
#include <SyncMLNotifier.h>

// CONSTANTS
// Channel for app starter notifier
const TUid KSmlAppLaunchChannel = {0x101F8769};

// Channel for server dialog notifier
const TUid KSmlDlgChannel       = {0x101F876A};
const TUid KSmlFwUpdChannel     = {0x102072BF}; // Channel for firmware update notifier

// Number of notifiers
const TInt KSmlNotifierArrayIncrement = 3;  // Number of notifiers

// FORWARD DECLARATIONS
class CEikonEnv;

// CLASS DECLARATION

/**
* The base class for the SyncML notifiers.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CSyncMLNotifierBase ) : public CActive,
                                           public MEikSrvNotifierBase2
    {
    public:  // Constructors and destructor
    
        /**
        * Destructor.
        */
        virtual ~CSyncMLNotifierBase();


    protected:  // New functions
        
        /**
        * Used in asynchronous notifier launch to store received parameters 
        * into members variables and make needed initializations.        
        * @since Series 60 3.0
        * @param aBuffer A buffer containing received parameters
        * @param aReturnVal The return value to be passed back.
        * @param aMessage Should be completed when the notifier is deactivated.
        * @return None.
        */
        virtual void GetParamsL(const TDesC8& aBuffer, 
                                      TInt aReplySlot, 
                                const RMessagePtr2& aMessage) = 0;

        /**
        * A utility function for requesting the activation of lights.
        * @since Series 60 3.0
        * @param None.
        * @return None.
        */
        void TurnLightsOn();

        /**
        * A utility function for checking whether autolock is on.
        * @since Series 60 3.0
        * @param None.
        * @return A boolean according to autolock state.
        */
        TBool AutoLockOnL();

		/**
		* A function for checking the status of Apps key.
        * @since Series 60 3.0
		* @param aEnable A Boolean according to Apps key status
		* @return None.
		*/
		void SuppressAppSwitching( TBool aEnable );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        virtual void ConstructL();

    protected:  // Functions from base classes

        /**
        * From CActive Gets called when a request completes.
        * @param None.
        * @return None.
        */
        virtual void RunL() = 0;

        /**
        * From CActive Gets called when a leave occurres in RunL.
        * @param aError Symbian OS errorcode.
        * @return None.
        */
        virtual TInt RunError( TInt aError );

        /**
        * From MEikSrvNotifierBase2 Called when a notifier is first loaded 
        * to allow any initial construction that is required.
        * @param None.
        * @return A structure containing priority and channel info.
        */
        virtual TNotifierInfo RegisterL() = 0;
        
        /**
        * From MEikSrvNotifierBase2 The notifier has been deactivated 
        * so resources can be freed and outstanding messages completed.
        * @param None.
        * @return None.
        */
        virtual void Cancel();

    protected:

        /**
        * C++ default constructor.
        */
        CSyncMLNotifierBase();

    private: // Functions from base classes        
        
        /**
        * From MEikSrvNotifierBase2 Called when all resources allocated 
        * by notifiers should be freed.
        * @param None.
        * @return None.
        */
        virtual void Release();

        /**
        * From MEikSrvNotifierBase2 Return the priority a notifier takes 
        * and the channels it acts on.
        * @param None.
        * @return A structure containing priority and channel info.
        */
        virtual TNotifierInfo Info() const;

        /**
        * From MEikSrvNotifierBase2 Synchronic notifier launch.        
        * @param aBuffer Received parameter data.
        * @return A pointer to return value.
        */
        virtual TPtrC8 StartL( const TDesC8& aBuffer );
        
        /**
        * From MEikSrvNotifierBase2 Asynchronic notifier launch.
        * @param aBuffer A buffer containing received parameters
        * @param aReturnVal The return value to be passed back.
        * @param aMessage Should be completed when the notifier is deactivated.
        * @return A pointer to return value.
        */
        virtual void StartL( const TDesC8& aBuffer,
                                   TInt aReplySlot,
                             const RMessagePtr2& aMessage );
        
        /**
        * From MEikSrvNotifierBase2 Updates a currently active notifier.
        * @param aBuffer The updated data.
        * @return A pointer to return value.
        */
        virtual TPtrC8 UpdateL( const TDesC8& aBuffer );       

        /**
        * From CActive Gets called when a request is cancelled.
        * @param None.
        * @return None.
        */
        virtual void DoCancel();   
		
    protected:  // Data
        // Received message
        RMessagePtr2    iMessage;
        
        // Reply slot
        TInt            iReplySlot;

        // Flag for releasing messages
        TBool           iNeedToCompleteMessage;
        
        // Notifier parameters structure
        TNotifierInfo   iInfo;
        
        // Local eikonenv
        CEikonEnv*      iEikEnv;

        // Apps key status 
        TBool           iAppsKeyBlocked;

        // Flag for eikon env.
        TInt            iResourceFileFlag;
    };

#endif      // CSYNCMLNOTIFIERBASE_H
            
// End of File
