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
* Description:  Source code file for device info extension management
*  interface unit tests.
*
*/

// CLASS HEADER
#include "devinfextmgmtif_test.h"

// EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <nsmldevinfextdatacontainerplugin.h>

// INTERNAL INCLUDES
#include "exampledevinfextdatacontainerplugin.h"

//  LOCAL DEFINITIONS
namespace
    {
    _LIT8( KTestXNam1, "TestExtensionName" );
    _LIT8( KTestXNam2, "AnotherExtensionName" );
    _LIT8( KTestXNam3, "ExtensionNameWithNoValues" );

    _LIT8( KTestXVal1, "FirstValue" );
    _LIT8( KTestXVal2, "SecondValue" );
    _LIT8( KTestXVal3, "ThirdValue" );
    _LIT8( KTestXVal4, "FourthValue" );
    }

// - Construction -----------------------------------------------------------
CDevInfExtMgmtIf_Test* CDevInfExtMgmtIf_Test::NewL()
    {
    CDevInfExtMgmtIf_Test* self = CDevInfExtMgmtIf_Test::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CDevInfExtMgmtIf_Test* CDevInfExtMgmtIf_Test::NewLC()
    {
    CDevInfExtMgmtIf_Test* self = new ( ELeave ) CDevInfExtMgmtIf_Test();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CDevInfExtMgmtIf_Test::~CDevInfExtMgmtIf_Test()
    {
    if( iEcomPlugin )
        {
        delete iEcomPlugin;
        REComSession::FinalClose();
        }
    }

CDevInfExtMgmtIf_Test::CDevInfExtMgmtIf_Test()
    {
    }

void CDevInfExtMgmtIf_Test::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Setup & Teardown ---------------------------------------------------------

void CDevInfExtMgmtIf_Test::SetupEmptyL()
    {
    // Construct the plugin normally; in its default construction, no
    //  extensions are available.
    TRAPD( err, iEcomPlugin = CNSmlDevInfExtDataContainerPlugin::NewL() );
    if( err != KErrNone )
        {
        EUNIT_PRINT( _L( "Leave while instantiating plugin: %d" ), err );
        }
    EUNIT_ASSERT_DESC( iEcomPlugin, "Plugin instantiation failed." );
    }

void CDevInfExtMgmtIf_Test::SetupOneXNamL()
    {
    // Default construction
    SetupEmptyL();

    // Add XNam + XVals
    CExampleDevInfExtDataContainerPlugin* plugin = 
        reinterpret_cast < CExampleDevInfExtDataContainerPlugin* >( iEcomPlugin );

    plugin->InsertExtension( KTestXNam1, KTestXVal1 );
    plugin->InsertExtension( KTestXNam1, KTestXVal2 );
    }

void CDevInfExtMgmtIf_Test::SetupMultipleXNamL()
    {
    // Default construction
    SetupEmptyL();

    // Add XNam + XVals
    CExampleDevInfExtDataContainerPlugin* plugin = 
        reinterpret_cast < CExampleDevInfExtDataContainerPlugin* >( iEcomPlugin );

    plugin->InsertExtension( KTestXNam1, KTestXVal1 );
    plugin->InsertExtension( KTestXNam1, KTestXVal2 );
    plugin->InsertExtension( KTestXNam2, KTestXVal3 );
    plugin->InsertExtension( KTestXNam2, KTestXVal4 );
    plugin->InsertExtension( KTestXNam3, KNullDesC8 );
    }

void CDevInfExtMgmtIf_Test::Teardown()
    {
    if( iEcomPlugin )
        {
        delete iEcomPlugin;
        }
    REComSession::FinalClose();
    }

// - Actual test methods ------------------------------------------------------
void CDevInfExtMgmtIf_Test::NewLL()
    {
    // Instantiate the example plugin
    CNSmlDevInfExtDataContainerPlugin* ecomPlugin = NULL;

    // Instantiate the plugin
    TRAPD( err, ecomPlugin = CNSmlDevInfExtDataContainerPlugin::NewL() );
    if( err != KErrNone )
        {
        EUNIT_PRINT( _L( "Leave while instantiating plugin: %d" ), err );
        }

    EUNIT_ASSERT( ecomPlugin );    
    delete ecomPlugin;
    ecomPlugin = NULL;

    // Tell the framework that plugin is no longer used - resources are released
    REComSession::FinalClose();
    }

// Run tests for the test plugin, when there are no extension configured
// in the plugin.
void CDevInfExtMgmtIf_Test::TestEmptyPluginL()
    {
    // Debug
    PrintExtensions();

    TInt extCount = 0;
    TRAPD( err, extCount = iEcomPlugin->GetExtensionCountL() );
    EUNIT_ASSERT_DESC( err == KErrNone, "GetExtensionCountL() leaves" );
    EUNIT_ASSERT( extCount == 0 );

    // Ensure that other methods leave, as there are not extensions
    TRAP( err, iEcomPlugin->GetExtNameL( 0 ) );
    EUNIT_ASSERT( err == KErrArgument );
    TRAP( err, iEcomPlugin->GetExtNameL( 1 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueCountL( 0 ) );
    EUNIT_ASSERT( err == KErrArgument );
    TRAP( err, iEcomPlugin->GetExtValueCountL( 1 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueL( 0, 0 ) );
    EUNIT_ASSERT( err == KErrArgument );
    }

// Run tests for the test plugin, when there is one extension configured
// in the plugin.
void CDevInfExtMgmtIf_Test::TestOneXNamPluginL()
    {
    // Debug
    PrintExtensions();

    TInt extCount = 0;
    TRAPD( err, extCount = iEcomPlugin->GetExtensionCountL() );
    EUNIT_ASSERT_DESC( err == KErrNone, "GetExtensionCountL() leaves" );
    EUNIT_ASSERT( extCount == 1 );

    // Check legal values
    TBuf8< 50 > tmpBuf;
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtNameL( 0 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXNam1 ) == 0 );

    TInt xValCount = 0;
    EUNIT_ASSERT_NO_LEAVE( xValCount = iEcomPlugin->GetExtValueCountL( 0 ) );
    EUNIT_ASSERT( xValCount == 2 );

    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 0, 0 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal1 ) == 0 );
    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 0, 1 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal2 ) == 0 );

    // Ensure that methods leave with illegal indices
    TRAP( err, iEcomPlugin->GetExtNameL( 1 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueCountL( 1 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueL( 1, 0 ) );
    EUNIT_ASSERT( err == KErrArgument );
    }

// Run tests for the test plugin, when there are multiple extensions configured
// in the plugin.
void CDevInfExtMgmtIf_Test::TestMultipleXNamPluginL()
    {
    // Debug
    PrintExtensions();   

    TInt extCount = 0;
    TRAPD( err, extCount = iEcomPlugin->GetExtensionCountL() );
    EUNIT_ASSERT_DESC( err == KErrNone, "GetExtensionCountL() leaves" );
    EUNIT_ASSERT( extCount == 3 );

    // Check legal values
    //  1st extension (index 0)
    TBuf8< 50 > tmpBuf;
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtNameL( 0 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXNam1 ) == 0 );

    TInt xValCount = 0;
    EUNIT_ASSERT_NO_LEAVE( xValCount = iEcomPlugin->GetExtValueCountL( 0 ) );
    EUNIT_ASSERT( xValCount == 2 );

    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 0, 0 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal1 ) == 0 );
    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 0, 1 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal2 ) == 0 );

    //  2nd extension (index 1)
    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtNameL( 1 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXNam2 ) == 0 );

    xValCount = 0;
    EUNIT_ASSERT_NO_LEAVE( xValCount = iEcomPlugin->GetExtValueCountL( 1 ) );
    EUNIT_ASSERT( xValCount == 2 );

    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 1, 0 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal3 ) == 0 );
    tmpBuf.Zero();
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtValueL( 1, 1 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXVal4 ) == 0 );

    //  3nd extension (index 2)
    EUNIT_ASSERT_NO_LEAVE( tmpBuf = iEcomPlugin->GetExtNameL( 2 ) );
    EUNIT_ASSERT( tmpBuf.Compare( KTestXNam3 ) == 0 );

    xValCount = 0;
    EUNIT_ASSERT_NO_LEAVE( xValCount = iEcomPlugin->GetExtValueCountL( 2 ) );
    EUNIT_ASSERT( xValCount == 0 );

    // Ensure that methods leave with illegal indices
    TRAP( err, iEcomPlugin->GetExtNameL( -1 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueCountL( 3 ) );
    EUNIT_ASSERT( err == KErrArgument );

    TRAP( err, iEcomPlugin->GetExtValueL( 5, 2 ) );
    EUNIT_ASSERT( err == KErrArgument );
    }

