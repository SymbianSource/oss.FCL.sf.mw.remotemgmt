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
* Description:  Header file for operator settings unit tests.
*
*/

#ifndef __NSMLDSOPERATORSETTINGS_TEST_H
#define __NSMLDSOPERATORSETTINGS_TEST_H

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATIONS
class CNSmlDSOperatorSettings;

NONSHARABLE_CLASS( CNSmlDSOperatorSettings_Test ): public CEUnitTestSuiteClass
    {
    public:
        static CNSmlDSOperatorSettings_Test* NewL();
        static CNSmlDSOperatorSettings_Test* NewLC();
       ~CNSmlDSOperatorSettings_Test();

    private: // Constructors

        CNSmlDSOperatorSettings_Test();
        void ConstructL();

    private: // New methods

        void Empty() {};

        void SetupL();

        void Teardown();
        
        void NewLL();
        
        void IsOperatorProfileLL();

        void CustomSwvValueLCL();

        void CustomModValueLCL();

        void CustomManValueLCL();
        
        void PopulateStatusCodeListLL();
	
    private: // data
        
        CNSmlDSOperatorSettings* iOperatorSettings;
        
        EUNIT_DECLARE_TEST_TABLE;
    };

#endif // __NSMLDSOPERATORSETTINGS_TEST_H

// End of File  
