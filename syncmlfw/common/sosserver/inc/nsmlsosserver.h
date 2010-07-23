/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Symbian OS server that dispatches SyncML client requests to various engine components. 
*
*/


#ifndef __NSMLSOSSERVER_H__
#define __NSMLSOSSERVER_H__

#ifndef SYNCML_V3
#define SYNCML_V3
#endif

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------
#include <e32base.h>
#include <e32property.h>
#include <s32mem.h> 
#include <badesca.h>
#include <SyncMLClient.h>
#include <SyncMLDef.h>
#include <SyncMLObservers.h>

#include <nsmlconstants.h>
#include "NSmlDMAgent.h"
#include "NSmlDSAgent.h"

#include "nsmldssettings.h"
#include "nsmldmsettings.h"

#include "nsmlsosserverdefs.h"
#include "NSmlAlertQueue.h"

#include "nsmlsosthread.h"
#include <sbeclient.h>
#include <abclient.h>

 #include <SettingEnforcementInfo.h>
class MNSmlSignalHandler;
class CNSmlThreadObserver;
// --------------------------------------------------------------------------
// Panic reasons
// --------------------------------------------------------------------------

enum TNSmlSOSServerPanic
    {
    ECleanupCreateError,
    EMainSchedulerError
    };

// --------------------------------------------------------------------------
// Class forwards
// --------------------------------------------------------------------------

class CSyncMLFilter;
class CNSmlSOSSession;

class CNSmlThreadParams;
class CNSmlThreadStart;
class MNSmlSignalHandler;
class CNSmlDSHostClient;
class CNSmlDSSettings;

/**
* Container class for a job progress events that have not
* been notified.
*
*  @since 
*/
struct TNSmlProgressEvent
    {
    TNSmlProgressEventType iEventType;
    MSyncMLProgressObserver::TStatus iProgressStatus;
    MSyncMLProgressObserver::TErrorLevel iErrorLevel;
    MSyncMLProgressObserver::TSyncMLDataSyncModifications iClientMods;
    MSyncMLProgressObserver::TSyncMLDataSyncModifications iServerMods;
    
    TInt iInfo1;
    TInt iInfo2;    
    TInt iInfo3;
    TInt iInfo4;
    };


/**
* Container class for a job events that have not
* been notified (ESmlJobStart, ESmlJobStop).  
*
*  @since 
*/
struct TNSmlJobEvent
    {
    MSyncMLEventObserver::TEvent iEvent;    
    TInt iEndStatus;
    TInt iJobId; 
    TUint dummy;   // not used : to prevent (UDEB) memmove panic                       
    };
    
/**
* Container class for a job created by client.
*  Job queue is comprised of CNSmlJob instances.
*
*  @since 
*/
class CNSmlJob : public CBase
	{
public:
       
	CNSmlJob();
	~CNSmlJob();

	TInt JobId() const;
	TInt ProfileId() const;
    TInt CreatorId() const;
    TInt TransportId() const;
	TSmlUsageType UsageType() const;
    TNSmlJobType JobType() const;
    TSmlSyncType SyncType() const;
    void TaskIds( RArray<TInt>& aTasks ) const;
    const CNSmlSOSSession* OwnerSession() const;
    CNSmlSOSSession* OwnerSessionNonConst() const;
	TBool JobRunning( ) const;

	void SetJobId( const TInt aJobId );
	void SetProfId( const TInt aProfId );
    void SetCreatorId( const TInt aCreatorId );
    void SetTransportId( const TInt aTransportId );
    void SetUsageType( const TSmlUsageType aUsageType );
    void SetJobType( const TNSmlJobType aJobType );
    void SetSyncType( const TSmlSyncType aSyncType );
    void AddTaskIdL( const TInt aTaskId );
    void SetOwnerSession( CNSmlSOSSession* aSession );
 	void SetJobRunning( const TBool aRunning );
 	void SetJobIapId(const TInt aIapId); 	
 	TInt GetJobIapId() const;
public:
	CNSmlJob* iNext;
    HBufC8* iPackage;
    TInt iSessionId;
    CArrayFix<TNSmlContentTypeInfo>* iCtInfo;
    TBool iPending;
	TBool iRunning;
	TBool iSilentJob;
	TInt iUimode;
private:
    TInt iJobId;
    TInt iProfId;
    TInt iCreatorId;
    TInt iTransportId;
    RArray<TInt> iTaskIds;
    TSmlUsageType iUsageType;
    TNSmlJobType iJobType;
    TSmlSyncType iSyncType;
    CNSmlSOSSession* iSession;    
    TInt iIapId;
	};

