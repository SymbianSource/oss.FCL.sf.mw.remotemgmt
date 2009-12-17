/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TestHistoryLog Module. This is	the main TestScripter class 
*					used for testing HistoryLog component. Contains all function declaration.
*
*/


#ifndef TESTHISTORYLOG_H
#define TESTHISTORYLOG_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

//#include <SyncMLObservers.h>
#include "NSmlHistoryArray.h"

// CONSTANTS
const TInt KErrorOne = -1;
const TInt KErrorFive = -5;
const TInt KErrorEleven = -11;
const TInt KOne = 1;
const TInt KTwo = 2;
const TInt KThree = 3;
const TInt KFour = 4;
const TInt KFive = 5;
const TInt KTwelve = 12;
const TInt KTaskId1 = 12344;
const TInt KTaskId2 = 12345;
const TInt KMaxMsgSize = 6;
const TInt KMaxBufSize = 1024;
_LIT8( KVendorInfo , "Vendor" );
_LIT8( KMsgDigest , "Digest" );

// Logging path
_LIT( KTestHistoryLogLogPath, "\\logs\\testframework\\TestHistoryLog\\" ); 
// Log file
_LIT( KTestHistoryLogLogFile, "TestHistoryLog.txt" ); 

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  CTestHistoryLog: This class is the main class for TestHistoryLog module.
*  Holds the pointer for CNSmlHistoryArray and pointers for other classes are created 
*  in function definition scope.
*/
class CTestHistoryLog : public CScriptBase
    {
    public:  // Constructors and destructor   
        /**
        * Two-phased constructor.
        */
        static CTestHistoryLog* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CTestHistoryLog();

    public: // Functions from base classes
        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
    private:
        /**
        * C++ default constructor.
        */
        CTestHistoryLog( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        //ADD NEW METHOD DEC HERE       
        TInt CreateHistoryArrayL( CStifItemParser& /*aItem*/ );
        TInt CreateHistoryArrayOwnedL( CStifItemParser& /*aItem*/ );
        TInt DeleteHistoryArray( CStifItemParser& /*aItem*/ );
        TInt ResetHistoryArray( CStifItemParser& /*aItem*/ );
        TInt AddNullEntryL( CStifItemParser& /*aItem*/ );
        TInt AddNewJobEntryL( CStifItemParser& /*aItem*/ );
        TInt AddNewPushMsgEntryL( CStifItemParser& /*aItem*/ );
        TInt AddNewPushMsgAndTaskEntryL( CStifItemParser& /*aItem*/ );
        TInt RemoveEntry( CStifItemParser& /*aItem*/ );
        TInt SortByType( CStifItemParser& /*aItem*/ );
        TInt SortByTime( CStifItemParser& /*aItem*/ );
        TInt DoStreamingL( CStifItemParser& /*aItem*/ );
        TInt AddMoreThanFivePushMsgsL( CStifItemParser& /*aItem*/ );
        TInt AddMoreThanFiveJobsL( CStifItemParser& /*aItem*/ );
        TInt CreateAndDestroyAlertInfoL( CStifItemParser& /*aItem*/ );
        TInt CreateAndDestroyJobL( CStifItemParser& /*aItem*/ );
        TInt CreateAndDestroyPushMsgL( CStifItemParser& /*aItem*/ );
        TInt SetGetAndResetAlertInfoL( CStifItemParser& /*aItem*/ );
        
    private:    // Data
        CNSmlHistoryArray* iHistoryArray;
        TBool iOwnerShip;        
    };

#endif      // ALERTQUEUE_H
            
// End of File
