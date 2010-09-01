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
* Description:   Header for updater active object
*
*/



#ifndef __UPDATER_H___
#define __UPDATER_H___

// INCLUDES
#include <etelmm.h>
#include <mmtsy_names.h> // for etel
#include <bautils.h>
#include <schinfo.h>

#include "FotaServer.h"
#include "fotaUpdateAgentComms.h"
#include "fotaengine.h"

// CONSTANTS
_LIT (KUpdateResultFile, "update.resp");
_LIT (KUpdateRequestFile, "update.req");
_LIT (KUpdateBitmap ,"installing.bmp");
_LIT (KRestartingBitmap ,"restarting.bmp");
_LIT (KUpdateTimeStampFileName, "updatetimestamp");
_LIT8 (KSwupdPath8, "c:\\private\\102072C4\\");
_LIT8 (KSwupdFileExt8, ".swupd");
_LIT(KDefaultSched, "1h:4h:1d:3d");  
_LIT(Ksep,";");  

// FORWARD  DECLARATIONS
class CFotaServer;
class CFotaUpdate;

// CLASS DECLARATION



/**
 *  Update related activities
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS( CFotaUpdate ): public CActive 
    {
public:

    static CFotaUpdate* NewL (CFotaServer* aServer);

    virtual ~CFotaUpdate ();


    /**
     * Start update
     *
     * @since   S60   v3.1
     * @param   aParams         details
     * @return  none
     */
    void StartUpdateL( const TDownloadIPCParams &aParams );


    /**
     * Update. boot.
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void UpdateL();


    /**
     * Check update result written update agent. Is it there?
     *
     * @since   S60   v3.1
     * @param   aRfs
     * @return  is it there
     */
    static TBool CheckUpdateResults( RFs& aRfs );



    /**
     * Delete update result file
     *
     * @since   S60   v3.1
     * @param   aRfs
     * @return  none
     */
    static void DeleteUpdateResultFileL( RFs& aRfs );



    /**
     * Execute update result file. That is, read result code. Send it to srv.
     *
     * @since   S60   v3.1
     * @param   aRfs
     * @return  none
     */
    void ExecuteUpdateResultFileL( RFs& aRfs );
    
    /**
     * Checks the FOTA Install update note variation     
     * @since   S60   v3.2
     * @param   None
     * @return  TBool, ETrue / EFalse
     */
    TBool CheckUpdateVariations();

    /**
     * HandleUpdateAcceptStartL handle the update process when the user pressed Now
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void HandleUpdateAcceptStartL(TBool aSkipBatteryChk = EFalse);
    
    /**
     * This method handles any update related error.
     *
     * @since   S60   v5.2
     * @param   the error code and value if applicable
     * @return  None, can leave with system wide error.
     */
    void HandleUpdateErrorL(TInt aError, TInt aValue = 0);
    
    /**
     * This method handles any encryption related error.
     *
     * @since   S60   v5.2
     * @param   the error code
     * @return  None, can leave with system wide error.
     */
    void HandleEncryptionErrorL(TInt aError);


protected: // from base classes


    /**
     * Hanlde notifier clicking
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */    
    virtual void RunL();


    /**
     * Cancel outstanding reqeust
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */    
    void DoCancel();


    /**
     * Handle leaving runl
     *
     * @since   S60   v3.1
     * @param   aError  error
     * @return  none
     */
    virtual TInt RunError(TInt aError);
    /**
     * Enum for Interval type of scheduling
     *
     * @since   S60   v3.2
     */   
    enum TUpdateIntervalType
	{
	/** The interval is based on hours. */
	EHourly,
	/** The interval is based on days. */
	EDaily,
	/** The interval is based on months. */
    EMonthly,
	/** The interval is based on years. */
	EYearly,
    /**Show the No reminder             */  
	ENoReminderOn,
	/**Show the No reminder             */   
    ENoReminderOff,
    /* Added for postpone limit, When All the postpones are used and user presses end key this command is sent */
    EEndKeyReminder
	};

