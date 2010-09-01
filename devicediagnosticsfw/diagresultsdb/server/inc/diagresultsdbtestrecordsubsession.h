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
* Description:  Symbian OS server subsession.
*  libraries   : 
*
*/


#ifndef DIAGRESULTSDBTESTRECORDSUBSESSION_H
#define DIAGRESULTSDBTESTRECORDSUBSESSION_H

//System includes
#include <e32cmn.h> //RMessage2
#include <e32base.h> //CObject 

//Forward declarations
class CDiagResultsDbSession;
class CDiagResultsDbTestRecordHandle;
class MRecordCompletionObserver;

/**
* Server Subsession 
*
* @since S60 v5.0
**/
class CDiagResultsDbTestRecordSubsession: public CObject, 
                                          public MRecordCompletionObserver
	{
public:  

    /**
    * NewL.
    * 
    * @param aSession Main Database session.
    * @param aTestRecord The test record that this subsession handles.
    *                    The record is either totally empty or contains data.
    * @param aResumeTestRecord Indicates can opened test record modified.
    *
    * @return Test record subsession.                    
    **/
	static CDiagResultsDbTestRecordSubsession* NewL(
	                                 CDiagResultsDbSession* aSession, 
	                                 CDiagResultsDbTestRecordHandle* aTestRecordHandle,
	                                 TBool aReadonly
	                                 );
	                                 
    /**
    * Destructor.
    **/
    ~CDiagResultsDbTestRecordSubsession();

    /**
    * Handles the client request. 
    *
    * @param aMessage Details of the client request.
    * @return ETrue if request was asynchronous, EFalse otherwise.
    **/
    TBool DispatchMessageL(const RMessage2& aMessage);
    
    /**
    * Returns the test record that this subsession represents.
    *
    * @return The test record.
    **/
    CDiagResultsDbTestRecordHandle* CurrentTestRecordHandle();
    
    /**
    * Returns the value that client send when connecting.
    * ETrue = Readonly subsession.
    * EFalse = Writable subsession.
    **/ 
    TBool ReadonlySubsession() const;
    
public: //MRecordCompletionObserver
        
    /**
    * This is called when test record is completed.
    *
    * @param aError Symbian error code or KErrNone.
    **/   
    void CompletedRecordL( TInt aError );    

protected:

    /**
    * C++ Constructor
    *
    * @param aSession Main Database session.
    * @param aTestRecord The test record that this subsession handles.
    *                    The record is either totally empty or contains data.
    **/
    CDiagResultsDbTestRecordSubsession(
                        CDiagResultsDbSession* aSession, 
                        CDiagResultsDbTestRecordHandle* aTestRecord,
                        TBool aReadonly
                        );
                      
    /**
    * ConstructL.
    **/                    
    void ConstructL();                    
    
private: // Client request handling functions. 
         // Look at DiagResultsDatabaseCommon.h for the client requests.

    void LogTestResultL( const RMessage2& aMessage );
    
    void GetTestResultL( const RMessage2& aMessage );
    
    void CompleteTestRecordL( const RMessage2& aMessage );
    
    void GetTestResultsL( CDiagResultsDbTestRecordHandle* aTestRecord, 
                          const RMessage2& aMessage );
    
    void GetTestUidsL( const RMessage2& aMessage );
       
    void GetEngineParamL( const RMessage2& aMessage );
    
    TBool Completed() const;
    
    void GetStatusL ( const RMessage2& aMessage );
    
    void SuspendTestRecordL( const RMessage2& aMessage );
    
    void CancelLogTestResult( const RMessage2& aMessage );
    
private: // Data

    // Main DB session
    CDiagResultsDbSession* iSession;
    
    // Test record that represents this subsession.
    CDiagResultsDbTestRecordHandle* iTestRecordHandle;
    
    // Details of the client request.
    RMessage2 iMsg;
    
    // Dynamic flat buffer for transmitting data across IPC.
    CBufFlat* iBuffer;
    
    // Read or Write-mode 
    TBool iReadonly;
    
    TBool iCompletedLogTest;
	};
	
#endif // DIAGRESULTSDBTESTRECORDSUBSESSION_H	
