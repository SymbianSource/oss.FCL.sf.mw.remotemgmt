/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for device info extension management
*  interface unit tests.
*
*/

#ifndef __DEVINFEXTMGMTIF_TEST_H__
#define __DEVINFEXTMGMTIF_TEST_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATION
class CNSmlDevInfExtDataContainerPlugin;

// CLASS DEFINITION
/**
 * Unit test class for Device info extension management interface
 * (ECom plugin).
 */
NONSHARABLE_CLASS( CDevInfExtMgmtIf_Test )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static CDevInfExtMgmtIf_Test* NewL();
        static CDevInfExtMgmtIf_Test* NewLC();
        ~CDevInfExtMgmtIf_Test();

    private: // Constructors

        CDevInfExtMgmtIf_Test();
        void ConstructL();

    private:
        // Setup and teardown
        void Empty() {};
        void SetupEmptyL();
        void SetupOneXNamL();
        void SetupMultipleXNamL();
        void Teardown();

        // Test methods
        void NewLL();
        void TestEmptyPluginL();
        void TestOneXNamPluginL();
        void TestMultipleXNamPluginL();

        // Utilities
        void PrintExtensions();

    private: // Data
        CNSmlDevInfExtDataContainerPlugin* iEcomPlugin;

    EUNIT_DECLARE_TEST_TABLE;

    };

#endif // __DEVINFEXTMGMTIF_TEST_H__
