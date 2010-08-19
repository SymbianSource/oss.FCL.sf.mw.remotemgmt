/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DllMain file for operator settings unit tests.
*
*/

// INCLUDES
#include <CEUnitTestSuite.h>
#include "nsmldsoperatorsettings_test.h"
#include <nsmldebug.h>
/**
* ut_nsmldsoperatorsettings.dll test suite factory function.
*/
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _DBG_FILE(_S8("CreateTestSuiteL : Begin"));
    CEUnitTestSuite* rootSuite = 
        CEUnitTestSuite::NewLC( _L( "Unit test for operator settings" ) );

    // Unit tests for operator settings class (CNSmlDSOperatorSettings)
    MEUnitTest* settingsSuite = CNSmlDSOperatorSettings_Test::NewL();
    CleanupStack::PushL( settingsSuite );
    rootSuite->AddL( settingsSuite );
    CleanupStack::Pop( settingsSuite );

    CleanupStack::Pop( rootSuite );

    _DBG_FILE(_S8("CreateTestSuiteL : Begin"));
    return rootSuite;
    }