// - Utilities ----------------------------------------------------------------
void CDevInfExtMgmtIf_Test::PrintExtensions()
    {
    EUNIT_PRINT( _L( "Extension count %d" ), iEcomPlugin->GetExtensionCountL() );

    for ( TInt i = 0; i < iEcomPlugin->GetExtensionCountL(); i++ )
        {
        EUNIT_PRINT( _L("EXTENSION NUMBER %d"), i );

        EUNIT_PRINT( _L( "Name" ) );
        EUNIT_PRINT( iEcomPlugin->GetExtNameL( i ) );

        EUNIT_PRINT( _L( "Values" ) );
        for ( TInt j = 0; j < iEcomPlugin->GetExtValueCountL( i ); j++ )
            { 
            EUNIT_PRINT( iEcomPlugin->GetExtValueL( i, j ) );
            }

        EUNIT_PRINT( _L("END OF EXTENSION %d"), i );
        }
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    CDevInfExtMgmtIf_Test,
    "Unit tests for Device info ext management interface.",
    "UNIT" )

EUNIT_TEST(
    "Verify Ecom plugin instantiation",
    "CNSmlDevInfExtDataContainerPlugin",
    "NewL",
    "FUNCTIONALITY",
    Empty, NewLL, Empty)

EUNIT_TEST(
    "Test the behaviour with no extensions",
    "CNSmlDevInfExtDataContainerPlugin",
    "NewL",
    "FUNCTIONALITY",
    SetupEmptyL, TestEmptyPluginL, Teardown)

EUNIT_TEST(
    "Test the behaviour with one extension",
    "CNSmlDevInfExtDataContainerPlugin",
    "NewL",
    "FUNCTIONALITY",
    SetupOneXNamL, TestOneXNamPluginL, Teardown)

EUNIT_TEST(
    "Test the behaviour with multiple extensions",
    "CNSmlDevInfExtDataContainerPlugin",
    "NewL",
    "FUNCTIONALITY",
    SetupMultipleXNamL, TestMultipleXNamPluginL, Teardown)
    
EUNIT_END_TEST_TABLE

// End of file