private:

    CFotaUpdate ();

    /**
     * Check if there's enough battery power to update
     *
     * @since   S60   v3.1
     * @param   aUSBCharger
     * @return  TBool is there enough battery
     */
    TBool CheckBatteryL(TBool aUSBCharger);
        
    /**
     * HandleUpdateAcceptLaterL handle the update process when the user pressed Later
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void HandleUpdateAcceptLaterL();
    
    /**
     * HandleErrorBasedonChargingTypeL handle the error based on chrager type 
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void HandleErrorBasedonChargingTypeL();
    
    /**
     * SetUpdateFailedStatesL Set the state to update failed
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void SetUpdateFailedStatesL();
    
    /**
     * GetSchedulesInfoL Get the schedule info of FOTA update
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void GetSchedulesInfoL();
    
    /**
     * DisplayInstallationNoteType determines the proper installation note based on the Cenrep keys.
     *
     * @since   S60   v5.0
     * @param   None
     * @return  None
     */
    void DisplayInstallationNoteTypeL();


    /**
     * Launch notifier for user to interact with
     *
     * @since   S60   v3.1
     * @param   aNotetype
     * @param   aIntparam
     * @return  none
     */
    void LaunchNotifierL(  const TSyncMLFwUpdNoteTypes aNotetype , const TInt aIntParam,const TInt aEnc=EFalse);
    
    
    /**
     * Launch the postpone information note, when the postpone limit feature is enabled.
     *
     * @since   S60   v5.0
     * @param   aNotetype
     * @param   aIntparam
     * @param   aIntParam1
     * @param   aIntervalType
     * @param   aInterval
     * @return  none
     */
    void LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype,const TInt aIntParam,const TInt aIntParam1,TIntervalType aIntervalType,const TInt aInterval);

	/**
	 * Prepare and display Reminder Dialog using RFotaReminderDlg
	 * on radio button setting page
	 * @since 	S60	  v3.1
	 * @param None
	 * @return none
	 */
	void ShowReminderDialogL();
	
	/**
	 * Create the Schedule 
	 * @since 	S60	  v3.1
	 *
	 * @param package id 
	 * @param intervaltype
	 * @param interval
	 * @return TInt 
	 */
    TInt CreateScheduleL ( const TInt aPackageId
            ,TIntervalType aIntervalType,const TInt aInterval);
            
            
  /**
	 * Create the new Schedule 
	 * @since 	S60	  v3.1
	 *
	 * @param package id 
	 * @param aRet
	 * @param aRadioSelectionIndex
	 * @return None
	 */

    void CreateNewScheduleL(TInt aRet, TInt aRadioSelectionIndex);
    
  /**
	 * Fins the schedule or deletes it 
	 * @since 	S60	  v3.1
	 *
	 * @param aDelete if true the delete the schedule 
	 * @return TScheduleEntryInfo2
	 */
    TScheduleEntryInfo2 FindScheduleL ( const TBool aDelete );
    /**  
	 * Parse the schedule string from Cenrep
	 * @since 	S60	  v3.2
	 * @param None
	 * @return none
	 */
    void ParseScheduleStringL();

public:


	/**
	* Creates the Array of radio buttons and returns to the caller
	*/
	CDesCArray* CreateRadionButtonArrayLC(const TScheduleEntryInfo2 aScEntry,TInt& aSelectionIndex);

private:

    /**
     * State of the update package. For convenience.
     */
    TPackageState iUpdateState;


    /**
     * reference to fotaserver.
     * Not own
     */
    CFotaServer*    iFotaServer;


    /**
     * Notifier
     */
    RNotifier       iNotifier;


   
   /**
     * Interval Type  
     */
    
    RArray<TInt>  iIntervalType; 
    
    /**
     * Interval  
     */
     RArray<TInt>  iInterval;
    /**
     * Notifier params
     */
    TSyncMLFwUpdNotifParams     iNotifParams;



    /**
     * NOtifier response
     */    
    TSyncMLFwUpdNotifRetValPckg  iDummyResponsePckg;

    
    /**
     * file server session
     * not own
     */
    RFs*            iFs; 
    
    /**
     * scheduled  times
     */
    TBuf<255> iscInfo;
	
	/** 
	* fota update reminder task
	*/
	TFotaScheduledUpdate* iScheduledUpdate;
	/*
	 * flag to handle showing the reminder dialog twice.
	 */
	TBool iHandleUpdateAcceptLater;
	
    /*
     * tells the battery charge to monitor for.
     */
	TInt iChargeToMonitor;
	
	/*
	 * to finalize update later
	 */
	 
 	TBool iFinalizeLater;
	 
		/*
	 * flag to enable the FOTA Update Postpone  limit feature.
	 */
	TInt iFOTAUICustomization;
	
	/*
	 * To Store the number of postpones done by the user.
	 */
	TInt iFOTAUserPostponeCount;
	
	/*
	 * To Store the maximum number of postpones allowed for the user.
	 */
	TInt iFOTAMaxPostponeCount;
	
	/*
	 * To Get the Configuration values related to FOTA
	 */
	CRepository* iCentrep;
	
    };




#endif // __UPDATER_H___