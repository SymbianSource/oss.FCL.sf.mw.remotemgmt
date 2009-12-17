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
* Description:        TestPnPUtil.h
*
*/









#ifndef TESTPNPUTIL_H
#define TESTPNPUTIL_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <PnpUtilImpl.h>
#include <PnpProvUtil.h>
#include <e32base.h>
#include <apmstd.h>
#include <centralrepository.h>


// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KTestPnpUtilLogPath, "\\logs\\testframework\\TestPnpUtil\\" ); 
// Log file
_LIT( KTestPnpUtilLogFile, "TestPnpUtil.txt" ); 
_LIT( KTestPnpUtilLogFileWithTitle, "TestPnpUtil_[%S].txt" );

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
class CTestPnpUtil;
class TDataType;

// CLASS DECLARATION

/**
*  CTestPnpUtil test class for STIF Test Framework TestScripter.
*  other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestPnpUtil) :public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestPnpUtil* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestPnpUtil();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        

    private:

        /**
        * C++ default constructor.
        */
        CTestPnpUtil( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // Prohibit assigment operator if not deriving from CBase.
        
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ConstructUriParamL();
		TInt LaunchOnlineSupportTestL();
		TInt GetProvAdapvalueL();
		TInt SetProvAdapvalueL();
		TInt PnpGetApplicationUidL();
		TInt PnpSetApplicationUidL();
		TInt ProvNewLC();
		TInt PnPUtilFormatMncCodeL();
		TInt PnPUtilStoreAccessPointL();
		TInt PnPUtilOperatorLongNameL();
		TInt PnPUtilRegisteredInHomeNetworkL();
		TInt GetAndSetNetworkMncL();
		TInt GetAndSetNetworkMccL();
		TInt GetAndSetHomeMncL();
		TInt GetAndSetHomeMccL();
		TInt PnPUtilFetchNetworkInfoL();
		TInt PnPUtilFetchHomeNetworkInfoL();
		TInt PnPUtilImsiL();
		TInt PnPUtilGetKeyInfoL();
		TInt PnPUtilGetNonceL();
		TInt PnPUtilGetNonceValidityTimeL();
		TInt PnPUtilCreateNewNonceL();
		TInt PnPUtilGetTokenValueL();
		TInt PnPUtilGetTokenValidityTimeL();
		TInt PnPUtilCreateNewTokenL();
		TInt PnPUtilVersionL();
		TInt PnPUtilNewLC();
		TInt PnPUtilNewL();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
        
    private:
    	CPnpUtilImpl *pnputil;

    };

#endif      // TESTPNPUTIL_H

// End of File