/**
* Object for a job sub session.
*
*  @since 
*/
class CNSmlJobObject : public CObject
	{
public:
       
	CNSmlJobObject();
	~CNSmlJobObject();

    TInt JobId() const;
    void SetJobId( const TInt aJobId );

private:
    TInt iJobId;        
	};

/**
* Object for a profile sub session.
*  This class is used for reading and updating profile data.
*
*  @since 
*/
class CNSmlProfile : public CObject
	{
public:
    CNSmlProfile( TSmlUsageType aUsageType, CNSmlSOSSession& aSession );
	~CNSmlProfile();

    TInt ProfileId() const;
    void SetLocked();
    TBool IsLocked();

    TInt FetchDataL( const TInt aId, TBool aAllowHidden );
    void ConnectionListL( RArray<TInt>& aArray );
    void TaskListL( RArray<TInt>& aArray );
    
    TInt DataSize();
    const TPtr8& ReadData();

    TInt UpdateDataL( TInt& aId , const TDesC8& aData, TBool& aIsHidden );
    TBool DeleteTaskL( const TInt aId );

    void SetCreatorId( const TInt aCreatorId );  

    TSmlUsageType GetUsageType();
private:
    TInt iProfId;
    const TSmlUsageType iUsageType;
    CNSmlSOSSession& iSession;
    TBool iLocked;
    TInt iCreatorId;

    CBufBase* iBuffer;
    TPtr8 iDataPtr;
	};

/**
* Object for a task sub session.
*  This class is used for reading and updating task data.
*
*  @since 
*/
class CNSmlTask : public CObject
	{
public:
	CNSmlTask( const TInt aProfId, TBool aNewTask, CNSmlSOSSession& aSession );
	~CNSmlTask();

    TInt FetchDataL( const TInt aTaskId );
    TInt GetSupportedFiltersL();

    TInt DataSize();
    const TPtr8& ReadData();
    TInt FilterSize();
    const TPtr8& ReadFilter();

    TInt UpdateDataL( TInt& aId, const TDesC8& aData );
    
    void SetCreatorId( const TInt aCreatorId );
    void SetDataProviderId(  const TInt aTaskUID );

    TInt PrepareFilterBufferL( const TInt aTaskId );
    
private:
	void PackFilterArrayL( RPointerArray<CSyncMLFilter>& aFilterArray, TInt aMatchType , TInt aChangeInfo = 0 );
	
private:
    const TInt iProfId;
    TBool iCreatingNew;
    CNSmlSOSSession& iSession;
    
    TInt iCreatorId;
    TInt iTaskUID;
	
    CBufBase* iBuffer;    
    TPtr8 	iDataPtr;   
    CBufBase* iFilterBuffer;    
    TPtr8 	iFilterBufPtr;
	};

/**
* Object for a connection sub session.
*  This class is used for reading and updating connection data.
*
*  @since 
*/
class CNSmlConnection : public CObject
	{
public:
	CNSmlConnection( const TInt aProfId, const TInt aTransId, CNSmlSOSSession& aSession );
	~CNSmlConnection();

    TInt FetchDataL();

    TInt DataSize();
    const TPtr8& ReadData();

    TInt UpdateDataL( const TDesC8& aData );

private:
    const TInt iProfId;
    const TInt iTransId;
    CNSmlSOSSession& iSession;

    CBufBase* iBuffer;
    TPtr8 iDataPtr;
	};

/**
* Object for a history log sub session.
*  This class is used for reading and resetting history log data.
*
*  @since 
*/
class CNSmlHistoryLog : public CObject
	{
public:
	CNSmlHistoryLog( const TInt aProfId, CNSmlSOSSession& aSession );
	~CNSmlHistoryLog();

    TInt FetchDataL();

    TInt DataSize();
    const TPtr8& ReadData();

    void ResetL();

private:
    const TInt iProfId;
    CNSmlSOSSession& iSession;

    CBufBase* iBuffer;
    HBufC8* iData;
    TPtr8 iDataPtr;
	};

