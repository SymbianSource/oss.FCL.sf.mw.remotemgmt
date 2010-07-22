/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of DM Device Dialogs
 */

#ifndef __DMDEVDIALOGSERVER_H__
#define __DMDEVDIALOGSERVER_H__

#include <e32base.h>
#include <e32std.h>
#include <hbdevicedialogsymbian.h>
#include <hbindicatorsymbian.h>
#include <hbsymbianvariant.h>
#include <SyncMLObservers.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include "dmdevdialogclientserver.h"
#include "dmdevicedialogdebug.h"

class CDmDeviceDialogSession;
class CDmDeviceDialogServer;
class CHbIndicatorSymbian;


_LIT(KDeviceDialogType,"com.nokia.hb.devicemanagementdialog/1.0");
_LIT(KNotifierType, "syncmlfw");
_LIT(KProfileId, "profileid");
_LIT(KUImode, "uimode");
_LIT(KServerdisplayname, "serverdisplayname");
_LIT(KDMJobId, "jobid");
_LIT(KServerpushalertInfo, "serverpushinformative");
_LIT(KServerpushalertConfirm, "serverpushconfirmative");
_LIT(KDMProgressIndicatorType, "com.nokia.devicemanagement.progressindicatorplugin/1.0");
_LIT(KFotaDefaultProfile, "fotadefaultprofile");
_LIT(KPkgZeroTimeout, "pkgzerotimeout");


const TInt KRequestTriggerWaitTime(1000000);

const TInt KDefaultNotifierId = 1000000;
const TInt KConnectNotifierId = 1000001;
// Device manager key UID
const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};
// CenRep keys for default dm profile used in the
// firmware update over the air.
const TUint32 KNSmlDMDefaultFotaProfileKey = 0x00000002;

#define KSosServerUid   0x101f99fb
#define KFotaServerUid  0x102072C4
#define KScomoServerUid 0x200267FB



