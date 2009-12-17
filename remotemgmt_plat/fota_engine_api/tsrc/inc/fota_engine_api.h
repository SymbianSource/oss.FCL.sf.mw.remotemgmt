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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



#ifndef FOTA_ENGINE_API_H
#define FOTA_ENGINE_API_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <fotaengine.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;
/** Timeout to let cfotadownloader callstack to empty*/
const TInt32 KDownloadStartWaitTime(1000000);
const TInt32 KDownloadKillerWaitTime(1);

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( Kfota_engine_apiLogPath, "\\logs\\testframework\\fota_engine_api\\" ); 
// Log file
_LIT( Kfota_engine_apiLogFile, "fota_engine_api.txt" ); 
_LIT( Kfota_engine_apiLogFileWithTitle, "fota_engine_api_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cfota_engine_api;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Cfota_engine_api test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cfota_engine_api) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cfota_engine_api* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cfota_engine_api();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        RFotaEngineSession session;

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
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        Cfota_engine_api( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

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
        virtual TInt ExampleL( CStifItemParser& aItem );
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

		TInt DownloadUpdateL(  CStifItemParser& aItem );
		
		TInt RFotaEngineSessionCons(  CStifItemParser& aItem );
		TInt OpenL(  CStifItemParser& aItem );
		TInt CloseL(  CStifItemParser& aItem );
		TInt DownloadL(  CStifItemParser& aItem );
		TInt IsPackageStoreSizeAvailableL(  CStifItemParser& aItem );
		TInt GetStateL(  CStifItemParser& aItem );
		TInt GetResultL(  CStifItemParser& aItem );
		TInt CurrentVersionL(  CStifItemParser& aItem );
		TInt GetUpdatePackageIdsL(  CStifItemParser& aItem );
		TInt VersionL(  CStifItemParser& aItem );
		TInt GenericAlertSentL(  CStifItemParser& aItem );
		TInt ScheduledUpdateL(  CStifItemParser& aItem );
		TInt TryResumeDownloadL(  CStifItemParser& aItem );		
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		CPeriodic*  iDownloadKiller;
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // FOTA_ENGINE_API_H

// End of File
