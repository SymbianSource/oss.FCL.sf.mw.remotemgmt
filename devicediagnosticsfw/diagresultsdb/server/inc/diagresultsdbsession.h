/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Symbian OS server session.
*  libraries   : 
*
*/


#ifndef DIAGRESULTSDBSESSION_H
#define DIAGRESULTSDBSESSION_H

class CDiagResultsDbServer;
class CDiagResultsDbTestRecord;

#include "diagresultsdbstore.h"

//System includes
#include <e32cmn.h> //TUid
#include <e32base.h>

/**
* Server session. Represents a session (version 2) 
* for a client thread on the server-side.
*
* @since S60 v5.0
**/
class CDiagResultsDbSession : 
                        public CSession2, 
                        public MRecordLoadingObserver
	{
public:

    /**
    * Destructor.
    **/
	~CDiagResultsDbSession();
		
	/**
	* NewL. 
	*
	* @param aServer Server object.
	* @return DB session.
	**/	
	static CDiagResultsDbSession* NewL(CDiagResultsDbServer * aServer);
	
public: // From CSession
    
    /**
    * Handles the servicing of a client request that has been 
    * passed to the server.
    *
    * @param aMessage The message containing the details of the client request.
    **/
	void ServiceL(const RMessage2 &aMessage);

public: // New methods

    /**
    * Handle the client message.
    *
    * @param aMessage Details of the client request.
    * @return ETrue if client request is asynchronous, EFalse is synchronous.
    **/
	TBool DispatchMessageL(const RMessage2 &aMessage);
	
	/**
	* Create a new subsession.
	*
	* @param aTestRecord Test record that represents the subsession.
	**/
	void CreateSubsessionL(CDiagResultsDbTestRecordHandle* aTestRecordHandle,
	                       TBool aReadonly );
	
	/**
	* Close the subsession.
	**/
	void CloseSubsessionL();
   
    /**
    * Returns the Uid of the results database file.
    *
    * @return TUid of the database file.
    **/
    TUid DbUid() const;
    
    /**
    * Returns the results store.
    *
    * @return Results store.
    **/
    CDiagResultsDbStore& Store();
    
    /**
    * Helper function to read buffer from client side.
    *
    * @param aMessage Details of the client request.
    * @param aParam Message argument number.
    * @param aBuffer Buffer that is filled with client side data.
    **/
    void ReadBufferL(const RMessage2& aMessage, TInt aParam, 
                     CBufFlat*& aBuffer);
    
    /**
    * Subsession must tell to the session has it written data.
    **/
    void HasWritten();
            
    /**
    * Indicates has the session wrote any data.
    * @return ETrue if session has wrote data, EFalse otherwise.
    **/
    TBool SessionHasWritten() const;    
    
    /**
    * Turn off compacting. For example KErrDiskFull causes compacting to fail.
    **/
    void DoNotCompact();   
    
    
public: //From MRecordLoadingObserver
    
    /**
    * Returns an array of test records. Contains all test records 
    * from a single database file.
    *
    * @param aError Indicates if there were any errors.
    * @param aArray An array of test records. Ownership is transferred.
    **/
    void ExistingRecordsL( TInt aError, 
                           RPointerArray<CDiagResultsDbTestRecord>* aArray );
    
protected: // service functions for client requests    
           // Look at DiagResultsDatabaseCommon.h for the client requests.

    void ConnectSubsessionL( const RMessage2 &aMessage );
     
    void CreateNewRecordL ( const RMessage2 &aMessage );
    
    void GetLastRecordL( const RMessage2 &aMessage );
    
    void GetLastNotCompletedRecordL( const RMessage2 &aMessage );
    
    void GetRecordListL( const RMessage2 &aMessage );
    
    void GetRecordInfoListL( const RMessage2 &aMessage );
    
    void GetLastResultsL( const RMessage2 &aMessage );
    
    void CancelLastResultsL(const RMessage2 &aMessage);
    
    void GetSingleLastResultL( const RMessage2 &aMessage );
    
private:

    /**
	* Constructor.
	*
	* @param aServer Server object.
	**/
	CDiagResultsDbSession(CDiagResultsDbServer * aServer);

    /**
    * ConstructL.
    **/
	void ConstructL();
	
	/**
	* Find database item from the array based on uid.
	*
	* @param aUid Uid of the database item.
	* @param aArray Pointer array that contains test records.
	* @return Database item or NULL if not found.
	**/
	CDiagResultsDatabaseItem* FindDatabaseItemL( 
	                        TUid aUid, 
	                        RPointerArray<CDiagResultsDbTestRecord>* aArray );
	
    /**
	* Search for the newest test results.
	*
	* @param aUidArray Contains the uids to be searched.
	* @param aTestRecordArray Contains all test records.
	* @param aResultsArray Contains the found results or NULL if the test 
	*                      result was not found.	
	**/                        
    void SearchLastResultsL( 
                         const CArrayFixFlat<TUid>& aUidArray, 
                         RPointerArray<CDiagResultsDbTestRecord>& aTestRecordArray, 
                         RPointerArray<CDiagResultsDatabaseItem>& aResultsArray );
        
    /**
    * Reads uids that are searched from the buffer and creates the test result array.
    *
    * @param aArray Contains all test records.
    **/                         
    void FindLastResultsL( RPointerArray<CDiagResultsDbTestRecord>& aArray );     
    
    /**
    * Read last results buffer if there would be any additional test results.
    *
    * @param aUidArray Test results to be searched.
    * @param aResultsArray results array. Found test results are appended into this.
    **/
    void CheckLastResultsBufferL( 
                         const CArrayFixFlat<TUid>& aUidArray,                           
                         RPointerArray<CDiagResultsDatabaseItem>& aResultsArray );
    
    /**
    * Read last results buffer if there would be any additional test results.
    *
    * @param aTestUid UID to be searched from the last buffer.
    * @param aResult Returns the last result if found. Otherwise NULL.
    **/
    void CheckLastResultsBufferL(
                         TUid aTestUid,                           
                         CDiagResultsDatabaseItem*& aResult );
                        	                        
	
private: // Data

    // Server pointer.
	CDiagResultsDbServer *iServer;
	
	// Counts subsession.
	CObjectCon* iSubsessionContainer;
	
	// Index to the container.
	CObjectIx* iSubsessionIndex; 
	
	// Client request details.
	RMessage2 iMsg;
	
	//Indicates are we handling InitiateGetLastResults or InitiateGetLastResult
	TInt iLastResultCommand;
	
	// This is needed when InitiateGetLastResults is called.
	RMessage2 iLastResultsMsg;
	
    // This is needed when InitiateGetLastResult is called.
	RMessage2 iLastSingleResultsMsg;
    
    // Database uid. 
    TUid iDbUid; 
    
    // Contains the permanent file store.
    CDiagResultsDbStore* iStore;
    
    // GetLastResultsL operation needs this.
    RPointerArray<CDiagResultsDatabaseItem>* iBufferedLastResults;
    
    // Contains buffered test result.
    // 
    CDiagResultsDatabaseItem* iBufferedSingleResult;
    
    // Indicates has this session wrote any data.
    TBool iHasWrittenData;
    
    // Dynamic flat buffer for transmitting data across IPC.
    CBufFlat* iBuffer;
	};

#endif // DIAGRESULTSDBSESSION_H
