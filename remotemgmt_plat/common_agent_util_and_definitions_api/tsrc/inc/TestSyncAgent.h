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
* Description:  TestSyncAgent is a STIF TestModule to test the Synch Agent API
*
*/



#ifndef TESTSYNCAGENT_H
#define TESTSYNCAGENT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <NSmlPhoneInfo.h>

// Logging path
_LIT( KTestSyncAgentLogPath, "\\logs\\testframework\\TestSyncAgent\\" ); 
// Log file
_LIT( KTestSyncAgentLogFile, "TestSyncAgent.txt" ); 

// FORWARD DECLARATIONS
class CTestSyncAgent;


// CLASS DECLARATION
/**
*  CTestSyncAgent test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS(CTestSyncAgent) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestSyncAgent* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestSyncAgent();

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
        CTestSyncAgent( CTestModuleIf& aTestModuleIf );

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
				/**
				* Getting the Software Version and Printing to Log
				*/
        TInt GetSoftwareVersion( CStifItemParser& aItem );

				/**
				* Getting the Phone Data, Manufacturer, Model ID, Serial No, etc..
				* and also converting the data from Unicode to UTF8 format and vice versa
				*/      
        TInt GetPhoneDataL( CStifItemParser& aItem );

				/**
				* Converting data from Unicode Format to UTF8 format
				*/      
        TInt UnicodeToUTF8L( CStifItemParser& aItem );        
        
				/**
				* Converting data from UTF8 format to Unicode Format
				*/      
        TInt UTF8ToUnicodeL( CStifItemParser& aItem );        
        
        
    private:    // Data
        
				CNSmlPhoneInfo *iPhoneInfo;
    };

#endif      // TESTSYNCAGENT_H

// End of File