/**
* Object for a data provider sub session.
*  This class is used for reading data provider data.
*
*  @since 
*/
class CNSmlDataProvider : public CObject
	{
public:
	CNSmlDataProvider( const TInt aId, CNSmlSOSSession& aSession );
	~CNSmlDataProvider();

    void FetchDataL();

    TInt DataSize();
    const TPtr8& ReadData();

private:
    const TInt iId;

    CBufBase* iBuffer;
    TPtr8 iDataPtr;
    CNSmlSOSSession& iSession;
	};

/**
* An active class for handling jobs, queuing and outstanding event and progress requests.
*  This class also keeps record of connected clients and closes server when it is not used.
*
*  @since 
*/
class CNSmlSOSHandler : public CActive, public MSyncMLProgressObserver, MNSmlAlertObserver 
	{
public:
    static CNSmlSOSHandler* NewL();
    ~CNSmlSOSHandler();

    void LockProfile( const TInt aProfId );
    void RemoveLock( const TInt aProfId );
    TBool IsLocked( const TInt aProfId );

    TInt AddJobL( CNSmlJob* aJob, TInt& aJobId );
    CNSmlJob* FindJob( TInt aJobId );
    TInt StopJob( const TInt aJobId, const TInt aCreatorId );
    void CurrentJob( TInt& aId, TSmlUsageType& aType );
    void QueuedJobsL( RArray<TInt>& aArray, const TSmlUsageType aType );

    void AddProgressMsgL( const RMessage2& aMessage );
    void AddContactSuiteProgressMsgL( const RMessage2& aMessage );
    TBool CompleteBufProgressMsg( const TDesC8& aBuf, const CNSmlSOSSession* aSession );
	TBool CompleteBufContactSuiteProgressMsg( const TDesC8& aBuf, const CNSmlSOSSession* aSession );
    void CancelProgressMsg( const CNSmlSOSSession* aSession );
    void CancelContactSuiteProgressMsg( const CNSmlSOSSession* aSession );

    void AddEventMsgL( const RMessage2& aMessagePtr );
    void CancelEventMsg( TInt aHandle );
    void CompleteEventMsgL( MSyncMLEventObserver::TEvent aEvent, TInt aId = 0, TInt aError = 0, TInt aAdditional = 0 );

    void ServerAlertL( TDesC8& aData, TSmlUsageType aType, TSmlProtocolVersion aVersion, TSmlTransportId aBearerType );
    
    // FOTA
    void GenericAlertL( HBufC8* aMgmtUri, HBufC8* aMetaType, HBufC8* aMetaFormat, TInt aFinalResult, HBufC8* aCorrelator );
	// FOTA end
    void GenericAlertL( HBufC8* aCorrelator, RArray<CNSmlDMAlertItem>* aItem  );
	
    void IncreaseSessionCount();
    void DecreaseSessionCount();

    void ServerSuspendedL( const TBool aSuspended );
    TBool IsSuspended();
    void ClearSessionFromJob( const CNSmlSOSSession* aSession );
    void ClosePendingMessages( const CNSmlSOSSession* aSession );
    
    void RegisterContactSuiteObserversWithProfileId(TInt aProfileId);
    TInt GetContactSuiteRegisteredProfileId() 
        {
        return iContactSuiteObserverProfileId;
        }
    TInt GetCurrentOngoingSessionProfileId()
        {
        if( iCurrentJob != NULL )
            return iCurrentJob->ProfileId();
        else
            return 0;
        }
    
public:
    // from MNSmlAlertObserver
	void CreateJobL( CNSmlAlertJobInfo& aJobInfo );
	void DoDisconnectL();	

    // from MSyncMLProgressObserver
    void OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
    void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2 );		
    void OnSyncMLDataSyncModifications( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods,
                                                      const TSyncMLDataSyncModifications& aServerMods );
