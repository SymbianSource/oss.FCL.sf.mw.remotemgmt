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
* Description:  Operator settings unit tests
*
*/

// INTERNAL INCLUDES
#include "nsmldsoperatorsettings_test.h"

// EXTERNAL INCLUDES
#include <NSmlDSOperatorSettings.h>
#include <EUnitMacros.h>
#include <centralrepository.h> //CRepository
#include <nsmloperatordatacrkeys.h> // KCRUidOperatorDatasyncInternalKeys

// CONSTANTS

// - Construction -----------------------------------------------------------

CNSmlDSOperatorSettings_Test* CNSmlDSOperatorSettings_Test::NewL()
    {
    CNSmlDSOperatorSettings_Test* self = CNSmlDSOperatorSettings_Test::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNSmlDSOperatorSettings_Test* CNSmlDSOperatorSettings_Test::NewLC()
    {
    CNSmlDSOperatorSettings_Test* self = new( ELeave ) CNSmlDSOperatorSettings_Test();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNSmlDSOperatorSettings_Test::~CNSmlDSOperatorSettings_Test()
    {

    }

CNSmlDSOperatorSettings_Test::CNSmlDSOperatorSettings_Test()
    {
    
    }

void CNSmlDSOperatorSettings_Test::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    CRepository* rep = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys );
    rep->Reset();
    delete rep;
    }

// - Setup & teardown ---------------------------------------------------------

// ++++++++++++ SETUP METHODS ++++++++++++

void CNSmlDSOperatorSettings_Test::SetupL()
    {
    iOperatorSettings = CNSmlDSOperatorSettings::NewL();
    }

//// ++++++++++++ TEARDOWN METHODS ++++++++++++
//
void CNSmlDSOperatorSettings_Test::Teardown()
    {
    delete iOperatorSettings;
    iOperatorSettings = NULL;
    }

// - Test methods -------------------------------------------------------------

// MEMORY ALLOCATION FOR DATASTORE
void CNSmlDSOperatorSettings_Test::NewLL()
    {
    CNSmlDSOperatorSettings* settings = NULL;
    EUNIT_ASSERT_NO_LEAVE( settings = CNSmlDSOperatorSettings::NewL() );
    EUNIT_ASSERT( settings );
    delete settings;
    }

void CNSmlDSOperatorSettings_Test::IsOperatorProfileLL()
    {
    EUNIT_ASSERT( iOperatorSettings->IsOperatorProfileL( _L("www.test.com") ) )
    EUNIT_ASSERT( !iOperatorSettings->IsOperatorProfileL( _L("www.test2.com") ) )    
    }

void CNSmlDSOperatorSettings_Test::CustomSwvValueLCL()
    {
    HBufC8* swv( NULL );
    swv = iOperatorSettings->CustomSwvValueLC();
    EUNIT_PRINT( *swv );
    if( swv->Compare( _L8("1.0") ) != 0 )
        {
        EUNIT_FAIL_TEST( "Wrong software value" );
        }
    CleanupStack::PopAndDestroy( swv );
    }

void CNSmlDSOperatorSettings_Test::CustomModValueLCL()
    {
    HBufC8* model( NULL );
    model = iOperatorSettings->CustomModValueLC();
    EUNIT_PRINT( *model );
    if( model->Compare( _L8("Mod") ) != 0 )
        {
        EUNIT_FAIL_TEST( "Wrong model value" );
        }
    CleanupStack::PopAndDestroy( model );
    }

void CNSmlDSOperatorSettings_Test::CustomManValueLCL()
    {
    HBufC8* manufacturer( NULL );
    manufacturer = iOperatorSettings->CustomManValueLC();
    EUNIT_PRINT( *manufacturer );
    if( manufacturer->Compare( _L8("Nokia") ) != 0 )
        {
        EUNIT_FAIL_TEST( "Wrong manufacturer value" );
        }
    CleanupStack::PopAndDestroy( manufacturer );
    }


void CNSmlDSOperatorSettings_Test::PopulateStatusCodeListLL()
    {
    RArray< TInt > statusCodeArray;
    iOperatorSettings->PopulateStatusCodeListL( statusCodeArray );
    for( TInt i = 0; i < statusCodeArray.Count(); i++ )
        {
        TBuf<10> code;
        TInt j = statusCodeArray[ i ];
        code.AppendNum( j );
        EUNIT_PRINT( code );
        }

    EUNIT_ASSERT( statusCodeArray[ 0 ] == 211 );
    EUNIT_ASSERT( statusCodeArray[ 1 ] == 400 );
    EUNIT_ASSERT( statusCodeArray[ 2 ] == 401 );
    EUNIT_ASSERT( !( statusCodeArray[ 3 ] == 401 ) );

    statusCodeArray.Close();
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    CNSmlDSOperatorSettings_Test,
    "Unit test for operator settings",
    "UNIT" )

EUNIT_TEST(
    "Test to verify memory allocation for the object",
    "CNSmlDSOperatorSettings",
    "NewL",
    "FUNCTIONALITY",
    Empty, NewLL, Empty)

EUNIT_TEST(
    "Test to if profile is operator profile",
    "CNSmlDSOperatorSettings",
    "IsOperatorProfileL",
    "FUNCTIONALITY",
    SetupL, IsOperatorProfileLL, Teardown )

EUNIT_TEST(
    "Test to verify Swv value",
    "CNSmlDSOperatorSettings",
    "CustomSwvValueLC",
    "FUNCTIONALITY",
    SetupL, CustomSwvValueLCL, Teardown )

EUNIT_TEST(
    "Test to verify Model value",
    "CNSmlDSOperatorSettings",
    "CustomModValueLC",
    "FUNCTIONALITY",
    SetupL, CustomModValueLCL, Teardown )

EUNIT_TEST(
    "Test to verify Manufacture value",
    "CNSmlDSOperatorSettings",
    "CustomManValueLC",
    "FUNCTIONALITY",
    SetupL, CustomManValueLCL, Teardown )

EUNIT_TEST(
    "Test to verify status code values",
    "CNSmlDSOperatorSettings",
    "PopulateStatusCodeListL",
    "FUNCTIONALITY",
    SetupL, PopulateStatusCodeListLL, Teardown )

EUNIT_END_TEST_TABLE

//  End of File
