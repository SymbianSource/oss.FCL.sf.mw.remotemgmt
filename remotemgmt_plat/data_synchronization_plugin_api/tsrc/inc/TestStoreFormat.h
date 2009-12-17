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
* Description:   ?Description
*
*/




#ifndef TESTSTOREFORMAT_H
#define TESTSTOREFORMAT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <SmlDataFormat.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KTestStoreFormatLogPath, "\\logs\\testframework\\TestStoreFormat\\" ); 
// Log file
_LIT( KTestStoreFormatLogFile, "TestStoreFormat.txt" ); 
_LIT( KTestStoreFormatLogFileWithTitle, "TestStoreFormat_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTestStoreFormat;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTestStoreFormat test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestStoreFormat) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestStoreFormat* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestStoreFormat();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

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
        CTestStoreFormat( CTestModuleIf& aTestModuleIf );

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

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    
    	//TInt CreateMimeFormatL(CStifItemParser& aItem );
    	TInt CreateDataFieldL(CStifItemParser& aItem );
    	TInt CreateDataFieldTestL(CStifItemParser& aItem );
    	TInt SetDisplayNameL(CStifItemParser& aItem );
    	TInt SetAndGetSyncMaskL(CStifItemParser& aItem );
    	TInt SetndGetMimeCountL(CStifItemParser& aItem );
    	TInt SetndGetMimeFomatTxL(CStifItemParser& aItem );
    	TInt SetndGetMimeFomatRxL(CStifItemParser& aItem );
    	TInt SetndGetFlagsL(CStifItemParser& aItem );
    	TInt SetAndGetMaxSizeL(CStifItemParser& aItem);
    	TInt SetAndGetMaxItemsL(CStifItemParser& aItem);
    	TInt SetAndGetFolderPropertyL(CStifItemParser& aItem);
    	TInt SetAndGetFilterCapabilityL(CStifItemParser& aItem);
    	void ReadFormatResourceLC(TResourceReader& aReader);
    	TInt CreateMimeFormatL();
    	TInt ExternalizeL();
    	
    	TInt SetMimePropertiesL();
        TInt GetMimePropertiesL();
    	
    	TInt CreateDataPropertyL();
    	TInt SetDataPropertyL();

    	TInt TestL(CStifItemParser& aItem);
    	//functions for testing the class CSmlDataField class
    	TInt CreateSmlDataFieldL(CStifItemParser& aItem );
    	TInt DataFieldGetAndSetL(CStifItemParser& aItem );
    	TInt DataFieldGetAndSetNameL(CStifItemParser& aItem );
    	TInt DataFieldGetAndSetDataTypeL(CStifItemParser& aItem );
    	TInt DataFieldGetAndSetEnumValueL(CStifItemParser& aItem );
    	TInt CreateFilterCapabilityL(CStifItemParser& aItem);
    	TInt SetFilterCapabilityL();


    	
    	//function for testing the class CSmlDataPropertyparam
    	TInt DataPropParamSetAndGetDataFieldL(CStifItemParser& aItem );
        
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

#endif      // TESTSTOREFORMAT_H

// End of File