public:

	HBufC8* WriteSyncErrorL( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
	HBufC8* WriteSyncProgressL( TStatus aStatus, TInt aInfo1, TInt aInfo2 );
	HBufC8* WriteSyncModificationsL( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods,
                                                   const TSyncMLDataSyncModifications& aServerMods );
                                                     
private:
    CNSmlSOSHandler();
    void ConstructL();

    TInt CreateJobId();

    void StartDSSyncL();
    void CancelDSSync();
    void StartDMSyncL();
    void CancelDMSync();

    void FinishCurrentJobL();

    TBool SearchSimilarJobs( const CNSmlJob* aJob );
    void ReadTasksFromDBL( RArray<TInt>& aArray );
    TSmlSyncType ReadSyncTypeForTaskL( const TInt aTaskId );
    void HandleEventMsg( MSyncMLEventObserver::TEvent, TInt status = 0 );

	CNSmlDbCaps::ENSmlSyncTypes ConvertSyncTypes(TSmlSyncType aSyncType);
    // from CActive
    void DoCancel();
    void RunL();

private:
    RArray<RMessage2> iProgressMsgs;
    RArray<RMessage2> iEventMsgs;
    RArray<RMessage2> iContactSuiteProgressMsgs;
    
    RArray<TInt> iProfileLocks;

	CNSmlJob* iCurrentJob;
    TInt iJobIndex;
    TInt iContactSuiteObserverProfileId;    

    RDesWriteStream iWrtStream;
    
    CNSmlMessageQueue* iMsgQueue;
	RLibrary iSessionLib;
    TInt iSessionCount;	

    TBool iSuspended;
	
	CNSmlThreadParams *iParams;
	CNSmlThreadObserver* iThreadObserver;
	};

/**
* An active class to follow system backup and restore state.
*  This class informs handler to suspend server when needed.
*
*  @since 
*/
class CNSmlSOSBackup : public CActive
	{
public:
    static CNSmlSOSBackup* NewL( CNSmlSOSHandler* aHandler );
	~CNSmlSOSBackup();

    void Subscribe();

private:    
	CNSmlSOSBackup( CNSmlSOSHandler* aHandler );
    void ConstructL();
    
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

	void BackUpModeL();
	void DeleteDataInRestoreL();
private:
    CNSmlSOSHandler* iHandler;
    RProperty iProperty;
    TBool iRestoring;
	};

/**
* A server class to intialize server and create sessions. 
*
*  @since 
*/
class CNSmlSOSServer : public CServer2
	{
public:
    static void PanicServer( TNSmlSOSServerPanic aPanic );
    static TInt ThreadFunction( TAny* aStarted );

    static CNSmlSOSServer* NewL();
	~CNSmlSOSServer();
   
public:
    CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
	CObjectCon* NewContainerL();

private:    
	CNSmlSOSServer();
    void ConstructL();

private:
    CNSmlSOSHandler* iHandler;
    CObjectConIx* iObjConIndex;
    mutable CNSmlSOSBackup* iBackup;
	};

