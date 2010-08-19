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
* Description:  DllMain file for device info extension management
*  interface unit tests.
*
*/

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>
#include "devinfextmgmtif_test.h"

/**
* UT_nsmldscontactsdataprovider.dll test suite factory function.
*/
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = 
        CEUnitTestSuite::NewLC( _L( "Unit test for DevInfo extension mgmt interface" ) );

    MEUnitTest* extMgmtIfSuite = CDevInfExtMgmtIf_Test::NewL();
    CleanupStack::PushL( extMgmtIfSuite );
    rootSuite->AddL( extMgmtIfSuite );
    CleanupStack::Pop( extMgmtIfSuite );

    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
