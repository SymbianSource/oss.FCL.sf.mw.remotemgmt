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
* Description:  definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/




#ifndef TESTENGINEAPIS_H
#define TESTENGINEAPIS_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <CWPEngine.h>
#include <MWPContextObserver.h>
#include <BADESCA.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KtestengineapisLogPath, "\\logs\\testframework\\testengineapis\\" ); 
// Log file
_LIT( KtestengineapisLogFile, "testengineapis.txt" ); 

_LIT(KProxy,"PROXY_TEST");
_LIT(KTPS,"TPS"); 
_LIT(KName,"TEST_CASE_CONTEXT");

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctestengineapis;
class CWPContextObserver;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctestengineapis test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctestengineapis) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctestengineapis* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctestengineapis();

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
        Ctestengineapis( CTestModuleIf& aTestModuleIf );

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
		virtual TInt CreateandDeleteL( );
		HBufC8* GetfilecontentsL(TPtrC aString );
		TInt ImportDocumentL(CStifItemParser& aItem );
		void SetUpEngineL( );
		TInt PopulateL();
		TInt ImportL(CStifItemParser& aItem);
		TInt ItemCountL(CStifItemParser& aItem);
		TInt InternAndExternL(CStifItemParser& aItem);
		TInt SaveL(CStifItemParser& aItem);
		TInt SummaryTitleandTextL(CStifItemParser& aItem );
		TInt SetAsDefaultL(CStifItemParser& aItem);
		TInt StoreAndRestoreL(CStifItemParser& aItem);
		TInt DetailsL(CStifItemParser& aItem);
		TInt CreateContextL(CStifItemParser& aItem);
		TInt DeleteContextL(CStifItemParser& aItem);
		TInt ContextDataCountL( );
		TInt ContextUidsL( );
		TUint32 ContextcreateL( );
		TInt ContextNameL( );
		TInt ContextTPSL( );
		TInt ContextProxiesL( );
		TInt ContextL(CStifItemParser& aItem);
		TInt BuilderL();
		TInt CurrentContextL();
		TInt CreateparameterNewL();
		TInt CreateparameterNewLC();
		TInt CSetandGetparamIDL();
		TInt CSetandGetparamNameL();
		TInt CSetandGetparamValueL();
		TInt InternAndExternparamL();
		TInt GetparamTypeL();
		TInt InsertCharacteristicL();
		TInt InsertLinkCharacteristicL();
		TInt SetandGetCharacternameL();
		TInt SetDataCharacteristicL();
		TInt SetIndexDataCharacteristicL();
		TInt DeleteAllDataCharacteristicL();
		TInt GetParamValuecharactristicL();
		TInt AccesspointL();
		TInt ReadHBufCL(CStifItemParser& aItem );
		TInt CheckURI( CStifItemParser& aItem );
		TInt CheckIPV6( CStifItemParser& aItem );
		TInt CheckIPV4( CStifItemParser& aItem );
		TInt CWPAdapterL();
		TInt DeleteContextDataL( );
		TInt ContextExistsForUidL();
		TInt ContextExistsForTPSL( CStifItemParser& aItem );
		TInt CharacteristicSetData1L( );
		TInt CharacteristicSetData2L( );
		TInt AcceptL();
		TInt ExternparamL();
		TInt NewLEngineL();
		TInt CreateandDeleteNewL( );
		void SetUpContextObserverL(TInt data);
		TInt RegisterContextObserverL();
		TInt UnRegisterContextObserverL();
		TInt CreateSetAPL(TPtrC& aOrig);
		TInt SetAPDetailsL(CStifItemParser& aItem );
        TInt GetAPIDL(CStifItemParser& aItem);
		
		//TInt AcceptL(CStifItemParser& aItem);
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
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    CWPEngine* iEngine;
    CDesC16Array* iProxies;
    TBool flag;
    //Context Observer
    CWPContextObserver* iObserver;
    };
// Class declaration of Context Observer
// This class is used for creating a dummy object
// of Context observer
class CWPContextObserver : public CBase,  public MWPContextObserver
    {
public:

    //Constructors
    void ConstructL(TInt data);
    static CWPContextObserver* NewL(TInt data);

    //Destructor.
    virtual ~CWPContextObserver();

    /**
     * Called when the context database is changed.
     */
    void ContextChangeL(RDbNotifier::TEvent aEvent);

private:
    CWPContextObserver();

private:
    TInt data;
    };

#endif      // TESTENGINEAPIS_H

// End of File