/**
* A session class to manage and deliver messages from client.
*  This class creates and closes sub sessions.
*
*  @since 
*/
class CNSmlSOSSession : public CSession2
	{
public:
	static CNSmlSOSSession* NewL( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler );
	void ServiceL( const RMessage2 &aMessage );
    void DispatchMessageL( const RMessage2 &aMessage );

    void StoreToEventBuffer( MSyncMLEventObserver::TEvent aEvent, TInt aJobId, TInt aStatus );
    void StoreToProgessEventBuffer( const TNSmlProgressEvent& aProgressEventItem );
    
    CNSmlDSSettings& DSSettings();
    CNSmlDMSettings& DMSettings();
    CNSmlDSHostClient& HostClient();
    
    TBool ProgressEventsBuffered();
private:	
	    
    // event request
    void StartEventRequest( const RMessage2& aMessage );
    void CancelEventRequest( const RMessage2& aMessage );
    
    // progress event request
    void StartProgressRequest( const RMessage2& aMessage );
    void CancelProgressRequest( const RMessage2& aMessage );

    // client session 
    void CurrentJobL( const RMessage2& aMessage );
    void QueuedJobsL( const RMessage2& aMessage );
    void ProfileListL( const RMessage2& aMessage );
    void PackIntDataL( const RMessage2& aMessage );
    void DeleteProfileL( const RMessage2& aMessage );
    
    // job
    void CreateJobL( const TNSmlJobType aJobType, const RMessage2& aMessage );     
    void OpenJobL( const RMessage2& aMessage ); 
    void GetJobL( const RMessage2& aMessage );   
    void StopJob( const RMessage2& aMessage );
    void CloseJobL( const RMessage2& aMessage );
	CNSmlJobObject* JobFromHandle( TUint aHandle );
	void CheckJobDataL( CNSmlJob* aJob );
	
    // profile
    void CreateProfileL( const RMessage2& aMessage, const TSmlUsageType aUsageType );
    void OpenProfileL( const RMessage2& aMessage, const TSmlUsageType aUsageType );
    void GetProfileL( const RMessage2& aMessage );
    void ConnectionListL( const RMessage2& aMessage );
    void TaskListL( const RMessage2& aMessage );
    void DeleteTaskL( const RMessage2& aMessage );
    void SetProfileL( const RMessage2& aMessage );
    void CloseProfile( const RMessage2& aMessage );
    CNSmlProfile* ProfileFromHandle( TUint aHandle );
    
    // task
    void CreateTaskL( const RMessage2& aMessage );
    void OpenTaskL( const RMessage2& aMessage );
    void GetTaskL( const RMessage2& aMessage );
    void OpenTaskSupportedFiltersL( const RMessage2& aMessage );
    void GetTaskSupportedFiltersL( const RMessage2& aMessage );
    void SetTaskL( const RMessage2& aMessage );
    void CloseTaskL( const RMessage2& aMessage );
    CNSmlTask* TaskFromHandle( TUint aHandle );

    // connection
    void OpenConnectionL( const RMessage2& aMessage );
    void GetConnectionL( const RMessage2& aMessage );
    void SetConnectionL( const RMessage2& aMessage );
    void CloseConnectionL( const RMessage2& aMessage );
    CNSmlConnection* ConnectionFromHandle( TUint aHandle );

    // history log
    void OpenHistoryLogL( const RMessage2& aMessage );
    void GetHistoryLogL( const RMessage2& aMessage );
    void ResetHistoryLogL( const RMessage2& aMessage );
    void CloseHistoryLogL( const RMessage2& aMessage );
    CNSmlHistoryLog* HistoryLogFromHandle( TUint aHandle );

    // data provider
    void OpenDataProviderL( const RMessage2& aMessage );
    void GetDataProviderL( const RMessage2& aMessage );
    void CloseDataProviderL( const RMessage2& aMessage );
    CNSmlDataProvider* DataProviderFromHandle( TUint aHandle );

    void ServerAlertL( const RMessage2& aMessage );
    void InitDMAuthInfoL( const RMessage2& aMessage );
    void GetDMAuthInfoL( const RMessage2& aMessage );
    void SetDMAuthInfoL( const RMessage2& aMessage );
    
    TBool CheckSettingEnforcementStateL( const RMessage2& aMessage, 
            KSettingEnforcements aSetting );
    // FOTA
	void AddDMGenericAlertL( const RMessage2& aMessage );
	// FOTA end
	void AddGenericAlertL( const RMessage2& aMessage );
private:

	TBool MessageFromDMServer( const RMessage2& aMessage );
    void CompleteBufferedEventL();
    void CompleteBufferedProgressEventL();
	void CompleteBufferedContactSuiteProgressEventL();
    void BufferDoModifications( const TNSmlProgressEvent& aProgressEventItem );
    void BufferContactSuiteDoModifications( const TNSmlProgressEvent& aProgressEventItem );
	void ConstructL();
	CNSmlSOSSession( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler );
	~CNSmlSOSSession(); // used by CSession2 -> private.
	
private:
    CNSmlSOSServer* iServer;
    CNSmlSOSHandler* iHandler;
  
    CObjectCon* iObjCon;  // Contains all the pointers to CObject classes (SubSessions).
    CObjectIx* iObjIndex; // Used to find the object from handle.

    RArray<TInt> iArray; // This array keeps TInt data for ..ListStart and ..ListNext commands use.
    TInt iArrayIndex;    // This index indicates the next object in array to be packed.
    
    TInt iEventMsgHandle;

    RArray<TNSmlJobEvent> iEventBuffer;  //Job event buffer  
    RArray<TNSmlProgressEvent>  iProgressEventBuffer;    
	RArray<TNSmlProgressEvent>  iContactSuiteProgressEventBuffer;    
    CBufBase* iDMAuthInfo;  
    CNSmlDSHostClient* iDsClient;
    CNSmlDSSettings* iDSSettings;      
    CNSmlDMSettings* iDMSettings;   
	};

TInt LaunchServer();

#endif // __NSMLSESSIONSERVER_H__
