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
* Description:  Database (content) specific data encapsulation
*
*/


#ifndef __NSMLDSCONTENTITEM_H__
#define __NSMLDSCONTENTITEM_H__

// INCLUDES
#include <e32base.h>
#include <SyncMLHistory.h>
#include <SyncMLDataFilter.h>
#include "NSmlDSAgent.h"

// FORWARD DECLARATIONS
class CNSmlURI;
class CNSmlDSLUIDBuffer;
class CNSmlDSAgentLog;
class CNSmlDSMapContainer;

// CLASS DECLARATION

/**
* Class that encapsulates content specific data.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSContentItem : public CBase
    {
    public: // constructors and destructor.
        /**
        * Standard C++ constructor.
        */
        CNSmlDSContentItem();
		
        /**
        * Destructor.
        */
        ~CNSmlDSContentItem();
		
    public: // data
        // data store name
        HBufC* iStoreName;
        // local database URI
        CNSmlURI* iLocalDatabase;
        // remote database URI
        CNSmlURI* iRemoteDatabase;
        // datastore UID
        TSmlDataProviderId iImplementationUID;
        // ETrue if the store is opened
        TBool iStoreOpened;
        // AgentLog instance
        CNSmlDSAgentLog* iAgentLog;
        // used sync type
        HBufC8* iSyncType;  //RD_SUSPEND_RESUME
        //Stores the SyncType proposed by Client for a Sync session
        HBufC8* iClientSyncType; //RD_SUSPEND_RESUME
        //ETrue if the Resume session rejected by server
        TBool iRefreshRequired;
        //Set to ETrue if the session is resumed for a content
        TBool iSessionResumed;
        // used server id
        HBufC* iServerId;
        // local UID buffer
        CNSmlDSLUIDBuffer* iLUIDBuffer;
        // buffer for map items
        CNSmlDSMapContainer* iMapContainer;
        // history log task info entry
        CSyncMLHistoryJob::TTaskInfo* iTaskInfo;
        // ETrue if current datastore is interrupted
        TBool iInterrupted;
        // ETrue if interrupted immediately
        TBool iImmediatelyInterruption;
        // ETrue if there is status error code from server
        TBool iStatusErrorInSyncLog;
        // interrupted before sync session started
        TBool iInterruptedBeforeSync;
        // ETrue if waiting for initialization alert
        TBool iWaitingInitAlert;
        // batch mode flag
        TBool iBatchModeOn;
        // transaction mode flag
        TBool iAtomicModeOn;
        // number of warnings
        TInt iWarnings;
        // task id
        TInt iTaskId;
        // synchronization status
        CNSmlDSAgent::TNSmlAgentSyncStatus iSyncStatus;
        // ETrue if client item count has been asked
        TBool iClientItemCountAsked;
        // ETrue if server item count has been asked
        TBool iServerItemCountAsked;
        // number of items from server
        TInt iServerItemCount;
        // server's maximum object size
        TInt iMaxObjSizeInServer;
        // ETrue if datastore is alerted by the server
        TBool iServerAlerted;
        // array of used data filters
        RPointerArray<CSyncMLFilter> iFilters;
        // filter match type
        TSyncMLFilterMatchType iFilterMatchType;
	};

#endif // __NSMLDSCONTENTITEM_H__

// End of File
