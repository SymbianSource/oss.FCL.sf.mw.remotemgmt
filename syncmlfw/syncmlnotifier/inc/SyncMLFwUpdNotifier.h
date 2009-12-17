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
* Description:  Synchronisation server alert and application launcher notifier.
*
*/



#ifndef CSYNCMLFWUPDNOTIFIER_H
#define CSYNCMLFWUPDNOTIFIER_H

//  INCLUDES
#include <SyncMLNotifierParams.h>
#include "SyncMLNotifierBase.h"    // Base class
#include "SyncMLMessageQueryDialog.h"
#include <SyncService.h>

// CONSTANTS
// Taken from S60\OmaDSAppUi\inc\NSmlDMSyncPrivateCRKeys.h
const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};
const TUint32 KNSmlDMFotaConfigurationKey = 0x00000001;
const TUint KNSmlDMFotaEnableFotaViewFlag = 0x0004;

//Constants required to indicate key presses in dialogs
enum 
	{
    ESyncMLDlgRespKeyNone = -3,
    ESyncMLDlgRespKeyOk = -2,
    ESyncMLDlgRespKeyOthers = -1,
    ESyncMLDlgRespKeyCancel = 0
    };


//Constants used when update happen NSC/DM 
enum {
		EFotaUpdateNSC=1,
		EFotaUpdateNSCBg,
		EFotaUpdateDM
};

// CLASS DECLARATION
/**
*  Notifier for showing the rsult of the firmware
*  update to the user.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CSyncMLFwUpdNotifier ) : public CSyncMLNotifierBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLFwUpdNotifier* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLFwUpdNotifier();

    private: // New functions

        /**
        * Shows a note describing the error situation that has been encountered
        * when checking the prerequisites for firmware update.
        * @param None
        * @return Result to the query
        */
        TInt ShowUpdateErrorNoteL();

        /**
        * Shows a note describing the result of firmware update.
        * @param None
        * @return Result to the query
        */
        TInt ShowUpdateResultNoteL();

        /**
        * Informs the user about the firmware update and its consequences.
        * Queries if the user wants to continue with the update.
        * @param None
        * @return Result to the query
        */
        TInt ShowUpdateStartQueryL();
		
        /**
        * Informs the user about the firmware update while encrypted phone memory.
        * Queries if the user wants to continue with the update.
        * @param None
        * @return Result to the query
        */
        TInt ShowUpdateStartQueryEncryptionL();
        
        /**
        * Shows a note describing the result of firmware update. Also queries
        * user to encrypt memory 
        * @param None
        * @return Result to the query
        */
        TInt ShowUpdateResultWithEncryptionNoteL();

        /**
        * Retrieves the profile name using the profile identifier stored
        * in iNoteInfo.
        * @param aServerName Returns the clear-text name of the corresponding server.
        * @return None
        */
        void RetrieveProfileNameL( TDes& aServerName );
        /*Information note shown  to the user after selecting No reminder case in
	    * the reminder dialog. This note informs the user that downloaded package 
 	    * can be installed manually. 
 	    */

        TInt ShowNoReminderQueryL();
        
        /*Message query note shown  to the user when ever there is no enough memory
        * space to download the update package
 	    */
        TInt ShowNotEnoughMemoryL( TInt &aMemoryNeeded ,TBool aOmaDL );

				/*Message query note shown to the user when ever the user cancels the download
				 * and whether the user wants to resume the download at a later point of time. 
				 * @param None
         * @return Result to the query
 	       */
				TInt ShowDownloadUserCancelL();
		
				/*Message note shown to the user when ever 
				 * the user postponds the download 
		     * @param None
         * @return Result to the query
 	       */
				TInt ShowDownloadPostponedL();
		
				/*Message note shown to the user when ever 
				 * the user cancels the download 
		     * @param None
         * @return Result to the query
 	       */
				TInt ShowDownloadCancelledL();
		
				/*Message note shown to the user when ever 
				 * the user wants to resume the download.
		     * @param None
         * @return Result to the query
 	       */
				TInt ShowDownloadResumeL();
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
        * From CSyncMLNotifierBase Gets called when a request completes.
        * @param None.
        * @return None.
        */
        void RunL();

        /**
        * From CSyncMLNotifierBase Used in asynchronous notifier launch to 
        * store received parameters into members variables and 
        * make needed initializations.
        * @param aBuffer A buffer containing received parameters
        * @param aReturnVal The return value to be passed back.
        * @param aMessage Should be completed when the notifier is deactivated.
        * @return None.
        */
        void GetParamsL( const TDesC8&       aBuffer,
                               TInt          aReplySlot,
                         const RMessagePtr2& aMessage );
        /**
        * IsLanguageSupported
        * @param ret
        * @return True if the language is supported.
        */                 
				TBool IsLanguageSupportedL();
    private:

        /**
        * C++ default constructor.
        */
        CSyncMLFwUpdNotifier();

    private: // Data
        // Type of note to be shown
        TSyncMLFwUpdNoteTypes   iNoteType;
        
        // Possible additional information required by the note
        TInt                    iNoteInfo;
        
        //Memory needed for the for the fota update package to be downloaded
//        #ifdef RD_FOTA_FLEXIBLE_MEMORY_USAGE
        TInt        iFotaMemoryNeeded;
//        #endif
        //Checks whether dialog from RUNL popped and waiting for user input
        TBool iRunLhang;
        
        //Reference to the Dialog launched from RunL
        CSyncMLMessageQueryDialog* iDlg;
        
		//To decide on encryption note or default note.
        TBool iFotaEncryptReq;

    };


#endif      // CSYNCMLFWUPDNOTIFIER_H   
            
// End of File