class CDmDeviceDialogServer : public CServer2,
        public MHbDeviceDialogObserver,
        public MSyncMLEventObserver,
        public MHbIndicatorSymbianObserver
    {
    friend class CDmDeviceDialogSession;

public:
    enum TNoteType
        {
        EPkgZeroNote,
        EConnectNote,
        EInformativeAlertNote,
        EConfirmationAlertNote
        };
    
    enum TSyncMLSANSupport
        {
        ESANNotSpecified,
        ESANSilent,
        ESANUserInformative,
        ESANUserInteractive
        };
    /**
     * Destructor
     */
    virtual ~CDmDeviceDialogServer();

    /**
     * Static method which creates the server instance
     * @param None
     * @return CServer2*
     */
    static CServer2* NewLC();

    /**
     * Stops the session and closes the server
     * @param None
     * @return None
     */
    void CloseServer();

    /**
     * For asynchronous network request to be 
     * logged on active object, using CPeriodic
     * @param None
     * @return None
     */
     void PeriodicServerCloseL();
    
    /**
     * Launches package zero device dialog
     * @param aProfileId, ProfileId of the DM server
     * @param ajobId, Id of the DM job created due to package zero
     * @param aUimode, Server alert ui mode type
     * @param aReplySlot, Response to Sos server
     * @param aMessage, reference to RMessage2 of current session.
     * @return None.
     */
    void LaunchPkgZeroNoteL(TInt &aProfileId, TInt &aJobId,
                    const TInt &aReplySlot, TInt &aUimode,
                    const RMessage2& aMessage);
    
    /**
     * Launches display alert specific device dialog
     * @param aAlertText, Text to be dsiplayed on display alert     
     * @param aMessage, reference to RMessage2 of current session.
     * @return None.
     */   
    void ShowDisplayalertL(const TDesC &aAlertText,
                    const RMessage2& aMessage);
    
    /**
     * Launches Confirmation(1101) alert dialog
     * @param aTimeout, Timeout of the dialog. 
     * @param aNoteDisplayText, Text to be displayed on the note           
     * @param aNoteHeader, Text to be displayed as dialog header  
     * @param aMessage, reference to RMessage2 of current session.       
     * @return None.        
     */   
    void ShowConfirmationalertL(const TInt& aTimeOut, const TDesC& aHeader,
            const TDesC& aText, const RMessage2& aMessage);
    
    /**
     * Keeps track of number of asynchronous requests
     * @param None.      
     * @return None.        
     */    
    void NewAsyncRequest();
    
    /**
     * Tells any request pending or not
     * @param None.      
     * @return None.        
     */    
    TBool RequestPending();
    
    /**
     * Cancels the earlier requested server shutdown
     * annd serves for the incoming request.
     * @param None.      
     * @return None.        
     */
    void CancelServerCloseRequest(); 
    
    /**
     * Callback method from Device dialog plugin for user action.
     * @param aData, User response to the device dialog note            
     * @return None.        
     */   
    virtual void DataReceived(CHbSymbianVariantMap& aData);

    /**
     * Callback method from Device dialog plugin when dialog gets closed.
     * @param aCompletionCode, Error code.            
     * @return None.        
     */    
    virtual void DeviceDialogClosed(TInt aCompletionCode);

    /**
    * From MSyncMLEventObserver
    * Called when SyncML session events oocur.
    * @param aEvent Sync event
    * @param aIdentifier Identifier for the sync event
    * @param aError Synce event error
    * @param aAdditionalData.
    * @return None.
    */    
    virtual void OnSyncMLSessionEvent(TEvent aEvent, TInt aIdentifier,
            TInt aError, TInt aAdditionalData);
    
    /**
     * Tells whether package zero query is currently shown or not.
     * @param None.            
     * @return TBool, status of package zero note display.        
     */
    TBool IsPkgZeroNoteDisplaying();
    
    /**
     * Cancels outstanding package zero request.
     * @param None.            
     * @return None.        
     */
    void CancelPendingServerAlert();
    
    /**
     * Callback method from Device dialog Indicator when user taps on Indicator.
     * @param aType - Type of the indicator that user interacted with.
     * @param aData - Data sent by the indicator.               
     * @return None.        
     */    
    virtual void  IndicatorUserActivated (const TDesC &aType,
                        CHbSymbianVariantMap &aData); 

private:

    /**
     * second phase constructor
     * @param None
     */
    void ConstructL();

    /**
     * Default constructor
     * @param None
     */
    CDmDeviceDialogServer();

    /**
    * Called to close the server.
    * @param None.    
    * @return None.
    */    
    void StopServer();
    
    /**    
    * Dsimisses the Connect device dialog
    * @param None.
    * @return None.
    */     
    void DismissDialog();
    
    /**
    * Starts DM Job after user accepted the package zero message.
    * @param None.
    * @return None.
    */     
    void GetDMJobStatusL();
    
    /**
     * Tells whether connect dialog is currently shown or not.
     * @param None.
     * @return boolean, status of the connect dialog.
     */
    TBool IsConenctDialogDisplaying();
    
    /**
    * Display connect dialog, if dismissed earlier.
    * @param None.
    * @return None.
    */     
    void ShowConnectDialogAgainL();
    
    /**
    * Called to process the response of user action on specified Device dialog
    * @param aData, User response to the device dialog note           
    * @return None.
    */     
    void DevDialogResponseL(CHbSymbianVariantMap& aData);
    
protected:

    /**
     * Calls when a New session is getting created
     * @param aVersion
     * @param aMessage
     * @return CSession2*
     */
    CSession2* NewSessionL(const TVersion& aVersion,
            const RMessage2& aMessage) const;

private:
    //Periodic timer used while shutting down the server
    CPeriodic* iServerCloseRequest;

    //DM Server Profile Id
    TInt iProfileId;

    //Current Dm Session Job Id
    TInt iJobId;
    
    //Server alert type of package zero message
    TUint iUimode;
    
    //Pointer to Device dialog server
    CHbDeviceDialogSymbian* iDevDialog;
    
    //Reference to Indicator framework
    CHbIndicatorSymbian* iDmIndicator;
    
    //Status variable to identify the status of Connecting note
    TBool iPkgZeroConnectionNoteShown;
    
    //User response of device dialog note
    TInt iResponse;

    //RMessage reference used to notify the caller
    RMessage2 iMessage;
    
    //Status variable to know whether server can be stopped or not
    TBool iStopServer;
    
    // SyncML Session
    RSyncMLSession iSyncMLSession;

    // Job id after starting DM Job
    TInt iSyncJobId;

    // DM job
    RSyncMLDevManJob iSyncJob;    
    
    //Status variable to know whether a DM Job is ongoing or not
    TBool iJobongoing;
    
    //Status variable to know currently any device dialog is displayed or not
    TBool iAnyDialogDisplaying;
    
    //Status variable to know whether an Indicator is launched or not
    TBool iIndicatorLaunched;
    
    //Identifies the number of requests pending
    TInt iSessionCount; 
    
    //Identifies whether package zero note is displaying or not
    TBool iPkgZeroQueryNoteDisplaying;
    
	//Response slot to write the result
    TInt iReplySlot;
    };

#endif
